/*
 *----------------------------------------------------------------------------------------
 *    Kernel/mpf
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
 *    File Name      : tk_mpfimpl.c
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Fixed Size Memory Pool Function.
 *----------------------------------------------------------------------------------------
 */

#include "tk_mpfimpl.h"

#if KNL_CFG_MAX_MPFID > 0

#define KNL_MPF_ROUNDUNIT    ( sizeof(FREEL) )

typedef struct free_list {
        struct free_list *   next;
}                            FREEL;

/*
 * Fixed size memory pool control block
 */
typedef struct fix_memorypool_control_block {
        QUEUE                wait_queue;         /* Memory pool wait queue */
        ID                   mpfid;              /* Fixed size memory pool ID */
        VP                   exinf;              /* Extended information */
        ATR                  mpfatr;             /* Memory pool attribute */
        int32_t              mpfcnt;             /* Number of blocks whole memory pool */
        int32_t              blfsz;              /* Fixed size memory block size */
        int32_t              mpfsz;              /* Whole memory pool size */
        int32_t              frbcnt;             /* Number of blocks in free area */
        VP                   mempool;            /* Top address of memory pool */
        VP                   unused;             /* Top address of unused area */
        FREEL *              freelist;           /* Free block list */
        OBJLOCK              lock;               /* Lock for object exclusive access */
#if TK_USE_OBJ_NAME                              /* name */
        uint8_t              name[TK_OBJ_NAME_LENGTH];
#endif
}                            MPFCB;

/*
 * Fixed size memory pool control block table and free table pointer definition
 */
#ifdef KNL_CFG_USE_FUNC_KNL_MPF_CBTABLE
Zeroinit MPFCB knl_mpf_cbtable[TK_NUM_MPFID];    /* Fixed size memory pool control block */
Zeroinit QUEUE knl_mpf_freecb;                   /* FreeQue */
#define KNL_MPF_GET_CB(id)   ( &knl_mpf_cbtable[KNL_CFG_INDEX_MPF(id)] )
#endif /* KNL_CFG_USE_FUNC_KNL_MPF_CBTABLE */

#ifdef KNL_CFG_USE_FUNC_TK_GET_MPF
/*
 * Definition of fixed size memory pool wait specification
 */
static WSPEC knl_mpf_wspec_tfifo = { TTW_MPF, NULL, NULL };
static WSPEC knl_mpf_wspec_tpri  = { TTW_MPF, knl_mpf_chgpri, NULL };
#endif /* KNL_CFG_USE_FUNC_TK_GET_MPF */

/*
 *    Description   : Return end address in memory pool area
 *    Param         : MPFCB *mpfcb : mpf control block 
 *    Return Code   : VP : pointer to mempool end
 */
Inline VP knl_mpf_end( MPFCB *mpfcb )
{
    return (uint8_t*)mpfcb->mempool + mpfcb->mpfsz;
}

/*
 *    Description   : Round buffer size by roundunit
 *    Param         : int32_t blfsz : buffer size
 *    Return Code   : int32_t : rounded buffer size.
 */
Inline  int32_t knl_mpf_round( int32_t blfsz )
{
    uint32_t sz;
    
    sz = (uint32_t)blfsz;
    sz = ( sz + (KNL_MPF_ROUNDUNIT-1u) ) & ( ~(KNL_MPF_ROUNDUNIT-1u) );

    return (int32_t)sz;
}

#ifdef KNL_CFG_USE_FUNC_TK_GET_MPF
/*
 *    Function Name : knl_mpl_chgpri
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Processing if the priority of wait task changes
 *    Param         : TCB *tcb : task control block to change the priority
 *                    int32_t oldpri : old priority, maybe is -1
 *    Return Code   : void
 */
void knl_mpf_chgpri( TCB *tcb, int32_t oldpri )
{
    MPFCB  *mpfcb;

    mpfcb = KNL_MPF_GET_CB(tcb->wid);
    knl_tsk_gcbchgpri((GCB*)mpfcb, tcb);
}
#endif /* KNL_CFG_USE_FUNC_TK_GET_MPF */

#ifdef KNL_CFG_USE_FUNC_KNL_MPF_GETNAME
#if TK_USE_OBJ_NAME
/*
 *    Function Name : knl_mpf_getname
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get object name from control block
 *    Param         : ID id : fixed size pool id
 *                    uint8_t **name : fixed size pool DS name pointer
 *    Return Code   : ER : Error code
 */
ER knl_mpf_getname(ID id, uint8_t **name)
{
    MPFCB  *mpfcb;
    ER  ercd = E_OK;

    KNL_CHK_MPFID(id)

    KNL_INT_BEGIN_DISABLE
    mpfcb = KNL_MPF_GET_CB(id);
    if ( mpfcb->mpfid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
    if ( (mpfcb->mpfatr & TA_DSNAME) == 0u ) {
        ercd = E_OBJ;
        goto error_exit;
    }
    *name = mpfcb->name;

error_exit:
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* TK_USE_OBJ_NAME */
#endif /* KNL_CFG_USE_FUNC_KNL_MPF_GETNAME */

#ifdef KNL_CFG_USE_FUNC_KNL_MPF_INIT
/*
 *    Function Name : knl_mpf_init
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Initialization of fixed size memory pool control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
ER knl_mpf_init( void )
{
    MPFCB  *mpfcb, *mpfcbend;

    /* Get system information */
    if ( TK_NUM_MPFID < 1 ) {
        return E_SYS;
    }

    /* Register all control blocks onto FreeQue */
    knl_que_init(&knl_mpf_freecb);
    mpfcbend = knl_mpf_cbtable + TK_NUM_MPFID;
    for ( mpfcb = knl_mpf_cbtable; mpfcb < mpfcbend; mpfcb++ ) {
        mpfcb->mpfid = 0;
        knl_tsk_initlock(&mpfcb->lock);
        knl_que_insert(&mpfcb->wait_queue, &knl_mpf_freecb);
    }

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_KNL_MPF_INIT */

#ifdef KNL_CFG_USE_FUNC_TK_CRE_MPF
/*
 *    Function Name : tk_cre_mpf_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Create fixed size memory pool
 *    Param         : T_CMPF *pk_cmpf : Information about the mpf to be created
 *    Return Code   : ID : fixed size memory pool ID or Error code
 */
SYSCALL ID tk_cre_mpf_impl( T_CMPF *pk_cmpf )
{
#if KNL_CFG_CHK_RSATR
    const ATR VALID_MPFATR = {
         TA_TPRI
        |TA_RNG3
        |TA_USERBUF
#if TK_USE_OBJ_NAME
        |TA_DSNAME
#endif
    };
#endif
    MPFCB  *mpfcb;
    ID  mpfid;
    int32_t  blfsz, mpfsz;
    VP  mempool;

    KNL_CHK_RSATR(pk_cmpf->mpfatr, VALID_MPFATR)
    KNL_CHK_PAR(pk_cmpf->mpfcnt > 0)
    KNL_CHK_PAR(pk_cmpf->blfsz > 0)
#if !TK_USE_MALLOC
    /* TA_USERBUF must be specified if configured in no Imalloc */
    KNL_CHK_PAR((pk_cmpf->mpfatr & TA_USERBUF) != 0);
#endif
    KNL_CHK_DISPATCH()

    blfsz = knl_mpf_round(pk_cmpf->blfsz);
    mpfsz = blfsz * pk_cmpf->mpfcnt;

#if TK_USE_MALLOC
    if ( (pk_cmpf->mpfatr & TA_USERBUF) != 0u ) {
        /* Size of user buffer must be multiples of sizeof(FREEL) */
        if ( blfsz != pk_cmpf->blfsz ) {
            return E_PAR;
        }
        /* Use user buffer */
        mempool = pk_cmpf->bufptr;
    } else {
        /* Allocate memory for memory pool */
        mempool = knl_mem_imalloc((uint32_t)mpfsz);
        if ( mempool == NULL ) {
            return E_NOMEM;
        }
    }
#else
    /* Size of user buffer must be larger than sizeof(FREEL) */
    if ( blfsz != pk_cmpf->blfsz ) {
        return E_PAR;
    }
    /* Use user buffer */
    mempool = pk_cmpf->bufptr;
#endif

    /* Get control block from FreeQue */
    KNL_INT_DISABLE
    mpfcb = (MPFCB*)knl_que_removenext(&knl_mpf_freecb);
    KNL_INT_ENABLE

    if ( mpfcb == NULL ) {
#if TK_USE_MALLOC
        if ( (pk_cmpf->mpfatr & TA_USERBUF) == 0u ) {
            knl_mem_ifree(mempool);
        }
#endif
        return E_LIMIT;
    }

    knl_tsk_lock(&mpfcb->lock);
    mpfid = KNL_CFG_ID_MPF(mpfcb - knl_mpf_cbtable);

    /* Initialize control block */
    knl_que_init(&mpfcb->wait_queue);
    mpfcb->exinf    = pk_cmpf->exinf;
    mpfcb->mpfatr   = pk_cmpf->mpfatr;
    mpfcb->mpfcnt   = mpfcb->frbcnt = pk_cmpf->mpfcnt;
    mpfcb->blfsz    = blfsz;
    mpfcb->mpfsz    = mpfsz;
    mpfcb->unused   = mpfcb->mempool = mempool;
    mpfcb->freelist = NULL;
#if TK_USE_OBJ_NAME
    if ( (pk_cmpf->mpfatr & TA_DSNAME) != 0u ) {
        strncpy((char*)&mpfcb->name, (char*)&pk_cmpf->dsname, TK_OBJ_NAME_LENGTH);
    }
#endif

    mpfcb->mpfid    = mpfid;  /* Set ID after completion */
    knl_tsk_unlock(&mpfcb->lock);

    return mpfid;
}
#endif /* KNL_CFG_USE_FUNC_TK_CRE_MPF */

#ifdef KNL_CFG_USE_FUNC_TK_DEL_MPF
/*
 *    Function Name : tk_del_mpf_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Delete fixed size memory pool
 *    Param         : ID mplid : fixed size memory pool ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_del_mpf_impl( ID mpfid )
{
    MPFCB *mpfcb;
    VP mempool = NULL;
    ATR memattr = 0u;
    ER ercd = E_OK;

    KNL_CHK_MPFID(mpfid)
    KNL_CHK_DISPATCH()

    mpfcb = KNL_MPF_GET_CB(mpfid);

    knl_tsk_lock(&mpfcb->lock);
    if ( mpfcb->mpfid == 0 ) {
        ercd = E_NOEXS;
    } else {
        KNL_INT_DISABLE
        mempool = mpfcb->mempool;
        memattr = mpfcb->mpfatr;

        /* Release wait state of task (E_DLT) */
        knl_tsk_waidel(&mpfcb->wait_queue);

        /* Return to FreeQue */
        knl_que_insert(&mpfcb->wait_queue, &knl_mpf_freecb);
        mpfcb->mpfid = 0;
        KNL_INT_ENABLE
    }
    knl_tsk_unlock(&mpfcb->lock);

#if TK_USE_MALLOC
    if ( (mempool != NULL) && ((memattr & TA_USERBUF) == 0u) ) {
        knl_mem_ifree(mempool);
    }
#endif

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_DEL_MPF */

#ifdef KNL_CFG_USE_FUNC_TK_GET_MPF
/*
 *    Function Name : tk_get_mpf_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get fixed size memory block
 *    Param         : ID mpfid :  fixed size memory pool ID
 *                    VP* p_blk : Block start address
 *                    TMO tmout : Timeout
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_get_mpf_impl( ID mpfid, VP *p_blf, TMO tmout )
{
    MPFCB  *mpfcb;
    FREEL  *fl;
    ER  ercd = E_OK;

    KNL_CHK_MPFID(mpfid)
    KNL_CHK_TMOUT(tmout)
    KNL_CHK_DISPATCH()

    mpfcb = KNL_MPF_GET_CB(mpfid);

    knl_tsk_lock(&mpfcb->lock);
    if ( mpfcb->mpfid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }

    /* If there is no space, ready for wait */
    if ( mpfcb->frbcnt <= 0 ) {
        goto wait_mpf;
    } else {
        /* Get memory block */
        if ( mpfcb->freelist != NULL ) {
            fl = mpfcb->freelist;
            mpfcb->freelist = fl->next;
            *p_blf = fl;
        } else {
            *p_blf = mpfcb->unused;
            mpfcb->unused = (uint8_t*)mpfcb->unused + mpfcb->blfsz;
        }
        mpfcb->frbcnt--;
    }

error_exit:
    knl_tsk_unlock(&mpfcb->lock);

    return ercd;

wait_mpf:
    /* Ready for wait */
    KNL_TSK_BEGIN_CRITICAL_SECTION
    knl_tsk_execution->wspec = ( (mpfcb->mpfatr & TA_TPRI) != 0u )?
                &knl_mpf_wspec_tpri: &knl_mpf_wspec_tfifo;
    knl_tsk_execution->wercd = &ercd;
    knl_tsk_execution->winfo.mpf.p_blf = p_blf;
    knl_tsk_gcbmakwai((GCB*)mpfcb, tmout);

    knl_tsk_unlock(&mpfcb->lock);
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_GET_MPF */

#ifdef KNL_CFG_USE_FUNC_TK_REF_MPF
/*
 *    Function Name : tk_ref_mpf_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer fixed size pool state
 *    Param         : ID mpfid : fixed size pool ID
 *                    T_RMPF *pk_rmpf : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_ref_mpf_impl( ID mpfid, T_RMPF *pk_rmpf )
{
    MPFCB *mpfcb;
    ER ercd = E_OK;

    KNL_CHK_MPFID(mpfid)
    KNL_CHK_DISPATCH()

    mpfcb = KNL_MPF_GET_CB(mpfid);

    knl_tsk_lock(&mpfcb->lock);
    if ( mpfcb->mpfid == 0 ) {
        ercd = E_NOEXS;
    } else {
        KNL_INT_DISABLE
        pk_rmpf->wtsk = knl_tsk_waitid(&mpfcb->wait_queue);
        KNL_INT_ENABLE
        pk_rmpf->exinf = mpfcb->exinf;
        pk_rmpf->frbcnt = mpfcb->frbcnt;
    }
    knl_tsk_unlock(&mpfcb->lock);

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_REF_MPF */

#ifdef KNL_CFG_USE_FUNC_TK_REL_MPF
/*
 *    Function Name : tk_rel_mpf_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Return fixed size memory block
 *    Param         : ID mpfid : fixed size memory pool ID
 *                    VP blk : Memory block start address
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_rel_mpf_impl( ID mpfid, VP blf )
{
    MPFCB *mpfcb;
    TCB *tcb;
    FREEL *fl;
    ER ercd = E_OK;

    KNL_CHK_MPFID(mpfid)
    KNL_CHK_DISPATCH()

    mpfcb = KNL_MPF_GET_CB(mpfid);

    knl_tsk_lock(&mpfcb->lock);
    if ( mpfcb->mpfid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
#if KNL_CFG_CHK_PAR
    if ( ( blf >= knl_mpf_end(mpfcb) ) || ( blf < mpfcb->mempool ) ||                 
         ( (((uint8_t*)blf - (uint8_t*)mpfcb->mempool) % mpfcb->blfsz) != 0 ) ) {
        ercd = E_PAR;
        goto error_exit;
    }
#endif

    KNL_INT_DISABLE
    if ( !knl_que_isempty(&mpfcb->wait_queue) ) {
        /* Send memory block to waiting task,
           and then release the task */
        tcb = (TCB*)mpfcb->wait_queue.next;
        *tcb->winfo.mpf.p_blf = blf;
        knl_tsk_wairelok(tcb);
        KNL_INT_ENABLE
    } else {
        KNL_INT_ENABLE
        /* Free memory block */
        fl = (FREEL*)blf;
        fl->next = mpfcb->freelist;
        mpfcb->freelist = fl;
        mpfcb->frbcnt++;
    }

error_exit:
    knl_tsk_unlock(&mpfcb->lock);

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_REL_MPF */

/*
 * Debugger support function
 */
#if TK_USE_DBGSPT

#ifdef KNL_CFG_USE_FUNC_TD_LST_MPF
/*
 *    Function Name : td_lst_mpf_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer fixed size memory pool usage state
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
SYSCALL int32_t td_lst_mpf_impl( ID list[], int32_t nent )
{
    MPFCB *mpfcb, *mpfcbend;
    int32_t n = 0;

    KNL_INT_BEGIN_DISABLE
    mpfcbend = knl_mpf_cbtable + TK_NUM_MPFID;
    for ( mpfcb = knl_mpf_cbtable; mpfcb < mpfcbend; mpfcb++ ) {
        if ( mpfcb->mpfid != 0 ) {
            if ( n < nent ) {
                *list = KNL_CFG_ID_MPF(mpfcb - knl_mpf_cbtable);
                list++;
            }
            n++;
        }
    }
    KNL_INT_END_DISABLE

    return n;
}
#endif /* KNL_CFG_USE_FUNC_TD_LST_MPF */

#ifdef KNL_CFG_USE_FUNC_TD_MPF_QUE
/*
 *    Function Name : td_mpf_que_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference fixed size memory pool wait queue
 *    Param         : ID mpfid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
SYSCALL int32_t td_mpf_que_impl( ID mpfid, ID list[], int32_t nent )
{
    MPFCB *mpfcb;
    QUEUE *q;
    ER ercd = E_OK;

    KNL_CHK_MPFID(mpfid)

    mpfcb = KNL_MPF_GET_CB(mpfid);

    KNL_INT_BEGIN_DISABLE
    if ( mpfcb->mpfid == 0 ) {
        ercd = E_NOEXS;
    } else {
        int32_t n = 0;
        for ( q = mpfcb->wait_queue.next; q != &mpfcb->wait_queue; q = q->next ) {
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
#endif /* KNL_CFG_USE_FUNC_TD_MPF_QUE */

#ifdef KNL_CFG_USE_FUNC_TD_REF_MPF
/*
 *    Function Name : td_ref_mpf_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference fixed size memory pool state
 *    Param         : ID mpfid : Object ID
 *                    TD_RMPF *pk_rmpf : Address of status information packet
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_ref_mpf_impl( ID mpfid, TD_RMPF *pk_rmpf )
{
    MPFCB *mpfcb;
    ER ercd = E_OK;

    KNL_CHK_MPFID(mpfid)

    mpfcb = KNL_MPF_GET_CB(mpfid);

    KNL_INT_BEGIN_DISABLE
    if ( mpfcb->mpfid == 0 ) {
        ercd = E_NOEXS;
    } else if ( knl_tsk_islock(&mpfcb->lock) ) {
        ercd = E_CTX;
    } else {
        pk_rmpf->wtsk = knl_tsk_waitid(&mpfcb->wait_queue);
        pk_rmpf->exinf = mpfcb->exinf;
        pk_rmpf->frbcnt = mpfcb->frbcnt;
    }
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TD_REF_MPF */

#endif /* TK_USE_DBGSPT */

#endif /* KNL_CFG_MAX_MPFID */
