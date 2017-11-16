/*
 *----------------------------------------------------------------------------------------
 *    TM
 *
 *    Copyright (C) 2008-2013 by Dalian uLoong Co.,Ltd. All rights reserved.
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
 *    File Name      : tm_bsp.c
 *    Create Date    : 2014/05/07-2014/05/25
 *    Author         : Cuihl-Cuihl
 *    Description    : Power entry routine about chip.
 *----------------------------------------------------------------------------------------
 */

#include <tm_bsp.h>
#include <tm_pow.h>

/*
 *    Function Name : tm_pow_lowenter
 *    Create Date   : 2014/05/07-2014/05/07
 *    Author        : Cuihl-Cuihl
 *    Description   : enter sleep mode preprocess function.
 *    Param         : none
 *    Return Code   : none
 */
void tm_pow_lowenter(void)
{
    return;
}

/*
 *    Function Name : tm_pow_lowleave
 *    Create Date   : 2014/05/07-2014/05/07
 *    Author        : Cuihl-Cuihl
 *    Description   : leave sleep mode postprocess function.
 *    Param         : none
 *    Return Code   : none
 */
void tm_pow_lowleave(void)
{
    return;
}

/*
 *    Function Name : tm_pow_offenter
 *    Create Date   : 2014/05/07-2014/05/25
 *    Author        : Cuihl-Cuihl
 *    Description   : enter stop mode preprocess function.
 *    Param         : none
 *    Return Code   : none
 */
void tm_pow_offenter(void)
{
    uint32_t tmpreg = 0u;
    uint32_t count;
    
    for(count=500u;count>0u;count--){
        ;
    }
  
    /* Select the regulator state in STOP mode ---------------------------------*/
    tmpreg = PWR->CR;

    /* Clear PDDS and LPDSR bits */
    tmpreg &= 0xfffffffcu;
  
    /* Set LPDSR bit according to PWR_Regulator value */
    tmpreg |= 0x0001u;
  
    /* Store the new value */
    PWR->CR = tmpreg;
  
    /* Set SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR |= SCB_SCR_SLEEPDEEP;
}

/*
 *    Function Name : tm_pow_offleave
 *    Create Date   : 2014/05/07-2014/05/07
 *    Author        : Cuihl-Cuihl
 *    Description   : leave stop mode postprocess function.
 *    Param         : none
 *    Return Code   : none
 */
void tm_pow_offleave(void)
{
    /* Reset SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP);
    
    tm_initrcc();
}
