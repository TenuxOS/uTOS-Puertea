/*
 *----------------------------------------------------------------------------------------
 *    Kernel/cyc
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
 *    File Name      : tk_cycimpl.h
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Cyclic Management Header definition .
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_CYCIMPL_H__
#define __TK_CYCIMPL_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#if KNL_CFG_MAX_CYCID > 0

/*
 *    Description   : Get object name from control block
 *    Param         : ID id : cyclic handler id
 *                    uint8_t **name : cyclic handler DS name pointer
 *    Return Code   : ER : Error code
 */
extern ER knl_cyc_getname(ID id, uint8_t **name);

/*
 *    Description   : Initialization of cyclic handler control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
extern ER knl_cyc_init( void );

/*
 *    Description   : Create cyclic handler
 *    Param         : T_CCYC *pk_ccyc : Address of cyclic handler definition packet
 *    Return Code   : ID : cyclic handler ID or Error code
 */
extern ID tk_cre_cyc_impl( T_CCYC *pk_ccyc );

/*
 *    Description   : Delete cyclic handler
 *    Param         : ID cycid : cyclic handler ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_cyc_impl( ID cycid );

/*
 *    Description   : Refer cyclic handler state
 *    Param         : ID cycid : cyclic handle ID
 *                    T_RCYC* pk_rcyc : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_cyc_impl( ID cycid, T_RCYC* pk_rcyc );

/*
 *    Description   : Start cyclic handler
 *    Param         : ID cycid : cyclic handler ID
 *    Return Code   : ER : Error code
 */
extern ER tk_sta_cyc_impl( ID cycid );

/*
 *    Description   : Stop cyclic handler
 *    Param         : ID cycid : cyclic handler ID
 *    Return Code   : ER : Error code
 */
extern ER tk_stp_cyc_impl( ID cycid );

#if TK_USE_DBGSPT

/*
 *    Description   : Refer cyclic handler object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_cyc_impl( ID list[], int32_t nent );

/*
 *    Description   : Reference cyclic handler object state
 *    Param         : ID cycid : Object ID
 *                    TD_RCYC* pk_rcyc : Address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_cyc_impl( ID cycid, TD_RCYC* pk_rcyc );

#endif /* TK_USE_DBGSPT */
#endif /* KNL_CFG_MAX_CYCID */

#ifdef __cplusplus
}
#endif

#endif /* __TK_CYCIMPL_H__ */
