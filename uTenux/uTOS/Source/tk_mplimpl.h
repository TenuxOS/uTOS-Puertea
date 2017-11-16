/*
 *----------------------------------------------------------------------------------------
 *    Kernel/mpl
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
 *    File Name      : tk_mplimpl.h
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Variable Size Memory Pool Header definition .
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_MPLIMPL_H__
#define __TK_MPLIMPL_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#if KNL_CFG_MAX_MPLID > 0

/*
 *    Description   : Processing if the priority of wait task changes.
 *                    You need to execute with interrupt disable.
 *    Param         : TCB *tcb : task control block to change the priority
 *                    int32_t oldpri : old priority, maybe is -1
 *    Return Code   : void
 */
extern void knl_mpl_chgpri( TCB *tcb, int32_t oldpri );

/*
 *    Description   : Get object name from control block
 *    Param         : ID id :variable size memory pool id
 *                    uint8_t **name : variable size memory pool DS name pointer
 *    Return Code   : ER : Error code
 */
extern ER knl_mpl_getname(ID id, uint8_t **name);

/*
 *    Description   : Initialization of variable size memory pool control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
extern ER knl_mpl_init( void );

/*
 *    Description   : Processing if the wait task is freed
 *    Param         : TCB *tcb : task to release waiting
 *    Return Code   : ER : Error code
 */
extern void knl_mpl_relwai( TCB *tcb );

/*
 *    Description   : Create variable size memory pool
 *    Param         : T_CMPL *pk_cmpl : Information about the mpl to be created
 *    Return Code   : ID : Variable-size memory pool ID or Error code
 */
extern ID tk_cre_mpl_impl( T_CMPL *pk_cmpl );

/*
 *    Description   : Delete variable size memory pool
 *    Param         : ID mplid : Variable-size memory pool ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_mpl_impl( ID mplid );

/*
 *    Description   : Get variable size memory block
 *    Param         : ID mplid : Variable-size memory pool ID
 *                    int32_t blksz : Memory block size (in bytes)
 *                    VP* p_blk : Block start address
 *                    TMO tmout : Timeout
 *    Return Code   : ER : Error code
 */
extern ER tk_get_mpl_impl( ID mplid, int32_t blksz, VP* p_blk, TMO tmout );

/*
 *    Description   : Refer variable size memory pool state
 *    Param         : ID mplid : variable size memory pool ID
 *                    T_RMPL *pk_rmpl : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_mpl_impl( ID mplid, T_RMPL *pk_rmpl );

/*
 *    Description   : Return variable size memory block
 *    Param         : ID mplid : Variable-size memory pool ID
 *                    VP blk : Memory block start address
 *    Return Code   : ER : Error code
 */
extern ER tk_rel_mpl_impl( ID mplid, VP blk );

#if TK_USE_DBGSPT

/*
 *    Description   : Refer variable size memory pool object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_mpl_impl( ID list[], int32_t nent );

/*
 *    Description   : Reference variable size memory pool wait queue
 *    Param         : ID mplid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
extern int32_t td_mpl_que_impl( ID mplid, ID list[], int32_t nent );

/*
 *    Description   : Reference variable size memory pool state
 *    Param         : ID mplid : Object ID
 *                    TD_RMPL *pk_rmpl : Address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_mpl_impl( ID mplid, TD_RMPL *pk_rmpl );

#endif /* TK_USE_DBGSPT */
#endif /* KNL_CFG_MAX_MPLID */

#ifdef __cplusplus
}
#endif

#endif /* __TK_MPLIMPL_H__ */
