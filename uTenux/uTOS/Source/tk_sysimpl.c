/*
 *----------------------------------------------------------------------------------------
 *    Kernel/sys
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
 *    File Name      : tk_sysimpl.c
 *    Create Date    : 2012/7/9-2014/5/1
 *    Author         : WangShb
 *    Description    : uT/OS System Management Function.
 *----------------------------------------------------------------------------------------
 */

#include "tk_almimpl.h"
#include "tk_cycimpl.h"
#include "tk_flgimpl.h"
#include "tk_mbfimpl.h"
#include "tk_mbximpl.h"
#include "tk_mpfimpl.h"
#include "tk_mplimpl.h"
#include "tk_mtximpl.h"
#include "tk_porimpl.h"
#include "tk_semimpl.h"
#include "tk_tskimpl.h"
#include "tk_sysimpl.h"

/* User Boot define message */
#if TK_USE_MESSAGE
static const uint8_t knl_sys_bootmsg[] = { KNL_CFG_BOOT_MESSAGE };
#endif

#if TK_USE_OBJ_NAME
#ifdef KNL_CFG_USE_FUNC_KNL_SYS_GETNAME
/*
 *    Function Name : knl_sys_getname
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Get DS object name
 *    Param         : uint32_t type : object type
 *                    ID id : object ID
 *                    uint8_t *dsname : address to return DS object name
 *    Return Code   : ER : Error code
 */
ER knl_sys_getname( uint32_t objtype, ID objid, uint8_t **name)
{
    ER  ercd;

    switch (objtype) {
#if KNL_CFG_MAX_TSKID > 0
        case TN_TSK:
            ercd = knl_tsk_getname(objid, name);
            break;
#endif

#if KNL_CFG_MAX_SEMID > 0
        case TN_SEM:
            ercd = knl_sem_getname(objid, name);
            break;
#endif

#if KNL_CFG_MAX_FLGID > 0
        case TN_FLG:
            ercd = knl_flg_getname(objid, name);
            break;
#endif

#if KNL_CFG_MAX_MBXID > 0
        case TN_MBX:
            ercd = knl_mbx_getname(objid, name);
            break;
#endif

#if KNL_CFG_MAX_MBFID > 0
        case TN_MBF:
            ercd = knl_mbf_getname(objid, name);
            break;
#endif

#if KNL_CFG_MAX_PORID > 0
        case TN_POR:
            ercd = knl_por_getname(objid, name);
            break;
#endif

#if KNL_CFG_MAX_MTXID > 0
        case TN_MTX:
            ercd = knl_mtx_getname(objid, name);
            break;
#endif

#if KNL_CFG_MAX_MPLID > 0
        case TN_MPL:
            ercd = knl_mpl_getname(objid, name);
            break;
#endif

#if KNL_CFG_MAX_MPFID > 0
        case TN_MPF:
            ercd = knl_mpf_getname(objid, name);
            break;
#endif

#if KNL_CFG_MAX_CYCID > 0
        case TN_CYC:
            ercd = knl_cyc_getname(objid, name);
            break;
#endif

#if KNL_CFG_MAX_ALMID > 0
        case TN_ALM:
            ercd = knl_alm_getname(objid, name);
            break;
#endif

        default:
            ercd = E_PAR;
            break;

    }

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_KNL_SYS_GETNAME */
#endif /* TK_USE_OBJ_NAME */

#ifdef KNL_CFG_USE_FUNC_TK_DIS_DSP
/*
 *    Function Name : tk_dis_dsp_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Dispatch disable
 *    Param         : void
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_dis_dsp_impl( void )
{
    KNL_CHK_CTX(!KNL_TSK_IN_LOC())

    knl_tsk_dspdisabled = KNL_TSK_DDS_DISABLE;

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_TK_DIS_DSP */

#ifdef KNL_CFG_USE_FUNC_TK_ENA_DSP
/*
 *    Function Name : tk_ena_dsp_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Enable Dispatch
 *    Param         : void
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_ena_dsp_impl( void )
{
    KNL_CHK_CTX(!KNL_TSK_IN_LOC())

    knl_tsk_dspdisabled = KNL_TSK_DDS_ENABLE;
    if ( knl_tsk_execution != knl_tsk_schedule ) {
        knl_dsp_signal();
    }

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_TK_ENA_DSP */

/*
 *    Function Name : tk_ext_sys
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Output shutdown message, Execute finalization and stop or dead loop
 *    Param         : void
 *    Return Code   : void
 */
SYSCALL void tk_ext_sys( void )
{
    uint32_t  imask = 0u;

    imask = imask;
#if KNL_CFG_USE_CLEANUP
    /* Shutdown message output */
#if TK_USE_MESSAGE
    tm_putstring((uint8_t*)&"\n<< SYSTEM SHUTDOWN >>\n");
#endif
    knl_tck_shutdown();
    knl_cpu_shutdown();
    /* No return */
#else
    DI(imask);
    tm_monitor();
#endif /* KNL_CFG_USE_CLEANUP */
}

#ifdef KNL_CFG_USE_FUNC_TK_GET_TID
/*
 *    Function Name : tk_get_tid_impl
 *    Create Date   : 2012/7/9-2013/4/20
 *    Author        : wangshb
 *    Description   : Get Task Identifier
 *    Param         : void
 *    Return Code   : ID : ID of the task in RUN state
 */
SYSCALL ID tk_get_tid_impl( void )
{
    return ( knl_tsk_execution == NULL )? 0: knl_tsk_execution->tskid;
}
#endif /* KNL_CFG_USE_FUNC_TK_GET_TID */

#ifdef KNL_CFG_USE_FUNC_TK_REF_SYS
/*
 *    Function Name : tk_ref_sys_impl
 *    Create Date   : 2012/7/9-2013/4/20
 *    Author        : wangshb
 *    Description   : Reference System Status
 *    Param         : T_RSYS *pk_rsys : Address of packet for returning status information
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_ref_sys_impl( T_RSYS *pk_rsys )
{
    if ( KNL_TSK_IN_INDP() ) {
        pk_rsys->sysstat = TSS_INDP;
    } else {
        if ( KNL_TSK_IN_QTSK() ) {
            pk_rsys->sysstat = TSS_QTSK;
        } else {
            pk_rsys->sysstat = TSS_TSK;
        }
        if ( KNL_TSK_IN_LOC() ) {
            pk_rsys->sysstat |= TSS_DINT;
        }
        if ( KNL_TSK_IN_DDSP() ) {
            pk_rsys->sysstat |= TSS_DDSP;
        }
    }
    pk_rsys->runtskid = ( knl_tsk_execution != NULL )? knl_tsk_execution->tskid: 0;
    pk_rsys->schedtskid = ( knl_tsk_schedule != NULL )? knl_tsk_schedule->tskid: 0;

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_TK_REF_SYS */

#ifdef KNL_CFG_USE_FUNC_TK_REF_VER
/*
 *    Function Name : tk_ref_ver_impl
 *    Create Date   : 2012/7/9-2014/2/1
 *    Author        : wangshb
 *    Description   : Refer version information,If there is no kernel version information,
 *                    If there is no kernel version information,set 0 in each information.
 *                    (Do NOT cause errors.)
 *    Param         : T_RVER *pk_rver : Start address of packet for version information
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_ref_ver_impl( T_RVER *pk_rver )
{
    /* uT/OS Maker Code */
    pk_rver->maker   = (uint16_t)KNL_CFG_VER_MAKER;  
    /* uT/OS Product Code */
    pk_rver->prid    = (uint16_t)KNL_CFG_VER_PRID;   
    /* Specification version */
    pk_rver->spver   = (uint16_t)KNL_CFG_VER_SPVER;  
    /* uT/OS Main Version */
    pk_rver->prver   = (uint16_t)KNL_CFG_VER_PRVER;  
    /* uT/OS Sub Version */
    pk_rver->prno[0] = (uint16_t)KNL_CFG_VER_PRNO1;  
    /* uT/OS Build Number */
    pk_rver->prno[1] = (uint16_t)KNL_CFG_VER_PRNO2;  
    /* uT/OS Build Number */
    pk_rver->prno[2] = (uint16_t)KNL_CFG_VER_PRNO3;
    /* uT/OS Build Number */
    pk_rver->prno[3] = (uint16_t)KNL_CFG_VER_PRNO4;  

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_TK_REF_VER */

#ifdef KNL_CFG_USE_FUNC_TK_ROT_RDQ
/*
 *    Function Name : tk_rot_rdq_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Rotate ready queue
 *    Param         : PRI tskpri : Task priority
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_rot_rdq_impl( PRI tskpri )
{
    KNL_CHK_PRI_RUN(tskpri)

    KNL_TSK_BEGIN_CRITICAL_SECTION
    if ( tskpri == TPRI_RUN ) {
        if ( KNL_TSK_IN_INDP() ) {
            knl_tsk_rdyquerotrun();
        } else {
            knl_tsk_rdyquerotrdy(knl_tsk_execution->priority);
        }
    } else {
        knl_tsk_rdyquerotrdy(KNL_TSK_ITN_PRI(tskpri));
    }
    KNL_TSK_END_CRITICAL_SECTION

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_TK_ROT_RDQ */

#ifdef KNL_CFG_USE_FUNC_TK_SET_POW
/*
 *    Function Name : tk_set_pow
 *    Create Date   : 2014/5/1-2014/5/1
 *    Author        : wangshb
 *    Description   : Set Power-saving mode
 *    Param         : uint32_t powmode : power-saving mode
 *    Return Code   : ER : Error code
 */
SYSCALL ER tk_set_pow_impl( uint32_t powmode )
{
    ER  ercd = E_OK;

    KNL_CHK_INTSK()

    KNL_TSK_BEGIN_CRITICAL_SECTION
    switch ( powmode ) {
        case TPW_DOSUSPEND:
            knl_pow_off();
            break;
        case TPW_DISLOWPOW:
            if ( knl_pow_discnt >= KNL_POW_LOW_LIMIT ){
                ercd = E_QOVR;
            } else {
                knl_pow_discnt++;
            }
            break;
      case TPW_ENALOWPOW:
            if ( knl_pow_discnt <= 0u ){
                ercd = E_OBJ;
            } else {
                knl_pow_discnt--;
            }
            break;
      default:
            ercd = E_PAR;
            break;
    }
    KNL_TSK_END_CRITICAL_SECTION

    return ercd;
}
#endif /* KNL_CFG_USE_FUNC_TK_SET_POW */

/*
 *    Function Name : tk_sta_sys
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Initialize kernel and create/start initial task
 *    Param         : T_CTSK *pk_ctsk : Information about init task
 *    Return Code   : void
 */
SYSCALL void tk_sta_sys( T_CTSK *pk_ctsk )
{
    ER  ercd;
#if TK_USE_MESSAGE
    uint8_t  *name = NULL;
#endif
    
#if TK_USE_MESSAGE
    /* Boot message */
    tm_putstring((const uint8_t*)&knl_sys_bootmsg);
    
    /* uT/OS message, fomula is uT/OS V1.x.xx(build xxxxxx) is starting up ... */
    tm_putstring((uint8_t*)&"uT/OS V");
    tm_putchar((int32_t)(KNL_CFG_VER_PRVER / 256u));
    tm_putchar((int32_t)'.');
    tm_putchar((int32_t)(KNL_CFG_VER_PRVER % 256u));
    tm_putchar((int32_t)'.');
    tm_putchar((int32_t)(KNL_CFG_VER_PRNO1 / 256u ));
    tm_putchar((int32_t)(KNL_CFG_VER_PRNO1 % 256u ));
    tm_putstring((uint8_t*)&"(build ");
    tm_putchar((int32_t)(KNL_CFG_VER_PRNO2 / 256u ));
    tm_putchar((int32_t)(KNL_CFG_VER_PRNO2 % 256u ));
    tm_putchar((int32_t)(KNL_CFG_VER_PRNO3 / 256u ));
    tm_putchar((int32_t)(KNL_CFG_VER_PRNO3 % 256u ));
    tm_putchar((int32_t)(KNL_CFG_VER_PRNO4 / 256u ));
    tm_putchar((int32_t)(KNL_CFG_VER_PRNO4 % 256u ));
    tm_putstring((uint8_t*)&") is starting up ... \n\n");
#endif /* TK_USE_MESSAGE */

    KNL_INT_DISABLE

    /*
     * Target-dependent initialization
     */
    ercd = knl_cpu_init();
    if ( ercd < E_OK ) {
#if TK_USE_MESSAGE
        name = (uint8_t*)&"cpu";
#endif
        goto error_exit;
    }
    
    /*
     * Each module initialization
     */
    ercd = knl_tsk_init();
    if ( ercd < E_OK ) {
#if TK_USE_MESSAGE
        name = (uint8_t*)&"tsk";
#endif
        goto error_exit;
    }
    
#if KNL_CFG_MAX_SEMID > 0
#ifdef KNL_CFG_USE_FUNC_KNL_SEM_INIT
    ercd = knl_sem_init();
    if ( ercd < E_OK ) {
#if TK_USE_MESSAGE
        name = (uint8_t*)&"sem";
#endif
        goto error_exit;
    }
#endif
#endif

#if KNL_CFG_MAX_FLGID > 0
#ifdef KNL_CFG_USE_FUNC_KNL_FLG_INIT
    ercd = knl_flg_init();
    if ( ercd < E_OK ) {
#if TK_USE_MESSAGE
        name = (uint8_t*)&"flg";
#endif
        goto error_exit;
    }
#endif
#endif

#if KNL_CFG_MAX_MBXID > 0
#ifdef KNL_CFG_USE_FUNC_KNL_MBX_INIT
    ercd = knl_mbx_init();
    if ( ercd < E_OK ) {
#if TK_USE_MESSAGE
        name = (uint8_t*)&"mbx";
#endif
        goto error_exit;
    }
#endif
#endif

#if KNL_CFG_MAX_MTXID > 0
#ifdef KNL_CFG_USE_FUNC_KNL_MTX_INIT
    ercd = knl_mtx_init();
    if ( ercd < E_OK ) {
#if TK_USE_MESSAGE
        name = (uint8_t*)&"mtx";
#endif
        goto error_exit;
    }
#endif
#endif

#if KNL_CFG_MAX_MBFID > 0
#ifdef KNL_CFG_USE_FUNC_KNL_MBF_INIT
    ercd = knl_mbf_init();
    if ( ercd < E_OK ) {
#if TK_USE_MESSAGE
        name = (uint8_t*)&"mbf";
#endif
        goto error_exit;
    }
#endif
#endif

#if KNL_CFG_MAX_PORID > 0
#ifdef KNL_CFG_USE_FUNC_KNL_POR_INIT
    ercd = knl_por_init();
    if ( ercd < E_OK ) {
#if TK_USE_MESSAGE
        name = (uint8_t*)&"por";
#endif
        goto error_exit;
    }
#endif
#endif

#if KNL_CFG_MAX_MPFID > 0
#ifdef KNL_CFG_USE_FUNC_KNL_MPF_INIT
    ercd = knl_mpf_init();
    if ( ercd < E_OK ) {
#if TK_USE_MESSAGE
        name = (uint8_t*)&"mpf";
#endif
        goto error_exit;
    }
#endif
#endif

#if KNL_CFG_MAX_MPLID > 0
#ifdef KNL_CFG_USE_FUNC_KNL_MPL_INIT
    ercd = knl_mpl_init();
    if ( ercd < E_OK ) {
#if TK_USE_MESSAGE
        name = (uint8_t*)&"mpl";
#endif
        goto error_exit;
    }
#endif
#endif

#if KNL_CFG_MAX_CYCID > 0
#ifdef KNL_CFG_USE_FUNC_KNL_CYC_INIT
    ercd = knl_cyc_init();
    if ( ercd < E_OK ) {
#if TK_USE_MESSAGE
        name = (uint8_t*)&"cyc";
#endif
        goto error_exit;
    }
#endif
#endif
 
#if KNL_CFG_MAX_ALMID > 0
#ifdef KNL_CFG_USE_FUNC_KNL_ALM_INIT
    ercd = knl_alm_init();
    if ( ercd < E_OK ) {
#if TK_USE_MESSAGE
        name = (uint8_t*)&"alm";
#endif
        goto error_exit;
    }
#endif
#endif

    ercd = knl_tck_init();
    if ( ercd < E_OK ) {
#if TK_USE_MESSAGE
        name = (uint8_t*)&"tick";
#endif
        goto error_exit;
    }
    
    /*
     * Create/start initial task
     */
    ercd = tk_cre_tsk_impl(pk_ctsk);
    if ( ercd < E_OK ) {
#if TK_USE_MESSAGE
        name = (uint8_t*)&"inittsk";
#endif
        goto error_exit;
    }

    ercd = tk_sta_tsk_impl((ID)ercd, 0);
    if ( ercd < E_OK ) {
#if TK_USE_MESSAGE
        name = (uint8_t*)&"inittsk";
#endif
        goto error_exit;
    }

    knl_dsp_force();
    /* No return */

error_exit:
#if TK_USE_MESSAGE
    tm_putstring(name);
    tm_putstring((uint8_t*)&" : module initialize error\n");
#endif
    tm_monitor();
}

/*
 *  Debugger support function
 */
#if TK_USE_DBGSPT

#ifdef KNL_CFG_USE_FUNC_TD_REF_DSNAME
/*
 *    Function Name : td_ref_dsname_impl
 *    Create Date   : 2012/7/9-2013/4/20
 *    Author        : wangshb
 *    Description   : Refer to DS Object Name
 *    Param         : uint32_t type : object type
 *                    ID id : object ID
 *                    uint8_t *dsname : address to return DS object name
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_ref_dsname_impl( uint32_t type, ID id, uint8_t *dsname )
{
#if TK_USE_OBJ_NAME
    uint8_t  *name_cb;
    ER  ercd;

    ercd = knl_sys_getname(type, id, &name_cb);
    if (ercd == E_OK) {
        strncpy((char*)dsname, (char*)name_cb, TK_OBJ_NAME_LENGTH);
    }

    return ercd;
#else
    return E_NOSPT;
#endif /* TK_USE_OBJ_NAME */
}
#endif /* KNL_CFG_USE_FUNC_TD_REF_DSNAME */

#ifdef KNL_CFG_USE_FUNC_TD_REF_SYS
/*
 *    Function Name : td_ref_sys_impl
 *    Create Date   : 2012/7/9-2013/4/20
 *    Author        : wangshb
 *    Description   : Reference System Status
 *    Param         : TD_RSYS *pk_rsys : Address of packet for returning status
 *                    information
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_ref_sys_impl( TD_RSYS *pk_rsys )
{
    if ( KNL_TSK_IN_INDP() ) {
        pk_rsys->sysstat = TSS_INDP;
    } else {
        if ( KNL_TSK_IN_QTSK() ) {
            pk_rsys->sysstat = TSS_QTSK;
        } else {
            pk_rsys->sysstat = TSS_TSK;
        }
        if ( KNL_TSK_IN_LOC() ) {
            pk_rsys->sysstat |= TSS_DINT;
        }
        if ( KNL_TSK_IN_DDSP() ) {
            pk_rsys->sysstat |= TSS_DDSP;
        }
    }
    pk_rsys->runtskid = ( knl_tsk_execution != NULL )? knl_tsk_execution->tskid: 0;
    pk_rsys->schedtskid = ( knl_tsk_schedule != NULL )? knl_tsk_schedule->tskid: 0;

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_TD_REF_SYS */

#ifdef KNL_CFG_USE_FUNC_TD_SET_DSNAME
/*
 *    Function Name : td_set_dsname_impl
 *    Create Date   : 2012/7/9-2013/4/20
 *    Author        : wangshb
 *    Description   : Set DS Object Name
 *    Param         : uint32_t type : object type
 *                    ID id : object ID
 *                    uint8_t *dsname : DS object name to be set
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_set_dsname_impl( uint32_t type, ID id, uint8_t *dsname )
{
#if TK_USE_OBJ_NAME
    uint8_t  *name_cb;
    ER  ercd;

    ercd = knl_sys_getname(type, id, &name_cb);
    if (ercd == E_OK) {
        strncpy((char*)name_cb, (char*)dsname, TK_OBJ_NAME_LENGTH);
    }

    return ercd;
#else
    return E_NOSPT;
#endif /* TK_USE_OBJ_NAME */
}
#endif /* KNL_CFG_USE_FUNC_TD_SET_DSNAME */


#endif /* TK_USE_DBGSPT */
