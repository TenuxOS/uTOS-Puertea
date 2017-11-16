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
 *    File Name      : tm_tmr.c
 *    Create Date    : 2014/5/20-2014/5/31
 *    Author         : wangShb
 *    Description    : Timer Counter routine function.
 *----------------------------------------------------------------------------------------
 */

#include <stm32l1xx.h>
#include "tm_tmr.h"

#if TK_USE_SYSTICK_LESS                          /* Use Timer as uT/OS tick source */

#define TM_TMR_RELOAD_MSK    0xFFFFFFFFU         /* Tim 32bit max reload value */

Zeroinit uint32_t tm_tmr_clkdiv;                 /* Dividing rate of timer clock */
Zeroinit uint32_t tm_tmr_maxperiod;              /* Max possible suppressed period(ms) */
Zeroinit uint32_t tm_tmr_extperiod;              /* Extend CPU sleep period(ms) */
Zeroinit uint32_t tm_tmr_load;                   /* Reload value for one tick period */

/*
 *    Function Name : tm_tmr_clrint
 *    Create Date   : 2014/5/20-2014/5/31
 *    Author        : wangshb
 *    Description   : Clear timer interrupt at the beginning of the timer handler.
 *                    Depending on the type of hardware.
 *    Param         : void
 *    Return Code   : void
 */
void tm_tmr_clrint( void )
{
    /* Clear update interrupt flag */
    TIM2->SR &= ~(TIM_SR_UIF | TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF );
}

/*
 *    Function Name : tm_tmr_endofint
 *    Create Date   : 2014/5/20-2014/5/31
 *    Author        : wangshb
 *    Description   : Clear timer interrupt at the end of the timer handler.
 *                    Depending on the type of hardware.
 *    Param         : void
 *    Return Code   : void
 */
void tm_tmr_endofint( void )
{
}

/*
 *    Function Name : tm_tmr_getnsec
 *    Create Date   : 2014/5/20-2014/5/31
 *    Author        : wangshb
 *    Description   : Get processing time from the previous timer interrupt to the current
 *                    (nanosecond), Consider the possibility that the timer interrupt
 *                    occurred during the interrupt disable and calculate the processing
 *                    time within the following
 *                    range: 0 <= Processing time < KNL_CFG_TIMER_PERIOD * 2
 *    Param         : none
 *    Return Code   : uint32_t: processing time(nanosecond)
 */
uint32_t tm_tmr_getnsec( void )
{
    uint32_t ofs, max, unf;
    uint32_t imask;

    /* Disable interrupt */
    imask = __get_PRIMASK();
    __set_PRIMASK( 0x1u );

    max = ( TIM2->ARR & TM_TMR_RELOAD_MSK ) + 1u;
    do {
        unf = TIM2->SR & TIM_SR_UIF;
        ofs = TIM2->CNT & TM_TMR_RELOAD_MSK;
    } while ( unf != ( TIM2->SR & TIM_SR_UIF ) );
    if ( unf != 0u ) {
        ofs += max;
    }

    /* Enable interrupt */
    __set_PRIMASK( imask );

    ofs = ofs * 1000u * tm_tmr_clkdiv / KNL_CFG_TIMER_CLOCK;
    return ofs;
}

/*
 *    Function Name : tm_tmr_restore
 *    Create Date   : 2014/5/20-2014/5/31
 *    Author        : wangshb
 *    Description   : Reload and startup hardware timer and enable periodical interrupt.
 *    Param         : void
 *    Return Code   : void
 */
RELTIM tm_tmr_restore(void )
{
    uint32_t  imask, unf, load;
    RELTIM  period = 0;

    if ( tm_tmr_extperiod > KNL_CFG_TIMER_PERIOD ) {
        /* Caculate reload value for period */
        load = ( tm_tmr_extperiod * (KNL_CFG_TIMER_CLOCK * 1000u) / tm_tmr_clkdiv ) -1u;

        /* Disable interrupt */
        imask = __get_PRIMASK();
        __set_PRIMASK( 0x1u );

        /* Stop timer */
        TIM2->CR1 &= ~TIM_CR1_CEN;

        unf = TIM2->SR & TIM_SR_UIF;

        if ( unf > 0u ) {                            /* Wakeup by count finish */
            /* Clear the timer interrupt wether occurred during calculate the processing time */
            TIM2->SR &= ~(TIM_SR_UIF | TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF );

            period = tm_tmr_extperiod;
        } else {                                     /* Wakeup by Interrupt */
            period = ( load + 1u - ( TIM2->CNT & TM_TMR_RELOAD_MSK ) ) / tm_tmr_load;
            period *= KNL_CFG_TIMER_PERIOD;
        }
        TIM2->ARR   = tm_tmr_load;                   /* Set new reload register */

        /* Start timer count */
        TIM2->CR1 |= TIM_CR1_CEN;
        
        /* Enable interrupt */
        __set_PRIMASK( imask );
    }
    
    return period;
}

/*
 *    Function Name : tm_tmr_restart
 *    Create Date   : 2014/5/20-2014/5/31
 *    Author        : wangshb
 *    Description   : Reload by period and startup hardware timer and enable periodical 
 *                    interrupt.
 *    Param         : RELTIM period£ºnext tick event relative time
 *    Return Code   : void
 */
void tm_tmr_restart( RELTIM period )
{
    uint32_t  ofs, imask, load, unf;

    if ( period <= ( KNL_CFG_TIMER_PERIOD * 2u) ) {
        tm_tmr_extperiod = KNL_CFG_TIMER_PERIOD;
        return;
    }
    
    /* Round period by KNL_CFG_TIMER_PERIOD */
    tm_tmr_extperiod = ( period / KNL_CFG_TIMER_PERIOD ) * KNL_CFG_TIMER_PERIOD;

    /* Check period limit range */
    if ( tm_tmr_extperiod > tm_tmr_maxperiod ){
        tm_tmr_extperiod = tm_tmr_maxperiod;
    }

    /* Caculate reload value for period */
    load = ( tm_tmr_extperiod * (KNL_CFG_TIMER_CLOCK * 1000u) / tm_tmr_clkdiv ) -1u;

    /* Disable interrupt */
    imask = __get_PRIMASK();
    __set_PRIMASK( 0x1u );
    
    /* Stop timer */
    TIM2->CR1 &= ~TIM_CR1_CEN;

    /* Get timer count from the previous timer interrupt to the current */
    unf = TIM2->SR & TIM_SR_UIF;
    ofs = TIM2->CNT & TM_TMR_RELOAD_MSK ;
    if ( unf != 0u ) {
        ofs += tm_tmr_load + 1u;
    }

    load -= ofs;

    /* Clear the timer interrupt wether occurred during calculate the processing time */
    TIM2->SR &= ~(TIM_SR_UIF | TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF );

    /* Reset reload value */
    TIM2->ARR = load;
 
    /* Start timer count */
    TIM2->CR1 |= TIM_CR1_CEN;

    /* Enable interrupt */
    __set_PRIMASK( imask );
}

/*
 *    Function Name : tm_tmr_start
 *    Create Date   : 2014/5/20-2014/5/31
 *    Author        : wangshb
 *    Description   : Init and startup hardware timer and enable periodical interrupt.
 *    Param         : void
 *    Return Code   : void
 */
void tm_tmr_start( void )
{
    uint32_t  imask;
    
    /* Set dividing rate for 1MHz, because TIM2 is 32bit*/
    tm_tmr_clkdiv =  16u;

    /* Caculate reload value for predefined tick period */
    tm_tmr_load = ( KNL_CFG_TIMER_PERIOD * (KNL_CFG_TIMER_CLOCK * 1000u) / 
                    tm_tmr_clkdiv ) -1u;

    /* Caculate max possible suppressed period(ms) */
    tm_tmr_maxperiod = TM_TMR_RELOAD_MSK * tm_tmr_clkdiv / (KNL_CFG_TIMER_CLOCK * 1000u );
    
    /* Round by KNL_CFG_TIMER_PERIOD */
    tm_tmr_maxperiod = ( tm_tmr_maxperiod / KNL_CFG_TIMER_PERIOD ) * KNL_CFG_TIMER_PERIOD;
 
    /* TIM2 clock enable */
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    
    /* Disable interrupt */
    imask = __get_PRIMASK();
    __set_PRIMASK( 0x1u );

    /* Stop timer */
    TIM2->CR1 &= ~TIM_CR1_CEN;

    /* Set edge align mode and up counter and enable update */
    TIM2->CR1 &= ~( TIM_CR1_DIR | TIM_CR1_CMS | TIM_CR1_UDIS);

    /* Set update request source as only overflow */
    TIM2->CR1 |= TIM_CR1_URS;
    
    /* Enable Auto-reload preload */
    TIM2->CR1 |= TIM_CR1_ARPE;
    
    /* Set prescaler value */
    TIM2->PSC = tm_tmr_clkdiv - 1u;
    
    /* Set reload value */
    TIM2->ARR = tm_tmr_load;

    /* Enable TIM2 update and triger interrupt */
    TIM2->DIER |= TIM_DIER_UIE;
    
    /* Update and Clear update interrupt flag */
    TIM2->EGR = TIM_EGR_UG;
    TIM2->SR &= ~TIM_SR_UIF;
    
    /* Start timer count */
    TIM2->CR1 |= TIM_CR1_CEN;

    /* Set timer interrupt priority and mode, then enable */
    NVIC_SetPriority((IRQn_Type)TM_TMR_TIMER_IRQN, 0xeeu);
    NVIC_EnableIRQ((IRQn_Type)TM_TMR_TIMER_IRQN);

    /* Enable interrupt */
    __set_PRIMASK( imask );
}

/*
 *    Function Name : tm_tmr_terminate
 *    Create Date   : 2014/5/20-2014/5/31
 *    Author        : wangshb
 *    Description   : Stop hardware timer.Called when system stops.
 *    Param         : void
 *    Return Code   : void
 */
void tm_tmr_terminate( void )
{
    /* Stop timer count */
    TIM2->CR1 &= ~TIM_CR1_CEN;
    
    /* Timer interrupt disable */
    NVIC_DisableIRQ((IRQn_Type)TM_TMR_TIMER_IRQN);
}

#endif /* TK_USE_SYSTICK_LESS */
