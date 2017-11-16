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
 *    File Name      : tk_mbximpl.c
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Mailbox Function
 *----------------------------------------------------------------------------------------
 */

#include "tk_mbximpl.h"

#if KNL_CFG_MAX_MBXID > 0

/*
 * Head message
 */
#define KNL_MBX_HEADMSG(mbxcb)    ( (mbxcb)->mq_head.msgque[0] )

/*
 * Next message
 */
#define KNL_MBX_NEXTMSG(msg)      ( ((T_MSG*)(msg))->msgque[0] )

/*
 * Mailbox control block
 *
 *    'mq_head' is the first message queue pointer that
 *    points a message.
 *    It is NULL if the message queue is empty.
 *    'mq_tail' is a pointer that points end of message
 *    queue that is not empty.
 *    The message queue value is not guaranteed if the
 *    message queue is empty.
 *    It is used only if the message queue is FIFO (TA_MFIFO).
 */
typedef struct mailbox_control_block {
        QUEUE                wait_queue;         /* Mailbox wait queue */
        ID                   mbxid;              /* Mailbox ID */
        VP                   exinf;              /* Extended information */
        ATR                  mbxatr;             /* Mailbox attribute */
        T_MSG                mq_head;            /* Head of message queue */
        T_MSG *              mq_tail;            /* End of message queue */
#if TK_USE_OBJ_NAME                              /* name */
        uint8_t              name[TK_OBJ_NAME_LENGTH];
#endif
}                            MBXCB;

/*
 * Mailbox control block table and free table pointer definition
 */
#ifdef KNL_CFG_USE_FUNC_KNL_MBX_CBTABLE
Zeroinit MBXCB knl_mbx_cbtable[TK_NUM_MBXID];    /* Mailbox control block */
Zeroinit QUEUE knl_mbx_freecb;                   /* FreeQue */
#define KNL_MBX_GET_CB(id)   ( &knl_mbx_cbtable[KNL_CFG_INDEX_MBX(id)] )
#endif /* KNL_CFG_USE_FUNC_KNL_MBX_CBTABLE */

#ifdef KNL_CFG_USE_FUNC_TK_RCV_MBX
/*
 * Definition of mailbox wait specification
 */
static WSPEC knl_mbx_wspec_tfifo = { TTW_MBX, NULL, NULL };
static WSPEC knl_mbx_wspec_tpri  = { TTW_MBX, knl_mbx_chgpri, NULL };
#endif /* KNL_CFG_USE_FUNC_TK_RCV_MBX */

/*
 *    Description   : Insert a message queue following priority
 *    Param         :  T_MSG_PRI *pk_msg : pointer of a message queue
 *                    T_MSG *head : message to be inserted
 *    Return Code   : void
 */
Inline void knl_que_insert_mpri( T_MSG_PRI *pk_msg, T_MSG *head )
{
    T_MSG_PRI *msg;
    T_MSG *prevmsg = head;

    while ( (msg = (T_MSG_PRI*)KNL_MBX_NEXTMSG(prevmsg)) != NULL ) {
        if ( msg->msgpri > pk_msg->msgpri ) {
            break;
        }
        prevmsg = (T_MSG*)msg;
    }
    KNL_MBX_NEXTMSG(pk_msg) = msg;
    KNL_MBX_NEXTMSG(prevmsg) = pk_msg;
}

#ifdef KNL_CFG_USE_FUNC_TK_RCV_MBX
/*
 *    Function Name : knl_mbx_chgpri
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Processing if the priority of wait task changes
 *    Param         : TCB *tcb : task control block to change the priority
 *                    int32_t oldpri : old priority, maybe is -1
 *    Return Code   : void
 */
void knl_mbx_chgpri( TCB *tcb, int32_t oldpri )
{
    MBXCB  *mbxcb;

    mbxcb = KNL_MBX_GET_CB(tcb->wid);
    knl_tsk_gcbchgpri((GCB*)mbxcb, tcb);
}
#endif /* KNL_CFG_USE_FUNC_TK_RCV_MBX */

#ifdef KNL_CFG_USE_FUNC_KNL_MBX_GETNAME
#if TK_USE_OBJ_NAME
/*
 *    Function Name : knl_mbx_getname
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get object name from control block
 *    Param         : ID id : mailbox id
 *                    uint8_t **name : mailbox DS name pointer
 *    Return Code   : ER : Error code
 */
ER knl_mbx_getname(ID id, uint8_t **name)
{
    MBXCB  *mbxcb;
    ER  ercd = E_OK;

    KNL_CHK_MBXID(id)

    KNL_INT_BEGIN_DISABLE
    mbxcb = KNL_MBX_GET_CB(id);
    if ( mbxcb->mbxid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
    if ( (mbxcb->mbxatr & TA_DSNAME) == 0u ) {
        ercd = E_OBJ;
        goto error_exit;
    }
    *name = mbxcb->name;

error_exit:
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* TK_USE_OBJ_NAME */
#endif /* KNL_CFG_USE_FUNC_KNL_MBX_GETNAME */

#ifdef KNL_CFG_USE_FUNC_KNL_MBX_INIT
/*
 *    Function Name : knl_mbx_init
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Initialization of mailbox control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
ER knl_mbx_init( void )
{
    MBXCB *mbxcb, *mbxcbend;

    /* Get system information */
    if ( TK_NUM_MBXID < 1 ) {
        return E_SYS;
    }

    /* Register all control blocks onto FreeQue */
    knl_que_init(&knl_mbx_freecb);
    mbxcbend = knl_mbx_cbtable + TK_NUM_MBXID;
    for ( mbxcb = knl_mbx_cbtable; mbxcb < mbxcbend; mbxcb++ ) {
        mbxcb->mbxid = 0;
        knl_que_insert(&mbxcb->wait_queue, &knl_mbx_freecb);
    }

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_KNL_MBX_INIT */

#ifdef KNL_CFG_USE_FUNC_TK_CRE_MBX
/*
 *    Function Name : tk_cre_mbx_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Create mailbox
 *    Param         : T_CMBX *pk_cmbx : Information about the mailbox to be created
 *    Return Code   : ID : mailbox ID or Error code
 */
SYSCALL ID tk_cre_mbx_impl( T_CMBX *pk_cmbx )
{
#if KNL_CFG_CHK_RSATR
    const ATR VALID_MBXATR = {
         TA_MPRI
        |TA_TPRI
#if TK_USE_OBJ_NAME
        |TA_DSNAME
#endif
    };
#endif
    MBXCB *mbxcb;
    ID mbxid;
    ER ercd;

    KNL_CHK_RSATR(pk_cmbx->mbxatr, VALID_MBXATR)

    KNL_TSK_BEGIN_CRITICAL_SECTION
    /* Get control block from FreeQue */
    mbxcb = (MBXCB*)knl_que_removenext(&knl_mbx_freecb);
    if ( mbxcb == NULL ) {
        ercd = E_LIMIT;
    } else {
        mbxid = KNL_CFG_ID_MBX(mbxcb - knl_mbx_cbtable);

        /* Initialize control block */
        knl_que_init(&mbxcb->wait_queue);
        mbxcb->mbxid  = mbxid;
        mbxcb->exinf  = pk_cmbx->exinf;
        mbxcb->mbxatr = pk_cmbx->mbxatr;
        mbxcb->mq_head.msgque[0] = NULL;
#if TK_USE_OBJ_NAME
        if ( (pk_cmbx->mbxatr & TA_DSNAME) != 0u ) {
            strncpy((char*)&mbxcb->name, (char*)&pk_cmbx->dsname, TK_OBJ_NAME_LENGTH);
        }
#endif
        ercd = mbxid;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_CRE_MBX */

#ifdef KNL_CFG_USE_FUNC_TK_DEL_MBX
/*
 *    Function Name : tk_del_mbx_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Delete mailbox
 *    Param         : ID semid : mailbox ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_del_mbx_impl( ID mbxid )
{
    MBXCB  *mbxcb;
    ER  ercd = E_OK;

    KNL_CHK_MBXID(mbxid)

    mbxcb = KNL_MBX_GET_CB(mbxid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( mbxcb->mbxid == 0 ) {
        ercd = E_NOEXS;
    } else {
        /* Release wait state of task (E_DLT) */
        knl_tsk_waidel(&mbxcb->wait_queue);

        /* Return to FreeQue */
        knl_que_insert(&mbxcb->wait_queue, &knl_mbx_freecb);
        mbxcb->mbxid = 0;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_DEL_MBX */

#ifdef KNL_CFG_USE_FUNC_TK_RCV_MBX
/*
 *    Function Name : tk_rcv_mbx_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Receive from mailbox
 *    Param         : ID mbxid : mailbox ID
 *                    T_MSG **ppk_msg : Start address of message packet
 *                    TMO tmout : timeout
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_rcv_mbx_impl( ID mbxid, T_MSG **ppk_msg, TMO tmout )
{
    MBXCB  *mbxcb;
    ER  ercd = E_OK;

    KNL_CHK_MBXID(mbxid)
    KNL_CHK_TMOUT(tmout)
    KNL_CHK_DISPATCH()

    mbxcb = KNL_MBX_GET_CB(mbxid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( mbxcb->mbxid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }

    if ( KNL_MBX_HEADMSG(mbxcb) != NULL ) {
        /* Get message from head of queue */
        *ppk_msg = KNL_MBX_HEADMSG(mbxcb);
        KNL_MBX_HEADMSG(mbxcb) = KNL_MBX_NEXTMSG(*ppk_msg);
    } else {
        /* Ready for receive wait */
        knl_tsk_execution->wspec = ( (mbxcb->mbxatr & TA_TPRI) != 0u )?
                    &knl_mbx_wspec_tpri: &knl_mbx_wspec_tfifo;
        knl_tsk_execution->wercd = &ercd;
        knl_tsk_execution->winfo.mbx.ppk_msg = ppk_msg;
        knl_tsk_gcbmakwai((GCB*)mbxcb, tmout);
    }

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_RCV_MBX */

#ifdef KNL_CFG_USE_FUNC_TK_REF_MBX
/*
 *    Function Name : tk_ref_mbx_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer mailbox state
 *    Param         : ID mbxid : mailbox ID
 *                    T_RMBX *pk_rmbx : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_ref_mbx_impl( ID mbxid, T_RMBX *pk_rmbx )
{
    MBXCB  *mbxcb;
    ER  ercd = E_OK;

    KNL_CHK_MBXID(mbxid)

    mbxcb = KNL_MBX_GET_CB(mbxid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( mbxcb->mbxid == 0 ) {
        ercd = E_NOEXS;
    } else {
        pk_rmbx->exinf = mbxcb->exinf;
        pk_rmbx->wtsk = knl_tsk_waitid(&mbxcb->wait_queue);
        pk_rmbx->pk_msg = KNL_MBX_HEADMSG(mbxcb);
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_REF_MBX */

#ifdef KNL_CFG_USE_FUNC_TK_SND_MBX
/*
 *    Function Name : tk_snd_mbx_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Send to mailbox
 *    Param         : ID mbxid : mailbox ID
 *                    T_MSG *pk_msg : Message packet address
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_snd_mbx_impl( ID mbxid, T_MSG *pk_msg )
{
    MBXCB  *mbxcb;
    TCB  *tcb;
    ER  ercd = E_OK;

    KNL_CHK_MBXID(mbxid)

    mbxcb = KNL_MBX_GET_CB(mbxid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if (mbxcb->mbxid == 0) {
        ercd = E_NOEXS;
        goto error_exit;
    }

    if ( (mbxcb->mbxatr & TA_MPRI) != 0u ) {
        if ( ((T_MSG_PRI*)pk_msg)->msgpri <= 0u ) {
            ercd = E_PAR;
            goto error_exit;
        }
    }

    if ( !knl_que_isempty(&mbxcb->wait_queue) ) {
        /* Directly send to receive wait task */
        tcb = (TCB*)(mbxcb->wait_queue.next);
        *tcb->winfo.mbx.ppk_msg = pk_msg;
        knl_tsk_wairelok(tcb);

    } else {
        /* Connect message to queue */
        if ( (mbxcb->mbxatr & TA_MPRI) != 0u ) {
            /* Connect message to queue following priority */
            knl_que_insert_mpri((T_MSG_PRI*)pk_msg, &mbxcb->mq_head);
        } else {
            /* Connect to end of queue */
            KNL_MBX_NEXTMSG(pk_msg) = NULL;
            if ( KNL_MBX_HEADMSG(mbxcb) == NULL ) {
                KNL_MBX_HEADMSG(mbxcb) = pk_msg;
            } else {
                KNL_MBX_NEXTMSG(mbxcb->mq_tail) = pk_msg;
            }
            mbxcb->mq_tail = pk_msg;
        }
    }

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_SND_MBX */

/*
 *    Debugger support function
 */
#if TK_USE_DBGSPT

#ifdef KNL_CFG_USE_FUNC_TD_LST_MBX
/*
 *    Function Name : td_lst_mbx_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer mailbox object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
SYSCALL int32_t td_lst_mbx_impl( ID list[], int32_t nent )
{
    MBXCB  *mbxcb, *mbxcbend;
    int32_t  n = 0;

    KNL_INT_BEGIN_DISABLE
    mbxcbend = knl_mbx_cbtable + TK_NUM_MBXID;
    for ( mbxcb = knl_mbx_cbtable; mbxcb < mbxcbend; mbxcb++ ) {
        if ( mbxcb->mbxid != 0 ) {
            if ( n < nent ) {
                *list = mbxcb->mbxid;
                list++;
            }
            n++;
        }
    }
    KNL_INT_END_DISABLE

    return n;
}
#endif /* KNL_CFG_USE_FUNC_TD_LST_MBX */

#ifdef KNL_CFG_USE_FUNC_TD_MBX_QUE
/*
 *    Function Name : td_mbx_que_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference mailbox wait queue
 *    Param         : ID mbxid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
SYSCALL int32_t td_mbx_que_impl( ID mbxid, ID list[], int32_t nent )
{
    int32_t n = 0;
    MBXCB  *mbxcb;
    QUEUE  *q;
    ER  ercd = E_OK;

    KNL_CHK_MBXID(mbxid)

    mbxcb = KNL_MBX_GET_CB(mbxid);

    KNL_INT_BEGIN_DISABLE
    if ( mbxcb->mbxid == 0 ) {
        ercd = E_NOEXS;
    } else {
        for ( q = mbxcb->wait_queue.next; q != &mbxcb->wait_queue; q = q->next ) {
            if ( n < nent ) {
                *list = ((TCB*)q)->tskid;
                list++;
            }
            n++;
        }
        ercd = n;
    }
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TD_MBX_QUE */

#ifdef KNL_CFG_USE_FUNC_TD_REF_MBX
/*
 *    Function Name : td_ref_mbx_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference mailbox object state
 *    Param         : ID mbxid : Object ID
 *                    TD_RMBX *pk_rmbx : Address of status information packet
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_ref_mbx_impl( ID mbxid, TD_RMBX *pk_rmbx )
{
    MBXCB  *mbxcb;
    ER  ercd = E_OK;

    KNL_CHK_MBXID(mbxid)

    mbxcb = KNL_MBX_GET_CB(mbxid);

    KNL_INT_BEGIN_DISABLE
    if ( mbxcb->mbxid == 0 ) {
        ercd = E_NOEXS;
    } else {
        pk_rmbx->exinf = mbxcb->exinf;
        pk_rmbx->wtsk = knl_tsk_waitid(&mbxcb->wait_queue);
        pk_rmbx->pk_msg = KNL_MBX_HEADMSG(mbxcb);
    }
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TD_REF_MBX */

#endif /* TK_USE_DBGSPT */

#endif /* KNL_CFG_MAX_MBXID */
