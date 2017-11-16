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
 *    CPU:        STM32F401RE
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tm_bsp.c
 *    Create Date    : 2012/08/28-2017/9/28
 *    Author         : Wangshb-Wangshb
 *    Description    : BSP entry routine about chip.
 *----------------------------------------------------------------------------------------
 */

#include <tm_bsp.h>

/*
 * Definition of local macro constant
 */

/*
 * Declaration of local function prototype
 */
#if (__FPU_USED == 1)
static void tm_enafpu(void);
#endif /* __FPU_USED */
static void tm_initflash(void);
static void tm_initwdg(void);
static void tm_inituart(void);

#if (__FPU_USED == 1)
/*
 *    Function Name : tm_enafpu
 *    Create Date   : 2012/10/21-2013/12/23
 *    Author        : Wangshb-Luw
 *    Description   : Active FPU.
 *    Param         : none
 *    Return Code   : none
 */
static void tm_enafpu(void)
{
    uint32_t control;

    /* Set FPU full access and active */
    SCB->CPACR |= 0xfu << 20u;                   /* set CP10 and CP11 to Full Access */
    control=__get_CONTROL() | 0x4u;
    __set_CONTROL(control);                      /* set FPCA to active FPU */
}
#endif /* __FPU_USED */

/*
 *    Function Name : tm_initflash
 *    Create Date   : 2017/9/24-2017/9/28
 *    Author        : Wangshb
 *    Description   : init flash,enable buffer and 2 wait cycles
 *    Param         : none
 *    Return Code   : none
 */
static void tm_initflash(void)
{
    /* Unlock ACR register */
    FLASH_Unlock();

    /* Init ACR */
    FLASH_PrefetchBufferCmd(ENABLE);
    FLASH_DataCacheCmd(ENABLE);
    FLASH_InstructionCacheCmd(ENABLE);
    FLASH_SetLatency(FLASH_Latency_2);
    
    /* Lock ACR register again */
    FLASH_Lock();
}

/*
 *    Function Name : tm_initrcc
 *    Create Date   : 2017/9/24-2017/9/28
 *    Author        : Wangshb
 *    Description   : init rcc ,use 16MHZ HSI as input and set SYSCLK/HCLK as 84MHz,
 *                    PCLK2 as 84MHz,PCLK1 as 42MHz,PLL48CK as 48MHz.
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
 *    Create Date   : 2012/09/04-2013/12/25
 *    Author        : Wangshb-Luw
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
 *    Create Date   : 2017/09/24-2017/09/24
 *    Author        : Wangshb-Wangshb
 *    Description   : init uart as Asynchronous Mode,
 *                    115200bps, 8bit, non-parity,1 stop bit.
 *    Param         : none
 *    Return Code   : none
 */
static void tm_inituart(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    /* GPIOA clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 

    /* USART2 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

    /* set PA2/PA3 to Alternate function for USART2 transmit/receive */
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); 
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); 

    /* USART2's PA2/PA3 port set */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

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
 *    Create Date   : 2012/09/04-2013/12/23
 *    Author        : Wangshb-Luw
 *    Description   : Init bsp,for example clock and uart etc.
 *    Param         : void
 *    Return Code   : void
 */
void tm_initbsp ( void )
{
#if (__FPU_USED == 1)
    tm_enafpu();
#endif /* __FPU_USED */
    tm_initflash();
    tm_initrcc();
    tm_initwdg();
    tm_inituart();
}

/*
 *    Function Name : tm_senduart
 *    Create Date   : 2017/9/24-2017/09/24
 *    Author        : Wangshb
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
 *    Create Date   : 2017/9/24-2017/09/24
 *    Author        : Wangshb
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
