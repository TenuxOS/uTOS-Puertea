/*
 *----------------------------------------------------------------------------------------
 *    Config
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
 *    CPU:        STM32L152RB
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tk_config_depend.h
 *    Create Date    : 2013/9/23-2017/11/07
 *    Author         : Zhangzl-Cuihl/Wangshb
 *    Description    : System Configuration Definition.
 *----------------------------------------------------------------------------------------
 */
#ifndef __TK_CONFIG_DEPEND_H__
#define __TK_CONFIG_DEPEND_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Vector ROM region and Number definition, must be same as with startup code.
 */
#define TK_ROM_VECTORAREA_TOP     0x08000000UL   /* ROM VCETOR table start      */
#define TK_ROM_VECTORAREA_END     0x080000f4UL   /* ROM VCETOR table end   244B */
#define TK_ROM_VECTOR_NUMBER              61U    /* ROM VCETOR table size       */

/*
 * Kernel RAM region definition, ram region defined in link script must be same.
 */
#define TK_RAM_TOTALAREA_TOP      0x20000000UL   /* Total Internal RAM start    */
#define TK_RAM_VECTORAREA_TOP     0x20000000UL   /* RAM VCETOR table start      */
#define TK_RAM_VECTORAREA_END     0x200000f4UL   /* RAM VCETOR table end   0.2k */
#define TK_RAM_BSSDATAAREA_TOP    0x200000f4UL   /* Bss and Data area start     */
#define TK_RAM_BSSDATAAREA_END    0x20001c00UL   /* Bss and Data area end  6.8k */
#define TK_RAM_SYSTEMAREA_TOP     0x20001c00UL   /* Kernel malloc area start    */
#define TK_RAM_SYSTEMAREA_END     0x20003c00UL   /* Kernel malloc area end   8k */
#define TK_RAM_USERAREA_TOP       0x20003c00UL   /* User manual area start      */
#define TK_RAM_USERAREA_END       0x20004000UL   /* User manual area end     1k */
#define TK_RAM_UPSTACK_TOP        0x20004000UL   /* Stack startup area start    */
#define TK_RAM_UPSTACK_END        0x20004000UL   /* Stack startup area end   0k */
#define TK_RAM_TOTALAREA_END      0x20004000UL   /* Total Internal RAM end  16k */
#define TK_RAM_VECTOR_NUMBER              61U    /* RAM VCETOR table size       */

/*
 * Use Chip Deep Sleep ( Stop and Standby ) function
 */
#define TK_USE_POW_DEEP           1U

/*
 * Kernel tick source timer clock function and definition
 */
#define TK_USE_SYSTICK_LESS       0U             /* 0: use systick, 1: use other timer */
#if !TK_USE_SYSTICK_LESS
#define KNL_CFG_TIMER_CLOCK       16U            /* Timer clock input(MHz)£¬select MCK */
#else
#define KNL_CFG_TIMER_CLOCK       16U            /* Timer clock input(MHz)£¬select PCLK1 */
#endif
#define KNL_CFG_TIMER_INTLEVEL    0U             /* Timer interrupt level */
#define KNL_CFG_TIMER_PERIOD      10U            /* Timer period(ms), between 1 and 50 */


/*
 *  System object number config
 */
#define KNL_CFG_MAX_TSKID         10
#define KNL_CFG_MAX_SEMID         4
#define KNL_CFG_MAX_FLGID         4
#define KNL_CFG_MAX_MBXID         2
#define KNL_CFG_MAX_MTXID         2
#define KNL_CFG_MAX_MBFID         2
#define KNL_CFG_MAX_PORID         2
#define KNL_CFG_MAX_MPLID         2
#define KNL_CFG_MAX_MPFID         2
#define KNL_CFG_MAX_CYCID         2
#define KNL_CFG_MAX_ALMID         2

/*
 * Task priority config
 */
#define KNL_CFG_MAX_PRI           32U


/*
 * Use dynamic memory allocation when kernel init
 */
#define TK_USE_MALLOC             1U

/*
 * Use dynamic exception and high level programming language handler support define
 */
#define TK_USE_INT_DEFHDR         1U             /* Dynamic exception handler support */
#define TK_USE_INT_HLLHDR         1U             /* High level program language support */



/*
 * Use clean-up sequence when kernel exit
 */
#define KNL_CFG_USE_CLEANUP       1U

/*
 * Debugger support function
 *   0: Invalid
 *   1: Valid
 */
#define TK_USE_DBGSPT             0U

/*
 * Use program trace function (in debugger support)
 */
#define TK_USE_HOOK_TRACE         0U

/*
 * Use object name to each control block
 *   0: Do not use object name
 *   1: Use object name
 */
#define TK_USE_OBJ_NAME           1U
#define TK_OBJ_NAME_LENGTH        8U             /* Object name length */

/*
 * Use tm_monitor to output kernel message
 */
#define TK_USE_TM_MONITOR         1U

/*
 * Output (error) messages from Kernel
 *   0: Do not output message
 *   1: Output message
 */
#define TK_USE_MESSAGE            1U

/*
 * Predefined boot message
 */
#define KNL_CFG_BOOT_MESSAGE \
            "\n" \
            "-------------------------------------------------------\n" \
            "                       micro Tenux                     \n" \
            "             Supported MCU is ST STM32L152RB           \n" \
            "                       (build 0400)                    \n" \
            "  Copyright(c) 2008-2017 by Tenux Open Source Society  \n" \
            "-------------------------------------------------------\n" \
            "\n\0"

#ifdef __cplusplus
}
#endif

#endif /* __TK_CONFIG_DEPEND_H__ */
