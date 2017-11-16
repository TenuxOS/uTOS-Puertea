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
 *    File Name      : tk_mbfimpl.c
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Message Buffer Function.
 *----------------------------------------------------------------------------------------
 */

#include "tk_mbfimpl.h"

#if KNL_CFG_MAX_MBFID > 0

/*
 * Message header format
 */
typedef int32_t              HEADER;
#define KNL_MBF_HEADERSIZE   (sizeof(HEADER))
#define KNL_MBF_ROUNDUNIT    (sizeof(HEADER))

/*
 * Message buffer control block table and free table pointer definition
 */
#ifdef KNL_CFG_USE_FUNC_KNL_MBF_CBTABLE
Zeroinit MBFCB knl_mbf_cbtable[TK_NUM_MBFID];    /* Message buffer control block */
Zeroinit QUEUE knl_mbf_freecb;                   /* FreeQue */
#define KNL_MBF_GET_CB(id)   ( &knl_mbf_cbtable[KNL_CFG_INDEX_MBF(id)] )
#endif /* KNL_CFG_USE_FUNC_KNL_MBF_CBTABLE */

#ifdef KNL_CFG_USE_FUNC_TK_RCV_MBF
static WSPEC knl_mbf_rcv_wspec         = { TTW_RMBF, NULL, NULL };
#endif /* KNL_CFG_USE_FUNC_TK_RCV_MBF */

#ifdef KNL_CFG_USE_FUNC_TK_SND_MBF
/*
 * Definition of message buffer wait specification
 */
static WSPEC knl_mbf_snd_wspec_tfifo   = { TTW_SMBF, NULL, knl_mbf_relwai };
static WSPEC knl_mbf_snd_wspec_tpri    = { TTW_SMBF, knl_mbf_chgpri, knl_mbf_relwai };
#endif /* KNL_CFG_USE_FUNC_TK_SND_MBF */

/*
 * Declaration of local function prototype  
 */
static int32_t knl_mbf_getmsg( MBFCB *mbfcb, VP msg );
static void knl_mbf_sndmsg( MBFCB *mbfcb, VP msg, int32_t msgsz );

/*
 *    Description   : Check whether message buffer is empty
 *    Param         : MBFCB *mbfcb : message buffer control block
 *    Return Code   : bool8_t : If message buffer is empty, return true.
 */
Inline  bool8_t knl_mbf_empty( MBFCB *mbfcb )
{
    int8_t  b;

    b = ( mbfcb->frbufsz == mbfcb->bufsz)? true: false;
    return (bool8_t)b;
}

/*
 *    Description   : Check message buffer free space
 *    Param         : MBFCB *mbfcb : message buffer control block
 *                    int32_t msgsz : message size
 *    Return Code   : bool8_t : If 'msgsz' message is able to be stored, return true.
 */
Inline bool8_t knl_mbf_isfree( MBFCB *mbfcb, int32_t msgsz )
{
    int8_t  b;

    b = ( ( (int32_t)KNL_MBF_HEADERSIZE + msgsz ) <= mbfcb->frbufsz )? true: false;
    return (bool8_t)b;
}

/*
 *    Description   : Round message size by roundunit
 *    Param         : int32_t msgsz : message size
 *    Return Code   : int32_t : rounded message size.
 */
Inline  int32_t knl_mbf_round( int32_t msgsz )
{
    uint32_t sz;
    
    sz = (uint32_t)msgsz;
    sz = ( sz + (KNL_MBF_ROUNDUNIT-1u) ) & ( ~(KNL_MBF_ROUNDUNIT-1u) );

    return (int32_t)sz;
}

#ifdef KNL_CFG_USE_FUNC_TK_RCV_MBF
/*
 *    Function Name : knl_mbf_getmsg
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get a message from message buffer.Return the message size.
 *    Param         : MBFCB *mbfcb : address of message buffer control block
 *                    VP msg : Start address of message packet
 *    Return Code   : int32_t : msg size
 */
static int32_t knl_mbf_getmsg( MBFCB *mbfcb, VP msg )
{
    int32_t msgsz, actsz;
    int32_t remsz;
    int32_t head = mbfcb->head;
    uint8_t *buffer = mbfcb->buffer;

    actsz = msgsz = *(HEADER*)&buffer[head];
    mbfcb->frbufsz += (int32_t)KNL_MBF_HEADERSIZE + knl_mbf_round(msgsz);

    head += (int32_t)KNL_MBF_HEADERSIZE;
    if ( head >= mbfcb->bufsz ) {
        head = 0;
    }

    if ( (remsz = mbfcb->bufsz - head) < (int32_t)msgsz ) {
        memcpy(msg, &buffer[head], (size_t)remsz);
        msg = (uint8_t*)msg + remsz;
        msgsz -= (int32_t)remsz;
        head = 0;
    }
    memcpy(msg, &buffer[head], (size_t)msgsz);
    head += knl_mbf_round(msgsz);
    if ( head >= mbfcb->bufsz ) {
        head = 0;
    }

    mbfcb->head = head;

    return actsz;
}
#endif /* KNL_CFG_USE_FUNC_TK_RCV_MBF */

#ifdef KNL_CFG_USE_FUNC_KNL_MBF_SNDMSG
/*
 *    Function Name : knl_mbf_sndmsg
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Store the message to message buffer.
 *    Param         : MBFCB *mbfcb : address of message buffer control block
 *                    VP msg : Start address of message packet
 *                    int32_t msgsz : message size
 *    Return Code   : void
 */
static void knl_mbf_sndmsg( MBFCB *mbfcb, VP msg, int32_t msgsz )
{
    int32_t tail = mbfcb->tail;
    uint8_t *buffer = mbfcb->buffer;
    int32_t remsz;

    mbfcb->frbufsz -= (int32_t)KNL_MBF_HEADERSIZE + knl_mbf_round(msgsz);

    *(HEADER*)&buffer[tail] = msgsz;
    tail += (int32_t)KNL_MBF_HEADERSIZE;
    if ( tail >= mbfcb->bufsz ) {
        tail = 0;
    }

    if ( (remsz = mbfcb->bufsz - tail) < (int32_t)msgsz ) {
        memcpy(&buffer[tail], msg, (size_t)remsz);
        msg = (uint8_t*)msg + remsz;
        msgsz -= (int32_t)remsz;
        tail = 0;
    }
    memcpy(&buffer[tail], msg, (size_t)msgsz);
    tail += knl_mbf_round(msgsz);
    if ( tail >= mbfcb->bufsz ) {
        tail = 0;
    }

    mbfcb->tail = tail;
}
#endif /* KNL_CFG_USE_FUNC_KNL_MBF_SNDMSG */

#ifdef KNL_CFG_USE_FUNC_TK_SND_MBF
/*
 *    Function Name : knl_mbf_chgpri
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Processing if the priority of wait task changes
 *    Param         : TCB *tcb : task control block to change the priority
 *                    int32_t oldpri : old priority, maybe is -1
 *    Return Code   : void
 */
void knl_mbf_chgpri( TCB *tcb, int32_t oldpri )
{
    MBFCB  *mbfcb;

    mbfcb = KNL_MBF_GET_CB(tcb->wid);
    if ( oldpri >= 0 ) {
        /* Reorder wait queue */
        knl_tsk_gcbchgpri((GCB*)mbfcb, tcb);
    }

    /* If the new head task in a send wait queue is able to sent, 
       send its message */
    knl_mbf_wakeup(mbfcb);
}
#endif /* KNL_CFG_USE_FUNC_TK_SND_MBF */

#ifdef KNL_CFG_USE_FUNC_KNL_MBF_GETNAME
#if TK_USE_OBJ_NAME
/*
 *    Function Name : knl_mbf_getname
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get object name from control block
 *    Param         : ID id : message buffer id
 *                    uint8_t **name : message buffer DS name pointer
 *    Return Code   : ER : Error code
 */
ER knl_mbf_getname(ID id, uint8_t **name)
{
    MBFCB *mbfcb;
    ER ercd = E_OK;

    KNL_CHK_MBFID(id)

    KNL_INT_BEGIN_DISABLE
    mbfcb = KNL_MBF_GET_CB(id);
    if ( mbfcb->mbfid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
    if ( (mbfcb->mbfatr & TA_DSNAME) == 0u ) {
        ercd = E_OBJ;
        goto error_exit;
    }
    *name = mbfcb->name;

error_exit:
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* TK_USE_OBJ_NAME */
#endif /* KNL_CFG_USE_FUNC_KNL_MBF_GETNAME */

#ifdef KNL_CFG_USE_FUNC_KNL_MBF_INIT
/*
 *    Function Name : knl_mbf_init
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Initialization of message buffer control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
ER knl_mbf_init( void )
{
    MBFCB *mbfcb, *mbfcbend;

    /* Get system information */
    if ( TK_NUM_MBFID < 1 ) {
        return E_SYS;
    }

    /* Register all control blocks onto FreeQue */
    knl_que_init(&knl_mbf_freecb);
    mbfcbend = knl_mbf_cbtable + TK_NUM_MBFID;
    for ( mbfcb = knl_mbf_cbtable; mbfcb < mbfcbend; mbfcb++ ) {
        mbfcb->mbfid = 0;
        knl_que_insert(&mbfcb->send_queue, &knl_mbf_freecb);
    }

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_KNL_MBF_INIT */

#ifdef KNL_CFG_USE_FUNC_TK_SND_MBF
/*
 *    Function Name : knl_mbf_relwai
 *    Create Date   : 2012/7/9-2014/1/1
 *    Author        : wangshb
 *    Description   : Processing if the wait task is released
 *    Param         : TCB *tcb : task control block to release
 *    Return Code   : void
 */
void knl_mbf_relwai( TCB *tcb )
{
    knl_mbf_chgpri(tcb, -1);
}
#endif /* KNL_CFG_USE_FUNC_TK_SND_MBF */

#ifdef KNL_CFG_USE_FUNC_KNL_MBF_WAKEUP
/*
 *    Function Name : knl_mbf_wakeup
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Accept message and release wait task,
 *                    as long as there are free message area.
 *    Param         : MBFCB *mbfcb : address of message buffer control block
 *    Return Code   : void
 */
void knl_mbf_wakeup( MBFCB *mbfcb )
{
    TCB *top;
    int32_t msgsz;

    while ( !knl_que_isempty(&mbfcb->send_queue) ) {
        top = (TCB*)mbfcb->send_queue.next;
        msgsz = top->winfo.smbf.msgsz;
        if ( !knl_mbf_isfree(mbfcb, msgsz) ) {
            break;
        }

        /* Store a message from waiting task and release it */
        knl_mbf_sndmsg(mbfcb, top->winfo.smbf.msg, msgsz);
        knl_tsk_wairelok(top);
    }
}
#endif /* KNL_CFG_USE_FUNC_KNL_MBF_WAKEUP */

#ifdef KNL_CFG_USE_FUNC_TK_CRE_MBF
/*
 *    Function Name : tk_cre_mbf_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Create message buffer
 *    Param         : T_CMBF *pk_cmbf : Information about the message buffer to be created
 *    Return Code   : ID : message buffer ID or Error code
 */
SYSCALL ID tk_cre_mbf_impl( T_CMBF *pk_cmbf )
{
#if KNL_CFG_CHK_RSATR
    const ATR VALID_MBFATR = {
         TA_TPRI
        |TA_USERBUF
#if TK_USE_OBJ_NAME
        |TA_DSNAME
#endif
    };
#endif
    MBFCB *mbfcb;
    ID mbfid;
    int32_t bufsz;
    uint8_t *msgbuf;
    ER ercd;

    KNL_CHK_RSATR(pk_cmbf->mbfatr, VALID_MBFATR)
    KNL_CHK_PAR(pk_cmbf->bufsz >= 0)
    KNL_CHK_PAR(pk_cmbf->maxmsz > 0)
#if !TK_USE_MALLOC
    /* TA_USERBUF must be specified if configured in no Imalloc */
    KNL_CHK_PAR((pk_cmbf->mbfatr & TA_USERBUF) != 0);
#endif
    bufsz = knl_mbf_round(pk_cmbf->bufsz);

    if ( bufsz > 0 ) {
#if TK_USE_MALLOC
        if ( (pk_cmbf->mbfatr & TA_USERBUF) != 0u ) {
            /* Size of user buffer must be multiples of sizeof(HEADER) */
            if ( bufsz != pk_cmbf->bufsz ) {
                return E_PAR;
            }
            /* Use user buffer */
            msgbuf = (uint8_t*) pk_cmbf->bufptr;
        } else {
            /* Allocate by kernel */
            msgbuf = knl_mem_imalloc((uint32_t)bufsz);
            if ( msgbuf == NULL ) {
                return E_NOMEM;
            }
        }
#else
        /* Size of user buffer must be multiples of sizeof(HEADER) */
        if ( bufsz != pk_cmbf->bufsz ) {
            return E_PAR;
        }
        /* Use user buffer */
        msgbuf = (uint8_t*) pk_cmbf->bufptr;
#endif
    } else {
        msgbuf = NULL;
    }

    KNL_TSK_BEGIN_CRITICAL_SECTION
    /* Get control block from FreeQue */
    mbfcb = (MBFCB*)knl_que_removenext(&knl_mbf_freecb);
    if ( mbfcb == NULL ) {
        ercd = E_LIMIT;
    } else {
        mbfid = KNL_CFG_ID_MBF(mbfcb - knl_mbf_cbtable);

        /* Initialize control block */
        knl_que_init(&mbfcb->send_queue);
        mbfcb->mbfid = mbfid;
        mbfcb->exinf = pk_cmbf->exinf;
        mbfcb->mbfatr = pk_cmbf->mbfatr;
        knl_que_init(&mbfcb->recv_queue);
        mbfcb->buffer = msgbuf;
        mbfcb->bufsz = mbfcb->frbufsz = bufsz;
        mbfcb->maxmsz = pk_cmbf->maxmsz;
        mbfcb->head = mbfcb->tail = 0;
#if TK_USE_OBJ_NAME
        if ( (pk_cmbf->mbfatr & TA_DSNAME) != 0u ) {
            strncpy((char*)&mbfcb->name, (char*)&pk_cmbf->dsname, TK_OBJ_NAME_LENGTH);
        }
#endif
        ercd = mbfid;
    }
    KNL_TSK_END_CRITICAL_SECTION

#if TK_USE_MALLOC
    if ( (ercd < E_OK) && (msgbuf != NULL) && ((pk_cmbf->mbfatr & TA_USERBUF) == 0u ) ) {
        knl_mem_ifree(msgbuf);
    }
#endif

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_CRE_MBF */

#ifdef KNL_CFG_USE_FUNC_TK_DEL_MBF
/*
 *    Function Name : tk_del_mbf_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Delete message buffer
 *    Param         : ID mbfid : message buffer ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_del_mbf_impl( ID mbfid )
{
    uint8_t  *msgbuf = NULL;
    MBFCB  *mbfcb;
    ER  ercd = E_OK;

    KNL_CHK_MBFID(mbfid)

    mbfcb = KNL_MBF_GET_CB(mbfid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( mbfcb->mbfid == 0 ) {
        ercd = E_NOEXS;
    } else {
        msgbuf = mbfcb->buffer;

        /* Release wait state of task (E_DLT) */
        knl_tsk_waidel(&mbfcb->recv_queue);
        knl_tsk_waidel(&mbfcb->send_queue);

        /* Return to FreeQue */
        knl_que_insert(&mbfcb->send_queue, &knl_mbf_freecb);
        mbfcb->mbfid = 0;
    }
    KNL_TSK_END_CRITICAL_SECTION

#if TK_USE_MALLOC
    if ( ( msgbuf != NULL ) && ( ((mbfcb->mbfatr & TA_USERBUF) == 0u ) ) ) {
        knl_mem_ifree(msgbuf);
    }
#endif

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_DEL_MBF */

#ifdef KNL_CFG_USE_FUNC_TK_RCV_MBF
/*
 *    Function Name : tk_rcv_mbf_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Receive from message buffer
 *    Param         : ID mbfid : message buffer ID
 *                    VP msg : Start address of receive message packet
 *                    TMO tmout : timeout
 *    Return Code   : ER : Error code
 */
SYSCALL int32_t tk_rcv_mbf_impl( ID mbfid, VP msg, TMO tmout )
{
    MBFCB *mbfcb;
    TCB *tcb;
    int32_t rcvsz;
    ER ercd = E_OK;

    KNL_CHK_MBFID(mbfid)
    KNL_CHK_TMOUT(tmout)
    KNL_CHK_DISPATCH()

    mbfcb = KNL_MBF_GET_CB(mbfid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if (mbfcb->mbfid == 0) {
        ercd = E_NOEXS;
        goto error_exit;
    }

    if ( !knl_mbf_empty(mbfcb) ) {
        /* Read from message buffer */
        rcvsz = knl_mbf_getmsg(mbfcb, msg);

        /* Accept message from sending task(s) */
        knl_mbf_wakeup(mbfcb);

    } else if ( !knl_que_isempty(&mbfcb->send_queue) ) {
        /* Receive directly from send wait task */
        tcb = (TCB*)mbfcb->send_queue.next;
        rcvsz = tcb->winfo.smbf.msgsz;
        memcpy(msg, tcb->winfo.smbf.msg, (size_t)rcvsz);
        knl_tsk_wairelok(tcb);
        knl_mbf_wakeup(mbfcb);
    } else {
        ercd = E_TMOUT;
        if ( tmout != TMO_POL ) {
            /* Ready for receive wait */
            knl_tsk_execution->wspec = &knl_mbf_rcv_wspec;
            knl_tsk_execution->wid = mbfid;
            knl_tsk_execution->wercd = &ercd;
            knl_tsk_execution->winfo.rmbf.msg = msg;
            knl_tsk_execution->winfo.rmbf.p_msgsz = &rcvsz;
            knl_tsk_makwai(tmout, mbfcb->mbfatr);
            knl_que_insert(&knl_tsk_execution->tskque, &mbfcb->recv_queue);
        }
    }

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ( ercd < E_OK )? ercd: rcvsz;
}
#endif /* KNL_CFG_USE_FUNC_TK_RCV_MBF */

#ifdef KNL_CFG_USE_FUNC_TK_REF_MBF
/*
 *    Function Name : tk_ref_mbf_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer message buffer state
 *    Param         : ID mbfid : message buffer ID
 *                    T_RMBF *pk_rmbf : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_ref_mbf_impl( ID mbfid, T_RMBF *pk_rmbf )
{
    MBFCB *mbfcb;
    TCB *tcb;
    ER ercd = E_OK;

    KNL_CHK_MBFID(mbfid)

    mbfcb = KNL_MBF_GET_CB(mbfid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( mbfcb->mbfid == 0 ) {
        ercd = E_NOEXS;
    } else {
        pk_rmbf->exinf = mbfcb->exinf;
        pk_rmbf->wtsk = knl_tsk_waitid(&mbfcb->recv_queue);
        pk_rmbf->stsk = knl_tsk_waitid(&mbfcb->send_queue);
        if ( !knl_mbf_empty(mbfcb) ) {
            pk_rmbf->msgsz = *(HEADER*)&mbfcb->buffer[mbfcb->head];
        } else {
            if ( !knl_que_isempty(&mbfcb->send_queue) ) {
                tcb = (TCB*)mbfcb->send_queue.next;
                pk_rmbf->msgsz = tcb->winfo.smbf.msgsz;
            } else {
                pk_rmbf->msgsz = 0;
            }
        }
        pk_rmbf->frbufsz = mbfcb->frbufsz;
        pk_rmbf->maxmsz = mbfcb->maxmsz;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_REF_MBF */

#ifdef KNL_CFG_USE_FUNC_TK_SND_MBF
/*
 *    Function Name : tk_snd_mbf_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Send to message buffer
 *    Param         : ID mbfid : message buffer ID
 *                    VP msg : Start address of send message packet
 *                    int32_t msgsz : Send message size (in bytes)
 *                    TMO tmout : timeout
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_snd_mbf_impl( ID mbfid, VP msg, int32_t msgsz, TMO tmout )
{
    bool8_t  isfree;
    MBFCB  *mbfcb;
    TCB  *tcb;
    ER  ercd = E_OK;

    KNL_CHK_MBFID(mbfid)
    KNL_CHK_PAR(msgsz > 0)
    KNL_CHK_TMOUT(tmout)
    KNL_CHK_DISPATCH_POL(tmout)

    mbfcb = KNL_MBF_GET_CB(mbfid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( mbfcb->mbfid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
#if KNL_CFG_CHK_PAR
    if ( msgsz > mbfcb->maxmsz ) {
        ercd = E_PAR;
        goto error_exit;
    }
#endif

    if ( !knl_que_isempty(&mbfcb->recv_queue) ) {
        /* Send directly to the receive wait task */
        tcb = (TCB*)mbfcb->recv_queue.next;
        memcpy(tcb->winfo.rmbf.msg, msg, (size_t)msgsz);
        *tcb->winfo.rmbf.p_msgsz = msgsz;
        knl_tsk_wairelok(tcb);

    } else {
        tcb = knl_tsk_gcbwaiquetop((GCB*)mbfcb, knl_tsk_execution);
        isfree = knl_mbf_isfree(mbfcb, msgsz);
        if ( ( KNL_TSK_IN_INDP() || ( tcb == knl_tsk_execution ) ) &&  isfree ) {
            /* Store the message to message buffer */
            knl_mbf_sndmsg(mbfcb, msg, msgsz);

        } else {
            ercd = E_TMOUT;
            if ( tmout != TMO_POL ) {
                /* Ready for send wait */
                knl_tsk_execution->wspec = ( (mbfcb->mbfatr & TA_TPRI) != 0u )?
                        &knl_mbf_snd_wspec_tpri: &knl_mbf_snd_wspec_tfifo;
                knl_tsk_execution->wercd = &ercd;
                knl_tsk_execution->winfo.smbf.msg = msg;
                knl_tsk_execution->winfo.smbf.msgsz = msgsz;
                knl_tsk_gcbmakwai((GCB*)mbfcb, tmout);
            }
        }
    }

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_SND_MBF */

/*
 *    Debugger support function
 */
#if TK_USE_DBGSPT

#ifdef KNL_CFG_USE_FUNC_TD_LST_MBF
/*
 *    Function Name : td_lst_mbf_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer message buffer object ID List
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
SYSCALL int32_t td_lst_mbf_impl( ID list[], int32_t nent )
{
    MBFCB *mbfcb, *mbfcbend;
    int32_t n = 0;

    KNL_INT_BEGIN_DISABLE
    mbfcbend = knl_mbf_cbtable + TK_NUM_MBFID;
    for ( mbfcb = knl_mbf_cbtable; mbfcb < mbfcbend; mbfcb++ ) {
        if ( mbfcb->mbfid != 0 ) {
            if ( n < nent ) {
                *list = mbfcb->mbfid;
                list++;
            }
            n++;
        }
    }
    KNL_INT_END_DISABLE

    return n;
}
#endif /* KNL_CFG_USE_FUNC_TD_LST_MBF */

#ifdef KNL_CFG_USE_FUNC_TD_REF_MBF
/*
 *    Function Name : td_ref_mbf_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference message buffer object state
 *    Param         : ID mbfid : Object ID
 *                    TD_RMBF *pk_rmbf : Address of status information packet
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_ref_mbf_impl( ID mbfid, TD_RMBF *pk_rmbf )
{
    MBFCB *mbfcb;
    TCB *tcb;
    ER ercd = E_OK;

    KNL_CHK_MBFID(mbfid)

    mbfcb = KNL_MBF_GET_CB(mbfid);

    KNL_INT_BEGIN_DISABLE
    if ( mbfcb->mbfid == 0 ) {
        ercd = E_NOEXS;
    } else {
        pk_rmbf->exinf = mbfcb->exinf;
        pk_rmbf->wtsk = knl_tsk_waitid(&mbfcb->recv_queue);
        pk_rmbf->stsk = knl_tsk_waitid(&mbfcb->send_queue);
        if ( !knl_mbf_empty(mbfcb) ) {
            pk_rmbf->msgsz = *(HEADER*)&mbfcb->buffer[mbfcb->head];
        } else {
            if ( !knl_que_isempty(&mbfcb->send_queue) ) {
                tcb = (TCB*)mbfcb->send_queue.next;
                pk_rmbf->msgsz = tcb->winfo.smbf.msgsz;
            } else {
                pk_rmbf->msgsz = 0;
            }
        }
        pk_rmbf->frbufsz = mbfcb->frbufsz;
        pk_rmbf->maxmsz = mbfcb->maxmsz;
    }
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TD_REF_MBF */

#ifdef KNL_CFG_USE_FUNC_TD_RMBF_QUE
/*
 *    Function Name : td_rmbf_que_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference message buffer receive wait queue
 *    Param         : ID mbfid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
SYSCALL int32_t td_rmbf_que_impl( ID mbfid, ID list[], int32_t nent )
{
    MBFCB *mbfcb;
    QUEUE *q;
    ER ercd = E_OK;

    KNL_CHK_MBFID(mbfid)

    mbfcb = KNL_MBF_GET_CB(mbfid);

    KNL_INT_BEGIN_DISABLE
    if ( mbfcb->mbfid == 0 ) {
        ercd = E_NOEXS;
    } else {
        int32_t n = 0;
        for ( q = mbfcb->recv_queue.next; q != &mbfcb->recv_queue; q = q->next ) {
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
#endif /* KNL_CFG_USE_FUNC_TD_RMBF_QUE */

#ifdef KNL_CFG_USE_FUNC_TD_SMBF_QUE
/*
 *    Function Name : td_smbf_que_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference message buffer send wait queue
 *    Param         : ID mbfid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : int32_t : Number of waiting tasks or Error code
 */
SYSCALL int32_t td_smbf_que_impl( ID mbfid, ID list[], int32_t nent )
{
    MBFCB *mbfcb;
    QUEUE *q;
    ER ercd = E_OK;

    KNL_CHK_MBFID(mbfid)

    mbfcb = KNL_MBF_GET_CB(mbfid);

    KNL_INT_BEGIN_DISABLE
    if ( mbfcb->mbfid == 0 ) {
        ercd = E_NOEXS;
    } else {
        int32_t n = 0;
        for ( q = mbfcb->send_queue.next; q != &mbfcb->send_queue; q = q->next ) {
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
#endif /* KNL_CFG_USE_FUNC_TD_SMBF_QUE */

#endif /* TK_USE_DBGSPT */

#endif /* KNL_CFG_MAX_MBFID */
