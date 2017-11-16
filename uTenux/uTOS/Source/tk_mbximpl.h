/*
 *----------------------------------------------------------------------------------------
 *    Kernel/mbx
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
 *    File Name      : tk_mbximpl.h
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Mailbox Header definition .
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_MBXIMPL_H__
#define __TK_MBXIMPL_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#if KNL_CFG_MAX_MBXID > 0

/*
 *    Description   : Processing if the priority of wait task changes
 *    Param         : TCB *tcb : task control block to change the priority
 *                    int32_t oldpri : old priority, maybe is -1
 *    Return Code   : void
 */
extern void knl_mbx_chgpri( TCB *tcb, int32_t oldpri );

/*
 *    Description   : Get object name from control block
 *    Param         : ID id : mailbox id
 *                    uint8_t **name : mailbox DS name pointer
 *    Return Code   : ER : Error code
 */
extern ER knl_mbx_getname(ID id, uint8_t **name);

/*
 *    Description   : Initialization of mailbox control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
extern ER knl_mbx_init( void );

/*
 *    Description   : Create mailbox
 *    Param         : T_CMBX *pk_cmbx : Information about the mailbox to be created
 *    Return Code   : ID : mailbox ID or Error code
 */
extern ID tk_cre_mbx_impl( T_CMBX *pk_cmbx );

/*
 *    Description   : Delete mailbox
 *    Param         : ID semid : mailbox ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_mbx_impl( ID mbxid );

/*
 *    Description   : Receive from mailbox
 *    Param         : ID mbxid : mailbox ID
 *                    T_MSG **ppk_msg : Start address of message packet
 *                    TMO tmou : timeout
 *    Return Code   : ER : Error code
 */
extern ER tk_rcv_mbx_impl( ID mbxid, T_MSG **ppk_msg, TMO tmout );

/*
 *    Description   : Refer mailbox state
 *    Param         : ID mbxid : mailbox ID
 *                    T_RMBX *pk_rmbx : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_mbx_impl( ID mbxid, T_RMBX *pk_rmbx );

/*
 *    Description   : Send to mailbox
 *    Param         : ID mbxid : mailbox ID
 *                    T_MSG *pk_msg : Message packet address
 *    Return Code   : ER : Error code
 */
extern ER tk_snd_mbx_impl( ID mbxid, T_MSG *pk_msg );

#if TK_USE_DBGSPT

/*
 *    Description   : Refer mailbox object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_mbx_impl( ID list[], int32_t nent );

/*
 *    Description   : Reference mailbox wait queue
 *    Param         : ID mbxid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
extern int32_t td_mbx_que_impl( ID mbxid, ID list[], int32_t nent );

/*
 *    Description   : Reference mailbox object state
 *    Param         : ID mbxid : Object ID
 *                    TD_RMBX *pk_rmbx : Address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_mbx_impl( ID mbxid, TD_RMBX *pk_rmbx );

#endif /* TK_USE_DBGSPT */
#endif /* KNL_CFG_MAX_MBXID */

#ifdef __cplusplus
}
#endif

#endif /* __TK_MBXIMPL_H__ */
