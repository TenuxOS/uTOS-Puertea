/*
 *----------------------------------------------------------------------------------------
 *    Kernel/sys
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
 *    File Name      : tk_sysimpl.h
 *    Create Date    : 2012/7/9-2014/5/1
 *    Author         : WangShb
 *    Description    : uT/OS System Management Header definition .
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_SYSIMPL_H__
#define __TK_SYSIMPL_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 *    Description   : Get DS object name
 *    Param         : uint32_t type : object type
 *                    ID id : object ID
 *                    uint8_t *dsname : address to return DS object name
 *    Return Code   : ER : Error code
 */
extern ER knl_sys_getname( uint32_t objtype, ID objid, uint8_t **name);

/*
 *    Description   : Dispatch disable
 *    Param         : void
 *    Return Code   : ER : Error code
 */
extern ER tk_dis_dsp_impl( void );

/*
 *    Description   : Enable Dispatch
 *    Param         : void
 *    Return Code   : ER : Error code
 */
extern ER tk_ena_dsp_impl( void );

/*
 *    Description   : Output shutdown message, Execute finalization and stop or dead loop
 *    Param         : void
 *    Return Code   : void
 */
extern void tk_ext_sys( void );

/*
 *    Description   : Get Task Identifier
 *    Param         : void
 *    Return Code   : ID : ID of the task in RUN state
 */
extern ID tk_get_tid_impl( void );

/*
 *    Description   : Reference System Status
 *    Param         : T_RSYS *pk_rsys : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_sys_impl( T_RSYS *pk_rsys );

/*
 *    Description   : Refer version information,If there is no kernel version information,
 *                    If there is no kernel version information,set 0 in each information.
 *                    (Do NOT cause errors.)
 *    Param         : T_RVER *pk_rver : Start address of packet for version information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_ver_impl( T_RVER *pk_rver );

/*
 *    Description   : Rotate ready queue
 *    Param         : PRI tskpri : Task priority
 *    Return Code   : ER : Error code
 */
extern ER tk_rot_rdq_impl( PRI tskpri );

/*
 *    Description   : Set Power-saving mode
 *    Param         : uint32_t powmode : power-saving mode
 *    Return Code   : ER : Error code
 */
extern ER tk_set_pow_impl( uint32_t powmode );

/*
 *    Description   : Initialize kernel and create/start initial task
 *    Param         : T_CTSK *pk_ctsk : Information about init task
 *    Return Code   : void
 */
extern void tk_sta_sys( T_CTSK *pk_ctsk );

#if TK_USE_DBGSPT

/*
 *    Description   : Refer to DS Object Name
 *    Param         : uint32_t type : object type
 *                    ID id : object ID
 *                    uint8_t *dsname : address to return DS object name
 *    Return Code   : ER : Error code
 */
extern ER td_ref_dsname_impl( uint32_t type, ID id, uint8_t *dsname );

/*
 *    Description   : Reference System Status
 *    Param         : TD_RSYS *pk_rsys : Address of packet for returning status
 *                    information
 *    Return Code   : ER : Error code
 */
extern ER td_ref_sys_impl( TD_RSYS *pk_rsys );

/*
 *    Description   : Set DS Object Name
 *    Param         : uint32_t type : object type
 *                    ID id : object ID
 *                    uint8_t *dsname : DS object name to be set
 *    Return Code   : ER : Error code
 */
extern ER td_set_dsname_impl( uint32_t type, ID id, uint8_t *dsname );

#endif /* TK_USE_DBGSPT */

#ifdef __cplusplus
}
#endif

#endif /* __TK_SYSIMPL_H__ */
