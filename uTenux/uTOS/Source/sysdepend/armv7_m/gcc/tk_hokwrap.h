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
 *    CPU:        CORTEX M3 Compiled by uLoong uT/STUDIO
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tk_hokwrap.h
 *    Create Date    : 2009/12/27-2014/4/1
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
    .extern knl_hok_tkentry
    .extern knl_hok_tdentry

    .macro KNL_HOK_TKWRAPER_MACRO name, code
    .text
    .syntax unified
    .balign 2
    .global \name
    .type   \name,function
\name:
    push    {r4}
    add     r4, sp, #4
    push    {lr}
    ldr     r12, =\code
    bl      knl_hok_tkentry
    pop     {lr}
    pop     {r4}
    bx      lr
    .endm

    .macro KNL_HOK_TDWRAPER_MACRO name, code
    .text
    .syntax unified
    .balign 2
    .global \name
    .type   \name,function
\name:
    push    {r4}
    add     r4, sp, #4
    push    {lr}
    ldr     r12, =\code
    bl      knl_hok_tdentry
    pop     {lr}
    pop     {r4}
    bx      lr
    .endm
#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* _TK_HOKWRAP_H__ */
