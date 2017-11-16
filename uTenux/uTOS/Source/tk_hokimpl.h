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
 *    CPU:        NONE
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tk_hokimpl.h
 *    Create Date    : 2013/12/22-2014/5/4
 *    Author         : WangShb
 *    Description    : uT/OS Hook Header definition .
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_HOKIMPL_H__
#define __TK_HOKIMPL_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#if TK_USE_DBGSPT

/*
 *  System call entry table number and pointer (tk_ and td_ function)
 */
#define KNL_HOK_TKTBL_NUM    85                  /* tk_ function num  */
#define KNL_HOK_TDTBL_NUM    45                  /* td_ function num  */

#ifndef Assembler

extern  FP knl_hok_tksvctbl[KNL_HOK_TKTBL_NUM];  /* tk_ function entry table */
extern  FP knl_hok_tdsvctbl[KNL_HOK_TDTBL_NUM];  /* td_ function entry table */

/*
 * Hook Dsp routine address
 */
extern  FP knl_hok_dspexecfp;
extern  FP knl_hok_dspstopfp;

/*
 * Hook Int routine address
 */
extern  FP knl_hok_intenterfp;
extern  FP knl_hok_intleavefp;

/*
 * Hook SVC routine address
 */
extern  FP knl_hok_svcenterfp;
extern  FP knl_hok_svcleavefp;

/*
 *    Description   : Define Task Dispatch Hook Routine
 *    Param         : TD_HDSP *hdsp : Hook routine definition information
 *    Return Code   : ER : Error code
 */
extern ER td_hok_dsp_impl( TD_HDSP *hdsp );

/*
 *    Description   : Set/Cancel EIT handler hook routine
 *    Param         : TD_HINT *hint : Hook routine definition information
 *    Return Code   : ER : Error code
 */
extern ER td_hok_int_impl( TD_HINT *hint );

/*
 *    Description   : Define System Call/Extended SVC Hook Routine
 *    Param         : TD_HSVC *hsvc : Hook routine definition information
 *    Return Code   : ER : Error code
 */
extern ER td_hok_svc_impl( TD_HSVC *hsvc );

#endif /* Assembler */

#endif /* TK_USE_DBGSPT */

#ifdef __cplusplus
}
#endif

#endif /* __TK_HOKIMPL_H__ */
