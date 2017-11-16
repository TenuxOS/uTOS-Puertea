/*
 *----------------------------------------------------------------------------------------
 *    Kernel/dsp
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
 *    CPU:        ARM7TDMI Compiled by uLoong uT/STUDIO
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : knl_dsp.S
 *    Create Date    : 2009/12/27-2014/5/1
 *    Author         : wangshb
 *    Description    : CPU-Dependent dispatcher Operation function.
 *----------------------------------------------------------------------------------------
 */

#include "knl_kernel.h"

#if (TK_USE_DBGSPT && TK_USE_HOOK_TRACE)
#define KNL_DSP_TMP_STKSIZE  256
#else
#define KNL_DSP_TMP_STKSIZE  128
#endif

/*
 * Imports external symbols
 */
    .extern knl_hok_dspexecjmp
    .extern knl_hok_dspstopjmp
    .extern knl_pow_discnt
    .extern knl_pow_low
    .extern knl_tsk_dspdisabled
    .extern knl_tsk_execution
    .extern knl_tsk_mode
    .extern knl_tsk_schedule

/*
 * Define public symbols
 */
    .bss
    .balign 4
    /* Temporal stack used when knl_dsp_force is called */
knl_dsp_tmpstack:
    .space  KNL_DSP_TMP_STKSIZE,0

/*
 *    Function Name : knl_dsp_entry,
 *                    knl_dsp_force,
 *                    knl_dsp_retint
 *    Create Date   : 2009/12/27-2014/5/1
 *    Author        : wangshb
 *    Description   : Dispatcher,save contexts 'ssp' to TCB.include three parts.
 *                    1.knl_dsp_force:
 *                         Throw away the current contexts and forcibly dispatch to
 *                         'knl_tsk_schedule.'
 *                         Called directly by jump (bx) but do not return.
 *                         Called on the undefined stack state (undefined 'ssp').
 *                         Called on the interrupt disable state.
 *                    2.knl_dsp_entry:
 *                         Normal dispatch processing.
 *                    3.knl_dsp_retint:
 *                         Called when dispatch is required by 'tk_ret_int().'
 *    Param         : none
 *    Return Code   : none
 */
    .text
    .arm
    .balign 4
    .global knl_dsp_force
    .type   knl_dsp_force, function
knl_dsp_force:
    /* Interrupt is disabled(CPSR.I=1 F=1),during SVC mode */

    /* Switch to temporal stack */
    ldr     sp, =(knl_dsp_tmpstack + KNL_DSP_TMP_STKSIZE)

    ldr     r12, =knl_tsk_dspdisabled
    ldr     r0, =1
    str     r0, [r12]                            /* Dispatch disable */

    ldr     r4, =knl_tsk_execution               /* R4 = &knl_tsk_execution */
    ldr     r0, =0
#if TK_USE_DBGSPT
    ldr     r8, [r4]
#endif
    str     r0, [r4]                             /* knl_tsk_execution = NULL */

    msr     cpsr_c, #TK_PSR_SVC                  /* Interrupt enable */
    b       l_hokjmp

    .balign 4
    .global knl_dsp_entry, knl_dsp_retint
    .type   knl_dsp_entry, function
    .type   knl_dsp_retint, function
knl_dsp_entry:
    /* Interrupt is disabled(CPSR.I=1 F=1),during SVC mode */
    stmfd   sp!, {r12, lr}
    mrs     r12, cpsr
    bic     r12, r12, #TK_PSR_DI
    stmfd   sp!, {r12}
    stmfd   sp!, {lr}                            /* Context save (R14_svc) */

knl_dsp_retint:
    /* Interrupt is disabled(CPSR.I=1 F=1),during SVC mode */
    ldr     r12, =knl_tsk_dspdisabled
    ldr     lr, =1
    str     lr, [r12]                            /* Dispatch disable */

    msr     cpsr_c, #TK_PSR_SVC                  /* Interrupt enable */

    ldr     r12, =knl_tsk_mode
    ldr     r12, [r12]
    sub     sp, sp, #15*4
    stmia   sp, {r0-r11, r12, sp, lr}^           /* Context save */

    ldr     r4, =knl_tsk_execution               /* R4 = &knl_tsk_execution */
    ldr     r0, =0
    ldr     r8, [r4]
                                                 /* Save 'ssp' to TCB */
    str     sp, [r8, #KNL_TSK_TCB_CTXB + KNL_TSK_CTXB_SSP]
    str     r0, [r4]                             /* knl_tsk_execution = NULL */

l_hokjmp:
    /* During interrupt enable */
#if (TK_USE_DBGSPT && TK_USE_HOOK_TRACE)
    ldr     r12, =knl_hok_dspstopjmp             /* Hook processing */
    ldr     pc, [r12]
/*
 * Dispatcher hook stop function return address
 */
    .global knl_dsp_hokstopret
    .type   knl_dsp_hokstopret, function
knl_dsp_hokstopret:
#endif
    ldr     r5, =knl_tsk_schedule                /* R5 = &knl_tsk_schedule */
    ldr     r6, =knl_pow_discnt                  /* R6 = &knl_pow_discnt */

    /* Judge if goto 'knl_tsk_schedule' or 'knl_pow_low' */
l_lowpow:
    msr     cpsr_c, #TK_PSR_SVC | TK_PSR_DI      /* Interrupt disable */
    ldr     r8, [r5]                             /* R8 = knl_tsk_schedule */
    cmp     r8, #0                               /* Is there 'knl_tsk_schedule'? */
    bne     l_dispatch

    /* Because there is no task that should be executed, move to the power-saving mode */
    ldr     r8,  =knl_pow_low
    ldr     r12, [r6]                            /* Is power-saving mode disabled? */
    cmp     r12, #0
    mov     lr, pc
    bxeq    r8                                   /* call knl_pow_low() */
    msr     cpsr_c, #TK_PSR_SVC                  /* Interrupt enable */
    b       l_lowpow

l_dispatch:                                      /* Switch to 'knl_tsk_schedule' */
    /* During interrupt disable CPSR.I=1 F=1 */
    str     r8, [r4]                             /* knl_tsk_execution = knl_tsk_schedule */
                                                 /* Restore 'ssp' from TCB */
    ldr     sp, [r8, #KNL_TSK_TCB_CTXB + KNL_TSK_CTXB_SSP]

#if (TK_USE_DBGSPT && TK_USE_HOOK_TRACE)
    ldr     r12, =knl_hok_dspexecjmp             /* Hook processing */
    ldr     pc, [r12]
/*
 * Dispatcher hook exec function return address
 */
    .global knl_dsp_hokexecret
    .type   knl_dsp_hokexecret, function
knl_dsp_hokexecret:
#endif

    ldr     r12, =knl_tsk_dspdisabled
    ldr     lr, =0
    str     lr, [r12]                            /* Dispatch enable */

    ldmia   sp, {r0-r11, r12, sp, lr}^           /* Context restore */
    nop
    add     sp, sp, #15*4
    ldr     lr, =knl_tsk_mode
    str     r12, [lr]                            /* knl_tsk_mode restore */

    ldmfd   sp!, {lr}                            /* R14_svc restore */

    ldmfd    sp!, {r12}                          /* below code means EXC_RETURN */
    msr      spsr_fsxc, r12
    ldmfd    sp!, {r12, pc}^

    .end
