/*
 *----------------------------------------------------------------------------------------
 *    Kernel/mpl
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
 *    File Name      : tk_mplimpl.c
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Variable Size Memory Pool Function.
 *----------------------------------------------------------------------------------------
 */

#include "tk_mplimpl.h"

#if KNL_CFG_MAX_MPLID > 0

/*
 * Variable size memory pool control block
 *  'areaque' connects memory blocks in address ascending order
 *  'freeque' connects memory blocks in size increasing order
 *
 *  Order of members must not be changed because a part of members
 *  are used with casting to IMACB.
 */
typedef struct memorypool_control_block {
        QUEUE                wait_queue;         /* Memory pool wait queue */
        ID                   mplid;              /* Variable size memory pool ID */
        VP                   exinf;              /* Extended information */
        ATR                  mplatr;             /* Memory pool attribute */
        int32_t              mplsz;              /* Whole memory pool size */
        QUEUE                areaque;            /* Queue connecting all blocks */
        QUEUE                freeque;            /* Queue connecting free blocks */
        QUEUE                areaque_end;        /* the last element of areaque */
        VP                   mempool;            /* Top address of memory pool */
#if TK_USE_OBJ_NAME                              /* name */
        uint8_t              name[TK_OBJ_NAME_LENGTH]; 
#endif
}                            MPLCB;

/*
 * Variable size memory pool control block table and free table pointer definition
 */
#ifdef KNL_CFG_USE_FUNC_KNL_MPL_CBTABLE
Zeroinit MPLCB knl_mpl_cbtable[TK_NUM_MPLID];    /* Variable size pool control block */
Zeroinit QUEUE knl_mpl_freecb;                   /* FreeQue */
#define KNL_MPL_GET_CB(id)   ( &knl_mpl_cbtable[KNL_CFG_INDEX_MPL(id)] )
#endif /* KNL_CFG_USE_FUNC_KNL_MPL_CBTABLE */

#ifdef KNL_CFG_USE_FUNC_TK_GET_MPL
/*
 * Definition of variable size memory pool wait specification
 */
static WSPEC knl_mpl_wspec_tfifo = { TTW_MPL, NULL, knl_mpl_relwai };
static WSPEC knl_mpl_wspec_tpri  = { TTW_MPL, knl_mpl_chgpri, knl_mpl_relwai };
#endif /* KNL_CFG_USE_FUNC_TK_GET_MPL */

/*
 * Declaration of local function prototype  
 */
static void knl_mpl_appendfreeareabound( MPLCB *mplcb, QUEUE *aq );
static VP knl_mpl_getblk( MPLCB *mplcb, int32_t blksz );
static void knl_mpl_initblk( MPLCB *mplcb );
static ER knl_mpl_relblk( MPLCB *mplcb, VP blk );
static void knl_mpl_wakeup( MPLCB *mplcb );

/*
 *    Description   : Maximum free area size
 *    Param         : MPLCB *mplcb : address of mpl control block
 *    Return Code   : int32_t : free area size
 */
Inline int32_t knl_mpl_maxfreesize( MPLCB *mplcb )
{
    if ( knl_que_isempty(&mplcb->freeque) ) {
        return 0;
    }
    return KNL_MEM_FREESIZE(mplcb->freeque.prev);
}

#ifdef KNL_CFG_USE_FUNC_KNL_MPL_APPENDFREEAREABOUND
/*
 *    Function Name : knl_mpl_appendfreeareabound
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Registration of free area on FreeQue,
 *                    Specialized version for merging with top/end area
 *    Param         : MPLCB *mplcb : address of mpl control block
 *                    QUEUE *aq : address of area queue
 *    Return Code   : void
 */
static void knl_mpl_appendfreeareabound( MPLCB *mplcb, QUEUE *aq )
{
    IMACB  *imacb = (IMACB*)&(mplcb->mplsz);
    QUEUE  *fq, *aqtop, *aqend;
    int32_t    size;

    if ( aq == &(mplcb->areaque) ) {
        aqtop = (QUEUE*)mplcb->mempool;
    } else {
        aqtop = aq + 1;
    }

    if ( aq->next == &(mplcb->areaque_end) ) {
        aqend = (QUEUE*)((uint8_t*)mplcb->mempool + mplcb->mplsz);
    } else {
        aqend = aq->next;
    }

    size = (int32_t)((uint8_t*)aqend - (uint8_t*)aqtop);

    /* Registration position search */
    /*  Search the free area whose size is equal to 'blksz',
     *  or larger than 'blksz' but closest.
     *  If it does not exist, return '&imacb->freeque'.
     */
    fq = knl_mem_searchfreearea(imacb, size);

    /* Register */
    knl_mem_clrareaflag(aq, KNL_MEM_AREA_USE);
    if ( ( fq != &imacb->freeque ) && ( KNL_MEM_FREESIZE(fq) == size ) ) {
        (aqtop + 1)->next = (fq + 1)->next;
        (fq  + 1)->next = aqtop + 1;
        (aqtop + 1)->prev = fq + 1;
        (aqtop + 1)->next->prev = aqtop + 1;
        aqtop->next = NULL;
    } else {
        knl_que_insert(aqtop, fq);
        (aqtop + 1)->next = NULL;
        (aqtop + 1)->prev = (QUEUE*)size;
    }
}
#endif /* KNL_CFG_USE_FUNC_KNL_MPL_APPENDFREEAREABOUND */

#ifdef KNL_CFG_USE_FUNC_KNL_MPL_GETBLK
/*
 *    Function Name : knl_mpl_getblk
 *    Create Date   : 2012/7/9-2014/2/1
 *    Author        : wangshb
 *    Description   : Get memory block,'blksz' must be larger than minimum fragment size
 *                    and adjusted by ROUNDSZ unit.
 *    Param         : MPLCB *mplcb : address of mpl control block
 *                    int32_t blksz : Memory block size (in bytes)
 *    Return Code   : ER : Error code
 */
static VP knl_mpl_getblk( MPLCB *mplcb, int32_t blksz )
{
    QUEUE    *q, *aq, *aq2;
    IMACB*    imacb = (IMACB*)&(mplcb->mplsz);

    /* Search FreeQue */
    q = knl_mem_searchfreearea(imacb, blksz);
    if ( q == &(imacb->freeque) ) {
        return NULL;
    }

    /* remove free area from FreeQue */
    knl_mem_removefreeque(q);
    aq = ((VP)q == mplcb->mempool) ? &(mplcb->areaque) :  q - 1;

    /* If there is a fragment smaller than the minimum fragment size,
       allocate them together */
    if ( KNL_MEM_FREESIZE(q) - blksz >= (int32_t)(KNL_MEM_MIN_FRAGMENT + sizeof(QUEUE))) {

        /* Divide the area into 2. */
        aq2 = (QUEUE*)((uint8_t*)q + blksz);
        knl_mem_insertareaque(aq, aq2);

        /* Register the remaining area onto FreeQue */
        if ( aq2->next == &(mplcb->areaque_end) ) {
            knl_mpl_appendfreeareabound(mplcb, aq2);
        } else {
            knl_mem_appendfreearea(imacb, aq2);
        }
    }
    knl_mem_setareaflag(aq, KNL_MEM_AREA_USE);

    return (VP)q;
}
#endif /* KNL_CFG_USE_FUNC_KNL_MPL_GETBLK */

#ifdef KNL_CFG_USE_FUNC_TK_CRE_MPL
/*
 *    Function Name : knl_mpl_initblk
 *    Create Date   : 2012/7/9-2014/2/1
 *    Author        : wangshb
 *    Description   : Initial memory pool block setting
 *    Param         : MPLCB *mplcb : address of mpl control block
 *    Return Code   : void
 */
static void knl_mpl_initblk( MPLCB *mplcb )
{
    knl_que_init(&mplcb->areaque);
    knl_que_init(&mplcb->freeque);

    /* Register onto AreaQue */
    knl_mem_insertareaque(&mplcb->areaque, &mplcb->areaque_end);

    /* Set KNL_MEM_AREA_USE for locations that must not be free area */
    knl_mem_setareaflag(&mplcb->areaque_end, KNL_MEM_AREA_USE);

    /* Register onto FreeQue */
    knl_mpl_appendfreeareabound(mplcb, &mplcb->areaque);
}
#endif /* KNL_CFG_USE_FUNC_TK_CRE_MPL */

#ifdef KNL_CFG_USE_FUNC_KNL_MPL_RELBLK
/*
 *    Function Name : knl_mpl_relblk
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Free memory block
 *    Param         : MPLCB *mplcb : address of mpl control block
 *                    VP blk : Memory block start address
 *    Return Code   : ER : Error code
 */
static ER knl_mpl_relblk( MPLCB *mplcb, VP blk )
{
    QUEUE  *aq;
    IMACB*  imacb = (IMACB*)&(mplcb->mplsz);

    aq = (blk == mplcb->mempool) ? &(mplcb->areaque) : (QUEUE*)blk - 1;

#if KNL_CFG_CHK_PAR
    if ( !knl_mem_chkareaflag(aq, KNL_MEM_AREA_USE) ) {
        return E_PAR;
    }
#endif
    knl_mem_clrareaflag(aq, KNL_MEM_AREA_USE);

    if ( !knl_mem_chkareaflag(aq->next, KNL_MEM_AREA_USE) ) {
        /* Merge to the next area */
        knl_mem_removefreeque(aq->next + 1);
        knl_mem_removeareaque(aq->next);
    }
    if ( !knl_mem_chkareaflag(aq->prev, KNL_MEM_AREA_USE) ) {
        /* Merge to the previous area */
        QUEUE *fq;
        aq = aq->prev;
        fq = (aq == &(mplcb->areaque)) ? (QUEUE*)(mplcb->mempool) : aq + 1;

        knl_mem_removefreeque(fq);
        knl_mem_removeareaque(aq->next);
    }

    /* Register free area onto FreeQue */
    if ( ( aq == &(mplcb->areaque) ) || ( aq->next == &(mplcb->areaque_end) ) ) {
        knl_mpl_appendfreeareabound(mplcb, aq);
    } else {
        knl_mem_appendfreearea(imacb, aq);
    }

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_KNL_MPL_RELBLK */

#ifdef KNL_CFG_USE_FUNC_KNL_MPL_WAKEUP
/*
 *    Function Name : knl_mpl_wakeup
 *    Create Date   : 2012/7/9-2013/4/22
 *    Author        : wangshb
 *    Description   : Allocate memory and release wait task,
 *                    as long as there are enough free memory.
 *    Param         : MPLCB *mplcb : address of mpl control block
 *    Return Code   : ER : Error code
 */
static void knl_mpl_wakeup( MPLCB *mplcb )
{
    TCB  *top;
    VP  blk;
    int32_t  blksz;

    while ( !knl_que_isempty(&mplcb->wait_queue) ) {
        top = (TCB*)mplcb->wait_queue.next;
        blksz = top->winfo.mpl.blksz;

        /* Check free space */
        if ( blksz > knl_mpl_maxfreesize(mplcb) ) {
            break;
        }

        /* Get memory block */
        blk = knl_mpl_getblk(mplcb, blksz);
        *top->winfo.mpl.p_blk = blk;

        /* Release wait task */
        knl_tsk_wairelok(top);
    }
}
#endif /* KNL_CFG_USE_FUNC_KNL_MPL_WAKEUP */

#ifdef KNL_CFG_USE_FUNC_TK_GET_MPL
/*
 *    Function Name : knl_mpl_chgpri
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Processing if the priority of wait task changes.
 *                    You need to execute with interrupt disable.
 *    Param         : TCB *tcb : task control block to change the priority
 *                    int32_t oldpri : old priority, maybe is -1
 *    Return Code   : void
 */
void knl_mpl_chgpri( TCB *tcb, int32_t oldpri )
{
    MPLCB    *mplcb;

    mplcb = KNL_MPL_GET_CB(tcb->wid);
    if ( oldpri >= 0 ) {
        /* Reorder wait line */
        knl_tsk_gcbchgpri((GCB*)mplcb, tcb);
    }

    /* From the new top task of a wait queue, free the assign
       wait of memory blocks as much as possible. */
    knl_mpl_wakeup(mplcb);
}
#endif /* KNL_CFG_USE_FUNC_TK_GET_MPL */

#ifdef KNL_CFG_USE_FUNC_KNL_MPL_GETNAME
#if TK_USE_OBJ_NAME
/*
 *    Function Name : knl_mpl_getname
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get object name from control block
 *    Param         : ID id :variable size memory pool id
 *                    uint8_t **name : variable size memory pool DS name pointer
 *    Return Code   : ER : Error code
 */
ER knl_mpl_getname(ID id, uint8_t **name)
{
    MPLCB    *mplcb;
    ER    ercd = E_OK;

    KNL_CHK_MPLID(id)

    KNL_INT_BEGIN_DISABLE
    mplcb = KNL_MPL_GET_CB(id);
    if ( mplcb->mplid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
    if ( (mplcb->mplatr & TA_DSNAME) == 0u ) {
        ercd = E_OBJ;
        goto error_exit;
    }
    *name = mplcb->name;

error_exit:
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* TK_USE_OBJ_NAME */
#endif /* KNL_CFG_USE_FUNC_KNL_MPL_GETNAME */

#ifdef KNL_CFG_USE_FUNC_KNL_MPL_INIT
/*
 *    Function Name : knl_mpl_init
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Initialization of variable size memory pool control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
ER knl_mpl_init( void )
{
    MPLCB  *mplcb, *mplcbend;

    if ( TK_NUM_MPLID < 1 ) {
        return E_SYS;
    }

    /* Register all control blocks onto FreeQue */
    knl_que_init(&knl_mpl_freecb);
    mplcbend = knl_mpl_cbtable + TK_NUM_MPLID;
    for ( mplcb = knl_mpl_cbtable; mplcb < mplcbend; mplcb++ ) {
        mplcb->mplid = 0;
        knl_que_insert(&mplcb->wait_queue, &knl_mpl_freecb);
    }

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_KNL_MPL_INIT */

#ifdef KNL_CFG_USE_FUNC_TK_GET_MPL
/*
 *    Function Name : knl_mpl_relwai
 *    Create Date   : 2012/7/9-2014/2/1
 *    Author        : wangshb
 *    Description   : Processing if the wait task is freed
 *    Param         : TCB *tcb : task to release waiting
 *    Return Code   : ER : Error code
 */
void knl_mpl_relwai( TCB *tcb )
{
    knl_mpl_chgpri(tcb, -1);
}
#endif /* KNL_CFG_USE_FUNC_TK_GET_MPL */

#ifdef KNL_CFG_USE_FUNC_TK_CRE_MPL
/*
 *    Function Name : tk_cre_mpl_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Create variable size memory pool
 *    Param         : T_CMPL *pk_cmpl : Information about the mpl to be created
 *    Return Code   : ID : Variable-size memory pool ID or Error code
 */
SYSCALL ID tk_cre_mpl_impl( T_CMPL *pk_cmpl )
{
#if KNL_CFG_CHK_RSATR
    const ATR VALID_MPLATR = {
         TA_TPRI
        |TA_RNG3
        |TA_USERBUF
#if TK_USE_OBJ_NAME
        |TA_DSNAME
#endif
    };
#endif
    MPLCB  *mplcb;
    ID  mplid;
    int32_t  mplsz;
    VP  mempool;
    ER  ercd;

    KNL_CHK_RSATR(pk_cmpl->mplatr, VALID_MPLATR)
    KNL_CHK_PAR(pk_cmpl->mplsz > 0)
#if !TK_USE_MALLOC
    /* TA_USERBUF must be specified if configured in no Imalloc */
    KNL_CHK_PAR((pk_cmpl->mplatr & TA_USERBUF) != 0);
#endif
    KNL_CHK_DISPATCH()

    mplsz = knl_mem_roundsize(pk_cmpl->mplsz);

#if TK_USE_MALLOC
    if ( (pk_cmpl->mplatr & TA_USERBUF) != 0u ) {
        /* Size of user buffer must be multiples of sizeof(QUEUE)
           and larger than sizeof(QUEUE)*2 */
        if ( mplsz != pk_cmpl->mplsz ) {
            return E_PAR;
        }
        /* Use user buffer */
        mempool = pk_cmpl->bufptr;
    } else {
        /* Allocate memory for memory pool */
        mempool = knl_mem_imalloc((uint32_t)mplsz);
        if ( mempool == NULL ) {
            return E_NOMEM;
        }
    }
#else
    /* Size of user buffer must be multiples of sizeof(QUEUE)
        and larger than sizeof(QUEUE)*2 */
    if ( mplsz != pk_cmpl->mplsz ) {
        return E_PAR;
    }
    /* Use user buffer */
    mempool = pk_cmpl->bufptr;
#endif

    KNL_TSK_BEGIN_CRITICAL_SECTION
    /* Get control block from FreeQue */
    mplcb = (MPLCB*)knl_que_removenext(&knl_mpl_freecb);
    if ( mplcb == NULL ) {
        ercd = E_LIMIT;
    } else {
        mplid = KNL_CFG_ID_MPL(mplcb - knl_mpl_cbtable);

        /* Initialize control block */
        knl_que_init(&mplcb->wait_queue);
        mplcb->mplid  = mplid;
        mplcb->exinf  = pk_cmpl->exinf;
        mplcb->mplatr = pk_cmpl->mplatr;
        mplcb->mplsz  = mplsz;
#if TK_USE_OBJ_NAME
        if ( (pk_cmpl->mplatr & TA_DSNAME) != 0u ) {
            strncpy((char*)&mplcb->name, (char*)&pk_cmpl->dsname, TK_OBJ_NAME_LENGTH);
        }
#endif

        mplcb->mempool = mempool;

        /* Initialize memory pool */
        knl_mpl_initblk(mplcb);

        ercd = mplid;
    }
    KNL_TSK_END_CRITICAL_SECTION

#if TK_USE_MALLOC
    if ( (ercd < E_OK) && ((pk_cmpl->mplatr & TA_USERBUF) == 0u) ) {
        knl_mem_ifree(mempool);
    }
#endif

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_CRE_MPL */

#ifdef KNL_CFG_USE_FUNC_TK_DEL_MPL
/*
 *    Function Name : tk_del_mpl_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Delete variable size memory pool
 *    Param         : ID mplid : Variable-size memory pool ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_del_mpl_impl( ID mplid )
{
    MPLCB  *mplcb;
    VP  mempool = NULL;
    ATR  memattr = 0u;
    ER  ercd = E_OK;

    KNL_CHK_MPLID(mplid)
    KNL_CHK_DISPATCH()

    mplcb = KNL_MPL_GET_CB(mplid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( mplcb->mplid == 0 ) {
        ercd = E_NOEXS;
    } else {
        mempool = mplcb->mempool;
        memattr = mplcb->mplatr;

        /* Free wait state of task (E_DLT) */
        knl_tsk_waidel(&mplcb->wait_queue);

        /* Return to FreeQue */
        knl_que_insert(&mplcb->wait_queue, &knl_mpl_freecb);
        mplcb->mplid = 0;
    }
    KNL_TSK_END_CRITICAL_SECTION

#if TK_USE_MALLOC
    if ( (ercd == E_OK) && ((memattr & TA_USERBUF) == 0u) ) {
        knl_mem_ifree(mempool);
    }
#endif

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_DEL_MPL */

#ifdef KNL_CFG_USE_FUNC_TK_GET_MPL
/*
 *    Function Name : tk_get_mpl_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get variable size memory block
 *    Param         : ID mplid : Variable-size memory pool ID
 *                    int32_t blksz : Memory block size (in bytes)
 *                    VP* p_blk : Block start address
 *                    TMO tmout : Timeout
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_get_mpl_impl( ID mplid, int32_t blksz, VP* p_blk, TMO tmout )
{
    TCB  *tcb;
    MPLCB  *mplcb;
    VP  blk = NULL;
    ER  ercd = E_OK;

    KNL_CHK_MPLID(mplid)
    KNL_CHK_PAR(blksz > 0)
    KNL_CHK_TMOUT(tmout)
    KNL_CHK_DISPATCH()

    mplcb = KNL_MPL_GET_CB(mplid);
    blksz = knl_mem_roundsize(blksz);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( mplcb->mplid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }

#if KNL_CFG_CHK_PAR
    if ( blksz > mplcb->mplsz ) {
        ercd = E_PAR;
        goto error_exit;
    }
#endif

    tcb = knl_tsk_gcbwaiquetop((GCB*)mplcb, knl_tsk_execution);
    blk = knl_mpl_getblk(mplcb, blksz);
    if ( ( tcb == knl_tsk_execution ) && ( blk != NULL ) ) {
        /* Get memory block */
        *p_blk = blk;
    } else {
        /* Ready for wait */
        knl_tsk_execution->wspec = ( (mplcb->mplatr & TA_TPRI) != 0u )?
                    &knl_mpl_wspec_tpri: &knl_mpl_wspec_tfifo;
        knl_tsk_execution->wercd = &ercd;
        knl_tsk_execution->winfo.mpl.blksz = blksz;
        knl_tsk_execution->winfo.mpl.p_blk = p_blk;
        knl_tsk_gcbmakwai((GCB*)mplcb, tmout);
    }

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_GET_MPL */

#ifdef KNL_CFG_USE_FUNC_TK_REF_MPL
/*
 *    Function Name : tk_ref_mpl_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer variable size memory pool state
 *    Param         : ID mplid : variable size memory pool ID
 *                    T_RMPL *pk_rmpl : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_ref_mpl_impl( ID mplid, T_RMPL *pk_rmpl )
{
    MPLCB    *mplcb;
    QUEUE    *fq, *q;
    int32_t    frsz, blksz;
    ER    ercd = E_OK;

    KNL_CHK_MPLID(mplid)
    KNL_CHK_DISPATCH()

    mplcb = KNL_MPL_GET_CB(mplid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( mplcb->mplid == 0 ) {
        ercd = E_NOEXS;
    } else {
        pk_rmpl->exinf = mplcb->exinf;
        pk_rmpl->wtsk  = knl_tsk_waitid(&mplcb->wait_queue);
        frsz = 0;
        for ( fq = mplcb->freeque.next; fq != &mplcb->freeque; fq = fq->next ) {
            blksz = KNL_MEM_FREESIZE(fq);
            frsz += blksz;
            for ( q = (fq+1)->next; q != NULL; q = q->next ) {
                frsz += blksz;
            }
        }
        pk_rmpl->frsz  = frsz;
        pk_rmpl->maxsz = knl_mpl_maxfreesize(mplcb);
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_REF_MPL */

#ifdef KNL_CFG_USE_FUNC_TK_REL_MPL
/*
 *    Function Name : tk_rel_mpl_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Return variable size memory block
 *    Param         : ID mplid : Variable-size memory pool ID
 *                    VP blk : Memory block start address
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_rel_mpl_impl( ID mplid, VP blk )
{
    MPLCB  *mplcb;
    ER  ercd = E_OK;

    KNL_CHK_MPLID(mplid)
    KNL_CHK_DISPATCH()

    mplcb = KNL_MPL_GET_CB(mplid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( mplcb->mplid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
#if KNL_CFG_CHK_PAR
    if ( ( (int8_t*)blk < (int8_t*)mplcb->mempool ) || 
         ( (int8_t*)blk > ( (int8_t*)mplcb->mempool + mplcb->mplsz ) ) ) {
        ercd = E_PAR;
        goto error_exit;
    }
#endif

    /* Free memory block */
    ercd = knl_mpl_relblk(mplcb, blk);
    if ( ercd < E_OK ) {
        goto error_exit;
    }

    /* Assign memory block to waiting task */
    knl_mpl_wakeup(mplcb);

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_REL_MPL */

/*
 *    Debugger support function
 */
#if TK_USE_DBGSPT

#ifdef KNL_CFG_USE_FUNC_TD_LST_MPL
/*
 *    Function Name : td_lst_mpl_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer variable size memory pool object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
SYSCALL int32_t td_lst_mpl_impl( ID list[], int32_t nent )
{
    MPLCB *mplcb, *mplcbend;
    int32_t n = 0;

    KNL_INT_BEGIN_DISABLE
    mplcbend = knl_mpl_cbtable + TK_NUM_MPLID;
    for ( mplcb = knl_mpl_cbtable; mplcb < mplcbend; mplcb++ ) {
        if ( mplcb->mplid != 0 ) {
            if ( n < nent ) {
                *list = KNL_CFG_ID_MPL(mplcb - knl_mpl_cbtable);
                list++;
            }
            n++;
        }
    }
    KNL_INT_END_DISABLE

    return n;
}
#endif /* KNL_CFG_USE_FUNC_TD_LST_MPL */

#ifdef KNL_CFG_USE_FUNC_TD_MPL_QUE
/*
 *    Function Name : td_mpl_que_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference variable size memory pool wait queue
 *    Param         : ID mplid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
SYSCALL int32_t td_mpl_que_impl( ID mplid, ID list[], int32_t nent )
{
    MPLCB    *mplcb;
    QUEUE    *q;
    ER    ercd = E_OK;

    KNL_CHK_MPLID(mplid)

    mplcb = KNL_MPL_GET_CB(mplid);

    KNL_INT_BEGIN_DISABLE
    if ( mplcb->mplid == 0 ) {
        ercd = E_NOEXS;
    } else {
        int32_t n = 0;
        for ( q = mplcb->wait_queue.next; q != &mplcb->wait_queue; q = q->next ) {
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
#endif /* KNL_CFG_USE_FUNC_TD_MPL_QUE */

#ifdef KNL_CFG_USE_FUNC_TD_REF_MPL
/*
 *    Function Name : td_ref_mpl_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference variable size memory pool state
 *    Param         : ID mplid : Object ID
 *                    TD_RMPL *pk_rmpl : Address of status information packet
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_ref_mpl_impl( ID mplid, TD_RMPL *pk_rmpl )
{
    MPLCB    *mplcb;
    QUEUE    *fq, *q;
    int32_t    frsz, blksz;
    ER    ercd = E_OK;

    KNL_CHK_MPLID(mplid)

    mplcb = KNL_MPL_GET_CB(mplid);

    KNL_INT_BEGIN_DISABLE
    if ( mplcb->mplid == 0 ) {
        ercd = E_NOEXS;
    } else {
        pk_rmpl->exinf = mplcb->exinf;
        pk_rmpl->wtsk  = knl_tsk_waitid(&mplcb->wait_queue);
        frsz = 0;
        for ( fq = mplcb->freeque.next; fq != &mplcb->freeque; fq = fq->next ) {
            blksz = KNL_MEM_FREESIZE(fq);
            frsz += blksz;
            for ( q = (fq+1)->next; q != NULL; q = q->next ) {
                frsz += blksz;
            }
        }
        pk_rmpl->frsz  = frsz;
        pk_rmpl->maxsz = knl_mpl_maxfreesize(mplcb);
    }
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TD_REF_MPL */

#endif /* TK_USE_DBGSPT */

#endif /* KNL_CFG_MAX_MPLID */
