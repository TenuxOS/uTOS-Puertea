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
 *    CPU:        CORTEX M4
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : knl_ctx.h
 *    Create Date    : 2009/12/27-2014/4/1
 *    Author         : wangshb
 *    Description    : Kernel context routine head file.
 *----------------------------------------------------------------------------------------
 */

#ifndef _KNL_CTX_H__
#define _KNL_CTX_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Definition of minimum task stack size
 *  Minimum task stack size when setting the task stack size per task by 'tk_cre_tsk().'
 *  this size must be larger than the size of SSTACKFRAME
 */
#if !TK_USE_FPU
#define KNL_CTX_MIN_TASK_STACK_SIZE   \
                             128
#else
#define KNL_CTX_MIN_TASK_STACK_SIZE   \
                             256
#endif /* TK_USE_FPU */

/*
 * Size of task stack area destroyed by 'make_dormant()', Size is to 'taskmode' position.
 * In other words, the size of area required to write by 'knl_ctx_setframe().'
 */
#if !TK_USE_FPU
#define KNL_CTX_DORMANT_STACK_SIZE   \
                             ( sizeof(uint32_t) * 9u )
#else
#define KNL_CTX_DORMANT_STACK_SIZE   \
                             ( sizeof(uint32_t) * 43u )
#endif /* TK_USE_FPU */
                             
#ifndef Assembler

/*
 * System stack configuration at task startup
 */
typedef struct {
        uint32_t             r4;                 /* R4        */
        uint32_t             r5;                 /* R5        */
        uint32_t             r6;                 /* R6        */
        uint32_t             r7;                 /* R7        */
        uint32_t             r8;                 /* R8        */
        uint32_t             r9;                 /* R9        */
        uint32_t             r10;                /* R10       */
        uint32_t             r11;                /* R11       */
        uint32_t             taskmode;           /* used for saving knl_taskmode */
#if TK_USE_FPU
        uint32_t             s16;                /* S16       */
        uint32_t             s17;                /* S17       */
        uint32_t             s18;                /* S18       */   
        uint32_t             s19;                /* S19       */
        uint32_t             s20;                /* S20       */
        uint32_t             s21;                /* S21       */
        uint32_t             s22;                /* S22       */
        uint32_t             s23;                /* S23       */
        uint32_t             s24;                /* S24       */
        uint32_t             s25;                /* S25       */
        uint32_t             s26;                /* S26       */
        uint32_t             s27;                /* S27       */
        uint32_t             s28;                /* S28       */
        uint32_t             s29;                /* S29       */
        uint32_t             s30;                /* S30       */
        uint32_t             s31;                /* S31       */
#endif /* TK_USE_FPU */
        uint32_t             r0;                 /* R0        */
        uint32_t             r1;                 /* R1        */
        uint32_t             r2;                 /* R2        */
        uint32_t             r3;                 /* R3        */
        uint32_t             r12;                /* R12       */
        VP                   lr;                 /* LR        */
        VP                   pc;                 /* PC        */
        uint32_t             xpsr;               /* PSR       */
#if TK_USE_FPU
        uint32_t             s0;                 /* S0        */
        uint32_t             s1;                 /* S1        */
        uint32_t             s2;                 /* S2        */   
        uint32_t             s3;                 /* S3        */
        uint32_t             s4;                 /* S4        */
        uint32_t             s5;                 /* S5        */
        uint32_t             s6;                 /* S6        */
        uint32_t             s7;                 /* S7        */
        uint32_t             s8;                 /* S8        */
        uint32_t             s9;                 /* S9        */
        uint32_t             s10;                /* S10       */
        uint32_t             s11;                /* S11       */
        uint32_t             s12;                /* S12       */
        uint32_t             s13;                /* S13       */
        uint32_t             s14;                /* S14       */
        uint32_t             s15;                /* S15       */
        uint32_t             fpscr;              /* FPSCR     */
        uint32_t             reserved;           /* RESERVED  */
#endif /* TK_USE_FPU */
}                            SSTACKFRAME;

/*
 *    Description   : Clear up task contexts.
 *    Param         : TCB *tcb: task control block pointer
 *    Return Code   : void
 */
Inline void knl_ctx_cleanup( TCB *tcb )
{
}

/*
 *    Description   : Create stack frame for task startup.
 *                    Call from 'make_dormant()'
 *    Param         : TCB *tcb: task control block pointer
 *    Return Code   : void
 */
Inline void knl_ctx_setframe( TCB *tcb )
{
    uint32_t pc, xpsr;
    SSTACKFRAME *ssp;

    ssp = tcb->isstack;
    ssp--;

    pc = (uint32_t)tcb->task;
    xpsr = TK_PSR_T;                             /* Thumb mode */

    /* CPU context initialization */
    ssp->taskmode  = 0u;                         /* Initial taskmode */
    ssp->xpsr = xpsr;                            /* Initial SR */
    ssp->pc = (VP)pc;                            /* Task startup address */
    tcb->tskctxb.ssp = ssp;                      /* System stack */

#if TK_USE_FPU
    /* Initial FPU register to 0 */
    ssp->s0  = 0u; 
    ssp->s1  = 0u; 
    ssp->s2  = 0u;
    ssp->s3  = 0u;
    ssp->s4  = 0u;
    ssp->s5  = 0u;
    ssp->s6  = 0u;
    ssp->s7  = 0u;
    ssp->s8  = 0u;
    ssp->s9  = 0u;
    ssp->s10 = 0u; 
    ssp->s11 = 0u; 
    ssp->s12 = 0u;
    ssp->s13 = 0u;
    ssp->s14 = 0u;
    ssp->s15 = 0u;
    ssp->s16 = 0u;
    ssp->s17 = 0u;
    ssp->s18 = 0u;
    ssp->s19 = 0u;
    ssp->s20 = 0u; 
    ssp->s21 = 0u; 
    ssp->s22 = 0u;
    ssp->s23 = 0u;
    ssp->s24 = 0u;
    ssp->s25 = 0u;
    ssp->s26 = 0u;
    ssp->s27 = 0u;
    ssp->s28 = 0u;
    ssp->s29 = 0u;
    ssp->s30 = 0u; 
    ssp->s31 = 0u; 
    ssp->fpscr = 0u;
    ssp->reserved =0u;
#endif /* TK_USE_FPU */
}

/*
 *    Description   : Set task startup code.
 *                    Called by 'tk_sta_tsk()' processing.
 *    Param         : TCB *tcb: task control block pointer
 *                    int32_t stacd: task startup code
 *    Return Code   : void
 */
Inline void knl_ctx_setstacd( TCB *tcb, int32_t stacd )
{
    SSTACKFRAME *ssp;

    ssp= tcb->tskctxb.ssp;
    ssp->r0 = (uint32_t)stacd;
    ssp->r1 = (uint32_t)tcb->exinf;
}

/*
 *    Description   : Get task register contents.
 *    Param         : TCB *tcb: task control block pointer
 *                    T_REGS *regs: general purpose register pointer
 *                    T_EIT *eit: exception-related register pointer
 *                    T_CREGS *cregs: control register pointer
 *    Return Code   : void
 */
extern void knl_ctx_getreg( TCB *tcb, T_REGS *regs, T_EIT *eit, T_CREGS *cregs );

/*
 *    Description   : Set task register contents.
 *    Param         : TCB *tcb: task control block pointer
 *                    T_REGS *regs: general purpose register pointer
 *                    T_EIT *eit: exception-related register pointer
 *                    T_CREGS *cregs: control register pointer
 *    Return Code   : void
 */
extern void knl_ctx_setreg( TCB *tcb, T_REGS *regs, T_EIT *eit, T_CREGS *cregs );

#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* _KNL_CTX_H__ */
