/*
 *----------------------------------------------------------------------------------------
 *    Include/tk
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
 *    File Name      : tk_kernel.h
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS general definition .
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_KERNEL_H__
#define __TK_KERNEL_H__

#include <tk_machine.h>

#ifndef Assembler
#include <stdbool.h>                             /* Defined by C compiler */
#include <stddef.h>                              /* Defined by C compiler */
#include <stdint.h>                              /* Defined by C compiler */
#include <string.h>                              /* Defined by C compiler */
#endif

#include <tk_typedef.h>
#include <tk_config.h>
#include <tk_errno.h>
#include <tk_syscall.h>

#endif /* __TK_KERNEL_H__ */
