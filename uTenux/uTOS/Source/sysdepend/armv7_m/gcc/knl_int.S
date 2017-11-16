/*
 *----------------------------------------------------------------------------------------
 *    Kernel/int
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
 *    CPU:        CORTEX M3 Compiled by uLoong uT/STUDIO
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : knl_int.S
 *    Create Date    : 2009/12/27-2014/4/1
 *    Author         : wangshb
 *    Description    : CPU-Dependent interrupt Operation function.
 *----------------------------------------------------------------------------------------
 */

#include "knl_kernel.h"

/*
 * Imports external symbols
 */
    .extern knl_dsp_retint
    .extern knl_hok_intenterjmp
    .extern knl_hok_intleavejmp
    .extern knl_int_hllhdr
    .extern knl_tck_handler
    .extern knl_tsk_dspdisabled
    .extern knl_tsk_execution
    .extern knl_tsk_independent
    .extern knl_tsk_schedule

/*
 *    Function Name : knl_int_defaulthdr
 *    Create Date   : 2013/10/01-2014/4/1
 *    Author        : wangshb
 *    Description   : Default exeception and interrupt process handler
 *    Param         : none
 *    Return Code   : none
 */
    .text
    .syntax unified
    .balign 2
    .global knl_int_defaulthdr
    .type   knl_int_defaulthdr, function
knl_int_defaulthdr:
    b       knl_int_defaulthdr

/*
 *    Function Name : knl_int_dis
 *    Create Date   : 2009/12/27-2014/4/1
 *    Author        : wangshb
 *    Description   : Disable external interrupt
 *                     PRIMASK interrupt flag is in  disabled status.
 *    Param         : none
 *    Return Code   : uint32_t: PRIMASK prior to change.
 */
    .text
    .syntax unified
    .balign 2
    .global knl_int_dis
    .type   knl_int_dis,function
knl_int_dis:
    mrs     r0, primask
    ldr     r1, =TK_PMK_D
    msr     primask, r1
    bx      lr

/*
 *    Function Name : knl_int_ena
 *    Create Date   : 2009/12/27-2014/4/1
 *    Author        : wangshb
 *    Description   : Enable external interrupt (restore to original state),
 *                    Updates PRIMASK interrupt disabled flag with the intsts.
 *    Param         : uint32_t intsts: PRIMASK remains unchanged apart
 *                                     from the interrupt disabled flag.
 *    Return Code   : uint32_t: PRIMASK prior to change.
 */
    .text
    .syntax unified
    .balign 2
    .global knl_int_ena
    .type   knl_int_ena, function
knl_int_ena:
    mrs     r1, primask
    msr     primask, r0
    mov     r0, r1
    bx      lr

#if ( TK_USE_INT_DEFHDR && TK_USE_INT_HLLHDR )
/*
 *    Function Name : knl_int_excwraphdr,knl_int_intwraphdr
 *    Create Date   : 2009/12/27-2014/4/1
 *    Author        : wangshb
 *    Description   : High level programming language routine for interrupt handler
 *                    prepare to call high language interrupt handler,as below:
 *                      void intdhr( uint32_t dintno, VP ctxptr );
 *                    because CM core auto pushs r0-3,r12  when interrupt is occurred
 *                    so that here no need to saving r0-3,r12.
 *    Param         : none
 *    Return Code   : none
 */
    .text
    .syntax unified
    .balign 2
    .global knl_int_excwraphdr
    .global knl_int_intwraphdr
    .type   knl_int_excwraphdr, function
    .type   knl_int_intwraphdr, function
knl_int_excwraphdr:
knl_int_intwraphdr:
    cpsid   i
    push    {lr}

    mrs     r0, ipsr                             /* r0 = dintno */
    mov     r1, sp                               /* r1 = sp */

    ldr     r12, =knl_tsk_independent            /* Task independent part */
    ldr     lr, [r12]
    add     lr, lr, #1
    str     lr, [r12]

#if (TK_USE_DBGSPT && TK_USE_HOOK_TRACE)
    ldr     r12, =knl_hok_intenterjmp
    ldr     pc, [r12]
/*
 * Interrupt hook enter function return address
 */
    .global knl_int_hokenterret
    .type   knl_int_hokenterret, function
knl_int_hokenterret:
#endif /* TK_USE_DBGSPT && TK_USE_HOOK_TRACE */

    ldr     r12, =knl_int_hllhdr
    lsl     r3, r0, #2                           /* r3 is offset */
    ldr     r12, [r12, r3]
    blx     r12                                  /* call hll_inthdr[n](dintno) */

#if (TK_USE_DBGSPT && TK_USE_HOOK_TRACE)
    ldr     r12, =knl_hok_intleavejmp
    ldr     pc, [r12]
/*
 * Interrupt hook leave function return address
 */
    .global knl_int_hokleaveret
    .type   knl_int_hokleaveret, function
knl_int_hokleaveret:
#endif /* TK_USE_DBGSPT && TK_USE_HOOK_TRACE */

    ldr     r12, =knl_tsk_independent
    ldr     lr, [r12]
    sub     lr, lr, #1
    str     lr, [r12]

    cpsie   i
    pop     {lr}
    b       tk_ret_int_impl

#endif /* TK_USE_INT_DEFHDR && TK_USE_INT_HLLHDR */

/*
 *    Function Name : knl_int_tckhdr
 *    Create Date   : 2009/12/27-2014/4/1
 *    Author        : wangshb
 *    Description   : Hardware timer interrrupt/exception handler
 *                     for calling high level language routine(knl_tck_handler)
 *    Param         : none
 *    Return Code   : none
 */
    .text
    .syntax unified
    .balign 2
    .global knl_int_tckhdr
    .type   knl_int_tckhdr, function
knl_int_tckhdr:
    push    {lr}
    ldr     r1, =knl_tsk_independent             /* Enter task independent part */
    ldr     r2, [r1]
    add     r3, r2, #1
    str     r3, [r1]
    push    {r1, r2}
    bl      knl_tck_handler                      /* call tick handler() */

    pop     {r1,r2}
    str     r2, [r1]                             /* Leave task independent part */
    pop     {lr}

    b       tk_ret_int_impl

/*
 *    Function Name : tk_ret_int_impl
 *    Create Date   : 2009/12/27-2014/4/1
 *    Author        : wangshb
 *    Description   : Return from Interrupt Handler
 *    Param         : none
 *    Return Code   : none
 */
    .text
    .syntax unified
    .balign 2
    .global tk_ret_int_impl
    .type   tk_ret_int_impl, function
tk_ret_int_impl:
    cpsid   i

    ldr     r0, =knl_tsk_independent             /* Is it a nesting interrupt? */
    ldr     r0, [r0]
    cmp     r0, #0
    bne     l_nodsp

    ldr     r0, =knl_tsk_dspdisabled             /* Is it during dispatch disable? */
    ldr     r0, [r0]
    cmp     r0, #0
    bne     l_nodsp

    ldr     r0, =knl_tsk_execution               /* Is dispatch required? */
    ldr     r1, =knl_tsk_schedule
    ldr     r0, [r0]
    ldr     r1, [r1]
    cmp     r0, r1
    bne     knl_dsp_retint                       /* To dispatch processing */

l_nodsp:
    cpsie   i
    bx      lr

    .end

