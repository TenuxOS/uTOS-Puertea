/*
 *----------------------------------------------------------------------------------------
 *    Kernel/mbf
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
 *    File Name      : tk_mbfimpl.h
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Message Buffer Header definition .
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_MBFIMPL_H__
#define __TK_MBFIMPL_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#if KNL_CFG_MAX_MBFID > 0

/*
 * Message buffer control block
 *
 *  Because Receive wait task (TTW_MBF) and Send wait task (TTW_SMBF)
 *  do not co-exist for one message buffer, the wait queue may be
 *  allowed to share.
 *  However, when the size of message buffer is 0, it is difficult
 *  to judge the wait queue if it is for receive or send,
 *  therefore do not use this method.
 */
typedef struct messagebuffer_control_block {
        QUEUE                send_queue;         /* Message buffer send wait queue */
        ID                   mbfid;              /* message buffer ID */
        VP                   exinf;              /* Extended information */
        ATR                  mbfatr;             /* Message buffer attribute */
        QUEUE                recv_queue;         /* Message buffer receive wait queue */
        int32_t              bufsz;              /* Message buffer size */
        int32_t              maxmsz;             /* Maximum length of message */
        int32_t              frbufsz;            /* Free buffer size */
        int32_t              head;               /* First message store address */
        int32_t              tail;               /* Next to the last message store addr */
        uint8_t *            buffer;             /* Message buffer address */
#if TK_USE_OBJ_NAME                              /* name */
        uint8_t              name[TK_OBJ_NAME_LENGTH];  
#endif
}                            MBFCB;

/*
 *    Description   : Processing if the priority of wait task changes
 *    Param         : TCB *tcb : task control block to change the priority
 *                    int32_t oldpri : old priority, maybe is -1
 *    Return Code   : void
 */
extern void knl_mbf_chgpri( TCB *tcb, int32_t oldpri );

/*
 *    Description   : Get object name from control block
 *    Param         : ID id : message buffer id
 *                    uint8_t **name : message buffer DS name pointer
 *    Return Code   : ER : Error code
 */
extern ER knl_mbf_getname(ID id, uint8_t **name);

/*
 *    Description   : Initialization of message buffer control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
extern ER knl_mbf_init( void );

/*
 *    Description   : Processing if the wait task is released
 *    Param         : TCB *tcb : task control block to release
 *    Return Code   : void
 */
extern void knl_mbf_relwai( TCB *tcb );
/*
 *    Description   : Accept message and release wait task,
 *                    as long as there are free message area.
 *    Param         : MBFCB *mbfcb : address of message buffer control block
 *    Return Code   : void
 */
extern void knl_mbf_wakeup( MBFCB *mbfcb );

/*
 *    Description   : Create message buffer
 *    Param         : T_CMBF *pk_cmbf : Information about the message buffer to be created
 *    Return Code   : ID : message buffer ID or Error code
 */
extern ID tk_cre_mbf_impl( T_CMBF *pk_cmbf );

/*
 *    Description   : Delete message buffer
 *    Param         : ID mbfid : message buffer ID
 *    Return Code   : ER : Error code
 */
extern ER tk_del_mbf_impl( ID mbfid );

/*
 *    Description   : Receive from message buffer
 *    Param         : ID mbfid : message buffer ID
 *                    VP msg : Start address of receive message packet
 *                    TMO tmout : timeout
 *    Return Code   : ER : Error code
 */
extern int32_t tk_rcv_mbf_impl( ID mbfid, VP msg, TMO tmout );

/*
 *    Description   : Refer message buffer state
 *    Param         : ID mbfid : message buffer ID
 *                    T_RMBF *pk_rmbf : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
extern ER tk_ref_mbf_impl( ID mbfid, T_RMBF *pk_rmbf );

/*
 *    Description   : Send to message buffer
 *    Param         : ID mbfid : message buffer ID
 *                    VP msg : Start address of send message packet
 *                    int32_t msgsz : Send message size (in bytes)
 *                    TMO tmout : timeout
 *    Return Code   : ER : Error code
 */
extern ER tk_snd_mbf_impl( ID mbfid, VP msg, int32_t msgsz, TMO tmout );

#if TK_USE_DBGSPT

/*
 *    Description   : Refer message buffer object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
extern int32_t td_lst_mbf_impl( ID list[], int32_t nent );

/*
 *    Description   : Reference message buffer object state
 *    Param         : ID mbfid : Object ID
 *                    TD_RMBF *pk_rmbf : Address of status information packet
 *    Return Code   : ER : Error code
 */
extern ER td_ref_mbf_impl( ID mbfid, TD_RMBF *pk_rmbf );

/*
 *    Description   : Reference message buffer receive wait queue
 *    Param         : ID mbfid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
extern int32_t td_rmbf_que_impl( ID mbfid, ID list[], int32_t nent );

/*
 *    Description   : Reference message buffer send wait queue
 *    Param         : ID mbfid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
extern int32_t td_smbf_que_impl( ID mbfid, ID list[], int32_t nent );

#endif /* TK_USE_DBGSPT */
#endif /* KNL_CFG_MAX_MBFID */

#ifdef __cplusplus
}
#endif

#endif /* __TK_MBFIMPL_H__ */
