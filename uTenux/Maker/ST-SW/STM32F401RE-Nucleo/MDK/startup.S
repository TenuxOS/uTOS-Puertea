/*
 *----------------------------------------------------------------------------------------
 *    Cpu
 *
 *    Copyright (C) 2008-2013 by Dalian uLoong Co.,Ltd. All rights reserved.
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
 *    CPU:        STM32F407VG Compiled by KEIL RVMDK
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : startup.S
 *    Create Date    : 2012/09/07-2014/3/7
 *    Author         : WangShb-Liumh
 *    Description    : Start up module
 *----------------------------------------------------------------------------------------
 */

/*
 * Imports external symbols
 */
    EXTERN main
    EXTERN ||Image$$EXEC_INTERROM$$RO$$Limit||
    EXTERN ||Image$$EXEC_BSSDATARAM$$RW$$Base||
    EXTERN ||Image$$EXEC_BSSDATARAM$$RW$$Limit||
    EXTERN ||Image$$EXEC_BSSDATARAM$$ZI$$Base||
    EXTERN ||Image$$EXEC_BSSDATARAM$$ZI$$Limit||
    EXTERN ||Image$$EXEC_STACKRAM$$Limit||

/*
 * Vector Table
 */
    AREA |.vector|,CODE,READONLY
    ALIGN  4
    GLOBAL __Vectors [DATA]
    GLOBAL __Vectors_End [DATA]
__Vectors
     /*    Internal Exceptions Vector Define                                        */
    DCD     ||Image$$EXEC_STACKRAM$$Limit||      /* 00: Main Stack                  */
    DCD     reset_handler                        /* 01: Reset Handler               */
    DCD     default_handler                      /* 02: NMI Handler                 */
    DCD     default_handler                      /* 03: Hard Fault Handler          */
    DCD     default_handler                      /* 04: MPU Fault Handler           */
    DCD     default_handler                      /* 05: Bus Fault Handler           */
    DCD     default_handler                      /* 06: Usage Fault Handler         */
    DCD     0                                    /* 07: Reserved                    */
    DCD     0                                    /* 08: Reserved                    */
    DCD     0                                    /* 09: Reserved                    */
    DCD     0                                    /* 10: Reserved                    */
    DCD     0                                    /* 11: SVCall Handler              */
    DCD     default_handler                      /* 12: Debug Monitor Handler       */
    DCD     0                                    /* 13: Reserved                    */
    DCD     0                                    /* 14: PendSV Handler              */
    DCD     0                                    /* 15: Systick handler             */

    /*    External Interrupts Vector Define                                         */
    DCD     default_handler                      /* 16: */
    DCD     default_handler                      /* 17: */
    DCD     default_handler                      /* 18: */
    DCD     default_handler                      /* 19: */
    DCD     default_handler                      /* 20: */
    DCD     default_handler                      /* 21: */
    DCD     default_handler                      /* 22: */
    DCD     default_handler                      /* 23: */
    DCD     default_handler                      /* 24: */
    DCD     default_handler                      /* 25: */
    DCD     default_handler                      /* 26: */
    DCD     default_handler                      /* 27: */
    DCD     default_handler                      /* 28: */
    DCD     default_handler                      /* 29: */
    DCD     default_handler                      /* 30: */
    DCD     default_handler                      /* 31: */
    DCD     default_handler                      /* 32: */
    DCD     default_handler                      /* 33: */
    DCD     default_handler                      /* 34: */
    DCD     default_handler                      /* 35: */
    DCD     default_handler                      /* 36: */
    DCD     default_handler                      /* 37: */
    DCD     default_handler                      /* 38: */
    DCD     default_handler                      /* 39: */
    DCD     default_handler                      /* 40: */
    DCD     default_handler                      /* 41: */
    DCD     default_handler                      /* 42: */
    DCD     default_handler                      /* 43: */
    DCD     default_handler                      /* 44: */
    DCD     default_handler                      /* 45: */
    DCD     default_handler                      /* 46: */
    DCD     default_handler                      /* 47: */
    DCD     default_handler                      /* 48: */
    DCD     default_handler                      /* 49: */
    DCD     default_handler                      /* 50: */
    DCD     default_handler                      /* 51: */
    DCD     default_handler                      /* 52: */
    DCD     default_handler                      /* 53: */
    DCD     default_handler                      /* 54: */
    DCD     default_handler                      /* 55: */
    DCD     default_handler                      /* 56: */
    DCD     default_handler                      /* 57: */
    DCD     default_handler                      /* 58: */
    DCD     default_handler                      /* 59: */
    DCD     default_handler                      /* 60: */
    DCD     default_handler                      /* 61: */
    DCD     default_handler                      /* 62: */
    DCD     default_handler                      /* 63: */
    DCD     default_handler                      /* 64: */
    DCD     default_handler                      /* 65: */
    DCD     default_handler                      /* 66: */
    DCD     default_handler                      /* 67: */
    DCD     default_handler                      /* 68: */
    DCD     default_handler                      /* 69: */
    DCD     default_handler                      /* 70: */
    DCD     default_handler                      /* 71: */
    DCD     default_handler                      /* 72: */
    DCD     default_handler                      /* 73: */
    DCD     default_handler                      /* 74: */
    DCD     default_handler                      /* 75: */
    DCD     default_handler                      /* 76: */
    DCD     default_handler                      /* 77: */
    DCD     default_handler                      /* 78: */
    DCD     default_handler                      /* 79: */
    DCD     default_handler                      /* 80: */
    DCD     default_handler                      /* 81: */
    DCD     default_handler                      /* 82: */
    DCD     default_handler                      /* 83: */
    DCD     default_handler                      /* 84: */
    DCD     default_handler                      /* 85: */
    DCD     default_handler                      /* 86: */
    DCD     default_handler                      /* 87: */
    DCD     default_handler                      /* 88: */
    DCD     default_handler                      /* 89: */
    DCD     default_handler                      /* 90: */
    DCD     default_handler                      /* 91: */
    DCD     default_handler                      /* 92: */
    DCD     default_handler                      /* 93: */
    DCD     default_handler                      /* 94: */
    DCD     default_handler                      /* 95: */
    DCD     default_handler                      /* 96: */
    DCD     default_handler                      /* 97: */
__Vectors_End

/*
 * Reset handle routine
 */
    AREA |.cstart|,CODE,READONLY
    NOP
    THUMB
    ALIGN  2
    GLOBAL reset_handler
reset_handler
    cpsid   i                                    /*Disable interrupt   */

/*
 *     Loop to copy data from read only memory to RAM. The ranges of copy from /to
 *      are specified by following symbols evaluated in linker script.
 *      All address must be aligned to 4 bytes boundary.
 *      ||Image$$EXEC_INTERROM$$RO$$Limit||:
 *                   End of code section, i.e., begin of data sections to copy from.
 *      ||Image$$EXEC_BSSDATARAM$$RW$$Base||:
 *                   start of RAM address range that data should be copied to.
 *      ||Image$$EXEC_BSSDATARAM$$RW$$Limit||:
 *                   end of RAM address range that data should be copied to
 */
    ldr     r1, =||Image$$EXEC_INTERROM$$RO$$Limit||   /* src address */
    ldr     r2, =||Image$$EXEC_BSSDATARAM$$RW$$Base||  /* dst address */
    ldr     r3, =||Image$$EXEC_BSSDATARAM$$RW$$Limit||
    subs    r3, r3, r2                                 /* r3 := section size */
    beq     data_done                                  /* if start == end */

data_loop
    ldmia   r1!, {r0}
    stmia   r2!, {r0}
    subs    r3,  r3, #4
    bne     data_loop                                  /* if data_size != 0 */
data_done

/*
 *     Loop to zero out BSS section, which uses following symbols in linker script.
 *      All address must be aligned to 4 bytes boundary.
 *      ||Image$$EXEC_BSSDATARAM$$ZI$$Base|| : start of BSS section.
 *      ||Image$$EXEC_BSSDATARAM$$ZI$$Limit||: end of BSS section.
 */
    ldr     r1, =||Image$$EXEC_BSSDATARAM$$ZI$$Base||  /* dst address */
    ldr     r2, =||Image$$EXEC_BSSDATARAM$$ZI$$Limit||
    subs    r2, r2, r1                                 /* r2 := section size */
    beq     bss_done                                   /* if start == end */
    mov     r0, #0

bss_loop
    stmia   r1!, {r0}
    subs    r2,  r2, #4
    bne     bss_loop                                   /* if bss_size != 0 */
bss_done

/*
 *  Goto main entry
 */
    cpsie   i                                          /*Enable interrupt   */
    ldr     r12, =main
    mov     r0, #0
    blx     r12
reset_end
    b       reset_end

/*
 * Default exeception and interrupt process handler define
 */
    GLOBAL default_handler
default_handler
    b       default_handler

    END

