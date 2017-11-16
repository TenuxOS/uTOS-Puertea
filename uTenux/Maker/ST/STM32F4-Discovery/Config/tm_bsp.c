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
 *    File Name      : tm_bsp.c
 *    Create Date    : 2012/08/28-2017/11/4
 *    Author         : Wangshb-Houlw/Wangshb
 *    Description    : BSP entry routine about chip.
 *----------------------------------------------------------------------------------------
 */

#include <tm_bsp.h>

/*
 * Definition of local macro constant
 */
/* FLASH_KEYR Register            */
#define FLASH_KEY1                  0x45670123U
#define FLASH_KEY2                  0xCDEF89ABU

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
#define GPIO_AFRH_SHIFT(x)          ( 0x04u * ( ( x ) - 8u) )
#define GPIO_AFRL_SHIFT(x)          ( 0x04u * ( x ) )

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
 *    Create Date   : 2012/09/04-2013/12/23
 *    Author        : Wangshb-Luw
 *    Description   : init flash,enable buffer and 5 wait cycles
 *    Param         : none
 *    Return Code   : none
 */
static void tm_initflash(void)
{
    uint32_t data;

    /* Unlock ACR register */
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;

    /* Init ACR */
    FLASH->ACR = FLASH_ACR_DCEN |FLASH_ACR_ICEN |FLASH_ACR_LATENCY_5WS;

    /* Lock ACR register again */
    FLASH->CR = FLASH_CR_LOCK;
    data = FLASH->ACR;
    data = data;
}

/*
 *    Function Name : tm_initrcc
 *    Create Date   : 2011/9/5-2014/5/6
 *    Author        : Cuihl-Luw/Wangshb
 *    Description   : init rcc ,use 8/25MHZ HSE as input and set SYSCLK/HCLK as 168MHz,
 *                    PCLK2 as 84MHz,PCLK1 as 42MHz.
 *    Param         : none
 *    Return Code   : none
 */
void tm_initrcc(void)
{
    uint32_t data;

    /* Enable HSE */
    RCC->CR |=  RCC_CR_HSEON;

    /* Test if HSE is ready */
    while( (RCC->CR & RCC_CR_HSERDY) == 0u ){
        ;
    }

    /* Set HCLK=SYSCLK,PCLK2=HCLK/2,PCLK1=HCLK/4 */
    RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_HPRE)) | RCC_CFGR_HPRE_DIV1;
    RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_PPRE2)) | RCC_CFGR_PPRE2_DIV2;
    RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_PPRE1)) | RCC_CFGR_PPRE1_DIV4;


    /* Set HSE as PLL source,and Enable PLL, P=2,M=8/25,N=336 */
    RCC->PLLCFGR &= ( (~RCC_PLLCFGR_PLLSRC) & (~RCC_PLLCFGR_PLLP) & \
              (~RCC_PLLCFGR_PLLN)&(~RCC_PLLCFGR_PLLM) );
    RCC->PLLCFGR |= ( RCC_PLLCFGR_PLLSRC_HSE | \
            RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_8 | \
            (HSE_VALUE/1000000u) );

    RCC->CR |= RCC_CR_PLLON;

    /* Test if PLL is ready */
    while( (RCC->CR & RCC_CR_PLLRDY) == 0u ){
        ;
    }

    /* Switch pll as system clock */
    data = ( RCC->CFGR & (~RCC_CFGR_SW )) | RCC_CFGR_SW_PLL;
    RCC->CFGR = data;

    /* Test if pll switch successfully */
    while( (RCC->CFGR & RCC_CFGR_SWS_PLL) == 0u ){
        ;
    }

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
 *    Create Date   : 2012/09/04-2013/12/23
 *    Author        : Cuihl-Luw
 *    Description   : init uart as Asynchronous Mode,
 *                    115200bps, 8bit, non-parity,1 stop bit.
 *    Param         : none
 *    Return Code   : none
 */
static void tm_inituart(void)
{
    uint32_t data;

    /* GPIOC clock enable */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    /* USART3 clock enable */
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    /* set PC10/PC11 to Alternate function for USART3 transmit/receive */
    data = GPIOC->MODER;
    data &= (~GPIO_MODER_MODER10);
    data |= ( GPIO_MODER_MODER10_1 );
    GPIOC->MODER = data;

    data = GPIOC->OTYPER;
    data &= ( ~(GPIO_OTYPER_OT_10) );
    GPIOC->OTYPER = data;

    data = GPIOC->PUPDR;
    data &= ( ~(GPIO_PUPDR_PUPDR10) );
    data |= (GPIO_PUPDR_PUPDR10_0 );
    GPIOC->PUPDR = data;

    data = GPIOC->OSPEEDR;
    data &= ( ~(GPIO_OSPEEDER_OSPEEDR10) );
    data |= ( GPIO_OSPEEDER_OSPEEDR10_1 );
    GPIOC->OSPEEDR = data;

    data = GPIOC->AFR[1];
    data &= ( ~( GPIO_AFR_MASK<<GPIO_AFRH_SHIFT(10u)) );
    data |= ( GPIO_AFR_AF7<<GPIO_AFRH_SHIFT(10u) );
    GPIOC->AFR[1] = data;

    data = GPIOC->MODER;
    data &= (~GPIO_MODER_MODER11);
    data |= ( GPIO_MODER_MODER11_1 );
    GPIOC->MODER = data;

    data = GPIOC->PUPDR;
    data &= ( ~(GPIO_PUPDR_PUPDR11) );
    data |= (GPIO_PUPDR_PUPDR11_0 );
    GPIOC->PUPDR = data;

    data = GPIOC->OSPEEDR;
    data &= ( ~(GPIO_OSPEEDER_OSPEEDR11) );
    data |= ( GPIO_OSPEEDER_OSPEEDR11_1 );
    GPIOC->OSPEEDR = data;

    data = GPIOC->AFR[1];
    data &= ( ~( GPIO_AFR_MASK<<GPIO_AFRH_SHIFT(11u)) );
    data |= ( GPIO_AFR_AF7<<GPIO_AFRH_SHIFT(11u) );
    GPIOC->AFR[1] = data;

    /* Disable USART and TX and RX */
    USART3->CR1 &= 0xdff3u;

    /* Clear STOP bit,CLKEN,CPOL,CPHA */
    USART3->CR2 &= 0x8060u;

    /* Set 8bit,non parity,no interupt */
    USART3->CR1 &= 0xe80Fu;

    /* Disable CTS and DMA */
    USART3->CR3 &= 0xf800u;

    /* Set baud=115200bps */
    USART3->BRR = 0x016cu;

    /* Enable receiver and transmitter and USART */
    USART3->CR1 |= 0x200cu;

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
 *    Create Date   : 2012/9/5-2013/12/23
 *    Author        : Cuihl-Luw
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

    return;
}

/*
 *    Function Name : tm_recvuart
 *    Create Date   : 2011/9/5-2013/12/23
 *    Author        : Cuihl-Luw
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
