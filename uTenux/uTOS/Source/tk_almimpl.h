/*
 *----------------------------------------------------------------------------------------
 *    Kernel/alm
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
 *    File Name      : tk_almimpl.h
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Alarm Management Header definition .
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_ALMIMPL_H__
#define __TK_ALMIMPL_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#if KNL_CFG_MAX_ALMID > 0

/*
 *    Description   : Get object name from control block
 *    Param         : ID id : alarm handler id
 *                    uint8_t **name : alarm handler DS name pointer
 *    Return Code   : ER : Error code
 */
extern ER knl_alm_getname(ID id, uint8_t **name);

/*
 *    Description   : Initialization of alarm handler control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
extern ER knl_alm_init( void );

/*
 *    Description   : Create alarm handler
 *    Param         : T_CALM *pk_calm : Address of alarm handler definition packet
 *    Return Code   : ID : alarm handler ID or Error code
 */
extern ID tk_cre_alm_impl( T_CALM *pk_calm );

/*
 *    Description   : Delete alarm handler
 *    Param         : ID almid : alarm handler ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_alm_impl( ID almid );

/*
 *    Description   : Refer alarm handler state
 *    Param         : ID almid : alarm handle ID
 *                    T_RALM *pk_ralm : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_alm_impl( ID almid, T_RALM *pk_ralm );

/*
 *    Description   : Start alarm handler
 *    Param         : ID almid : alarm handler ID
 *                    RELTIM almtim : Alarm handler start time (alarm time)
 *    Return Code   : ER : Error code
 */
extern ER tk_sta_alm_impl( ID almid, RELTIM almtim );

/*
 *    Description   : Stop alarm handler
 *    Param         : ID almid : alarm handler ID
 *    Return Code   : ER : Error code
 */
extern ER tk_stp_alm_impl( ID almid );

#if TK_USE_DBGSPT

/*
 *    Description   : Refer alarm handler object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_alm_impl( ID list[], int32_t nent );

/*
 *    Description   : Reference alarm handler object state
 *    Param         : ID almid : Object ID
 *                    TD_RALM *pk_ralm : Address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_alm_impl( ID almid, TD_RALM *pk_ralm );

#endif /* TK_USE_DBGSPT */
#endif /* KNL_CFG_MAX_ALMID */

#ifdef __cplusplus
}
#endif

#endif /* __TK_ALMIMPL_H__ */
