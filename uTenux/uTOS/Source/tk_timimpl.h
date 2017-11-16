/*
 *----------------------------------------------------------------------------------------
 *    Kernel/tim
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
 *    File Name      : tk_timimpl.h
 *    Create Date    : 2012/7/9-2014/5/4
 *    Author         : WangShb
 *    Description    : uT/OS Time Management Header definition .
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_TIMIMPL_H__
#define __TK_TIMIMPL_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 *    Description   : Get Next Tick Event Relative Time
 *    Param         : void
 *    Return Code   : RELTIM : next tick event relative time
 */
extern RELTIM tk_get_etm_impl( void );

/*
 *    Description   : Get Operating Time
 *    Param         : SYSTIM *pk_tim : Address of packet for returning operating time
 *    Return Code   : ER : Error code
 */
extern ER tk_get_otm_impl( SYSTIM *pk_tim );

/*
 *    Description   : Get Time
 *    Param         : SYSTIM *pk_tim : Address of a packet to return current time
 *    Return Code   : ER : Error code
 */
extern ER tk_get_tim_impl( SYSTIM *pk_tim );

/*
 *    Description   : Set Time
 *    Param         : SYSTIM *pk_tim : Address of current time packet
 *    Return Code   : ER : Error code
 */
extern ER tk_set_tim_impl( SYSTIM *pk_tim );

#if TK_USE_DBGSPT

/*
 *    Description   : Get System Operating Time
 *    Param         : SYSTIM *tim : Address of packet for returning operating time (ms)
 *                    uint32_t *ofs : Location for returning elapsed time from tim (ns)
 *    Return Code   : ER : Error code
 */
extern ER td_get_otm_impl( SYSTIM *tim, uint32_t *ofs );

/*
 *    Description   : Get System Time
 *    Param         : SYSTIM *tim : Address of packet for returning current time (ms)
 *                    uint32_t *ofs : Location for returning elapsed time from tim (ns)
 *    Return Code   : ER : Error code
 */
extern ER td_get_tim_impl( SYSTIM *tim, uint32_t *ofs );

#endif /* TK_USE_DBGSPT */

#ifdef __cplusplus
}
#endif

#endif /* __TK_TIMIMPL_H__ */
