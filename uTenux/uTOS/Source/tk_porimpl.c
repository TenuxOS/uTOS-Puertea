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
 *    File Name      : tk_porimpl.c
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : uT/OS Rendezvous Function.
 *----------------------------------------------------------------------------------------
 */

#include "tk_porimpl.h"

#if KNL_CFG_MAX_PORID > 0

/*
 * Check validity of rendezvous number
 */
#define KNL_POR_CHK_RDVNO(rdvno)   {                                                     \
        ID tskid = knl_por_gettskid(rdvno);                                              \
        if ( !KNL_CFG_CHK_TSKID(tskid) ) {                                               \
            return E_OBJ;                                                                \
        }                                                                                \
}

#define KNL_POR_RDVNO_SHIFT  ( sizeof(RNO) * 8u / 2u )

/*
 * Rendezvous port control block
 */
typedef struct port_control_block {
        QUEUE                call_queue;         /* Port call wait queue */
        ID                   porid;              /* Port ID */
        VP                   exinf;              /* Extended information */
        ATR                  poratr;             /* Port attribute */
        QUEUE                accept_queue;       /* Port accept wait queue */
        int32_t              maxcmsz;            /* Maximum length of call message */
        int32_t              maxrmsz;            /* Maximum length of reply message */
#if TK_USE_OBJ_NAME                              /* name */
        uint8_t              name[TK_OBJ_NAME_LENGTH];
#endif
}                            PORCB;

/*
 * Rendezvous port control block table and free table pointer definition
 */
#ifdef KNL_CFG_USE_FUNC_KNL_POR_CBTABLE
Zeroinit PORCB knl_por_cbtable[TK_NUM_PORID];    /* Rendezvous port control block */
Zeroinit QUEUE knl_por_freecb;                   /* FreeQue */
#define KNL_POR_GET_CB(id)    ( &knl_por_cbtable[KNL_CFG_INDEX_POR(id)] )
#endif /* KNL_CFG_USE_FUNC_KNL_POR_CBTABLE */

#ifdef KNL_CFG_USE_FUNC_KNL_POR_CAL
/*
 * Definition of rendezvous wait specification
 */
WSPEC knl_por_wspec_cal_tfifo     = { TTW_CAL, NULL, NULL };
WSPEC knl_por_wspec_cal_tpri      = { TTW_CAL, knl_por_chgpri, NULL };
#endif /* KNL_CFG_USE_FUNC_KNL_POR_CAL */

#ifdef KNL_CFG_USE_FUNC_KNL_POR_RDV
WSPEC knl_por_wspec_rdv           = { TTW_RDV, NULL, NULL };
#endif /* KNL_CFG_USE_FUNC_KNL_POR_RDV */

#ifdef KNL_CFG_USE_FUNC_TK_ACP_POR
static WSPEC knl_por_wspec_acp    = { TTW_ACP, NULL, NULL };
#endif /* KNL_CFG_USE_FUNC_TK_ACP_POR */

/*
 *    Description   : Genarate a new rendezvous number
 *    Param         : TCB *tcb : task control block 
 *    Return Code   : RNO : returned rendezvous number
 */
Inline RNO knl_por_genrdvno( TCB *tcb )
{
    RNO rdvno;

    rdvno = tcb->wrdvno;
    tcb->wrdvno += (RNO)( 1u << KNL_POR_RDVNO_SHIFT );

    return rdvno;
}

/*
 *    Description   : Get task ID from rendezvous number
 *    Param         : RNO rdvno : rendezvous number
 *    Return Code   : ID : returned task ID
 */
Inline ID knl_por_gettskid( RNO rdvno )
{
    uint32_t tskid;
    
    tskid = (uint32_t)rdvno & ( ( 1u << KNL_POR_RDVNO_SHIFT ) - 1u );
    return (ID)tskid;
}

#ifdef KNL_CFG_USE_FUNC_KNL_POR_CAL
/*
 *    Function Name : knl_por_chgpri
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Processing if the priority of send wait task changes
 *    Param         : TCB *tcb : task control block to change the priority
 *                    int32_t oldpri : old priority, maybe is -1
 *    Return Code   : void
 */
void knl_por_chgpri( TCB *tcb, int32_t oldpri )
{
    PORCB  *porcb;

    porcb = KNL_POR_GET_CB(tcb->wid);
    knl_tsk_gcbchgpri((GCB*)porcb, tcb);
}
#endif /* KNL_CFG_USE_FUNC_KNL_POR_CAL */

#ifdef KNL_CFG_USE_FUNC_KNL_POR_GETNAME
#if TK_USE_OBJ_NAME
/*
 *    Function Name : knl_por_getname
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get rendezvous port object name from control block
 *    Param         : ID id : rendezvous port ID
 *                    uint8_t **name : address of rendezvous port DS name
 *    Return Code   : ER : Error code
 */
ER knl_por_getname(ID id, uint8_t **name)
{
    PORCB *porcb;
    ER  ercd = E_OK;

    KNL_CHK_PORID(id)

    KNL_INT_BEGIN_DISABLE
    porcb = KNL_POR_GET_CB(id);
    if ( porcb->porid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
    if ( (porcb->poratr & TA_DSNAME) == 0u ) {
        ercd = E_OBJ;
        goto error_exit;
    }
    *name = porcb->name;

error_exit:
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* TK_USE_OBJ_NAME */
#endif /* #define KNL_CFG_USE_FUNC_KNL_POR_GETNAME */

#ifdef KNL_CFG_USE_FUNC_KNL_POR_INIT
/*
 *    Function Name : knl_por_init
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Initialization of port control block
 *    Param         : void
 *    Return Code   : ER : Error code
 */
ER knl_por_init( void )
{
    PORCB  *porcb, *porcbend;

    /* Get system information */
    if ( TK_NUM_PORID < 1 ) {
        return E_SYS;
    }

    /* Register all control blocks onto FreeQue */
    knl_que_init(&knl_por_freecb);
    porcbend = knl_por_cbtable + TK_NUM_PORID;
    for ( porcb = knl_por_cbtable; porcb < porcbend; porcb++ ) {
        porcb->porid = 0;
        knl_que_insert(&porcb->call_queue, &knl_por_freecb);
    }

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_KNL_POR_INIT */

#ifdef KNL_CFG_USE_FUNC_TK_ACP_POR
/*
 *    Function Name : tk_acp_por_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Accept rendezvous
 *    Param         : ID porid : Rendezvous port ID
 *                    uint32_t acpptn : Accept bitpattern(indicating condition for acceptance)
 *                    RNO *p_rdvno : Rendezvous number
 *                    VP msg : Message packet address
 *                    TMO tmout : Timeout
 *    Return Code   : int32_t : Call message size (in bytes) or Error code
 */
SYSCALL int32_t tk_acp_por_impl( ID porid, uint32_t acpptn, RNO *p_rdvno, VP msg, TMO tmout )
{
    PORCB  *porcb;
    TCB  *tcb;
    QUEUE  *queue;
    RNO  rdvno;
    int32_t  cmsgsz;
    ER  ercd = E_OK;

    KNL_CHK_PORID(porid)
    KNL_CHK_PAR(acpptn != 0u)
    KNL_CHK_TMOUT(tmout)
    KNL_CHK_DISPATCH()

    porcb = KNL_POR_GET_CB(porid);
    
    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( porcb->porid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }

    /* Search call wait task */
    queue = porcb->call_queue.next;
    while ( queue != &porcb->call_queue ) {
        tcb = (TCB*)queue;
        queue = queue->next;
        if ( (acpptn & tcb->winfo.cal.calptn) != 0u ) {
            /* Receive message */
            *p_rdvno = rdvno = knl_por_genrdvno(tcb);
            cmsgsz = tcb->winfo.cal.cmsgsz;
            if ( cmsgsz > 0 ) {
                memcpy(msg, tcb->winfo.cal.msg, (uint32_t)cmsgsz);
            }

            knl_tsk_waican(tcb);

            /* Make the other task at rendezvous end wait state */
            tcb->wspec = &knl_por_wspec_rdv;
            tcb->wid = 0;
            tcb->winfo.rdv.rdvno = rdvno;
            tcb->winfo.rdv.msg = tcb->winfo.cal.msg;
            tcb->winfo.rdv.maxrmsz = porcb->maxrmsz;
            tcb->winfo.rdv.p_rmsgsz = tcb->winfo.cal.p_rmsgsz;
            knl_tck_insert(&tcb->wtmeb, TMO_FEVR, (CBACK)knl_tsk_waireltmo, tcb);
            knl_que_init(&tcb->tskque);

            goto error_exit;
        }
    }

    ercd = E_TMOUT;
    if ( tmout != TMO_POL ) {
        /* Ready for rendezvous accept wait */
        knl_tsk_execution->wspec = &knl_por_wspec_acp;
        knl_tsk_execution->wid = porid;
        knl_tsk_execution->wercd = &ercd;
        knl_tsk_execution->winfo.acp.acpptn = acpptn;
        knl_tsk_execution->winfo.acp.msg = msg;
        knl_tsk_execution->winfo.acp.p_rdvno = p_rdvno;
        knl_tsk_execution->winfo.acp.p_cmsgsz = &cmsgsz;
        knl_tsk_makwai(tmout, porcb->poratr);
        knl_que_insert(&knl_tsk_execution->tskque, &porcb->accept_queue);
    }

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ( ercd < E_OK )? ercd: cmsgsz;
}
#endif /* KNL_CFG_USE_FUNC_TK_ACP_POR */

#ifdef KNL_CFG_USE_FUNC_TK_CAL_POR
/*
 *    Function Name : tk_cal_por_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Call rendezvous
 *    Param         : ID porid : Rendezvous port ID
 *                    uint32_t calptn : Call bit pattern (indicating conditions of the caller)
 *                    VP msg : Message packet address
 *                    int32_t cmsgsz : Call message size (in bytes)
 *                    TMO tmout : Timeout
 *    Return Code   : int32_t : Reply message size (in bytes) or Error code
 */
SYSCALL int32_t tk_cal_por_impl( ID porid, uint32_t calptn, VP msg, int32_t cmsgsz, TMO tmout )
{
    PORCB  *porcb;
    TCB  *tcb;
    QUEUE  *queue;
    RNO  rdvno;
    int32_t  rmsgsz;
    ER  ercd = E_OK;

    KNL_CHK_PORID(porid)
    KNL_CHK_PAR(calptn != 0u)
    KNL_CHK_PAR(cmsgsz >= 0)
    KNL_CHK_TMOUT(tmout)
    KNL_CHK_DISPATCH()

    porcb = KNL_POR_GET_CB(porid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( porcb->porid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
#if KNL_CFG_CHK_PAR
    if ( cmsgsz > porcb->maxcmsz ) {
        ercd = E_PAR;
        goto error_exit;
    }
#endif

    /* Search accept wait task */
    queue = porcb->accept_queue.next;
    while ( queue != &porcb->accept_queue ) {
        tcb = (TCB*)queue;
        queue = queue->next;
        if ( (calptn & tcb->winfo.acp.acpptn) != 0u ) {
            /* Send message */
            rdvno = knl_por_genrdvno(knl_tsk_execution);
            if ( cmsgsz > 0 ) {
                memcpy(tcb->winfo.acp.msg, msg, (uint32_t)cmsgsz);
            }
            *tcb->winfo.acp.p_rdvno = rdvno;
            *tcb->winfo.acp.p_cmsgsz = cmsgsz;
            knl_tsk_wairelok(tcb);

            /* Ready for rendezvous end wait */
            ercd = E_TMOUT;
            knl_tsk_execution->wspec = &knl_por_wspec_rdv;
            knl_tsk_execution->wid = 0;
            knl_tsk_execution->wercd = &ercd;
            knl_tsk_execution->winfo.rdv.rdvno = rdvno;
            knl_tsk_execution->winfo.rdv.msg = msg;
            knl_tsk_execution->winfo.rdv.maxrmsz = porcb->maxrmsz;
            knl_tsk_execution->winfo.rdv.p_rmsgsz = &rmsgsz;
            knl_tsk_makwai(TMO_FEVR, porcb->poratr);
            knl_que_init(&knl_tsk_execution->tskque);

            goto error_exit;
        }
    }

    /* Ready for rendezvous call wait */
    knl_tsk_execution->wspec = ( (porcb->poratr & TA_TPRI) != 0u )?
                        &knl_por_wspec_cal_tpri: &knl_por_wspec_cal_tfifo;
    knl_tsk_execution->wercd = &ercd;
    knl_tsk_execution->winfo.cal.calptn = calptn;
    knl_tsk_execution->winfo.cal.msg = msg;
    knl_tsk_execution->winfo.cal.cmsgsz = cmsgsz;
    knl_tsk_execution->winfo.cal.p_rmsgsz = &rmsgsz;
    knl_tsk_gcbmakwai((GCB*)porcb, tmout);

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ( ercd < E_OK )? ercd: rmsgsz;
}
#endif /* KNL_CFG_USE_FUNC_TK_CAL_POR */

#ifdef KNL_CFG_USE_FUNC_TK_CRE_POR
/*
 *    Function Name : tk_cre_por_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Create rendezvous port
 *    Param         : T_CPOR *pk_cpor : Information about rendezvous port to be created
 *    Return Code   : ID : Port ID or Error code
 */
SYSCALL ID tk_cre_por_impl( T_CPOR *pk_cpor )
{
#if KNL_CFG_CHK_RSATR
    const ATR VALID_PORATR = {
         TA_TPRI
#if TK_USE_OBJ_NAME
        |TA_DSNAME
#endif
    };
#endif
    PORCB  *porcb;
    ID  porid;
    ER  ercd;

    KNL_CHK_RSATR(pk_cpor->poratr, VALID_PORATR)
    KNL_CHK_PAR(pk_cpor->maxcmsz >= 0)
    KNL_CHK_PAR(pk_cpor->maxrmsz >= 0)
    KNL_CHK_INTSK()

    KNL_TSK_BEGIN_CRITICAL_SECTION
    /* Get control block from FreeQue */
    porcb = (PORCB*)knl_que_removenext(&knl_por_freecb);
    if ( porcb == NULL ) {
        ercd = E_LIMIT;
    } else {
        porid = KNL_CFG_ID_POR(porcb - knl_por_cbtable);

        /* Initialize control block */
        knl_que_init(&porcb->call_queue);
        porcb->porid = porid;
        porcb->exinf = pk_cpor->exinf;
        porcb->poratr = pk_cpor->poratr;
        knl_que_init(&porcb->accept_queue);
        porcb->maxcmsz = pk_cpor->maxcmsz;
        porcb->maxrmsz = pk_cpor->maxrmsz;
#if TK_USE_OBJ_NAME
        if ( (pk_cpor->poratr & TA_DSNAME) != 0u ) {
            strncpy((char*)&porcb->name, (char*)&pk_cpor->dsname, TK_OBJ_NAME_LENGTH);
        }
#endif
        ercd = porid;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_CRE_POR */

#ifdef KNL_CFG_USE_FUNC_TK_DEL_POR
/*
 *    Function Name : tk_del_por_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Delete rendezvous port
 *    Param         : ID porid : Rendezvous port ID
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_del_por_impl( ID porid )
{
    PORCB  *porcb;
    ER  ercd = E_OK;

    KNL_CHK_PORID(porid)
    KNL_CHK_INTSK()

    porcb = KNL_POR_GET_CB(porid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( porcb->porid == 0 ) {
        ercd = E_NOEXS;
    } else {
        /* Release wait state of task (E_DLT) */
        knl_tsk_waidel(&porcb->call_queue);
        knl_tsk_waidel(&porcb->accept_queue);

        /* Return to FreeQue */
        knl_que_insert(&porcb->call_queue, &knl_por_freecb);
        porcb->porid = 0;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_DEL_POR */

#ifdef KNL_CFG_USE_FUNC_TK_FWD_POR
/*
 *    Function Name : tk_fwd_por_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Forward Rendezvous to Other Port
 *    Param         : ID porid : Rendezvous port ID
 *                    uint32_t calptn : Call bit pattern (indicating conditions of the caller)
 *                    RNO rdvno : Rendezvous number before forwarding
 *                    VP msg : Message packet address
 *                    int32_t cmsgsz : Call message size (in bytes)
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_fwd_por_impl( ID porid, uint32_t calptn, RNO rdvno, VP msg, int32_t cmsgsz )
{
    PORCB  *porcb;
    TCB  *caltcb, *tcb;
    QUEUE  *queue;
    RNO  new_rdvno;
    ER  ercd = E_OK;

    KNL_CHK_PORID(porid)
    KNL_CHK_PAR(calptn != 0u)
    KNL_POR_CHK_RDVNO(rdvno)
    KNL_CHK_PAR(cmsgsz >= 0)
    KNL_CHK_INTSK()

    porcb = KNL_POR_GET_CB(porid);
    caltcb = KNL_TSK_GET_CB(knl_por_gettskid(rdvno));

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( porcb->porid == 0 ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
#if KNL_CFG_CHK_PAR
    if ( cmsgsz > porcb->maxcmsz ) {
        ercd = E_PAR;
        goto error_exit;
    }
#endif
    if ( ( (caltcb->state & TS_WAIT) == 0u ) || ( caltcb->wspec != &knl_por_wspec_rdv )  ||
         ( rdvno != caltcb->winfo.rdv.rdvno ) ) {
        ercd = E_OBJ;
        goto error_exit;
    }
    if ( porcb->maxrmsz > caltcb->winfo.rdv.maxrmsz ) {
        ercd = E_OBJ;
        goto error_exit;
    }
#if KNL_CFG_CHK_PAR
    if ( cmsgsz > caltcb->winfo.rdv.maxrmsz ) {
        ercd = E_PAR;
        goto error_exit;
    }
#endif

    /* Search accept wait task */
    queue = porcb->accept_queue.next;
    while ( queue != &porcb->accept_queue ) {
        tcb = (TCB*)queue;
        queue = queue->next;
        if ( (calptn & tcb->winfo.acp.acpptn) != 0u ) {
            /* Send message */
            new_rdvno = knl_por_genrdvno(caltcb);
            if ( cmsgsz > 0 ) {
                memcpy(tcb->winfo.acp.msg, msg, (uint32_t)cmsgsz);
            }
            *tcb->winfo.acp.p_rdvno = new_rdvno;
            *tcb->winfo.acp.p_cmsgsz = cmsgsz;
            knl_tsk_wairelok(tcb);

            /* Change rendezvous end wait of the other task */
            caltcb->winfo.rdv.rdvno = new_rdvno;
            caltcb->winfo.rdv.msg = caltcb->winfo.cal.msg;
            caltcb->winfo.rdv.maxrmsz = porcb->maxrmsz;
            caltcb->winfo.rdv.p_rmsgsz = caltcb->winfo.cal.p_rmsgsz;

            goto error_exit;
        }
    }

    /* Change the other task to rendezvous call wait */
    caltcb->wspec = ( (porcb->poratr & TA_TPRI) != 0u )?
                &knl_por_wspec_cal_tpri: &knl_por_wspec_cal_tfifo;
    caltcb->wid = porid;
    caltcb->winfo.cal.calptn = calptn;
    caltcb->winfo.cal.msg = caltcb->winfo.rdv.msg;
    caltcb->winfo.cal.cmsgsz = cmsgsz;
    caltcb->winfo.cal.p_rmsgsz = caltcb->winfo.rdv.p_rmsgsz;
    knl_tck_insert(&caltcb->wtmeb, TMO_FEVR, (CBACK)knl_tsk_waireltmo, caltcb);
    if ( (porcb->poratr & TA_TPRI) != 0u ) {
        knl_tsk_queinspri(caltcb, &porcb->call_queue);
    } else {
        knl_que_insert(&caltcb->tskque, &porcb->call_queue);
    }

    if ( cmsgsz > 0 ) {
        memcpy(caltcb->winfo.cal.msg, msg, (uint32_t)cmsgsz);
    }

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_FWD_POR */

#ifdef KNL_CFG_USE_FUNC_TK_REF_POR
/*
 *    Function Name : tk_ref_por_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reference rendezvous port Status
 *    Param         : ID porid : Rendezvous port ID
 *                    T_RPOR *pk_rpor : Start address of packet for status information
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_ref_por_impl( ID porid, T_RPOR *pk_rpor )
{
    PORCB  *porcb;
    ER  ercd = E_OK;

    KNL_CHK_PORID(porid)

    porcb = KNL_POR_GET_CB(porid);

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( porcb->porid == 0 ) {
        ercd = E_NOEXS;
    } else {
        pk_rpor->exinf = porcb->exinf;
        pk_rpor->wtsk = knl_tsk_waitid(&porcb->call_queue);
        pk_rpor->atsk = knl_tsk_waitid(&porcb->accept_queue);
        pk_rpor->maxcmsz = porcb->maxcmsz;
        pk_rpor->maxrmsz = porcb->maxrmsz;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_REF_POR */

#ifdef KNL_CFG_USE_FUNC_TK_RPL_RDV
/*
 *    Function Name : tk_rpl_rdv_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Reply rendezvous
 *    Param         : RNO rdvno : Rendezvous number
 *                    VP msg : Reply message packet address
 *                    int32_t rmsgsz : Reply message size (in bytes)
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_rpl_rdv_impl( RNO rdvno, VP msg, int32_t rmsgsz )
{
    TCB  *caltcb;
    ER  ercd = E_OK;

    KNL_POR_CHK_RDVNO(rdvno)
    KNL_CHK_PAR(rmsgsz >= 0)
    KNL_CHK_INTSK()

    caltcb = KNL_TSK_GET_CB(knl_por_gettskid(rdvno));

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( ( (caltcb->state & TS_WAIT) == 0u ) || ( caltcb->wspec != &knl_por_wspec_rdv ) || 
         ( rdvno != caltcb->winfo.rdv.rdvno ) ) {
        ercd = E_OBJ;
        goto error_exit;
    }
#if KNL_CFG_CHK_PAR
    if ( rmsgsz > caltcb->winfo.rdv.maxrmsz ) {
        ercd = E_PAR;
        goto error_exit;
    }
#endif

    /* Send message */
    if ( rmsgsz > 0 ) {
        memcpy(caltcb->winfo.rdv.msg, msg, (uint32_t)rmsgsz);
    }
    *caltcb->winfo.rdv.p_rmsgsz = rmsgsz;
    knl_tsk_wairelok(caltcb);

error_exit:
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_RPL_RDV */

/*
 *    Debugger support function
 */
#if TK_USE_DBGSPT

#ifdef KNL_CFG_USE_FUNC_TD_ACP_QUE
/*
 *    Function Name : td_acp_que_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer rendezvous accept wait queue
 *    Param         : ID porid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : ER : Error code
 */
SYSCALL int32_t td_acp_que_impl( ID porid, ID list[], int32_t nent )
{
    PORCB  *porcb;
    QUEUE  *q;
    ER  ercd = E_OK;

    KNL_CHK_PORID(porid)

    porcb = KNL_POR_GET_CB(porid);

    KNL_INT_BEGIN_DISABLE
    if ( porcb->porid == 0 ) {
        ercd = E_NOEXS;
    } else {
        int32_t n = 0;
        for ( q = porcb->accept_queue.next; q != &porcb->accept_queue; q = q->next ) {
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
#endif /* KNL_CFG_USE_FUNC_TD_ACP_QUE */

#ifdef KNL_CFG_USE_FUNC_TD_CAL_QUE
/*
 *    Function Name : td_cal_que_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer rendezvous call wait queue
 *    Param         : ID porid : Object ID
 *                    ID list[] : Location of waiting task IDs
 *                    int32_t nent : Maximum number of list entries
 *    Return Code   : ER : Error code
 */
SYSCALL int32_t td_cal_que_impl( ID porid, ID list[], int32_t nent )
{
    PORCB  *porcb;
    QUEUE  *q;
    ER  ercd = E_OK;

    KNL_CHK_PORID(porid)

    porcb = KNL_POR_GET_CB(porid);

    KNL_INT_BEGIN_DISABLE
    if ( porcb->porid == 0 ) {
        ercd = E_NOEXS;
    } else {
        int32_t n = 0;
        for ( q = porcb->call_queue.next; q != &porcb->call_queue; q = q->next ) {
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
#endif /* KNL_CFG_USE_FUNC_TD_CAL_QUE */

#ifdef KNL_CFG_USE_FUNC_TD_LST_POR
/*
 *    Function Name : td_lst_por_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer rendezvous port object ID list
 *    Param         : ID list[] : Location of object ID list
 *                    int32_t nent : Maximum number of list entries to retrieve
 *    Return Code   : int32_t : Number of objects used or Error code
 */
SYSCALL int32_t td_lst_por_impl( ID list[], int32_t nent )
{
    PORCB  *porcb, *porcbend;
    int32_t  n = 0;

    KNL_INT_BEGIN_DISABLE
    porcbend = knl_por_cbtable + TK_NUM_PORID;
    for ( porcb = knl_por_cbtable; porcb < porcbend; porcb++ ) {
        if ( porcb->porid != 0 ) {
            if ( n < nent ) {
                *list = porcb->porid;
                list++;
            }
            n++;
        }
    }
    KNL_INT_END_DISABLE

    return n;
}
#endif /* KNL_CFG_USE_FUNC_TD_LST_POR */

#ifdef KNL_CFG_USE_FUNC_TD_REF_POR
/*
 *    Function Name : td_ref_por_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Refer rendezvous port
 *    Param         : ID porid : Object ID
 *                    TD_RPOR *rpor : address of status information packet
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_ref_por_impl( ID porid, TD_RPOR *pk_rpor )
{
    PORCB *porcb;
    ER  ercd = E_OK;

    KNL_CHK_PORID(porid)

    porcb = KNL_POR_GET_CB(porid);

    KNL_INT_BEGIN_DISABLE
    if ( porcb->porid == 0 ) {
        ercd = E_NOEXS;
    } else {
        pk_rpor->exinf = porcb->exinf;
        pk_rpor->wtsk = knl_tsk_waitid(&porcb->call_queue);
        pk_rpor->atsk = knl_tsk_waitid(&porcb->accept_queue);
        pk_rpor->maxcmsz = porcb->maxcmsz;
        pk_rpor->maxrmsz = porcb->maxrmsz;
    }
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TD_REF_POR */

#endif /* TK_USE_DBGSPT */

#endif /* KNL_CFG_MAX_PORID */
