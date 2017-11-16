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
 *    CPU:        CORTEX M4 Compiled by uLoong uT/STUDIO
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : knl_dsp.S
 *    Create Date    : 2009/12/27-2014/4/30
 *    Author         : WangShb-wangshb
 *    Description    : CPU-Dependent dispatcher Operation function.
 *----------------------------------------------------------------------------------------
 */

#include "knl_kernel.h"

#if (TK_USE_DBGSPT && TK_USE_HOOK_TRACE)
#define KNL_DSP_TMP_STKSIZE  256
#else
#define KNL_DSP_TMP_STKSIZE  128
#endif /* TK_USE_DBGSPT && TK_USE_HOOK_TRACE */

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
 *    Create Date   : 2009/12/27-2014/4/30
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
 *                         Called by PendSV exception.
 *                    3.knl_dsp_retint:
 *                         Called when dispatch is required by 'tk_ret_int().'
 *    Param         : none
 *    Return Code   : none
 */
    .text
    .syntax unified
    .balign 2
    .global knl_dsp_force
    .type   knl_dsp_force, function
knl_dsp_force:
    /* Interrupt is disabled,during Privileged Thread Mode */

    /* Switch to temporal stack */
    ldr     sp, =(knl_dsp_tmpstack + KNL_DSP_TMP_STKSIZE)

    ldr     r12, =knl_tsk_dspdisabled
    ldr     r0, =1
    str     r0, [r12]                            /* Dispatch disable */

    ldr     r4, =knl_tsk_execution               /* R4 = &knl_tsk_execution */
    ldr     r0, =0
#if TK_USE_DBGSPT
    ldr     r8, [r4]                             /* R8 = knl_tsk_execution */
#endif
    str     r0, [r4]                             /* knl_tsk_execution = NULL */
    cpsie   i                                    /* Interrupt enable */
    b       l_hokjmp

    .balign 2
    .global knl_dsp_entry, knl_dsp_retint
    .type   knl_dsp_entry, function
    .type   knl_dsp_retint, function
knl_dsp_entry:
knl_dsp_retint:
    /* Interrupt is disabled,during Privileged Handler Mode */
    ldr     r12, =knl_tsk_dspdisabled
    ldr     r0, =1
    str     r0, [r12]                            /* Dispatch disable */
    cpsie   i                                    /* Interrupt enable */
#if TK_USE_FPU
    vpush   {s16-s31}
#endif /* TK_USE_FPU */
    ldr     r12, =knl_tsk_mode
    ldr     r0, [r12]
    push    {r0}
    push    {r4-r11}
    ldr     r4, =knl_tsk_execution               /* R4 = &knl_tsk_execution */
    ldr     r0, =0
    ldr     r8, [r4]                             /* R8 = knl_tsk_execution */
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
#endif /* TK_USE_DBGSPT && TK_USE_HOOK_TRACE */
    ldr     r5, =knl_tsk_schedule                /* R5 = &knl_tsk_schedule */
    ldr     r6, =knl_pow_discnt                  /* R6 = &knl_pow_discnt */

    /* Judge if goto 'knl_tsk_schedule' or 'knl_pow_low' */
l_lowpow:
    cpsid   i
    ldr     r8, [r5]                             /* R8 = knl_tsk_schedule */
    cmp     r8, #0                               /* Is there 'knl_tsk_schedule'? */
    bne     l_dispatch

    /* Because there is no task that should be executed, move to the power-saving mode */
    push    {lr}
    ldr     r12, [r6]
    cmp     r12, #0                              /* Is 'knl_pow_low' disabled? */
    it      eq
    bleq    knl_pow_low                          /* call knl_pow_low() */
    pop     {lr}
    cpsie   i                                    /* Interrupt enable */
    b       l_lowpow

l_dispatch:                                      /* Switch to 'knl_tsk_schedule' */
    /* During interrupt disable */
    str     r8, [r4]                             /* knl_tsk_execution=knl_tsk_schedule */
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
#endif /* TK_USE_DBGSPT && TK_USE_HOOK_TRACE */

    ldr     r12, =knl_tsk_dspdisabled
    ldr     r0, =0
    str     r0, [r12]                            /* Dispatch enable */
    mrs     r12,psr
    ldr     r0,=0x1FF
    and     r12,r12,r0
    cmp     r12,#0
    beq     l_noexc                              /*  if 0, through call knl_dsp_force() */
    bgt     l_inexc                              /*  if >0, through PENDSV */

l_noexc:
    pop     {r4-r11}
    ldr     r12, =knl_tsk_mode
    pop     {r0}
    str     r0, [r12]
#if TK_USE_FPU
    vpop    {s16-s31}
#endif /* TK_USE_FPU */
    ldr     r0, [sp, #24]                        /* load stack's pc to r0 */
    orr     r0, r0, #1
    str     r0, [sp, #24]
    ldr     r0, [sp, #28]                        /* load stack's psr to r0 */
    mov     r1, #(0x01<<9)                       /* test psr bit[9]==1? */
    tst     r0, r1
#if !TK_USE_FPU
    beq     l_normalstk
    ldr     r0, [sp, #24]                        /* load stack's pc to r0 */
    str     r0, [sp, #32]                        /* save pc to stack's 9th */
    pop     {r0-r3}                              /* yes, exception stack is 9 words*/
    pop     {r12}
    pop     {lr}
    add     sp, sp, #8
    cpsie   i                                    /* just before schedule to next task */
    pop     {pc}
l_normalstk:                                     /* no, normal */
    ldr     r0, [sp, #24]                        /* load stack's pc to r0 */
    str     r0, [sp, #28]                        /* save pc to stack's psr */
    pop     {r0-r3}
    pop     {r12}
    pop     {lr}
    add     sp, sp, #4
    cpsie   i                                    /* just before schedule to next task */
    pop     {pc}
#else
    beq     l_normalstk
                                                 /* yes, exception stack is 27 words*/
    ldr     r0, [sp, #96]                        /* load stack's fpscr to r0 */
    vmsr    fpscr, r0                            /* move r0 to fpscr */
    ldr     r0, [sp, #24]                        /* load stack's pc to r0 */
    str     r0, [sp, #104]                       /* save pc to stack's 27th */
    pop     {r0-r3}
    pop     {r12}
    pop     {lr}
    add     sp, sp, #8
    vpop    {s0-s15}
    add     sp, sp, #8
    cpsie   i                                    /* just before schedule to next task */
    pop     {pc}
l_normalstk:                                     /* no, normal */
    ldr     r0, [sp, #96]                        /* load stack's fpscr to r0 */
    vmsr    fpscr, r0                            /* move r0 to fpscr */
    ldr     r0, [sp, #24]                        /* load stack's pc to r0 */
    str     r0, [sp, #100]                       /* save pc to stack's reserved */
    pop     {r0-r3}
    pop     {r12}
    pop     {lr}
    add     sp, sp, #8
    vpop    {s0-s15}
    add     sp, sp, #4
    cpsie   i                                    /* just before schedule to next task */
    pop     {pc}
#endif /* !TK_USE_FPU */

l_inexc:
    pop     {r4-r11}
    ldr     r12, =knl_tsk_mode
    pop     {r0}
    str     r0, [r12]
#if TK_USE_FPU
    vpop    {s16-s31}
#endif /* TK_USE_FPU */
    cpsie   i                                    /* just before schedule to next task */
    bx      lr                                   /* lr is EXC_RETURN */

    .end
