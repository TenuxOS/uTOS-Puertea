/*
 *----------------------------------------------------------------------------------------
 *    Kernel/pow
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
 *    File Name      : knl_pow.c
 *    Create Date    : 2012/7/9-2014/5/6
 *    Author         : WangShb
 *    Description    : Cpu power routine Function.
 *----------------------------------------------------------------------------------------
 */

#include "knl_pow.h"

#if TK_USE_POW_DEEP
#include "tm_pow.h"
#endif

#ifdef KNL_CFG_USE_FUNC_POW_DISCNT
/*
 * Number of times for disabling power-saving mode switch
 *  If it is 0, the mode switch is enabled.
 */
uint32_t knl_pow_discnt = 0u;
#endif /* KNL_CFG_USE_FUNC_POW_DISCNT */

/*
 *    Function Name : knl_pow_low
 *    Create Date   : 2009/12/27-2014/5/6
 *    Author        : wangshb
 *    Description   : Switch to power-saving mode, then wakeup through exception.
 *    Param         : void
 *    Return Code   : void
 */
void knl_pow_low( void )
{
#if TK_USE_MESSAGE
    tm_putstring((uint8_t *)&"low power enter into sleep state\n");
#endif

#if TK_USE_POW_DEEP
    tm_pow_lowenter();
#endif

    knl_tck_restart();
    __WFI();
    knl_tck_restore();

#if TK_USE_POW_DEEP
    tm_pow_lowleave();
#endif

#if TK_USE_MESSAGE
    tm_putstring((uint8_t *)&"low power leave from sleep state\n");
#endif
}

/*
 *    Function Name : knl_pow_off
 *    Create Date   : 2009/12/27-2014/5/4
 *    Author        : wangshb
 *    Description   : Switch to suspend mode, then wakeup through specail exception.
 *    Param         : void
 *    Return Code   : void
 */
void knl_pow_off( void )
{
#if TK_USE_MESSAGE
    tm_putstring((uint8_t *)&"off power enter into deep sleep state\n");
#endif

#if TK_USE_POW_DEEP
    tm_pow_offenter();
    __WFI();
    tm_pow_offleave();
#endif

#if TK_USE_MESSAGE
    tm_putstring((uint8_t *)&"off power leave from deep sleep state\n");
#endif
}
