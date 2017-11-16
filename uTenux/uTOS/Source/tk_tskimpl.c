/*
 *----------------------------------------------------------------------------------------
 *    Kernel/tsk
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
 *    File Name      : tk_tskimpl.c
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Task Management and  Synchronize Function.
 *----------------------------------------------------------------------------------------
 */

#include "tk_tskimpl.h"
#include "knl_ctx.h"
#include "tk_mtximpl.h"

#ifdef KNL_CFG_USE_FUNC_TK_DLY_TSK
/*
 * Definition of task delay wait specification
 */
static WSPEC knl_tsk_wspec_dly    = { TTW_DLY, NULL, NULL };
#endif /* KNL_CFG_USE_FUNC_TK_DLY_TSK */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_WSPEC_SLP
/*
 * Definition of task sleep wait specification
 */
static WSPEC knl_tsk_wspec_slp    = { TTW_SLP, NULL, NULL };
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_WSPEC_SLP */

#ifdef KNL_CFG_USE_FUNC_TK_CAN_WUP
/*
 *    Function Name : tk_can_wup_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Cancel Wakeup Task
 *    Param         : ID tskid : Task ID
 *    Return Code   : int32_t : Number of queued wakeup requests or Error Code
 */
SYSCALL int32_t tk_can_wup_impl( ID tskid )
{
    TCB  *tcb;
    ER  ercd = E_OK;

    KNL_CHK_TSKID_SELF(tskid)

    tcb = KNL_TSK_GET_CB_SELF(tskid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    switch ( tcb->state ) {
        case TS_NONEXIST:
            ercd = E_NOEXS;
            break;
        case TS_DORMANT:
            ercd = E_OBJ;
            break;
        default:
            ercd = tcb->wupcnt;
            tcb->wupcnt = 0;
            break;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_CAN_WUP */

#ifdef KNL_CFG_USE_FUNC_TK_CHG_PRI
/*
 *    Function Name : tk_chg_pri_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Change Task Priority
 *    Param         : ID tskid : Task ID
 *                    PRI tskpri : Task priority
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_chg_pri_impl( ID tskid, PRI tskpri )
{
    TCB *tcb;
    PRI priority;
    ER  ercd;

    KNL_CHK_TSKID_SELF(tskid)
    KNL_CHK_PRI_INI(tskpri)

    tcb = KNL_TSK_GET_CB_SELF(tskid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( tcb->state == TS_NONEXIST ) {
        ercd = E_NOEXS;
        goto error_exit;
    }

    /* Conversion priority to internal expression */
    if ( tskpri == TPRI_INI ) {
        priority = tcb->ipriority;
    } else {
        priority = KNL_TSK_ITN_PRI(tskpri);
    }

#if KNL_CFG_MAX_MTXID > 0
    /* Mutex priority change limit */
    ercd = knl_mtx_limpri(tcb, priority);
    if ( ercd < E_OK ) {
        goto error_exit;
    }

    tcb->bpriority = priority;
    priority = (PRI)ercd;
#else
    tcb->bpriority = priority;
#endif

    /* Change priority */
    knl_tsk_chgpri(tcb, priority);

    ercd = E_OK;

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_CHG_PRI */

#ifdef KNL_CFG_USE_FUNC_TK_CRE_TSK
/*
 *    Function Name : tk_cre_tsk_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Create Task
 *    Param         : VT_CTSK *pk_ctsk: Information about the task to be created
 *    Return Code   : ID : Task ID or Error Code
 */
SYSCALL ID tk_cre_tsk_impl( T_CTSK *pk_ctsk )
{
#if KNL_CFG_CHK_RSATR
    const ATR VALID_TSKATR = {                   /* Valid value of task attribute */
         TA_HLNG
        |TA_RNG3
        |TA_USERBUF
#if TK_USE_OBJ_NAME
        |TA_DSNAME
#endif
    };
#endif
    int32_t  sstksz;
    VP  stack;
    ER  ercd;
    TCB  *tcb;
    
    KNL_CHK_RSATR(pk_ctsk->tskatr, VALID_TSKATR)
#if !TK_USE_MALLOC
    /* TA_USERBUF must be specified if configured in no Imalloc */
    KNL_CHK_PAR((pk_ctsk->tskatr & TA_USERBUF) != 0)
#endif
    KNL_CHK_PRI(pk_ctsk->itskpri)
    KNL_CHK_PAR(pk_ctsk->stksz >= KNL_CTX_MIN_TASK_STACK_SIZE)

    /* Adjust stack size by 8 bytes */
    sstksz  = (pk_ctsk->stksz  + 7) / 8 * 8;

#if TK_USE_MALLOC
    if ( (pk_ctsk->tskatr & TA_USERBUF) != 0u ) {
        /* Size of User buffer must be multiples of 8 */
        if ( sstksz != pk_ctsk->stksz ) {
            return E_PAR;
        }
        /* Use user buffer */
        stack = pk_ctsk->bufptr;
    } else {
        /* Allocate system stack area */
        stack = knl_mem_imalloc((uint32_t)sstksz);
        if ( stack == NULL ) {
            return E_NOMEM;
        }
    }
#else
    /* Size of User buffer must be multiples of 8 */
    if ( sstksz != pk_ctsk->stksz ) {
        return E_PAR;
    }
    /* Use user buffer */
    stack = pk_ctsk->bufptr;
#endif

    KNL_TSK_BEGIN_CRITICAL_SECTION
    /* Get control block from FreeQue */
    tcb = (TCB*)knl_que_removenext(&knl_tsk_freecb);
    if ( tcb == NULL ) {
        ercd = E_LIMIT;
        goto error_exit;
    }

    /* Initialize control block */
    tcb->exinf     = pk_ctsk->exinf;
    tcb->tskatr    = pk_ctsk->tskatr & ~TA_RNG3; /* handling as TA_RNG0 */
    tcb->task      = pk_ctsk->task;
    tcb->ipriority = KNL_TSK_ITN_PRI(pk_ctsk->itskpri);
    tcb->sstksz    = sstksz;
#if TK_USE_OBJ_NAME
    if ( (pk_ctsk->tskatr & TA_DSNAME) != 0u ) {
        strncpy((char*)&tcb->name, (char*)&pk_ctsk->dsname, TK_OBJ_NAME_LENGTH);
    }
#endif

    /* Set stack pointer */
    tcb->isstack = (uint8_t*)stack + sstksz;

    /* Set initial value of task operation mode */
    tcb->isysmode = 1;
    tcb->sysmode  = 1;

    /* make it to DORMANT state */
    knl_tsk_makdor(tcb);

    ercd = tcb->tskid;

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

#if TK_USE_MALLOC
    if ( (ercd < E_OK) && ((pk_ctsk->tskatr & TA_USERBUF) == 0u) ) {
        knl_mem_ifree(stack);
    }
#endif

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_CRE_TSK */

#ifdef KNL_CFG_USE_FUNC_TK_DEL_TSK
/*
 *    Function Name : tk_del_tsk_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Delete Task
 *    Param         : ID tskid : Task I
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_del_tsk_impl( ID tskid )
{
    TCB *tcb;
    TSTAT  state;
    ER  ercd = E_OK;

    KNL_CHK_TSKID(tskid)
    KNL_CHK_NONSELF(tskid)

    tcb = KNL_TSK_GET_CB(tskid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    state = (TSTAT)tcb->state;
    if ( state != TS_DORMANT ) {
        if ( state == TS_NONEXIST ) {
            ercd = E_NOEXS;
        } else {
            ercd = E_OBJ;
        }
    } else {
        knl_tsk_del(tcb);
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_DEL_TSK */

#ifdef KNL_CFG_USE_FUNC_TK_DLY_TSK
/*
 *    Function Name : tk_dly_tsk_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Delay Task
 *    Param         : RELTIM dlytim : Delay time
 *    Return Code   : ER : c
 */
SYSCALL ER tk_dly_tsk_impl( RELTIM dlytim )
{
    ER  ercd = E_OK;

    KNL_CHK_DISPATCH()

    if ( dlytim > 0u ) {
        KNL_TSK_BEGIN_CRITICAL_SECTION
        knl_tsk_execution->wspec = &knl_tsk_wspec_dly;
        knl_tsk_execution->wid = 0;
        knl_tsk_execution->wercd = &ercd;
        knl_tsk_makwaireltim(dlytim, TA_NULL);
        knl_que_init(&knl_tsk_execution->tskque);
        KNL_TSK_END_CRITICAL_SECTION
    }

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_DLY_TSK */

#ifdef KNL_CFG_USE_FUNC_TK_EXD_TSK
/*
 *    Function Name : tk_exd_tsk_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Exit and Delete Task
 *    Param         : void
 *    Return Code   : void
 */
SYSCALL void tk_exd_tsk_impl( void )
{
    /* Check context error */
#if KNL_CFG_CHK_ININDP
    if ( KNL_TSK_IN_INDP() ) {
#if TK_USE_MESSAGE
        tm_putstring((uint8_t*)&"tk_exd_tsk was called in the task independent\n");
#endif
        tm_monitor();                            /* To monitor */
    }
#endif
#if KNL_CFG_CHK_DISDSP
    if ( KNL_TSK_IN_DDSP() ) {
#if TK_USE_MESSAGE
        tm_putstring((uint8_t*)&"tk_exd_tsk was called in the dispatch disabled\n");
#endif
    }
#endif

    KNL_INT_DISABLE
    knl_tsk_ter(knl_tsk_execution);
    knl_tsk_del(knl_tsk_execution);

    knl_dsp_force();
    /* No return */
}
#endif /* KNL_CFG_USE_FUNC_TK_EXD_TSK */

#ifdef KNL_CFG_USE_FUNC_TK_EXT_TSK
/*
 *    Function Name : tk_ext_tsk_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Exit Task
 *    Param         : void
 *    Return Code   : void
 */
SYSCALL void tk_ext_tsk_impl( void )
{
#ifdef KNL_CTX_DORMANT_STACK_SIZE
    /* To avoid destroying stack used in 'knl_tsk_makdor',
       allocate the dummy area on the stack. */
    volatile uint8_t _dummy[KNL_CTX_DORMANT_STACK_SIZE];
#endif

    /* Check context error */
#if KNL_CFG_CHK_ININDP
    if ( KNL_TSK_IN_INDP() ) {
#if TK_USE_MESSAGE
        tm_putstring((uint8_t*)&"tk_ext_tsk was called in the task independent\n");
#endif
        tm_monitor();
    }
#endif
#if KNL_CFG_CHK_DISDSP
    if ( KNL_TSK_IN_DDSP() ) {
#if TK_USE_MESSAGE
        tm_putstring((uint8_t*)&"tk_ext_tsk was called in the dispatch disabled\n");
#endif
    }
#endif

    KNL_INT_DISABLE
    knl_tsk_ter(knl_tsk_execution);
    knl_tsk_makdor(knl_tsk_execution);

    knl_dsp_force();
    /* No return */

#ifdef KNL_CTX_DORMANT_STACK_SIZE
    /* for WARNING */
    _dummy[0] = _dummy[0];
#endif
}
#endif /* KNL_CFG_USE_FUNC_TK_EXT_TSK */

#ifdef KNL_CFG_USE_FUNC_TK_FRSM_TSK
/*
 *    Function Name : tk_frsm_tsk_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Resume Task Force
 *    Param         : ID tskid : Task ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_frsm_tsk_impl( ID tskid )
{
    TCB  *tcb;
    ER  ercd = E_OK;

    KNL_CHK_TSKID(tskid)
    KNL_CHK_NONSELF(tskid)

    tcb = KNL_TSK_GET_CB(tskid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    switch ( tcb->state ) {
        case TS_NONEXIST:
            ercd = E_NOEXS;
            break;
        case TS_DORMANT:
            ercd = E_OBJ;
            break;
        case TS_READY:
            ercd = E_OBJ;
            break;
        case TS_WAIT:
            ercd = E_OBJ;
            break;
        case TS_SUSPEND:
            tcb->suscnt = 0;
            knl_tsk_makrdy(tcb);
            break;
        case TS_WAITSUS:
            tcb->suscnt = 0;
            tcb->state = TS_WAIT;
            break;
        default:
            ercd = E_SYS;
            break;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_FRSM_TSK */

#ifdef KNL_CFG_USE_FUNC_TK_GET_REG
/*
 *    Function Name : tk_get_reg_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get Task Registers
 *    Param         : ID tskid : Task ID
 *                    T_REGS *pk_regs : General registers
 *                    T_EIT *pk_eit : Registers saved when EIT occurs
 *                    T_CREGS *pk_cregs : Control registers
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_get_reg_impl( ID tskid, T_REGS *pk_regs, T_EIT *pk_eit, T_CREGS *pk_cregs )
{
    TCB *tcb;
    ER  ercd = E_OK;

    KNL_CHK_INTSK()
    KNL_CHK_TSKID(tskid)
    KNL_CHK_NONSELF(tskid)

    tcb = KNL_TSK_GET_CB(tskid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( tcb->state == TS_NONEXIST ) {
        ercd = E_NOEXS;
    } else {
        knl_ctx_getreg(tcb, pk_regs, pk_eit, pk_cregs);
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_GET_REG */

#ifdef KNL_CFG_USE_FUNC_TK_REF_TSK
/*
 *    Function Name : tk_ref_tsk_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference Task Status
 *    Param         : ID tskid : Task ID
 *                    T_RTSK *pk_rtsk : Address of packet for returning task status
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_ref_tsk_impl( ID tskid, T_RTSK *pk_rtsk )
{
    TCB *tcb;
    TSTAT state;
    ER ercd = E_OK;

    KNL_CHK_TSKID_SELF(tskid)

    tcb = KNL_TSK_GET_CB_SELF(tskid);

    memset(pk_rtsk, 0, sizeof(*pk_rtsk));

    KNL_TSK_BEGIN_CRITICAL_SECTION
    state = (TSTAT)tcb->state;
    if ( state == TS_NONEXIST ) {
        ercd = E_NOEXS;
    } else {
        if ( ( state == TS_READY ) && ( tcb == knl_tsk_execution ) ) {
            pk_rtsk->tskstat = TTS_RUN;
        } else {
            pk_rtsk->tskstat = (uint32_t)state << 1;
        }
        if ( (state & TS_WAIT) != 0u ) {
            pk_rtsk->tskwait = tcb->wspec->tskwait;
            pk_rtsk->wid     = tcb->wid;
        }
        pk_rtsk->exinf     = tcb->exinf;
        pk_rtsk->tskpri    = KNL_TSK_ETN_PRI(tcb->priority);
        pk_rtsk->tskbpri   = KNL_TSK_ETN_PRI(tcb->bpriority);
        pk_rtsk->wupcnt    = tcb->wupcnt;
        pk_rtsk->suscnt    = tcb->suscnt;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_REF_TSK */

#ifdef KNL_CFG_USE_FUNC_TK_REL_WAI
/*
 *    Function Name : tk_rel_wai_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Release Wait
 *    Param         : ID tskid : Task ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_rel_wai_impl( ID tskid )
{
    TCB  *tcb;
    TSTAT  state;
    ER  ercd = E_OK;

    KNL_CHK_TSKID(tskid)

    tcb = KNL_TSK_GET_CB(tskid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    state = (TSTAT)tcb->state;
    if ( (state & TS_WAIT) == 0u ) {
        if ( state == TS_NONEXIST ) {
            ercd = E_NOEXS;
        } else {
            ercd = E_OBJ;
        }
    } else {
        knl_tsk_wairelng(tcb, E_RLWAI);
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_REL_WAI */

#ifdef KNL_CFG_USE_FUNC_TK_RSM_TSK
/*
 *    Function Name : tk_rsm_tsk_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Resume Task
 *    Param         : ID tskid : Task ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_rsm_tsk_impl( ID tskid )
{
    TCB  *tcb;
    ER  ercd = E_OK;

    KNL_CHK_TSKID(tskid)
    KNL_CHK_NONSELF(tskid)

    tcb = KNL_TSK_GET_CB(tskid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    switch ( tcb->state ) {
        case TS_NONEXIST:
            ercd = E_NOEXS;
            break;
        case TS_DORMANT:
            ercd = E_OBJ;
            break;
        case TS_READY:
            ercd = E_OBJ;
            break;
        case TS_WAIT:
            ercd = E_OBJ;
            break;
        case TS_SUSPEND:
            --tcb;
            if ( tcb->suscnt == 0 ) {
                knl_tsk_makrdy(tcb);
            }
            break;
        case TS_WAITSUS:
            --tcb;
            if ( tcb->suscnt == 0 ) {
                tcb->state = TS_WAIT;
            }
            break;
        default:
            ercd = E_SYS;
            break;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_RSM_TSK */

#ifdef KNL_CFG_USE_FUNC_TK_SET_REG
/*
 *    Function Name : tk_set_reg_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Set Task Registers
 *    Param         : ID tskid : Task ID
 *                    T_REGS *pk_regs : General registers
 *                    T_EIT *pk_eit : Registers saved when EIT occurs
 *                    T_CREGS *pk_cregs : Control registers
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_set_reg_impl( ID tskid, T_REGS *pk_regs, T_EIT *pk_eit, T_CREGS *pk_cregs )
{
    TCB *tcb;
    ER  ercd = E_OK;

    KNL_CHK_INTSK()
    KNL_CHK_TSKID(tskid)
    KNL_CHK_NONSELF(tskid)

    tcb = KNL_TSK_GET_CB(tskid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( tcb->state == TS_NONEXIST ) {
        ercd = E_NOEXS;
    } else {
        knl_ctx_setreg(tcb, pk_regs, pk_eit, pk_cregs);
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_SET_REG */

#ifdef KNL_CFG_USE_FUNC_TK_SLP_TSK
/*
 *    Function Name : tk_slp_tsk_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Sleep Task,Move its own task state to wait state
 *    Param         : TMO tmout : Timeout
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_slp_tsk_impl( TMO tmout )
{
    ER  ercd = E_OK;

    KNL_CHK_TMOUT(tmout)
    KNL_CHK_DISPATCH()

    KNL_TSK_BEGIN_CRITICAL_SECTION

    if ( knl_tsk_execution->wupcnt > 0 ) {
        knl_tsk_execution->wupcnt--;
    } else {
        ercd = E_TMOUT;
        if ( tmout != TMO_POL ) {
            knl_tsk_execution->wspec = &knl_tsk_wspec_slp;
            knl_tsk_execution->wid = 0;
            knl_tsk_execution->wercd = &ercd;
            knl_tsk_makwai(tmout, TA_NULL);
            knl_que_init(&knl_tsk_execution->tskque);
        }
    }

    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_SLP_TSK */

#ifdef KNL_CFG_USE_FUNC_TK_STA_TSK
/*
 *    Function Name : tk_sta_tsk_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Start Task
 *    Param         : ID tskid : Task ID
 *                    int32_t stacd : Task start code
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_sta_tsk_impl( ID tskid, int32_t stacd )
{
    TCB *tcb;
    TSTAT  state;
    ER  ercd = E_OK;

    KNL_CHK_TSKID(tskid)
    KNL_CHK_NONSELF(tskid)

    tcb = KNL_TSK_GET_CB(tskid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    state = (TSTAT)tcb->state;
    if ( state != TS_DORMANT ) {
        if ( state == TS_NONEXIST ) {
            ercd = E_NOEXS;
        } else {
            ercd = E_OBJ;
        }
    } else {
        knl_ctx_setstacd(tcb, stacd);
        knl_tsk_makrdy(tcb);
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_STA_TSK */

#ifdef KNL_CFG_USE_FUNC_TK_SUS_TSK
/*
 *    Function Name : tk_sus_tsk_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Suspend Task
 *    Param         : ID tskid : Task ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_sus_tsk_impl( ID tskid )
{
    TCB  *tcb;
    TSTAT  state;
    ER  ercd = E_OK;

    KNL_CHK_TSKID(tskid)
    KNL_CHK_NONSELF(tskid)

    tcb = KNL_TSK_GET_CB(tskid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    state = (TSTAT)tcb->state;
    if ( !knl_tsk_alive(state) ) {
        if ( state == TS_NONEXIST ) {
            ercd = E_NOEXS;
        } else {
            ercd = E_OBJ;
        }
        goto error_exit;
    }
    if ( ( tcb == knl_tsk_execution ) && ( knl_tsk_dspdisabled >= KNL_TSK_DDS_DISABLE ) ){
        ercd = E_CTX;
        goto error_exit;
    }
    if ( tcb->suscnt == INT32_MAX ) {
        ercd = E_QOVR;
        goto error_exit;
    }

    /* Update suspend request count */
    ++tcb->suscnt;

    /* Move to forced wait state */
    if ( state == TS_READY ) {
        knl_tsk_maknordy(tcb);
        tcb->state = TS_SUSPEND;

    } else if ( state == TS_WAIT ) {
        tcb->state = TS_WAITSUS;
    }

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_SUS_TSK */

#ifdef KNL_CFG_USE_FUNC_TK_TER_TSK
/*
 *    Function Name : tk_ter_tsk_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Terminate Task
 *    Param         : ID tskid : Task ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_ter_tsk_impl( ID tskid )
{
    TCB *tcb;
    TSTAT  state;
    ER  ercd = E_OK;

    KNL_CHK_TSKID(tskid)
    KNL_CHK_NONSELF(tskid)

    tcb = KNL_TSK_GET_CB(tskid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    state = (TSTAT)tcb->state;
    if ( !knl_tsk_alive(state) ) {
        if ( state == TS_NONEXIST ) {
            ercd = E_NOEXS;
        } else {
            ercd = E_OBJ;
        }
    } else if ( tcb->klocked ) {
        /* Normally, it does not become this state.
         * When the state is page-in wait in the virtual memory
         * system and when trying to terminate any task,
         * it becomes this state.
         */
        ercd = E_OBJ;
    } else {
        knl_tsk_ter(tcb);
        knl_tsk_makdor(tcb);
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_TER_TSK */

#ifdef KNL_CFG_USE_FUNC_TK_WUP_TSK
/*
 *    Function Name : tk_wup_tsk_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Wakeup Task
 *    Param         : ID tskid : Task ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_wup_tsk_impl( ID tskid )
{
    TCB  *tcb;
    TSTAT  state;
    ER  ercd = E_OK;

    KNL_CHK_TSKID(tskid)
    KNL_CHK_NONSELF(tskid)

    tcb = KNL_TSK_GET_CB(tskid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    state = (TSTAT)tcb->state;
    if ( !knl_tsk_alive(state) ) {
        if ( state == TS_NONEXIST ) {
            ercd = E_NOEXS;
        } else {
            ercd = E_OBJ;
        }
    } else if ( ( (state & TS_WAIT) != 0u ) && ( tcb->wspec == &knl_tsk_wspec_slp ) ) {
        knl_tsk_wairelok(tcb);
    } else if ( tcb->wupcnt == INT32_MAX ) {
        ercd = E_QOVR;
    } else {
        ++tcb->wupcnt;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_WUP_TSK */

/*
 *  Debug support function
 */
#if TK_USE_DBGSPT

#ifdef KNL_CFG_USE_FUNC_TD_GET_REG
/*
 *    Function Name : td_get_reg_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get Task Register
 *    Param         : ID tskid : Task ID (TSK_SELF cannot be designated
 *                    T_REGS *regs : General registers
 *                    T_EIT *eit : Registers saved when exception is raised
 *                    T_CREGS *cregs : Control registers
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_get_reg_impl( ID tskid, T_REGS *regs, T_EIT *eit, T_CREGS *cregs )
{
    TCB  *tcb;
    ER  ercd = E_OK;

    KNL_CHK_TSKID(tskid)

    tcb = KNL_TSK_GET_CB(tskid);
    if ( tcb == knl_tsk_execution ) {
        return E_OBJ;
    }

    KNL_INT_BEGIN_DISABLE
    if ( tcb->state == TS_NONEXIST ) {
        ercd = E_NOEXS;
    } else {
        knl_ctx_getreg(tcb, regs, eit, cregs);
    }
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TD_GET_REG */

#ifdef KNL_CFG_USE_FUNC_TD_INF_TSK
/*
 *    Function Name : td_inf_tsk_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference Task Statistics
 *    Param         : ID tskid : Task ID (TSK_SELF can be designated)
 *                    TD_ITSK *pk_itsk : Address of packet for returning task statistics
 *                    bool8_t clr : Task statistics clear flag
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_inf_tsk_impl( ID tskid, TD_ITSK *pk_itsk, bool8_t clr )
{
    TCB  *tcb;
    ER  ercd = E_OK;

    KNL_CHK_TSKID_SELF(tskid)

    tcb = KNL_TSK_GET_CB_SELF(tskid);

    KNL_INT_BEGIN_DISABLE
    if ( tcb->state == TS_NONEXIST ) {
        ercd = E_NOEXS;
    } else {
        pk_itsk->stime = tcb->stime;
        pk_itsk->utime = tcb->utime;
        if ( clr ) {
            tcb->stime = 0u;
            tcb->utime = 0u;
        }
    }
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TD_INF_TSK */

#ifdef KNL_CFG_USE_FUNC_TD_LST_TSK
/*
 *    Function Name : td_lst_tsk_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference Object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error Code
 */
SYSCALL int32_t td_lst_tsk_impl( ID list[], int32_t nent )
{
    int32_t  n = 0;
    TCB  *tcb, *tskcbend;

    KNL_INT_BEGIN_DISABLE
    tskcbend = knl_tsk_cbtable + TK_NUM_TSKID;
    for ( tcb = knl_tsk_cbtable; tcb < tskcbend; tcb++ ) {
        if ( tcb->state != TS_NONEXIST ) {
            if ( n < nent ) {
                *list = tcb->tskid;
                list++;
            }
            n++;
        }
    }
    KNL_INT_END_DISABLE

    return n;
}
#endif /* KNL_CFG_USE_FUNC_TD_LST_TSK */

#ifdef KNL_CFG_USE_FUNC_TD_RDY_QUE
/*
 *    Function Name : td_rdy_que_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get Task Precedence
 *    Param         : PRI pri : Task priority
 *                    ID list[] : Location of task ID list
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of priority pri tasks in a run state or Error Code
 */
SYSCALL int32_t td_rdy_que_impl( PRI pri, ID list[], int32_t nent )
{
    int32_t  n = 0;
    QUEUE  *q, *tskque;

    KNL_CHK_PRI(pri)

    KNL_INT_BEGIN_DISABLE
    tskque = &knl_tsk_rdyque.tskque[KNL_TSK_ITN_PRI(pri)];
    for ( q = tskque->next; q != tskque; q = q->next ) {
        if ( n < nent ) {
            *list = ((TCB*)q)->tskid;
            list++;
        }
        n++;
    }
    KNL_INT_END_DISABLE

    return n;
}
#endif /* KNL_CFG_USE_FUNC_TD_RDY_QUE */

#ifdef KNL_CFG_USE_FUNC_TD_REF_TSK
/*
 *    Function Name : td_ref_tsk_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference Task State
 *    Param         : ID tskid : Task ID (TSK_SELF can be specified)
 *                    TD_RTSK *pk_rtsk : Address of Packet for returning the task state
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_ref_tsk_impl( ID tskid, TD_RTSK *pk_rtsk )
{
    TCB  *tcb;
    TSTAT  state;
    ER  ercd = E_OK;

    KNL_CHK_TSKID_SELF(tskid)

    tcb = KNL_TSK_GET_CB_SELF(tskid);

    memset(pk_rtsk, 0, sizeof(*pk_rtsk));

    KNL_INT_BEGIN_DISABLE
    state = (TSTAT)tcb->state;
    if ( state == TS_NONEXIST ) {
        ercd = E_NOEXS;
    } else {
        if ( ( state == TS_READY ) && ( tcb == knl_tsk_execution ) ) {
            pk_rtsk->tskstat = TTS_RUN;
        } else {
            pk_rtsk->tskstat = (uint32_t)state << 1;
        }
        if ( (state & TS_WAIT) != 0u ) {
            pk_rtsk->tskwait = tcb->wspec->tskwait;
            pk_rtsk->wid     = tcb->wid;
        }
        pk_rtsk->exinf     = tcb->exinf;
        pk_rtsk->tskpri    = KNL_TSK_ETN_PRI(tcb->priority);
        pk_rtsk->tskbpri   = KNL_TSK_ETN_PRI(tcb->bpriority);
        pk_rtsk->wupcnt    = tcb->wupcnt;
        pk_rtsk->suscnt    = tcb->suscnt;

        pk_rtsk->task      = tcb->task;
        pk_rtsk->stksz     = tcb->sstksz;
        pk_rtsk->istack    = tcb->isstack;
    }
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TD_REF_TSK */

#ifdef KNL_CFG_USE_FUNC_TD_SET_REG
/*
 *    Function Name : td_set_reg_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Set Task Register
 *    Param         : ID tskid : Task ID (TSK_SELF cannot be specified)
 *                    T_REGS *regs : General registers
 *                    T_EIT *eit : Registers saved when exception is raised
 *                    T_CREGS *cregs : Control registers
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_set_reg_impl( ID tskid, T_REGS *regs, T_EIT *eit, T_CREGS *cregs )
{
    TCB  *tcb;
    ER  ercd = E_OK;

    KNL_CHK_TSKID(tskid)

    tcb = KNL_TSK_GET_CB(tskid);
    if ( tcb == knl_tsk_execution ) {
        return E_OBJ;
    }

    KNL_INT_BEGIN_DISABLE
    if ( tcb->state == TS_NONEXIST ) {
        ercd = E_NOEXS;
    } else {
        knl_ctx_setreg(tcb, regs, eit, cregs);
    }
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TD_SET_REG */

#endif /* TK_USE_DBGSPT */
