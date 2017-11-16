/*
 *----------------------------------------------------------------------------------------
 *    Kernel/bit
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
 *    CPU:        NONE
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : knl_bitop.h
 *    Create Date    : 2012/9/4-2014/1/3
 *    Author         : wangshb
 *    Description    : bit operation routine head file.
 *----------------------------------------------------------------------------------------
 */

#include "knl_kernel.h"

#ifndef _KNL_BIT_H__
#define _KNL_BIT_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef Assembler

/*
 *    Description   : clear specified bit
 *    Param         : VP base: pointer to source
 *                    int32_t offset: specified bit offset
 *    Return Code   : none
 */
extern void knl_bit_clr( VP base, int32_t offset );
/*
 *    Description   : set specified bit
 *    Param         : VP base: pointer to source
 *                    int32_t offset: specified bit offset
 *    Return Code   : none
 */
extern void knl_bit_set( VP base, int32_t offset );
/*
 *    Description   : perform 1 search on bit string
 *    Param         : VP base:  pointer to source
 *                    int32_t offset: specified bit offset
 *                    int32_t width:  max search bit number
 *    Return Code   : int32_t position: position of bit '1'
 */
extern int32_t knl_bit_search( VP base, int32_t offset, int32_t width );

#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* _KNL_BIT_H__*/
