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
 *    CPU:        NONE
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tk_intimpl.h
 *    Create Date    : 2012/7/9-2014/2/28
 *    Author         : WangShb
 *    Description    : uT/OS Interrupt Management Header definition .
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_INTIMPL_H__
#define __TK_INTIMPL_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#if TK_USE_INT_DEFHDR

/*
 * High level programming language interrupt handler table
 */
extern FP knl_int_hllhdr[TK_RAM_VECTOR_NUMBER];

/*
 *    Description   : Interrupt handler definition
 *    Param         : uint32_t dintno : Interrupt definition number
 *                    T_DINT *pk_dint : Packet of interrupt handler definition information
 *    Return Code   : ER : Error code
 */
extern ER tk_def_int_impl( uint32_t dintno, T_DINT *pk_dint );

#endif /* TK_USE_INT_DEFHDR */

#ifdef __cplusplus
}
#endif

#endif /* __TK_INTIMPL_H__ */
