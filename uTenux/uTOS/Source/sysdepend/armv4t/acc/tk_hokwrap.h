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
 *    CPU:        ARM7TDMI Compiled by KEIL RVMDK
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tk_hokwrap.h
 *    Create Date    : 2009/12/27-2014/5/1
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
    EXTERN  knl_hok_tkentry
    EXTERN  knl_hok_tdentry

    MACRO 
    KNL_HOK_TKWRAPER_MACRO $name, $code
    AREA |.text|,CODE,READONLY
    ARM
    ALIGN 4
    GLOBAL $name [CODE]
$name
    stmfd   sp!, {r4}
    add     r4, sp, #4
    stmfd   sp!, {lr}
    ldr     r12, =$code
    bl      knl_hok_tkentry
    ldmfd   sp!, {lr}
    ldmfd   sp!, {r4}
    bx      lr
    MEND

    MACRO
    KNL_HOK_TDWRAPER_MACRO $name, $code
    AREA |.text|,CODE,READONLY
    ARM
    ALIGN 4
    GLOBAL $name [CODE]
$name
    stmfd   sp!, {lr}
    ldr     r12, =$code
    bl      knl_hok_tdentry
    ldmfd   sp!, {lr}
    bx      lr
    MEND
#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* _TK_HOKWRAP_H__ */
