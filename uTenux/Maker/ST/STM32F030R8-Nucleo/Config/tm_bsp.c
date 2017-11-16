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
 *    CPU:        STM32F030R8
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tm_bsp.c
 *    Create Date    : 2013/3/19-2017/10/22
 *    Author         : Wangshb
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
#define GPIO_AFRH_SHIFT(x)          ( 0x04u * ( ( x ) - 8u ) )
#define GPIO_AFRL_SHIFT(x)          ( 0x04u * ( x ) )

/*
 * Declaration of local function prototype  
 */
static void tm_initflash(void);
static void tm_initwdg(void);
static void tm_inituart(void);

/*
 *    Function Name : tm_initflash
 *    Create Date   : 2013/3/19-2013/12/18
 *    Author        : wangshb/Zhangzl-wangshb
 *    Description   : init flash,set prefetch cycle as 1 and enable
 *    Param         : none
 *    Return Code   : none
 */
static void tm_initflash(void)
{
    uint32_t data;

    /* Unlock ACR register */
    FLASH->KEYR = FLASH_FKEY1;
    FLASH->KEYR = FLASH_FKEY2;

    /* Init ACR */
    FLASH->ACR = FLASH_ACR_LATENCY |FLASH_ACR_PRFTBE;

    /* Lock ACR register again */
    FLASH->CR = FLASH_CR_LOCK;
    data = FLASH->ACR;
    data = data;

    return;
}

/*
 *    Function Name : tm_initrcc
 *    Create Date   : 2013/3/19-2017/10/22
 *    Author        : wangshb/Zhangzl-wangshb
 *    Description   : init rcc ,use 8MHZ HSI as input and set SYSCLK/HCLK as 48MHz,
 *                    PCLK as 24MHz.
 *    Param         : none
 *    Return Code   : none
 */
void tm_initrcc(void)
{
    /* Enable HSI */
    RCC->CR |= RCC_CR_HSION;

    /* Test if HSI is ready */
    while( (RCC->CR & RCC_CR_HSIRDY) == 0u ){
        ;
    }

    /* Set HCLK=SYSCLK,PCLK=HCLK/2 */
    RCC->CFGR &= (~RCC_CFGR_HPRE);
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

    RCC->CFGR &= (~RCC_CFGR_PPRE);
    RCC->CFGR |= RCC_CFGR_PPRE_DIV2;

    /* Set HSI as PLL source,and Enable PLL */
    RCC->CFGR &= ~( RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL );
    RCC->CFGR |= RCC_CFGR_PLLMULL12 ;
    RCC->CR |= RCC_CR_PLLON;

    /* Test if PLL is ready */
    while( (RCC->CR & RCC_CR_PLLRDY) == 0u ){
        ;
    }

    /* Switch pll as system clock */
    RCC->CFGR &= (~RCC_CFGR_SW);
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Test if pll switch successfully */
   while( (RCC->CFGR & RCC_CFGR_SWS_PLL) == 0u ){
        ;
   }

    return;
}

/*
 *    Function Name : tm_initwdg
 *    Create Date   : 2012/09/04-2013/12/18
 *    Author        : wangshb
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
 *    Create Date   : 2013/3/19-2017/10/22
 *    Author        : Zhangzl-wangshb
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

    /* USART2 clock enable */
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    /* set PA2/PA3 to Alternate function for USART2 transmit/receive */
    data = GPIOA->MODER;
    data &= (~GPIO_MODER_MODER2);
    data |= ( GPIO_MODER_MODER2_1 );
    GPIOA->MODER = data;

    data = GPIOA->OTYPER;
    data &= ( ~(GPIO_OTYPER_OT_2) );
    GPIOA->OTYPER = (uint16_t)data;

    data = GPIOA->PUPDR;
    data &= ( ~(GPIO_PUPDR_PUPDR2) );
    data |= (GPIO_PUPDR_PUPDR2_0 );
    GPIOA->PUPDR = data;

    data = GPIOA->OSPEEDR;
    data &= ( ~(GPIO_OSPEEDER_OSPEEDR2) );
    data |= ( GPIO_OSPEEDER_OSPEEDR2 );
    GPIOA->OSPEEDR = data;

    data = GPIOA->AFR[0];
    data &= ( ~( GPIO_AFR_MASK << GPIO_AFRL_SHIFT(2u) ) );
    data |= ( GPIO_AFR_AF1 << GPIO_AFRL_SHIFT(2u) );
    GPIOA->AFR[0] = data;

    data = GPIOA->MODER;
    data &= (~GPIO_MODER_MODER3);
    data |= ( GPIO_MODER_MODER3_1 );
    GPIOA->MODER = data;

    data = GPIOA->PUPDR;
    data &= ( ~(GPIO_PUPDR_PUPDR3) );
    data |= (GPIO_PUPDR_PUPDR3_0 );
    GPIOA->PUPDR = data;

    data = GPIOA->OSPEEDR;
    data &= ( ~(GPIO_OSPEEDER_OSPEEDR3) );
    data |= ( GPIO_OSPEEDER_OSPEEDR3 );
    GPIOA->OSPEEDR = data;

    data = GPIOA->AFR[0];
    data &= ( ~( GPIO_AFR_MASK << GPIO_AFRL_SHIFT(3u) ) );
    data |= ( GPIO_AFR_AF1 << GPIO_AFRL_SHIFT(3u) );
    GPIOA->AFR[0] = data;

    /* Disable USART and TX and RX */
    USART2->CR1 &= 0xdff3u;

    /* Clear STOP bit,CLKEN,CPOL,CPHA */
    USART2->CR2 &= 0x8060u;

    /* Set 8bit,non parity,no interupt */
    USART2->CR1 &= 0xe80fu;

    /* Disable CTS and DMA */
    USART2->CR3 &= 0xf800u;

    /* Set baud=115200bps */
    USART2->BRR = 0x00d0u;

    /* Enable receiver and transmitter and USART */
    USART2->CR1 |= USART_CR1_MME | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;

    return;
}

/*
 *    Function Name : tm_initbsp
 *    Create Date   : 2012/09/04-2013/12/18
 *    Author        : wangshb
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
 *    Create Date   : 2013/3/19-2017/10/22
 *    Author        : wangzhn-wangshb
 *    Description   : send a char to uart util success
 *    Param         : uint8_t c: char to send
 *    Return Code   : none.
 */
void tm_senduart(uint8_t c)
{
    /* Wait for the transmitter to be ready while */
    while( (USART2->ISR & USART_ISR_TXE) == 0u){
        ;
    }

    /* Send character */
    USART2->TDR = c;

    /* Wait for the transmitter to be complete */
    while( (USART2->ISR & USART_ISR_TC) == 0u){
        ;
    }
}

/*
 *    Function Name : tm_recvuart
 *    Create Date   : 2013/3/19-2017/10/22
 *    Author        : wangzhn-wangshb
 *    Description   : receive char from uart,util success
 *    Param         : none.
 *    Return Code   : uint8_t c: char to receive
 */
uint8_t tm_recvuart(void)
{
    uint8_t c;

    /* Wait for the receiver to be ready while */
    while( (USART2->ISR & USART_ISR_RXNE ) == 0u){
       ;
    }

    c = (uint8_t)USART2->RDR;

    return c;
}
