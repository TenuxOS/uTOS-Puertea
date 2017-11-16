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
 *    CPU:        STM32F103RB
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tm_bsp.c
 *    Create Date    : 2012/08/28-2017/11/02
 *    Author         : Wangshb-wangshb
 *    Description    : BSP entry routine about chip.
 *----------------------------------------------------------------------------------------
 */

#include <tm_bsp.h>

/*
 * Declaration of local function prototype  
 */
static void tm_initflash(void);
static void tm_initwdg(void);
static void tm_inituart(void);

/*
 *    Function Name : tm_initflash
 *    Create Date   : 2012/09/04-2014/03/05
 *    Author        : wangshb-Liuyf
 *    Description   : init flash,set prefetch cycle as 2 and enable
 *    Param         : none
 *    Return Code   : none
 */
static void tm_initflash(void)
{
    /* Set flash prefetch cycle as 2 and Enable */
    FLASH->ACR =( FLASH->ACR & FLASH_ACR_LATENCY ) | FLASH_ACR_LATENCY_2;

    FLASH->ACR |= FLASH_ACR_PRFTBE;

    return;
}

/*
 *    Function Name : tm_initrcc
 *    Create Date   : 2012/09/04-2017/11/02
 *    Author        : wangshb-wangshb
 *    Description   : init rcc ,use 8MHZ HSI as input and set SYSCLK/HCLK/PCLK2 as 48MHz,
 *                    PCLK1 as 24MHz.
 *    Param         : none
 *    Return Code   : none
 */
void tm_initrcc(void)
{
    SystemInit();

    return;
}

/*
 *    Function Name : tm_initwdg
 *    Create Date   : 2012/09/04-2013/03/05
 *    Author        : wangshb-Liuyf
 *    Description   : init watch dog
 *    Param         : none
 *    Return Code   : none
 */
static void tm_initwdg(void)
{
    return;
}

/*
 *    Function Name : tm_inituart
 *    Create Date   : 2012/09/04-2017/11/02
 *    Author        : wangshb-wangshb
 *    Description   : init uart as Asynchronous Mode,
 *                    115200bps, 8bit, non-parity,1 stop bit.
 *    Param         : none
 *    Return Code   : none
 */
static void tm_inituart(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
   /* GPIOA and AFIO clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); 
    
    /* USART2 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    
    /* set PA2/PA3 to Alternate function for USART2 transmit/receive */
    GPIO_PinRemapConfig(GPIO_Remap_USART2,DISABLE); 

    /* USART2's PA2/PA3 port set, Tx as alternate function push-pull, Rx as alternate function floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);      
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 

    /* USART2 init */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    /* Enable USART */
    USART_Cmd(USART2, ENABLE);

    return;
}

/*
 *    Function Name : tm_initbsp
 *    Create Date   : 2012/09/04-2014/03/05
 *    Author        : wangshb-Liuyf
 *    Description   : Init bsp,for example clock and uart etc.
 *    Param         : void
 *    Return Code   : void
 */
void tm_initbsp ( void )
{
    tm_initflash();
    tm_initrcc();
    tm_initwdg();
    tm_inituart();
}

/*
 *    Function Name : tm_senduart
 *    Create Date   : 2012/9/5-2017/11/02
 *    Author        : wangshb-wangshb
 *    Description   : send a char to uart util success
 *    Param         : uint8_t c: char to send
 *    Return Code   : none.
 */
void tm_senduart(uint8_t c)
{
    /* Wait for the transmitter to be ready while */
    while( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){
        ;
    }

    /* Send character */
    USART_SendData(USART2, (uint16_t)c);

    /* Wait for the transmitter to be ready while */
    while( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){
        ;
    }

    return;
}

/*
 *    Function Name : tm_recvuart
 *    Create Date   : 2011/9/5-2017/11/02
 *    Author        : wangshb-wangshb
 *    Description   : receive char from uart,util success
 *    Param         : none.
 *    Return Code   : uint8_t c: char to receive
 */
uint8_t tm_recvuart(void)
{
   uint8_t c;

    /* Wait for the receiver to be ready while */
    while( USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET){
         ;
    }
    
    c = (uint8_t)USART_ReceiveData(USART2);

    return c;
}
