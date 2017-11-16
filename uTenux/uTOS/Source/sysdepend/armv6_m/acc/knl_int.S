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
 *    CPU:        CORTEX M0 Compiled by KEIL RVMDK
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : knl_int.S
 *    Create Date    : 2013/3/19-2014/4/1
 *    Author         : wangshb
 *    Description    : CPU-Dependent interrupt Operation function.
 *----------------------------------------------------------------------------------------
 */

#include "knl_kernel.h"

/*
 * Imports external symbols
 */
    EXTERN knl_dsp_retint [CODE]
    EXTERN knl_tck_handler [CODE]
    EXTERN knl_tsk_dspdisabled
    EXTERN knl_tsk_execution
    EXTERN knl_tsk_independent
    EXTERN knl_tsk_schedule

/*
 *    Function Name : knl_int_defaulthdr
 *    Create Date   : 2013/10/01-2014/4/1
 *    Author        : wangshb
 *    Description   : Default exeception and interrupt process handler
 *    Param         : none
 *    Return Code   : none
 */
    AREA |.text|,CODE,READONLY
    THUMB
    ALIGN 2
    GLOBAL knl_int_defaulthdr [CODE]
knl_int_defaulthdr
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
    AREA |.text|,CODE,READONLY
    THUMB
    ALIGN 2
    GLOBAL knl_int_dis [CODE]
knl_int_dis
    mrs     r0, primask
    ldr     r1, =TK_PMK_D
    msr     primask, r1
    bx      lr
    nop

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
    AREA |.text|,CODE,READONLY
    THUMB
    ALIGN 2
    GLOBAL knl_int_ena [CODE]
knl_int_ena
    mrs     r1, primask
    msr     primask, r0
    mov     r0, r1
    bx      lr

/*
 *    Function Name : knl_int_tckhdr
 *    Create Date   : 2009/12/27-2014/4/1
 *    Author        : wangshb
 *    Description   : Hardware timer interrrupt/exception handler
 *                     for calling high level language routine(knl_tck_handler)
 *    Param         : none
 *    Return Code   : none
 */
    AREA |.text|,CODE,READONLY
    THUMB
    ALIGN 2
    PRESERVE8
    GLOBAL knl_int_tckhdr [CODE]
knl_int_tckhdr
    push    {lr}
    ldr     r1, =knl_tsk_independent             /* Enter task independent part */
    ldr     r2, [r1]
    adds    r3, r2, #1
    str     r3, [r1]
    push    {r1, r2}
    ldr     r1, =knl_tck_handler
    blx     r1                                   /* call tick handler() */
    
    pop     {r1,r2}
    str     r2, [r1]                             /* Leave task independent part */
    pop     {r2}
    mov      lr, r2

    b       tk_ret_int_impl
    nop

/*
 *    Function Name : tk_ret_int_impl
 *    Create Date   : 2009/12/27-2014/4/1
 *    Author        : wangshb
 *    Description   : Return from Interrupt Handler
 *    Param         : none
 *    Return Code   : none
 */
    AREA |.text|,CODE,READONLY
    THUMB
    ALIGN 2
    GLOBAL tk_ret_int_impl [CODE]
tk_ret_int_impl
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
    beq     l_nodsp
l_dsp                                            /* To dispatch processing */
    ldr     r0, =knl_dsp_retint
    bx      r0

l_nodsp
    cpsie   i
    bx      lr
    nop
    
    END
