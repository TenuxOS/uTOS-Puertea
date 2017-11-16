/*
 *----------------------------------------------------------------------------------------
 *    Kernel/alm
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
 *    File Name      : tk_almimpl.c
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Alarm Management Function .
 *----------------------------------------------------------------------------------------
 */

#include "tk_almimpl.h"

#if KNL_CFG_MAX_ALMID > 0

/*
 * Alarm handler control block
 */
typedef struct alarm_handler_control_block {
        VP                   exinf;              /* Extended information */
        ATR                  almatr;             /* Alarm handler attribute */
        FP                   almhdr;             /* Alarm handler address */
        uint32_t             almstat;            /* Alarm handler state */
        TMEB                 almtmeb;            /* Timer event block */
#if TK_USE_OBJ_NAME                              /* name */
        uint8_t              name[TK_OBJ_NAME_LENGTH]; 
#endif
}                            ALMCB;

/*
 * Alarm handler control block table and free table pointer definition
 */
#ifdef KNL_CFG_USE_FUNC_KNL_ALM_CBTABLE
Zeroinit ALMCB knl_alm_cbtable[TK_NUM_ALMID];    /* Alarm handler control block */
Zeroinit QUEUE knl_alm_freecb;                   /* FreeQue */
#define KNL_ALM_GET_CB(id)   ( &knl_alm_cbtable[KNL_CFG_INDEX_ALM(id)] )
#endif /* KNL_CFG_USE_FUNC_KNL_ALM_CBTABLE */

/*
 * Declaration of local function prototype  
 */
static void knl_alm_callhdr( ALMCB *almcb );
static void knl_alm_imm_callhdr( ALMCB *almcb );

/*
 *    Description   : Register onto tick event queue
 *    Param         : ALMCB *almcb : Alarm control block
 *                    RELTIM reltim : Alarm handler start time (alarm time)
 *    Return Code   : void
 */
Inline void knl_alm_tick_insert( ALMCB *almcb, RELTIM reltim )
{
    LSYSTIM ltime;

    /* To guarantee to start the handler after the specified time,
       add KNL_CFG_TIMER_PERIOD */
    ltime = ll_add(knl_tck_current_time, uitoll(reltim));
    ltime = ll_add(ltime, uitoll(KNL_CFG_TIMER_PERIOD));

    knl_tck_insabs(&almcb->almtmeb, ltime, (CBACK)knl_alm_callhdr, almcb);
}

#ifdef KNL_CFG_USE_FUNC_KNL_ALM_CALLHDR
/*
 *    Function Name : knl_alm_callhdr
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Call Alarm handler start routine
 *    Param         : ALMCB *almcb : Alarm control block
 *    Return Code   : void
 */
static void knl_alm_callhdr( ALMCB *almcb )
{
    almcb->almstat &= ~TALM_STA;

    /* Execute alarm handler/ Enable interrupt nesting */
    KNL_INT_ENABLE_UPTO(KNL_CFG_TIMER_INTLEVEL)
    (*(almcb->almhdr))(almcb->exinf);
    KNL_INT_DISABLE
}
#endif /* KNL_CFG_USE_FUNC_KNL_ALM_CALLHDR */

#ifdef KNL_CFG_USE_FUNC_TK_STA_ALM
/*
 *    Function Name : knl_alm_imm_callhdr
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Alarm handler immediate call
 *    Param         : ALMCB *almcb : Alarm control block
 *    Return Code   : void
 */
static void knl_alm_imm_callhdr( ALMCB *almcb )
{
    almcb->almstat &= ~TALM_STA;

    /* Execute alarm handler in task-independent part
       (Keep interrupt disabled) */
    KNL_TSK_ENTER_IDP
    (*(almcb->almhdr))(almcb->exinf);
    KNL_TSK_LEAVE_IDP
}
#endif /* KNL_CFG_USE_FUNC_TK_STA_ALM */

#ifdef KNL_CFG_USE_FUNC_KNL_ALM_GETNAME
#if TK_USE_OBJ_NAME
/*
 *    Function Name : knl_alm_getname
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get object name from control block
 *    Param         : ID id : alarm handler id
 *                    uint8_t **name : alarm handler DS name pointer
 *    Return Code   : ER : Error code
 */
ER knl_alm_getname( ID id, uint8_t **name )
{
    ALMCB *almcb;
    ER ercd = E_OK;

    KNL_CHK_ALMID(id)

    KNL_INT_BEGIN_DISABLE
    almcb = KNL_ALM_GET_CB(id);
    if ( almcb->almhdr == NULL ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
    if ( (almcb->almatr & TA_DSNAME) == 0u ) {
        ercd = E_OBJ;
        goto error_exit;
    }
    *name = almcb->name;

error_exit:
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* TK_USE_OBJ_NAME */
#endif /* KNL_CFG_USE_FUNC_KNL_ALM_GETNAME */

#ifdef KNL_CFG_USE_FUNC_KNL_ALM_INIT
/*
 *    Function Name : knl_alm_init
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Initialization of alarm handler control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
ER knl_alm_init( void )
{
    ALMCB *almcb, *almcbend;

    /* Get system information */
    if ( TK_NUM_ALMID < 1 ) {
        return E_SYS;
    }

    /* Register all control blocks onto FreeQue */
    knl_que_init(&knl_alm_freecb);
    almcbend = knl_alm_cbtable + TK_NUM_ALMID;
    for ( almcb = knl_alm_cbtable; almcb < almcbend; almcb++ ) {
        almcb->almhdr = NULL; /* Unregistered handler */
        knl_que_insert((QUEUE*)almcb, &knl_alm_freecb);
    }

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_KNL_ALM_INIT */

#ifdef KNL_CFG_USE_FUNC_TK_CRE_ALM
/*
 *    Function Name : tk_cre_alm_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Create alarm handler
 *    Param         : T_CALM *pk_calm : Address of alarm handler definition packet
 *    Return Code   : ID : alarm handler ID or Error code
 */
SYSCALL ID tk_cre_alm_impl( T_CALM *pk_calm )
{
#if KNL_CFG_CHK_RSATR
    const ATR VALID_ALMATR = {
         TA_HLNG
#if TK_USE_OBJ_NAME
        |TA_DSNAME
#endif
    };
#endif
    ALMCB *almcb;
    ER ercd = E_OK;

    KNL_CHK_RSATR(pk_calm->almatr, VALID_ALMATR)
    KNL_CHK_PAR(pk_calm->almhdr != NULL)

    KNL_TSK_BEGIN_CRITICAL_SECTION
    /* Get control block from free queue */
    almcb = (ALMCB*)knl_que_removenext(&knl_alm_freecb);
    if ( almcb == NULL ) {
        ercd = E_LIMIT;
        goto error_exit;
    }

    /* Initialize control block */
    almcb->exinf   = pk_calm->exinf;
    almcb->almatr  = pk_calm->almatr;
    almcb->almhdr  = pk_calm->almhdr;
    almcb->almstat = TALM_STP;
#if TK_USE_OBJ_NAME
    if ( (pk_calm->almatr & TA_DSNAME) != 0u ) {
        strncpy((char*)&almcb->name, (char*)&pk_calm->dsname, TK_OBJ_NAME_LENGTH);
    }
#endif

    ercd = KNL_CFG_ID_ALM(almcb - knl_alm_cbtable);

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_CRE_ALM */

#ifdef KNL_CFG_USE_FUNC_TK_DEL_ALM
/*
 *    Function Name : tk_del_alm_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Delete alarm handler
 *    Param         : ID almid : alarm handler ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_del_alm_impl( ID almid )
{
    ALMCB *almcb;
    ER ercd = E_OK;

    KNL_CHK_ALMID(almid)

    almcb = KNL_ALM_GET_CB(almid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( almcb->almhdr == NULL ) {               /* Unregistered handler */
        ercd = E_NOEXS;
    } else {
        if ( (almcb->almstat & TALM_STA) != 0u ) {
            /* Delete from timer event queue */
            knl_tck_delete(&almcb->almtmeb);
        }

        /* Return to FreeQue */
        knl_que_insert((QUEUE*)almcb, &knl_alm_freecb);
        almcb->almhdr = NULL;                    /* Unregistered handler */
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_DEL_ALM */

#ifdef KNL_CFG_USE_FUNC_TK_REF_ALM
/*
 *    Function Name : tk_ref_alm_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer alarm handler state
 *    Param         : ID almid : alarm handle ID
 *                    T_RALM *pk_ralm : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_ref_alm_impl( ID almid, T_RALM *pk_ralm )
{
    ALMCB *almcb;
    LSYSTIM ltime;
    ER ercd = E_OK;

    KNL_CHK_ALMID(almid)

    almcb = KNL_ALM_GET_CB(almid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( almcb->almhdr == NULL ) {               /* Unregistered handler */
        ercd = E_NOEXS;
    } else {
        if ( (almcb->almstat & TALM_STA) != 0u ) {
            ltime = almcb->almtmeb.time;
            ltime = ll_sub(ltime, knl_tck_current_time);
            ltime = ll_sub(ltime, uitoll(KNL_CFG_TIMER_PERIOD));
            if ( ll_sign(ltime) < 0 ) {
                ltime = ltoll(0);
            }
        } else {
            ltime = ltoll(0);
        }

        pk_ralm->exinf   = almcb->exinf;
        pk_ralm->lfttim  = lltoul(ltime);
        pk_ralm->almstat = almcb->almstat;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_REF_ALM */

#ifdef KNL_CFG_USE_FUNC_TK_STA_ALM
/*
 *    Function Name : tk_sta_alm_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Start alarm handler
 *    Param         : ID almid : alarm handler ID
 *                    RELTIM almtim : Alarm handler start time (alarm time)
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_sta_alm_impl( ID almid, RELTIM almtim )
{
    ALMCB *almcb;
    ER ercd = E_OK;

    KNL_CHK_ALMID(almid)

    almcb = KNL_ALM_GET_CB(almid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( almcb->almhdr == NULL ) {               /* Unregistered handler */
        ercd = E_NOEXS;
        goto error_exit;
    }

    if ( (almcb->almstat & TALM_STA) != 0u ) {
        /* Cancel current settings */
        knl_tck_delete(&almcb->almtmeb);
    }

    if ( almtim > 0u ) {
        /* Register onto timer event queue */
        knl_alm_tick_insert(almcb, almtim);
        almcb->almstat |= TALM_STA;
    } else {
        /* Immediate execution */
        knl_alm_imm_callhdr(almcb);
    }

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_STA_ALM */

#ifdef KNL_CFG_USE_FUNC_TK_STP_ALM
/*
 *    Function Name : tk_stp_alm_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Stop alarm handler
 *    Param         : ID almid : alarm handler ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_stp_alm_impl( ID almid )
{
    ALMCB *almcb;
    ER ercd = E_OK;

    KNL_CHK_ALMID(almid)

    almcb = KNL_ALM_GET_CB(almid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( almcb->almhdr == NULL ) {               /* Unregistered handler */
        ercd = E_NOEXS;
    } else {
        if ( (almcb->almstat & TALM_STA) != 0u ) {
            /* Stop alarm handler address */
            knl_tck_delete(&almcb->almtmeb);
            almcb->almstat &= ~TALM_STA;
        }
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_STP_ALM */

/*
 * Debug support function
 */
#if TK_USE_DBGSPT

#ifdef KNL_CFG_USE_FUNC_TD_LST_ALM
/*
 *    Function Name : td_lst_alm_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer alarm handler object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
SYSCALL int32_t td_lst_alm_impl( ID list[], int32_t nent )
{
    ALMCB *almcb, *almcbend;
    int32_t n = 0;

    KNL_INT_BEGIN_DISABLE
    almcbend = knl_alm_cbtable + TK_NUM_ALMID;
    for ( almcb = knl_alm_cbtable; almcb < almcbend; almcb++ ) {
        /* Unregistered handler */
        if ( almcb->almhdr != NULL ) {
            if ( n < nent ) {
                *list = KNL_CFG_ID_ALM(almcb - knl_alm_cbtable);
                list++;
            }
            n++;
        }
    }
    KNL_INT_END_DISABLE

    return n;
}
#endif /* KNL_CFG_USE_FUNC_TD_LST_ALM */

#ifdef KNL_CFG_USE_FUNC_TD_REF_ALM
/*
 *    Function Name : td_ref_alm_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference alarm handler object state
 *    Param         : ID almid : Object ID
 *                    TD_RALM *pk_ralm : Address of status information packet
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_ref_alm_impl( ID almid, TD_RALM *pk_ralm )
{
    ALMCB *almcb;
    LSYSTIM ltime;
    ER ercd = E_OK;

    KNL_CHK_ALMID(almid)

    almcb = KNL_ALM_GET_CB(almid);

    KNL_INT_BEGIN_DISABLE
    if ( almcb->almhdr == NULL ) {               /* Unregistered handler */
        ercd = E_NOEXS;
    } else {
        if ( (almcb->almstat & TALM_STA) != 0u ) {
            ltime = almcb->almtmeb.time;
            ltime = ll_sub(ltime, knl_tck_current_time);
            ltime = ll_sub(ltime, uitoll(KNL_CFG_TIMER_PERIOD));
            if ( ll_sign(ltime) < 0 ) {
                ltime = ltoll(0);
            }
        } else {
            ltime = ltoll(0);
        }

        pk_ralm->exinf   = almcb->exinf;
        pk_ralm->lfttim  = lltoul(ltime);
        pk_ralm->almstat = almcb->almstat;
    }
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TD_REF_ALM */

#endif /* TK_USE_DBGSPT */

#endif /* KNL_CFG_MAX_ALMID */
