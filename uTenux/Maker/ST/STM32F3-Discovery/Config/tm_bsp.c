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
 *    CPU:        STM32F303VC
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tm_bsp.c
 *    Create Date    : 2013/12/13-2017/11/07
 *    Author         : Mamx/Cuihl-Wangshb
 *    Description    : BSP entry routine about chip.
 *----------------------------------------------------------------------------------------
 */

#include <tm_bsp.h>

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
 *    Create Date   : 2012/10/21-2013/10/31
 *    Author        : wangshb
 *    Description   : Active FPU.
 *    Param         : none
 *    Return Code   : none
 */
static void tm_enafpu(void)
{
    uint32_t control;

    /* Set FPU full access and active */
    SCB->CPACR |= 0xfu << 20u;                     /* set CP10 and CP11 to Full Access */
    control=__get_CONTROL() | 0x4u;
    __set_CONTROL(control);                      /* set FPCA to active FPU */
}
#endif /* __FPU_USED */

/*
 *    Function Name : tm_initflash
 *    Create Date   : 2013/3/19-2013/12/13
 *    Author        : Mamx-Mamx
 *    Description   : init flash,
 *    Param	        : none
 *    Return Code   : none
 */
static void tm_initflash(void)
{
    /* Unlock ACR register */
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;

    /* Set flash prefetch cycle as 2 and Enable */
    FLASH->ACR &= (uint8_t)(~FLASH_ACR_LATENCY);
    FLASH->ACR |= FLASH_ACR_LATENCY_1;
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Lock ACR register again */
    FLASH->CR |= FLASH_CR_LOCK;
}

/*
 *    Function Name : tm_initrcc
 *    Create Date   : 2013/3/19-2014/07/29
 *    Author        : Mamx-Wangshb
 *    Description   : init rcc ,use 8MHZ HSI as input and set SYSCLK/HCLK/PCLK2 as 64MHz,
 *                    PCLK1 as 32MHz
 *    Param	        : none
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

    /* Set HCLK=SYSCLK,PCLK2=HCLK,PCLK1=HCLK/2 */
    RCC->CFGR &= (~RCC_CFGR_HPRE);
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

    RCC->CFGR &= (~RCC_CFGR_PPRE2);
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

    RCC->CFGR &= (~RCC_CFGR_PPRE1);
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

    /* Set HSI as PLL source,and Enable PLL */
    RCC->CFGR &= RCC_CFGR_PLLMULL;
    RCC->CFGR |= RCC_CFGR_PLLMULL16 | RCC_CFGR_PLLSRC_HSI_Div2;
    RCC->CR |= RCC_CR_PLLON;

    /* Test if PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0u){
        ;
    }

    /*Switch pll as system clock */
    RCC->CFGR &= (~RCC_CFGR_SW);
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Test if pll switch successfully */
    while((RCC->CFGR & RCC_CFGR_SWS_PLL) == 0u){
        ;
    }

    return;
}

/*
 *    Function Name : tm_initwdg
 *    Create Date   : 2013/3/26-2013/12/13
 *    Author        : Mamx-Mamx
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
 *    Create Date   : 2013/3/22-2014/07/29
 *    Author        : Mamx-Wangshb
 *    Description   : init uart as Asynchronous Mode,
 *                    115200bps, 8bit, non-parity,1 stop bit.
 *    Param	        : none
 *    Return Code   : none
 */
static void tm_inituart(void)
{
    /* GPIOA clock enable */
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    /* UART1 clock enable */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    /* set PA9/PA10 to Alternate function for UART1 transmit/receive */
    GPIOA->MODER &= (~GPIO_MODER_MODER9);
    GPIOA->MODER |= GPIO_MODER_MODER9_1;

    GPIOA->OTYPER &= (uint16_t)(~GPIO_OTYPER_OT_9);

    GPIOA->PUPDR &= (~GPIO_PUPDR_PUPDR9);
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR9_0;

    GPIOA->OSPEEDR &= (~GPIO_OSPEEDER_OSPEEDR9);
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9_0 | GPIO_OSPEEDER_OSPEEDR9_1;

    GPIOA->AFR[1] &= (~GPIO_AFRH_AFRH1);
    GPIOA->AFR[1] |= 0x70u;

    GPIOA->MODER &= (~GPIO_MODER_MODER10);
    GPIOA->MODER |= GPIO_MODER_MODER10_1;

    GPIOA->PUPDR &= (~GPIO_PUPDR_PUPDR10);
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR10_0;

    GPIOA->OSPEEDR &= (~GPIO_OSPEEDER_OSPEEDR10);
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10_0 | GPIO_OSPEEDER_OSPEEDR10_1;

    GPIOA->AFR[1] &= (~GPIO_AFRH_AFRH2);
    GPIOA->AFR[1] |= 0x700u;

    /* Disable USART and TX and RX */
    USART1->CR1 &= (~(USART_CR1_RE | USART_CR1_TE));

    /* Clear STOP bit,CLKEN,CPOL,CPHA */
    USART1->CR2 &= 0x00ff007fu;

    /* Set 8bit,non parity,no interupt */
    USART1->CR1 &= 0xffffe80fu;

    /* Disable CTS and DMA */
    USART1->CR3 &= 0xfffff800u;

    /* Set baud=115200bps */
    USART1->BRR = 0x22cu;

    /* Enable receiver and transmitter and USART */
    USART1->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;

    return;
}

/*
 *    Function Name : tm_initbsp
 *    Create Date   : 2012/09/04-2013/10/31
 *    Author        : Wangshb-Mamx
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
 *    Create Date   : 2013/3/26-2013/12/16
 *    Author        : Mamx-Mamx
 *    Description   : send a char to uart util success
 *    Param	        : uint8_t c: char to send
 *    Return Code   : none.
 */
void tm_senduart(uint8_t c)
{
    /* Wait for the transmitter to be ready while */
  while((USART1->ISR & USART_ISR_TXE) == 0u){
      ;
  }

    /* Send character */
    USART1->TDR = c;

    /* Wait for the transmission complete */
  while((USART1->ISR & USART_ISR_TC) == 0u){
      ;
  }
}

/*
 *    Function Name : tm_recvuart
 *    Create Date   : 2013/3/25-2013/12/16
 *    Author        : Mamx-Mamx
 *    Description   : receive char from uart,util success
 *    Param	        : none.
 *    Return Code   : uint8_t c: char to receive
 */
uint8_t tm_recvuart(void)
{
    uint8_t c;

    /* Wait for the receiver to be ready while */
    while((USART1->ISR & USART_ISR_RXNE) == 0u){
        ;
    }

    c = (uint8_t)USART1->RDR;

    return c;
}
