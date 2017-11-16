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
 *    CPU:        ARM7TDMI Compiled by IAR EWARM
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : knl_int.S
 *    Create Date    : 2009/12/27-2014/5/1
 *    Author         : wangshb
 *    Description    : CPU-Dependent interrupt Operation function.
 *----------------------------------------------------------------------------------------
 */

#include "knl_kernel.h"

/*
 * Imports external symbols
 */
    EXTERN  knl_dsp_retint
    EXTERN  knl_hok_intenterjmp
    EXTERN  knl_hok_intleavejmp
    EXTERN  knl_int_hllhdr
    EXTERN  knl_tck_handler
    EXTERN  knl_tsk_dspdisabled
    EXTERN  knl_tsk_execution
    EXTERN  knl_tsk_independent
    EXTERN  knl_tsk_schedule

/*
 *    Function Name : knl_int_defaulthdr
 *    Create Date   : 2009/12/27-2014/2/28
 *    Author        : wangshb
 *    Description   : Default exeception and interrupt process handler
 *    Param         : none
 *    Return Code   : none
 */
    SECTION .text:CODE
    ARM
    ALIGNROM 2
    PUBLIC  knl_int_defaulthdr
knl_int_defaulthdr:
    b       knl_int_defaulthdr

/*
 *    Function Name : knl_int_dis
 *    Create Date   : 2009/12/27-2014/2/28
 *    Author        : wangshb
 *    Description   : Disable external interrupt, 
 *                     CPSR_C interrupt flag is in  disabled status.
 *    Param         : none
 *    Return Code   : uint32_t: CPSR_C prior to change.
 */
    SECTION .text:CODE
    ARM
	ALIGNROM 2
    PUBLIC  knl_int_dis
knl_int_dis:
    mrs     r0, cpsr
    orr     r12, r0, #TK_PSR_DI
    msr     cpsr_c, r12
    bx      lr

/*
 *    Function Name : knl_int_ena
 *    Create Date   : 2009/12/27-2014/2/28
 *    Author        : wangshb
 *    Description   : Enable external interrupt (restore to original state),
 *                    Updates CPSR_C interrupt disabled flag with the intsts.
 *    Param         : uint32_t intsts: CPSR remains unchanged apart 
 *                                     from the interrupt disabled flag.
 *    Return Code   : uint32_t: CPSR prior to change.
 */
    SECTION .text:CODE
    ARM
	ALIGNROM 2
    PUBLIC  knl_int_ena
knl_int_ena:
    and     r3, r0, #TK_PSR_DI
    mrs     r0, cpsr
    bic     r2, r0, #TK_PSR_DI
    orr     r2, r2, r3
    msr     cpsr_c, r2
    bx      lr

#if ( TK_USE_INT_DEFHDR && TK_USE_INT_HLLHDR )
/*
 *    Function Name : knl_int_excwraphdr,knl_int_intwraphdr
 *    Create Date   : 2009/12/27-2014/5/1
 *    Author        : wangshb
 *    Description   : High level programming language routine for interrupt handler
 *                    prepare to call high language interrupt handler,as below:
 *                      void intdhr( uint32_t dintno, VP ctxptr );
 *    Param         : none
 *    Return Code   : none
 */
    SECTION .text:CODE
    ARM
    ALIGNROM 2
    PUBLIC  knl_int_excwraphdr
    PUBLIC  knl_int_intwraphdr
knl_int_excwraphdr:
    /* Unknown mode/During interrupt disable CPSR.I=1 F=? */
    stmfd   sp!, {r3}                            /* Register save */
    b       l_inthdr

knl_int_intwraphdr:
    /* Unknown mode/During interrupt disable CPSR.I=1 F=? */
    mrs     lr, cpsr
    and     lr, lr, #TK_PSR_SYS
    cmp     lr, #TK_PSR_SVC
    stmeqfd sp!, {r3}                            /* If it is SWI, also save 'r3' */
l_inthdr: 
    stmfd   sp!, {r0-r2}                         /* Register save */
                                                 /* r3 = Vector table begin */
    ldr     r3, =TK_RAM_VECTORAREA_TOP+TK_EXC_NUMBER*4
    sub     r3, r12, r3                          /* r3 = Vector table offset */

                                                 /* Argument of handler */
    mov     r0, r3, lsr #2                       /* r0 = dintno */
    add     r1, sp, #4*4                         /* r1 = sp */

    mrs     r2, cpsr                             /* r2 = CPSR save */
    msr     cpsr_c, #TK_PSR_IRQ|TK_PSR_DI        /* Move to IRQ mode/Interrupt disable */

    stmfd   sp!, {r2, lr}
    stmfd   sp!, {r12}

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
    PUBLIC knl_int_hokenterret
knl_int_hokenterret:
#endif /*TK_USE_DBGSPT && TK_USE_HOOK_TRACE*/

    ldr     r12, =knl_int_hllhdr
    ldr     r12, [r12, r3]
    mov     lr, pc
    bx      r12                                  /* call hll_inthdr[n](dintno, sp) */

#if (TK_USE_DBGSPT && TK_USE_HOOK_TRACE)
    ldr     r12, =knl_hok_intleavejmp
    ldr     pc, [r12]
/*
 * Interrupt hook leave function return address
 */
    PUBLIC knl_int_hokleaveret
knl_int_hokleaveret:
#endif /*TK_USE_DBGSPT && TK_USE_HOOK_TRACE*/

    ldmfd   sp!, {r12}                           /* IRQ mode register restore */
    ldmfd   sp!, {r2, r3}                        /* r2 = Original mode */
                                                 /* r3 = R14_irq */
    orr     r2, r2, #TK_PSR_DI
    msr     cpsr_c, r2

    ldr     r12, =knl_tsk_independent
    ldr     lr, [r12]
    sub     lr, lr, #1
    str     lr, [r12]

    ldmfd   sp!, {r0-r2}                         /* Register restore */
    swp     r3, r3, [sp]                         /* r3 restore, R14_svc/irq save */
    bl      tk_ret_int_impl

#endif /* TK_USE_INT_DEFHDR && TK_USE_INT_HLLHDR */

/*
 *    Function Name : knl_int_irqhdr
 *    Create Date   : 2009/12/27-2014/3/14
 *    Author        : wangshb
 *    Description   : Interrupt process handler
 *    Param         : none
 *    Return Code   : none
 */
    SECTION .text:CODE
    ARM
    ALIGNROM 2
    PUBLIC  knl_int_irqhdr
knl_int_irqhdr:
    sub     lr, lr, #4
    stmfd   sp!, {lr}                            /* sp-> lr_irq */
    
    TM_INT_PROTECT_MODE_MACRO                    /* definition in tm_int.h */
    
    stmfd   sp!, {r12}                           /* sp-> r12, lr_irq */
    mrs     r12, spsr
    stmfd   sp!, {r12}                           /* sp-> spsr_irq, r12, lr_irq */
    stmfd   sp!, {r3}                            /* sp-> r3, spsr_irq, r12, lr_irq */

    ldr     lr, =TM_INT_VECTOR_ISR               /* lr = address of IRQ No register */
    ldr     lr, [lr]                             /* lr = IRQ No. */
                                                 /* inturrept vector table */
    ldr     r12, =TK_RAM_VECTORAREA_TOP+TK_EXC_NUMBER*4*2
    add     r12, r12, lr, LSL #2                 /* r12 = &vector[IRQ No.] */
    ldr     r3, [r12]                            /* r3 = vector[IRQ No.] */
    mov     lr, pc
    bx      r3

/*
 *    Function Name : knl_int_tckhdr
 *    Create Date   : 2009/12/27-2014/2/28
 *    Author        : wangshb
 *    Description   : Hardware timer interrrupt/exception handler
 *                     for calling high level language routine(knl_tck_handler)
 *    Param         : none
 *    Return Code   : none
 */
    SECTION .text:CODE
    ARM
    ALIGNROM 2
    PUBLIC  knl_int_tckhdr
knl_int_tckhdr:
    /* IRQ mode/During interrupt disable CPSR.I=1 F=? */
    msr     cpsr_c, #TK_PSR_IRQ|TK_PSR_DI        /* Move to IRQ mode/Interrupt disable */

    stmfd   sp!, {r0-r2, r4-r5, lr}              /* Register save */

    ldr     r4, =knl_tsk_independent             /* Enter task independent part */
    ldr     r5, [r4]
    add     r0, r5, #1
    str     r0, [r4]

    /* Return by interrupt disable CPSR.I=1 F=1 */
    ldr     r0, =knl_tck_handler
    mov     lr, pc
    bx      r0                                   /* call tick handler() */

    str     r5, [r4]                             /* Leave task independent part */

    ldmfd   sp!, {r0-r2, r4-r5, lr}              /* Register restore */

    swp     r3, lr, [sp]                         /* r3 restore, r14_irq save */
    bl      tk_ret_int_impl

/*
 *    Function Name : tk_ret_int_impl
 *    Create Date   : 2009/12/27-2014/5/1
 *    Author        : wangshb
 *    Description   : Return from Interrupt Handler
 *    Param         : none
 *    Return Code   : none
 */
    SECTION .text:CODE
    ARM
    ALIGNROM 2
    PUBLIC  tk_ret_int_impl
tk_ret_int_impl:
    mrs     r12, cpsr
    and     lr, r12, #TK_PSR_SYS
    cmp     lr, #TK_PSR_SVC
    beq     l_retsvc                             /* Is it 'tk_ret_int' from SWI? */

                                                 /* from IRQ/FIQ mode */
    msr     cpsr_c, #TK_PSR_SVC|TK_PSR_DI
    /* Save 'r2, lr' for work (Save 'r3-' for acquiring location) */
    stmfd   sp!, {r4, r5, r6}
    stmfd   sp!, {lr}
    stmfd   sp!, {r2}
    add     r2, sp, #4                           /* r2 -> lr_svc */
    msr     cpsr_c, r12

    orr     r12, r12, #TK_PSR_DI
    bic     r12, r12, #TK_PSR_T
    cmp     lr, #TK_PSR_FIQ
    msr     cpsr_c, r12                          /* Move to IRQ/FIQ mode/Interrupt disable */
    /*  r2=ssp, sp=isp*/

    ldmfd   sp!, {r12, lr}                       /* Copy from 'isp' to 'ssp'/Free 'isp' */
                                                 /* trash R14_irq */
    str     lr, [r2, #1*4]                       /* SPSR_xxx */
    ldmfd   sp!, {r12, lr}
    strne   r12, [r2, #2*4]                      /* R12_xxx (except for FIQ) */
    str     lr, [r2, #3*4]                       /* R14_xxx (Return address) */

    msr     cpsr_c, #TK_PSR_SVC|TK_PSR_DI        /* Move to SVC mode/Interrupt disable */

    ldmfd   sp!, {r2}                            /* r2 restore */
    b       l_retint

l_retsvc:
    msr     cpsr_c, #TK_PSR_SVC|TK_PSR_DI        /* Interrupt disable */

l_retint:

    ldr     r12, =knl_tsk_independent            /* Is it a nesting interrupt? */
    ldr     r12, [r12]
    cmp     r12, #0
    bne     l_nodsp

    ldr     r12, =knl_tsk_dspdisabled            /* Is it during dispatch disable? */
    ldr     r12, [r12]
    cmp     r12, #0
    bne     l_nodsp

    ldr     r12, [sp, #4]                        /* SPSR */
    tst     r12, #TK_PSR_I|TK_PSR_F              /* Is it an exception during interrupt disable? */
    bne     l_nodsp

    ldr     r12, =knl_tsk_execution              /* Is dispatch required? */
    ldr     lr, =knl_tsk_schedule
    ldr     r12, [r12]
    ldr     lr, [lr]
    cmp     r12, lr
    bne     knl_dsp_retint                       /* To dispatch processing */

l_nodsp:
    ldmfd    sp!, {lr}                           /* lr restore */
    ldmfd    sp!, {r12}                          /* below code means EXC_RETURN */
    msr      spsr_fsxc, r12
    ldmfd    sp!, {r12, pc}^
    
    END
