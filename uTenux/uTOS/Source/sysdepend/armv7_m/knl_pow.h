/*
 *----------------------------------------------------------------------------------------
 *    Kernel/pow
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
 *    CPU:        CORTEX M3
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : knl_pow.h
 *    Create Date    : 2012/7/9-2014/5/1
 *    Author         : WangShb
 *    Description    : Cpu power routine head file.
 *----------------------------------------------------------------------------------------
 */

#ifndef _KNL_POW_H__
#define _KNL_POW_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef Assembler

/* Maximum number for disabling */
#define KNL_POW_LOW_LIMIT    0x7FFFU

/*
 * Number of times for disabling power-saving mode switch
 *  If it is 0, the mode switch is enabled.
 */
extern uint32_t knl_pow_discnt;

/*
 *    Description   : Switch to power-saving mode.
 *    Param         : void
 *    Return Code   : void
 */
extern void knl_pow_low( void );
/*
 *    Description   : Move to suspend mode.
 *    Param         : void
 *    Return Code   : void
 */
extern void knl_pow_off( void );

#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* _KNL_POW_H__ */
