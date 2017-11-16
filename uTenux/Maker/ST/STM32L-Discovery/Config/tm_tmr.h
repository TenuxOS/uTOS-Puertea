/*
 *----------------------------------------------------------------------------------------
 *    TM/Tmr
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
 *    CPU:        STM32L152RB
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tm_tmr.h
 *    Create Date    : 2014/5/20-2014/5/31
 *    Author         : Wangshb
 *    Description    : Timer Counter routine head file.
 *----------------------------------------------------------------------------------------
 */

#ifndef _TM_TMR_H__
#define _TM_TMR_H__

#include <tk_machine.h>
#include <tk_typedef.h>
#include <tk_config_depend.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef Assembler

#if TK_USE_SYSTICK_LESS                          /* Use Timer as uT/OS tick source */

/* 
 * Set hardware timer interrupt used for source of OS tick 
 */
#define TM_TMR_TIMER_IRQN    28                  /* TIM2_IRQn = 28 */

/*
 * Settable interval range (millisecond)
 */
#define TM_TMR_PERIOD_MIN    1U
#define TM_TMR_PERIOD_MAX    50U

extern uint32_t  tm_tmr_clkdiv;                  /* Dividing rate of timer clock */

/*
 *    Description   : Clear timer interrupt at the beginning of the timer handler.
 *                    Depending on the type of hardware.
 *    Param         : void
 *    Return Code   : void
 */
extern void tm_tmr_clrint( void );

/*
 *    Description   : Clear timer interrupt at the end of the timer handler.
 *                    Depending on the type of hardware.
 *    Param         : void
 *    Return Code   : void
 */
extern void tm_tmr_endofint( void );

/*
 *    Description   : Get processing time from the previous timer interrupt to the current
 *                    (nanosecond), Consider the possibility that the timer interrupt
 *                    occurred during the interrupt disable and calculate the processing
 *                    time within the following
 *                    range: 0 <= Processing time < KNL_CFG_TIMER_PERIOD * 2
 *    Param         : none
 *    Return Code   : uint32_t: processing time(nanosecond)
 */
extern uint32_t tm_tmr_getnsec( void );

/*
 *    Description   : Reload and startup hardware timer and enable periodical interrupt.
 *    Param         : void
 *    Return Code   : void
 */
extern RELTIM tm_tmr_restore(void );

/*
 *    Description   : Reload by period and startup hardware timer and enable periodical 
 *                    interrupt.
 *    Param         : RELTIM period£ºnext tick event relative time
 *    Return Code   : void
 */
extern void tm_tmr_restart( RELTIM period );

/*
 *    Description   : Init and startup hardware timer and enable periodical interrupt.
 *    Param         : none
 *    Return Code   : none
 */
extern void tm_tmr_start( void );

/*
 *    Description   : Stop hardware timer.Called when system stops.
 *    Param         : none
 *    Return Code   : none
 */
extern void tm_tmr_terminate( void );

#endif /* !TK_USE_SYSTICK_LESS */

#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* _TM_TMR_H__ */
