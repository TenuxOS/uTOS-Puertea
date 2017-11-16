/*
 *----------------------------------------------------------------------------------------
 *    Kernel/por
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
 *    File Name      : tk_porimpl.h
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Rendezvous Header definition .
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_PORIMPL_H__
#define __TK_PORIMPL_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#if KNL_CFG_MAX_PORID > 0

/*
 *    Description   : Processing if the priority of send wait task changes
 *    Param         : TCB *tcb : task control block to change the priority
 *                    int32_t oldpri : old priority, maybe is -1
 *    Return Code   : void
 */
void knl_por_chgpri( TCB *tcb, int32_t oldpri );

/*
 *    Description   : Get rendezvous port object name from control block
 *    Param         : ID id : rendezvous port ID
 *                    uint8_t **name : address of rendezvous port DS name
 *    Return Code   : ER : Error code
 */
extern ER knl_por_getname(ID id, uint8_t **name);

/*
 *    Description   : Initialization of port control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
extern ER knl_por_init( void );

/*
 *    Description   : Accept rendezvous
 *    Param         : ID porid : Rendezvous port ID
 *                    uint32_t acpptn : Accept bitpattern(indicating condition )
 *                    RNO *p_rdvno : Rendezvous number
 *                    VP msg : Message packet address
 *                    TMO tmout : Timeout
 *    Return Code   : int32_t : Call message size (in bytes) or Error code
 */
extern int32_t tk_acp_por_impl( ID porid, uint32_t acpptn, RNO *p_rdvno, VP msg, TMO tmout );

/*
 *    Description   : Call rendezvous
 *    Param         : ID porid : Rendezvous port ID
 *                    uint32_t calptn : Call bit pattern (indicating conditions )
 *                    VP msg : Message packet address
 *                    int32_t cmsgsz : Call message size (in bytes)
 *                    TMO tmout : Timeout
 *    Return Code   : int32_t : Reply message size (in bytes) or Error code
 */
extern int32_t tk_cal_por_impl( ID porid, uint32_t calptn, VP msg, int32_t cmsgsz, TMO tmout );

/*
 *    Description   : Create rendezvous port
 *    Param         : T_CPOR *pk_cpor : Information about rendezvous port to be created
 *    Return Code   : ID : Port ID or Error code
 */
extern ID tk_cre_por_impl( T_CPOR *pk_cpor );

/*
 *    Description   : Delete rendezvous port
 *    Param         : ID porid : Rendezvous port ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_por_impl( ID porid );

/*
 *    Description   : Forward Rendezvous to Other Port
 *    Param         : ID porid : Rendezvous port ID
 *                    uint32_t calptn : Call bit pattern (indicating conditions )
 *                    RNO rdvno : Rendezvous number before forwarding
 *                    VP msg : Message packet address
 *                    int32_t cmsgsz : Call message size (in bytes)
 *    Return Code   : ER : Error code
 */
extern ER tk_fwd_por_impl( ID porid, uint32_t calptn, RNO rdvno, VP msg, int32_t cmsgsz );

/*
 *    Description   : Reference rendezvous port Status
 *    Param         : ID porid : Rendezvous port ID
 *                    T_RPOR *pk_rpor : Start address of packet for status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_por_impl( ID porid, T_RPOR *pk_rpor );

/*
 *    Description   : Reply rendezvous
 *    Param         : RNO rdvno : Rendezvous number
 *                    VP msg : Reply message packet address
 *                    int32_t rmsgsz : Reply message size (in bytes)
 *    Return Code   : ER : Error code
 */
extern ER tk_rpl_rdv_impl( RNO rdvno, VP msg, int32_t rmsgsz );

#if TK_USE_DBGSPT

/*
 *    Description   : Refer rendezvous accept wait queue
 *    Param         : ID porid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : ER : Error code
 */
extern int32_t td_acp_que_impl( ID porid, ID list[], int32_t nent );

/*
 *    Description   : Refer rendezvous call wait queue
 *    Param         : ID porid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : ER : Error code
 */
extern int32_t td_cal_que_impl( ID porid, ID list[], int32_t nent );

/*
 *    Description   : Refer rendezvous port object ID list
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_por_impl( ID list[], int32_t nent );

/*
 *    Description   : Refer rendezvous port
 *    Param         : ID porid : Object ID
 *                    TD_RPOR *rpor : address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_por_impl( ID porid, TD_RPOR *pk_rpor );

#endif /* TK_USE_DBGSPT */
#endif /* KNL_CFG_MAX_PORID */

#ifdef __cplusplus
}
#endif

#endif /* __TK_PORIMPL_H__ */
