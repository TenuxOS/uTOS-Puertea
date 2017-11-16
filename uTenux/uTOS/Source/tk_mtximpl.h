/*
 *----------------------------------------------------------------------------------------
 *    Kernel/mtx
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
 *    File Name      : tk_mtximpl.h
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Mutex Header definition .
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_MTXIMPL_H__
#define __TK_MTXIMPL_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#if KNL_CFG_MAX_MTXID > 0

#ifndef KNL_MTX_MTXCB_TYPEDEF
#define KNL_MTX_MTXCB_TYPEDEF
typedef struct mutex_control_block MTXCB;
#endif

/*
 *    Description   : Processing if the priority of wait task changes
 *    Param         : TCB *tcb : task control block to change the priority
 *                    int32_t oldpri : old priority, maybe is -1
 *    Return Code   : void
 */
extern void knl_mtx_chgpri( TCB *tcb, int32_t oldpri );

/*
 *    Description   : Get object name from control block
 *    Param         : ID id : mutex id
 *                    uint8_t **name : mutex DS name pointer
 *    Return Code   : ER : Error code
 */
extern ER knl_mtx_getname(ID id, uint8_t **name);

/*
 *    Description   : Initialization of mutex control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
extern ER knl_mtx_init(void);

/*
 *    Description   : Limit the priority change by mutex at task priority change
 *                      1.If the 'tcb' task locks mutex, cannot set lower priority than the
 *                    highest priority in all mutexes which hold lock. In such case,
 *                    return the highest priority of locked mutex.
 *                      2.If mutex with TA_CEILING attribute is locked or waiting to be locked,
 *                    cannot set higher priority than the lowest within the highest
 *                    priority limit of mutex with TA_CEILING attribute.
 *                    In this case, return E_ILUSE.
 *                      3.Other than above, return the 'priority'.
 *    Param         : TCB *tcb : task control block to change the priority
 *                    PRI priority : new priority
 *    Return Code   : int32_t : priority to return, E_ILUSE if error
 */
extern int32_t knl_mtx_limpri( TCB *tcb, PRI priority );

/*
 *    Description   : Release the lock and delete it from list, and then adjust the
 *                    priority of task.
 *                    Set the highest priority between listed below:
 *                    (A) The highest priority in all mutexes in which 'tcb' task locks.
 *                    (B) The base priority of 'tcb' task.
 *    Param         : TCB *tcb : task control block with mutex
 *                    MTXCB *relmtxcb : mutex control block to release
 *    Return Code   : void
 */
extern void knl_mtx_release( TCB *tcb, MTXCB *relmtxcb );

/*
 *    Description   : Processing if the wait task is released (For TA_INHERIT only)
 *    Param         : TCB *tcb : task to release waiting
 *    Return Code   : void
 */
extern void knl_mtx_relwai( TCB *tcb );

/*
 *    Description   : Free mutex when task is terminated
 *                    Free all mutexes which the task holds.
 *                    Do not need to handle mutex list and priority of terminated task.
 *    Param         : TCB *tcb : task control block which is terminated
 *    Return Code   : void
 */
extern void knl_mtx_sigall( TCB *tcb );

/*
 *    Description   : Create mutex
 *    Param         : T_CMTX *pk_cmtx : Information about the mutex to be created
 *    Return Code   : ID : Mutex ID or Error code
 */
extern ID tk_cre_mtx_impl( T_CMTX *pk_cmtx );

/*
 *    Description   : Delete mutex
 *    Param         : ID mtxid : Mutex ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_mtx_impl( ID mtxid );

/*
 *    Description   : Lock mutex
 *    Param         : ID mtxid : Mutex ID
 *                    TMO tmout : timeout
 *    Return Code   : ER : Error code
 */
extern ER tk_loc_mtx_impl( ID mtxid, TMO tmout );

/*
 *    Description   : Refer mutex state
 *    Param         : ID mtxid : Mutex ID
 *                    T_RMTX *pk_rmtx : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_mtx_impl( ID mtxid, T_RMTX *pk_rmtx );

/*
 *    Description   : Unlock mutex
 *    Param         : ID mtxid : Mutex ID
 *    Return Code   : ER : Error code
 */
extern ER tk_unl_mtx_impl( ID mtxid );

#if TK_USE_DBGSPT

/*
 *    Description   : Refer mutex object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_mtx_impl( ID list[], int32_t nent );

/*
 *    Description   : Reference mutex wait queue
 *    Param         : ID mtxid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
extern int32_t td_mtx_que_impl( ID mtxid, ID list[], int32_t nent );

/*
 *    Description   : Refer mutex state
 *    Param         : ID mtxid : Object ID
 *                    TD_RMTX *pk_rmtx : Address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_mtx_impl( ID mtxid, TD_RMTX *pk_rmtx );

#endif /* TK_USE_DBGSPT */
#endif /* KNL_CFG_MAX_MTXID */

#ifdef __cplusplus
}
#endif

#endif /* __TK_MTXIMPL_H__ */
