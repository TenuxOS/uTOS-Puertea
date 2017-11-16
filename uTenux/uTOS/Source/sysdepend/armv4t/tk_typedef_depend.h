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
 *    CPU:        ARM7TDMI
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tk_typedef_depend.h
 *    Create Date    : 2012/12/26-2014/5/6
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
#define TK_PSR_N             0x80000000          /* Condition flag Negative */
#define TK_PSR_Z             0x40000000          /* Zero */
#define TK_PSR_C             0x20000000          /* Carry */
#define TK_PSR_V             0x10000000          /* Overflow */

#define TK_PSR_I             0x00000080          /* Interrupt (IRQ) disable */
#define TK_PSR_F             0x00000040          /* Fast Interrupt (FIQ) disable */
#define TK_PSR_DI            0x000000C0          /* All Interrupts disable =PSR_I|PSR_F */
    
#define TK_PSR_T             0x00000020          /* Thumb mode */
    
                                                 /* Processor mode [4:0] */
#define TK_PSR_USR           0x10                /* User mode */
#define TK_PSR_FIQ           0x11                /* Fast Interrupt (FIQ) mode */
#define TK_PSR_IRQ           0x12                /* Interrupt (IRQ) mode */
#define TK_PSR_SVC           0x13                /* Supervisor mode */
#define TK_PSR_ABT           0x17                /* Abort mode */
#define TK_PSR_UND           0x1b                /* Undefined order mode */
#define TK_PSR_SYS           0x1f                /* System mode */

#define TK_BIGENDIAN         0U                  /* 1 if big endian, o if little endian */
#define TK_INT_BITWIDTH      32U                 /* 32 if CPU is 32bit architecture */
#define TK_EXC_NUMBER        8                   /* Exception number */
#define TK_ALIGN64                               /* align flag */
  
#ifndef Assembler

/*
 *    Description   : Disable exception and interrupt by
 *                    Set CPSR_C = TK_PSR_DI and return the original CPSR.
 *    Param         : uint32_t intsts: the original value of CPSR in CPU
 *    Return Code   : none
 */
#define DI(intsts)           ( (intsts) = knl_int_dis() )

/*
 *    Description   : Enable exception and interrupt by
 *                    Set CPSR_C = intsts, Return the original CPSR .
 *    Param         : uint32_t intsts: the value of CPSR to set
 *    Return Code   : uint32_t cursts: the original value of CPSR in CPU
 */
#define EI(intsts)           ( (intsts) = knl_int_ena(intsts) )

/*
 *    Description   : Get exception status in intsts.
 *    Param         : uint32_t intsts: the value of exception status
 *    Return Code   : true if exception disable ,false if exception enable
 */
#define isDI(intsts)         ( ((intsts) & (uint32_t)TK_PSR_DI) != 0u )

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
        uint32_t             cpsr;               /* Program status register */
        uint32_t             taskmode;           /* Task mode flag */
}                            T_EIT;

/*
 * Control register                              tk_get_reg tk_set_reg
 */
typedef struct t_cregs {
        VP                   ssp;                /* System stack pointer usuealy R13_svc */
        VP                   usp;                /* User stack pointer R13_usr */
}                            T_CREGS;

/*
 * System call/extension SVC caller information  td_hok_svc
 */
typedef struct td_calinf {
        VP                   ssp;                /* System stack pointer */
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
