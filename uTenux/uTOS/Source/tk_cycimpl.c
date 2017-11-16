/*
 *----------------------------------------------------------------------------------------
 *    Kernel/cyc
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
 *    File Name      : tk_cycimpl.c
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Cyclic Management Function.
 *----------------------------------------------------------------------------------------
 */

#include "tk_cycimpl.h"

#if KNL_CFG_MAX_CYCID > 0

/*
 * Cyclic handler control block
 */
typedef struct cyclic_handler_control_block {
        VP                   exinf;              /* Extended information */
        ATR                  cycatr;             /* Cyclic handler attribute */
        FP                   cychdr;             /* Cyclic handler address */
        uint32_t             cycstat;            /* Cyclic handler state */
        RELTIM               cyctim;             /* Cyclic time */
        TMEB                 cyctmeb;            /* Timer event block */
#if TK_USE_OBJ_NAME                              /* name */
        uint8_t              name[TK_OBJ_NAME_LENGTH];    
#endif
}                            CYCCB;

/*
 * Cyclic handler control block table and free table pointer definition
 */
#ifdef KNL_CFG_USE_FUNC_KNL_CYC_CBTABLE
Zeroinit CYCCB knl_cyc_cbtable[TK_NUM_CYCID];    /* Cyclic handler control block */
Zeroinit QUEUE knl_cyc_freecb;                   /* FreeQue */
#define KNL_CYC_GET_CB(id)   ( &knl_cyc_cbtable[KNL_CFG_INDEX_CYC(id)] )
#endif /* KNL_CFG_USE_FUNC_KNL_CYC_CBTABLE */

/*
 * Declaration of local function prototype  
 */
static void knl_cyc_callhdr( CYCCB *cyccb );
static void knl_cyc_imm_callhdr( CYCCB *cyccb );

/*
 *    Description   : Next startup time
 *    Param         : CYCCB *cyccb : cyclic control block
 *    Return Code   : LSYSTIM : next time
 */
Inline LSYSTIM knl_cyc_next_time( CYCCB *cyccb )
{
    int64_t n;
    LSYSTIM ltime;

    ltime = ll_add(cyccb->cyctmeb.time, uitoll(cyccb->cyctim));

    if ( ll_cmp(ltime, knl_tck_current_time) <= 0 ) {

        /* Adjust time to be later than current time */
        ltime = ll_sub(knl_tck_current_time, cyccb->cyctmeb.time);
        n  = lui_div(ltime, cyccb->cyctim);
        ll_inc(&n);
        ltime = lui_mul(n, cyccb->cyctim);
        ltime = ll_add(cyccb->cyctmeb.time, ltime);
    }

    return ltime;
}

/*
 *    Description   : Register tick event queue
 *    Param         : CYCCB *cyccb : cyclic control block
 *                    LSYSTIM tm : cylic time
 *    Return Code   : void
 */
Inline void knl_cyc_tick_insert( CYCCB *cyccb, LSYSTIM tm )
{
    knl_tck_insabs(&cyccb->cyctmeb, tm, (CBACK)knl_cyc_callhdr, cyccb);
}

#ifdef KNL_CFG_USE_FUNC_KNL_CYC_CALLHDR
/*
 *    Function Name : knl_cyc_callhdr
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Call cyclic handler routine
 *    Param         : CYCCB *cyccb : cyclic control block
 *    Return Code   : void
 */
static void knl_cyc_callhdr( CYCCB *cyccb )
{
    /* Set next startup time */
    knl_cyc_tick_insert(cyccb, knl_cyc_next_time(cyccb));

    /* Execute cyclic handler / Enable interrupt nest */
    KNL_INT_ENABLE_UPTO(KNL_CFG_TIMER_INTLEVEL)
    (*(cyccb->cychdr))(cyccb->exinf);
    KNL_INT_DISABLE
}
#endif /* KNL_CFG_USE_FUNC_KNL_CYC_CALLHDR */

#ifdef KNL_CFG_USE_FUNC_TK_CRE_CYC
/*
 *    Function Name : knl_cyc_imm_callhdr
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Immediate call of cyclic handler
 *    Param         : CYCCB *cyccb : cyclic control block
 *    Return Code   : void
 */
static void knl_cyc_imm_callhdr( CYCCB *cyccb )
{
    /* Set next startup time */
    knl_cyc_tick_insert(cyccb, knl_cyc_next_time(cyccb));

    /* Execute cyclic handler in task-independent part
       (Keep interrupt disabled) */
    KNL_TSK_ENTER_IDP
    (*(cyccb->cychdr))(cyccb->exinf);
    KNL_TSK_LEAVE_IDP
}
#endif /* KNL_CFG_USE_FUNC_TK_CRE_CYC */

#ifdef KNL_CFG_USE_FUNC_KNL_CYC_GETNAME
#if TK_USE_OBJ_NAME
/*
 *    Function Name : knl_cyc_getname
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get object name from control block
 *    Param         : ID id : cyclic handler id
 *                    uint8_t **name : cyclic handler DS name pointer
 *    Return Code   : ER : Error code
 */
ER knl_cyc_getname(ID id, uint8_t **name)
{
    CYCCB *cyccb;
    ER ercd = E_OK;

    KNL_CHK_CYCID(id)

    KNL_INT_BEGIN_DISABLE
    cyccb = KNL_CYC_GET_CB(id);
    if ( cyccb->cychdr == NULL ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
    if ( (cyccb->cycatr & TA_DSNAME) == 0u ) {
        ercd = E_OBJ;
        goto error_exit;
    }
    *name = cyccb->name;

error_exit:
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* TK_USE_OBJ_NAME */
#endif /* KNL_CFG_USE_FUNC_KNL_CYC_GETNAME */

#ifdef KNL_CFG_USE_FUNC_KNL_CYC_INIT
/*
 *    Function Name : knl_cyc_init
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Initialization of cyclic handler control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
ER knl_cyc_init( void )
{
    CYCCB *cyccb, *cyccbend;

    /* Get system information */
    if ( TK_NUM_CYCID < 1 ) {
        return E_SYS;
    }

    /* Register all control blocks onto FreeQue */
    knl_que_init(&knl_cyc_freecb);
    cyccbend = knl_cyc_cbtable + TK_NUM_CYCID;
    for ( cyccb = knl_cyc_cbtable; cyccb < cyccbend; cyccb++ ) {
        cyccb->cychdr = NULL;                    /* Unregistered handler */
        knl_que_insert((QUEUE*)cyccb, &knl_cyc_freecb);
    }

    return E_OK;
}
#endif /* #define KNL_CFG_USE_FUNC_KNL_CYC_INIT */

#ifdef KNL_CFG_USE_FUNC_TK_CRE_CYC
/*
 *    Function Name : tk_cre_cyc_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Create cyclic handler
 *    Param         : T_CCYC *pk_ccyc : Address of cyclic handler definition packet
 *    Return Code   : ID : cyclic handler ID or Error code
 */
SYSCALL ID tk_cre_cyc_impl( T_CCYC *pk_ccyc )
{
#if KNL_CFG_CHK_RSATR
    const ATR VALID_CYCATR = {
         TA_HLNG
        |TA_STA
        |TA_PHS
#if TK_USE_OBJ_NAME
        |TA_DSNAME
#endif
    };
#endif
    CYCCB *cyccb;
    LSYSTIM ltime;
    ER ercd = E_OK;

    KNL_CHK_RSATR(pk_ccyc->cycatr, VALID_CYCATR)
    KNL_CHK_PAR(pk_ccyc->cychdr != NULL)
    KNL_CHK_PAR(pk_ccyc->cyctim > 0u)

    KNL_TSK_BEGIN_CRITICAL_SECTION
    /* Get control block from FreeQue */
    cyccb = (CYCCB*)knl_que_removenext(&knl_cyc_freecb);
    if ( cyccb == NULL ) {
        ercd = E_LIMIT;
        goto error_exit;
    }

    /* Initialize control block */
    cyccb->exinf   = pk_ccyc->exinf;
    cyccb->cycatr  = pk_ccyc->cycatr;
    cyccb->cychdr  = pk_ccyc->cychdr;
    cyccb->cyctim  = pk_ccyc->cyctim;
#if TK_USE_OBJ_NAME
    if ( (pk_ccyc->cycatr & TA_DSNAME) != 0u ) {
        strncpy((char*)&cyccb->name, (char*)&pk_ccyc->dsname, TK_OBJ_NAME_LENGTH);
    }
#endif

    /* First startup time
     *  To guarantee the start of handler after the specified time,
     *  add KNL_CFG_TIMER_PERIOD. 
     */
    ltime = ll_add(knl_tck_current_time, uitoll(pk_ccyc->cycphs));
    ltime = ll_add(ltime, uitoll(KNL_CFG_TIMER_PERIOD));

    if ( (pk_ccyc->cycatr & TA_STA) != 0u ) {
        /* Start cyclic handler */
        cyccb->cycstat = TCYC_STA;

        if ( pk_ccyc->cycphs == 0u ) {
            /* Immediate execution */
            cyccb->cyctmeb.time = ltime;
            knl_cyc_imm_callhdr(cyccb);
        } else {
            /* Register onto timer event queue */
            knl_cyc_tick_insert(cyccb, ltime);
        }
    } else {
        /* Initialize only counter */
        cyccb->cycstat = TCYC_STP;
        cyccb->cyctmeb.time = ltime;
    }

    ercd = KNL_CFG_ID_CYC(cyccb - knl_cyc_cbtable);

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_CRE_CYC */

#ifdef KNL_CFG_USE_FUNC_TK_DEL_CYC
/*
 *    Function Name : tk_del_cyc_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Delete cyclic handler
 *    Param         : ID cycid : cyclic handler ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_del_cyc_impl( ID cycid )
{
    CYCCB *cyccb;
    ER ercd = E_OK;

    KNL_CHK_CYCID(cycid)

    cyccb = KNL_CYC_GET_CB(cycid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( cyccb->cychdr == NULL ) { /* Unregistered handler */
        ercd = E_NOEXS;
    } else {
        if ( (cyccb->cycstat & TCYC_STA) != 0u ) {
            /* Delete timer event queue */
            knl_tck_delete(&cyccb->cyctmeb);
        }

        /* Return to FreeQue */
        knl_que_insert((QUEUE*)cyccb, &knl_cyc_freecb);
        cyccb->cychdr = NULL; /* Unregistered handler */
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_DEL_CYC */

#ifdef KNL_CFG_USE_FUNC_TK_REF_CYC
/*
 *    Function Name : tk_ref_cyc_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer cyclic handler state
 *    Param         : ID cycid : cyclic handle ID
 *                    T_RCYC* pk_rcyc : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_ref_cyc_impl( ID cycid, T_RCYC* pk_rcyc )
{
    CYCCB *cyccb;
    LSYSTIM ltime;
    ER ercd = E_OK;

    KNL_CHK_CYCID(cycid)

    cyccb = KNL_CYC_GET_CB(cycid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( cyccb->cychdr == NULL ) {               /* Unregistered handler */
        ercd = E_NOEXS;
    } else {
        ltime = cyccb->cyctmeb.time;
        if ( (cyccb->cycstat & TCYC_STA) == 0u ) {
            if ( ll_cmp(ltime, knl_tck_current_time) <= 0 ) {
                ltime = knl_cyc_next_time(cyccb);
            }
        }
        ltime = ll_sub(ltime, knl_tck_current_time);
        ltime = ll_sub(ltime, uitoll(KNL_CFG_TIMER_PERIOD));
        if ( ll_sign(ltime) < 0 ) {
            ltime = ltoll(0);
        }

        pk_rcyc->exinf   = cyccb->exinf;
        pk_rcyc->lfttim  = lltoul(ltime);
        pk_rcyc->cycstat = cyccb->cycstat;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_REF_CYC */

#ifdef KNL_CFG_USE_FUNC_TK_STA_CYC
/*
 *    Function Name : tk_sta_cyc_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Start cyclic handler
 *    Param         : ID cycid : cyclic handler ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_sta_cyc_impl( ID cycid )
{
    CYCCB *cyccb;
    LSYSTIM ltime;
    ER ercd = E_OK;

    KNL_CHK_CYCID(cycid)

    cyccb = KNL_CYC_GET_CB(cycid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( cyccb->cychdr == NULL ) {               /* Unregistered handler */
        ercd = E_NOEXS;
        goto error_exit;
    }

    if ( (cyccb->cycatr & TA_PHS) != 0u ) {
        /* Continue cyclic phase */
        if ( (cyccb->cycstat & TCYC_STA) == 0u ) {
            /* Start cyclic handler */
            ltime = cyccb->cyctmeb.time;
            if ( ll_cmp(ltime, knl_tck_current_time) <= 0 ) {
                ltime = knl_cyc_next_time(cyccb);
            }
            knl_cyc_tick_insert(cyccb, ltime);
        }
    } else {
        /* Reset cyclic interval */
        if ( (cyccb->cycstat & TCYC_STA) != 0u ) {
            /* Stop once */
            knl_tck_delete(&cyccb->cyctmeb);
        }

        /* FIRST ACTIVATION TIME
         *  Adjust the first activation time with KNL_CFG_TIMER_PERIOD.
         *  KNL_CFG_TIMER_PERIOD is Timer interrupt interval (millisecond).
         */
        ltime = ll_add(knl_tck_current_time, uitoll(cyccb->cyctim));
        ltime = ll_add(ltime, uitoll(KNL_CFG_TIMER_PERIOD));

        /* Start cyclic handler */
        knl_cyc_tick_insert(cyccb, ltime);
    }
    cyccb->cycstat |= TCYC_STA;

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_STA_CYC */

#ifdef KNL_CFG_USE_FUNC_TK_STP_CYC
/*
 *    Function Name : tk_stp_cyc_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Stop cyclic handler
 *    Param         : ID cycid : cyclic handler ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_stp_cyc_impl( ID cycid )
{
    CYCCB *cyccb;
    ER ercd = E_OK;

    KNL_CHK_CYCID(cycid)

    cyccb = KNL_CYC_GET_CB(cycid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( cyccb->cychdr == NULL ) {               /* Unregistered handler */
        ercd = E_NOEXS;
    } else {
        if ( (cyccb->cycstat & TCYC_STA) != 0u ) {
            /* Stop cyclic handler */
            knl_tck_delete(&cyccb->cyctmeb);
        }
        cyccb->cycstat &= ~TCYC_STA;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_STP_CYC */

/*
 * Debug support function
 */
#if TK_USE_DBGSPT

#ifdef KNL_CFG_USE_FUNC_TD_LST_CYC
/*
 *    Function Name : td_lst_cyc_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer cyclic handler object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
SYSCALL int32_t td_lst_cyc_impl( ID list[], int32_t nent )
{
    CYCCB *cyccb, *cyccbend;
    int32_t n = 0;

    KNL_INT_BEGIN_DISABLE
    cyccbend = knl_cyc_cbtable + TK_NUM_CYCID;
    for ( cyccb = knl_cyc_cbtable; cyccb < cyccbend; cyccb++ ) {
        /* Unregistered handler */
        if ( cyccb->cychdr != NULL ) {
            if ( n < nent ) {
                *list = KNL_CFG_ID_CYC(cyccb - knl_cyc_cbtable);
                list++;
            }
            n++;
        }
    }
    KNL_INT_END_DISABLE

    return n;
}
#endif /* KNL_CFG_USE_FUNC_TD_LST_CYC */

#ifdef KNL_CFG_USE_FUNC_TD_REF_CYC
/*
 *    Function Name : td_ref_cyc_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference cyclic handler object state
 *    Param         : ID cycid : Object ID
 *                    TD_RCYC* pk_rcyc : Address of status information packet
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_ref_cyc_impl( ID cycid, TD_RCYC* pk_rcyc )
{
    CYCCB *cyccb;
    LSYSTIM ltime;
    ER ercd = E_OK;

    KNL_CHK_CYCID(cycid)

    cyccb = KNL_CYC_GET_CB(cycid);

    KNL_INT_BEGIN_DISABLE
    if ( cyccb->cychdr == NULL ) {               /* Unregistered handler */
        ercd = E_NOEXS;
    } else {
        ltime = cyccb->cyctmeb.time;
        if ( (cyccb->cycstat & TCYC_STA) == 0u ) {
            if ( ll_cmp(ltime, knl_tck_current_time) <= 0 ) {
                ltime = knl_cyc_next_time(cyccb);
            }
        }
        ltime = ll_sub(ltime, knl_tck_current_time);
        ltime = ll_sub(ltime, uitoll(KNL_CFG_TIMER_PERIOD));
        if ( ll_sign(ltime) < 0 ) {
            ltime = ltoll(0);
        }

        pk_rcyc->exinf   = cyccb->exinf;
        pk_rcyc->lfttim  = lltoul(ltime);
        pk_rcyc->cycstat = cyccb->cycstat;
    }
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TD_REF_CYC */

#endif /* TK_USE_DBGSPT */

#endif /* KNL_CFG_MAX_CYCID */

