/*
 *----------------------------------------------------------------------------------------
 *    TM
 *
 *    Copyright (C) 2008-2017 by Dalian uLoong Co.,Ltd. All rights reserved.
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
 *    CPU:        STM32F407VG
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tm_bsp.h
 *    Create Date    : 2012/7/9-2017/11/04
 *    Author         : Wangshb-Wangshb
 *    Description    : BSP entry Definition about chip.
 *----------------------------------------------------------------------------------------
 */

#ifndef __TM_BSP_H__
#define __TM_BSP_H__

#include <tk_machine.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RCC HSE Value                  */
#define HSE_VALUE                 8000000U       /*the External oscillator in Hz */

#ifndef Assembler

#include <stm32f4xx.h>

/*
 *    Description   : init rcc.
 *    Param         : none
 *    Return Code   : none
 */
extern void tm_initrcc(void);

/*
 *    Description   : Init bsp.
 *    Param         : void
 *    Return Code   : void
 */
extern void tm_initbsp ( void );

/*
 *    Description   : send a char to uart util success
 *    Param         : uint8_t c: char to send
 *    Return Code   : none.
 */
extern void tm_senduart(uint8_t c);

/*
 *    Description   : receive char from uart,util success
 *    Param         : none.
 *    Return Code   : uint8_t c: char to receive
 */
extern uint8_t tm_recvuart(void);

#endif

#ifdef __cplusplus
}
#endif

#endif /* __TM_BSP_H__ */
