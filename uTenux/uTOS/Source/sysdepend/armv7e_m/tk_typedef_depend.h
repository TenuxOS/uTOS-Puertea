/*
 *----------------------------------------------------------------------------------------
 *    Include/tk
 *
 *    Copyright (C) 2008-2014 by Dalian uLoong Co.,Ltd. All rights reserved.
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
 *    CPU:        CORTEX M4
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tk_typedef_depend.h
 *    Create Date    : 2012/12/26-2014/5/9
 *    Author         : wangshb
 *    Description    : uT/OS CPU dependent data type definitions.
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_TYPEDEF_DEPEND_H__
#define __TK_TYPEDEF_DEPEND_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Program status register (PSR) */
#define TK_PSR_N             0x80000000U         /* Condition flag Negative */
#define TK_PSR_Z             0x40000000U         /* Zero */
#define TK_PSR_C             0x20000000U         /* Carry */
#define TK_PSR_V             0x10000000U         /* Overflow */
#define TK_PSR_T             0x01000000U         /* Thumb mode */

/* Interrupt  mask register (Primask) */
#define TK_PMK_D             0x1                 /* Interrupt disable */
#define TK_PMK_E             0x0                 /* Interrupt enable */

/* Except  mask register (Faultmask) */
#define TK_FMK_D             0x1                 /* Except disable */
#define TK_FMK_E             0x0                 /* Except enable */

/* Control register (Control) */
#define TK_CTL_MSP           0x0                 /* Select MSP */
#define TK_CTL_PSP           0x2                 /* Select PSP */
#define TK_CTL_SVC           0x0                 /* Select supervisor mode */
#define TK_CTL_USR           0x1   

#define TK_BIGENDIAN         0U                  /* 1 if big endian, o if little endian */
#define TK_INT_BITWIDTH      32U                 /* 32 if CPU is 32bit architecture */
#define TK_EXC_NUMBER        16U                 /* Exception number */
#define TK_ALIGN64                               /* align flag */
  
#ifndef Assembler

/*
 *    Description   : Disable exception and interrupt by
 *                    Set PRIMASK = 1 and return the original PRIMASK.
 *    Param         : uint32_t intsts: the original value of PRIMASK in CPU
 *    Return Code   : none
 */
#define DI(intsts)           ( (intsts) = knl_int_dis() )

/*
 *    Description   : Enable exception and interrupt by
 *                    Set PRIMASK = intsts, Return the original PRIMASK .
 *    Param         : uint32_t intsts: the value of PRIMASK to set
 *    Return Code   : uint32_t cursts: the original value of PRIMASK in CPU
 */
#define EI(intsts)           ( (intsts) = knl_int_ena(intsts) )

/*
 *    Description   : Get exception status in intsts.
 *    Param         : uint32_t intsts: the value of exception status
 *    Return Code   : true if exception disable ,false if exception enable
 */
#define isDI(intsts)         ( ((intsts) & (uint32_t)TK_PMK_D) != 0u )

/* Convert external interrupt vector number to interrupt definition number */
#define DINTNO(intvec)       (uint32_t)( (intvec) + (int32_t)TK_EXC_NUMBER )

/*
 * General purpose register                      tk_get_reg tk_set_reg
 */
typedef struct t_regs {
        uint32_t             r[13];              /* General purpose register R0-R12 */
        VP                   lr;                 /* Link register R14 */
}                            T_REGS;

/*
 * Exception-related register                    tk_get_reg tk_set_reg
 */
typedef struct t_eit {
        VP                   pc;                 /* Program counter R15 */
        uint32_t             psr;                /* Program status register */
        uint32_t             taskmode;           /* Task mode flag */
}                            T_EIT;

/*
 * Control register                              tk_get_reg tk_set_reg
 */
typedef struct t_cregs {
        VP                   ssp;                /* System stack pointer usuealy MSP */
        VP                   usp;                /* User stack pointer MSP or PSP */
}                            T_CREGS;

/*
 * System call/extension SVC caller information  td_hok_svc
 */
typedef struct td_calinf {
        VP                   ssp;                /* System stack pointer usuealy MSP */
        VP                   r11;                /* Frame pointer when calling */
}                            TD_CALINF;

/*
 *    Description   : Disable exception and interrupt by
 *                    Set PRIMASK = 1 and return the original PRIMASK.
 *    Param         : none
 *    Return Code   : uint32_t intsts: the value of PRIMASK in CPU
 */
extern uint32_t knl_int_dis( void );

/*
 *    Description   : Enable exception and interrupt by
 *                    Set PRIMASK = intsts, Return the original PRIMASK .
 *    Param         : uint32_t intsts: the value of PRIMASK to set
 *    Return Code   : uint32_t cursts: the value of PRIMASK in CPU
 */
extern uint32_t knl_int_ena( uint32_t intsts );

#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* __TK_TYPEDEF_DEPEND_H__ */
