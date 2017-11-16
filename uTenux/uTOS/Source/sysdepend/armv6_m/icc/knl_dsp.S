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
 *    CPU:        CORTEX M0 Compiled by IAR EWARM
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : knl_dsp.S
 *    Create Date    : 2013/3/19-2014/3/6
 *    Author         : wangshb
 *    Description    : CPU-Dependent dispatcher Operation function.
 *----------------------------------------------------------------------------------------
 */

#include "knl_kernel.h"
    
#define KNL_DSP_TMP_STKSIZE  128

/*
 * Imports external symbols
 */
    EXTERN  knl_pow_discnt
    EXTERN  knl_pow_low
    EXTERN  knl_tsk_dspdisabled
    EXTERN  knl_tsk_execution
    EXTERN  knl_tsk_mode
    EXTERN  knl_tsk_schedule

/*
 * Define public symbols
 */
    SECTION .bss:DATA
    ALIGNRAM 2
    /* Temporal stack used when knl_dsp_force is called */
knl_dsp_tmpstack:
    DS8     KNL_DSP_TMP_STKSIZE
    
/*
 *    Function Name : knl_dsp_entry,
 *                    knl_dsp_force,
 *                    knl_dsp_retint
 *    Create Date   : 2009/12/27-2014/2/28
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
    SECTION .text:CODE
    THUMB
    ALIGNROM 1
    PUBLIC  knl_dsp_force
knl_dsp_force:
    /* Interrupt is disabled,during Privileged Thread Mode */
	
    /* Switch to temporal stack */
    ldr     r0, =(knl_dsp_tmpstack + KNL_DSP_TMP_STKSIZE) 
    mov     sp, r0
    ldr     r2, =knl_tsk_dspdisabled
    ldr     r0, =1
    str     r0, [r2]                             /* Dispatch disable */

    ldr     r4, =knl_tsk_execution               /* R4 = &ctxtsk */
    ldr     r0, =0
    str     r0, [r4]                             /* ctxtsk = NULL */
    cpsie   i                                    /* Interrupt enable */
    b       l_hokjmp

    ALIGNROM 1
    PUBLIC knl_dsp_entry, knl_dsp_retint
knl_dsp_entry:
knl_dsp_retint:
    /* Interrupt is disabled,during Privileged Handler Mode */
    ldr     r2, =knl_tsk_dspdisabled
    ldr     r0, =1
    str     r0, [r2]                             /* Dispatch disable */
    cpsie   i                                    /* Interrupt enable */
    ldr     r2, =knl_tsk_mode
    ldr     r0, [r2]
    push    {r0}
    mov     r0, r11
    push    {r0}                                 /* Push r11 to stack */
    mov     r0, r10
    push    {r0}                                 /* Push r10 to stack */
    mov     r0, r9
    push    {r0}                                 /* Push r9 to stack */
    mov     r0, r8
    push    {r0}                                 /* Push r8 to stack */
    push    {r4-r7}
    ldr     r4, =knl_tsk_execution               /* R4 = &ctxtsk */
    ldr     r7, [r4]                             /* R7 = ctxtsk */
    mov     r0, sp
    movs    r1, #KNL_TSK_TCB_CTXB + KNL_TSK_CTXB_SSP
    str     r0, [r7, r1]                         /* Save 'ssp' to TCB */
    ldr     r0, =0
    str     r0, [r4]                             /* ctxtsk = NULL */

l_hokjmp:
    /* During interrupt enable */
    ldr     r5, =knl_tsk_schedule                /* R5 = &schedtsk */
    ldr     r6, =knl_pow_discnt                  /* R6 = &knl_pow_discnt */

    /* Judge if goto 'knl_tsk_schedule'or'knl_pow_low' */
l_lowpow:  
    cpsid   i
    ldr     r3, [r5]                             /* R3 = schedtsk */
    cmp     r3, #0                               /* Is there 'schedtsk'? */
    bne     l_dispatch

    /* Because there is no task that should be executed, move to the power-saving mode */
    push    {lr}
    ldr     r3, =knl_pow_low
    ldr     r2, [r6]
    cmp     r2, #0                               /* Is 'knl_power_low' disabled? */
    bne     l_nolow                              /* call knl_power_low() */
    blx     r3
l_nolow:
    pop     {r2}
    mov     lr, r2
    cpsie   i                                    /* Interrupt enable */
    b       l_lowpow

l_dispatch:                                     /* Switch to 'schedtsk' */
    /* During interrupt disable */
    str     r3, [r4]                             /* ctxtsk = schedtsk */
                                                 /* Restore 'ssp' from TCB */
    ldr     r0, [r3, #KNL_TSK_TCB_CTXB + KNL_TSK_CTXB_SSP]    
    mov     sp, r0
    ldr     r2, =knl_tsk_dspdisabled
    ldr     r0, =0
    str     r0, [r2]                             /* Dispatch enable */
    mrs     r2,psr
    ldr     r0,=0x1FF
    ands    r2,r2,r0
    cmp     r2,#0
    beq     l_noexc                              /*  if 0, through call knl_dsp_force() */
    bgt     l_inexc                              /*  if >0, through PENDSV */

l_noexc:
    pop     {r4-r7}
    pop     {r0}                                 /* Pop stack to r8 */
    mov     r8, r0
    pop     {r0}                                 /* Pop stack to r9 */
    mov     r9, r0
    pop     {r0}                                 /* Pop stack to r10 */
    mov     r10, r0
    pop     {r0}                                 /* Pop stack to r11 */
    mov     r11, r0
    ldr     r2, =knl_tsk_mode
    pop     {r0}

    str     r0, [r2]
    mov     r3, sp
    ldr     r0, [r3, #24]                        /* load stack's pc to r0 */
    movs    r1, #1
    orrs    r0, r0, r1
    str     r0, [r3, #24]
    ldr     r0, [r3, #28]                        /* load stack's psr to r0 */
    movs    r1, #1
    lsls    r1, #9
    tst     r0, r1                               /* test psr bit[9]==1? */
    beq     l_normalstk                          /* yes, exception stack is 9 words*/
    ldr     r0, [r3, #16]                        /* load stack's r12 to r0 */
    mov     r12, r0
    ldr     r0, [r3, #20]                        /* load stack's lr to r0 */
    mov     lr, r0
    ldr     r0, [r3, #24]                        /* load stack's pc to r0 */
    str     r0, [r3, #32]                        /* set  stack's 9th to pc*/
    pop     {r0-r3}
    add     sp, sp, #16
    cpsie   i                                    /* just before schedule to next task */
    pop     {pc}
l_normalstk:                                     /* no, normal */
     ldr    r0, [r3, #16]                        /* load stack's r12 to r0 */
    mov     r12, r0
    ldr     r0, [r3, #20]                        /* load stack's lr to r0 */
    mov     lr, r0
    ldr     r0, [r3, #24]                        /* load stack's pc to r0 */
    str     r0, [r3, #28]                        /* set  stack's psr to pc*/
    pop     {r0-r3}
    add     sp, sp, #12
    cpsie   i                                    /* just before schedule to next task */
    pop     {pc}

l_inexc:
    pop     {r4-r7}
    pop     {r0}                                 /* Pop stack to r8 */
    mov     r8, r0
    pop     {r0}                                 /* Pop stack to r9 */
    mov     r9, r0
    pop     {r0}                                 /* Pop stack to r10 */
    mov     r10, r0
    pop     {r0}                                 /* Pop stack to r11 */
    mov     r11, r0
    ldr     r2, =knl_tsk_mode
    pop     {r0}
    str     r0, [r2]
    cpsie   i                                    /* just before schedule to next task */
    bx      lr                                   /* lr is EXC_RETURN */

    END
