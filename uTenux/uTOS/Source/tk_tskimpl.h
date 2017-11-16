/*
 *----------------------------------------------------------------------------------------
 *    Kernel/tsk
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
 *    File Name      : tk_tskimpl.h
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Task Management and  Synchronize Header definition .
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_TSKIMPL_H__
#define __TK_TSKIMPL_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 *    Description   : Cancel Wakeup Task
 *    Param         : ID tskid : Task ID
 *    Return Code   : int32_t : Number of queued wakeup requests or Error Code
 */
extern int32_t tk_can_wup_impl( ID tskid );

/*
 *    Description   : Change Task Priority
 *    Param         : ID tskid : Task ID
 *                    PRI tskpri : Task priority
 *    Return Code   : ER : Error code
 */
extern ER tk_chg_pri_impl( ID tskid, PRI tskpri );

/*
 *    Description   : Create Task
 *    Param         : T_CTSK *pk_ctsk: Information about the task to be created
 *    Return Code   : ID : Task ID or Error Code
 */
extern ID tk_cre_tsk_impl( T_CTSK *pk_ctsk );

/*
 *    Description   : Delete Task
 *    Param         : ID tskid : Task I
 *    Return Code   : ER : Error code
 */
extern ER tk_del_tsk_impl( ID tskid );

/*
 *    Description   : Delay Task
 *    Param         : RELTIM dlytim : Delay time
 *    Return Code   : ER : Error code
 */
extern ER tk_dly_tsk_impl( RELTIM dlytim );

/*
 *    Description   : Exit and Delete Task
 *    Param         : void
 *    Return Code   : void
 */
extern void tk_exd_tsk_impl( void );

/*
 *    Description   : Exit Task
 *    Param         : void
 *    Return Code   : void
 */
extern void tk_ext_tsk_impl( void );

/*
 *    Description   : Resume Task Force
 *    Param         : ID tskid : Task ID
 *    Return Code   : ER : Error code
 */
extern ER tk_frsm_tsk_impl( ID tskid );

/*
 *    Description   : Get Task Registers
 *    Param         : ID tskid : Task ID
 *                    T_REGS *pk_regs : General registers
 *                    T_EIT *pk_eit : Registers saved when EIT occurs
 *                    T_CREGS *pk_cregs : Control registers
 *    Return Code   : ER : Error code
 */
extern ER tk_get_reg_impl( ID tskid, T_REGS *pk_regs, T_EIT *pk_eit, T_CREGS *pk_cregs );

/*
 *    Description   : Reference Task Status
 *    Param         : ID tskid : Task ID
 *                    T_RTSK *pk_rtsk : Address of packet for returning task status
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_tsk_impl( ID tskid, T_RTSK *pk_rtsk );

/*
 *    Description   : Release Wait
 *    Param         : ID tskid : Task ID
 *    Return Code   : ER : Error code
 */
extern ER tk_rel_wai_impl( ID tskid );

/*
 *    Description   : Resume Task
 *    Param         : ID tskid : Task ID
 *    Return Code   : ER : Error code
 */
extern ER tk_rsm_tsk_impl( ID tskid );

/*
 *    Description   : Set Task Registers
 *    Param         : ID tskid : Task ID
 *                    T_REGS *pk_regs : General registers
 *                    T_EIT *pk_eit : Registers saved when EIT occurs
 *                    T_CREGS *pk_cregs : Control registers
 *    Return Code   : ER : Error code
 */
extern ER tk_set_reg_impl( ID tskid, T_REGS *pk_regs, T_EIT *pk_eit, T_CREGS *pk_cregs );

/*
 *    Description   : Sleep Task,Move its own task state to wait state
 *    Param         : TMO tmout : Timeout
 *    Return Code   : ER : Error code
 */
extern ER tk_slp_tsk_impl( TMO tmout );

/*
 *    Description   : Start Task
 *    Param         : ID tskid : Task ID
 *                    int32_t stacd : Task start code
 *    Return Code   : ER : Error code
 */
extern ER tk_sta_tsk_impl( ID tskid, int32_t stacd );

/*
 *    Description   : Suspend Task
 *    Param         : ID tskid : Task ID
 *    Return Code   : ER : Error code
 */
extern ER tk_sus_tsk_impl( ID tskid );

/*
 *    Description   : Terminate Task
 *    Param         : ID tskid : Task ID
 *    Return Code   : ER : Error code
 */
extern ER tk_ter_tsk_impl( ID tskid );

/*
 *    Description   : Wakeup Task
 *    Param         : ID tskid : Task ID
 *    Return Code   : ER : Error code
 */
extern ER tk_wup_tsk_impl( ID tskid );

#if TK_USE_DBGSPT

/*
 *    Description   : Get Task Register
 *    Param         : ID tskid : Task ID (TSK_SELF cannot be designated
 *                    T_REGS *regs : General registers
 *                    T_EIT *eit : Registers saved when exception is raised
 *                    T_CREGS *cregs : Control registers
 *    Return Code   : ER : Error code
 */
extern ER td_get_reg_impl( ID tskid, T_REGS *regs, T_EIT *eit, T_CREGS *cregs );

/*
 *    Description   : Reference Task Statistics
 *    Param         : ID tskid : Task ID (TSK_SELF can be designated)
 *                    TD_ITSK *pk_itsk : Address of packet for returning task statistics
 *                    bool8_t clr : Task statistics clear flag
 *    Return Code   : ER : Error code
 */
extern ER td_inf_tsk_impl( ID tskid, TD_ITSK *pk_itsk, bool8_t clr );

/*
 *    Description   : Reference Object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error Code
 */
extern int32_t td_lst_tsk_impl( ID list[], int32_t nent );

/*
 *    Description   : Get Task Precedence
 *    Param         : PRI pri : Task priority
 *                    ID list[] : Location of task ID list
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of priority pri tasks in a run state or Error Code
 */
extern int32_t td_rdy_que_impl( PRI pri, ID list[], int32_t nent );

/*
 *    Description   : Reference Task State
 *    Param         : ID tskid : Task ID (TSK_SELF can be specified)
 *                    TD_RTSK *pk_rtsk : Address of Packet for returning the task state
 *    Return Code   : ER : Error code
 */
extern ER td_ref_tsk_impl( ID tskid, TD_RTSK *pk_rtsk );

/*
 *    Description   : Set Task Register
 *    Param         : ID tskid : Task ID (TSK_SELF cannot be specified)
 *                    T_REGS *regs : General registers
 *                    T_EIT *eit : Registers saved when exception is raised
 *                    T_CREGS *cregs : Control registers
 *    Return Code   : ER : Error code
 */
extern ER td_set_reg_impl( ID tskid, T_REGS *regs, T_EIT *eit, T_CREGS *cregs );

#endif /* TK_USE_DBGSPT */

#ifdef __cplusplus
}
#endif

#endif /* __TK_TSKIMPL_H__ */
