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
 *    CPU:        STM32F411RE
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tm_bsp.c
 *    Create Date    : 2012/08/28-2017/9/24
 *    Author         : Wangshb-Zhangzl
 *    Description    : BSP entry routine about chip.
 *----------------------------------------------------------------------------------------
 */

#include <tm_bsp.h>

/*
 * Definition of local macro constant
 */


/* Definition for USARTx clock resources */
#define USARTx                           USART2
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE() 

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_2
#define USARTx_TX_GPIO_PORT              GPIOA  
#define USARTx_TX_AF                     GPIO_AF7_USART2
#define USARTx_RX_PIN                    GPIO_PIN_3
#define USARTx_RX_GPIO_PORT              GPIOA 
#define USARTx_RX_AF                     GPIO_AF7_USART2

UART_HandleTypeDef UartHandle;

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
 *    Create Date   : 2012/09/04-2017/11/12
 *    Author        : Wangshb/Luw-Zhangzl/Wangshb
 *    Description   : init flash,enable buffer and 2 wait cycles
 *    Param         : none
 *    Return Code   : none
 */
static void tm_initflash(void)
{
    /* Unlock ACR register */
    HAL_FLASH_Unlock();
    
    /* Init ACR */
    __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
    __HAL_FLASH_DATA_CACHE_ENABLE();
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
    __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_2);
    
    /* Lock ACR register again */
    HAL_FLASH_Lock();
}

/*
 *    Function Name : tm_initrcc
 *    Create Date   : 2011/9/5-2017/11/12
 *    Author        : Cuihl/Luw-Zhangzl/Wangshb
 *    Description   : init rcc ,use 16MHZ HSI as input and set SYSCLK/HCLK as 84MHz,
 *                    PCLK2 as 84MHz,PCLK1 as 42MHz,PLL48CK as 48MHz.
 *    Param         : none
 *    Return Code   : none
 */
void tm_initrcc(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    SystemInit();

    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* The voltage scaling allows optimizing the power consumption when the device is 
       clocked below the maximum system frequency, to update the voltage scaling value 
       regarding system frequency refer to product datasheet.  */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    /* Enable HSI Oscillator and activate PLL with HSI as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = 0x10;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    while(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
       clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
    while(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
    }

    /* Disable Systick, because HAL_RCC_ClockConfig init Systick */
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
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
 *    Create Date   : 2012/09/04-2017/11/12
 *    Author        : Cuihl/Luw-Zhangzl/Wangshb
 *    Description   : init uart as Asynchronous Mode,
 *                    115200bps, 8bit, non-parity,1 stop bit.
 *    Param         : none
 *    Return Code   : none
 */
static void tm_inituart(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;


    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    USARTx_TX_GPIO_CLK_ENABLE();
    USARTx_RX_GPIO_CLK_ENABLE();

    /* Enable USARTx clock */
    USARTx_CLK_ENABLE(); 

    /*##-2- Configure peripheral GPIO ##########################################*/  
    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = USARTx_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = USARTx_TX_AF;
  
    HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin = USARTx_RX_PIN;
    GPIO_InitStruct.Alternate = USARTx_RX_AF;

    HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);

    /*##-1- Configure the UART peripheral ######################################*/
    /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
    /* USART configured as follow:
       - Word Length = 8 Bits
       - Stop Bit = One Stop bit
       - Parity = NONE parity
       - BaudRate = 115200 baud
       - Hardware flow control disabled (RTS and CTS signals) */
    UartHandle.Instance = USARTx;

    UartHandle.Init.BaudRate     = 115200;
    UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits     = UART_STOPBITS_1;
    UartHandle.Init.Parity       = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode         = UART_MODE_TX_RX;
    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

    while(HAL_UART_Init(&UartHandle) != HAL_OK)
    {
    }

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
 *    Create Date   : 2012/9/5-2017/11/12
 *    Author        : Cuihl/Luw-Zhangzl/Wangshb
 *    Description   : send a char to uart util success
 *    Param         : uint8_t c: char to send
 *    Return Code   : none.
 */
void tm_senduart(uint8_t c)
{
    while(HAL_UART_Transmit(&UartHandle, (uint8_t *)&c, 1, 0xFFFF) != HAL_OK )
    {
    }
    return;
}

/*
 *    Function Name : tm_recvuart
 *    Create Date   : 2011/9/5-2017/11/12
 *    Author        : Cuihl/Luw-Zhangzl/Wangshb
 *    Description   : receive char from uart,util success
 *    Param         : none.
 *    Return Code   : uint8_t c: char to receive
 */
uint8_t tm_recvuart(void)
{
    uint8_t c;

    while(HAL_UART_Receive(&UartHandle, (uint8_t *)&c, 1, 0xFFFF) != HAL_OK )
    {
    }

    return c;
}
