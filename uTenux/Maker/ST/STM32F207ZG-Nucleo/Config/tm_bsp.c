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
 *    CPU:        STM32F207ZG
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tm_bsp.c
 *    Create Date    : 2012/08/28-2017/11/12
 *    Author         : Wangshb/Houlw-Wangshb
 *    Description    : BSP entry routine about chip.
 *----------------------------------------------------------------------------------------
 */

#include <tm_bsp.h>

/*
 * Definition of local macro constant
 */
/* FLASH_KEYR Register            */
#define FLASH_KEYR_KEY1             0x45670123U
#define FLASH_KEYR_KEY2             0xCDEF89ABU

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
#define GPIO_AFR_AF10               0x0AU
#define GPIO_AFR_AF11               0x0BU
#define GPIO_AFR_AF12               0x0CU
#define GPIO_AFR_AF13               0x0DU
#define GPIO_AFR_AF14               0x0EU
#define GPIO_AFR_AF15               0x0FU
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
 *    Create Date   : 2012/09/04-2013/12/26
 *    Author        : wangshb-Houlw
 *    Description   : init flash,set prefetch cycle as 2 and enable
 *    Param         : none
 *    Return Code   : none
 */
static void tm_initflash(void)
{
    /* Unlock ACR register */
    FLASH->KEYR = FLASH_KEYR_KEY1;
    FLASH->KEYR = FLASH_KEYR_KEY2;

    /* Init ACR */
    FLASH->ACR = FLASH_ACR_DCEN |FLASH_ACR_ICEN |FLASH_ACR_LATENCY_5WS;

    /* Lock ACR register again */
    FLASH->CR = FLASH_CR_LOCK;
}

/*
 *    Function Name : tm_initrcc
 *    Create Date   : 2012/09/04-2017/11/07
 *    Author        : Houlw-Wangshb
 *    Description   : init rcc ,use 16MHZ HSI as input and set SYSCLK/HCLK as 120MHz,
 *                    PCLK2 as 60MHz, PCLK1 as 30MHz.
 *    Param         : none
 *    Return Code   : none
 */
void tm_initrcc(void)
{
    /* Enable HSI */
    RCC->CR |=  RCC_CR_HSION;

    /* Test if HSI is ready */
    while( (RCC->CR & RCC_CR_HSIRDY) == 0u ){
        ;
    }

    /* Set HCLK=SYSCLK,PCLK2=HCLK/2,PCLK1=HCLK/4 */
    RCC->CFGR &= (~RCC_CFGR_HPRE);
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
    
    RCC->CFGR &= (~RCC_CFGR_PPRE2);
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
    
    RCC->CFGR &= (~RCC_CFGR_PPRE1);
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

    /* Set HSI as PLL source,and Enable PLL, P=2,M=16,N=240 */
    RCC->PLLCFGR &= ( (~RCC_PLLCFGR_PLLSRC) & (~RCC_PLLCFGR_PLLP) & \
              (~RCC_PLLCFGR_PLLN)&(~RCC_PLLCFGR_PLLM) );
    RCC->PLLCFGR |= ( RCC_PLLCFGR_PLLSRC_HSI | \
            RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_7 | \
            (HSI_VALUE/1000000u) );

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
 *    Create Date   : 2012/09/04-2013/12/26
 *    Author        : wangshb-Houlw
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
 *    Create Date   : 2012/09/04-2017/11/12
 *    Author        : Houlw-Wangshb
 *    Description   : init uart as Asynchronous Mode,
 *                    115200bps, 8bit, non-parity,1 stop bit.
 *    Param         : none
 *    Return Code   : none
 */
static void tm_inituart(void)
{
    uint32_t data;

    /* GPIOD clock enable */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    /* USART3 clock enable */
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    /* set PD8/PD9 to Alternate function for USART3 transmit/receive */
    data = GPIOD->MODER;
    data &= (~GPIO_MODER_MODER8);
    data |= ( GPIO_MODER_MODER8_1 );
    GPIOD->MODER = data;

    data = GPIOD->OTYPER;
    data &= ( ~(GPIO_OTYPER_OT_8) );
    GPIOD->OTYPER = data;

    data = GPIOD->PUPDR;
    data &= ( ~(GPIO_PUPDR_PUPDR8) );
    data |= (GPIO_PUPDR_PUPDR8_0 );
    GPIOD->PUPDR = data;

    data = GPIOD->OSPEEDR;
    data &= ( ~(GPIO_OSPEEDER_OSPEEDR8) );
    data |= ( GPIO_OSPEEDER_OSPEEDR8_1 );
    GPIOD->OSPEEDR = data;

    data = GPIOD->AFR[1];
    data &= ( ~( GPIO_AFR_MASK<<GPIO_AFRH_SHIFT(8u)) );
    data |= ( GPIO_AFR_AF7<<GPIO_AFRH_SHIFT(8u) );
    GPIOD->AFR[1] = data;

    data = GPIOD->MODER;
    data &= (~GPIO_MODER_MODER9);
    data |= ( GPIO_MODER_MODER9_1 );
    GPIOD->MODER = data;

    data = GPIOD->PUPDR;
    data &= ( ~(GPIO_PUPDR_PUPDR9) );
    data |= (GPIO_PUPDR_PUPDR9_0 );
    GPIOD->PUPDR = data;

    data = GPIOD->OSPEEDR;
    data &= ( ~(GPIO_OSPEEDER_OSPEEDR9) );
    data |= ( GPIO_OSPEEDER_OSPEEDR9_1 );
    GPIOD->OSPEEDR = data;

    data = GPIOD->AFR[1];
    data &= ( ~( GPIO_AFR_MASK<<GPIO_AFRH_SHIFT(9u)) );
    data |= ( GPIO_AFR_AF7<<GPIO_AFRH_SHIFT(9u) );
    GPIOD->AFR[1] = data;

    /* Disable USART and TX and RX */
    USART3->CR1 &= 0xdff3u;

    /* Clear STOP bit,CLKEN,CPOL,CPHA */
    USART3->CR2 &= 0x8060u;

    /* Set 8bit,non parity,no interupt */
    USART3->CR1 &= 0xE80Fu;

    /* Disable CTS and DMA */
    USART3->CR3 &= 0xf800u;

    /* Set baud=115200bps */
    USART3->BRR = 0x0100u;

    /* Enable receiver and transmitter and USART */
    USART3->CR1 |= 0x0200cu;

    return;
}

/*
 *    Function Name : tm_initbsp
 *    Create Date   : 2012/09/04-2013/10/31
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
 *    Create Date   : 2012/9/5-2017/11/07
 *    Author        : wangzhn-Houlw
 *    Description   : send a char to uart util success
 *    Param         : uint8_t c: char to send
 *    Return Code   : none.
 */
void tm_senduart(uint8_t c)
{
    /* Wait for the transmitter to be ready while */
    while( (USART3->SR & USART_SR_TXE) == 0u){
        ;
    }

    /* Send character */
    USART3->DR = c;

    /* Wait for the transmitter to be ready while */
    while( (USART3->SR & USART_SR_TXE) == 0u){
            ;
    }
}

/*
 *    Function Name : tm_recvuart
 *    Create Date   : 2011/9/5-2017/11/07
 *    Author        : wangzhn-Houlw
 *    Description   : receive char from uart,util success
 *    Param         : none.
 *    Return Code   : uint8_t c: char to receive
 */
uint8_t tm_recvuart(void)
{
    uint8_t c;

    /* Wait for the receiver to be ready while */
    while( ((USART3->SR) & USART_SR_RXNE) == 0u){
         ;
    }

    c = (uint8_t)USART3->DR;

    return c;
}
