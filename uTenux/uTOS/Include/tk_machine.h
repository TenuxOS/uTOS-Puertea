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
 *    CPU:        NONE
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tk_machine.h
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : Machine type definition.
 *                     Describes differences between platforms, such as complier,
 *                     for example gcc, iar cc, arm cc etc.
 *                     Where the source code must be differentiated according to platform,
 *                     macros and other definitions given here should be used.
 *                     Because this definition is not used solely by the C language
 *                     source file, so non-macro definitions are not permitted.
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_MACHINE_H__
#define __TK_MACHINE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * GNUC Compiler
 */
#ifdef __GNUC__

/* Compiler key word definition */
#ifndef Inline
#ifdef  __cplusplus
#define Inline               inline
#else
#define Inline               static __inline__
#endif
#endif

#ifndef Asm
#define Asm                  __asm__ volatile
#endif

#ifndef Assembler
#ifdef  __ASSEMBLER__
#define Assembler            __ASSEMBLER__
#define End                  .end
#endif
#endif

/*
 * No initialization section
 */
#define Zeroinit

#endif /* __GNUC__ */

/*
 * IAR C Compiler
 */
#if defined(__ICCARM__) || defined(__IASMARM__)

/* Compiler key word definition */
#ifndef Inline
#ifdef __cplusplus
#define Inline               inline
#else
#define Inline               static inline
#endif
#endif

#ifndef Asm
#define Asm                  __asm
#endif

#ifndef Assembler
#ifdef  __IASMARM__
#define Assembler            __IASMARM__
#define End                  END
#endif
#endif

/*
 * No initialization section
 */
#define Zeroinit

#endif /* __ICCARM__ */

/*
 * ARM C Compiler
 */
#if defined(__CC_ARM) || defined(__ASM_ARM)

/* Compiler key word definition */
#ifndef Inline
#ifdef  __cplusplus
#define Inline               __inline
#else
#define Inline               static __inline
#endif
#endif

#ifndef Asm
#define Asm                  __asm
#endif

#ifndef Assembler
#ifdef  __ASM_ARM
#define Assembler            __ASM_ARM
#define End                  END
#endif
#endif

/*
 * No initialization section
 */
#define Zeroinit             __attribute__((zero_init))

#endif /* __CC_ARM */

#ifndef Assembler

#include <stdbool.h>                             /* Defined by C compiler */
#include <stdint.h>                              /* Defined by C compiler */

/*
 * 64 bit length integer
 */
#define ltoll(a)             ( (int64_t)(a) )
#define ultoll(a)            ( (int64_t)(a) )
#define uitoll(a)            ( (int64_t)(a) )
#define lltol(a)             ( (int32_t)(a) )
#define lltoul(a)            ( (uint32_t)(a) )
#define ll_add(a,b)          ( (a) + (b) )
#define ll_sub(a,b)          ( (a) - (b) )
#define ll_mul(a,b)          ( (a) * (b) )
#define li_mul(a,b)          ( (a) * (b) )
#define lui_mul(a,b)         ( (a) * ((int64_t)(b)) )
#define ll_div(a,b)          ( (a) / (b) )
#define li_div(a,b)          ( (a) / (b) )
#define lui_div(a,b)         ( (a) / ((int64_t)(b)) )
#define ll_mod(a,b)          ( (a) % (b) )
#define li_mod(a,b)          ( (a) % (b) )
#define lui_mod(a,b)         ( (a) % (int64_t)(b)) )
#define ll_cmp(a,b)          ( (a) - (b) )       /* +:a>b,0:a=b,-:a<b */
#define ll_sign(a)           ( (a) )             /* +:a>0,0:a=0,-:a<0 */
#define ll_neg(a)            ( -(a) )
#define ll_inc(a)            ( (*(a))++ )
#define ll_dec(a)            ( (*(a))-- )

/*
 *    Description   : Conversion from high long and low long to long long
 *    Param         : int32_t h : high part of long long
 *                    uint32_t l : low part of long long
 *    Return Code   : int64_t : converted long long
 */
Inline int64_t ll_hilo(const int32_t h, const uint32_t l)
{
    uint64_t u;
    
    u = (uint64_t)h;
    u = ( u << 32u ) | l;
    return (int64_t)u;
}

/*
 *    Description   : Extract high long from long long
 *    Param         : int64_t h : long long
 *    Return Code   : int32_t : high part of long long
 */
Inline int32_t ll_hi(const int64_t ll)
{
    uint64_t u;
    
    u = (uint64_t)ll;
    u = u >> 32u ;
    return (int32_t)u;
}

/*
 *    Description   : Extract low long from long long
 *    Param         : int64_t h : long long
 *    Return Code   : uint32_t : low part of long long
 */
Inline uint32_t ll_lo(const int64_t ll)
{
    return (uint32_t)ll;
}
  
#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* __TK_MACHINE_H__ */
