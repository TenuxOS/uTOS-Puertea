/*
 *----------------------------------------------------------------------------------------
 *    Kernel/mpf
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
 *    File Name      : tk_mpfimpl.h
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Fixed Size Memory Pool Header definition .
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_MPFIMPL_H__
#define __TK_MPFIMPL_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#if KNL_CFG_MAX_MPFID > 0

/*
 *    Description   : Processing if the priority of wait task changes
 *    Param         : TCB *tcb : task control block to change the priority
 *                    int32_t oldpri : old priority, maybe is -1
 *    Return Code   : void
 */
extern void knl_mpf_chgpri( TCB *tcb, int32_t oldpri );

/*
 *    Description   : Get object name from control block
 *    Param         : ID id : fixed size pool id
 *                    uint8_t **name : fixed size pool DS name pointer
 *    Return Code   : ER : Error code
 */
extern ER knl_mpf_getname(ID id, uint8_t **name);

/*
 *    Description   : Initialization of fixed size memory pool control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
extern ER knl_mpf_init( void );

/*
 *    Description   : Create fixed size memory pool
 *    Param         : T_CMPF *pk_cmpf : Information about the mpf to be created
 *    Return Code   : ID : fixed size memory pool ID or Error code
 */
extern ID tk_cre_mpf_impl( T_CMPF *pk_cmpf );

/*
 *    Description   : Delete fixed size memory pool
 *    Param         : ID mplid : fixed size memory pool ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_mpf_impl( ID mpfid );

/*
 *    Description   : Get fixed size memory block
 *    Param         : ID mpfid :  fixed size memory pool ID
 *                    VP* p_blk : Block start address
 *                    TMO tmout : Timeout
 *    Return Code   : ER : Error code
 */
extern ER tk_get_mpf_impl( ID mpfid, VP *p_blf, TMO tmout );

/*
 *    Description   : Refer fixed size pool state
 *    Param         : ID mpfid : fixed size pool ID
 *                    T_RMPF *pk_rmpf : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_mpf_impl( ID mpfid, T_RMPF *pk_rmpf );

/*
 *    Description   : Return fixed size memory block
 *    Param         : ID mpfid : fixed size memory pool ID
 *                    VP blk : Memory block start address
 *    Return Code   : ER : Error code
 */
extern ER tk_rel_mpf_impl( ID mpfid, VP blf );

#if TK_USE_DBGSPT

/*
 *    Description   : Refer fixed size memory pool usage state
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_mpf_impl( ID list[], int32_t nent );

/*
 *    Description   : Reference fixed size memory pool wait queue
 *    Param         : ID mpfid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
extern int32_t td_mpf_que_impl( ID mpfid, ID list[], int32_t nent );

/*
 *    Description   : Reference fixed size memory pool state
 *    Param         : ID mpfid : Object ID
 *                    TD_RMPF *pk_rmpf : Address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_mpf_impl( ID mpfid, TD_RMPF *pk_rmpf );

#endif /* TK_USE_DBGSPT */
#endif /* KNL_CFG_MAX_MPFID */

#ifdef __cplusplus
}
#endif

#endif /* __TK_MPFIMPL_H__ */
