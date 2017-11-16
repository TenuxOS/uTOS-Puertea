/*
 *----------------------------------------------------------------------------------------
 *    Kernel/hok
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
 *    File Name      : knl_hok.S
 *    Create Date    : 2013/12/22-2014/5/31
 *    Author         : wangshb
 *    Description    : CPU-Dependent Hoook Operation function.
 *----------------------------------------------------------------------------------------
 */

#include "tk_hokimpl.h"

/*
 * Imports external symbols
 */
    .extern knl_dsp_hokstopret
    .extern knl_dsp_hokexecret
    .extern knl_hok_dspexecfp
    .extern knl_hok_dspstopfp
    .extern knl_hok_intenterfp
    .extern knl_hok_intleavefp
    .extern knl_hok_svcenterfp
    .extern knl_hok_svcleavefp
    .extern knl_hok_tksvctbl
    .extern knl_hok_tdsvctbl
    .extern knl_int_hokenterret
    .extern knl_int_hokleaveret
    .extern knl_tsk_mode

#if (TK_USE_DBGSPT && TK_USE_HOOK_TRACE)
/*
 * Dispatcher hook function enter and exit entry
 */
    .data
    .balign 4
    .global knl_hok_dspexecjmp
    .global knl_hok_dspstopjmp
    .type   knl_dsp_hokexecret, function
    .type   knl_dsp_hokstopret, function
knl_hok_dspexecjmp:
    .long   knl_dsp_hokexecret
knl_hok_dspstopjmp:
    .long   knl_dsp_hokstopret

/*
 *    Function Name : knl_hok_dspexec,knl_hok_dspstop
 *    Create Date   : 2009/12/27-2014/4/1
 *    Author        : wangshb
 *    Description   : Task dispatcher high language hook routine entry
 *                       void stop( ID tskid, int32_t lsid, uint32_t tskstat )
 *                       void exec( ID tskid, int32_t lsid )
 *    Param         : none
 *    Return Code   : none
 */
    .text
    .syntax unified
    .balign 2
    .type   knl_hok_dspstop, function
knl_hok_dspstop:
    cmp     r8, #0                               /* r8 = knl_tsk_execution */
    beq     l_notask

    ldrb    r2, [r8, #KNL_TSK_TCB_STATE]         /* tskstat */
    mov     r2, r2, lsl #1
    ldr     r0, [r8, #KNL_TSK_TCB_TSKID]         /* tskid */

    push    {lr}
    ldr     lr, =knl_hok_dspstopfp
    ldr     r12, [lr]
    blx     r12                                  /* call stop(tskid, lsid, tskstat) */
    pop     {lr}

l_notask:
    b       knl_dsp_hokstopret

    .type   knl_hok_dspexec, function
knl_hok_dspexec:
                                                 /* r8 = knl_tsk_execution */
    ldr     r0, [r8, #KNL_TSK_TCB_TSKID]         /* tskid */

    push    {lr}
    ldr     r12, =knl_hok_dspexecfp
    ldr     r12, [r12]
    blx     r12                                  /* call exec(tskid, lsid) */
    pop    {lr}

    b       knl_dsp_hokexecret
    nop

/*
 *    Function Name : knl_hok_dsp,knl_hok_undsp
 *    Create Date   : 2009/12/27-2014/4/1
 *    Author        : wangshb
 *    Description   : Set/Free task dispatcher hook routine
 *                    called by td_hok_dsp_impl
 *    Param         : none
 *    Return Code   : none
 */
    .text
    .syntax unified
    .balign 2
    .global knl_hok_dsp
    .type   knl_hok_dsp, function
knl_hok_dsp:
    ldr     r0, =knl_hok_dspexecjmp
    ldr     r1, =knl_hok_dspstopjmp
    ldr     r2, =knl_hok_dspexec
    ldr     r3, =knl_hok_dspstop
    str     r2, [r0]
    str     r3, [r1]
    bx      lr

    .global knl_hok_undsp
    .type   knl_hok_undsp, function
knl_hok_undsp:
    ldr     r0, =knl_hok_dspexecjmp
    ldr     r1, =knl_hok_dspstopjmp
    ldr     r2, =knl_dsp_hokexecret
    ldr     r3, =knl_dsp_hokstopret
    str     r2, [r0]
    str     r3, [r1]
    bx      lr

#endif /* TK_USE_DBGSPT && TK_USE_HOOK_TRACE */

#if ( TK_USE_INT_DEFHDR && TK_USE_INT_HLLHDR )
#if (TK_USE_DBGSPT && TK_USE_HOOK_TRACE)
/*
 * Interrupt hook function enter and exit entry
 */
    .data
    .balign 4
    .global knl_hok_intenterjmp
    .global knl_hok_intleavejmp
    .type   knl_int_hokenterret, function
    .type   knl_int_hokleaveret, function
knl_hok_intenterjmp:
    .long   knl_int_hokenterret
knl_hok_intleavejmp:
    .long   knl_int_hokleaveret

/*
 *    Function Name : knl_hok_intenter,knl_hok_intleave
 *    Create Date   : 2009/12/27-2014/2/1
 *    Author        : wangshb
 *    Description   : Interrupt handler hook routine call
 *                      void enter( uint32_t dintno, VP ctxptr );
 *                      void leave( uint32_t dintno, VP ctxptr );
 *    Param         : none
 *    Return Code   : none
 */
    .text
    .syntax unified
    .balign 2
    .type    knl_hok_intenter, function
knl_hok_intenter:
    push    {r0, r1}                             /* Register save */
    push    {r3}

    ldr     lr,  =knl_hok_intenterfp
    ldr     r12, [lr]
    blx     r12                                  /* call enter(dintno, sp) */

    pop     {r3}                                 /* Register restore */
    ldmfd   sp, {r0, r1}                         /* Leave 'dintno,' 'sp' on stack */
    b       knl_int_hokenterret

    .type   knl_hok_intleave, function
knl_hok_intleave:
    pop     {r0, r1}                             /* Restore 'dintno,' 'sp' */

    ldr     lr, =knl_hok_intleavefp
    ldr     r12, [lr]
    blx     r12                                  /* call leave(dintno, info) */

    b       knl_int_hokleaveret

/*
 *    Function Name : knl_hok_int,knl_hok_unint
 *    Create Date   : 2009/12/27-2014/4/1
 *    Author        : wangshb
 *    Description   : Set/Free interrupt handler hook routine
 *    Param         : none
 *    Return Code   : none
 */
    .text
    .syntax unified
    .balign 2
    .global knl_hok_int
    .type   knl_hok_int, function
knl_hok_int:
    ldr     r0, =knl_hok_intenterjmp
    ldr     r1, =knl_hok_intleavejmp
    ldr     r2, =knl_hok_intenter
    ldr     r3, =knl_hok_intleave
    str     r2, [r0]
    str     r3, [r1]
    bx      lr

    .global knl_hok_unint
    .type   knl_hok_unint, function
knl_hok_unint:
    ldr     r0, =knl_hok_intenterjmp
    ldr     r1, =knl_hok_intleavejmp
    ldr     r2, =knl_int_hokenterret
    ldr     r3, =knl_int_hokleaveret
    str     r2, [r0]
    str     r3, [r1]
    bx      lr

#endif /* TK_USE_DBGSPT && TK_USE_HOOK_TRACE */
#endif /* TK_USE_INT_DEFHDR && TK_USE_INT_HLLHDR */

#if (TK_USE_DBGSPT && TK_USE_HOOK_TRACE)
/*
 * System service call hook function enter and exit entry
 */
    .data
    .balign 4
    .global knl_hok_svcenterjmp
    .global knl_hok_svcleavejmp
    .type   knl_hok_svcenterret, function
    .type   knl_hok_svcleaveret, function
knl_hok_svcenterjmp:
    .long   knl_hok_svcenterret
knl_hok_svcleavejmp:
    .long   knl_hok_svcleaveret

/*
 *    Function Name : knl_hok_svcenter,knl_hok_svcleave
 *    Create Date   : 2009/12/27-2014/4/1
 *    Author        : wangshb
 *    Description   : System call/Extended SVC hook routine call
 *                       VP enter( FN fncd, TD_CALINF *calinf, ... )
 *                       void leave( FN fncd, int32_t ret, VP exinf )
 *    Param         : none
 *    Return Code   : none
 */
    .text
    .syntax unified
    .balign 2
    .type   knl_hok_svcenter, function
knl_hok_svcenter:
    push    {r0-r3, r8-r9}                       /* Save argument and register for work */
    mov     r8, sp                               /* r8 = Keep stack position */

    ldr     r12, [r11, #-4]                      /* Frame pointer when calling */
    push    {r11, r12}                           /* Create TD_CALINF */
    mov     r9, sp                               /* r9 = &TD_CALINF */

    ldr     lr, [r11, #4]                        /* lr = Function code */
    cmp     lr, #0                               /* < 0: System call */
    bge     l_hoksvcenter                        /* >= 0: Extended SVC */

    mov     lr, lr, lsr #8
    and     lr, lr, #0xff                        /* Number of arguments */

    cmp     lr, #5
    itt     ge
    ldrge   r12, [r4]
    pushge  {r12}                                /* Fifth argument */
    cmp     lr, #4
    it      ge
    pushge  {r3}                                 /* Fourth argument */
    cmp     lr, #3
    it      ge
    pushge  {r2}                                 /* Third argument */
    mov     r3, r1                               /* Second argument */
l_hoksvcenter:
    mov     r2, r0                               /* First argument */
    mov     r1, r9                               /* calinf */
    ldr     r0, [r11, #4]                        /* fncd */
    ldr     lr, =knl_hok_svcenterfp
    ldr     r12, [lr]
    blx     r12                                  /* exinf = enter(fncd, ...) */

    mov     r9, r0                               /* Temporarily save 'exinf' */
    mov     sp, r8                               /* Return stack position */
    pop     {r0-r3, r8}                          /* Restore argument and register */
    mov     r12, r9                              /* r12 as temp register */
    pop     {r9}                                 /* 'r9' restore */
    push    {r12}                                /* 'exinf' save */
    b       knl_hok_svcenterret

    .type   knl_hok_svcleave, function
knl_hok_svcleave:
    mov     r1, r0                               /* r1 = ret */

    mov     lr, #0
    ldr     r0, [r11, #4]                        /* r0 = Function code */
    cmp     r0, #0                               /*      < 0: System call */
    bge     l_hoksvcleave                        /*    >= 0: Extended SVC */

    mov     lr, r0, lsr #8
    and     lr, lr, #0xff                        /* Number of arguments */
    subs    lr, lr, #4
    it      lt
    movlt   lr, #0
l_hoksvcleave:
    add     lr, lr, #3                           /* Whether 'hook_enter' is executed */
    sub     r12, sp, r11                         /* Check by stack usage */
    sub     r11, r11, #4*4
    mov     sp, r11                              /* Loc in which 'sp = exinf' is saved */
    cmp     lr, r12, lsr #2                      /* If 'hook_enter' is executed, */
    ite     ne
    ldrne    r2, [sp]                            /* Get 'exinf' from stack */
    ldreq    r2, =0                              /* If 'exinf' is not saved, 0 */

    str     r9, [sp]                             /* r9 save */
    mov     r9, r1                               /* Save 'ret' in 'r9' */

    ldr     lr, =knl_hok_svcleavefp
    ldr     r12, [lr]
    blx     r12                                  /* call leave(fncd, ret, exinf) */

    mov     r0, r9                               /* r0 = ret restore */
    pop     {r9}                                 /* r9 restore */
    b       knl_hok_svcleaveret

/*
 *    Function Name : knl_hok_svc,knl_hok_unsvc
 *    Create Date   : 2009/12/27-2014/4/1
 *    Author        : wangshb
 *    Description   : Set/Free system call/extended SVC hook routine
 *                    called by td_hok_svc_impl
 *    Param         : none
 *    Return Code   : none
 */
    .text
    .syntax unified
    .balign 2
    .global knl_hok_svc
    .type   knl_hok_svc, function
knl_hok_svc:
    ldr     r0, =knl_hok_svcenterjmp
    ldr     r1, =knl_hok_svcleavejmp
    ldr     r2, =knl_hok_svcenter
    ldr     r3, =knl_hok_svcleave
    str     r2, [r0]
    str     r3, [r1]
    bx      lr

    .global knl_hok_unsvc
    .type   knl_hok_unsvc, function
knl_hok_unsvc:
    ldr     r0, =knl_hok_svcenterjmp
    ldr     r1, =knl_hok_svcleavejmp
    ldr     r2, =knl_hok_svcenterret
    ldr     r3, =knl_hok_svcleaveret
    str     r2, [r0]
    str     r3, [r1]
    bx      lr

#endif /* TK_USE_DBGSPT && TK_USE_HOOK_TRACE */

#if (TK_USE_DBGSPT &&  TK_USE_HOOK_TRACE)
/*
 *    Function Name : knl_hok_tdentry
 *    Create Date   : 2009/12/27-2014/4/1
 *    Author        : wangshb
 *    Description   : Debugger support function service call entry
 *                    called by td_*_* function entry
 *    Param         : none
 *    Return Code   : none
 */
    .text
    .syntax unified
    .balign 2
    .global knl_hok_tdentry
    .type   knl_hok_tdentry, function
knl_hok_tdentry:
    push    {r10, r11, lr}                        /* Save register for work */
    mov     r11, r12                              /* lr = Function code */
    mov     r10, r11, asr #16
    ldr     r11, =KNL_HOK_TDTBL_NUM -1 + 0xffff8000
    cmp     r10, r11
    bgt     l_tdillsvc

    ldr     r11, =knl_hok_tdsvctbl - (0xffff8000 << 2)
    lsl     r10, r10, #2
    ldr     r12, [r11, r10]                      /* uT/OS service call (DS) */
    blx     r12

l_tdretsvc:
    pop     {r10, r11, lr}                       /* Restore register for work */
    bx      lr

l_tdillsvc:
    ldr     r0, =E_RSFN
    b       l_tdretsvc
    nop
#endif /* (TK_USE_DBGSPT && TK_USE_HOOK_TRACE) */

#if (TK_USE_DBGSPT && TK_USE_HOOK_TRACE)
/*
 *    Function Name : knl_hok_tkentry
 *    Create Date   : 2009/12/27-2014/5/31
 *    Author        : wangshb
 *    Description   : System call entry,Do not need to save the temporary register.
 *                    called by tk_*_* function entry
 *    Param         : none
 *    Return Code   : none
 */
    .text
    .syntax unified
    .balign 2
    .global knl_hok_tkentry
    .type   knl_hok_tkentry, function
knl_hok_tkentry:
    cmp     r12, #0                              /* < 0: System call */
                                                 /*>= 0: Extended SVC */
    bge     l_nomorestack                        /* don't create stack */
    push    {r12,lr}
    mrs     r12, psr
    push    {r12}
l_nomorestack:
    push    {r10, r11}                           /* Save register for work */
    add     r11, sp, #2*4                        /* Frame pointer to psr in stack*/

    ldr     r12, =knl_tsk_mode                   /* Task mode flag update */
    ldr     r10, [r12]
    push    {r10}                                /* taskmode save */
    mov     lr, r10, lsl #16
    str     lr, [r12]

    ldr     r12, =knl_hok_svcenterjmp            /* Hook processing */
    ldr     pc, [r12]
knl_hok_svcenterret:
    ldr     lr, [r11, #4]                        /* r12 = Function code(in stack) */
                                                 /* lr =Function code(now) */
    cmp     lr, #0                               /* < 0: System call */
    bge     l_esvc                               /* >= 0: Extended SVC */

    /* uT/OS System Call */
    mov     r10, lr, asr #16                     /* r10 = Function number */
    ldr     r12, =KNL_HOK_TKTBL_NUM -1 + 0xffff8000
    cmp     r10, r12
    bgt     l_tkillsvc

    mov     lr, lr, lsr #8
    and     lr, lr, #0xff                        /* lr = Number of arguments */
    cmp     lr, #5
    bne     l_nocopy
    ldr     r12, [r4]                            /* Copy fifth argument */
    push    {r12}
l_nocopy:
    ldr     r12, =knl_hok_tksvctbl - (0xffff8000 << 2)
    lsl     r10, r10, #2
    ldr     lr, [r12, r10]                       /* uT/OS system call */
    mov     r12, lr
    blx     r12
    mov     sp, r11
    sub     sp, sp, #3*4
l_tkretsvc:
    ldr     r12, =knl_hok_svcleavejmp            /* Hook processing */
    ldr     pc, [r12]
knl_hok_svcleaveret:
    pop     {r1, r10, r11}                       /* Restore register for work */
    ldr     r12, =knl_tsk_mode                   /* Task mode restore */
    str     r1, [r12]
    pop     {r12}
    msr     xpsr_nzcvq, r12
    pop     {r12}
    pop     {pc}

l_tkillsvc:
    ldr     r0, =E_RSFN
    b       l_tkretsvc

l_esvc:
    ldr     r0, =E_SYS
    b       l_tkretsvc
#endif /* TK_USE_DBGSPT && TK_USE_HOOK_TRACE */

#if (TK_USE_DBGSPT && TK_USE_HOOK_TRACE)
/*
 *    Function Name : knl_hok_unspt
 *    Create Date   : 2009/12/27-2014/4/1
 *    Author        : wangshb
 *    Description   : Unsupported system call
 *    Param         : none
 *    Return Code   : none
 */
    .text
    .syntax unified
    .balign 2
    .global knl_hok_unspt
    .type   knl_hok_unspt, function
knl_hok_unspt:
    ldr     r0, =E_RSFN
    str     r0, [sp]
    bx      lr
    nop
#endif /* TK_USE_DBGSPT && TK_USE_HOOK_TRACE */

    .end
