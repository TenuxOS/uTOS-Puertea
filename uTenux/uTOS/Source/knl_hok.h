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
 *    CPU:        NONE
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : knl_hok.h
 *    Create Date    : 2013/12/22-2014/2/13
 *    Author         : wangshb
 *    Description    : Hook routine head file.
 *----------------------------------------------------------------------------------------
 */

#ifndef _KNL_HOK_H__
#define _KNL_HOK_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef Assembler

/*
 * Dispatcher hook function enter and exit entry
 */
extern FP knl_hok_dspexecjmp;
extern FP knl_hok_dspstopjmp;

/*
 * Interrupt hook function enter and exit entry
 */
extern FP knl_hok_intenterjmp;
extern FP knl_hok_intleavejmp;

/*
 * System service call hook function enter and exit entry
 */
extern FP knl_hok_svcenterjmp;
extern FP knl_hok_svcleavejmp;

/*
 *    Description   : Set/Free task dispatcher hook routine
 *                    called by td_hok_dsp_impl
 *    Param         : none
 *    Return Code   : none
 */
extern void knl_hok_dsp( void );
extern void knl_hok_undsp( void );

/*
 *    Description   : Set/Free interrupt handler hook routine
 *    Param         : void
 *    Return Code   : void
 */
extern void knl_hok_int( void );
extern void knl_hok_unint( void );

/*
 *    Description   : Set/Free system call/extended SVC hook routine
 *                    called by td_hok_svc_impl
 *    Param         : void
 *    Return Code   : void
 */
extern void knl_hok_svc( void );
extern void knl_hok_unsvc( void );

/*
 *    Description   : Debugger support function service call entry
 *                    called by td_*_* function entry
 *    Param         : void
 *    Return Code   : void
 */
extern void knl_hok_tdentry( void );

/*
 *    Description   : System call entry,Do not need to save the temporary register.
 *                    called by tk_*_* function entry
 *    Param         : void
 *    Return Code   : void
 */
extern void knl_hok_tkentry( void );

/*
 *    Description   : Unsupported system call
 *    Param         : void
 *    Return Code   : void
 */
extern void knl_hok_unspt( void );

#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* _KNL_HOK_H__ */
