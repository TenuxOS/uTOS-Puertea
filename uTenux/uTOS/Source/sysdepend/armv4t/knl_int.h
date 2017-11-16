/*
 *----------------------------------------------------------------------------------------
 *    Kernel/int
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
 *    File Name      : knl_int.h
 *    Create Date    : 2009/12/27-2014/5/1
 *    Author         : wangshb
 *    Description    : CPU-Dependent interrupt Definition head file.
 *----------------------------------------------------------------------------------------
 */

#ifndef _KNL_INT_H__
#define _KNL_INT_H__

#include "knl_kernel.h"
#include "tm_int.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef Assembler

/* Start/End interrupt disable section */
#define KNL_INT_BEGIN_DISABLE     { uint32_t _cpsr_ = knl_int_dis();
#define KNL_INT_BETWEEN_DISABLE   ( !isDI(_cpsr_) )
#define KNL_INT_END_DISABLE       knl_int_ena(_cpsr_); }

/* Interrupt enable/disable */
#define KNL_INT_ENABLE            { knl_int_ena(0u); }
#define KNL_INT_DISABLE           { knl_int_dis(); }
#define KNL_INT_IS_DISABLED       ( isDI(__get_CPSR_IF()) )

/* Enable interrupt nesting, through has a higher priority than 'level.' */
#define KNL_INT_ENABLE_UPTO(level) { knl_int_ena(0u); }

/*
 * Interrupt hook enter and leave function return address
 */
extern FP knl_int_hokenterret;
extern FP knl_int_hokleaveret;

#if TK_USE_INT_DEFHDR
/*
 *    Description   : Set interrupt handler
 *                     8 words in ROM and RAM VECTORAREA is 8 code("ldr pc, [pc, #0x18]")
 *                     so that exception table is begin from offset(0x8*4)
 *    Param         : uint32_t dintno: from 0 to TK_RAM_VECTOR_NUMBER
 *                    FP inthdr: int handler pointer
 *    Return Code   : none
 */
Inline void knl_int_defhdr( uint32_t dintno, FP inthdr )
{
    FP * pramvec, * promvec;

    promvec = (FP*)TK_ROM_VECTORAREA_TOP + (uint32_t)TK_EXC_NUMBER;
    pramvec = (FP*)TK_RAM_VECTORAREA_TOP + (uint32_t)TK_EXC_NUMBER;
    if( ( dintno != 0u ) && ( dintno < TK_RAM_VECTOR_NUMBER ) ){
        if( inthdr != NULL ){
            pramvec[dintno] = inthdr;
            if( dintno >= (uint32_t)TK_EXC_NUMBER ){
                tm_int_setsource( ( dintno - (uint32_t)TK_EXC_NUMBER ), (uint32_t)inthdr );
            }
        } else {
            if( dintno < (uint32_t)TK_EXC_NUMBER ){
                pramvec[dintno] = promvec[dintno];
            } else {
                pramvec[dintno] = NULL;
                tm_int_setsource( ( dintno - (uint32_t)TK_EXC_NUMBER ), (uint32_t)NULL );
            }
        }
    }
}
#endif /* TK_USE_INT_DEFHDR */

#if TK_USE_INT_DEFHDR
/*
 *    Description   : Move Vector table from flash to RAM, and remap vector table.
 *                     Because ROM table have 8 words code and 8 words table, so moved 
 *                     item number is 2*number
 *    Param         : uint32_t vector: flash vector table start address
 *                    uint32_t number: flash vector table items number
 *                    uint32_t ramvector: ram vector table start address
 *    Return Code   : void
 */
Inline void knl_int_setvtor ( uint32_t vector, uint32_t number, uint32_t ramvector )
{
    uint32_t* pvec;
    uint32_t* pramvec;

    pvec = (uint32_t*)vector;
    pramvec = (uint32_t*)ramvector;
    for( number = 2u * number ; number > 0u ; number-- ){
        *pramvec = *pvec;
        pvec++;
        pramvec++;
    }
    tm_int_remap();
}
#endif /* TK_USE_INT_DEFHDR */

/*
 *    Description   : Default exeception and interrupt process handler
 *    Param         : none
 *    Return Code   : none
 */
extern void knl_int_defaulthdr( void );

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

/*
 *    Description   : High level programming language routine for interrupt handler
 *                    prepare to call high language interrupt handler,as below:
 *                      void intdhr( uint32_t dintno, VP ctxptr );
 *    Param         : none
 *    Return Code   : none
 */
extern void knl_int_excwraphdr( void );
extern void knl_int_intwraphdr( void );

/*
 *    Description   : Interrupt process handler
 *    Param         : none
 *    Return Code   : none
 */
extern void knl_int_irqhdr( void );

/*
 *    Description   : Hardware timer interrrupt/exception handler
 *                     for calling high level language routine(knl_tck_handler)
 *    Param         : none
 *    Return Code   : none
 */
extern void knl_int_tckhdr( void );

/*
 *    Description   : Return from Interrupt Handler
 *    Param         : none
 *    Return Code   : none
 */
extern void tk_ret_int_impl( void );

#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* _KNL_INT_H__ */
