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
 *    File Name      : knl_tmr.h
 *    Create Date    : 2012/7/9-2014/5/30
 *    Author         : wangShb
 *    Description    : Systick Timer routine head file.
 *----------------------------------------------------------------------------------------
 */

#ifndef _KNL_TMR_H__
#define _KNL_TMR_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef Assembler

#if !TK_USE_SYSTICK_LESS                         /* Use Systick as uT/OS tick source */

/* 
 * Set hardware timer interrupt used for source of OS tick 
 */
#define KNL_TMR_TIMER_IRQN   SysTick_IRQn

/*
 * Settable interval range (millisecond)
 */
#define KNL_TMR_PERIOD_MIN   1U
#define KNL_TMR_PERIOD_MAX   50U

extern uint32_t knl_tmr_clkdiv;                  /* Dividing rate of timer clock */
extern uint32_t knl_tmr_maxperiod;               /* Max possible suppressed period(ms) */
extern uint32_t knl_tmr_extperiod;               /* Extend CPU sleep period(ms) */
extern uint32_t knl_tmr_load;                    /* Reload value for one tick period */

/*
 *    Description   : Clear timer interrupt at the beginning of the timer handler.
 *                    Depending on the type of hardware.
 *                     for Cortex M,timer is systick.
 *    Param         : void
 *    Return Code   : void
 */
Inline void knl_tmr_clrint( void )
{
}

/*
 *    Description   : Clear timer interrupt at the end of the timer handler.
 *                    Depending on the type of hardware.
 *                     for Cortex M,timer is systick.
 *    Param         : void
 *    Return Code   : void
 */
Inline void knl_tmr_endofint( void )
{
}

/*
 *    Description   : Get processing time from the previous timer interrupt to the current
 *                    (nanosecond), Consider the possibility that the timer interrupt
 *                    occurred during the interrupt disable and calculate the processing
 *                    time within the following
 *                    range: 0 <= Processing time < KNL_CFG_TIMER_PERIOD * 2
 *                    for Cortex M,timer is systick.
 *    Param         : void
 *    Return Code   : uint32_t: processing time(nanosecond)
 */
Inline uint32_t knl_tmr_getnsec( void )
{
    uint32_t ofs, max, unf;
    uint32_t imask;

    DI(imask);

    max = (SysTick->LOAD & SysTick_LOAD_RELOAD_Msk) + 1u;
    do {
        /* get COUNTFLAG bit */
        unf = SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk;
        ofs = SysTick->VAL & SysTick_VAL_CURRENT_Msk;
    } while ( unf != (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) );
    ofs = max-ofs;
    if ( unf != 0u ) {
        ofs += max;
    }

    EI(imask);

    ofs = ofs * 1000u* knl_tmr_clkdiv / KNL_CFG_TIMER_CLOCK;
    return ofs;
}

/*
 *    Description   : Reload and startup hardware timer and enable periodical interrupt.
 *    Param         : void
 *    Return Code   : void
 */
Inline RELTIM knl_tmr_restore(void )
{
    uint32_t  imask, unf, load;
    RELTIM  period = 0;

    if ( knl_tmr_extperiod > KNL_CFG_TIMER_PERIOD ) {
        /* Caculate reload value for period */
        load = ( knl_tmr_extperiod * (KNL_CFG_TIMER_CLOCK * 1000u) / knl_tmr_clkdiv ) -1u;

        DI(imask);

        /* Get count flag */
        unf = SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk;
        
        /* Disable systick */
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

        if ( unf > 0u ) {                            /* Wakeup by count finish */
            /* Clear the timer interrupt wether occurred during calculate the processing time */
            SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;

            period = knl_tmr_extperiod;
        } else {                                     /* Wakeup by Interrupt */
            period = ( load + 1u - ( SysTick->VAL & SysTick_VAL_CURRENT_Msk ) ) / knl_tmr_load;
            period *= KNL_CFG_TIMER_PERIOD;
        }
        SysTick->LOAD  = knl_tmr_load;               /* Set new reload register */
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;    /* Enable systick */
        
        EI(imask);
    }
    
    return period;
}

/*
 *    Description   : Reload by period and startup hardware timer and enable periodical 
 *                    interrupt.
 *    Param         : RELTIM period£ºnext tick event relative time
 *    Return Code   : void
 */
Inline void knl_tmr_restart( RELTIM period )
{
    uint32_t  ofs, imask, load, unf;

    if ( period <= ( KNL_CFG_TIMER_PERIOD * 2u) ) {
        knl_tmr_extperiod = KNL_CFG_TIMER_PERIOD;
        return;
    }
    
    /* Round period by KNL_CFG_TIMER_PERIOD */
    knl_tmr_extperiod = ( period / KNL_CFG_TIMER_PERIOD ) * KNL_CFG_TIMER_PERIOD;

    /* Check period limit range */
    if ( knl_tmr_extperiod > knl_tmr_maxperiod ){
        knl_tmr_extperiod = knl_tmr_maxperiod;
    }

    /* Caculate reload value for period */
    load = ( knl_tmr_extperiod * (KNL_CFG_TIMER_CLOCK * 1000u) / knl_tmr_clkdiv ) -1u;

    DI(imask);

    /* Get count flag */
    unf = SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk;
    
    /* Disable systick */
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;   

    /* Get timer value from the previous timer interrupt to the current */
    ofs = knl_tmr_load + 1u - ( SysTick->VAL & SysTick_VAL_CURRENT_Msk );
    if ( unf != 0u ) {
        ofs += knl_tmr_load + 1u;
    }

    load -= ofs;

    /* Clear the timer interrupt wether occurred during calculate the processing time */
    if ( ( SCB->ICSR & SCB_ICSR_PENDSTSET_Msk ) !=0u ){
        SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
    }

    /* Config systick timer */
    SysTick->LOAD  = load;                       /* set new reload register */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;    /* Enable systick */

    EI(imask);
}

/*
 *    Description   : Init and Startup hardware timer and enable periodical interrupt.
 *                    for Cortex M,timer is systick.
 *    Param         : void
 *    Return Code   : void
 */
extern void knl_tmr_start( void );

/*
 *    Description   : Stop hardware timer.Called when system stops.
 *                    for Cortex M,timer is systick.
 *    Param         : void
 *    Return Code   : void
 */
extern void knl_tmr_terminate( void );

#else                                            /* Use External timer as tick source */

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

#endif /* !TK_USE_SYSTICK_LESS */

#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* _KNL_TMR_H__ */
