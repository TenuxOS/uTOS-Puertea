/*
 *----------------------------------------------------------------------------------------
 *    Kernel/Cpu
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
 *    File Name      : knl_cpu.c
 *    Create Date    : 2009/12/27-2014/5/30
 *    Author         : wangshb
 *    Description    : CPU-Dependent Initialization/Finalization routine function.
 *----------------------------------------------------------------------------------------
 */

#include "knl_cpu.h"

/*
 *    Function Name : knl_cpu_init
 *    Create Date   : 2009/12/27-2014/5/30
 *    Author        : wangshb
 *    Description   : CPU-dependent initialization,
 *                    Register exception handler used on OS
 *                    and Set exception priority and ram vector
 *                    and Init internal memory blcok for OS management.
 *    Param         : none
 *    Return Code   : ER ercd: E_OK is success.
 */
ER knl_cpu_init( void )
{
    uint32_t  imask;
    ER  ercd;
#if TK_USE_INT_DEFHDR
    uint32_t  vnum;
#endif /* TK_USE_INT_DEFHDR */

    ercd = E_OK;
    DI(imask);
  
    /* Set exception entry stack frame align flag for 8 Bytes align */
    knl_int_setstkalign();

    /* set PendSV priority */
    NVIC_SetPriority(PendSV_IRQn, 0xffu);

#if TK_USE_INT_DEFHDR
    /* set up ram vector from flash vector */
    vnum = (uint32_t)TK_ROM_VECTOR_NUMBER;
    knl_int_setvtor(TK_ROM_VECTORAREA_TOP, vnum, TK_RAM_VECTORAREA_TOP);

    /* Register exception handler for PENDSV,TIMER */
    knl_int_defhdr(DINTNO(PendSV_IRQn), knl_dsp_entry);
    knl_int_defhdr(DINTNO(KNL_TMR_TIMER_IRQN), knl_int_tckhdr);
#endif /* TK_USE_INT_DEFHDR */

#if TK_USE_MALLOC
    knl_mem_sysareatop = (VP)TK_RAM_SYSTEMAREA_TOP;
    knl_mem_sysareaend = (VP)TK_RAM_SYSTEMAREA_END;
    ercd = knl_mem_init();
    if( ercd < E_OK ) {
#if TK_USE_MESSAGE
        tm_putstring((uint8_t*)&"!ERROR! init_Imalloc\n");
#endif
    }
#endif

    EI(imask);
    return ercd;
}

#if KNL_CFG_USE_CLEANUP
/*
 *    Function Name : knl_cpu_shutdown
 *    Create Date   : 2009/12/27-2013/2/14
 *    Author        : wangshb
 *    Description   : CPU-dependent finalization.
 *    Param         : none
 *    Return Code   : none
 */
void knl_cpu_shutdown( void )
{
}
#endif /* KNL_CFG_USE_CLEANUP */
