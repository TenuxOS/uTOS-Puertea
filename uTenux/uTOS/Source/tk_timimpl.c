/*
 *----------------------------------------------------------------------------------------
 *    Kernel/tim
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
 *    File Name      : tk_timimpl.c
 *    Create Date    : 2012/7/9-2014/5/4
 *    Author         : WangShb
 *    Description    : uT/OS Time Management Function.
 *----------------------------------------------------------------------------------------
 */

#include "tk_timimpl.h"

#ifdef KNL_CFG_USE_FUNC_TK_GET_ETM
/*
 *    Function Name : tk_get_etm_impl
 *    Create Date   : 2012/7/9-2014/5/4
 *    Author        : wangshb
 *    Description   : Get Next Tick Event Relative Time
 *    Param         : void
 *    Return Code   : RELTIM : next tick event relative time
 */
SYSCALL RELTIM tk_get_etm_impl( void )
{
    RELTIM rtim;
    
    KNL_TSK_BEGIN_CRITICAL_SECTION
    rtim = knl_tck_getetim();
    KNL_TSK_END_CRITICAL_SECTION

    return rtim;
}
#endif /* KNL_CFG_USE_FUNC_TK_GET_ETM */

#ifdef KNL_CFG_USE_FUNC_TK_GET_OTM
/*
 *    Function Name : tk_get_otm_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get Operating Time
 *    Param         : SYSTIM *pk_tim : Address of packet for returning operating time
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_get_otm_impl( SYSTIM *pk_tim )
{
    KNL_TSK_BEGIN_CRITICAL_SECTION
    *pk_tim = knl_tck_tosystim(knl_tck_current_time);
    KNL_TSK_END_CRITICAL_SECTION

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_TK_GET_OTM */

#ifdef KNL_CFG_USE_FUNC_TK_GET_TIM
/*
 *    Function Name : tk_get_tim_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get Time
 *    Param         : SYSTIM *pk_tim : Address of a packet to return current time
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_get_tim_impl( SYSTIM *pk_tim )
{
    KNL_TSK_BEGIN_CRITICAL_SECTION
    *pk_tim = knl_tck_tosystim(KNL_TCK_REAL_TIME);
    KNL_TSK_END_CRITICAL_SECTION

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_TK_GET_TIM */

#ifdef KNL_CFG_USE_FUNC_TK_SET_TIM
/*
 *    Function Name : tk_set_tim_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Set Time
 *    Param         : SYSTIM *pk_tim : Address of current time packet
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_set_tim_impl( SYSTIM *pk_tim )
{
    KNL_CHK_PAR(pk_tim->hi >= 0)

    KNL_TSK_BEGIN_CRITICAL_SECTION
    knl_tck_realtime_ofs = ll_sub(knl_tck_tolsystim(pk_tim), knl_tck_current_time);
    KNL_TSK_END_CRITICAL_SECTION

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_TK_SET_TIM */

/*
 *  Debug support function
 */
#if TK_USE_DBGSPT

#ifdef KNL_CFG_USE_FUNC_TD_GET_OTM
/*
 *    Function Name : td_get_otm_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get System Operating Time
 *    Param         : SYSTIM *tim : Address of packet for returning operating time (ms)
 *                    uint32_t *ofs : Location for returning elapsed time from tim (nanoseconds
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_get_otm_impl( SYSTIM *tim, uint32_t *ofs )
{
    KNL_INT_BEGIN_DISABLE
    *ofs = knl_tmr_getnsec();
    *tim = knl_tck_tosystim(knl_tck_current_time);
    KNL_INT_END_DISABLE

    return E_OK;
}

#endif /* KNL_CFG_USE_FUNC_TD_GET_OTM */

#ifdef KNL_CFG_USE_FUNC_TD_GET_TIM
/*
 *    Function Name : td_get_tim_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get System Time
 *    Param         : SYSTIM *tim : Address of packet for returning current time (ms)
 *                    uint32_t *ofs : Location for returning elapsed time from tim (nanoseconds)
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_get_tim_impl( SYSTIM *tim, uint32_t *ofs )
{
    KNL_INT_BEGIN_DISABLE
    *ofs = knl_tmr_getnsec();
    *tim = knl_tck_tosystim(KNL_TCK_REAL_TIME);
    KNL_INT_END_DISABLE

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_TD_GET_TIM */

#endif /* TK_USE_DBGSPT */
