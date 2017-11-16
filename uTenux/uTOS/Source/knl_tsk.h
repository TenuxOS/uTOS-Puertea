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
 *    File Name      : knl_tsk.h
 *    Create Date    : 2012/9/4-2014/4/1
 *    Author         : wangshb
 *    Description    : Task internal operation routine head file.
 *----------------------------------------------------------------------------------------
 */

#include "knl_kernel.h"

#ifndef _KNL_TSK_H__
#define _KNL_TSK_H__

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------------------
 *    Macro Define Section
 *--------------------------------------------------------------------------------------*/

/* Task ready queue's bitmap width and number */
#define KNL_TSK_BITMAPUNIT   ( sizeof(uint32_t) * 8u )
#define KNL_TSK_BITMAP       ( (TK_NUM_PRI + KNL_TSK_BITMAPUNIT - 1u) / KNL_TSK_BITMAPUNIT )

/* 
 * CTXB struct member offset, used in knl_*.S
 */
/* CTXB.ssp offset */
#define KNL_TSK_CTXB_SSP     0

/* 
 * WINFO union's size, min is 0bytes, max is 20bytes
 */
#if KNL_CFG_MAX_PORID > 0
#define KNL_TSK_WINFO_SIZE   20
#else
#if KNL_CFG_MAX_FLGID > 0
#define KNL_TSK_WINFO_SIZE   12
#else
#if ( KNL_CFG_MAX_MBFID > 0 ) || ( KNL_CFG_MAX_MPLID > 0 )
#define KNL_TSK_WINFO_SIZE   8
#else
#if ( KNL_CFG_MAX_SEMID > 0 ) || ( KNL_CFG_MAX_MBXID > 0 ) || ( KNL_CFG_MAX_MPFID > 0 )
#define KNL_TSK_WINFO_SIZE   4
#else
#define KNL_TSK_WINFO_SIZE   0
#endif
#endif
#endif
#endif

/* 
 * TCB struct member offset and size, used in knl_*.S
 */

/* TCB.tskid offset, formula is sizeof(QUEUE  */
#define KNL_TSK_TCB_TSKID    8

/* TCB.state offset, formula is sizeof(QUEUE ) + sizeof(ID) +sizeof(VP) + sizeof(ATR)
 *                            + sizeof(FP) + sizeof(int32_t) + 7*sizeof(uint8_t) */
#define KNL_TSK_TCB_STATE    35

/* TCB.winfo offset, formula is KNL_TSK_TCB_STATE + sizeof(uint8_t) +  6*sizeof(int32_t) */
#define KNL_TSK_TCB_WINFO    60

/* TCB.wtmeb offset */
#define KNL_TSK_TCB_WTMEB    ( KNL_TSK_TCB_WINFO + KNL_TSK_WINFO_SIZE )

/* TCB.wrdvno size, formula is sizeof(RNO) */
#if KNL_CFG_MAX_PORID > 0
#define KNL_TSK_TCB_POR_SIZE 4
#else
#define KNL_TSK_TCB_POR_SIZE 0
#endif

/* TCB.mtxlist size, formula is sizeof(MTXCB*) */
#if KNL_CFG_MAX_MTXID > 0
#define KNL_TSK_TCB_MTX_SIZE 4
#else
#define KNL_TSK_TCB_MTX_SIZE 0
#endif

/* TCB.stime and .utime size, formula is 2*sizeof(uint32_t) */
#if TK_USE_DBGSPT && defined(KNL_CFG_USE_FUNC_TD_INF_TSK)
#define KNL_TSK_TCB_TIM_SIZE 8
#else
#define KNL_TSK_TCB_TIM_SIZE 0
#endif

/* TCB from .wtmeb to .issstack size, sizeof(TMEB) is 24 */
#define KNL_TSK_TCB_WTMEB2ISSTACK_SIZE                                                   \
                             ( 24 + KNL_TSK_TCB_POR_SIZE + KNL_TSK_TCB_MTX_SIZE          \
                               + KNL_TSK_TCB_TIM_SIZE )

/* TCB.isstack offset */
#define KNL_TSK_TCB_ISSTACK  ( KNL_TSK_TCB_WTMEB + KNL_TSK_TCB_WTMEB2ISSTACK_SIZE )

/* TCB.takctxb offset, sizeof(CTXB) is 4 */
#define KNL_TSK_TCB_CTXB     ( KNL_TSK_TCB_ISSTACK + 4 ) 

#ifndef Assembler

/*
 * Task dispatch disable state
 *  0 = DDS_ENABLE       : ENABLE
 *  1 = DDS_DISABLE_IMPLICIT : DISABLE with implicit process
 *  2 = DDS_DISABLE      : DISABLE with tk_dis_dsp()
 *  |   |
 *  |   use in *.c
 *  use in *.S
 *    --> Do NOT change these literals, because using in assembler code
 *
 *  'dispatch_disabled' records dispatch disable status set by tk_dis_dsp()
 *  for some CPU, that accepts delayed interrupt.
 *  In this case, you can NOT refer the dispatch disabled status
 *  only by 'dispatch_disabled'.
 *  Use 'in_ddsp()' to refer the task dispatch status.
 *  'in_ddsp()' is a macro definition in CPU-dependent definition files.
 */
#define KNL_TSK_DDS_ENABLE   (0)
#define KNL_TSK_DDS_DISABLE_IMPLICIT                                                     \
                             (1)                 /* set with implicit process */
#define KNL_TSK_DDS_DISABLE  (2)                 /* set by tk_dis_dsp() */

/*
 * Task priority internal/external expression conversion macro
 */
#define KNL_TSK_ETN_PRI(x)   ( (x) + TK_MIN_PRI )
#define KNL_TSK_ITN_PRI(x)   ( (x) - TK_MIN_PRI )

/*
 * Move to/Restore task independent part Macro and function
 */
#define KNL_TSK_ENTER_IDP    { knl_tsk_enteridp(); }
#define KNL_TSK_LEAVE_IDP    { knl_tsk_leaveidp(); }

/*
 * Get TCB from task ID.
 */
#define KNL_TSK_GET_CB(id)   ( &knl_tsk_cbtable[KNL_CFG_INDEX_TSK(id)] )
#define KNL_TSK_GET_CB_SELF(id)                                                          \
                             ( ( (id) == TSK_SELF ) ? knl_tsk_execution :                \
                                                      KNL_TSK_GET_CB(id) )

/*
 * Check system state Macro and function
 */

/* When a system call is called during dispatch disable, true
 * Also include the task independent part as during dispatch disable. */
#define KNL_TSK_IN_DDSP()    ( KNL_INT_IS_DISABLED || knl_tsk_dspdisabled  ||            \
                               KNL_TSK_IN_INDP() )

/* When a system call is called from the task independent part, true  */
#define KNL_TSK_IN_INDP()    ( ( knl_tsk_independent > 0 ) ||                            \
                               ( knl_tsk_execution == NULL ) )

/* When a system call is called during CPU lock (interrupt disable), true
 * Also include the task independent part as during CPU lock. */
#define KNL_TSK_IN_LOC()     ( KNL_INT_IS_DISABLED || KNL_TSK_IN_INDP() )

/* When a system call is called during executing the quasi task part, true
 * Valid only when KNL_TSK_IN_INDP() == false because it is not discriminated from
 * the task independent part. */
#define KNL_TSK_IN_QTSK()    ( knl_tsk_execution->sysmode > knl_tsk_execution->isysmode )

/*
 * Start/End critical section Macro
 */
#define KNL_TSK_BEGIN_CRITICAL_SECTION                                                   \
                             KNL_INT_BEGIN_DISABLE

#define KNL_TSK_END_CRITICAL_SECTION                                                     \
                             if ( KNL_INT_BETWEEN_DISABLE                                \
                                  && ( knl_tsk_execution != knl_tsk_schedule )           \
                                  && ( !( knl_tsk_independent > 0 ))                     \
                                  && ( !knl_tsk_dspdisabled ) ) {                        \
                                    knl_dsp_signal();                                    \
                             }                                                           \
                             KNL_INT_END_DISABLE


/*----------------------------------------------------------------------------------------
 *    Struct Define Section
 *--------------------------------------------------------------------------------------*/
 

/*
 * Task context block (CTXB)
 */
typedef struct {
        VP                   ssp;                /* Task stack pointer */
}                            CTXB;

/*
 * Generic part of control block (GCB)
 *  For synchronization between tasks and communication object, the head  
 *  part of control block is common. The followings are common routines.
 *  Define common part as GCB (generic control block) type.
 *  Cannot use these routines if an object has multiple wait queues
 *  and when it operates a wait queue after the first one.
 *  Cannot use these routines if TA_TPRI, TA_NODISWAI object attribute
 *  bits are used for other purposes since these bits are checked.
 */
typedef struct generic_control_block {
        QUEUE                wait_queue;         /* Wait queue */
        ID                   objid;              /* Object ID */
        VP                   exinf;              /* Extended information */
        ATR                  objatr;             /* Object attribute */
        /* It is OK to have another field after this point, */
        /* but it is not used for generic operation routines. */
}                            GCB ;


/*
 * Object lock, Locked task is the highest run priority, Unable to nest lock.
 */
typedef struct objlock {
        QUEUE                wtskq;              /* Wait task queue */
}                            OBJLOCK;

/*
 * Ready queue block (RDYQUE)
 *  In the ready queue, the task queue 'tskque' is provided per priority.
 *  The task TCB is registered onto queue with the applicable priority.
 *  For effective ready queue search, the bitmap area 'bitmap' is provided
 *  to indicate whether there are tasks in task queue per priority.
 *
 *  Also, to search a task at the highest priority in the ready queue
 *  effectively, put the highest task priority in the 'top_priority' field.
 *  If the ready queue is empty, set the value in this field to TK_NUM_PRI.
 *  In this case, to return '0' with refering 'tskque[top_priority]',
 *      there is 'null' field which is always '0'.
 *
 *  Multiple READY tasks with kernel lock do not exist at the same time.
 */
typedef struct ready_queue {
        PRI                  top_priority;       /* Highest priority in ready queue */
        QUEUE                tskque[TK_NUM_PRI]; /* Task queue per priority */
        TCB *                null;               /* When the ready queue is empty, */
        uint32_t             bitmap[KNL_TSK_BITMAP]; /* Bitmap area per priority */
        TCB *                klocktsk;           /* READY task with kernel lock */
}                            RDYQUE;


/*
 * Internal task state expression (TSTAT)
    '& TS_WAIT' can check whether in the wait state.
 */
typedef enum {
        TS_NONEXIST          = 0,                /* Unregistered state */
        TS_READY             = 1,                /* RUN or READY state */
        TS_WAIT              = 2,                /* WAIT state */
        TS_SUSPEND           = 4,                /* SUSPEND state */
        TS_WAITSUS           = 6,                /* Both WAIT and SUSPEND state */
        TS_DORMANT           = 8                 /* DORMANT state */
}                            TSTAT;


/*
 * Wait information in task control block (WINFO)
 *  Must be before TCB Definition,because used in TCB Definition
 */

/* Semaphore wait (TTW_SEM) */
typedef struct {
        int32_t              cnt;                /* Request resource number */
}                            WINFO_SEM;

/* Event flag wait (TTW_FLG) */
typedef struct {
        uint32_t             waiptn;             /* Wait bit pattern */
        uint32_t             wfmode;             /* Wait mode */
        uint32_t *           p_flgptn;           /* Address that has a bit pattern at */
                                                 /* wait released */
}                            WINFO_FLG;

/* Mailbox wait (TTW_MBX) */
typedef struct {
        T_MSG **             ppk_msg;            /* Address that has the head of a */
                                                 /* message packet */
}                            WINFO_MBX;

/* Message buffer receive/send wait (TTW_RMBF, TTW_SMBF) */
typedef struct {
        VP                   msg;                /* Address that has a received msg */
        int32_t *            p_msgsz;            /* Address that has a received msg size*/
}                            WINFO_RMBF;

typedef struct {
        VP                   msg;                /* Send message head address */
        int32_t              msgsz;              /* Send message size */
}                            WINFO_SMBF;

/* Rendezvous call/accept/end wait (TTW_CAL, TTW_ACP, TTW_RDV) */
typedef struct {
        uint32_t             calptn;             /* Bit pattern that indicates caller */
                                                 /* select condition */
        VP                   msg;                /* Address that has a message */
        int32_t              cmsgsz;             /* Call message size */
        int32_t *            p_rmsgsz;           /* Address that has a reply msg size */
}                            WINFO_CAL;

typedef struct {
        uint32_t             acpptn;             /* Bit pattern that indicates receiver */
                                                 /* select condition */
        VP                   msg;                /* Address that has a call message */
        RNO *                p_rdvno;            /* Address that has the rdv number */
        int32_t *            p_cmsgsz;           /* Address that has the call msg size */
}                            WINFO_ACP;

typedef struct {
        RNO                  rdvno;              /* Rendezvous number */
        VP                   msg;                /* Address that has a message */
        int32_t              maxrmsz;            /* Maximum length of reply message */
        int32_t *            p_rmsgsz;           /* Address that has a reply msg size */
}                            WINFO_RDV;

/* Variable size memory pool wait (TTW_MPL) */
typedef struct {
        int32_t              blksz;              /* Memory block size */
        VP *                 p_blk;              /* Address that has head of a mem block*/
}                            WINFO_MPL;

/* Fixed size memory pool wait (TTW_MPF) */
typedef struct {
        VP *                 p_blf;             /* Address that has head of a mem block*/
}                            WINFO_MPF;

/* Definition of wait information union */
typedef union wait_infor_block {
#if KNL_CFG_MAX_SEMID > 0
        WINFO_SEM            sem;
#endif
#if KNL_CFG_MAX_FLGID > 0
        WINFO_FLG            flg;
#endif
#if KNL_CFG_MAX_MBXID > 0
        WINFO_MBX            mbx;
#endif
#if KNL_CFG_MAX_MBFID > 0
        WINFO_RMBF           rmbf;
        WINFO_SMBF           smbf;
#endif
#if KNL_CFG_MAX_PORID > 0
        WINFO_CAL            cal;
        WINFO_ACP            acp;
        WINFO_RDV            rdv;
#endif
#if KNL_CFG_MAX_MPLID > 0
        WINFO_MPL            mpl;
#endif
#if KNL_CFG_MAX_MPFID > 0
        WINFO_MPF            mpf;
#endif
}                            WINFO;


/*
 * Task control block (TCB)
 */
#ifndef KNL_TSK_TCB_TYPEDEF
#define KNL_TSK_TCB_TYPEDEF
typedef struct task_control_block  TCB;
#endif

struct task_control_block {
        QUEUE                tskque;             /* Task queue */
        ID                   tskid;              /* Task ID */
        VP                   exinf;              /* Extended information */
        ATR                  tskatr;             /* Task attribute */
        FP                   task;               /* Task startup address */

        int32_t              sstksz;             /* stack size */

        int32_t              :0;                 /* ### From here */
        int8_t               isysmode;           /* Task oper mode initial value */
        int16_t              sysmode;            /* Task oper mode,quasitask call level */
        int32_t              :0;                 /* ###To here, since it might be accessed
                                                    from outside of the critical section,
                                                    need to be assigned as an independent
                                                    word. Also, there is a case where one
                                                    word is read from 'reqdct' and is read
                                                    all at once from 'reqdct', 'isysmode',
                                                    and 'sysmode', so do not change the
                                                    order and size. */

        PRI                  ipriority;          /* Priority at task startup */
        PRI                  bpriority;          /* Base priority */
        PRI                  priority;           /* Current priority */

        uint8_t              state;              /* TSTAT,Task state (Int. expression) */

        uint32_t             klockwait:1;        /* true at wait kernel lock */
        uint32_t             klocked:1;          /* true at hold kernel lock */

        WSPEC *              wspec;              /* Wait specification */
        ID                   wid;                /* Wait object ID */
        int32_t              wupcnt;             /* Number of wakeup requests queuing */
        int32_t              suscnt;             /* Number of SUSPEND request nests */
        ER *                 wercd;              /* Wait error code set area */
        WINFO                winfo;              /* Wait information */
        TMEB                 wtmeb;              /* Wait timer event block */

#if KNL_CFG_MAX_PORID > 0
        RNO                  wrdvno;             /* For creating rendezvous number */
#endif
#if KNL_CFG_MAX_MTXID > 0
        MTXCB *              mtxlist;            /* List of hold mutexes */
#endif

#if TK_USE_DBGSPT && defined(KNL_CFG_USE_FUNC_TD_INF_TSK)
        uint32_t             stime;              /* System execution time (ms) */
        uint32_t             utime;              /* User execution time (ms) */
#endif

        VP                   isstack;            /* stack pointer initial value */
        TK_ALIGN64                               /* alignment for CTXB.ssp */
        CTXB                 tskctxb;            /* Task context block */
#if TK_USE_OBJ_NAME                              /* name */
        uint8_t              name[TK_OBJ_NAME_LENGTH];
#endif
};


/*
 * Wait specification block (WSPEC)
 */
#ifndef KNL_TSK_WSPEC_TYPEDEF
#define KNL_TSK_WSPEC_TYPEDEF
typedef struct wait_spec_block      WSPEC;
#endif

struct wait_spec_block {
        uint32_t             tskwait;            /* Wait factor */
                                                 /* Process at task priority change */
        void                 (*chg_pri_hook)(TCB *tcb, int32_t priority);
                                                 /* Process at task wait release */
        void                 (*rel_wai_hook)(TCB *tcn); 
};


/*----------------------------------------------------------------------------------------
 *    Global Variable Declare Section
 *--------------------------------------------------------------------------------------*/


/* Task independent part flag */
extern int32_t  knl_tsk_independent;

/* Task mode flag */
extern uint32_t  knl_tsk_mode;

/* Task dispatcher state flag */
extern int32_t  knl_tsk_dspdisabled;

/*
 * Task in execution
 *  ctxtsk is a variable that indicates TCB task in execution
 *  (= the task that CPU holds context). During system call processing,
 *  when checking information about the task that requested system call,
 *  use 'ctxtsk'. Only task dispatcher changes 'ctxtsk'.
 */
extern TCB *  knl_tsk_execution;

/*
 * Task which should be executed
 *  'schedtsk' is a variable that indicates the task TCB to be executed.
 *  If a dispatch is delayed by the delayed dispatch or dispatch disable, 
 *  it does not match with 'ctxtsk.' 
 */
extern TCB *  knl_tsk_schedule;

extern RDYQUE  knl_tsk_rdyque;                   /* Ready queue */

/*
 * Task execution control information
 */
extern TCB  knl_tsk_cbtable[TK_NUM_TSKID];       /* Task control block */
extern QUEUE  knl_tsk_freecb;                    /* FreeQue */

/*----------------------------------------------------------------------------------------
 *    Function prototype Declare Section
 *--------------------------------------------------------------------------------------*/
 
/*
 *    Description   : Move to task independent part
 *    Param         : void
 *    Return Code   : void
 */
Inline void knl_tsk_enteridp( void )
{
    knl_tsk_independent++;
}

/*
 *    Description   : Check whether is in task-independent part
 *    Param         : void
 *    Return Code   : bool8_t : If task is in the task-independent part, true
 */
Inline bool8_t knl_tsk_isidp( void )
{
    int8_t  b;
    
    b = ( knl_tsk_independent > 0 )? true: false;
    return (bool8_t)b;
}

/*
 *    Description   : Leave from task independent part
 *    Param         : void
 *    Return Code   : void
 */
Inline void knl_tsk_leaveidp( void )
{
    knl_tsk_independent--;
}

/*
 *    Description   : When the task priority changes, adjust the task position at the 
 *                     wait queue. It is called only if the object attribute TA_TPRI 
 *                     is specified.
 *    Param         : GCB *gcb: generic control block
 *                    TCB *tcb: task control block
 *    Return Code   : void
 */
extern void knl_tsk_gcbchgpri( GCB *gcb, TCB *tcb );

/*
 *    Description   : Change the active task state to wait state and connect to the timer
 *                     wait queue and the object wait queue. Also set 'wid' in 
 *                     'knl_tsk_execution'.
 *    Param         : GCB *gcb: generic control block
 *                    TMO tmout: TMO tmout
 *    Return Code   : void
 */
extern void knl_tsk_gcbmakwai( GCB *gcb, TMO tmout );

/*
 *    Description   : Search the first task of wait queue include "tcb" with target.
 *                     (Not insert "tcb" into wait queue.)
 *    Param         : GCB *gcb: generic control block
 *                    TCB *tcb: task control block
 *    Return Code   : TCB *tcb: task control block
 */
extern TCB* knl_tsk_gcbwaiquetop( GCB *gcb, TCB *tcb );
/*
 *    Description   : Initilize object lock.
 *    Param         : OBJLOCK *loc: object lock
 *    Return Code   : void
 */
Inline void knl_tsk_initlock( OBJLOCK *loc )
{
    loc->wtskq.next = NULL;
}
/*
 *    Description   : Check object lock.
 *    Param         : OBJLOCK *loc: object lock
 *    Return Code   : bool8_t
 */
Inline bool8_t knl_tsk_islock( OBJLOCK *loc )
{
    int8_t  b;
    
    b = ( loc->wtskq.next != NULL )? true: false;
    return (bool8_t)b;
}

/*
 *    Description   : Object lock Do not call from critical section.
 *    Param         : OBJLOCK *loc: object lock
 *    Return Code   : void
 */
extern void knl_tsk_lock( OBJLOCK *loc );

/*
 *    Description   : Object unlock, may be called from a critical section.
 *    Param         : OBJLOCK *loc: object lock
 *    Return Code   : void
 */
extern void knl_tsk_unlock( OBJLOCK *loc );

/*
 *    Description   : If the task is alive ( except NON-EXISTENT,DORMANT ), return true.
 *    Param         : TSTAT state: task state
 *    Return Code   : bool8_t: true if task is alive
 */
Inline bool8_t knl_tsk_alive( TSTAT state )
{
    int8_t  b;

    b = ( (state & (TS_READY|TS_WAIT|TS_SUSPEND)) != 0u )? true: false;
    return (bool8_t)b;
}

/*
 *    Description   : Change task priority, Then make the required task state transition
 *                    occur.
 *    Param         : TCB *tcb: task control block
 *                    PRI priority: priority
 *    Return Code   : void
 */
extern void knl_tsk_chgpri( TCB *tcb, PRI priority );

/*
 *    Description   : Task deletion Call from critical section
 *    Param         : TCB *tcb: task control block
 *    Return Code   : void
 */
extern void knl_tsk_del( TCB *tcb );

/*
 *    Description   : Get object name from control block.
 *    Param         : ID id: task ID
 *                    uint8_t **name: point to dsname
 *    Return Code   : ER : Error code
 */
extern ER knl_tsk_getname(ID id, uint8_t **name);

/*
 *    Description   : Task module initialization
 *    Param         : void
 *    Return Code   : ER : Error code
 */
extern ER knl_tsk_init( void );

/*
 *    Description   : Prepare task execution.
 *    Param         : TCB *tcb: task control block
 *    Return Code   : void
 */
extern void knl_tsk_makdor( TCB *tcb );

/*
 *    Description   : Set task to non-executable state(wait state,forced wait,or dormant  
 *                    state).
 *                     Delete the task from the ready queue.
 *                     If the deleted task is 'knl_tsk_schedule', set 'knl_tsk_schedule' 
 *                     to the highest priority task in the ready queue. 
 *                     When calling this function, 'tcb' task must be READY.
 *    Param         : TCB *tcb: task control block
 *    Return Code   : void
 */
extern void knl_tsk_maknordy( TCB *tcb );

/*
 *    Description   : Set task to READY state for making task executable.
 *                     If the 'tcb' task priority is higher than the executed task,
 *                     make it executable. If the priority is lower, connect the task 
 *                     to the ready queue.
 *                     Update the task state and insert in the ready queue. If necessary, 
 *                     update 'knl_tsk_schedule' and request to start task dispatcher. 
 *    Param         : TCB *tcb: task control block
 *    Return Code   : void
 */
extern void knl_tsk_makrdy( TCB *tcb );

/*
 *    Description   : Update the task state to release wait. When it becomes ready 
 *                      state, connect to the ready queue.
 *                      Call when the task is in the wait state (including double wait).
 *    Param         : TCB *tcb: task control blcok
 *    Return Code   : void
 */
Inline void knl_tsk_maknowai( TCB *tcb )
{
    if ( tcb->state == TS_WAIT ) {
        knl_tsk_makrdy(tcb);
    } else {
        tcb->state = TS_SUSPEND;
    }
}

/*
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
extern void knl_tsk_makwai( TMO tmout, ATR atr );

/*
 *    Description   : Change the active task state to wait state and connect to the
 *                     timer event queue. Normally, 'knl_tsk_execution' is in the RUN state, 
 *                     but when an interrupt occurs during executing system call, 
 *                     'knl_tsk_execution' may become the other state by system call called 
 *                     in the interrupt handler.
 *                     However, it does not be in WAIT state.
 *    Param         : TMO tmout: c
 *                    ATR atr: attribute
 *    Return Code   : void
 */
extern void knl_tsk_makwaireltim( RELTIM tmout, ATR atr );

/*
 *    Description   : Delete task from ready queue
 *                     Take out TCB from the applicable priority task queue, and if the 
 *                     task queue becomes empty, clear the applicable bit from the bitmap 
 *                     area.In addition, update 'top_priority' if the deleted task had the 
 *                     highest priority. In such case, use the bitmap area to search the 
 *                     second highest priority task.
 *    Param         : RDYQUE *rq: ready queue
 *                    TCB *tcb: task control blcok
 *    Return Code   : void
 */
Inline void knl_tsk_rdyquedel( RDYQUE *rq, TCB *tcb )
{
    int32_t i;
    PRI priority = tcb->priority;

    if ( rq->klocktsk == tcb ) {
        rq->klocktsk = NULL;
    }

    knl_que_remove(&tcb->tskque);
    if ( tcb->klockwait ) {
        /* Delete from kernel lock wait queue */
        tcb->klockwait = (uint32_t)false;
        return;
    }
    if ( !knl_que_isempty(&rq->tskque[priority]) ) {
        return;
    }

    knl_bit_clr(rq->bitmap, (int32_t)priority);
    if ( priority != rq->top_priority ) {
        return;
    }

    i = knl_bit_search(rq->bitmap, (int32_t)priority, (int32_t)TK_NUM_PRI - (int32_t)priority);
    if ( i >= 0 ) {
        rq->top_priority = priority + (PRI)i;
    } else {
        rq->top_priority = TK_NUM_PRI;
    }
}

/*
 *    Description   : Ready queue initialization.
 *    Param         : RDYQUE *rq: ready queue
 *    Return Code   : void
 */
Inline void knl_tsk_rdyqueinit( RDYQUE *rq )
{
    int32_t i;

    rq->top_priority = TK_NUM_PRI;
    for ( i = 0; i < (int32_t)TK_NUM_PRI; i++ ) {
        knl_que_init(&rq->tskque[i]);
    }
    rq->null = NULL;
    rq->klocktsk = NULL;
    memset(rq->bitmap, 0, sizeof(rq->bitmap));
}

/*
 *    Description   : Insert task in ready queue
 *                     Insert it at the end of the same priority tasks with task priority
 *                     indicated with 'tcb'. Set the applicable bit in the bitmap area and
 *                     update 'top_priority' if necessary. When updating 'top_priority,'
 *                     return true, otherwise false.
 *    Param         : RDYQUE *rq: ready queue
 *                    TCB *tcb: task control blcok
 *    Return Code   : bool8_t: true if success
 */
Inline bool8_t knl_tsk_rdyqueins( RDYQUE *rq, TCB *tcb )
{
    PRI priority = tcb->priority;

    knl_que_insert(&tcb->tskque, &rq->tskque[priority]);
    knl_bit_set(rq->bitmap, (int32_t)priority);

    if ( tcb->klocked ) {
        rq->klocktsk = tcb;
    }

    if ( priority < rq->top_priority ) {
        rq->top_priority = priority;
        return true;
    }
    return false;
}

/*
 *    Description   : Connect the task to the prioritized queue.
 *    Param         : TCB *tcb: task control blcok
 *                    QUEUE *queue: queue
 *    Return Code   : void
 */
Inline void knl_tsk_queinspri( TCB *tcb, QUEUE *queue )
{
    QUEUE *q;
    QUEUE *qstart, *qend;
    uint8_t val;

    qstart = qend = queue;
    val = tcb->priority;

    for ( q = qstart->next; q != qend; q = q->next ) {
        if ( ( (TCB*)q )->priority  > val ) {
            break;
        }
    }

    knl_que_insert(&tcb->tskque, q);
}

/*
 *    Description   : Insert task at head in ready queue.
 *    Param         : RDYQUE *rq: ready queue
 *                    TCB *tcb: task control blcok
 *    Return Code   : void
 */
Inline void knl_tsk_rdyqueinstop( RDYQUE *rq, TCB *tcb )
{
    PRI priority = tcb->priority;

    knl_que_insert(&tcb->tskque, rq->tskque[priority].next);
    knl_bit_set(rq->bitmap, (int32_t)priority);

    if ( tcb->klocked ) {
        rq->klocktsk = tcb;
    }

    if ( priority < rq->top_priority ) {
        rq->top_priority = priority;
    }
}

/*
 *    Description   : Put 'tcb' to the end of ready queue.
 *    Param         : RDYQUE *rq: ready queue
 *                    TCB *tcb: task control blcok
 *    Return Code   : TCB*: task control blcok
 */
Inline TCB* knl_tsk_rdyquemovend( RDYQUE *rq, TCB *tcb )
{
    QUEUE  *tskque = &rq->tskque[tcb->priority];

    knl_que_remove(&tcb->tskque);
    knl_que_insert(&tcb->tskque, tskque);

    return (TCB*)tskque->next;                   /* New task at head of queue */
}

/*
 *    Description   : Move the task, whose ready queue priority is 'priority', at head of
 *                     queue to the end of queue. Do nothing, if the queue is empty.
 *    Param         : RDYQUE *rq: ready queue
 *                    PRI priority : priority
 *    Return Code   : void
 */
Inline void knl_tsk_rdyquerot( RDYQUE *rq, PRI priority )
{
    QUEUE *tskque = &rq->tskque[priority];
    TCB *tcb;

    tcb = (TCB*)knl_que_removenext(tskque);
    if ( tcb != NULL ) {
        knl_que_insert((QUEUE*)tcb, tskque);
    }
}

/*
 *    Description   : Rotate the priority ready queue at 'priority'.
 *    Param         : PRI priority: priority
 *    Return Code   : void
 */
extern void knl_tsk_rdyquerotrdy( PRI priority );

/*
 *    Description   : Rotate ready queue including the highest priority task in the 
 *                    ready queue.
 *    Param         : void
 *    Return Code   : void
 */
extern void knl_tsk_rdyquerotrun( void );

/*
 *    Description   : Return the highest priority task in ready queue.
 *    Param         : RDYQUE *rq: ready queue
 *    Return Code   : TCB*: task control block
 */
Inline TCB* knl_tsk_rdyquetop( RDYQUE *rq )
{
    /* If there is a task at kernel lock, that is the highest priority task */
    if ( rq->klocktsk != NULL ) {
        return rq->klocktsk;
    }

    return (TCB*)rq->tskque[rq->top_priority].next;
}

/*
 *    Description   : Return the priority of the highest priority task in the ready queue.
 *    Param         : const RDYQUE *rq: ready queue
 *    Return Code   : PRI: priority
 */
Inline PRI knl_tsk_rdyquetoppri( const RDYQUE *rq )
{
    return rq->top_priority;
}

/*
 *    Description   : Reselect task to execute
 *                     Set 'schedtsk' to the head task at the ready queue.
 *    Param         : void
 *    Return Code   : void
 */
Inline void knl_tsk_reschedule( void )
{
    TCB *toptsk;

    toptsk = knl_tsk_rdyquetop(&knl_tsk_rdyque);
    if ( knl_tsk_schedule != toptsk ) {
        knl_tsk_schedule = toptsk;
        knl_dsp_request();
    }
}

/*
 *    Description   : Task finalization Call from critical section
 *    Param         : TCB *tcb: task control block
 *    Return Code   : void
 */
extern void knl_tsk_ter( TCB *tcb );

/*
 *    Description   :  Cancel task wait state.
 *                      Remove the task from the tick queue and the wait queue.
 *                      Do not update the task state.
 *    Param         : TCB *tcb: task control blcok
 *    Return Code   : void
 */
Inline void knl_tsk_waican( TCB *tcb )
{
    knl_tck_delete(&tcb->wtmeb);
    knl_que_remove(&tcb->tskque);
}

/*
 *    Description   : Release all tasks connected to the wait queue, and define it
 *                     as E_DLT error. Use when synchronization between tasks or 
 *                     communication object is deleted.
 *    Param         : QUEUE *wait_queue: wait queue
 *    Return Code   : void
 */
extern void knl_tsk_waidel( QUEUE *wait_queue );

/*
 *    Description   : Get ID of the head task in the wait queue.
 *    Param         : QUEUE *wait_queue: wait queue
 *    Return Code   : ID: task ID
 */
extern ID knl_tsk_waitid( QUEUE *wait_queue );

/*
 *    Description   : Release wait state of the task.
 *    Param         : TCB *tcb: task control blcok
 *    Return Code   : void
 */
Inline void knl_tsk_wairel( TCB *tcb )
{
    knl_tck_delete(&tcb->wtmeb);
    knl_que_remove(&tcb->tskque);
    knl_tsk_maknowai(tcb);
}

/*
 *    Description   : Release wait state of the task.
 *                     Remove the task from the timer queue and the wait queue, then
 *                     update the task state to ercd.
 *    Param         : TCB *tcb: task control block
 *                    ER ercd: ER ercd
 *    Return Code   : void
 */
extern void knl_tsk_waireler( TCB *tcb, ER ercd );

/*
 *    Description   : Release wait state of the task.
 *                     Remove the task from the timer queue and the wait queue, then
 *                     update the task state to E_OK.
 *    Param         : TCB *tcb: task control block
 *    Return Code   : void
 */
extern void knl_tsk_wairelok( TCB *tcb );

/*
 *    Description   : Release wait state of the task.
 *                     Remove the task from the timer queue and the wait queue, then
 *                     update the task state to ercd(<0).
 *                     use for releasing the forced wait task. 
 *    Param         : TCB *tcb: task control block
 *                    ER ercd: ER ercd
 *    Return Code   : void
 */
extern void knl_tsk_wairelng( TCB *tcb, ER ercd );

/*
 *    Description   : Release wait state of the task.
 *                     don't remove from the timer queue. Use for time out processing.
 *    Param         : TCB *tcb: task control block
 *    Return Code   : void
 */
extern void knl_tsk_waireltmo( TCB *tcb );

#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* _KNL_TSK_H__ */
