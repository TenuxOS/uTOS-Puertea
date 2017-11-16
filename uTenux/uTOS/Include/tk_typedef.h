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
 *    File Name      : tk_typedef.h
 *    Create Date    : 2013/2/16-2014/5/1
 *    Author         : WangShb
 *    Description    : uT/OS Standard data type definitions.
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_TYPEDEF_H__
#define __TK_TYPEDEF_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Common creation used for Task, Semaphore, Mail box, Mutex, Message buffer, Rendezvous,
 * Fixed size memory pool, Variable size memory pool, Cycle, Alarm
 * */
#define TA_NULL              0U                  /* No special attributes indicated */
  
#define TA_ASM               0x00000000UL        /* Program by assembler */
#define TA_HLNG              0x00000001UL        /* Program by high level language */

#define TA_USERBUF           0x00000020UL        /* Specify user buffer */
#define TA_DSNAME            0x00000040UL        /* Use object name */

#define TA_RNG0              0x00000000UL        /* Execute by protection level 0 */
#define TA_RNG1              0x00000100UL        /* Execute by protection level 1 */
#define TA_RNG2              0x00000200UL        /* Execute by protection level 2 */
#define TA_RNG3              0x00000300UL        /* Execute by protection level 3 */

#define TA_TFIFO             0x00000000UL        /* Manage wait task by FIFO */
#define TA_TPRI              0x00000001UL        /* Manage wait task by priority order */

/* Time out */
#define TMO_POL              0                   /* Polling */
#define TMO_FEVR             (-1)                /* Permanent wait */

/* Task creation */
#define TSK_SELF             0                   /* Its own task specify */
#define TPRI_INI             0U                  /* Specify priority at task startup */
#define TPRI_RUN             0U                  /* Specify highest priority during run */

/* Task state tskstat */
#define TTS_RUN              0x00000001U         /* RUN */
#define TTS_RDY              0x00000002U         /* READY */
#define TTS_WAI              0x00000004U         /* WAIT */
#define TTS_SUS              0x00000008U         /* SUSPEND */
#define TTS_WAS              0x0000000cU         /* WAIT-SUSPEND */
#define TTS_DMT              0x00000010U         /* DORMANT */
#define TTS_NODISWAI         0x00000080U         /* Wait disable rejection state */

/* Wait factor tskwait */
#define TTW_SLP              0x00000001UL        /* Wait caused by wakeup wait */
#define TTW_DLY              0x00000002UL        /* Wait caused by task delay */
#define TTW_SEM              0x00000004UL        /* Semaphore wait */
#define TTW_FLG              0x00000008UL        /* Event flag wait */
#define TTW_MBX              0x00000040UL        /* Mail box wait */
#define TTW_MTX              0x00000080UL        /* Mutex wait */
#define TTW_SMBF             0x00000100UL        /* Message buffer send wait */
#define TTW_RMBF             0x00000200UL        /* Message buffer receive wait */
#define TTW_CAL              0x00000400UL        /* Rendezvous call wait */
#define TTW_ACP              0x00000800UL        /* Rendezvous accept wait */
#define TTW_RDV              0x00001000UL        /* Rendezvous end wait */
#define TTW_MPF              0x00002000UL        /* Fixed size memory pool wait */
#define TTW_MPL              0x00004000UL        /* Variable size memory pool wait */

/* Semaphore creation */
#define TA_FIRST             0x00000000UL        /* Give pri to task at head waitqueue */
#define TA_CNT               0x00000002UL        /* Give pri to task request counts less*/

/* Event flag */
#define TA_WSGL              0x00000000UL        /* Disable multiple tasks wait */
#define TA_WMUL              0x00000008UL        /* Enable multiple tasks wait */

/* Event flag wait mode */
#define TWF_ANDW             0x00000000U         /* AND wait */
#define TWF_ORW              0x00000001U         /* OR wait */
#define TWF_CLR              0x00000010U         /* All clear specify */
#define TWF_BITCLR           0x00000020U         /* Only condition bit clear specify */

/* Mail box creation */
#define TA_MFIFO             0x00000000UL        /* Manage messages by FIFO */
#define TA_MPRI              0x00000002UL        /* Manage messages by priority order */

/* Mutex creation */
#define TA_INHERIT           0x00000002UL        /* Priority inherited protocol */
#define TA_CEILING           0x00000003UL        /* Upper limit priority protocol */

/* Cycle handler creation */
#define TA_STA               0x00000002UL        /* Cycle handler startup */
#define TA_PHS               0x00000004UL        /* Save cycle handler phase */
#define TA_DSNAME            0x00000040UL        /* Use object name */

#define TCYC_STP             0x00U               /* Cycle handler is not operating */
#define TCYC_STA             0x01U               /* Cycle handler is operating */

/* Alarm handler creation */
#define TALM_STP             0x00U               /* Alarm handler is not operating */
#define TALM_STA             0x01U               /* Alarm handler is operating */

/* System state */
#define TSS_TSK              0x00U               /* During execution of task part(ctx) */
#define TSS_DDSP             0x01U               /* During dispatch disable */
#define TSS_DINT             0x02U               /* During Interrupt disable */
#define TSS_INDP             0x04U               /* During execution of task indep part */
#define TSS_QTSK             0x08U               /* During execution of semi-task part */

/* Power-saving mode */
#define TPW_DOSUSPEND        0x01U               /* Transit to suspend state */
#define TPW_DISLOWPOW        0x02U               /* Power-saving mode switch disable */
#define TPW_ENALOWPOW        0x03U               /* Power-saving mode switch enable */

/*
 * Object name information                       td_ref_dsname, td_set_dsname
 */
#define TN_TSK               0x01U
#define TN_SEM               0x02U
#define TN_FLG               0x03U
#define TN_MBX               0x04U
#define TN_MBF               0x05U
#define TN_POR               0x06U
#define TN_MTX               0x07U
#define TN_MPL               0x08U
#define TN_MPF               0x09U
#define TN_CYC               0x0aU
#define TN_ALM               0x0bU

#ifndef Assembler

/*
 * C99 standard data type definition extension
 */
#ifndef __bool8_t_defined
typedef bool                 bool8_t;
#define __bool8_t_defined    1
#endif

/*
 * Common data type definition
 */
#define SYSCALL                                  /* System call Definition */

typedef int32_t              ER;                 /* Error code */
typedef int32_t              FN;                 /* Function code */
typedef int32_t              ID;                 /* ID general */
typedef void *               VP;                 /* Nonuniform type data pointer */
typedef uint32_t             ATR;                /* Object/handler attribute */
typedef uint8_t              PRI;                /* Priority */
typedef int32_t              RNO;                /* Rendezvous number */
typedef int32_t              TMO;                /* Time out setting */
typedef int32_t              MSEC;               /* Time general (millisecond) */
typedef void                 (*FP)();            /* Function address general */
typedef int32_t              (*FUNCP)();         /* Function address general */
typedef uint32_t             INTVEC;             /* Interrupt vector index number */
typedef uint32_t             RELTIM;             /* Relative time */

typedef struct systim {                          /* System time */
        int32_t              hi;                 /* Upper 32 bits */
        uint32_t             lo;                 /* Lower 32 bits */
}                            SYSTIM;

/*
 * Task creation information                     tk_cre_tsk
 */
typedef struct t_ctsk {
        VP                   exinf;              /* Extended information */
        ATR                  tskatr;             /* Task attribute */
        FP                   task;               /* Task startup address */
        PRI                  itskpri;            /* Priority at task startup */
        int32_t              stksz;              /* User stack size (byte) */
        uint8_t              dsname[8];          /* Object name */
        VP                   bufptr;             /* User buffer */
}                            T_CTSK;

/*
 * Task state information                        tk_ref_tsk
 */
typedef struct t_rtsk {
        VP                   exinf;              /* Extended information */
        PRI                  tskpri;             /* Current priority */
        PRI                  tskbpri;            /* Base priority */
        uint32_t             tskstat;            /* Task state */
        uint32_t             tskwait;            /* Wait factor */
        ID                   wid;                /* Wait object ID */
        int32_t              wupcnt;             /* Number of wakeup requests queuing */
        int32_t              suscnt;             /* Number of SUSPEND request nests */
}                            T_RTSK;

/*
 * Task statistics information                   td_inf_tsk
 */
typedef struct td_itsk {
        RELTIM               stime;              /* Cumulative system exec time(ms) */
        RELTIM               utime;              /* Cumulative user execution time(ms) */
}                            TD_ITSK;

/*
 * Task state information                        td_ref_tsk
 */
typedef struct td_rtsk {
        VP                   exinf;              /* Extended information */
        PRI                  tskpri;             /* Current priority */
        PRI                  tskbpri;            /* Base priority */
        uint32_t             tskstat;            /* Task state */
        uint32_t             tskwait;            /* Wait factor */
        ID                   wid;                /* Wait object ID */
        int32_t              wupcnt;             /* Number of wakeup requests queuing */
        int32_t              suscnt;             /* Number of SUSPEND request nests */
        FP                   task;               /* Task startup address */
        int32_t              stksz;              /* stack size (byte) */
        VP                   istack;             /* stack pointer initial value */
}                            TD_RTSK;

/*
 * Semaphore creation information                tk_cre_sem
 */
typedef struct t_csem {
        VP                   exinf;              /* Extended information */
        ATR                  sematr;             /* Semaphore attribute */
        int32_t              isemcnt;            /* Semaphore initial count value */
        int32_t              maxsem;             /* Semaphore maximum count value */
        uint8_t              dsname[8];          /* Object name */
}                            T_CSEM;

/*
 * Semaphore state information                   tk_ref_sem
 */
typedef struct t_rsem {
        VP                   exinf;              /* Extended information */
        ID                   wtsk;               /* Wait task ID */
        int32_t              semcnt;             /* Current semaphore value */
}                            T_RSEM;

/*
 * Semaphore state information                   td_ref_sem
 */
typedef struct td_rsem {
        VP                   exinf;              /* Extended information */
        ID                   wtsk;               /* Wait task ID */
        int32_t              semcnt;             /* Current semaphore value */
}                            TD_RSEM;

/*
 * Event flag creation information               tk_cre_flg
 */
typedef struct t_cflg {
        VP                   exinf;              /* Extended information */
        ATR                  flgatr;             /* Event flag attribute */
        uint32_t             iflgptn;            /* Event flag initial value */
        uint8_t              dsname[8];          /* Object name */
}                            T_CFLG;

/*
 * Event flag state information                  tk_ref_flg
 */
typedef struct t_rflg {
        VP                   exinf;              /* Extended information */
        ID                   wtsk;               /* Wait task ID */
        uint32_t             flgptn;             /* Current event flag pattern */
}                            T_RFLG;

/*
 * Event flag state information                  td_ref_flg
 */
typedef struct td_rflg {
        VP                   exinf;              /* Extended information */
        ID                   wtsk;               /* Wait task ID */
        uint32_t             flgptn;             /* Current event flag pattern */
}                            TD_RFLG;

/*
 * Mail box creation information                 tk_cre_mbx
 */
typedef struct t_cmbx {
        VP                   exinf;              /* Extended information */
        ATR                  mbxatr;             /* Mail box attribute */
        uint8_t              dsname[8];          /* Object name */
}                            T_CMBX;

/*
 * Mail box message header
 */
typedef struct t_msg {
        VP                   msgque[1];          /* Area for message queue */
}                            T_MSG;

typedef struct t_msg_pri {
        T_MSG                msgque;             /* Area for message queue */
        PRI                  msgpri;             /* Message priority */
}                            T_MSG_PRI;

/*
 * Mail box state information                     tk_ref_mbx
 */
typedef struct t_rmbx {
        VP                   exinf;              /* Extended information */
        ID                   wtsk;               /* Wait task ID */
        T_MSG *              pk_msg;             /* Next received message */
}                            T_RMBX;

/*
 * Mail box state information                    td_ref_mbx
 */
typedef struct td_rmbx {
        VP                   exinf;              /* Extended information */
        ID                   wtsk;               /* Wait task ID */
        T_MSG *              pk_msg;             /* Next received message */
}                            TD_RMBX;

/*
 * Mutex creation information                    tk_cre_mtx
 */
typedef struct t_cmtx {
        VP                   exinf;              /* Extended information */
        ATR                  mtxatr;             /* Mutex attribute */
        PRI                  ceilpri;            /* Upper limit priority of mutex */
        uint8_t              dsname[8];          /* Object name */
}                            T_CMTX;

/*
 * Mutex state information                       tk_ref_mtx
 */
typedef struct t_rmtx {
        VP                   exinf;              /* Extended information */
        ID                   htsk;               /* Locking task ID */
        ID                   wtsk;               /* Lock wait task ID */
}                            T_RMTX;

/*
 * Mutex state information                       td_ref_mtx
 */
typedef struct td_rmtx {
        VP                   exinf;              /* Extended information */
        ID                   htsk;               /* Locking task ID */
        ID                   wtsk;               /* Lock wait task ID */
}                            TD_RMTX;

/*
 * Message buffer creation information           tk_cre_mbf
 */
typedef struct t_cmbf {
        VP                   exinf;              /* Extended information */
        ATR                  mbfatr;             /* Message buffer attribute */
        int32_t              bufsz;              /* Message buffer size (byte) */
        int32_t              maxmsz;             /* Maximum length of message (byte) */
        uint8_t              dsname[8];          /* Object name */
        VP                   bufptr;             /* User buffer */
}                            T_CMBF;

/*
 * Message buffer state information              tk_ref_mbf
 */
typedef struct t_rmbf {
        VP                   exinf;              /* Extended information */
        ID                   wtsk;               /* Receive wait task ID */
        ID                   stsk;               /* Send wait task ID */
        int32_t              msgsz;              /* Next received message size (byte) */
        int32_t              frbufsz;            /* Free buffer size (byte) */
        int32_t              maxmsz;             /* Maximum length of message (byte) */
}                            T_RMBF;

/*
 * Message buffer state information              td_ref_mbf
 */
typedef struct td_rmbf {
        VP                   exinf;              /* Extended information */
        ID                   wtsk;               /* Receive wait task ID */
        ID                   stsk;               /* Send wait task ID */
        int32_t              msgsz;              /* Next received message size (byte) */
        int32_t              frbufsz;            /* Free buffer size (byte) */
        int32_t              maxmsz;             /* Maximum length of message (byte) */
}                            TD_RMBF;

/*
 * Rendezvous port creation information          tk_cre_por
 */
typedef struct t_cpor {
        VP                   exinf;              /* Extended information */
        ATR                  poratr;             /* Port attribute */
        int32_t              maxcmsz;            /* Maximum len of call message(byte) */
        int32_t              maxrmsz;            /* Maximum len of replay message(byte) */
        uint8_t              dsname[8];          /* Object name */
}                            T_CPOR;

/*
 * Rendezvous port state information             tk_ref_por
 */
typedef struct t_rpor {
        VP                   exinf;              /* Extended information */
        ID                   wtsk;               /* Call wait task ID */
        ID                   atsk;               /* Receive wait task ID */
        int32_t              maxcmsz;            /* Maximum len of call message(byte) */
        int32_t              maxrmsz;            /* Maximum len of replay message(byte) */
}                            T_RPOR;


/*
 * Rendezvous port state information             td_ref_por
 */
typedef struct td_rpor {
        VP                   exinf;              /* Extended information */
        ID                   wtsk;               /* Call wait task ID */
        ID                   atsk;               /* Receive wait task ID */
        int32_t              maxcmsz;            /* Max length of call message (byte) */
        int32_t              maxrmsz;            /* Max length of replay message (byte) */
}                            TD_RPOR;

/*
 * Fixed size memory pool state information      tk_cre_mpf
 */
typedef struct t_cmpf {
        VP                   exinf;              /* Extended information */
        ATR                  mpfatr;             /* Memory pool attribute */
        int32_t              mpfcnt;             /* Num of blocks in whole memory pool */
        int32_t              blfsz;              /* Fixed size memory block size (byte) */
        uint8_t              dsname[8];          /* Object name */
        VP                   bufptr;             /* User buffer */
}                            T_CMPF;

/*
 * Fixed size memory pool state information      tk_ref_mpf
 */
typedef struct t_rmpf {
        VP                   exinf;              /* Extended information */
        ID                   wtsk;               /* Wait task ID */
        int32_t              frbcnt;             /* Number of free area blocks */
}                            T_RMPF;

/*
 * Fixed size memory pool state information      td_ref_mpf
 */
typedef struct td_rmpf {
        VP                   exinf;              /* Extended information */
        ID                   wtsk;               /* Wait task ID */
        int32_t              frbcnt;             /* Number of free blocks */
}                            TD_RMPF;

/*
 * Variable size memory pool create information  tk_cre_mpl
 */
typedef struct t_cmpl {
        VP                   exinf;              /* Extended information */
        ATR                  mplatr;             /* Memory pool attribute */
        int32_t              mplsz;              /* Size of whole memory pool (byte) */
        uint8_t              dsname[8];          /* Object name */
        VP                   bufptr;             /* User buffer */
}                            T_CMPL;

/*
 * Variable size memory pool state information   tk_ref_mpl
 */
typedef struct t_rmpl {
        VP                   exinf;              /* Extended information */
        ID                   wtsk;               /* Wait task ID */
        int32_t              frsz;               /* Total size of free area (byte) */
        int32_t              maxsz;              /* Size of max continu free area(byte) */
}                            T_RMPL;

/*
 * Variable size memory pool state information   td_ref_mpl
 */
typedef struct td_rmpl {
        VP                   exinf;              /* Extended information */
        ID                   wtsk;               /* Wait task ID */
        int32_t              frsz;               /* Total size of free area (byte) */
        int32_t              maxsz;              /* Size of max continu free area(byte) */
}                            TD_RMPL;

/*
 * Cycle handler creation information            tk_cre_cyc
 */
typedef struct t_ccyc {
        VP                   exinf;              /* Extended information */
        ATR                  cycatr;             /* Cycle handler attribute */
        FP                   cychdr;             /* Cycle handler address */
        RELTIM               cyctim;             /* Cycle interval */
        RELTIM               cycphs;             /* Cycle phase */
        uint8_t              dsname[8];          /* Object name */
}                            T_CCYC;

/*
 * Cycle handler state information               tk_ref_cyc
 */
typedef struct t_rcyc {
        VP                   exinf;              /* Extended information */
        RELTIM               lfttim;             /* Remain time until next handler start*/
        uint32_t             cycstat;            /* Cycle handler status */
}                            T_RCYC;

/*
 * Cycle handler state information               td_ref_cyc
 */
typedef struct td_rcyc {
        VP                   exinf;              /* Extended information */
        RELTIM               lfttim;             /* Remain time until next handler start*/
        uint32_t             cycstat;            /* Cycle handler status */
}                            TD_RCYC;

/*
 * Alarm handler creation information            tk_cre_alm
 */
typedef struct t_calm {
        VP                   exinf;              /* Extended information */
        ATR                  almatr;             /* Alarm handler attribute */
        FP                   almhdr;             /* Alarm handler address */
        uint8_t              dsname[8];          /* Object name */
}                            T_CALM;

/*
 * Alarm handler state information               tk_ref_alm
 */
typedef struct t_ralm {
        VP                   exinf;              /* Extended information */
        RELTIM               lfttim;             /* Remaining time until handler startup*/
        uint32_t             almstat;            /* Alarm handler state */
}                            T_RALM;

/*
 * Alarm handler state information               td_ref_alm
 */
typedef struct td_ralm {
        VP                   exinf;              /* Extended information */
        RELTIM               lfttim;             /* Remaining time until handler startup*/
        uint32_t             almstat;            /* Alarm handler status */
}                            TD_RALM;

/*
 * Interrupt handler definition information      tk_def_int
 */
typedef struct t_dint {
        ATR                  intatr;             /* Interrupt handler attribute */
        FP                   inthdr;             /* Interrupt handler address */
}                            T_DINT;

/*
 * System state information                      tk_ref_sys
 */
typedef struct t_rsys {
        uint32_t             sysstat;            /* System state */
        ID                   runtskid;           /* ID of task in execution state */
        ID                   schedtskid;         /* ID of task that be the exec state */
}                            T_RSYS;

/*
 * Version information                           tk_ref_ver
 */
typedef struct t_rver {
        uint16_t             maker;              /* OS manufacturer */
        uint16_t             prid;               /* OS identification number */
        uint16_t             spver;              /* Specification version */
        uint16_t             prver;              /* OS product version */
        uint16_t             prno[4];            /* Product num, Product management inf */
}                            T_RVER;

/*
 * System state information                      td_ref_sys
 */
typedef struct td_rsys {
        uint32_t             sysstat;            /* System state */
        ID                   runtskid;           /* ID of task in execution state */
        ID                   schedtskid;         /* ID of task be in execution state */
}                            TD_RSYS;

/*
 * Task dispatch trace definition                td_hok_dsp
 */
typedef struct td_hdsp {
        FP                   exec;               /* Hook routine when starting execution*/
        FP                   stop;               /* Hook routine when stopping execution*/
}                            TD_HDSP;

/*
 * Exception/Interrupt trace definition          td_hok_int
 */
typedef struct td_hint {
        FP                   enter;              /* Hook routine before calling handler */
        FP                   leave;              /* Hook routine after calling handler */
}                            TD_HINT;

/*
 * System call/extended SVC trace definition     td_hok_svc
 */
typedef struct td_hsvc {
        FP                   enter;              /* Hook routine before calling */
        FP                   leave;              /* Hook routine after calling */
}                            TD_HSVC;

#endif /* Assembler */

#include <tk_typedef_depend.h>

#ifdef __cplusplus
}
#endif

#endif /* __TK_TYPEDEF_H__ */
