/*
 *----------------------------------------------------------------------------------------
 *    Kernel/mtx
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
 *    File Name      : tk_mtximpl.c
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Mutex Function.
 *----------------------------------------------------------------------------------------
 */

#include "tk_mtximpl.h"

#if KNL_CFG_MAX_MTXID > 0

/*
 * If there is a mutex 'mtxcb' with the task of lock wait, it is true
 */
#define KNL_MTX_IS_WAITED(mtxcb)  ( !knl_que_isempty(&(mtxcb)->wait_queue) )

/*
 * Return the highest priority in the task of lock wait at mutex 'mtxcb'
 */
#define KNL_MTX_HEAD_PRI(mtxcb)   ( ((TCB*)(mtxcb)->wait_queue.next)->priority )

/*
 * Reset priority of lock get task (For TA_INHERIT only)
 */
#define KNL_MTX_RESET_PRI(tcb)    knl_mtx_release((tcb), NULL)

/*
 * Mutex control block
 */
struct mutex_control_block {
        QUEUE                wait_queue;         /* Mutex wait queue */
        ID                   mtxid;              /* Mutex ID */
        VP                   exinf;              /* Extended information */
        ATR                  mtxatr;             /* Mutex attribute */
        PRI                  ceilpri;            /* Highest priority limit of mutex */
        TCB *                mtxtsk;             /* Mutex get task */
        MTXCB *              mtxlist;            /* Mutex get list */
#if TK_USE_OBJ_NAME                              /* name */
        uint8_t              name[TK_OBJ_NAME_LENGTH];
#endif
};

/*
 * Mutex control block table and free table pointer definition
 */
#ifdef KNL_CFG_USE_FUNC_KNL_MTX_CBTABLE
Zeroinit MTXCB knl_mtx_cbtable[TK_NUM_MTXID];    /* Mutex control block */
Zeroinit QUEUE knl_mtx_freecb;                   /* FreeQue */
#define KNL_MTX_GET_CB(id)   ( &knl_mtx_cbtable[KNL_CFG_INDEX_MTX(id)] )
#endif /* KNL_CFG_USE_FUNC_KNL_MTX_CBTABLE */


#ifdef KNL_CFG_USE_FUNC_TK_LOC_MTX
/*
 * Definition of mutex wait specification
 */
static WSPEC knl_wspec_mtx_tfifo   = { TTW_MTX, NULL, NULL };
static WSPEC knl_wspec_mtx_tpri    = { TTW_MTX, knl_mtx_chgpri, NULL };
static WSPEC knl_wspec_mtx_inherit = { TTW_MTX, knl_mtx_chgpri, knl_mtx_relwai };
#endif /* KNL_CFG_USE_FUNC_TK_LOC_MTX */

#ifdef KNL_CFG_USE_FUNC_TK_LOC_MTX
/*
 *    Function Name : knl_mtx_chgpri
 *    Create Date   : 2012/7/9-2014/2/1
 *    Author        : wangshb
 *    Description   : Processing if the priority of wait task changes
 *    Param         : TCB *tcb : task control block to change the priority
 *                    int32_t oldpri : old priority, maybe is -1
 *    Return Code   : void
 */
void knl_mtx_chgpri( TCB *tcb, int32_t oldpri )
{
    MTXCB *mtxcb;
    TCB *mtxtsk;

    mtxcb = KNL_MTX_GET_CB(tcb->wid);
    knl_tsk_gcbchgpri((GCB*)mtxcb, tcb);

    if ( (mtxcb->mtxatr & TA_CEILING) == TA_INHERIT ) {
        mtxtsk = mtxcb->mtxtsk;
        if ( mtxtsk->priority > tcb->priority ) {
            /* Since the highest priority of the lock wait task
               became higher, raise the lock get task priority
               higher */
            knl_tsk_chgpri(mtxtsk, tcb->priority);

        } else if ( (int32_t)mtxtsk->priority == oldpri ) {
            /* Since the highest priority of the lock wait task
               might become lower, adjust this priority */
            KNL_MTX_RESET_PRI(mtxtsk);
        }
    }
}
#endif /* KNL_CFG_USE_FUNC_TK_LOC_MTX */

#ifdef KNL_CFG_USE_FUNC_TK_LOC_MTX
/*
 *    Function Name : knl_mtx_relwai
 *    Create Date   : 2012/7/9-2014/2/1
 *    Author        : wangshb
 *    Description   : Processing if the wait task is released (For TA_INHERIT only)
 *    Param         : TCB *tcb : task to release waiting
 *    Return Code   : void
 */
void knl_mtx_relwai( TCB *tcb )
{
    MTXCB *mtxcb;
    TCB *mtxtsk;

    mtxcb = KNL_MTX_GET_CB(tcb->wid);
    mtxtsk = mtxcb->mtxtsk;

    if ( mtxtsk->priority == tcb->priority ) {
        /* Since the highest priority of the lock wait task might 
           become lower, adjust this priority */
        KNL_MTX_RESET_PRI(mtxtsk);
    }
}
#endif /* KNL_CFG_USE_FUNC_TK_LOC_MTX */

#ifdef KNL_CFG_USE_FUNC_KNL_MTX_GETNAME
#if TK_USE_OBJ_NAME
/*
 *    Function Name : knl_mtx_getname
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get object name from control block
 *    Param         : ID id : mutex id
 *                    uint8_t **name : mutex DS name pointer
 *    Return Code   : ER : Error code
 */
ER knl_mtx_getname(ID id, uint8_t **name)
{
    MTXCB *mtxcb;
    ER ercd = E_OK;

    KNL_CHK_MTXID(id)

    KNL_INT_BEGIN_DISABLE
    mtxcb = KNL_MTX_GET_CB(id);
    if ( mtxcb->mtxid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
    if ( (mtxcb->mtxatr & TA_DSNAME) == 0u ) {
        ercd = E_OBJ;
        goto error_exit;
    }
    *name = mtxcb->name;

error_exit:
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* TK_USE_OBJ_NAME */
#endif /* KNL_CFG_USE_FUNC_KNL_MTX_GETNAME */

#ifdef KNL_CFG_USE_FUNC_KNL_MTX_INIT
/*
 *    Function Name : knl_mtx_init
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Initialization of mutex control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
ER knl_mtx_init(void)
{
    MTXCB *mtxcb, *mtxcbend;

    /* Get system information */
    if ( TK_NUM_MTXID < 1 ) {
        return E_SYS;
    }

    /* Register all control blocks onto FreeQue */
    knl_que_init(&knl_mtx_freecb);
    mtxcbend = knl_mtx_cbtable + TK_NUM_MTXID;
    for( mtxcb = knl_mtx_cbtable; mtxcb < mtxcbend; mtxcb++ ) {
        mtxcb->mtxid = 0;
        knl_que_insert(&mtxcb->wait_queue, &knl_mtx_freecb);
    }

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_KNL_MTX_INIT */

#ifdef KNL_CFG_USE_FUNC_KNL_MTX_LIMPRI
/*
 *    Function Name : knl_mtx_limpri
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Limit the priority change by mutex at task priority change
 *                      1.If the 'tcb' task locks mutex, cannot set lower priority than the
 *                    highest priority in all mutexes which hold lock. In such case,
 *                    return the highest priority of locked mutex.
 *                      2.If mutex with TA_CEILING attribute is locked or waiting to be locked,
 *                    cannot set higher priority than the lowest within the highest
 *                    priority limit of mutex with TA_CEILING attribute.
 *                    In this case, return E_ILUSE.
 *                      3.Other than above, return the 'priority'.
 *    Param         : TCB *tcb : task control block to change the priority
 *                    PRI priority : new priority
 *    Return Code   : int32_t : priority to return, E_ILUSE if error
 */
int32_t knl_mtx_limpri( TCB *tcb, PRI priority )
{
    MTXCB *mtxcb;
    PRI hi_pri, low_pri, pri;

    hi_pri  = priority;
    low_pri = KNL_TSK_ITN_PRI(TK_MIN_PRI);

    /* Mutex lock wait */
    if ( ( (tcb->state & TS_WAIT) != 0u ) && ( (tcb->wspec->tskwait & TTW_MTX) != 0u ) ) {
        mtxcb = KNL_MTX_GET_CB(tcb->wid);
        if ( (mtxcb->mtxatr & TA_CEILING) == TA_CEILING ) {
            pri = mtxcb->ceilpri;
            if ( pri > low_pri ) {
                low_pri = pri;
            }
        }
    }

    /* Locked Mutex */
    pri = hi_pri;
    for ( mtxcb = tcb->mtxlist; mtxcb != NULL; mtxcb = mtxcb->mtxlist ) {
        switch ( mtxcb->mtxatr & TA_CEILING ) {
            case TA_CEILING:
                pri = mtxcb->ceilpri;
                if ( pri > low_pri ) {
                    low_pri = pri;
                }
                break;
            case TA_INHERIT:
                if ( KNL_MTX_IS_WAITED(mtxcb) ) {
                    pri = KNL_MTX_HEAD_PRI(mtxcb);
                }
                break;
            default: /* TA_TFIFO, TA_TPRI */
                /* nothing to do */
                break;
        }
        if ( pri < hi_pri ) {
            hi_pri = pri;
        }
    }

    if ( priority < low_pri ) {
        return E_ILUSE;
    }
    return (int32_t)hi_pri;
}
#endif /* KNL_CFG_USE_FUNC_KNL_MTX_LIMPRI */

#ifdef KNL_CFG_USE_FUNC_KNL_MTX_RELEASE
/*
 *    Function Name : knl_mtx_release
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Release the lock and delete it from list, and then adjust the
 *                    priority of task.
 *                    Set the highest priority between listed below:
 *                    (A) The highest priority in all mutexes in which 'tcb' task locks.
 *                    (B) The base priority of 'tcb' task.
 *    Param         : TCB *tcb : task control block with mutex
 *                    MTXCB *relmtxcb : mutex control block to release
 *    Return Code   : void
 */
void knl_mtx_release( TCB *tcb, MTXCB *relmtxcb )
{
    MTXCB *mtxcb, **prev;
    PRI newpri, pri;

    /* (B) The base priority of task */
    newpri = tcb->bpriority;

    /* (A) The highest priority in mutex which is locked */
    pri = newpri;
    prev = &tcb->mtxlist;
    while ( (mtxcb = *prev) != NULL ) {
        if ( mtxcb == relmtxcb ) {
            /* Delete from list */
            *prev = mtxcb->mtxlist;
        } else {

            switch ( mtxcb->mtxatr & TA_CEILING ) {
                case TA_CEILING:
                    pri = mtxcb->ceilpri;
                    break;
                case TA_INHERIT:
                    if ( KNL_MTX_IS_WAITED(mtxcb) ) {
                        pri = KNL_MTX_HEAD_PRI(mtxcb);
                    }
                    break;
                default: /* TA_TFIFO, TA_TPRI */
                    /* nothing to do */
                    break;
            }
            if ( newpri > pri ) {
                newpri = pri;
            }

            prev = &mtxcb->mtxlist;
        }
    }

    if ( newpri != tcb->priority ) {
        /* Change priority of lock get task */
        knl_tsk_chgpri(tcb, newpri);
    }
}
#endif /* KNL_CFG_USE_FUNC_KNL_MTX_RELEASE */

#ifdef KNL_CFG_USE_FUNC_KNL_MTX_SIGALL
/*
 *    Function Name : knl_mtx_sigall
 *    Create Date   : 2012/7/9-2014/2/1
 *    Author        : wangshb
 *    Description   : Free mutex when task is terminated
 *                    Free all mutexes which the task holds.
 *                    Do not need to handle mutex list and priority of terminated task.
 *    Param         : TCB *tcb : task control block which is terminated
 *    Return Code   : void
 */
void knl_mtx_sigall( TCB *tcb )
{
    MTXCB *mtxcb, *next_mtxcb;
    TCB *next_tcb;

    next_mtxcb = tcb->mtxlist;
    while ( (mtxcb = next_mtxcb) != NULL ) {
        next_mtxcb = mtxcb->mtxlist;

        if ( KNL_MTX_IS_WAITED(mtxcb) ) {
            next_tcb = (TCB*)mtxcb->wait_queue.next;

            /* Wake wait task */
            knl_tsk_wairelok(next_tcb);

            /* Change mutex get task */
            mtxcb->mtxtsk = next_tcb;
            mtxcb->mtxlist = next_tcb->mtxlist;
            next_tcb->mtxlist = mtxcb;

            if ( (mtxcb->mtxatr & TA_CEILING) == TA_CEILING ) {
                if ( next_tcb->priority > mtxcb->ceilpri ) {
                    /* Raise the priority for the task
                       that got lock to the highest
                       priority limit */
                    knl_tsk_chgpri(next_tcb, mtxcb->ceilpri);
                }
            }
        } else {
            /* No wait task */
            mtxcb->mtxtsk = NULL;
        }
    }
}
#endif /* KNL_CFG_USE_FUNC_KNL_MTX_SIGALL */

#ifdef KNL_CFG_USE_FUNC_TK_CRE_MTX
/*
 *    Function Name : tk_cre_mtx_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Create mutex
 *    Param         : T_CMTX *pk_cmtx : Information about the mutex to be created
 *    Return Code   : ID : Mutex ID or Error code
 */
SYSCALL ID tk_cre_mtx_impl( T_CMTX *pk_cmtx )
{
#if KNL_CFG_CHK_RSATR
    const ATR VALID_MTXATR = {
         TA_CEILING
#if TK_USE_OBJ_NAME
        |TA_DSNAME
#endif
    };
#endif
    MTXCB *mtxcb;
    ID mtxid;
    PRI ceilpri;
    ER ercd;

    KNL_CHK_RSATR(pk_cmtx->mtxatr, VALID_MTXATR)

    if ( (pk_cmtx->mtxatr & TA_CEILING) == TA_CEILING ) {
        KNL_CHK_PRI(pk_cmtx->ceilpri)
        ceilpri = KNL_TSK_ITN_PRI(pk_cmtx->ceilpri);
    } else {
        ceilpri = 0u;
    }

    KNL_TSK_BEGIN_CRITICAL_SECTION
    /* Get control block from FreeQue */
    mtxcb = (MTXCB*)knl_que_removenext(&knl_mtx_freecb);
    if ( mtxcb == NULL ) {
        ercd = E_LIMIT;
    } else {
        mtxid = KNL_CFG_ID_MTX(mtxcb - knl_mtx_cbtable);

        /* Initialize control block */
        knl_que_init(&mtxcb->wait_queue);
        mtxcb->mtxid   = mtxid;
        mtxcb->exinf   = pk_cmtx->exinf;
        mtxcb->mtxatr  = pk_cmtx->mtxatr;
        mtxcb->ceilpri = ceilpri;
        mtxcb->mtxtsk  = NULL;
        mtxcb->mtxlist = NULL;
#if TK_USE_OBJ_NAME
        if ( (pk_cmtx->mtxatr & TA_DSNAME) != 0u ) {
            strncpy((char*)&mtxcb->name, (char*)&pk_cmtx->dsname, TK_OBJ_NAME_LENGTH);
        }
#endif
        ercd = mtxid;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_CRE_MTX */


#ifdef KNL_CFG_USE_FUNC_TK_DEL_MTX
/*
 *    Function Name : tk_del_mtx_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Delete mutex
 *    Param         : ID mtxid : Mutex ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_del_mtx_impl( ID mtxid )
{
    MTXCB *mtxcb;
    ER ercd = E_OK;

    KNL_CHK_MTXID(mtxid)

    mtxcb = KNL_MTX_GET_CB(mtxid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( mtxcb->mtxid == 0 ) {
        ercd = E_NOEXS;
    } else {
        /* If there is a task that holds mutex to delete,
         * delete the mutex from the list
         * and adjust the task priority if necessary.
         */
        if ( mtxcb->mtxtsk != NULL ) {
            knl_mtx_release(mtxcb->mtxtsk, mtxcb);
        }

        /* Free wait state of task (E_DLT) */
        knl_tsk_waidel(&mtxcb->wait_queue);

        /* Return to FreeQue */
        knl_que_insert(&mtxcb->wait_queue, &knl_mtx_freecb);
        mtxcb->mtxid = 0;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_DEL_MTX */

#ifdef KNL_CFG_USE_FUNC_TK_LOC_MTX
/*
 *    Function Name : tk_loc_mtx_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Lock mutex
 *    Param         : ID mtxid : Mutex ID
 *                    TMO tmout : timeout
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_loc_mtx_impl( ID mtxid, TMO tmout )
{
    MTXCB *mtxcb;
    TCB *mtxtsk;
    ATR mtxatr;
    ER ercd = E_OK;

    KNL_CHK_MTXID(mtxid)
    KNL_CHK_TMOUT(tmout)
    KNL_CHK_DISPATCH()

    mtxcb = KNL_MTX_GET_CB(mtxid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( mtxcb->mtxid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
    if ( mtxcb->mtxtsk == knl_tsk_execution ) {
        ercd = E_ILUSE;  /* Multiplexed lock */
        goto error_exit;
    }

    mtxatr = mtxcb->mtxatr & TA_CEILING;
    if ( mtxatr == TA_CEILING ) {
        if ( knl_tsk_execution->bpriority < mtxcb->ceilpri ) {
            /* Violation of highest priority limit */
            ercd = E_ILUSE;
            goto error_exit;
        }
    }

    mtxtsk = mtxcb->mtxtsk;
    if ( mtxtsk == NULL ) {
        /* Get lock */
        mtxcb->mtxtsk = knl_tsk_execution;
        mtxcb->mtxlist = knl_tsk_execution->mtxlist;
        knl_tsk_execution->mtxlist = mtxcb;

        if ( mtxatr == TA_CEILING ) {
            if ( knl_tsk_execution->priority > mtxcb->ceilpri ) {
                /* Raise its own task to the highest
                   priority limit */
                knl_tsk_chgpri(knl_tsk_execution, mtxcb->ceilpri);
            }
        }
    } else {
        ercd = E_TMOUT;
        if ( tmout == TMO_POL ) {
            goto error_exit;
        }

        if ( mtxatr == TA_INHERIT ) {
            if ( mtxtsk->priority > knl_tsk_execution->priority ) {
                /* Raise the priority of task during
                   locking to the same priority as its
                   own task */
                knl_tsk_chgpri(mtxtsk, knl_tsk_execution->priority);
            }
        }

        /* Ready for wait */
        knl_tsk_execution->wspec = ( mtxatr == TA_TFIFO   )? &knl_wspec_mtx_tfifo:
                ( mtxatr == TA_INHERIT )? &knl_wspec_mtx_inherit:
                              &knl_wspec_mtx_tpri;
        knl_tsk_execution->wercd = &ercd;
        knl_tsk_execution->wid = mtxcb->mtxid;
        knl_tsk_makwai(tmout, mtxcb->mtxatr);
        if ( mtxatr == TA_TFIFO ) {
            knl_que_insert(&knl_tsk_execution->tskque, &mtxcb->wait_queue);
        } else {
            knl_tsk_queinspri(knl_tsk_execution, &mtxcb->wait_queue);
        }
    }

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_LOC_MTX */

#ifdef KNL_CFG_USE_FUNC_TK_REF_MTX
/*
 *    Function Name : tk_ref_mtx_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer mutex state
 *    Param         : ID mtxid : Mutex ID
 *                    T_RMTX *pk_rmtx : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_ref_mtx_impl( ID mtxid, T_RMTX *pk_rmtx )
{
    MTXCB *mtxcb;
    ER ercd = E_OK;

    KNL_CHK_MTXID(mtxid)

    mtxcb = KNL_MTX_GET_CB(mtxid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( mtxcb->mtxid == 0 ) {
        ercd = E_NOEXS;
    } else {
        pk_rmtx->exinf = mtxcb->exinf;
        pk_rmtx->htsk = ( mtxcb->mtxtsk != NULL )?
                    mtxcb->mtxtsk->tskid: 0;
        pk_rmtx->wtsk = knl_tsk_waitid(&mtxcb->wait_queue);
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_REF_MTX */

#ifdef KNL_CFG_USE_FUNC_TK_UNL_MTX
/*
 *    Function Name : tk_unl_mtx_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Unlock mutex
 *    Param         : ID mtxid : Mutex ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_unl_mtx_impl( ID mtxid )
{
    MTXCB *mtxcb;    
    TCB *tcb;
    ER ercd = E_OK;

    KNL_CHK_MTXID(mtxid)
    KNL_CHK_INTSK()

    mtxcb = KNL_MTX_GET_CB(mtxid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( mtxcb->mtxid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
    if ( mtxcb->mtxtsk != knl_tsk_execution ) {
        ercd = E_ILUSE;  /* This is not locked by its own task */
        goto error_exit;
    }

    /* Delete the mutex from the list,
       and adjust its own task priority if necessary. */
    knl_mtx_release(knl_tsk_execution, mtxcb);

    if ( KNL_MTX_IS_WAITED(mtxcb) ) {
        tcb = (TCB*)mtxcb->wait_queue.next;

        /* Release wait */
        knl_tsk_wairelok(tcb);

        /* Change mutex get task */
        mtxcb->mtxtsk = tcb;
        mtxcb->mtxlist = tcb->mtxlist;
        tcb->mtxlist = mtxcb;

        if ( (mtxcb->mtxatr & TA_CEILING) == TA_CEILING ) {
            if ( tcb->priority > mtxcb->ceilpri ) {
                /* Raise the priority of the task that
                   got lock to the highest priority limit */
                knl_tsk_chgpri(tcb, mtxcb->ceilpri);
            }
        }
    } else {
        /* No wait task */
        mtxcb->mtxtsk = NULL;
    }

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_UNL_MTX */

/*
 *    Debugger support function
 */
#if TK_USE_DBGSPT

#ifdef KNL_CFG_USE_FUNC_TD_LST_MTX
/*
 *    Function Name : td_lst_mtx_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer mutex object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
SYSCALL int32_t td_lst_mtx_impl( ID list[], int32_t nent )
{
    MTXCB *mtxcb, *mtxcbend;
    int32_t n = 0;

    KNL_INT_BEGIN_DISABLE
    mtxcbend = knl_mtx_cbtable + TK_NUM_MTXID;
    for ( mtxcb = knl_mtx_cbtable; mtxcb < mtxcbend; mtxcb++ ) {
        if ( mtxcb->mtxid != 0 ) {
            if ( n < nent ) {
                *list = mtxcb->mtxid;
                list++;
            }
            n++;
        }
    }
    KNL_INT_END_DISABLE

    return n;
}
#endif /* KNL_CFG_USE_FUNC_TD_LST_MTX */


#ifdef KNL_CFG_USE_FUNC_TD_MTX_QUE
/*
 *    Function Name : td_mtx_que_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference mutex wait queue
 *    Param         : ID mtxid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
SYSCALL int32_t td_mtx_que_impl( ID mtxid, ID list[], int32_t nent )
{
    int32_t n = 0;
    MTXCB *mtxcb;
    QUEUE *q;
    ER ercd = E_OK;

    KNL_CHK_MTXID(mtxid)

    mtxcb = KNL_MTX_GET_CB(mtxid);

    KNL_INT_BEGIN_DISABLE
    if ( mtxcb->mtxid == 0 ) {
        ercd = E_NOEXS;
    } else {
        for ( q = mtxcb->wait_queue.next; q != &mtxcb->wait_queue; q = q->next ) {
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
#endif /* KNL_CFG_USE_FUNC_TD_MTX_QUE */


#ifdef KNL_CFG_USE_FUNC_TD_REF_MTX
/*
 *    Function Name : td_ref_mtx_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer mutex state
 *    Param         : ID mtxid : Object ID
 *                    TD_RMTX *pk_rmtx : Address of status information packet
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_ref_mtx_impl( ID mtxid, TD_RMTX *pk_rmtx )
{
    MTXCB *mtxcb;
    ER ercd = E_OK;

    KNL_CHK_MTXID(mtxid)

    mtxcb = KNL_MTX_GET_CB(mtxid);

    KNL_INT_BEGIN_DISABLE
    if ( mtxcb->mtxid == 0 ) {
        ercd = E_NOEXS;
    } else {
        pk_rmtx->exinf = mtxcb->exinf;
        pk_rmtx->htsk = ( mtxcb->mtxtsk != NULL )?
                    mtxcb->mtxtsk->tskid: 0;
        pk_rmtx->wtsk = knl_tsk_waitid(&mtxcb->wait_queue);
    }
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TD_REF_MTX */

#endif /* TK_USE_DBGSPT */

#endif /* KNL_CFG_MAX_MTXID */
