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
 *    CPU:        NONE
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tk_intimpl.c
 *    Create Date    : 2012/7/9-2014/5/1
 *    Author         : WangShb
 *    Description    : uT/OS Interrupt Management Function.
 *----------------------------------------------------------------------------------------
 */

#include "tk_intimpl.h"

#if TK_USE_INT_DEFHDR

/*
 * High level programming language interrupt handler table
 */
#ifdef KNL_CFG_USE_FUNC_KNL_INT_HLLHDR
Zeroinit FP knl_int_hllhdr[TK_RAM_VECTOR_NUMBER];
#endif /* KNL_CFG_USE_FUNC_KNL_INT_HLLHDR */

#ifdef KNL_CFG_USE_FUNC_TK_DEF_INT
/*
 *    Function Name : tk_def_int_impl
 *    Create Date   : 2012/7/9-2014/5/1
 *    Author        : wangshb
 *    Description   : Interrupt handler definition
 *    Param         : uint32_t dintno : Interrupt definition number
 *                    T_DINT *pk_dint : Packet of interrupt handler definition information
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_def_int_impl( uint32_t dintno, T_DINT *pk_dint )
{
    FP inthdr;

    KNL_CHK_PAR(dintno < TK_ROM_VECTOR_NUMBER)
#if !TK_USE_INT_HLLHDR
    KNL_CHK_PAR((pk_dint->intatr & TA_HLNG) == 0);
#endif

    if ( pk_dint != NULL ) {
        /* Set interrupt handler */
        KNL_CHK_RSATR(pk_dint->intatr, TA_HLNG)

        inthdr = pk_dint->inthdr;

        KNL_TSK_BEGIN_CRITICAL_SECTION
#if TK_USE_INT_HLLHDR
        if ( (pk_dint->intatr & TA_HLNG) != 0u ) {
            knl_int_hllhdr[dintno] = inthdr;
            inthdr = ( dintno < (uint32_t)TK_EXC_NUMBER ) ? knl_int_excwraphdr: knl_int_intwraphdr;
        }
#endif
        knl_int_defhdr(dintno, inthdr);
        KNL_TSK_END_CRITICAL_SECTION
    } else {
        /* Clear interrupt handler */
        KNL_TSK_BEGIN_CRITICAL_SECTION
        knl_int_defhdr(dintno, NULL);
#if TK_USE_INT_HLLHDR
        knl_int_hllhdr[dintno] = NULL;
#endif
        KNL_TSK_END_CRITICAL_SECTION
    }

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_TK_DEF_INT */

#endif /* TK_USE_INT_DEFHDR */
