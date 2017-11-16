/*
 *----------------------------------------------------------------------------------------
 *    Kernel/sem
 *
 *    Copyright (C) 2008-2014 by Dalian uLoong Co.,Ltd. All rights reserved.
 *
 *    This program is open source software; developer can redistribute it and/or
 *    modify it under the terms of the U-License as published by the Tenux Open Source
 *    Society; either version 1 of the License, or (at developer option) any later
 *    Version.
 *
 *    This program is distributed in the hope that it will be useful,but WITHOUT ANY
 *    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 *    A PARTICULAR PURPOSE.  See the U-License for more details.
 *
 *    Developer should have received a copy of the U-License along with this program;
 *    if not, download from www.tenux.org(the web page of the Tenux Open Source Society).
 *
 *    CPU:        NONE
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tk_semimpl.c
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Semaphore Function.
 *----------------------------------------------------------------------------------------
 */

#include "tk_semimpl.h"

#if KNL_CFG_MAX_SEMID > 0

/*
 * Semaphore control block
 */
typedef struct semaphore_control_block {
        QUEUE                wait_queue;         /* Semaphore wait queue */
        ID                   semid;              /* Semaphore ID */
        VP                   exinf;              /* Extended information */
        ATR                  sematr;             /* Semaphore attribute */
        int32_t              semcnt;             /* Semaphore current count value */
        int32_t              maxsem;             /* Semaphore maximum count value */
#if TK_USE_OBJ_NAME                              /* name */
        uint8_t              name[TK_OBJ_NAME_LENGTH];       
#endif
}                            SEMCB;

/*
 * Semaphore control block table and free table pointer definition
 */
#ifdef KNL_CFG_USE_FUNC_KNL_SEM_CBTABLE
Zeroinit SEMCB knl_sem_cbtable[TK_NUM_SEMID];    /* Semaphore control block */
Zeroinit QUEUE knl_sem_freecb;                   /* FreeQue */
#define KNL_SEM_GET_CB(id)   ( &knl_sem_cbtable[KNL_CFG_INDEX_SEM(id)] )
#endif /* KNL_CFG_USE_FUNC_KNL_SEM_CBTABLE */

#ifdef KNL_CFG_USE_FUNC_TK_WAI_SEM
/*
 * Definition of semaphore wait specification
 */
static WSPEC knl_sem_wspec_tfifo = { TTW_SEM, NULL, knl_sem_relwai };
static WSPEC knl_sem_wspec_tpri  = { TTW_SEM, knl_sem_chgpri, knl_sem_relwai };
#endif /* KNL_CFG_USE_FUNC_TK_WAI_SEM */

#ifdef KNL_CFG_USE_FUNC_TK_WAI_SEM
/*
 *    Function Name : knl_sem_chgpri
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Processing if the priority of wait task changes
 *    Param         : TCB *tcb : task control block to change the priority
 *                    int32_t oldpri : old priority, maybe is -1
 *    Return Code   : void
 */
void knl_sem_chgpri( TCB *tcb, int32_t oldpri )
{
    SEMCB  *semcb;
    QUEUE  *queue;
    TCB  *top;

    semcb = KNL_SEM_GET_CB(tcb->wid);
    if ( oldpri >= 0 ) {
        /* Reorder wait line */
        knl_tsk_gcbchgpri((GCB*)semcb, tcb);
    }

    if ( (semcb->sematr & TA_CNT) != 0u ) {
        return;
    }

    /* From the head task in a wait queue, allocate semaphore counts
       and release wait state as much as possible */
    queue = semcb->wait_queue.next;
    while ( queue != &semcb->wait_queue ) {
        top = (TCB*)queue;
        queue = queue->next;

        /* Meet condition for releasing wait? */
        if ( semcb->semcnt < top->winfo.sem.cnt ) {
            break;
        }

        /* Release wait */
        knl_tsk_wairelok(top);

        semcb->semcnt -= top->winfo.sem.cnt;
    }
}
#endif /* KNL_CFG_USE_FUNC_TK_WAI_SEM */

#ifdef KNL_CFG_USE_FUNC_KNL_SEM_GETNAME
#if TK_USE_OBJ_NAME
/*
 *    Function Name : knl_sem_getname
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get object name from control block
 *    Param         : ID id : semaphore id
 *                    uint8_t **name : semaphore DS name pointer
 *    Return Code   : ER : Error code
 */
ER knl_sem_getname(ID id, uint8_t **name)
{
    SEMCB  *semcb;
    ER  ercd = E_OK;

    KNL_CHK_SEMID(id)

    KNL_INT_BEGIN_DISABLE
    semcb = KNL_SEM_GET_CB(id);
    if ( semcb->semid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
    if ( (semcb->sematr & TA_DSNAME) == 0u ) {
        ercd = E_OBJ;
        goto error_exit;
    }
    *name = semcb->name;

error_exit:
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* TK_USE_OBJ_NAME */
#endif /* KNL_CFG_USE_FUNC_KNL_SEM_GETNAME */

#ifdef KNL_CFG_USE_FUNC_KNL_SEM_INIT
/*
 *    Function Name : knl_sem_init
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Initialization of semaphore control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
ER knl_sem_init( void )
{
    SEMCB  *semcb, *semcbend;

    /* Get system information */
    if ( TK_NUM_SEMID < 1 ) {
        return E_SYS;
    }

    /* Register all control blocks onto FreeQue */
    knl_que_init(&knl_sem_freecb);
    semcbend = knl_sem_cbtable + TK_NUM_SEMID;
    for ( semcb = knl_sem_cbtable; semcb < semcbend; semcb++ ) {
        semcb->semid = 0;
        knl_que_insert(&semcb->wait_queue, &knl_sem_freecb);
    }

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_KNL_SEM_INIT */

#ifdef KNL_CFG_USE_FUNC_TK_WAI_SEM
/*
 *    Function Name : knl_sem_relwai
 *    Create Date   : 2012/7/9-2014/2/1
 *    Author        : wangshb
 *    Description   : Processing if the wait task is freed
 *    Param         : TCB *tcb : task to release waiting
 *    Return Code   : void
 */
void knl_sem_relwai( TCB *tcb )
{
    knl_sem_chgpri(tcb, -1);
}
#endif /* KNL_CFG_USE_FUNC_TK_WAI_SEM */

#ifdef KNL_CFG_USE_FUNC_TK_CRE_SEM
/*
 *    Function Name : tk_cre_sem_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Create semaphore
 *    Param         : T_CSEM *pk_csem : Information about the semaphore to be created
 *    Return Code   : ID : Semaphore ID or Error code
 */
SYSCALL ID tk_cre_sem_impl( T_CSEM *pk_csem )
{
#if KNL_CFG_CHK_RSATR
    const ATR VALID_SEMATR = {
         TA_TPRI
        |TA_CNT
#if TK_USE_OBJ_NAME
        |TA_DSNAME
#endif
    };
#endif
    SEMCB  *semcb;
    ID  semid;
    ER  ercd;

    KNL_CHK_RSATR(pk_csem->sematr, VALID_SEMATR)
    KNL_CHK_PAR(pk_csem->isemcnt >= 0)
    KNL_CHK_PAR(pk_csem->maxsem > 0)
    KNL_CHK_PAR(pk_csem->maxsem >= pk_csem->isemcnt)

    KNL_TSK_BEGIN_CRITICAL_SECTION
    /* Get control block from FreeQue */
    semcb = (SEMCB*)knl_que_removenext(&knl_sem_freecb);
    if ( semcb == NULL ) {
        ercd = E_LIMIT;
    } else {
        semid = KNL_CFG_ID_SEM(semcb - knl_sem_cbtable);

        /* Initialize control block */
        knl_que_init(&semcb->wait_queue);
        semcb->semid = semid;
        semcb->exinf = pk_csem->exinf;
        semcb->sematr = pk_csem->sematr;
        semcb->semcnt = pk_csem->isemcnt;
        semcb->maxsem = pk_csem->maxsem;
#if TK_USE_OBJ_NAME
        if ( (pk_csem->sematr & TA_DSNAME) != 0u ) {
            strncpy((char*)&semcb->name, (char*)&pk_csem->dsname, TK_OBJ_NAME_LENGTH);
        }
#endif
        ercd = semid;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_CRE_SEM */

#ifdef KNL_CFG_USE_FUNC_TK_DEL_SEM
/*
 *    Function Name : tk_del_sem_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Delete semaphore
 *    Param         : ID semid : Semaphore ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_del_sem_impl( ID semid )
{
    SEMCB  *semcb;
    ER  ercd = E_OK;

    KNL_CHK_SEMID(semid)

    semcb = KNL_SEM_GET_CB(semid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( semcb->semid == 0 ) {
        ercd = E_NOEXS;
    } else {
        /* Release wait state of task (E_DLT) */
        knl_tsk_waidel(&semcb->wait_queue);

        /* Return to FreeQue */
        knl_que_insert(&semcb->wait_queue, &knl_sem_freecb);
        semcb->semid = 0;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_DEL_SEM */

#ifdef KNL_CFG_USE_FUNC_TK_REF_SEM
/*
 *    Function Name : tk_ref_sem_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer semaphore state
 *    Param         : ID semid : Semaphore ID
 *                    T_RSEM *pk_rsem : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_ref_sem_impl( ID semid, T_RSEM *pk_rsem )
{
    SEMCB  *semcb;
    ER  ercd = E_OK;

    KNL_CHK_SEMID(semid)

    semcb = KNL_SEM_GET_CB(semid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( semcb->semid == 0 ) {
        ercd = E_NOEXS;
    } else {
        pk_rsem->exinf  = semcb->exinf;
        pk_rsem->wtsk   = knl_tsk_waitid(&semcb->wait_queue);
        pk_rsem->semcnt = semcb->semcnt;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_REF_SEM */

#ifdef KNL_CFG_USE_FUNC_TK_SIG_SEM
/*
 *    Function Name : tk_sig_sem_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Signal semaphore
 *    Param         : ID semid : Semaphore ID
 *                    int32_t cnt : Resource return count
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_sig_sem_impl( ID semid, int32_t cnt )
{
    SEMCB  *semcb;
    TCB  *tcb;
    QUEUE  *queue;
    ER  ercd = E_OK;
    bool8_t brk;
    
    KNL_CHK_SEMID(semid)
    KNL_CHK_PAR(cnt > 0)

    semcb = KNL_SEM_GET_CB(semid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( semcb->semid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
    if ( cnt > (semcb->maxsem - semcb->semcnt) ) {
        ercd = E_QOVR;
        goto error_exit;
    }

    /* Return semaphore counts */
    semcb->semcnt += cnt;

    /* Search task that frees wait */
    queue = semcb->wait_queue.next;
    while ( queue != &semcb->wait_queue ) {
        tcb = (TCB*)queue;
        queue = queue->next;

        /* Meet condition for Releasing wait? */
        if ( semcb->semcnt < tcb->winfo.sem.cnt ) {
            if ( (semcb->sematr & TA_CNT) == 0u ) {
                brk = true;
            }
        } else {
            /* Release wait */
            knl_tsk_wairelok(tcb);

            semcb->semcnt -= tcb->winfo.sem.cnt;
            if ( semcb->semcnt <= 0 ) {
                brk = true;
            }
        }
        if ( brk ) {
            break;
        }
    }

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_SIG_SEM */

#ifdef KNL_CFG_USE_FUNC_TK_WAI_SEM
/*
 *    Function Name : tk_wai_sem_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Wait on semaphore
 *    Param         : ID semid : Semaphore ID
 *                    int32_t cnt : Resource request count
 *                    TMO tmou : timeout
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_wai_sem_impl( ID semid, int32_t cnt, TMO tmout )
{
    TCB  *tcb;
    SEMCB  *semcb;
    ER  ercd = E_OK;

    KNL_CHK_SEMID(semid)
    KNL_CHK_PAR(cnt > 0)
    KNL_CHK_TMOUT(tmout)
    KNL_CHK_DISPATCH()

    semcb = KNL_SEM_GET_CB(semid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( semcb->semid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
#if KNL_CFG_CHK_PAR
    if ( cnt > semcb->maxsem ) {
        ercd = E_PAR;
        goto error_exit;
    }
#endif

    tcb = knl_tsk_gcbwaiquetop((GCB*)semcb, knl_tsk_execution);
    if ( ( ( ( semcb->sematr & TA_CNT ) != 0u ) || ( tcb == knl_tsk_execution ) ) && 
         ( semcb->semcnt >= cnt ) ) {
        /* Get semaphore count */
        semcb->semcnt -= cnt;

    } else {
        /* Ready for wait */
        knl_tsk_execution->wspec = ( (semcb->sematr & TA_TPRI) != 0u )?
                                   &knl_sem_wspec_tpri: &knl_sem_wspec_tfifo;
        knl_tsk_execution->wercd = &ercd;
        knl_tsk_execution->winfo.sem.cnt = cnt;
        knl_tsk_gcbmakwai((GCB*)semcb, tmout);
    }

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_WAI_SEM */

/*
 *    Debugger support function
 */
#if TK_USE_DBGSPT

#ifdef KNL_CFG_USE_FUNC_TD_LST_SEM
/*
 *    Function Name : td_lst_sem_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
SYSCALL int32_t td_lst_sem_impl( ID list[], int32_t nent )
{
    SEMCB  *semcb, *semcbend;
    int32_t  n = 0;

    KNL_INT_BEGIN_DISABLE
    semcbend = knl_sem_cbtable + TK_NUM_SEMID;
    for ( semcb = knl_sem_cbtable; semcb < semcbend; semcb++ ) {
        if ( semcb->semid != 0 ) {
            if ( n < nent ) {
                *list = semcb->semid;
                list++;
            }
            n++;
        }
    }
    KNL_INT_END_DISABLE

    return n;
}
#endif /* KNL_CFG_USE_FUNC_TD_LST_SEM */

#ifdef KNL_CFG_USE_FUNC_TD_REF_SEM
/*
 *    Function Name : td_ref_sem_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference object state
 *    Param         : ID semid : Object ID
 *                    TD_RSEM *pk_rsem : Address of status information packet
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_ref_sem_impl( ID semid, TD_RSEM *pk_rsem )
{
    SEMCB  *semcb;
    ER  ercd = E_OK;

    KNL_CHK_SEMID(semid)

    semcb = KNL_SEM_GET_CB(semid);

    KNL_INT_BEGIN_DISABLE
    if ( semcb->semid == 0 ) {
        ercd = E_NOEXS;
    } else {
        pk_rsem->exinf  = semcb->exinf;
        pk_rsem->wtsk   = knl_tsk_waitid(&semcb->wait_queue);
        pk_rsem->semcnt = semcb->semcnt;
    }
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TD_REF_SEM */

#ifdef KNL_CFG_USE_FUNC_TD_SEM_QUE
/*
 *    Function Name : td_sem_que_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference wait queue
 *    Param         : ID semid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
SYSCALL int32_t td_sem_que_impl( ID semid, ID list[], int32_t nent )
{
    int32_t  n = 0;
    SEMCB  *semcb;
    QUEUE  *q;
    ER  ercd;

    KNL_CHK_SEMID(semid)

    semcb = KNL_SEM_GET_CB(semid);

    KNL_INT_BEGIN_DISABLE
    if ( semcb->semid == 0 ) {
        ercd = E_NOEXS;
    } else {
        for ( q = semcb->wait_queue.next; q != &semcb->wait_queue; q = q->next ) {
            if ( n < nent ) {
                *list = ((TCB*)q)->tskid;
                list++;
            }
            n++;
        }
        ercd = n;
    }
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TD_SEM_QUE */

#endif /* TK_USE_DBGSPT */

#endif /* KNL_CFG_MAX_SEMID */
