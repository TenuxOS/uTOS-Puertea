/*
 *----------------------------------------------------------------------------------------
 *    Kernel/sem
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
 *    File Name      : tk_semimpl.h
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Semaphore Header definition .
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_SEMIMPL_H__
#define __TK_SEMIMPL_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#if KNL_CFG_MAX_SEMID > 0

/*
 *    Description   : Processing if the priority of wait task changes
 *    Param         : TCB *tcb : task control block to change the priority
 *                    int32_t oldpri : old priority, maybe is -1
 *    Return Code   : void
 */
extern void knl_sem_chgpri( TCB *tcb, int32_t oldpri );

/*
 *    Description   : Get object name from control block
 *    Param         : ID id : semaphore id
 *                    uint8_t **name : semaphore DS name pointer
 *    Return Code   : ER : Error code
 */
extern ER knl_sem_getname(ID id, uint8_t **name);

/*
 *    Description   : Initialization of semaphore control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
extern ER knl_sem_init( void );

/*
 *    Description   : Processing if the wait task is freed
 *    Param         : TCB *tcb : task to release waiting
 *    Return Code   : void
 */
extern void knl_sem_relwai( TCB *tcb );

/*
 *    Description   : Create semaphore
 *    Param         : T_CSEM *pk_csem : Information about the semaphore to be created
 *    Return Code   : ID : Semaphore ID or Error code
 */
extern ID tk_cre_sem_impl( T_CSEM *pk_csem );

/*
 *    Description   : Delete semaphore
 *    Param         : ID semid : Semaphore ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_sem_impl( ID semid );

/*
 *    Description   : Refer semaphore state
 *    Param         : ID semid : Semaphore ID
 *                    T_RSEM *pk_rsem : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_sem_impl( ID semid, T_RSEM *pk_rsem );

/*
 *    Description   : Signal semaphore
 *    Param         : ID semid : Semaphore ID
 *                    int32_t cnt : Resource return count
 *    Return Code   : ER : Error code
 */
extern ER tk_sig_sem_impl( ID semid, int32_t cnt );

/*
 *    Description   : Wait on semaphore
 *    Param         : ID semid : Semaphore ID
 *                    int32_t cnt : Resource request count
 *                    TMO tmou : timeout
 *    Return Code   : ER : Error code
 */
extern ER tk_wai_sem_impl( ID semid, int32_t cnt, TMO tmout );

#if TK_USE_DBGSPT

/*
 *    Description   : Refer object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_sem_impl( ID list[], int32_t nent );

/*
 *    Description   : Reference object state
 *    Param         : ID semid : Object ID
 *                    TD_RSEM *pk_rsem : Address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_sem_impl( ID semid, TD_RSEM *pk_rsem );

/*
 *    Description   : Reference wait queue
 *    Param         : ID semid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : ER : Error code
 */
extern int32_t td_sem_que_impl( ID semid, ID list[], int32_t nent );

#endif /* TK_USE_DBGSPT */
#endif /* KNL_CFG_MAX_SEMID */

#ifdef __cplusplus
}
#endif

#endif /* __TK_SEMIMPL_H__ */
