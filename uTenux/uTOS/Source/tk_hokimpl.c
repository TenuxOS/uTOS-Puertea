/*
 *----------------------------------------------------------------------------------------
 *    Kernel/hook
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
 *    File Name      : tk_hokimpl.c
 *    Create Date    : 2013/5/26-2014/5/4
 *    Author         : wangshb
 *    Description    : uT/OS Hook Function.
 *----------------------------------------------------------------------------------------
 */

#include "tk_almimpl.h"
#include "tk_cycimpl.h"
#include "tk_flgimpl.h"
#include "tk_intimpl.h"
#include "tk_mbfimpl.h"
#include "tk_mbximpl.h"
#include "tk_mpfimpl.h"
#include "tk_mplimpl.h"
#include "tk_mtximpl.h"
#include "tk_porimpl.h"
#include "tk_semimpl.h"
#include "tk_sysimpl.h"
#include "tk_timimpl.h"
#include "tk_tskimpl.h"
#include "tk_hokimpl.h"

#if (TK_USE_DBGSPT && TK_USE_HOOK_TRACE)
/*
 * Definition of unused system call
 */
#ifndef KNL_CFG_USE_FUNC_TK_CRE_SEM
#define tk_cre_sem_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_DEL_SEM
#define tk_del_sem_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_SIG_SEM
#define tk_sig_sem_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_WAI_SEM
#define tk_wai_sem_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_REF_SEM
#define tk_ref_sem_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_LST_SEM
#define td_lst_sem_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_REF_SEM
#define td_ref_sem_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_SEM_QUE
#define td_sem_que_impl      knl_hok_unspt
#endif

#ifndef KNL_CFG_USE_FUNC_TK_CRE_FLG
#define tk_cre_flg_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_DEL_FLG
#define tk_del_flg_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_SET_FLG
#define tk_set_flg_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_CLR_FLG
#define tk_clr_flg_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_WAI_FLG
#define tk_wai_flg_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_REF_FLG
#define tk_ref_flg_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_LST_FLG
#define td_lst_flg_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_REF_FLG
#define td_ref_flg_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_FLG_QUE
#define td_flg_que_impl      knl_hok_unspt
#endif

#ifndef KNL_CFG_USE_FUNC_TK_CRE_MBX
#define tk_cre_mbx_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_DEL_MBX
#define tk_del_mbx_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_SND_MBX
#define tk_snd_mbx_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_RCV_MBX
#define tk_rcv_mbx_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_REF_MBX
#define tk_ref_mbx_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_LST_MBX
#define td_lst_mbx_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_REF_MBX
#define td_ref_mbx_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_MBX_QUE
#define td_mbx_que_impl      knl_hok_unspt
#endif

#ifndef KNL_CFG_USE_FUNC_TK_CRE_MBF
#define tk_cre_mbf_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_DEL_MBF
#define tk_del_mbf_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_SND_MBF
#define tk_snd_mbf_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_RCV_MBF
#define tk_rcv_mbf_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_REF_MBF
#define tk_ref_mbf_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_LST_MBF
#define td_lst_mbf_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_REF_MBF
#define td_ref_mbf_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_SMBF_QUE
#define td_smbf_que_impl     knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_RMBF_QUE
#define td_rmbf_que_impl     knl_hok_unspt
#endif

#ifndef KNL_CFG_USE_FUNC_TK_CRE_POR
#define tk_cre_por_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_DEL_POR
#define tk_del_por_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_CAL_POR
#define tk_cal_por_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_ACP_POR
#define tk_acp_por_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_FWD_POR
#define tk_fwd_por_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_RPL_RDV
#define tk_rpl_rdv_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_REF_POR
#define tk_ref_por_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_LST_POR
#define td_lst_por_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_REF_POR
#define td_ref_por_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_ACP_QUE
#define td_acp_que_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_CAL_QUE
#define td_cal_que_impl      knl_hok_unspt
#endif

#ifndef KNL_CFG_USE_FUNC_TK_CRE_MTX
#define tk_cre_mtx_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_DEL_MTX
#define tk_del_mtx_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_LOC_MTX
#define tk_loc_mtx_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_UNL_MTX
#define tk_unl_mtx_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_REF_MTX
#define tk_ref_mtx_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_LST_MTX
#define td_lst_mtx_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_REF_MTX
#define td_ref_mtx_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_MTX_QUE
#define td_mtx_que_impl      knl_hok_unspt
#endif

#ifndef KNL_CFG_USE_FUNC_TK_DEF_INT
#define tk_def_int_impl      knl_hok_unspt
#endif

#ifndef KNL_CFG_USE_FUNC_TK_CRE_MPL
#define tk_cre_mpl_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_DEL_MPL
#define tk_del_mpl_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_GET_MPL
#define tk_get_mpl_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_REL_MPL
#define tk_rel_mpl_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_REF_MPL
#define tk_ref_mpl_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_LST_MPL
#define td_lst_mpl_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_REF_MPL
#define td_ref_mpl_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_MPL_QUE
#define td_mpl_que_impl      knl_hok_unspt
#endif

#ifndef KNL_CFG_USE_FUNC_TK_CRE_MPF
#define tk_cre_mpf_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_DEL_MPF
#define tk_del_mpf_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_GET_MPF
#define tk_get_mpf_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_REL_MPF
#define tk_rel_mpf_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_REF_MPF
#define tk_ref_mpf_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_LST_MPF
#define td_lst_mpf_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_REF_MPF
#define td_ref_mpf_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_MPF_QUE
#define td_mpf_que_impl      knl_hok_unspt
#endif

#ifndef KNL_CFG_USE_FUNC_TK_CRE_CYC
#define tk_cre_cyc_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_DEL_CYC
#define tk_del_cyc_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_STA_CYC
#define tk_sta_cyc_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_STP_CYC
#define tk_stp_cyc_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_REF_CYC
#define tk_ref_cyc_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_LST_CYC
#define td_lst_cyc_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_REF_CYC
#define td_ref_cyc_impl      knl_hok_unspt
#endif

#ifndef KNL_CFG_USE_FUNC_TK_CRE_ALM
#define tk_cre_alm_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_DEL_ALM
#define tk_del_alm_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_STA_ALM
#define tk_sta_alm_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_STP_ALM
#define tk_stp_alm_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TK_REF_ALM
#define tk_ref_alm_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_LST_ALM
#define td_lst_alm_impl      knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_REF_ALM
#define td_ref_alm_impl      knl_hok_unspt
#endif

#ifndef KNL_CFG_USE_FUNC_TK_SET_POW
#define tk_set_pow_impl      knl_hok_unspt
#endif

#ifndef KNL_CFG_USE_FUNC_TD_HOK_SVC
#define td_hok_svc_impl     knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_HOK_DSP
#define td_hok_dsp_impl     knl_hok_unspt
#endif
#ifndef KNL_CFG_USE_FUNC_TD_HOK_INT
#define td_hok_int_impl     knl_hok_unspt
#endif

/*
 *  System call entry table (tk_ function)
 */
FP knl_hok_tksvctbl[KNL_HOK_TKTBL_NUM]={ \
        (FP)knl_hok_unspt,   (FP)tk_cre_tsk_impl, (FP)tk_del_tsk_impl,  \
        (FP)tk_sta_tsk_impl, (FP)tk_ext_tsk_impl, (FP)tk_exd_tsk_impl,  \
        (FP)tk_ter_tsk_impl, (FP)tk_dis_dsp_impl, (FP)tk_ena_dsp_impl,  \
        (FP)tk_chg_pri_impl, (FP)tk_rot_rdq_impl, (FP)tk_rel_wai_impl,  \
        (FP)tk_get_tid_impl, (FP)tk_get_reg_impl, (FP)tk_set_reg_impl,  \
        (FP)tk_ref_tsk_impl, (FP)tk_sus_tsk_impl, (FP)tk_rsm_tsk_impl,  \
        (FP)tk_frsm_tsk_impl,(FP)tk_slp_tsk_impl, (FP)tk_wup_tsk_impl,  \
        (FP)tk_can_wup_impl, (FP)tk_cre_sem_impl, (FP)tk_del_sem_impl,  \
        (FP)tk_sig_sem_impl, (FP)tk_wai_sem_impl, (FP)tk_ref_sem_impl,  \
        (FP)tk_cre_mtx_impl, (FP)tk_del_mtx_impl, (FP)tk_loc_mtx_impl,  \
        (FP)tk_unl_mtx_impl, (FP)tk_ref_mtx_impl, (FP)tk_cre_flg_impl,  \
        (FP)tk_del_flg_impl, (FP)tk_set_flg_impl, (FP)tk_clr_flg_impl,  \
        (FP)tk_wai_flg_impl, (FP)tk_ref_flg_impl, (FP)tk_cre_mbx_impl,  \
        (FP)tk_del_mbx_impl, (FP)tk_snd_mbx_impl, (FP)tk_rcv_mbx_impl,  \
        (FP)tk_ref_mbx_impl, (FP)tk_cre_mbf_impl, (FP)tk_del_mbf_impl,  \
        (FP)tk_snd_mbf_impl, (FP)tk_rcv_mbf_impl, (FP)tk_ref_mbf_impl,  \
        (FP)tk_cre_por_impl, (FP)tk_del_por_impl, (FP)tk_cal_por_impl,  \
        (FP)tk_acp_por_impl, (FP)tk_fwd_por_impl, (FP)tk_rpl_rdv_impl,  \
        (FP)tk_ref_por_impl, (FP)tk_def_int_impl, (FP)knl_hok_unspt,    \
        (FP)tk_cre_mpl_impl, (FP)tk_del_mpl_impl, (FP)tk_get_mpl_impl,  \
        (FP)tk_rel_mpl_impl, (FP)tk_ref_mpl_impl, (FP)tk_cre_mpf_impl,  \
        (FP)tk_del_mpf_impl, (FP)tk_get_mpf_impl, (FP)tk_rel_mpf_impl,  \
        (FP)tk_ref_mpf_impl, (FP)tk_set_tim_impl, (FP)tk_get_tim_impl,  \
        (FP)tk_get_etm_impl, (FP)tk_get_otm_impl, (FP)tk_dly_tsk_impl,  \
        (FP)tk_cre_cyc_impl, (FP)tk_del_cyc_impl, (FP)tk_sta_cyc_impl,  \
        (FP)tk_stp_cyc_impl, (FP)tk_ref_cyc_impl, (FP)tk_cre_alm_impl,  \
        (FP)tk_del_alm_impl, (FP)tk_sta_alm_impl, (FP)tk_stp_alm_impl,  \
        (FP)tk_ref_alm_impl, (FP)tk_set_pow_impl, (FP)tk_ref_ver_impl,  \
        (FP)tk_ref_sys_impl};

/*
 * System call entry table (td_ function)
 */
FP knl_hok_tdsvctbl[KNL_HOK_TDTBL_NUM]={ 
        (FP)knl_hok_unspt,   (FP)td_lst_tsk_impl, (FP)td_lst_sem_impl,  \
        (FP)td_lst_flg_impl, (FP)td_lst_mbx_impl, (FP)td_lst_mtx_impl,  \
        (FP)td_lst_mbf_impl, (FP)td_lst_por_impl, (FP)td_lst_mpf_impl,  \
        (FP)td_lst_mpl_impl, (FP)td_lst_cyc_impl, (FP)td_lst_alm_impl,  \
        (FP)td_ref_sem_impl, (FP)td_ref_flg_impl, (FP)td_ref_mbx_impl,  \
        (FP)td_ref_mtx_impl, (FP)td_ref_mbf_impl, (FP)td_ref_por_impl,  \
        (FP)td_ref_mpf_impl, (FP)td_ref_mpl_impl, (FP)td_ref_cyc_impl,  \
        (FP)td_ref_alm_impl, (FP)td_ref_tsk_impl, (FP)td_inf_tsk_impl,  \
        (FP)td_get_reg_impl, (FP)td_set_reg_impl, (FP)td_ref_sys_impl,  \
        (FP)td_get_tim_impl, (FP)td_get_otm_impl, (FP)td_rdy_que_impl,  \
        (FP)td_sem_que_impl, (FP)td_flg_que_impl, (FP)td_mbx_que_impl,  \
        (FP)td_mtx_que_impl, (FP)td_smbf_que_impl,(FP)td_rmbf_que_impl, \
        (FP)td_cal_que_impl, (FP)td_acp_que_impl, (FP)td_mpf_que_impl,  \
        (FP)td_mpl_que_impl, (FP)td_hok_svc_impl, (FP)td_hok_dsp_impl,  \
        (FP)td_hok_int_impl, (FP)td_ref_dsname_impl, \
        (FP)td_set_dsname_impl };

/* 
 * Hook Dsp routine address 
 */
#ifdef KNL_CFG_USE_FUNC_KNL_HOK_DSPEXECFP
Zeroinit FP knl_hok_dspexecfp;
Zeroinit FP knl_hok_dspstopfp;
#endif /* KNL_CFG_USE_FUNC_KNL_HOK_DSPEXECFP */

/* 
 * Hook Int routine address 
 */
#ifdef KNL_CFG_USE_FUNC_KNL_HOK_INTENTERFP
Zeroinit FP knl_hok_intenterfp;
Zeroinit FP knl_hok_intleavefp;
#endif /* KNL_CFG_USE_FUNC_KNL_HOK_INTENTERFP */

/*
 * Hook SVC routine address
 */
#ifdef KNL_CFG_USE_FUNC_KNL_HOK_SVCENTERFP
Zeroinit FP knl_hok_svcenterfp;
Zeroinit FP knl_hok_svcleavefp;
#endif /* KNL_CFG_USE_FUNC_KNL_HOK_SVCENTERFP */

#ifdef KNL_CFG_USE_FUNC_TD_HOK_DSP
/*
 *    Function Name : td_hok_dsp_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Define Task Dispatch Hook Routine
 *    Param         : TD_HDSP *hdsp : Hook routine definition information
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_hok_dsp_impl( TD_HDSP *hdsp )
{
    KNL_INT_BEGIN_DISABLE
    if ( hdsp == NULL ) {                        /* Cancel dispatcher hook routine */
        /* Cancel */
        knl_hok_undsp();
    } else {
        /* Set */
        knl_hok_dspexecfp = hdsp->exec;
        knl_hok_dspstopfp = hdsp->stop;
        knl_hok_dsp();
    }
    KNL_INT_END_DISABLE

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_TD_HOK_DSP */

#ifdef KNL_CFG_USE_FUNC_TD_HOK_INT
/*
 *    Function Name : td_hok_int_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Set/Cancel EIT handler hook routine
 *    Param         : TD_HINT *hint : Hook routine definition information
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_hok_int_impl( TD_HINT *hint )
{
#if TK_USE_INT_HLLHDR
    KNL_INT_BEGIN_DISABLE
    if ( hint == NULL ) {                        /* Cancel int handler hook routine */
        /* Cancel */
        knl_hok_unint();
    } else {
        /* Set */
        knl_hok_intenterfp = hint->enter;
        knl_hok_intleavefp = hint->leave;
        knl_hok_int();
    }
    KNL_INT_END_DISABLE

    return E_OK;
#else
    return E_NOSPT;
#endif /* TK_USE_INT_HLLHDR */
}
#endif /* KNL_CFG_USE_FUNC_TD_HOK_INT */

#ifdef KNL_CFG_USE_FUNC_TD_HOK_SVC
/*
 *    Function Name : td_hok_svc_impl
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Define System Call/Extended SVC Hook Routine
 *    Param         : TD_HSVC *hsvc : Hook routine definition information
 *    Return Code   : ER : Error code
 */
SYSCALL ER td_hok_svc_impl( TD_HSVC *hsvc )
{
    KNL_INT_BEGIN_DISABLE
    if ( hsvc == NULL ) {                        /* Cancel system call hook routine */
        /* Cancel */
        knl_hok_unsvc();
    } else {
        /* Set */
        knl_hok_svcenterfp = hsvc->enter;
        knl_hok_svcleavefp = hsvc->leave;
        knl_hok_svc();
    }
    KNL_INT_END_DISABLE

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_TD_HOK_SVC */

#endif /* TK_USE_DBGSPT && TK_USE_HOOK_TRACE */


