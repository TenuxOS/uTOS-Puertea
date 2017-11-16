/*
 *----------------------------------------------------------------------------------------
 *    Kernel/dsp
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
 *    CPU:        ARM7TDMI
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : knl_dsp.h
 *    Create Date    : 2009/12/27-2014/2/25
 *    Author         : WangShb
 *    Description    : CPU-Dependent dispatch Definition head file.
 *----------------------------------------------------------------------------------------
 */

#ifndef _KNL_DSP_H__
#define _KNL_DSP_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef Assembler

/*
 * Dispatcher hook exec and stop function return address
 */
extern FP knl_dsp_hokexecret;
extern FP knl_dsp_hokstopret;

/*
 *    Description   : Normal dispatch processing.
 *                    Called by knl_dsp_signal().
 *    Param         : void
 *    Return Code   : void
 */
extern void knl_dsp_entry( void );

/*
 *    Description   : Throw away the current task context and forcibly
 *                    dispatch to the task that should be performed next.
 *                    Use at system startup and 'tk_ext_tsk, tk_exd_tsk.'
 *    Param         : void
 *    Return Code   : void
 */
extern void knl_dsp_force( void );

/*
 *    Description   : Request for task dispatcher startup.
 *                    Do nothing at this point because there is no delayed interrupt
 *                    function in ARM.
 *                    Perform dispatcher startup with END_CRITICAL_SECTION.
 *    Param         : void
 *    Return Code   : void
 */
Inline void knl_dsp_request( void )
{
}

/*
 *    Description   : Called when dispatch is required by 'tk_ret_int().'
 *    Param         : void
 *    Return Code   : void
 */
extern void knl_dsp_retint( void );

/*
 *    Description   : Signal task dispatcher
 *    Param         : void
 *    Return Code   : void
 */
Inline void knl_dsp_signal( void )
{
    knl_dsp_entry();
}

#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* _KNL_DSP_H__ */
