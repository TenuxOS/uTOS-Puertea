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
 *    CPU:        STM32L152RB
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tm_bsp.c
 *    Create Date    : 2019/23-2017/11/07
 *    Author         : Zhangzl/Cuihl-Wangshb
 *    Description    : BSP entry routine about chip.
 *----------------------------------------------------------------------------------------
 */

#include <tm_bsp.h>

/*
 * Definition of local macro constant
 */
/* GPIO_AFRL and AFRH Register   */
#define GPIO_AFR_MASK               0x04U
#define GPIO_AFR_AF0                0x00U
#define GPIO_AFR_AF1                0x01U
#define GPIO_AFR_AF2                0x02U
#define GPIO_AFR_AF3                0x03U
#define GPIO_AFR_AF4                0x04U
#define GPIO_AFR_AF5                0x05U
#define GPIO_AFR_AF6                0x06U
#define GPIO_AFR_AF7                0x07U
#define GPIO_AFR_AF8                0x08U
#define GPIO_AFR_AF9                0x09U
#define GPIO_AFR_AF10               0x0aU
#define GPIO_AFR_AF11               0x0bU
#define GPIO_AFR_AF12               0x0cU
#define GPIO_AFR_AF13               0x0dU
#define GPIO_AFR_AF14               0x0eU
#define GPIO_AFR_AF15               0x0fU
#define GPIO_AFRH_SHIFT(x)          (0x04u * ( ( x ) - 8u ) )
#define GPIO_AFRL_SHIFT(x)          (0x04u * ( x ) )

/*
 * Declaration of local function prototype  
 */
static void tm_initflash(void);
static void tm_initwdg(void);
static void tm_inituart(void);

/*
 *    Function Name : tm_initflash
 *    Create Date   : 2013/09/23-2013/12/27
 *    Author        : Zhangzl-Zhudx
 *    Description   : init flash,enable buffer and 5 wait cycles
 *    Param         : none
 *    Return Code   : none
 */
static void tm_initflash(void)
{
    return;
}

/*
 *    Function Name : tm_initrcc
 *    Create Date   : 2013/09/23-2013/12/27
 *    Author        : Zhangzl-Zhudx
 *    Description   : init rcc ,use 16MHZ HSI as input and set SYSCLK/HCLK as 16MHz,
 *                    PCLK2/PCLK1 as 16MHz.
 *    Param         : none
 *    Return Code   : none
 */

void tm_initrcc(void)
{
    /* Enable HSI */
    RCC->CR |= RCC_CR_HSION;

    /* Test if HSI is ready */
    while((RCC->CR & RCC_CR_HSIRDY) == 0u){
        ;
    }

    /* Switch HSI as system clock */
    RCC->CFGR &= (~RCC_CFGR_SW);
    RCC->CFGR |= RCC_CFGR_SW_HSI;

    /* Test if HSI switch successfully */
    while((RCC->CFGR & RCC_CFGR_SWS_HSI) == 0u){
        ;
    }

    return;

}

/*
 *    Function Name : tm_initwdg
 *    Create Date   : 2013/09/23-2013/12/27
 *    Author        : Zhangzl-Zhudx
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
 *    Create Date   : 2013/09/23-2013/12/27
 *    Author        : Zhangzl-Zhudx
 *    Description   : init uart as Asynchronous Mode,
 *                    115200bps, 8bit, non-parity,1 stop bit.
 *    Param         : none
 *    Return Code   : none
 */
static void tm_inituart(void)
{
    uint32_t data;

    /* GPIOA clock enable */
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    /* USART1 clock enable */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    /* set PA9/PA10 to Alternate function for USART1 +ansmit/receive */
    data = GPIOA->MODER;
    data &= (~GPIO_MODER_MODER9);
    data |= ( GPIO_MODER_MODER9_1 );
    GPIOA->MODER = data;

    data = GPIOA->OTYPER;
    data &= ( ~(GPIO_OTYPER_OT_9) );
    GPIOA->OTYPER = (uint16_t)data;

    data = GPIOA->PUPDR;
    data &= ( ~(GPIO_PUPDR_PUPDR9) );
    data |= (GPIO_PUPDR_PUPDR9_0 );
    GPIOA->PUPDR = data;

    data = GPIOA->OSPEEDR;
    data &= ( ~(GPIO_OSPEEDER_OSPEEDR9) );
    data |= ( GPIO_OSPEEDER_OSPEEDR9 );
    GPIOA->OSPEEDR = data;

    data = GPIOA->AFR[1];
    data &= ( ~( GPIO_AFR_MASK<<GPIO_AFRH_SHIFT(9u)) );
    data |= ( GPIO_AFR_AF7<<GPIO_AFRH_SHIFT(9u) );
    GPIOA->AFR[1] = data;

    data = GPIOA->MODER;
    data &= (~GPIO_MODER_MODER10);
    data |= ( GPIO_MODER_MODER10_1 );
    GPIOA->MODER = data;

    data = GPIOA->PUPDR;
    data &= ( ~(GPIO_PUPDR_PUPDR10) );
    data |= (GPIO_PUPDR_PUPDR10_0 );
    GPIOA->PUPDR = data;

    data = GPIOA->OSPEEDR;
    data &= ( ~(GPIO_OSPEEDER_OSPEEDR10) );
    data |= ( GPIO_OSPEEDER_OSPEEDR10 );
    GPIOA->OSPEEDR = data;

    data = GPIOA->AFR[1];
    data &= ( ~( GPIO_AFR_MASK<<GPIO_AFRH_SHIFT(10u)) );
    data |= ( GPIO_AFR_AF7<<GPIO_AFRH_SHIFT(10u) );
    GPIOA->AFR[1] = data;

    /* Disable USART and TX and RX */
    USART1->CR1 &= 0xdff3u;

    /* Clear STOP bit,CLKEN,CPOL,CPHA */
    USART1->CR2 &= 0x8060u;

    /* Set 8bit,non parity,no interupt */
    USART1->CR1 &= 0xe80fu;

    /* Disable CTS and DMA */
    USART1->CR3 &= 0xf800u;

    /* Set baud=115200bps */
    USART1->BRR = 0x08au;

    /* Enable receiver and transmitter and USART */
    USART1->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;

    return;
}


/*
 *    Function Name : tm_initbsp
 *    Create Date   : 2013/9/23-2013/12/27
 *    Author        : Zhangzl-Zhudx
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
 *    Create Date   : 2013/9/23-2013/12/27
 *    Author        : Zhangzl-Zhudx
 *    Description   : send a char to uart util success
 *    Param         : uint8_t c: char to send
 *    Return Code   : none.
 */
void tm_senduart(uint8_t c)
{
    /* Wait for the transmitter to be ready while */
    while( (USART1->SR & USART_SR_TXE) == 0u){
        ;
    }

    /* Send character */
    USART1->DR = c;

    /* Wait for the transmitter to be ready while */
    while( (USART1->SR & USART_SR_TXE) == 0u){
        ;
    }
}

/*
 *    Function Name : tm_recvuart
 *    Create Date   : 2013/9/23-2014/04/29
 *    Author        : Zhangzl-Cuihl
 *    Description   : receive char from uart,util success
 *    Param         : none.
 *    Return Code   : uint8_t c: char to receive
 */
uint8_t tm_recvuart(void)
{
    uint8_t c;

    /* Wait for the receiver to be ready while */
    while( (USART1->SR & USART_SR_RXNE ) == 0u){
       ;
    }

    c = (uint8_t)USART1->DR;

    return c;
}
