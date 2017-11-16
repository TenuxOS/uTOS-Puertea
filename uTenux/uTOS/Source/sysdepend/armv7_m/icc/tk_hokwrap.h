/*
 *----------------------------------------------------------------------------------------
 *    Kernel/hook
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
 *    CPU:        CORTEX M3 Compiled by IAR EWARM
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tk_hokwrap.h
 *    Create Date    : 2009/12/27-2014/2/28
 *    Author         : WangShb
 *    Description    : Debug and Hook SVC interface head file.
 *----------------------------------------------------------------------------------------
 */

#ifndef _TK_HOKWRAP_H__
#define _TK_HOKWRAP_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef Assembler
    EXTERN knl_hok_tkentry
    EXTERN knl_hok_tdentry

KNL_HOK_TKWRAPER_MACRO MACRO name, code
    SECTION .text:CODE
    THUMB
    ALIGNROM 1
    PUBLIC  name
name:
    push    {r4}
    add     r4, sp, #4
    push    {lr}
    ldr     r12, =code
    bl      knl_hok_tkentry
    pop     {lr}
    pop     {r4}
    bx      lr
    nop
    ENDM

KNL_HOK_TDWRAPER_MACRO MACRO name, code
    SECTION .text:CODE
    THUMB
    ALIGNROM 1
    PUBLIC  name
name:
    push    {r4}
    add     r4, sp, #4
    push    {lr}
    ldr     r12, =code
    bl      knl_hok_tdentry
    pop     {lr}
    pop     {r4}
    bx      lr
    nop
    ENDM
#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* _TK_HOKWRAP_H__ */
