/*
 *----------------------------------------------------------------------------------------
 *    Kernel/Cpu
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
 *    File Name      : knl_cpu.h
 *    Create Date    : 2012/7/9-2014/2/13
 *    Author         : wangshb
 *    Description    : Initializate/Finalizate CPU routine head file.
 *----------------------------------------------------------------------------------------
 */

#ifndef _KNL_CPU_H__
#define _KNL_CPU_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef Assembler

/*
 *    Description   : CPU-dependent initialization,
 *                    Register exception handler used on OS
 *                    and Set exception priority and ram vector.
 *    Param         : none
 *    Return Code   : ER ercd: E_OK is success.
 */
extern ER knl_cpu_init( void );

/*
 *    Description   : CPU-dependent finalization.
 *    Param         : none
 *    Return Code   : none
 */
extern void knl_cpu_shutdown( void );

#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* _KNL_CPU_H__ */
