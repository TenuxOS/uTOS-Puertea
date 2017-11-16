/*
 *----------------------------------------------------------------------------------------
 *    Include/tk
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
 *    File Name      : tk_syscall.h
 *    Create Date    : 2012/7/9-2014/5/4
 *    Author         : WangShb
 *    Description    : uT/OS system call common definition .
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_SYSCALL_H__
#define __TK_SYSCALL_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef Assembler

/*----------------------------------------------------------------------------------------
 * Definition for Task interface function 
 */

/*
 *    Description   : Cancel Wakeup Task
 *    Param         : ID tskid : Task ID
 *    Return Code   : int32_t : Number of queued wakeup requests or Error Code
 */
extern int32_t tk_can_wup( ID tskid );

/*
 *    Description   : Change Task Priority
 *    Param         : ID tskid : Task ID
 *                    PRI tskpri : Task priority
 *    Return Code   : ER : Error code
 */
extern ER tk_chg_pri( ID tskid, PRI tskpri );

/*
 *    Description   : Create Task
 *    Param         : T_CTSK *pk_ctsk: Information about the task to be created
 *    Return Code   : ID : Task ID or Error Code
 */
extern ID tk_cre_tsk( T_CTSK *pk_ctsk );

/*
 *    Description   : Delete Task
 *    Param         : ID tskid : Task I
 *    Return Code   : ER : Error code
 */
extern ER tk_del_tsk( ID tskid );

/*
 *    Description   : Delay Task
 *    Param         : RELTIM dlytim : Delay time
 *    Return Code   : ER : Error code
 */
extern ER tk_dly_tsk( RELTIM dlytim );

/*
 *    Description   : Exit and Delete Task
 *    Param         : void
 *    Return Code   : void
 */
extern void tk_exd_tsk( void );

/*
 *    Description   : Exit Task
 *    Param         : void
 *    Return Code   : void
 */
extern void tk_ext_tsk( void );

/*
 *    Description   : Resume Task Force
 *    Param         : ID tskid : Task ID
 *    Return Code   : ER : Error code
 */
extern ER tk_frsm_tsk( ID tskid );

/*
 *    Description   : Get Task Registers
 *    Param         : ID tskid : Task ID
 *                    T_REGS *pk_regs : General registers
 *                    T_EIT *pk_eit : Registers saved when EIT occurs
 *                    T_CREGS *pk_cregs : Control registers
 *    Return Code   : ER : Error code
 */
extern ER tk_get_reg( ID tskid, T_REGS *pk_regs, T_EIT *pk_eit, T_CREGS *pk_cregs );

/*
 *    Description   : Reference Task Status
 *    Param         : ID tskid : Task ID
 *                    T_RTSK *pk_rtsk : Address of packet for returning task status
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_tsk( ID tskid, T_RTSK *pk_rtsk );

/*
 *    Description   : Release Wait
 *    Param         : ID tskid : Task ID
 *    Return Code   : ER : Error code
 */
extern ER tk_rel_wai( ID tskid );

/*
 *    Description   : Resume Task
 *    Param         : ID tskid : Task ID
 *    Return Code   : ER : Error code
 */
extern ER tk_rsm_tsk( ID tskid );

/*
 *    Description   : Set Task Registers
 *    Param         : ID tskid : Task ID
 *                    T_REGS *pk_regs : General registers
 *                    T_EIT *pk_eit : Registers saved when EIT occurs
 *                    T_CREGS *pk_cregs : Control registers
 *    Return Code   : ER : Error code
 */
extern ER tk_set_reg( ID tskid, T_REGS *pk_regs, T_EIT *pk_eit, T_CREGS *pk_cregs );

/*
 *    Description   : Sleep Task,Move its own task state to wait state
 *    Param         : TMO tmout : Timeout
 *    Return Code   : ER : Error code
 */
extern ER tk_slp_tsk( TMO tmout );

/*
 *    Description   : Start Task
 *    Param         : ID tskid : Task ID
 *                    int32_t stacd : Task start code
 *    Return Code   : ER : Error code
 */
extern ER tk_sta_tsk( ID tskid, int32_t stacd );

/*
 *    Description   : Suspend Task
 *    Param         : ID tskid : Task ID
 *    Return Code   : ER : Error code
 */
extern ER tk_sus_tsk( ID tskid );

/*
 *    Description   : Terminate Task
 *    Param         : ID tskid : Task ID
 *    Return Code   : ER : Error code
 */
extern ER tk_ter_tsk( ID tskid );

/*
 *    Description   : Wakeup Task
 *    Param         : ID tskid : Task ID
 *    Return Code   : ER : Error code
 */
extern ER tk_wup_tsk( ID tskid );

/*
 *    Description   : Get Task Register
 *    Param         : ID tskid : Task ID (TSK_SELF cannot be designated
 *                    T_REGS *regs : General registers
 *                    T_EIT *eit : Registers saved when exception is raised
 *                    T_CREGS *cregs : Control registers
 *    Return Code   : ER : Error code
 */
extern ER td_get_reg( ID tskid, T_REGS *regs, T_EIT *eit, T_CREGS *cregs );

/*
 *    Description   : Reference Task Statistics
 *    Param         : ID tskid : Task ID (TSK_SELF can be designated)
 *                    TD_ITSK *pk_itsk : Address of packet for returning task statistics
 *                    bool8_t clr : Task statistics clear flag
 *    Return Code   : ER : Error code
 */
extern ER td_inf_tsk( ID tskid, TD_ITSK *itsk, bool8_t clr );

/*
 *    Description   : Reference Object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error Code
 */
extern int32_t td_lst_tsk( ID list[], int32_t nent );

/*
 *    Description   : Get Task Precedence
 *    Param         : PRI pri : Task priority
 *                    ID list[] : Location of task ID list
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of priority pri tasks in a run state or Error Code
 */
extern int32_t td_rdy_que( PRI pri, ID list[], int32_t nent );

/*
 *    Description   : Reference Task State
 *    Param         : ID tskid : Task ID (TSK_SELF can be specified)
 *                    TD_RTSK *pk_rtsk : Address of Packet for returning the task state
 *    Return Code   : ER : Error code
 */
extern ER td_ref_tsk( ID tskid, TD_RTSK *rtsk );

/*
 *    Description   : Set Task Register
 *    Param         : ID tskid : Task ID (TSK_SELF cannot be specified)
 *                    T_REGS *regs : General registers
 *                    T_EIT *eit : Registers saved when exception is raised
 *                    T_CREGS *cregs : Control registers
 *    Return Code   : ER : Error code
 */
extern ER td_set_reg( ID tskid, T_REGS *regs, T_EIT *eit, T_CREGS *cregs );

/*----------------------------------------------------------------------------------------
 * Definition for Semaphore interface function 
 */

/*
 *    Description   : Create semaphore
 *    Param         : T_CSEM *pk_csem : Information about the semaphore to be created
 *    Return Code   : ID : Semaphore ID or Error code
 */
extern ID tk_cre_sem( T_CSEM *pk_csem );

/*
 *    Description   : Delete semaphore
 *    Param         : ID semid : Semaphore ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_sem( ID semid );

/*
 *    Description   : Refer semaphore state
 *    Param         : ID semid : Semaphore ID
 *                    T_RSEM *pk_rsem : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_sem( ID semid, T_RSEM *pk_rsem );

/*
 *    Description   : Signal semaphore
 *    Param         : ID semid : Semaphore ID
 *                    int32_t cnt : Resource return count
 *    Return Code   : ER : Error code
 */
extern ER tk_sig_sem( ID semid, int32_t cnt );

/*
 *    Description   : Wait on semaphore
 *    Param         : ID semid : Semaphore ID
 *                    int32_t cnt : Resource request count
 *                    TMO tmou : timeout
 *    Return Code   : ER : Error code
 */
extern ER tk_wai_sem( ID semid, int32_t cnt, TMO tmout );

/*
 *    Description   : Refer object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_sem( ID list[], int32_t nent );

/*
 *    Description   : Reference object state
 *    Param         : ID semid : Object ID
 *                    TD_RSEM *pk_rsem : Address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_sem( ID semid, TD_RSEM *rsem );

/*
 *    Description   : Reference wait queue
 *    Param         : ID semid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : ER : Error code
 */
extern int32_t td_sem_que( ID semid, ID list[], int32_t nent );

/*----------------------------------------------------------------------------------------
 * Definition for Event flag interface function 
 */

/*
 *    Description   : Clear event flag
 *    Param         : ID flgid : event flag ID
 *                    uint32_t clrptn : Bit pattern to be clear
 *    Return Code   : ER : Error code
 */
extern ER tk_clr_flg( ID flgid, uint32_t clrptn );

/*
 *    Description   : Create event flag
 *    Param         : T_CFLG *pk_cflg : Information about the event flag to be created
 *    Return Code   : ID : event flag ID or Error code
 */
extern ID tk_cre_flg( T_CFLG *pk_cflg );

/*
 *    Description   : Delete event flag
 *    Param         : ID flgid : event flag ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_flg( ID flgid );

/*
 *    Description   : Refer event flag state
 *    Param         : ID flgid : event flag ID
 *                    T_RFLG *pk_rflg : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_flg( ID flgid, T_RFLG *pk_rflg );

/*
 *    Description   : Set event flag
 *    Param         : ID flgid : event flag ID
 *                    uint32_t setptn : Bit pattern to be set
 *    Return Code   : ER : Error code
 */
extern ER tk_set_flg( ID flgid, uint32_t setptn );

/*
 *    Description   : Wait event flag
 *    Param         : ID flgid : event flag ID
 *                    uint32_t waiptn : Wait bit pattern
 *                    uint32_t wfmode : Wait release condition
 *                    uint32_t *p_flgptn : Event flag bit pattern
 *                    TMO tmout : timeout
 *    Return Code   : ER : Error code
 */
extern ER tk_wai_flg( ID flgid, uint32_t waiptn, uint32_t wfmode, uint32_t *p_flgptn, TMO tmout );

/*
 *    Description   : Reference event flag wait queue
 *    Param         : ID flgid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
extern int32_t td_flg_que( ID flgid, ID list[], int32_t nent );

/*
 *    Description   : Refer event flag object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_flg( ID list[], int32_t nent );

/*
 *    Description   : Reference event flag object state
 *    Param         : ID flgid : Object ID
 *                    TD_RFLG *pk_rflg : Address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_flg( ID flgid, TD_RFLG *rflg );

/*----------------------------------------------------------------------------------------
 * Definition for Mail box interface function 
 */

/*
 *    Description   : Create mailbox
 *    Param         : T_CMBX *pk_cmbx : Information about the mailbox to be created
 *    Return Code   : ID : mailbox ID or Error code
 */
extern ID tk_cre_mbx( T_CMBX* pk_cmbx );

/*
 *    Description   : Delete mailbox
 *    Param         : ID semid : mailbox ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_mbx( ID mbxid );

/*
 *    Description   : Receive from mailbox
 *    Param         : ID mbxid : mailbox ID
 *                    T_MSG **ppk_msg : Start address of message packet
 *                    TMO tmou : timeout
 *    Return Code   : ER : Error code
 */
extern ER tk_rcv_mbx( ID mbxid, T_MSG **ppk_msg, TMO tmout );

/*
 *    Description   : Refer mailbox state
 *    Param         : ID mbxid : mailbox ID
 *                    T_RMBX *pk_rmbx : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_mbx( ID mbxid, T_RMBX *pk_rmbx );

/*
 *    Description   : Send to mailbox
 *    Param         : ID mbxid : mailbox ID
 *                    T_MSG *pk_msg : Message packet address
 *    Return Code   : ER : Error code
 */
extern ER tk_snd_mbx( ID mbxid, T_MSG *pk_msg );

/*
 *    Description   : Refer mailbox object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_mbx( ID list[], int32_t nent );

/*
 *    Description   : Reference mailbox wait queue
 *    Param         : ID mbxid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
extern int32_t td_mbx_que( ID mbxid, ID list[], int32_t nent );

/*
 *    Description   : Reference mailbox object state
 *    Param         : ID mbxid : Object ID
 *                    TD_RMBX *pk_rmbx : Address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_mbx( ID mbxid, TD_RMBX *rmbx );

/*----------------------------------------------------------------------------------------
 * Definition for Mutex interface function 
 */

/*
 *    Description   : Create mutex
 *    Param         : T_CMTX *pk_cmtx : Information about the mutex to be created
 *    Return Code   : ID : Mutex ID or Error code
 */
extern ID tk_cre_mtx( T_CMTX *pk_cmtx );

/*
 *    Description   : Delete mutex
 *    Param         : ID mtxid : Mutex ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_mtx( ID mtxid );

/*
 *    Description   : Lock mutex
 *    Param         : ID mtxid : Mutex ID
 *                    TMO tmout : timeout
 *    Return Code   : ER : Error code
 */
extern ER tk_loc_mtx( ID mtxid, TMO tmout );

/*
 *    Description   : Refer mutex state
 *    Param         : ID mtxid : Mutex ID
 *                    T_RMTX *pk_rmtx : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_mtx( ID mtxid, T_RMTX *pk_rmtx );

/*
 *    Description   : Unlock mutex
 *    Param         : ID mtxid : Mutex ID
 *    Return Code   : ER : Error code
 */
extern ER tk_unl_mtx( ID mtxid );

/*
 *    Description   : Refer mutex object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_mtx( ID list[], int32_t nent );

/*
 *    Description   : Reference mutex wait queue
 *    Param         : ID mtxid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
extern int32_t td_mtx_que( ID mtxid, ID list[], int32_t nent );

/*
 *    Description   : Refer mutex state
 *    Param         : ID mtxid : Object ID
 *                    TD_RMTX *pk_rmtx : Address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_mtx( ID mtxid, TD_RMTX *rmtx );

/*----------------------------------------------------------------------------------------
 * Definition for Message buffer interface function 
 */

/*
 *    Description   : Create message buffer
 *    Param         : T_CMBF *pk_cmbf : Information about the message buffer to be created
 *    Return Code   : ID : message buffer ID or Error code
 */
extern ID tk_cre_mbf( T_CMBF *pk_cmbf );

/*
 *    Description   : Delete message buffer
 *    Param         : ID mbfid : message buffer ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_mbf( ID mbfid );

/*
 *    Description   : Receive from message buffer
 *    Param         : ID mbfid : message buffer ID
 *                    VP msg : Start address of receive message packet
 *                    TMO tmout : timeout
 *    Return Code   : ER : Error code
 */
extern int32_t tk_rcv_mbf( ID mbfid, VP msg, TMO tmout );

/*
 *    Description   : Refer message buffer state
 *    Param         : ID mbfid : message buffer ID
 *                    T_RMBF *pk_rmbf : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_mbf( ID mbfid, T_RMBF *pk_rmbf );

/*
 *    Description   : Send to message buffer
 *    Param         : ID mbfid : message buffer ID
 *                    VP msg : Start address of send message packet
 *                    int32_t msgsz : Send message size (in bytes)
 *                    TMO tmout : timeout
 *    Return Code   : ER : Error code
 */
extern ER tk_snd_mbf( ID mbfid, VP msg, int32_t msgsz, TMO tmout );

/*
 *    Description   : Refer message buffer object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_mbf( ID list[], int32_t nent );

/*
 *    Description   : Reference message buffer object state
 *    Param         : ID mbfid : Object ID
 *                    TD_RMBF *pk_rmbf : Address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_mbf( ID mbfid, TD_RMBF *rmbf );

/*
 *    Description   : Reference message buffer receive wait queue
 *    Param         : ID mbfid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
extern int32_t td_rmbf_que( ID mbfid, ID list[], int32_t nent );

/*
 *    Description   : Reference message buffer send wait queue
 *    Param         : ID mbfid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
extern int32_t td_smbf_que( ID mbfid, ID list[], int32_t nent );

/*----------------------------------------------------------------------------------------
 * Definition for Rendezvous port interface function 
 */

/*
 *    Description   : Accept rendezvous
 *    Param         : ID porid : Rendezvous port ID
 *                    uint32_t acpptn : Accept bitpattern(indicating condition )
 *                    RNO *p_rdvno : Rendezvous number
 *                    VP msg : Message packet address
 *                    TMO tmout : Timeout
 *    Return Code   : int32_t : Call message size (in bytes) or Error code
 */
extern int32_t tk_acp_por( ID porid, uint32_t acpptn, RNO *p_rdvno, VP msg, TMO tmout );

/*
 *    Description   : Call rendezvous
 *    Param         : ID porid : Rendezvous port ID
 *                    uint32_t calptn : Call bit pattern (indicating conditions )
 *                    VP msg : Message packet address
 *                    int32_t cmsgsz : Call message size (in bytes)
 *                    TMO tmout : Timeout
 *    Return Code   : int32_t : Reply message size (in bytes) or Error code
 */
extern int32_t tk_cal_por( ID porid, uint32_t calptn, VP msg, int32_t cmsgsz, TMO tmout );

/*
 *    Description   : Create rendezvous port
 *    Param         : T_CPOR *pk_cpor : Information about rendezvous port to be created
 *    Return Code   : ID : Port ID or Error code
 */
extern ID tk_cre_por( T_CPOR *pk_cpor );

/*
 *    Description   : Delete rendezvous port
 *    Param         : ID porid : Rendezvous port ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_por( ID porid );

/*
 *    Description   : Forward Rendezvous to Other Port
 *    Param         : ID porid : Rendezvous port ID
 *                    uint32_t calptn : Call bit pattern (indicating conditions )
 *                    RNO rdvno : Rendezvous number before forwarding
 *                    VP msg : Message packet address
 *                    int32_t cmsgsz : Call message size (in bytes)
 *    Return Code   : ER : Error code
 */
extern ER tk_fwd_por( ID porid, uint32_t calptn, RNO rdvno, VP msg, int32_t cmsgsz );

/*
 *    Description   : Reference rendezvous port Status
 *    Param         : ID porid : Rendezvous port ID
 *                    T_RPOR *pk_rpor : Start address of packet for status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_por( ID porid, T_RPOR *pk_rpor );

/*
 *    Description   : Reply rendezvous
 *    Param         : RNO rdvno : Rendezvous number
 *                    VP msg : Reply message packet address
 *                    int32_t rmsgsz : Reply message size (in bytes)
 *    Return Code   : ER : Error code
 */
extern ER tk_rpl_rdv( RNO rdvno, VP msg, int32_t rmsgsz );

/*
 *    Description   : Refer rendezvous accept wait queue
 *    Param         : ID porid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : ER : Error code
 */
extern int32_t td_acp_que( ID porid, ID list[], int32_t nent );

/*
 *    Description   : Refer rendezvous call wait queue
 *    Param         : ID porid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : ER : Error code
 */
extern int32_t td_cal_que( ID porid, ID list[], int32_t nent );

/*
 *    Description   : Refer rendezvous port object ID list
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_por( ID list[], int32_t nent );

/*
 *    Description   : Refer rendezvous port
 *    Param         : ID porid : Object ID
 *                    TD_RPOR *rpor : address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_por( ID porid, TD_RPOR *rpor );

/*----------------------------------------------------------------------------------------
 * Definition for Fixed Size Memory Pool interface function 
 */

/*
 *    Description   : Create fixed size memory pool
 *    Param         : T_CMPF *pk_cmpf : Information about the mpf to be created
 *    Return Code   : ID : fixed size memory pool ID or Error code
 */
extern ID tk_cre_mpf( T_CMPF *pk_cmpf );

/*
 *    Description   : Delete fixed size memory pool
 *    Param         : ID mplid : fixed size memory pool ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_mpf( ID mpfid );

/*
 *    Description   : Get fixed size memory block
 *    Param         : ID mpfid :  fixed size memory pool ID
 *                    VP* p_blk : Block start address
 *                    TMO tmout : Timeout
 *    Return Code   : ER : Error code
 */
extern ER tk_get_mpf( ID mpfid, VP *p_blf, TMO tmout );

/*
 *    Description   : Refer fixed size pool state
 *    Param         : ID mpfid : fixed size pool ID
 *                    T_RMPF *pk_rmpf : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_mpf( ID mpfid, T_RMPF *pk_rmpf );

/*
 *    Description   : Return fixed size memory block
 *    Param         : ID mpfid : fixed size memory pool ID
 *                    VP blk : Memory block start address
 *    Return Code   : ER : Error code
 */
extern ER tk_rel_mpf( ID mpfid, VP blf );

/*
 *    Description   : Refer fixed size memory pool usage state
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_mpf( ID list[], int32_t nent );

/*
 *    Description   : Reference fixed size memory pool wait queue
 *    Param         : ID mpfid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
extern int32_t td_mpf_que( ID mpfid, ID list[], int32_t nent );

/*
 *    Description   : Reference fixed size memory pool state
 *    Param         : ID mpfid : Object ID
 *                    TD_RMPF *pk_rmpf : Address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_mpf( ID mpfid, TD_RMPF *rmpf );

/*----------------------------------------------------------------------------------------
 * Definition for Variable Size Memory Pool interface function 
 */

/*
 *    Description   : Create variable size memory pool
 *    Param         : T_CMPL *pk_cmpl : Information about the mpl to be created
 *    Return Code   : ID : Variable-size memory pool ID or Error code
 */
extern ID tk_cre_mpl( T_CMPL *pk_cmpl );

/*
 *    Description   : Delete variable size memory pool
 *    Param         : ID mplid : Variable-size memory pool ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_mpl( ID mplid );

/*
 *    Description   : Get variable size memory block
 *    Param         : ID mplid : Variable-size memory pool ID
 *                    int32_t blksz : Memory block size (in bytes)
 *                    VP* p_blk : Block start address
 *                    TMO tmout : Timeout
 *    Return Code   : ER : Error code
 */
extern ER tk_get_mpl( ID mplid, int32_t blksz, VP *p_blk, TMO tmout );

/*
 *    Description   : Refer variable size memory pool state
 *    Param         : ID mplid : variable size memory pool ID
 *                    T_RMPL *pk_rmpl : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_mpl( ID mplid, T_RMPL *pk_rmpl );

/*
 *    Description   : Return variable size memory block
 *    Param         : ID mplid : Variable-size memory pool ID
 *                    VP blk : Memory block start address
 *    Return Code   : ER : Error code
 */
extern ER tk_rel_mpl( ID mplid, VP blk );

/*
 *    Description   : Refer variable size memory pool object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_mpl( ID list[], int32_t nent );

/*
 *    Description   : Reference variable size memory pool wait queue
 *    Param         : ID mplid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
extern int32_t td_mpl_que( ID mplid, ID list[], int32_t nent );

/*
 *    Description   : Reference variable size memory pool state
 *    Param         : ID mplid : Object ID
 *                    TD_RMPL *pk_rmpl : Address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_mpl( ID mplid, TD_RMPL *rmpl );

/*----------------------------------------------------------------------------------------
 * Definition for Time Management interface function 
 */

/*
 *    Description   : Get Next Tick Event Relative Time
 *    Param         : void
 *    Return Code   : RELTIM : next tick event relative time
 */
extern RELTIM tk_get_etm( void );

/*
 *    Description   : Get Operating Time
 *    Param         : SYSTIM *pk_tim : Address of packet for returning operating time
 *    Return Code   : ER : Error code
 */
extern ER tk_get_otm( SYSTIM *pk_tim );

/*
 *    Description   : Get Time
 *    Param         : SYSTIM *pk_tim : Address of a packet to return current time
 *    Return Code   : ER : Error code
 */
extern ER tk_get_tim( SYSTIM *pk_tim );

/*
 *    Description   : Set Time
 *    Param         : SYSTIM *pk_tim : Address of current time packet
 *    Return Code   : ER : Error code
 */
extern ER tk_set_tim( SYSTIM *pk_tim );

/*
 *    Description   : Get System Operating Time
 *    Param         : SYSTIM *tim : Address of packet for returning operating time (ms)
 *                    uint32_t *ofs : Location for returning elapsed time from tim (ns)
 *    Return Code   : ER : Error code
 */
extern ER td_get_otm( SYSTIM *tim, uint32_t *ofs );

/*
 *    Description   : Get System Time
 *    Param         : SYSTIM *tim : Address of packet for returning current time (ms)
 *                    uint32_t *ofs : Location for returning elapsed time from tim (ns)
 *    Return Code   : ER : Error code
 */
extern ER td_get_tim( SYSTIM *tim, uint32_t *ofs );


/*----------------------------------------------------------------------------------------
 * Definition for Cyclic Handler interface function 
 */

/*
 *    Description   : Create cyclic handler
 *    Param         : T_CCYC *pk_ccyc : Address of cyclic handler definition packet
 *    Return Code   : ID : cyclic handler ID or Error code
 */
extern ID tk_cre_cyc( T_CCYC *pk_ccyc );

/*
 *    Description   : Delete cyclic handler
 *    Param         : ID cycid : cyclic handler ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_cyc( ID cycid );

/*
 *    Description   : Refer cyclic handler state
 *    Param         : ID cycid : cyclic handle ID
 *                    T_RCYC* pk_rcyc : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_cyc( ID cycid, T_RCYC *pk_rcyc );

/*
 *    Description   : Start cyclic handler
 *    Param         : ID cycid : cyclic handler ID
 *    Return Code   : ER : Error code
 */
extern ER tk_sta_cyc( ID cycid );

/*
 *    Description   : Stop cyclic handler
 *    Param         : ID cycid : cyclic handler ID
 *    Return Code   : ER : Error code
 */
extern ER tk_stp_cyc( ID cycid );

/*
 *    Description   : Refer cyclic handler object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_cyc( ID list[], int32_t nent );

/*
 *    Description   : Reference cyclic handler object state
 *    Param         : ID cycid : Object ID
 *                    TD_RCYC* pk_rcyc : Address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_cyc( ID cycid, TD_RCYC *rcyc );

/*----------------------------------------------------------------------------------------
 * Definition for Alarm Handler interface library 
 */

/*
 *    Description   : Create alarm handler
 *    Param         : T_CALM *pk_calm : Address of alarm handler definition packet
 *    Return Code   : ID : alarm handler ID or Error code
 */
extern ID tk_cre_alm( T_CALM *pk_calm );

/*
 *    Description   : Delete alarm handler
 *    Param         : ID almid : alarm handler ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_alm( ID almid );

/*
 *    Description   : Refer alarm handler state
 *    Param         : ID almid : alarm handle ID
 *                    T_RALM *pk_ralm : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_alm( ID almid, T_RALM *pk_ralm );

/*
 *    Description   : Start alarm handler
 *    Param         : ID almid : alarm handler ID
 *                    RELTIM almtim : Alarm handler start time (alarm time)
 *    Return Code   : ER : Error code
 */
extern ER tk_sta_alm( ID almid, RELTIM almtim );

/*
 *    Description   : Stop alarm handler
 *    Param         : ID almid : alarm handler ID
 *    Return Code   : ER : Error code
 */
extern ER tk_stp_alm( ID almid );

/*
 *    Description   : Refer alarm handler object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_alm( ID list[], int32_t nent );

/*
 *    Description   : Reference alarm handler object state
 *    Param         : ID almid : Object ID
 *                    TD_RALM *pk_ralm : Address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_alm( ID almid, TD_RALM *ralm );

/*----------------------------------------------------------------------------------------
 * Definition for Interrupt Management interface library 
 */

/*
 *    Description   : Interrupt handler definition
 *    Param         : uint32_t dintno : Interrupt definition number
 *                    T_DINT *pk_dint : Packet of interrupt handler definition information
 *    Return Code   : ER : Error code
 */
extern ER tk_def_int( uint32_t dintno, T_DINT *pk_dint );

/*
 *    Description   : Return from Interrupt Handler
 *    Param         : none
 *    Return Code   : none
 */
extern void tk_ret_int( void );

/*----------------------------------------------------------------------------------------
 * Definition for System Management interface function 
 */

/*
 *    Description   : Dispatch disable
 *    Param         : void
 *    Return Code   : ER : Error code
 */
extern ER tk_dis_dsp( void );

/*
 *    Description   : Enable Dispatch
 *    Param         : void
 *    Return Code   : ER : Error code
 */
extern ER tk_ena_dsp( void );

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
extern ID tk_get_tid( void );

/*
 *    Description   : Reference System Status
 *    Param         : T_RSYS *pk_rsys : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_sys( T_RSYS *pk_rsys );

/*
 *    Description   : Refer version information,If there is no kernel version information,
 *                    If there is no kernel version information,set 0 in each information.
 *                    (Do NOT cause errors.)
 *    Param         : T_RVER *pk_rver : Start address of packet for version information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_ver( T_RVER *pk_rver );

/*
 *    Description   : Rotate ready queue
 *    Param         : PRI tskpri : Task priority
 *    Return Code   : ER : Error code
 */
extern ER tk_rot_rdq( PRI tskpri );

/*
 *    Description   : Initialize kernel and create/start initial task
 *    Param         : T_CTSK *pk_ctsk : Information about init task
 *    Return Code   : void
 */
extern void tk_sta_sys( T_CTSK *pk_ctsk );

/*
 *    Description   : Refer to DS Object Name
 *    Param         : uint32_t type : object type
 *                    ID id : object ID
 *                    uint8_t *dsname : address to return DS object name
 *    Return Code   : ER : Error code
 */
extern ER td_ref_dsname( uint32_t type, ID id, uint8_t *dsname );

/*
 *    Description   : Reference System Status
 *    Param         : TD_RSYS *pk_rsys : Address of packet for returning status
 *                    information
 *    Return Code   : ER : Error code
 */
extern ER td_ref_sys( TD_RSYS *rsys );

/*
 *    Description   : Set DS Object Name
 *    Param         : uint32_t type : object type
 *                    ID id : object ID
 *                    uint8_t *dsname : DS object name to be set
 *    Return Code   : ER : Error code
 */
extern ER td_set_dsname( uint32_t type, ID id, uint8_t *dsname );

/*
 *    Description   : Set Power-saving mode
 *    Param         : uint32_t powmode : power-saving mode
 *    Return Code   : ER : Error code
 */
extern ER tk_set_pow( uint32_t powmode );

/*----------------------------------------------------------------------------------------
 * Definition for Hook Trace interface function 
 */

/*
 *    Description   : Define Task Dispatch Hook Routine
 *    Param         : TD_HDSP *hdsp : Hook routine definition information
 *    Return Code   : ER : Error code
 */
extern ER td_hok_dsp( TD_HDSP *hdsp );

/*
 *    Description   : Set/Cancel EIT handler hook routine
 *    Param         : TD_HINT *hint : Hook routine definition information
 *    Return Code   : ER : Error code
 */
extern ER td_hok_int( TD_HINT *hint );

/*
 *    Description   : Define System Call/Extended SVC Hook Routine
 *    Param         : TD_HSVC *hsvc : Hook routine definition information
 *    Return Code   : ER : Error code
 */
extern ER td_hok_svc( TD_HSVC *hsvc );


#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* __TK_SYSCALL_H__ */
