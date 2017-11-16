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
 *    CPU:        CORTEX M3
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : knl_tmr.c
 *    Create Date    : 2012/7/9-2014/5/30
 *    Author         : wangShb
 *    Description    : Systick Timer routine function.
 *----------------------------------------------------------------------------------------
 */

#include "knl_tmr.h"

#if !TK_USE_SYSTICK_LESS                         /* Use Systick as uT/OS tick source */

Zeroinit uint32_t knl_tmr_clkdiv;                /* Dividing rate of timer clock */
Zeroinit uint32_t knl_tmr_maxperiod;             /* Max possible suppressed period(ms) */
Zeroinit uint32_t knl_tmr_extperiod;             /* Extend CPU sleep period(ms) */
Zeroinit uint32_t knl_tmr_load;                  /* Reload value for one tick period */

/*
 *    Function Name : knl_tmr_start
 *    Create Date   : 2012/7/9-2014/5/30
 *    Author        : wangshb
 *    Description   : Init and startup hardware timer and enable periodical interrupt.
 *                    for Cortex M,timer is systick.
 *    Param         : void
 *    Return Code   : void
 */
void knl_tmr_start( void )
{
    uint32_t  imask;

    /* Set dividing rate to 1,because systick is 24bit*/
    knl_tmr_clkdiv =  1u;
    
    /* Caculate reload value for predefined tick period */
    knl_tmr_load = ( KNL_CFG_TIMER_PERIOD * (KNL_CFG_TIMER_CLOCK * 1000u) / 
                     knl_tmr_clkdiv ) -1u;

    /* Caculate max possible suppressed period(ms) */
    knl_tmr_maxperiod = SysTick_LOAD_RELOAD_Msk * knl_tmr_clkdiv / (KNL_CFG_TIMER_CLOCK * 1000u );
    
    /* Round by KNL_CFG_TIMER_PERIOD */
    knl_tmr_maxperiod = ( knl_tmr_maxperiod / KNL_CFG_TIMER_PERIOD ) * KNL_CFG_TIMER_PERIOD;

    /* Config systick timer */
    DI(imask);
    
    /* Check reload value is correct */
    if ( knl_tmr_load <= SysTick_LOAD_RELOAD_Msk ){
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk;
        SysTick->LOAD = knl_tmr_load;            /* set reload register */
                                                 /* Enable SysTick IRQ and Timer */
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk   |
                        SysTick_CTRL_ENABLE_Msk;
    }

    /* Set systick interrupt priority */
    NVIC_SetPriority(SysTick_IRQn, 0xeeu);
    
    EI(imask);
}

/*
 *    Function Name : knl_tmr_terminate
 *    Create Date   : 2012/7/9-2014/2/1
 *    Author        : wangshb
 *    Description   : Stop hardware timer.Called when system stops.
 *                    for Cortex M,timer is systick.
 *    Param         : void
 *    Return Code   : void
 */
void knl_tmr_terminate( void )
{
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk;
}

#endif /* !TK_USE_SYSTICK_LESS */
