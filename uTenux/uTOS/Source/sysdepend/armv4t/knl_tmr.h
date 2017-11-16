/*
 *----------------------------------------------------------------------------------------
 *    Kernel/tmr
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
 *    File Name      : knl_tmr.h
 *    Create Date    : 2009/12/27--2014/5/6
 *    Author         : wangShb
 *    Description    : Hardware Timer routine head file.
 *----------------------------------------------------------------------------------------
 */

#ifndef _KNL_TMR_H__
#define _KNL_TMR_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef Assembler

#if TK_USE_SYSTICK_LESS                          /* Use External timer as tick source */

#include "tm_tmr.h"

/* 
 * Set hardware timer interrupt used for source of OS tick 
 */
#define KNL_TMR_TIMER_IRQN   TM_TMR_TIMER_IRQN

/*
 * Settable interval range (millisecond)
 */
#define KNL_TMR_PERIOD_MIN   TM_TMR_PERIOD_MIN
#define KNL_TMR_PERIOD_MAX   TM_TMR_PERIOD_MAX

/*
 *    Description   : Clear timer interrupt at the beginning of the timer handler.
 *                    Depending on the type of hardware.
 *    Param         : void
 *    Return Code   : void
 */
#define knl_tmr_clrint       tm_tmr_clrint

/*
 *    Description   : Clear timer interrupt at the end of the timer handler.
 *                    Depending on the type of hardware.
 *    Param         : void
 *    Return Code   : void
 */
#define knl_tmr_endofint     tm_tmr_endofint

/*
 *    Description   : Get processing time from the previous timer interrupt to the current
 *                    (nanosecond), Consider the possibility that the timer interrupt
 *                    occurred during the interrupt disable and calculate the processing
 *                    time within the following
 *                    range: 0 <= Processing time < KNL_CFG_TIMER_PERIOD * 2
 *    Param         : none
 *    Return Code   : uint32_t: processing time(nanosecond)
 */
#define knl_tmr_getnsec      tm_tmr_getnsec

/*
 *    Description   : Reload and startup hardware timer and enable periodical interrupt.
 *    Param         : void
 *    Return Code   : void
 */
#define knl_tmr_restore      tm_tmr_restore

/*
 *    Description   : Reload by period and startup hardware timer and enable periodical 
 *                    interrupt.
 *    Param         : RELTIM period£ºnext tick event relative time
 *    Return Code   : void
 */
#define knl_tmr_restart      tm_tmr_restart

/*
 *    Description   : Init and startup hardware timer and enable periodical interrupt.
 *    Param         : none
 *    Return Code   : none
 */
#define knl_tmr_start        tm_tmr_start

/*
 *    Description   : Stop hardware timer.Called when system stops.
 *    Param         : none
 *    Return Code   : none
 */
#define knl_tmr_terminate    tm_tmr_terminate

#endif /* TK_USE_SYSTICK_LESS */

#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* _KNL_TMR_H__ */
