/*
 *----------------------------------------------------------------------------------------
 *    Kernel/Ctx
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
 *    CPU:        CORTEX M3
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : knl_ctx.c
 *    Create Date    : 2009/12/27-2014/2/13
 *    Author         : WangShb
 *    Description    : Kernel context routine function.
 *----------------------------------------------------------------------------------------
 */

#include "knl_ctx.h"

#ifdef KNL_CFG_USE_FUNC_KNL_CTX_GETREG
/*
 *    Function Name : knl_ctx_getreg
 *    Create Date   : 2012/7/9-2014/2/1
 *    Author        : wangshb
 *    Description   : Get task register contents.
 *    Param         : TCB *tcb: task control block pointer
 *                    T_REGS *regs: general purpose register pointer
 *                    T_EIT *eit: exception-related register pointer
 *                    T_CREGS *cregs: control register pointer
 *    Return Code   : void
 */
void knl_ctx_getreg( TCB *tcb, T_REGS *regs, T_EIT *eit, T_CREGS *cregs )
{
    SSTACKFRAME *ssp;

    ssp = tcb->tskctxb.ssp;

    if ( regs != NULL ) {
        regs->r[0]    = ssp->r0;
        regs->r[1]    = ssp->r1;
        regs->r[2]    = ssp->r2;
        regs->r[3]    = ssp->r3;
        regs->r[4]    = ssp->r4;
        regs->r[5]    = ssp->r5;
        regs->r[6]    = ssp->r6;
        regs->r[7]    = ssp->r7;
        regs->r[8]    = ssp->r8;
        regs->r[9]    = ssp->r9;
        regs->r[10]   = ssp->r10;
        regs->r[11]   = ssp->r11;
        regs->r[12]   = ssp->r12;
        regs->lr      = ssp->lr;
    }

    if ( eit != NULL ) {
        eit->pc       = ssp->pc;
        eit->psr      = ssp->xpsr;
        eit->taskmode = ssp->taskmode;
    }

    if ( cregs != NULL ) {
        cregs->ssp    = tcb->tskctxb.ssp;
        cregs->usp    = tcb->tskctxb.ssp;
    }
}
#endif /* KNL_CFG_USE_FUNC_KNL_CTX_GETREG */

#ifdef KNL_CFG_USE_FUNC_KNL_CTX_SETREG
/*
 *    Function Name : knl_ctx_setreg
 *    Create Date   : 2012/7/9-2014/2/1
 *    Author        : wangshb
 *    Description   : Set task register contents.
 *    Param            : TCB *tcb: task control block pointer
 *                    T_REGS *regs: general purpose register pointer
 *                    T_EIT *eit: exception-related register pointer
 *                    T_CREGS *cregs: control register pointer
 *    Return Code   : void
 */
void knl_ctx_setreg( TCB *tcb, T_REGS *regs, T_EIT *eit, T_CREGS *cregs )
{
    SSTACKFRAME *ssp;

    ssp = tcb->tskctxb.ssp;

    if ( cregs != NULL ) {
        ssp = cregs->ssp;
    }

    if ( regs != NULL ) {
        ssp->r0         = regs->r[0];
        ssp->r1         = regs->r[1];
        ssp->r2         = regs->r[2];
        ssp->r3         = regs->r[3];
        ssp->r4         = regs->r[4];
        ssp->r5         = regs->r[5];
        ssp->r6         = regs->r[6];
        ssp->r7         = regs->r[7];
        ssp->r8         = regs->r[8];
        ssp->r9         = regs->r[9];
        ssp->r10        = regs->r[10];
        ssp->r11        = regs->r[11];
        ssp->r12        = regs->r[12];
        ssp->lr         = regs->lr;
    }

    if ( eit != NULL ) {
        ssp->pc         = eit->pc;
        ssp->xpsr       = eit->psr;
        ssp->taskmode   = eit->taskmode;
    }

    if ( cregs != NULL ) {
        tcb->tskctxb.ssp  = cregs->ssp;
    }
}
#endif /* KNL_CFG_USE_FUNC_KNL_CTX_SETREG */
