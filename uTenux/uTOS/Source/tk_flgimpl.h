/*
 *----------------------------------------------------------------------------------------
 *    Kernel/flg
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
 *    File Name      : tk_flgimpl.h
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Event Flag Header definition .
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_FLGIMPL_H__
#define __TK_FLGIMPL_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#if KNL_CFG_MAX_FLGID > 0

/*
 *    Description   : Get object name from control block
 *    Param         : ID id : event flag id
 *                    uint8_t **name : event flag DS name pointer
 *    Return Code   : ER : Error code
 */
extern ER knl_flg_getname(ID id, uint8_t **name);

/*
 *    Description   : Initialization of event flag control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
extern ER knl_flg_init( void );

/*
 *    Description   : Clear event flag
 *    Param         : ID flgid : event flag ID
 *                    uint32_t clrptn : Bit pattern to be clear
 *    Return Code   : ER : Error code
 */
extern ER tk_clr_flg_impl( ID flgid, uint32_t clrptn );

/*
 *    Description   : Create event flag
 *    Param         : T_CFLG *pk_cflg : Information about the event flag to be created
 *    Return Code   : ID : event flag ID or Error code
 */
extern ID tk_cre_flg_impl( T_CFLG *pk_cflg );

/*
 *    Description   : Delete event flag
 *    Param         : ID flgid : event flag ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_flg_impl( ID flgid );

/*
 *    Description   : Refer event flag state
 *    Param         : ID flgid : event flag ID
 *                    T_RFLG *pk_rflg : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_flg_impl( ID flgid, T_RFLG *pk_rflg );

/*
 *    Description   : Set event flag
 *    Param         : ID flgid : event flag ID
 *                    uint32_t setptn : Bit pattern to be set
 *    Return Code   : ER : Error code
 */
extern ER tk_set_flg_impl( ID flgid, uint32_t setptn );

/*
 *    Description   : Wait event flag
 *    Param         : ID flgid : event flag ID
 *                    uint32_t waiptn : Wait bit pattern
 *                    uint32_t wfmode : Wait release condition
 *                    uint32_t *p_flgptn : Event flag bit pattern
 *                    TMO tmout : timeout
 *    Return Code   : ER : Error code
 */
extern ER tk_wai_flg_impl( ID flgid, uint32_t waiptn, uint32_t wfmode, uint32_t *p_flgptn, TMO tmout);

#if TK_USE_DBGSPT

/*
 *    Description   : Reference event flag wait queue
 *    Param         : ID flgid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
extern int32_t td_flg_que_impl( ID flgid, ID list[], int32_t nent );

/*
 *    Description   : Refer event flag object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_flg_impl( ID list[], int32_t nent );

/*
 *    Description   : Reference event flag object state
 *    Param         : ID flgid : Object ID
 *                    TD_RFLG *pk_rflg : Address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_flg_impl( ID flgid, TD_RFLG *pk_rflg );

#endif /* TK_USE_DBGSPT */
#endif /* KNL_CFG_MAX_FLGID */

#ifdef __cplusplus
}
#endif

#endif /* __TK_FLGIMPL_H__ */
