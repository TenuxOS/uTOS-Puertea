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
    /* Enable HSI */
    RCC->CR |= RCC_CR_HSION;

    /* Test if HSI is ready */
    while( (RCC->CR & RCC_CR_HSIRDY) == 0u ){
        ;
    }

    /* Set HCLK=SYSCLK,PCLK2=HCLK,PCLK1=HCLK/2 */
    RCC->CFGR &= (~RCC_CFGR_HPRE);
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

    RCC->CFGR &= (~RCC_CFGR_PPRE2);
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

    RCC->CFGR &= (~RCC_CFGR_PPRE1);
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

    /* Set HSI as PLL source,and Enable PLL */
    RCC->CFGR &= ~( RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL );
    RCC->CFGR |= ( RCC_CFGR_PLLSRC_HSI_Div2 | RCC_CFGR_PLLMULL12 );
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
   /* GPIOA and AFIO clock enable */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    
    /* USART2 clock enable */
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    /* set PA2/PA3 to Alternate function for USART2 transmit/receive */
    AFIO->MAPR &=  ~( AFIO_MAPR_USART2_REMAP );
    
    GPIOA->CRL &=  ~( GPIO_CRL_MODE2 | GPIO_CRL_CNF2 );
    GPIOA->CRL |= (GPIO_CRL_MODE2_0 |GPIO_CRL_MODE2_1 | GPIO_CRL_CNF2_1);

    GPIOA->CRL &=  ~( GPIO_CRL_MODE3 | GPIO_CRL_CNF3 );
    GPIOA->CRL |= (GPIO_CRL_CNF3_0);

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
    USART2->CR1 |= 0x0200cu;

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
    while( (USART2->SR & USART_SR_TXE) == 0u){
        ;
    }

    /* Send character */
    USART2->DR = c;

    /* Wait for the transmitter to be ready while */
    while( (USART2->SR & USART_SR_TXE) == 0u){
        ;
    }
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
    while( (USART2->SR & USART_SR_RXNE ) == 0u){
       ;
    }

    c = (uint8_t)USART2->DR;

    return c;
}
