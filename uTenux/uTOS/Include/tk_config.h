/*
 *----------------------------------------------------------------------------------------
 *    Config
 *
 *    Copyright (C) 2008-2017 by Dalian uLoong Co.,Ltd. All rights reserved.
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
 *    File Name      : tk_config.h
 *    Create Date    : 2012/7/9-2017/11/12
 *    Author         : WangShb-Wangshb
 *    Description    : System General Configuration Definition.
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_CONFIG_H__
#define __TK_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Check parameter
 *   0: Do not check parameter
 *   1: Check parameter
 */
#define KNL_CFG_CHK_NOSPT         1              /* Check unsupported function(E_NOSPT) */
#define KNL_CFG_CHK_RSATR         1              /* Check reserv attrib error(E_RSATR) */
#define KNL_CFG_CHK_PAR           1              /* Check parameter (E_PAR) */
#define KNL_CFG_CHK_ID            1              /* Check object ID range (E_ID) */
#define KNL_CFG_CHK_OACV          1              /* Check Object Access Violat (E_OACV) */
#define KNL_CFG_CHK_CTX           1              /* Check if task-indep part run(E_CTX) */
#define KNL_CFG_CHK_DISDSP        1              /* Check dispatch disable part */
#define KNL_CFG_CHK_ININDP        1              /* Check task independent part */
#define KNL_CFG_CHK_SELF          1              /* Check if self task specified(E_OBJ) */

/*
 *  Kernel version defintion
 */
#define KNL_CFG_VER_MAKER         0x554CU        /* uT/OS Maker Code     */
#define KNL_CFG_VER_PRID          0x7554U        /* uT/OS Product Code   */
#define KNL_CFG_VER_SPVER         0x6101U        /* Specfication Version */
#define KNL_CFG_VER_PRVER         0x3231U        /* uT/OS Main Version   */
#define KNL_CFG_VER_PRNO1         0x3030U        /* uT/OS Sub Version    */
#define KNL_CFG_VER_PRNO2         0x3030U        /* uT/OS Build Number   */
#define KNL_CFG_VER_PRNO3         0x3034U        /* uT/OS Build Number   */
#define KNL_CFG_VER_PRNO4         0x3830U        /* uT/OS Build Number   */

/*
 * System Limits Configuration Definition.
 */

/* Task configuration */
#define TK_MIN_TSKID              1
#define TK_MAX_TSKID              (KNL_CFG_MAX_TSKID)
#define TK_NUM_TSKID              (TK_MAX_TSKID)
#define KNL_CFG_CHK_TSKID(id)     ( ( (TK_MIN_TSKID) <= (id) ) &&                        \
                                    ( (id) <= (TK_MAX_TSKID) ) )
#define KNL_CFG_INDEX_TSK(id)     ( (id) - (TK_MIN_TSKID) )
#define KNL_CFG_ID_TSK(index)     ( (index) + (TK_MIN_TSKID) )

/* Semaphore configuration */
#define TK_MIN_SEMID              1
#define TK_MAX_SEMID              (KNL_CFG_MAX_SEMID)
#define TK_NUM_SEMID              (TK_MAX_SEMID)
#define KNL_CFG_CHK_SEMID(id)     ( ( (TK_MIN_SEMID) <= (id) ) &&                        \
                                    ( (id) <= (TK_MAX_SEMID) ) )
#define KNL_CFG_INDEX_SEM(id)     ( (id) - (TK_MIN_SEMID) )
#define KNL_CFG_ID_SEM(index)     ( (index) + (TK_MIN_SEMID) )

/* Event flag configuration */
#define TK_MIN_FLGID              1
#define TK_MAX_FLGID              (KNL_CFG_MAX_FLGID)
#define TK_NUM_FLGID              (TK_MAX_FLGID)
#define KNL_CFG_CHK_FLGID(id)     ( ( (TK_MIN_FLGID) <= (id) ) &&                        \
                                    ( (id) <= (TK_MAX_FLGID) ) )
#define KNL_CFG_INDEX_FLG(id)     ( (id) - (TK_MIN_FLGID) )
#define KNL_CFG_ID_FLG(index)     ( (index) + (TK_MIN_FLGID) )

/* Mailbox configuration */
#define TK_MIN_MBXID              1
#define TK_MAX_MBXID              (KNL_CFG_MAX_MBXID)
#define TK_NUM_MBXID              (TK_MAX_MBXID)
#define KNL_CFG_CHK_MBXID(id)     ( ( (TK_MIN_MBXID) <= (id) ) &&                        \
                                    ( (id) <= (TK_MAX_MBXID) ) )
#define KNL_CFG_INDEX_MBX(id)     ( (id) - (TK_MIN_MBXID) )
#define KNL_CFG_ID_MBX(index)     ( (index) + (TK_MIN_MBXID) )

/* Mutex configuration */
#define TK_MIN_MTXID              1
#define TK_MAX_MTXID              (KNL_CFG_MAX_MTXID)
#define TK_NUM_MTXID              (TK_MAX_MTXID)
#define KNL_CFG_CHK_MTXID(id)     ( ( (TK_MIN_MTXID) <= (id) ) &&                        \
                                    ( (id) <= (TK_MAX_MTXID) ) )
#define KNL_CFG_INDEX_MTX(id)     ( (id) - (TK_MIN_MTXID) )
#define KNL_CFG_ID_MTX(index)     ( (index) + (TK_MIN_MTXID) )

/* Message buffer configuration */
#define TK_MIN_MBFID              1
#define TK_MAX_MBFID              (KNL_CFG_MAX_MBFID)
#define TK_NUM_MBFID              (TK_MAX_MBFID)
#define KNL_CFG_CHK_MBFID(id)     ( ( (TK_MIN_MBFID) <= (id) ) &&                        \
                                    ( (id) <= (TK_MAX_MBFID) ) )
#define KNL_CFG_INDEX_MBF(id)     ( (id) - (TK_MIN_MBFID) )
#define KNL_CFG_ID_MBF(index)     ( (index) + (TK_MIN_MBFID) )

/* Rendezvous port configuration */
#define TK_MIN_PORID              1
#define TK_MAX_PORID              (KNL_CFG_MAX_PORID)
#define TK_NUM_PORID              (TK_MAX_PORID)
#define KNL_CFG_CHK_PORID(id)     ( ( (TK_MIN_PORID) <= (id) ) &&                        \
                                    ( (id) <= (TK_MAX_PORID) ) )
#define KNL_CFG_INDEX_POR(id)     ( (id) - (TK_MIN_PORID) )
#define KNL_CFG_ID_POR(index)     ( (index) + (TK_MIN_PORID) )

/* Variable size Memory pool configuration */
#define TK_MIN_MPLID              1
#define TK_MAX_MPLID              (KNL_CFG_MAX_MPLID)
#define TK_NUM_MPLID              (TK_MAX_MPLID)
#define KNL_CFG_CHK_MPLID(id)     ( ( (TK_MIN_MPLID) <= (id) ) &&                        \
                                    ( (id) <= (TK_MAX_MPLID) ) )
#define KNL_CFG_INDEX_MPL(id)     ( (id) - (TK_MIN_MPLID) )
#define KNL_CFG_ID_MPL(index)     ( (index) + (TK_MIN_MPLID) )

/* Fixed size memory pool configuration */
#define TK_MIN_MPFID              1
#define TK_MAX_MPFID              (KNL_CFG_MAX_MPFID)
#define TK_NUM_MPFID              (TK_MAX_MPFID)
#define KNL_CFG_CHK_MPFID(id)     ( ( (TK_MIN_MPFID) <= (id) ) &&                        \
                                    ( (id) <= (TK_MAX_MPFID) ) )
#define KNL_CFG_INDEX_MPF(id)     ( (id) - (TK_MIN_MPFID) )
#define KNL_CFG_ID_MPF(index)     ( (index) + (TK_MIN_MPFID) )

/* Cyclic handler configuration */
#define TK_MIN_CYCID              1
#define TK_MAX_CYCID              (KNL_CFG_MAX_CYCID)
#define TK_NUM_CYCID              (TK_MAX_CYCID)
#define KNL_CFG_CHK_CYCID(id)     ( ( (TK_MIN_CYCID) <= (id) ) &&                        \
                                    ( (id) <= (TK_MAX_CYCID) ) )
#define KNL_CFG_INDEX_CYC(id)     ( (id) - (TK_MIN_CYCID) )
#define KNL_CFG_ID_CYC(index)     ( (index) + (TK_MIN_CYCID) )

/* Alarm handler configuration */
#define TK_MIN_ALMID              1
#define TK_MAX_ALMID              (KNL_CFG_MAX_ALMID)
#define TK_NUM_ALMID              (TK_MAX_ALMID)
#define KNL_CFG_CHK_ALMID(id)     ( ( (TK_MIN_ALMID) <= (id) ) &&                        \
                                    ( (id) <= (TK_MAX_ALMID) ) )
#define KNL_CFG_INDEX_ALM(id)     ( (id) - (TK_MIN_ALMID) )
#define KNL_CFG_ID_ALM(index)     ( (index) + (TK_MIN_ALMID) )

/* Task priority configuration */
#define TK_MIN_PRI                1U             /* Min num = highest priority */
#define TK_MAX_PRI                (KNL_CFG_MAX_PRI)  /* Max num = lowest priority */
#define TK_NUM_PRI                (TK_MAX_PRI)   /* Number of priority levels */
#define KNL_CFG_CHK_PRI(pri)      ( ((TK_MIN_PRI) <= (pri)) && ((pri) <= (TK_MAX_PRI)) )

#include <tk_config_depend.h>

/* 
 * System function Configuration Definition.
 */

/* System normal calls */
#define KNL_CFG_USE_FUNC_TK_CRE_TSK
#define KNL_CFG_USE_FUNC_TK_DEL_TSK
#define KNL_CFG_USE_FUNC_TK_STA_TSK
#define KNL_CFG_USE_FUNC_TK_EXT_TSK
#define KNL_CFG_USE_FUNC_TK_EXD_TSK
#define KNL_CFG_USE_FUNC_TK_TER_TSK
#define KNL_CFG_USE_FUNC_TK_DIS_DSP
#define KNL_CFG_USE_FUNC_TK_ENA_DSP
#define KNL_CFG_USE_FUNC_TK_CHG_PRI
#define KNL_CFG_USE_FUNC_TK_ROT_RDQ
#define KNL_CFG_USE_FUNC_TK_REL_WAI
#define KNL_CFG_USE_FUNC_TK_GET_TID
#define KNL_CFG_USE_FUNC_TK_GET_REG
#define KNL_CFG_USE_FUNC_TK_SET_REG
#define KNL_CFG_USE_FUNC_TK_REF_TSK
#define KNL_CFG_USE_FUNC_TK_SUS_TSK
#define KNL_CFG_USE_FUNC_TK_RSM_TSK
#define KNL_CFG_USE_FUNC_TK_FRSM_TSK
#define KNL_CFG_USE_FUNC_TK_SLP_TSK
#define KNL_CFG_USE_FUNC_TK_WUP_TSK
#define KNL_CFG_USE_FUNC_TK_CAN_WUP
#define KNL_CFG_USE_FUNC_TK_DLY_TSK

#if KNL_CFG_MAX_SEMID > 0
#define KNL_CFG_USE_FUNC_KNL_SEM_CBTABLE
#define KNL_CFG_USE_FUNC_KNL_SEM_INIT
#define KNL_CFG_USE_FUNC_TK_CRE_SEM
#define KNL_CFG_USE_FUNC_TK_DEL_SEM
#define KNL_CFG_USE_FUNC_TK_SIG_SEM
#define KNL_CFG_USE_FUNC_TK_WAI_SEM
#define KNL_CFG_USE_FUNC_TK_REF_SEM
#if TK_USE_DBGSPT
#define KNL_CFG_USE_FUNC_KNL_SEM_GETNAME
#define KNL_CFG_USE_FUNC_TD_LST_SEM
#define KNL_CFG_USE_FUNC_TD_REF_SEM
#define KNL_CFG_USE_FUNC_TD_SEM_QUE
#endif
#endif

#if KNL_CFG_MAX_MTXID > 0
#define KNL_CFG_USE_FUNC_KNL_MTX_CBTABLE
#define KNL_CFG_USE_FUNC_KNL_MTX_INIT
#define KNL_CFG_USE_FUNC_KNL_MTX_LIMPRI
#define KNL_CFG_USE_FUNC_KNL_MTX_RELEASE
#define KNL_CFG_USE_FUNC_KNL_MTX_SIGALL
#define KNL_CFG_USE_FUNC_TK_CRE_MTX
#define KNL_CFG_USE_FUNC_TK_DEL_MTX
#define KNL_CFG_USE_FUNC_TK_LOC_MTX
#define KNL_CFG_USE_FUNC_TK_UNL_MTX
#define KNL_CFG_USE_FUNC_TK_REF_MTX
#if TK_USE_DBGSPT
#define KNL_CFG_USE_FUNC_KNL_MTX_GETNAME
#define KNL_CFG_USE_FUNC_TD_LST_MTX
#define KNL_CFG_USE_FUNC_TD_REF_MTX
#define KNL_CFG_USE_FUNC_TD_MTX_QUE
#endif
#endif

#if KNL_CFG_MAX_FLGID > 0
#define KNL_CFG_USE_FUNC_KNL_FLG_CBTABLE
#define KNL_CFG_USE_FUNC_KNL_FLG_INIT
#define KNL_CFG_USE_FUNC_TK_CRE_FLG
#define KNL_CFG_USE_FUNC_TK_DEL_FLG
#define KNL_CFG_USE_FUNC_TK_SET_FLG
#define KNL_CFG_USE_FUNC_TK_CLR_FLG
#define KNL_CFG_USE_FUNC_TK_WAI_FLG
#define KNL_CFG_USE_FUNC_TK_REF_FLG
#if TK_USE_DBGSPT
#define KNL_CFG_USE_FUNC_KNL_FLG_GETNAME
#define KNL_CFG_USE_FUNC_TD_LST_FLG
#define KNL_CFG_USE_FUNC_TD_REF_FLG
#define KNL_CFG_USE_FUNC_TD_FLG_QUE
#endif
#endif

#if KNL_CFG_MAX_MBXID > 0
#define KNL_CFG_USE_FUNC_KNL_MBX_CBTABLE
#define KNL_CFG_USE_FUNC_KNL_MBX_INIT
#define KNL_CFG_USE_FUNC_TK_CRE_MBX
#define KNL_CFG_USE_FUNC_TK_DEL_MBX
#define KNL_CFG_USE_FUNC_TK_SND_MBX
#define KNL_CFG_USE_FUNC_TK_RCV_MBX
#define KNL_CFG_USE_FUNC_TK_REF_MBX
#if TK_USE_DBGSPT
#define KNL_CFG_USE_FUNC_KNL_MBX_GETNAME
#define KNL_CFG_USE_FUNC_TD_LST_MBX
#define KNL_CFG_USE_FUNC_TD_REF_MBX
#define KNL_CFG_USE_FUNC_TD_MBX_QUE
#endif
#endif

#if KNL_CFG_MAX_MBFID > 0
#define KNL_CFG_USE_FUNC_KNL_MBF_CBTABLE
#define KNL_CFG_USE_FUNC_KNL_MBF_INIT
#define KNL_CFG_USE_FUNC_KNL_MBF_SNDMSG
#define KNL_CFG_USE_FUNC_KNL_MBF_WAKEUP
#define KNL_CFG_USE_FUNC_TK_CRE_MBF
#define KNL_CFG_USE_FUNC_TK_DEL_MBF
#define KNL_CFG_USE_FUNC_TK_SND_MBF
#define KNL_CFG_USE_FUNC_TK_RCV_MBF
#define KNL_CFG_USE_FUNC_TK_REF_MBF
#if TK_USE_DBGSPT
#define KNL_CFG_USE_FUNC_KNL_MBF_GETNAME
#define KNL_CFG_USE_FUNC_TD_LST_MBF
#define KNL_CFG_USE_FUNC_TD_REF_MBF
#define KNL_CFG_USE_FUNC_TD_SMBF_QUE
#define KNL_CFG_USE_FUNC_TD_RMBF_QUE
#endif
#endif

#if KNL_CFG_MAX_PORID > 0
#define KNL_CFG_USE_FUNC_KNL_POR_CBTABLE
#define KNL_CFG_USE_FUNC_KNL_POR_INIT
#define KNL_CFG_USE_FUNC_KNL_POR_CAL
#define KNL_CFG_USE_FUNC_KNL_POR_RDV
#define KNL_CFG_USE_FUNC_TK_CRE_POR
#define KNL_CFG_USE_FUNC_TK_DEL_POR
#define KNL_CFG_USE_FUNC_TK_CAL_POR
#define KNL_CFG_USE_FUNC_TK_ACP_POR
#define KNL_CFG_USE_FUNC_TK_FWD_POR
#define KNL_CFG_USE_FUNC_TK_RPL_RDV
#define KNL_CFG_USE_FUNC_TK_REF_POR
#if TK_USE_DBGSPT
#define KNL_CFG_USE_FUNC_KNL_POR_GETNAME
#define KNL_CFG_USE_FUNC_TD_LST_POR
#define KNL_CFG_USE_FUNC_TD_REF_POR
#define KNL_CFG_USE_FUNC_TD_CAL_QUE
#define KNL_CFG_USE_FUNC_TD_ACP_QUE
#endif
#endif

#if TK_USE_INT_DEFHDR
#define KNL_CFG_USE_FUNC_TK_DEF_INT
#if TK_USE_INT_HLLHDR
#define KNL_CFG_USE_FUNC_KNL_INT_HLLHDR
#endif
#endif
#define KNL_CFG_USE_FUNC_TK_RET_INT

#if KNL_CFG_MAX_MPLID > 0
#define KNL_CFG_USE_FUNC_KNL_MPL_CBTABLE
#define KNL_CFG_USE_FUNC_KNL_MPL_INIT
#define KNL_CFG_USE_FUNC_KNL_MPL_WAKEUP
#define KNL_CFG_USE_FUNC_TK_CRE_MPL
#define KNL_CFG_USE_FUNC_TK_DEL_MPL
#define KNL_CFG_USE_FUNC_TK_GET_MPL
#define KNL_CFG_USE_FUNC_TK_REL_MPL
#define KNL_CFG_USE_FUNC_TK_REF_MPL
#if TK_USE_DBGSPT
#define KNL_CFG_USE_FUNC_KNL_MPL_GETNAME
#define KNL_CFG_USE_FUNC_TD_LST_MPL
#define KNL_CFG_USE_FUNC_TD_REF_MPL
#define KNL_CFG_USE_FUNC_TD_MPL_QUE
#endif
#endif

#if KNL_CFG_MAX_MPFID > 0
#define KNL_CFG_USE_FUNC_KNL_MPF_CBTABLE
#define KNL_CFG_USE_FUNC_KNL_MPF_INIT
#define KNL_CFG_USE_FUNC_TK_CRE_MPF
#define KNL_CFG_USE_FUNC_TK_DEL_MPF
#define KNL_CFG_USE_FUNC_TK_GET_MPF
#define KNL_CFG_USE_FUNC_TK_REL_MPF
#define KNL_CFG_USE_FUNC_TK_REF_MPF
#if TK_USE_DBGSPT
#define KNL_CFG_USE_FUNC_KNL_MPF_GETNAME
#define KNL_CFG_USE_FUNC_TD_LST_MPF
#define KNL_CFG_USE_FUNC_TD_REF_MPF
#define KNL_CFG_USE_FUNC_TD_MPF_QUE
#endif
#endif

#define KNL_CFG_USE_FUNC_TK_GET_TIM
#define KNL_CFG_USE_FUNC_TK_GET_ETM
#define KNL_CFG_USE_FUNC_TK_GET_OTM
#define KNL_CFG_USE_FUNC_TK_SET_TIM

#if KNL_CFG_MAX_CYCID > 0
#define KNL_CFG_USE_FUNC_KNL_CYC_CBTABLE
#define KNL_CFG_USE_FUNC_KNL_CYC_CALLHDR
#define KNL_CFG_USE_FUNC_KNL_CYC_INIT
#define KNL_CFG_USE_FUNC_TK_CRE_CYC
#define KNL_CFG_USE_FUNC_TK_DEL_CYC
#define KNL_CFG_USE_FUNC_TK_STA_CYC
#define KNL_CFG_USE_FUNC_TK_STP_CYC
#define KNL_CFG_USE_FUNC_TK_REF_CYC
#if TK_USE_DBGSPT
#define KNL_CFG_USE_FUNC_KNL_CYC_GETNAME
#define KNL_CFG_USE_FUNC_TD_LST_CYC
#define KNL_CFG_USE_FUNC_TD_REF_CYC
#endif
#endif

#if KNL_CFG_MAX_ALMID > 0
#define KNL_CFG_USE_FUNC_KNL_ALM_CBTABLE
#define KNL_CFG_USE_FUNC_KNL_ALM_CALLHDR
#define KNL_CFG_USE_FUNC_KNL_ALM_INIT
#define KNL_CFG_USE_FUNC_TK_CRE_ALM
#define KNL_CFG_USE_FUNC_TK_DEL_ALM
#define KNL_CFG_USE_FUNC_TK_STA_ALM
#define KNL_CFG_USE_FUNC_TK_STP_ALM
#define KNL_CFG_USE_FUNC_TK_REF_ALM

#if TK_USE_DBGSPT
#define KNL_CFG_USE_FUNC_KNL_ALM_GETNAME
#define KNL_CFG_USE_FUNC_TD_LST_ALM
#define KNL_CFG_USE_FUNC_TD_REF_ALM
#endif
#endif

#define KNL_CFG_USE_FUNC_TK_REF_VER
#define KNL_CFG_USE_FUNC_TK_REF_SYS
#define KNL_CFG_USE_FUNC_TK_SET_POW
#define KNL_CFG_USE_FUNC_POW_DISCNT

#if TK_USE_DBGSPT
/* Refer each object usage state */
#define KNL_CFG_USE_FUNC_KNL_TSK_GETNAME
#define KNL_CFG_USE_FUNC_TD_LST_TSK
#define KNL_CFG_USE_FUNC_TD_REF_TSK
#define KNL_CFG_USE_FUNC_TD_INF_TSK
#define KNL_CFG_USE_FUNC_TD_GET_REG
#define KNL_CFG_USE_FUNC_TD_SET_REG

/* Refer system state */
#define KNL_CFG_USE_FUNC_TD_REF_SYS
#define KNL_CFG_USE_FUNC_TD_GET_TIM
#define KNL_CFG_USE_FUNC_TD_GET_OTM

/* Refer ready queue */
#define KNL_CFG_USE_FUNC_TD_RDY_QUE

/* Execution trace */
#if TK_USE_HOOK_TRACE
#define KNL_CFG_USE_FUNC_KNL_HOK_DSPEXECFP
#define KNL_CFG_USE_FUNC_KNL_HOK_SVCENTERFP
#define KNL_CFG_USE_FUNC_KNL_HOK_INTENTERFP
#define KNL_CFG_USE_FUNC_TD_HOK_SVC
#define KNL_CFG_USE_FUNC_TD_HOK_DSP
#if TK_USE_INT_DEFHDR
#define KNL_CFG_USE_FUNC_TD_HOK_INT
#endif
#endif

/* Object name */
#define KNL_CFG_USE_FUNC_KNL_SYS_GETNAME
#define KNL_CFG_USE_FUNC_TD_REF_DSNAME
#define KNL_CFG_USE_FUNC_TD_SET_DSNAME

#endif /* TK_USE_DBGSPT */

/* other functions */
#define KNL_CFG_USE_FUNC_KNL_TSK_WSPEC_SLP

/* task concerning */
#define KNL_CFG_USE_FUNC_CTXTSK
#define KNL_CFG_USE_FUNC_KNL_TSK_CBTABLE
#define KNL_CFG_USE_FUNC_KNL_TSK_INIT
#define KNL_CFG_USE_FUNC_KNL_TSK_MAKDOR
#define KNL_CFG_USE_FUNC_KNL_TSK_MAKRDY
#define KNL_CFG_USE_FUNC_KNL_TSK_MAKNORDY
#define KNL_CFG_USE_FUNC_KNL_TSK_CHGPRI
#define KNL_CFG_USE_FUNC_KNL_TSK_RDYQUEROTRDY
#define KNL_CFG_USE_FUNC_KNL_TSK_RUNRDYQUEROT
#define KNL_CFG_USE_FUNC_KNL_TSK_DEL
#define KNL_CFG_USE_FUNC_KNL_TSK_TER

/* wait concerning */
#define KNL_CFG_USE_FUNC_KNL_TSK_OKWAIREL
#define KNL_CFG_USE_FUNC_KNL_TSK_ERWAIREL
#define KNL_CFG_USE_FUNC_KNL_TSK_NGWAIREL
#define KNL_CFG_USE_FUNC_KNL_TSK_TMOWAIREL
#define KNL_CFG_USE_FUNC_KNL_TSK_MAKWAI
#define KNL_CFG_USE_FUNC_KNL_TSK_MAKWAIRELTIM
#define KNL_CFG_USE_FUNC_KNL_TSK_WAIDEL
#define KNL_CFG_USE_FUNC_KNL_TSK_WAITID
#define KNL_CFG_USE_FUNC_KNL_TSK_GCBMAKWAI
#define KNL_CFG_USE_FUNC_KNL_TSK_GCBCHGPRI
#define KNL_CFG_USE_FUNC_KNL_TSK_GCBWAIQUETOP

/* task register */
#define KNL_CFG_USE_FUNC_KNL_CTX_SETREG
#define KNL_CFG_USE_FUNC_KNL_CTX_GETREG

/* memory concerning */
#define KNL_CFG_USE_FUNC_KNL_MPL_APPENDFREEAREABOUND
#define KNL_CFG_USE_FUNC_KNL_MPL_GETBLK
#define KNL_CFG_USE_FUNC_KNL_MPL_RELBLK
#define KNL_CFG_USE_FUNC_KNL_MEM_SEARCHFREEAREA
#define KNL_CFG_USE_FUNC_KNL_MEM_APPENDFREEAREA
#define KNL_CFG_USE_FUNC_KNL_MEM_REMOVEFREEQUE
#define KNL_CFG_USE_FUNC_KNL_MEM_INSERTAREAQUE
#define KNL_CFG_USE_FUNC_KNL_MEM_AREAQUEREMOVE
#if TK_USE_MALLOC
#define KNL_CFG_USE_FUNC_KNL_MEM_IMACB
#define KNL_CFG_USE_FUNC_KNL_MEM_IMALLOC
#define KNL_CFG_USE_FUNC_KNL_MEM_ICALLOC
#define KNL_CFG_USE_FUNC_KNL_MEM_IFREE
#define KNL_CFG_USE_FUNC_KNL_MEM_INIT
#endif /* TK_USE_MALLOC */

#if !(TK_USE_DBGSPT && TK_USE_HOOK_TRACE)
/*
 * system call proxy for direct call.
 */
#define tk_cre_tsk_impl           tk_cre_tsk
#define tk_del_tsk_impl           tk_del_tsk
#define tk_sta_tsk_impl           tk_sta_tsk
#define tk_ext_tsk_impl           tk_ext_tsk
#define tk_exd_tsk_impl           tk_exd_tsk
#define tk_ter_tsk_impl           tk_ter_tsk
#define tk_dis_dsp_impl           tk_dis_dsp
#define tk_ena_dsp_impl           tk_ena_dsp
#define tk_chg_pri_impl           tk_chg_pri
#define tk_rot_rdq_impl           tk_rot_rdq
#define tk_rel_wai_impl           tk_rel_wai
#define tk_get_tid_impl           tk_get_tid
#define tk_get_reg_impl           tk_get_reg
#define tk_set_reg_impl           tk_set_reg
#define tk_ref_tsk_impl           tk_ref_tsk
#define tk_sus_tsk_impl           tk_sus_tsk
#define tk_rsm_tsk_impl           tk_rsm_tsk
#define tk_frsm_tsk_impl          tk_frsm_tsk
#define tk_slp_tsk_impl           tk_slp_tsk
#define tk_wup_tsk_impl           tk_wup_tsk
#define tk_can_wup_impl           tk_can_wup
#define tk_cre_sem_impl           tk_cre_sem
#define tk_del_sem_impl           tk_del_sem
#define tk_sig_sem_impl           tk_sig_sem
#define tk_wai_sem_impl           tk_wai_sem
#define tk_ref_sem_impl           tk_ref_sem
#define tk_cre_mtx_impl           tk_cre_mtx
#define tk_del_mtx_impl           tk_del_mtx
#define tk_loc_mtx_impl           tk_loc_mtx
#define tk_unl_mtx_impl           tk_unl_mtx
#define tk_ref_mtx_impl           tk_ref_mtx
#define tk_cre_flg_impl           tk_cre_flg
#define tk_del_flg_impl           tk_del_flg
#define tk_set_flg_impl           tk_set_flg
#define tk_clr_flg_impl           tk_clr_flg
#define tk_wai_flg_impl           tk_wai_flg
#define tk_ref_flg_impl           tk_ref_flg
#define tk_cre_mbx_impl           tk_cre_mbx
#define tk_del_mbx_impl           tk_del_mbx
#define tk_snd_mbx_impl           tk_snd_mbx
#define tk_rcv_mbx_impl           tk_rcv_mbx
#define tk_ref_mbx_impl           tk_ref_mbx
#define tk_cre_mbf_impl           tk_cre_mbf
#define tk_del_mbf_impl           tk_del_mbf
#define tk_snd_mbf_impl           tk_snd_mbf
#define tk_rcv_mbf_impl           tk_rcv_mbf
#define tk_ref_mbf_impl           tk_ref_mbf
#define tk_cre_por_impl           tk_cre_por
#define tk_del_por_impl           tk_del_por
#define tk_cal_por_impl           tk_cal_por
#define tk_acp_por_impl           tk_acp_por
#define tk_fwd_por_impl           tk_fwd_por
#define tk_rpl_rdv_impl           tk_rpl_rdv
#define tk_ref_por_impl           tk_ref_por
#define tk_def_int_impl           tk_def_int
#define tk_ret_int_impl           tk_ret_int
#define tk_cre_mpl_impl           tk_cre_mpl
#define tk_del_mpl_impl           tk_del_mpl
#define tk_get_mpl_impl           tk_get_mpl
#define tk_rel_mpl_impl           tk_rel_mpl
#define tk_ref_mpl_impl           tk_ref_mpl
#define tk_cre_mpf_impl           tk_cre_mpf
#define tk_del_mpf_impl           tk_del_mpf
#define tk_get_mpf_impl           tk_get_mpf
#define tk_rel_mpf_impl           tk_rel_mpf
#define tk_ref_mpf_impl           tk_ref_mpf
#define tk_set_tim_impl           tk_set_tim
#define tk_get_tim_impl           tk_get_tim
#define tk_get_etm_impl           tk_get_etm
#define tk_get_otm_impl           tk_get_otm
#define tk_dly_tsk_impl           tk_dly_tsk
#define tk_cre_cyc_impl           tk_cre_cyc
#define tk_del_cyc_impl           tk_del_cyc
#define tk_sta_cyc_impl           tk_sta_cyc
#define tk_stp_cyc_impl           tk_stp_cyc
#define tk_ref_cyc_impl           tk_ref_cyc
#define tk_cre_alm_impl           tk_cre_alm
#define tk_del_alm_impl           tk_del_alm
#define tk_sta_alm_impl           tk_sta_alm
#define tk_stp_alm_impl           tk_stp_alm
#define tk_ref_alm_impl           tk_ref_alm
#define tk_ref_ver_impl           tk_ref_ver
#define tk_ref_sys_impl           tk_ref_sys
#define tk_set_pow_impl           tk_set_pow

#define td_lst_tsk_impl           td_lst_tsk
#define td_lst_sem_impl           td_lst_sem
#define td_lst_flg_impl           td_lst_flg
#define td_lst_mbx_impl           td_lst_mbx
#define td_lst_mtx_impl           td_lst_mtx
#define td_lst_mbf_impl           td_lst_mbf
#define td_lst_por_impl           td_lst_por
#define td_lst_mpf_impl           td_lst_mpf
#define td_lst_mpl_impl           td_lst_mpl
#define td_lst_cyc_impl           td_lst_cyc
#define td_lst_alm_impl           td_lst_alm
#define td_ref_sem_impl           td_ref_sem
#define td_ref_flg_impl           td_ref_flg
#define td_ref_mbx_impl           td_ref_mbx
#define td_ref_mtx_impl           td_ref_mtx
#define td_ref_mbf_impl           td_ref_mbf
#define td_ref_por_impl           td_ref_por
#define td_ref_mpf_impl           td_ref_mpf
#define td_ref_mpl_impl           td_ref_mpl
#define td_ref_cyc_impl           td_ref_cyc
#define td_ref_alm_impl           td_ref_alm
#define td_ref_tsk_impl           td_ref_tsk
#define td_inf_tsk_impl           td_inf_tsk
#define td_get_reg_impl           td_get_reg
#define td_set_reg_impl           td_set_reg
#define td_ref_sys_impl           td_ref_sys
#define td_get_tim_impl           td_get_tim
#define td_get_otm_impl           td_get_otm
#define td_rdy_que_impl           td_rdy_que
#define td_sem_que_impl           td_sem_que
#define td_flg_que_impl           td_flg_que
#define td_mbx_que_impl           td_mbx_que
#define td_mtx_que_impl           td_mtx_que
#define td_smbf_que_impl          td_smbf_que
#define td_rmbf_que_impl          td_rmbf_que
#define td_cal_que_impl           td_cal_que
#define td_acp_que_impl           td_acp_que
#define td_mpf_que_impl           td_mpf_que
#define td_mpl_que_impl           td_mpl_que
#define td_hok_svc_impl           td_hok_svc
#define td_hok_dsp_impl           td_hok_dsp
#define td_hok_int_impl           td_hok_int
#define td_ref_dsname_impl        td_ref_dsname
#define td_set_dsname_impl        td_set_dsname

#endif /* !(TK_USE_DBGSPT && TK_USE_HOOK_TRACE) */

#ifdef __cplusplus
}
#endif

#endif /* __TK_CONFIG_H__ */
