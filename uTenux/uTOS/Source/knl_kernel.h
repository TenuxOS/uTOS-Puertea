/*
 *----------------------------------------------------------------------------------------
 *    Kernel/common
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
 *    File Name      : knl_kernel.h
 *    Create Date    : 2012/9/4-2014/2/28
 *    Author         : wangshb
 *    Description    : uT-Kernel Internal Common Definition head file.
 *----------------------------------------------------------------------------------------
 */

#ifndef _KNL_KERNEL_H__
#define _KNL_KERNEL_H__

#include <tk_kernel.h>
#include <tk_config.h>
#include <tm_monitor.h>

#ifndef Assembler

#ifdef CPU_ARMV4T

#define __MPU_PRESENT        0                   /* ARM7TDMI do not provide MPU         */

/*
 * ARM7TDMI Processor Exceptions Numbers
 */

typedef enum IRQn
{
        Undefined_IRQn       = -7,               /* 1 Undefined instruction Interrupt   */
        Software_IRQn        = -6,               /* 2 Software Interrupt                */
        PrefetchAbort_IRQn   = -5,               /* 3 Prefetch abort Interrupt          */
        DataAbort_IRQn       = -4,               /* 4 Data abort Interrupt              */
        /*                   = -3,                  5 Reserved Interrupt                */
        Normal_IRQn          = -2,               /* 6 Normal Interrupt                  */
        Fast_IRQn            = -1,               /* 7 Fast Interrupt                    */
}                            IRQn_Type;

#include "core_arm7.h"
#endif /* CPU_ARMV4T */

#ifdef CPU_ARMV6_M
/*
 * Cortex M0 uses 2 Bits for the Priority Levels
 */
#define __MPU_PRESENT        0                   /* M0 do not provide MPU              */
#define __NVIC_PRIO_BITS     2                   /* General 2 Bits for Priority Levels */

/*
 * Cortex-M0 Processor Exceptions Numbers
 */

typedef enum IRQn
{
        NonMaskableInt_IRQn  = -14,              /* 2  Non Maskable Interrupt           */
        HardFault_IRQn       = -13,              /* 3  Hard Fault Interrupt             */
        SVCall_IRQn          = -5,               /* 11 SV Call Interrupt                */
        PendSV_IRQn          = -2,               /* 14 Pend SV Interrupt                */
        SysTick_IRQn         = -1,               /* 15 System Tick Interrupt            */
}                            IRQn_Type;

#include "core_cm0.h"
#endif /* CPU_ARMV6_M */

#ifdef CPU_ARMV7_M
/*
 * Cortex M3 uses 4 Bits for the Priority Levels
 */
#define __MPU_PRESENT        0                   /* M3 maybe not provide MPU            */
#define __NVIC_PRIO_BITS     4                   /* General 4 Bits for Priority Levels  */

/*
 * Cortex-M3 Processor Exceptions Numbers
 */

typedef enum IRQn
{
        NonMaskableInt_IRQn  = -14,              /* 2 Non Maskable Interrupt            */
        MemoryManagement_IRQn \
                             = -12,              /* 4 Memory Management Interrupt       */
        BusFault_IRQn        = -11,              /* 5 Bus Fault Interrupt               */
        UsageFault_IRQn      = -10,              /* 6 Usage Fault Interrupt             */
        SVCall_IRQn          = -5,               /* 11 SV Call Interrupt                */
        DebugMonitor_IRQn    = -4,               /* 12 Debug Monitor Interrupt          */
        PendSV_IRQn          = -2,               /* 14 Pend SV Interrupt                */
        SysTick_IRQn         = -1,               /* 15 System Tick Interrupt            */
}                            IRQn_Type;

#include "core_cm3.h"
#endif /* CPU_ARMV7_M */

#ifdef CPU_ARMV7E_M
/*
 * Cortex M4 uses 4 Bits for the Priority Levels
 */
#define __MPU_PRESENT        0                   /* M4 provide MPU, but not used        */
#define __NVIC_PRIO_BITS     4                   /* General 4 Bits for Priority Levels  */

#if !TK_USE_FPU
#define __FPU_PRESENT        0                   /* FPU no present                      */
#else
#define __FPU_PRESENT        1                   /* FPU is present                      */
#endif /* TK_USE_FPU */

/*
 * Cortex-M4 Processor Exceptions Numbers
 */

typedef enum IRQn
{
        NonMaskableInt_IRQn  = -14,              /* 2 Non Maskable Interrupt            */
        MemoryManagement_IRQn \
                             = -12,              /* 4 Memory Management Interrupt       */
        BusFault_IRQn        = -11,              /* 5 Bus Fault Interrupt               */
        UsageFault_IRQn      = -10,              /* 6 Usage Fault Interrupt             */
        SVCall_IRQn          = -5,               /* 11 SV Call Interrupt                */
        DebugMonitor_IRQn    = -4,               /* 12 Debug Monitor Interrupt          */
        PendSV_IRQn          = -2,               /* 14 Pend SV Interrupt                */
        SysTick_IRQn         = -1,               /* 15 System Tick Interrupt            */
}                            IRQn_Type;

#include "core_cm4.h"
#endif /* CPU_ARMV7E_M */

/* ------------------------------------------------------------------------ */

#ifndef KNL_MTX_MTXCB_TYPEDEF
#define KNL_MTX_MTXCB_TYPEDEF
typedef struct mutex_control_block  MTXCB;
#endif

#ifndef KNL_TSK_TCB_TYPEDEF
#define KNL_TSK_TCB_TYPEDEF
typedef struct task_control_block   TCB;
#endif

#ifndef KNL_TSK_WSPEC_TYPEDEF
#define KNL_TSK_WSPEC_TYPEDEF
typedef struct wait_spec_block      WSPEC;
#endif

#endif /* Assembler */

#include "knl_int.h"
#include "knl_hok.h"
#include "knl_pow.h"
#include "knl_tmr.h"
#include "knl_dsp.h"
#include "knl_cpu.h"

#include "knl_chk.h"
#include "knl_bit.h"
#include "knl_que.h"
#include "knl_tck.h"
#include "knl_mem.h"
#include "knl_tsk.h"

#include "knl_ctx.h"

#endif /* _KNL_KERNEL_H__ */
