/*
 *----------------------------------------------------------------------------------------
 *    Kernel/flg
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
 *    File Name      : tk_flgimpl.c
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Event Flag Function.
 *----------------------------------------------------------------------------------------
 */

#include "tk_flgimpl.h"

#if KNL_CFG_MAX_FLGID > 0

/*
 * Event flag control block
 */
typedef struct eventflag_control_block {
        QUEUE                wait_queue;         /* Event flag wait queue */
        ID                   flgid;              /* Event flag ID */
        VP                   exinf;              /* Extended information */
        ATR                  flgatr;             /* Event flag attribute */
        uint32_t             flgptn;             /* Event flag current pattern */
#if TK_USE_OBJ_NAME                              /* name */
        uint8_t              name[TK_OBJ_NAME_LENGTH];    
#endif
}                            FLGCB;

/*
 * Event flag control block table and free table pointer definition
 */
#ifdef KNL_CFG_USE_FUNC_KNL_FLG_CBTABLE
Zeroinit FLGCB knl_flg_cbtable[TK_NUM_FLGID];    /* Event flag control block */
Zeroinit QUEUE knl_flg_freecb;                   /* FreeQue */
#define KNL_FLG_GET_CB(id)   ( &knl_flg_cbtable[KNL_CFG_INDEX_FLG(id)] )
#endif /* KNL_CFG_USE_FUNC_KNL_FLG_CBTABLE */

/*
 * Declaration of local function prototype  
 */
static void knl_flg_chgpri( TCB *tcb, int32_t oldpri );

#ifdef KNL_CFG_USE_FUNC_TK_WAI_FLG
/*
 * Definition of event flag wait specification
 */
static WSPEC knl_flg_wspec_tfifo = { TTW_FLG, NULL, NULL };
static WSPEC knl_flg_wspec_tpri  = { TTW_FLG, knl_flg_chgpri, NULL };
#endif /* KNL_CFG_USE_FUNC_TK_WAI_FLG */

/*
 *    Description   : Check for event flag wait release condition
 *    Param         : FLGCB *flgcb : flag control block
 *                    uint32_t waiptn : wait pattern
 *                    uint32_t wfmode : wait mode
 *    Return Code   : bool8_t : 
 */
Inline bool8_t knl_flg_cond( FLGCB *flgcb, uint32_t waiptn, uint32_t wfmode )
{
    int8_t  b;
    
    if ( (wfmode & TWF_ORW) != 0u ) {
        b = ( (flgcb->flgptn & waiptn) != 0u )? true: false;
        return (bool8_t)b;
    } else {
        b = ( (flgcb->flgptn & waiptn) == waiptn )? true: false;
        return (bool8_t)b;
    }
}

#ifdef KNL_CFG_USE_FUNC_TK_WAI_FLG
/*
 *    Function Name : knl_flg_chgpri
 *    Create Date   : 2012/7/9-2013/4/22
 *    Author        : wangshb
 *    Description   : Processing if the priority of wait task changes
 *    Param         : TCB *tcb : task control block to change the priority
 *                    int32_t oldpri : old priority, maybe is -1
 *    Return Code   : void
 */
static void knl_flg_chgpri( TCB *tcb, int32_t oldpri )
{
    FLGCB *flgcb;

    flgcb = KNL_FLG_GET_CB(tcb->wid);
    knl_tsk_gcbchgpri((GCB*)flgcb, tcb);
}
#endif /* KNL_CFG_USE_FUNC_TK_WAI_FLG */

#ifdef KNL_CFG_USE_FUNC_KNL_FLG_GETNAME
#if TK_USE_OBJ_NAME
/*
 *    Function Name : knl_flg_getname
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get object name from control block
 *    Param         : ID id : event flag id
 *                    uint8_t **name : event flag DS name pointer
 *    Return Code   : ER : Error code
 */
ER knl_flg_getname(ID id, uint8_t **name)
{
    FLGCB *flgcb;
    ER ercd = E_OK;

    KNL_CHK_FLGID(id)

    KNL_INT_BEGIN_DISABLE
    flgcb = KNL_FLG_GET_CB(id);
    if ( flgcb->flgid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
    if ( (flgcb->flgatr & TA_DSNAME) == 0u ) {
        ercd = E_OBJ;
        goto error_exit;
    }
    *name = flgcb->name;

error_exit:
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* TK_USE_OBJ_NAME */
#endif /* KNL_CFG_USE_FUNC_KNL_FLG_GETNAME */

#ifdef KNL_CFG_USE_FUNC_KNL_FLG_INIT
/*
 *    Function Name : knl_flg_init
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Initialization of event flag control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
ER knl_flg_init( void )
{
    FLGCB *flgcb, *flgcbend;

    /* Get system information */
    if ( TK_NUM_FLGID < 1 ) {
        return E_SYS;
    }

    /* Register all control blocks onto FreeQue */
    knl_que_init(&knl_flg_freecb);
    flgcbend = knl_flg_cbtable + TK_NUM_FLGID;
    for ( flgcb = knl_flg_cbtable; flgcb < flgcbend; flgcb++ ) {
        flgcb->flgid = 0;
        knl_que_insert(&flgcb->wait_queue, &knl_flg_freecb);
    }

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_KNL_FLG_INIT */

#ifdef KNL_CFG_USE_FUNC_TK_CLR_FLG
/*
 *    Function Name : tk_clr_flg_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Clear event flag
 *    Param         : ID flgid : event flag ID
 *                    uint32_t clrptn : Bit pattern to be clear
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_clr_flg_impl( ID flgid, uint32_t clrptn )
{
    FLGCB *flgcb;
    ER ercd = E_OK;

    KNL_CHK_FLGID(flgid)

    flgcb = KNL_FLG_GET_CB(flgid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( flgcb->flgid == 0 ) {
        ercd = E_NOEXS;
    } else {
        flgcb->flgptn &= clrptn;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_CLR_FLG */

#ifdef KNL_CFG_USE_FUNC_TK_CRE_FLG
/*
 *    Function Name : tk_cre_flg_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Create event flag
 *    Param         : T_CFLG *pk_cflg : Information about the event flag to be created
 *    Return Code   : ID : event flag ID or Error code
 */
SYSCALL ID tk_cre_flg_impl( T_CFLG *pk_cflg )
{
#if KNL_CFG_CHK_RSATR
    const ATR VALID_FLGATR = {
         TA_TPRI
        |TA_WMUL
#if TK_USE_OBJ_NAME
        |TA_DSNAME
#endif
    };
#endif
    FLGCB *flgcb;
    ID flgid;
    ER ercd;

    KNL_CHK_RSATR(pk_cflg->flgatr, VALID_FLGATR)

    KNL_TSK_BEGIN_CRITICAL_SECTION
    /* Get control block from FreeQue */
    flgcb = (FLGCB*)knl_que_removenext(&knl_flg_freecb);
    if ( flgcb == NULL ) {
        ercd = E_LIMIT;
    } else {
        flgid = KNL_CFG_ID_FLG(flgcb - knl_flg_cbtable);

        /* Initialize control block */
        knl_que_init(&flgcb->wait_queue);
        flgcb->flgid = flgid;
        flgcb->exinf = pk_cflg->exinf;
        flgcb->flgatr = pk_cflg->flgatr;
        flgcb->flgptn = pk_cflg->iflgptn;
#if TK_USE_OBJ_NAME
        if ( (pk_cflg->flgatr & TA_DSNAME) != 0u ) {
            strncpy((char*)&flgcb->name, (char*)&pk_cflg->dsname, TK_OBJ_NAME_LENGTH);
        }
#endif
        ercd = flgid;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_CRE_FLG */

#ifdef KNL_CFG_USE_FUNC_TK_DEL_FLG
/*
 *    Function Name : tk_del_flg_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Delete event flag
 *    Param         : ID flgid : event flag ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_del_flg_impl( ID flgid )
{
    FLGCB *flgcb;
    ER ercd = E_OK;

    KNL_CHK_FLGID(flgid)

    flgcb = KNL_FLG_GET_CB(flgid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( flgcb->flgid == 0 ) {
        ercd = E_NOEXS;
    } else {
        /* Release wait state of task (E_DLT) */
        knl_tsk_waidel(&flgcb->wait_queue);

        /* Return to FreeQue */
        knl_que_insert(&flgcb->wait_queue, &knl_flg_freecb);
        flgcb->flgid = 0;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_DEL_FLG */

#ifdef KNL_CFG_USE_FUNC_TK_REF_FLG
/*
 *    Function Name : tk_ref_flg_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer event flag state
 *    Param         : ID flgid : event flag ID
 *                    T_RFLG *pk_rflg : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_ref_flg_impl( ID flgid, T_RFLG *pk_rflg )
{
    FLGCB *flgcb;
    ER ercd = E_OK;

    KNL_CHK_FLGID(flgid)

    flgcb = KNL_FLG_GET_CB(flgid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( flgcb->flgid == 0 ) {
        ercd = E_NOEXS;
    } else {
        pk_rflg->exinf = flgcb->exinf;
        pk_rflg->wtsk = knl_tsk_waitid(&flgcb->wait_queue);
        pk_rflg->flgptn = flgcb->flgptn;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_REF_FLG */

#ifdef KNL_CFG_USE_FUNC_TK_SET_FLG
/*
 *    Function Name : tk_set_flg_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Set event flag
 *    Param         : ID flgid : event flag ID
 *                    uint32_t setptn : Bit pattern to be set
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_set_flg_impl( ID flgid, uint32_t setptn )
{
    FLGCB *flgcb;
    TCB *tcb;
    QUEUE *queue;
    uint32_t wfmode, waiptn;
    ER ercd = E_OK;
    bool8_t brk;

    KNL_CHK_FLGID(flgid)

    flgcb = KNL_FLG_GET_CB(flgid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( flgcb->flgid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }

    /* Set event flag */
    flgcb->flgptn |= setptn;

    /* Search task which should be released */
    queue = flgcb->wait_queue.next;
    while ( queue != &flgcb->wait_queue ) {
        tcb = (TCB*)queue;
        queue = queue->next;

        /* Meet condition for release wait? */
        waiptn = tcb->winfo.flg.waiptn;
        wfmode = tcb->winfo.flg.wfmode;
        if ( knl_flg_cond(flgcb, waiptn, wfmode) ) {

            /* Release wait */
            *tcb->winfo.flg.p_flgptn = flgcb->flgptn;
            knl_tsk_wairelok(tcb);

            /* Clear event flag */
            if ( (wfmode & TWF_BITCLR) != 0u ) {
                flgcb->flgptn &= ~waiptn;
                if ( flgcb->flgptn == 0u ) {
                    brk = true;
                }
            }
            if ( (wfmode & TWF_CLR) != 0u ) {
                flgcb->flgptn = 0u;
                brk = true;
            }
            if ( brk ) {
                break;
            }
        }
    }

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_SET_FLG */

#ifdef KNL_CFG_USE_FUNC_TK_WAI_FLG
/*
 *    Function Name : tk_wai_flg_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Wait event flag
 *    Param         : ID flgid : event flag ID
 *                    uint32_t waiptn : Wait bit pattern
 *                    uint32_t wfmode : Wait release condition
 *                    uint32_t *p_flgptn : Event flag bit pattern
 *                    TMO tmout : timeout
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_wai_flg_impl( ID flgid, uint32_t waiptn, uint32_t wfmode,                  \
                            uint32_t *p_flgptn, TMO tmout)
{
    FLGCB *flgcb;
    ER ercd = E_OK;

    KNL_CHK_FLGID(flgid)
    KNL_CHK_PAR(waiptn != 0u)
    KNL_CHK_PAR((wfmode & ~(TWF_ORW|TWF_CLR|TWF_BITCLR)) == 0u)
    KNL_CHK_TMOUT(tmout)
    KNL_CHK_DISPATCH()

    flgcb = KNL_FLG_GET_CB(flgid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( flgcb->flgid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
    if ( ( !knl_que_isempty(&flgcb->wait_queue) ) && ( (flgcb->flgatr & TA_WMUL) == 0u )) {
        /* Disable multiple tasks wait */
        ercd = E_OBJ;
        goto error_exit;
    }

    /* Meet condition for release wait? */
    if ( knl_flg_cond(flgcb, waiptn, wfmode) ) {
        *p_flgptn = flgcb->flgptn;

        /* Clear event flag */
        if ( (wfmode & TWF_BITCLR) != 0u ) {
            flgcb->flgptn &= ~waiptn;
        }
        if ( (wfmode & TWF_CLR) != 0u ) {
            flgcb->flgptn = 0u;
        }
    } else {
        /* Ready for wait */
        knl_tsk_execution->wspec = ( (flgcb->flgatr & TA_TPRI) != 0u )?
                    &knl_flg_wspec_tpri: &knl_flg_wspec_tfifo;
        knl_tsk_execution->wercd = &ercd;
        knl_tsk_execution->winfo.flg.waiptn = waiptn;
        knl_tsk_execution->winfo.flg.wfmode = wfmode;
        knl_tsk_execution->winfo.flg.p_flgptn = p_flgptn;
        knl_tsk_gcbmakwai((GCB*)flgcb, tmout);
    }

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_WAI_FLG */

/*
 * Debugger support function
 */
#if TK_USE_DBGSPT

#ifdef KNL_CFG_USE_FUNC_TD_FLG_QUE
/*
 *    Function Name : td_flg_que_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference event flag wait queue
 *    Param         : ID flgid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
SYSCALL int32_t td_flg_que_impl( ID flgid, ID list[], int32_t nent )
{
    FLGCB *flgcb;
    QUEUE *q;
    ER ercd = E_OK;

    KNL_CHK_FLGID(flgid)

    flgcb = KNL_FLG_GET_CB(flgid);

    KNL_INT_BEGIN_DISABLE
    if ( flgcb->flgid == 0 ) {
        ercd = E_NOEXS;
    } else {
        int32_t n = 0;
        for ( q = flgcb->wait_queue.next; q != &flgcb->wait_queue; q = q->next ) {
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
#endif /* KNL_CFG_USE_FUNC_TD_FLG_QUE */

#ifdef KNL_CFG_USE_FUNC_TD_LST_FLG
/*
 *    Function Name : td_lst_flg_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer event flag object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
SYSCALL int32_t td_lst_flg_impl( ID list[], int32_t nent )
{
    FLGCB *flgcb, *flgcbend;
    int32_t n = 0;

    KNL_INT_BEGIN_DISABLE
    flgcbend = knl_flg_cbtable + TK_NUM_FLGID;
    for ( flgcb = knl_flg_cbtable; flgcb < flgcbend; flgcb++ ) {
        if ( flgcb->flgid != 0 ) {
            if ( n < nent ) {
                *list = flgcb->flgid;
                list++;
            }
            n++;
        }
    }
    KNL_INT_END_DISABLE

    return n;
}
#endif /* KNL_CFG_USE_FUNC_TD_LST_FLG */

#ifdef KNL_CFG_USE_FUNC_TD_REF_FLG
/*
 *    Function Name : td_ref_flg_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference event flag object state
 *    Param         : ID flgid : Object ID
 *                    TD_RFLG *pk_rflg : Address of status information packet
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_ref_flg_impl( ID flgid, TD_RFLG *pk_rflg )
{
    FLGCB *flgcb;
    ER ercd = E_OK;

    KNL_CHK_FLGID(flgid)

    flgcb = KNL_FLG_GET_CB(flgid);

    KNL_INT_BEGIN_DISABLE
    if ( flgcb->flgid == 0 ) {
        ercd = E_NOEXS;
    } else {
        pk_rflg->exinf = flgcb->exinf;
        pk_rflg->wtsk = knl_tsk_waitid(&flgcb->wait_queue);
        pk_rflg->flgptn = flgcb->flgptn;
    }
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TD_REF_FLG */

#endif /* TK_USE_DBGSPT */

#endif /* KNL_CFG_MAX_FLGID */
