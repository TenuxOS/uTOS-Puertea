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
 *    File Name      : knl_tsk.c
 *    Create Date    : 2012/9/4-2014/4/1
 *    Author         : wangshb
 *    Description    : Task internal operation routine function.
 *----------------------------------------------------------------------------------------
 */

#include "knl_tsk.h"
#include "tk_mtximpl.h"

#ifdef KNL_CFG_USE_FUNC_CTXTSK
/*
 * Task independent part flag 
 */
Zeroinit int32_t  knl_tsk_independent;

/* 
 * Task mode flag 
 */
Zeroinit uint32_t  knl_tsk_mode;

/*
 * Task dispatch disable state
 */
Zeroinit int32_t knl_tsk_dspdisabled;            /* DDS_XXX see task.h */

/*
 * Task execution control 
 */
Zeroinit TCB *   knl_tsk_execution;              /* Task in execution */
Zeroinit TCB *   knl_tsk_schedule;               /* Task which should be executed */
Zeroinit RDYQUE  knl_tsk_rdyque;                 /* Ready queue */
#endif /* KNL_CFG_USE_FUNC_CTXTSK */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_CBTABLE
/*
 * Task control information
 */
Zeroinit TCB knl_tsk_cbtable[TK_NUM_TSKID];      /* Task control block */
Zeroinit QUEUE knl_tsk_freecb;                   /* FreeQue */
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_CBTABLE */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_CHGPRI
/*
 *    Function Name : knl_tsk_chgpri
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Change task priority, Then make the required task state transition
 *                    occur.
 *    Param         : TCB *tcb: task control block
 *                    PRI priority: priority
 *    Return Code   : void
 */
void knl_tsk_chgpri( TCB *tcb, PRI priority )
{
    PRI oldpri;

    if ( tcb->state == TS_READY ) {
        /*
         * When deleting a task from the ready queue, 
         * a value in the 'priority' field in TCB is needed. 
         * Therefore you need to delete the task from the
         * ready queue before changing 'tcb->priority.'
         */
        knl_tsk_rdyquedel(&knl_tsk_rdyque, tcb);
        tcb->priority = (uint8_t)priority;
        knl_tsk_rdyqueins(&knl_tsk_rdyque, tcb);
        knl_tsk_reschedule();
    } else {
        oldpri = tcb->priority;
        tcb->priority = priority;

        /* If the hook routine at the task priority change is defined, execute it */
        if ( ( (tcb->state & TS_WAIT) != 0u ) && ( tcb->wspec->chg_pri_hook ) ) {
            (*tcb->wspec->chg_pri_hook)(tcb, (int32_t)oldpri);
        }
    }
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_CHGPRI */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_DEL
/*
 *    Function Name : knl_tsk_del
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Task deletion Call from critical section
 *    Param         : TCB *tcb: task control block
 *    Return Code   : void
 */
void knl_tsk_del( TCB *tcb )
{
#if TK_USE_MALLOC
    if ( (tcb->tskatr & TA_USERBUF) == 0u ) {
        /* User buffer is not used */
        /* Free system stack */
        VP stack = (uint8_t*)tcb->isstack - tcb->sstksz;
        knl_mem_ifree(stack);
    }
#endif

    /* Return control block to FreeQue */
    knl_que_insert(&tcb->tskque, &knl_tsk_freecb);
    tcb->state = TS_NONEXIST;
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_DEL */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_GCBCHGPRI
/*
 *    Function Name : knl_tsk_gcbchgpri
 *    Create Date   : 2012/9/4-2014/2/1
 *    Author        : wangshb
 *    Description   : When the task priority changes, adjust the task position at the 
 *                     wait queue. It is called only if the object attribute TA_TPRI 
 *                     is specified.
 *    Param         : GCB *gcb: generic control block
 *                    TCB *tcb: task control block
 *    Return Code   : void
 */
void knl_tsk_gcbchgpri( GCB *gcb, TCB *tcb )
{
    knl_que_remove(&tcb->tskque);
    knl_tsk_queinspri(tcb, &gcb->wait_queue);
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_GCBCHGPRI */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_GCBMAKWAI
/*
 *    Function Name : knl_tsk_gcbmakwai
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Change the active task state to wait state and connect to the timer
 *                     wait queue and the object wait queue. Also set 'wid' in 
 *                     'knl_tsk_execution'.
 *    Param         : GCB *gcb: generic control block
 *                    TMO tmout: TMO tmout
 *    Return Code   : void
 */
void knl_tsk_gcbmakwai( GCB *gcb, TMO tmout )
{
    *knl_tsk_execution->wercd = E_TMOUT;
    if ( tmout != TMO_POL ) {
        knl_tsk_execution->wid = gcb->objid;
        knl_tsk_makwai(tmout, gcb->objatr);
        if ( (gcb->objatr & TA_TPRI) != 0u ) {
            knl_tsk_queinspri(knl_tsk_execution, &gcb->wait_queue);
        } else {
            knl_que_insert(&knl_tsk_execution->tskque, &gcb->wait_queue);
        }
    }
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_GCBMAKWAI */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_GCBWAIQUETOP
/*
 *    Function Name : knl_tsk_gcbwaiquetop
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Search the first task of wait queue include "tcb" with target.
 *                     (Not insert "tcb" into wait queue.)
 *    Param         : GCB *gcb: generic control block
 *                    TCB *tcb: task control block
 *    Return Code   : TCB *tcb: task control block
 */
TCB* knl_tsk_gcbwaiquetop( GCB *gcb, TCB *tcb )
{
    TCB *q;

    if ( knl_que_isempty(&gcb->wait_queue) ) {
        return tcb;
    }

    q = (TCB*)gcb->wait_queue.next;
    if ( (gcb->objatr & TA_TPRI) == 0u ) {
        return q;
    }

    return ( tcb->priority < q->priority )? tcb: q;
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_GCBWAIQUETOP */

/*
 *    Function Name : knl_tsk_lock
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Object lock Do not call from critical section.
 *    Param         : OBJLOCK *loc: object lock
 *    Return Code   : void
 */
void knl_tsk_lock( OBJLOCK *loc )
{
    bool8_t klocked;

    while ( true ) {
        KNL_TSK_BEGIN_CRITICAL_SECTION
        klocked = (bool8_t)knl_tsk_execution->klocked;
        if ( !klocked ) {
            if ( loc->wtskq.next == NULL ) {
                /* Lock */
                knl_que_init(&loc->wtskq);
                klocked = true;
                knl_tsk_execution->klocked = (uint32_t)klocked;
                knl_tsk_rdyque.klocktsk = knl_tsk_execution;
            } else {
                /* Ready for lock */
                knl_tsk_rdyquedel(&knl_tsk_rdyque, knl_tsk_execution);
                knl_tsk_execution->klockwait = (uint32_t)true;
                knl_que_insert(&knl_tsk_execution->tskque, &loc->wtskq);

                knl_tsk_schedule = knl_tsk_rdyquetop(&knl_tsk_rdyque);
                knl_dsp_request();
            }
        }
        KNL_TSK_END_CRITICAL_SECTION
        /* Since wait could be freed without getting lock,
           need to re-try if lock is not got */
        if ( klocked ) {
            break;
        }
    }
}

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_MAKDOR
/*
 *    Function Name : knl_tsk_makdor
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Prepare task execution.
 *    Param         : TCB *tcb: task control block
 *    Return Code   : void
 */
void knl_tsk_makdor( TCB *tcb )
{
    /* Initialize variables which should be reset at DORMANT state */
    tcb->state = TS_DORMANT;
    tcb->priority = tcb->bpriority = tcb->ipriority;
    tcb->sysmode = tcb->isysmode;
    tcb->wupcnt = 0;
    tcb->suscnt = 0;

    tcb->klockwait = (uint32_t)false;
    tcb->klocked   = (uint32_t)false;

#if TK_USE_DBGSPT && defined(KNL_CFG_USE_FUNC_TD_INF_TSK)
    tcb->stime = 0u;
    tcb->utime = 0u;
#endif

    tcb->wercd = NULL;

#if KNL_CFG_MAX_MTXID > 0
    tcb->mtxlist = NULL;
#endif

    /* Set context to start task */
    knl_ctx_setframe(tcb);
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_MAKDOR */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_MAKNORDY
/*
 *    Function Name : knl_tsk_maknordy
 *    Create Date   : 2012/9/4-2014/2/1
 *    Author        : wangshb
 *    Description   : Set task to non-executable state(wait state,forced wait,or dormant  
 *                    state).
 *                     Delete the task from the ready queue.
 *                     If the deleted task is 'knl_tsk_schedule', set 'knl_tsk_schedule' to the
 *                     highest priority task in the ready queue. 
 *                     When calling this function, 'tcb' task must be READY.
 *    Param         : TCB *tcb: task control block
 *    Return Code   : void
 */
void knl_tsk_maknordy( TCB *tcb )
{
    knl_tsk_rdyquedel(&knl_tsk_rdyque, tcb);
    if ( knl_tsk_schedule == tcb ) {
        knl_tsk_schedule = knl_tsk_rdyquetop(&knl_tsk_rdyque);
        knl_dsp_request();
    }
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_MAKNORDY */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_MAKRDY
/*
 *    Function Name : knl_tsk_makrdy
 *    Create Date   : 2012/9/4-2014/2/1
 *    Author        : wangshb
 *    Description   : Set task to READY state for making task executable.
 *                     If the 'tcb' task priority is higher than the executed task,
 *                     make it executable. If the priority is lower, connect the task 
 *                     to the ready queue.
 *                     Update the task state and insert in the ready queue. If necessary, 
 *                     update 'knl_tsk_schedule' and request to start task dispatcher. 
 *    Param         : TCB *tcb: task control block
 *    Return Code   : void
 */
void knl_tsk_makrdy( TCB *tcb )
{
    tcb->state = TS_READY;
    if ( knl_tsk_rdyqueins(&knl_tsk_rdyque, tcb) ) {
        knl_tsk_schedule = tcb;
        knl_dsp_request();
    }
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_MAKRDY */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_MAKWAI
/*
 *    Function Name : knl_tsk_makwai
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Change the active task state to wait state and connect to the
 *                     timer event queue. Normally, 'knl_tsk_execution' is in the RUN state, 
 *                     but when an interrupt occurs during executing system call, 
 *                     'knl_tsk_execution' may become the other state by system call called 
 *                     in the interrupt handler.
 *                     However, it does not be in WAIT state.
 *    Param         : TMO tmout: Timeout
 *                    ATR atr: attribute
 *    Return Code   : void
 */
void knl_tsk_makwai( TMO tmout, ATR atr )
{
    switch ( knl_tsk_execution->state ) {
        case TS_READY:
            knl_tsk_maknordy(knl_tsk_execution);
            knl_tsk_execution->state = TS_WAIT;
            break;
        case TS_SUSPEND:
            knl_tsk_execution->state = TS_WAITSUS;
            break;
        default:
            break;
    }
    knl_tck_insert(&knl_tsk_execution->wtmeb, tmout,(CBACK)knl_tsk_waireltmo,            \
                    knl_tsk_execution);
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_MAKWAI */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_MAKWAIRELTIM
/*
 *    Function Name : knl_tsk_makwaireltim
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Change the active task state to wait state and connect to the
 *                     timer event queue. Normally, 'knl_tsk_execution' is in the RUN state, 
 *                     but when an interrupt occurs during executing system call, 
 *                     'knl_tsk_execution' may become the other state by system call called 
 *                     in the interrupt handler.
 *                     However, it does not be in WAIT state.
 *    Param         : TMO tmout: Timeout
 *                    ATR atr: attribute
 *    Return Code   : void
 */
void knl_tsk_makwaireltim( RELTIM tmout, ATR atr )
{
    switch ( knl_tsk_execution->state ) {
        case TS_READY:
            knl_tsk_maknordy(knl_tsk_execution);
            knl_tsk_execution->state = TS_WAIT;
            break;
        case TS_SUSPEND:
            knl_tsk_execution->state = TS_WAITSUS;
            break;
        default:
            break;
    }
    knl_tck_insrel(&knl_tsk_execution->wtmeb, tmout, (CBACK)knl_tsk_waireltmo,           \
                    knl_tsk_execution);
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_MAKWAIRELTIM */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_RDYQUEROTRDY
/*
 *    Function Name : knl_tsk_rdyquerotrdy
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Rotate the priority ready queue at 'priority'.
 *    Param         : PRI priority: priority
 *    Return Code   : void
 */
void knl_tsk_rdyquerotrdy( PRI priority )
{
    knl_tsk_rdyquerot(&knl_tsk_rdyque, priority);
    knl_tsk_reschedule();
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_RDYQUEROTRDY */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_RUNRDYQUEROT
/*
 *    Function Name : knl_tsk_rdyquerotrun
 *    Create Date   : 2012/9/4-2014/2/1
 *    Author        : wangshb
 *    Description   : Rotate ready queue including the highest priority task in the 
 *                    ready queue.
 *    Param         : void
 *    Return Code   : void
 */
void knl_tsk_rdyquerotrun( void )
{
    if ( knl_tsk_schedule != NULL ) {
        knl_tsk_rdyquerot(&knl_tsk_rdyque,
        knl_tsk_rdyquetoppri(&knl_tsk_rdyque));
        knl_tsk_reschedule();
    }
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_RUNRDYQUEROT */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_TER
/*
 *    Function Name : knl_tsk_ter
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Task finalization Call from critical section
 *    Param         : TCB *tcb: task control block
 *    Return Code   : void
 */
void knl_tsk_ter( TCB *tcb )
{
    TSTAT state;

    state = (TSTAT)tcb->state;
    if ( state == TS_READY ) {
        knl_tsk_maknordy(tcb);

    } else if ( (state & TS_WAIT) != 0u ) {
        knl_tsk_waican(tcb);
        if ( tcb->wspec->rel_wai_hook != NULL ) {
            (*tcb->wspec->rel_wai_hook)(tcb);
        }
    }

#if KNL_CFG_MAX_MTXID > 0
    /* signal mutex */
    knl_mtx_sigall(tcb);
#endif

    knl_ctx_cleanup(tcb);
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_TER */

/*
 * Debug support function
 */
#if TK_USE_DBGSPT

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_GETNAME
#if TK_USE_OBJ_NAME
/*
 *    Function Name : knl_tsk_getname
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Get object name from control block.
 *    Param         : ID id: task ID
 *                    uint8_t **name: point to dsname
 *    Return Code   : ER : Error code
 */
ER knl_tsk_getname(ID id, uint8_t **name)
{
    TCB *tcb;
    ER ercd = E_OK;

    KNL_CHK_TSKID_SELF(id)

    KNL_INT_BEGIN_DISABLE
    tcb = KNL_TSK_GET_CB_SELF(id);
    if ( tcb->state == TS_NONEXIST ) {
        ercd = E_NOEXS;
        goto error_exit;
    }
    if ( (tcb->tskatr & TA_DSNAME) == 0u ) {
        ercd = E_OBJ;
        goto error_exit;
    }
    *name = tcb->name;

error_exit:
    KNL_INT_END_DISABLE

    return ercd;
}
#endif /* TK_USE_OBJ_NAME */
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_GETNAME */

#endif /* TK_USE_DBGSPT */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_INIT
/*
 *    Function Name : knl_tsk_init
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Task module initialization
 *    Param         : void
 *    Return Code   : ER : Error code
 */
ER knl_tsk_init( void )
{
    int32_t i;
    TCB *tcb;
    ID tskid;

    /* Get system information */
    if ( TK_NUM_TSKID < 1 ) {
        return E_SYS;
    }

    /* Initialize task execution control information */
    knl_tsk_execution = knl_tsk_schedule = NULL;
    knl_tsk_rdyqueinit(&knl_tsk_rdyque);
    knl_tsk_dspdisabled = KNL_TSK_DDS_ENABLE;

    /* Register all TCBs onto FreeQue */
    knl_que_init(&knl_tsk_freecb);
    tcb = knl_tsk_cbtable;
    for ( i = 0; i < TK_NUM_TSKID; i++ ) {
        tskid = KNL_CFG_ID_TSK(i);
        tcb->tskid = tskid;
        tcb->state = TS_NONEXIST;
#if KNL_CFG_MAX_PORID > 0
        tcb->wrdvno = tskid;
#endif

        knl_que_insert(&tcb->tskque, &knl_tsk_freecb);
        tcb++;
    }

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_INIT */

/*
 *    Function Name : knl_tsk_unlock
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Object unlock, may be called from a critical section.
 *    Param         : OBJLOCK *loc: object lock
 *    Return Code   : void
 */
void knl_tsk_unlock( OBJLOCK *loc )
{
    TCB *tcb;

    KNL_TSK_BEGIN_CRITICAL_SECTION
    knl_tsk_execution->klocked = (uint32_t)false;
    knl_tsk_rdyque.klocktsk = NULL;

    tcb = (TCB*)knl_que_removenext(&loc->wtskq);
    if ( tcb == NULL ) {
        /* Free lock */
        loc->wtskq.next = NULL;
    } else {
        /* Wake lock wait task */
        tcb->klockwait = (uint32_t)false;
        tcb->klocked = (uint32_t)true;
        knl_tsk_rdyqueinstop(&knl_tsk_rdyque, tcb);
    }

    knl_tsk_schedule = knl_tsk_rdyquetop(&knl_tsk_rdyque);
    if ( knl_tsk_execution != knl_tsk_schedule ) {
        knl_dsp_request();
    }
    KNL_TSK_END_CRITICAL_SECTION
}

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_WAIDEL
/*
 *    Function Name : knl_tsk_waidel
 *    Create Date   : 2012/9/4-2014/2/1
 *    Author        : wangshb
 *    Description   : Release all tasks connected to the wait queue, and define it
 *                     as E_DLT error. Use when synchronization between tasks or 
 *                     communication object is deleted.
 *    Param         : QUEUE *wait_queue: wait queue
 *    Return Code   : void
 */
void knl_tsk_waidel( QUEUE *wait_queue )
{
    TCB *tcb;

    while ( !knl_que_isempty(wait_queue) ) {
        tcb = (TCB*)wait_queue->next;
        knl_tsk_wairel(tcb);
        *tcb->wercd = E_DLT;
    }
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_WAIDEL */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_OKWAIREL
/*
 *    Function Name : knl_tsk_wairelok
 *    Create Date   : 2012/9/4-2014/2/1
 *    Author        : wangshb
 *    Description   : Release wait state of the task.
 *                     Remove the task from the timer queue and the wait queue, then
 *                     update the task state to E_OK.
 *    Param         : TCB *tcb: task control block
 *    Return Code   : void
 */
void knl_tsk_wairelok( TCB *tcb )
{
    knl_tsk_wairel(tcb);
    *tcb->wercd = E_OK;
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_OKWAIREL */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_ERWAIREL
/*
 *    Function Name : knl_tsk_waireler
 *    Create Date   : 2012/9/4-2014/2/1
 *    Author        : wangshb
 *    Description   : Release wait state of the task.
 *                     Remove the task from the timer queue and the wait queue, then
 *                     update the task state to ercd(>0).
 *    Param         : TCB *tcb: task control block
 *                    ER ercd: ER ercd
 *    Return Code   : void
 */
void knl_tsk_waireler( TCB *tcb, ER ercd )
{
    knl_tsk_wairel(tcb);
    *tcb->wercd = ercd;
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_ERWAIREL */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_NGWAIREL
/*
 *    Function Name : knl_tsk_wairelng
 *    Create Date   : 2012/9/4-2014/2/1
 *    Author        : wangshb
 *    Description   : Release wait state of the task.
 *                     Remove the task from the timer queue and the wait queue, then
 *                     update the task state to ercd(<0).
 *                     use for releasing the forced wait task. 
 *    Param         : TCB *tcb: task control block
 *                    ER ercd: ER ercd
 *    Return Code   : void
 */
void knl_tsk_wairelng( TCB *tcb, ER ercd )
{
    knl_tsk_wairel(tcb);
    if ( tcb->wspec->rel_wai_hook != NULL ) {
        (*tcb->wspec->rel_wai_hook)(tcb);
    }
    *tcb->wercd = ercd;
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_NGWAIREL */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_TMOWAIREL
/*
 *    Function Name : knl_tsk_waireltmo
 *    Create Date   : 2012/9/4-2014/2/1
 *    Author        : wangshb
 *    Description   : Release wait state of the task.
 *                     don't remove from the timer queue. Use for time out processing.
 *    Param         : TCB *tcb: task control block
 *    Return Code   : void
 */
void knl_tsk_waireltmo( TCB *tcb )
{
    knl_que_remove(&tcb->tskque);
    knl_tsk_maknowai(tcb);
    if ( tcb->wspec->rel_wai_hook != NULL ) {
        (*tcb->wspec->rel_wai_hook)(tcb);
    }
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_TMOWAIREL */

#ifdef KNL_CFG_USE_FUNC_KNL_TSK_WAITID
/*
 *    Function Name : knl_tsk_waitid
 *    Create Date   : 2012/9/4-2014/2/1
 *    Author        : wangshb
 *    Description   : Get ID of the head task in the wait queue.
 *    Param         : QUEUE *wait_queue: wait queue
 *    Return Code   : ID: task ID
 */
ID knl_tsk_waitid( QUEUE *wait_queue )
{
    if ( knl_que_isempty(wait_queue) ) {
        return 0;
    }

    return ((TCB*)wait_queue->next)->tskid;
}
#endif /* KNL_CFG_USE_FUNC_KNL_TSK_WAITID */
