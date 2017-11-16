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
 *    File Name      : tk_hokrap.S
 *    Create Date    : 2013/12/22-2014/5/31
 *    Author         : WangShb
 *    Description    : Debug and Trace Hook interface.
 *----------------------------------------------------------------------------------------
 */

#include "tk_hokwrap.h"

#if TK_INT_BITWIDTH == 32
/*
 * uT/OS function code
 */
#define KNL_HOK_TKFN_CRE_TSK      0x80010100
#define KNL_HOK_TKFN_DEL_TSK      0x80020100
#define KNL_HOK_TKFN_STA_TSK      0x80030200
#define KNL_HOK_TKFN_EXT_TSK      0x80040000
#define KNL_HOK_TKFN_EXD_TSK      0x80050000
#define KNL_HOK_TKFN_TER_TSK      0x80060100
#define KNL_HOK_TKFN_DIS_DSP      0x80070000
#define KNL_HOK_TKFN_ENA_DSP      0x80080000
#define KNL_HOK_TKFN_CHG_PRI      0x80090200
#define KNL_HOK_TKFN_ROT_RDQ      0x800a0100
#define KNL_HOK_TKFN_REL_WAI      0x800b0100
#define KNL_HOK_TKFN_GET_TID      0x800c0000
#define KNL_HOK_TKFN_GET_REG      0x800d0400
#define KNL_HOK_TKFN_SET_REG      0x800e0400
#define KNL_HOK_TKFN_REF_TSK      0x800f0200
#define KNL_HOK_TKFN_SUS_TSK      0x80100100
#define KNL_HOK_TKFN_RSM_TSK      0x80110100
#define KNL_HOK_TKFN_FRSM_TSK     0x80120100
#define KNL_HOK_TKFN_SLP_TSK      0x80130100
#define KNL_HOK_TKFN_WUP_TSK      0x80140100
#define KNL_HOK_TKFN_CAN_WUP      0x80150100
#define KNL_HOK_TKFN_CRE_SEM      0x80160100
#define KNL_HOK_TKFN_DEL_SEM      0x80170100
#define KNL_HOK_TKFN_SIG_SEM      0x80180200
#define KNL_HOK_TKFN_WAI_SEM      0x80190300
#define KNL_HOK_TKFN_REF_SEM      0x801a0200
#define KNL_HOK_TKFN_CRE_MTX      0x801b0100
#define KNL_HOK_TKFN_DEL_MTX      0x801c0100
#define KNL_HOK_TKFN_LOC_MTX      0x801d0200
#define KNL_HOK_TKFN_UNL_MTX      0x801e0100
#define KNL_HOK_TKFN_REF_MTX      0x801f0200
#define KNL_HOK_TKFN_CRE_FLG      0x80200100
#define KNL_HOK_TKFN_DEL_FLG      0x80210100
#define KNL_HOK_TKFN_SET_FLG      0x80220200
#define KNL_HOK_TKFN_CLR_FLG      0x80230200
#define KNL_HOK_TKFN_WAI_FLG      0x80240500
#define KNL_HOK_TKFN_REF_FLG      0x80250200
#define KNL_HOK_TKFN_CRE_MBX      0x80260100
#define KNL_HOK_TKFN_DEL_MBX      0x80270100
#define KNL_HOK_TKFN_SND_MBX      0x80280200
#define KNL_HOK_TKFN_RCV_MBX      0x80290300
#define KNL_HOK_TKFN_REF_MBX      0x802a0200
#define KNL_HOK_TKFN_CRE_MBF      0x802b0100
#define KNL_HOK_TKFN_DEL_MBF      0x802c0100
#define KNL_HOK_TKFN_SND_MBF      0x802d0400
#define KNL_HOK_TKFN_RCV_MBF      0x802e0300
#define KNL_HOK_TKFN_REF_MBF      0x802f0200
#define KNL_HOK_TKFN_CRE_POR      0x80300100
#define KNL_HOK_TKFN_DEL_POR      0x80310100
#define KNL_HOK_TKFN_CAL_POR      0x80320500
#define KNL_HOK_TKFN_ACP_POR      0x80330500
#define KNL_HOK_TKFN_FWD_POR      0x80340500
#define KNL_HOK_TKFN_RPL_RDV      0x80350300
#define KNL_HOK_TKFN_REF_POR      0x80360200
#define KNL_HOK_TKFN_DEF_INT      0x80370200
#define KNL_HOK_TKFN_RET_INT      0x80380000
#define KNL_HOK_TKFN_CRE_MPL      0x80390100
#define KNL_HOK_TKFN_DEL_MPL      0x803a0100
#define KNL_HOK_TKFN_GET_MPL      0x803b0400
#define KNL_HOK_TKFN_REL_MPL      0x803c0200
#define KNL_HOK_TKFN_REF_MPL      0x803d0200
#define KNL_HOK_TKFN_CRE_MPF      0x803e0100
#define KNL_HOK_TKFN_DEL_MPF      0x803f0100
#define KNL_HOK_TKFN_GET_MPF      0x80400300
#define KNL_HOK_TKFN_REL_MPF      0x80410200
#define KNL_HOK_TKFN_REF_MPF      0x80420200
#define KNL_HOK_TKFN_SET_TIM      0x80430100
#define KNL_HOK_TKFN_GET_TIM      0x80440100
#define KNL_HOK_TKFN_GET_ETM      0x80450100
#define KNL_HOK_TKFN_GET_OTM      0x80460100
#define KNL_HOK_TKFN_DLY_TSK      0x80470100
#define KNL_HOK_TKFN_CRE_CYC      0x80480100
#define KNL_HOK_TKFN_DEL_CYC      0x80490100
#define KNL_HOK_TKFN_STA_CYC      0x804a0100
#define KNL_HOK_TKFN_STP_CYC      0x804b0100
#define KNL_HOK_TKFN_REF_CYC      0x804c0200
#define KNL_HOK_TKFN_CRE_ALM      0x804d0100
#define KNL_HOK_TKFN_DEL_ALM      0x804e0100
#define KNL_HOK_TKFN_STA_ALM      0x804f0200
#define KNL_HOK_TKFN_STP_ALM      0x80500100
#define KNL_HOK_TKFN_REF_ALM      0x80510200
#define KNL_HOK_TKFN_SET_POW      0x80520100
#define KNL_HOK_TKFN_REF_VER      0x80530100
#define KNL_HOK_TKFN_REF_SYS      0x80540100

/*
 * uT/OS function code (Debugger Support)
 */
#define KNL_HOK_TDFN_LST_TSK      0x80010200
#define KNL_HOK_TDFN_LST_SEM      0x80020200
#define KNL_HOK_TDFN_LST_FLG      0x80030200
#define KNL_HOK_TDFN_LST_MBX      0x80040200
#define KNL_HOK_TDFN_LST_MTX      0x80050200
#define KNL_HOK_TDFN_LST_MBF      0x80060200
#define KNL_HOK_TDFN_LST_POR      0x80070200
#define KNL_HOK_TDFN_LST_MPF      0x80080200
#define KNL_HOK_TDFN_LST_MPL      0x80090200
#define KNL_HOK_TDFN_LST_CYC      0x800a0200
#define KNL_HOK_TDFN_LST_ALM      0x800b0200
#define KNL_HOK_TDFN_REF_SEM      0x800c0200
#define KNL_HOK_TDFN_REF_FLG      0x800d0200
#define KNL_HOK_TDFN_REF_MBX      0x800e0200
#define KNL_HOK_TDFN_REF_MTX      0x800f0200
#define KNL_HOK_TDFN_REF_MBF      0x80100200
#define KNL_HOK_TDFN_REF_POR      0x80110200
#define KNL_HOK_TDFN_REF_MPF      0x80120200
#define KNL_HOK_TDFN_REF_MPL      0x80130200
#define KNL_HOK_TDFN_REF_CYC      0x80140200
#define KNL_HOK_TDFN_REF_ALM      0x80150200
#define KNL_HOK_TDFN_REF_TSK      0x80160200
#define KNL_HOK_TDFN_INF_TSK      0x80170300
#define KNL_HOK_TDFN_GET_REG      0x80180400
#define KNL_HOK_TDFN_SET_REG      0x80190400
#define KNL_HOK_TDFN_REF_SYS      0x801a0100
#define KNL_HOK_TDFN_GET_TIM      0x801b0200
#define KNL_HOK_TDFN_GET_OTM      0x801c0200
#define KNL_HOK_TDFN_RDY_QUE      0x801d0300
#define KNL_HOK_TDFN_SEM_QUE      0x801e0300
#define KNL_HOK_TDFN_FLG_QUE      0x801f0300
#define KNL_HOK_TDFN_MBX_QUE      0x80200300
#define KNL_HOK_TDFN_MTX_QUE      0x80210300
#define KNL_HOK_TDFN_SMBF_QUE     0x80220300
#define KNL_HOK_TDFN_RMBF_QUE     0x80230300
#define KNL_HOK_TDFN_CAL_QUE      0x80240300
#define KNL_HOK_TDFN_ACP_QUE      0x80250300
#define KNL_HOK_TDFN_MPF_QUE      0x80260300
#define KNL_HOK_TDFN_MPL_QUE      0x80270300
#define KNL_HOK_TDFN_HOK_SVC      0x80280100
#define KNL_HOK_TDFN_HOK_DSP      0x80290100
#define KNL_HOK_TDFN_HOK_INT      0x802a0100
#define KNL_HOK_TDFN_REF_DSNAME   0x802b0300
#define KNL_HOK_TDFN_SET_DSNAME   0x802c0300
#endif

#if (TK_USE_DBGSPT && TK_USE_HOOK_TRACE)

    /* 
     * Task SVC interface
     */
    KNL_HOK_TKWRAPER_MACRO tk_can_wup, KNL_HOK_TKFN_CAN_WUP

    KNL_HOK_TKWRAPER_MACRO tk_chg_pri, KNL_HOK_TKFN_CHG_PRI

    KNL_HOK_TKWRAPER_MACRO tk_cre_tsk, KNL_HOK_TKFN_CRE_TSK

    KNL_HOK_TKWRAPER_MACRO tk_del_tsk, KNL_HOK_TKFN_DEL_TSK

    KNL_HOK_TKWRAPER_MACRO tk_dly_tsk, KNL_HOK_TKFN_DLY_TSK

    KNL_HOK_TKWRAPER_MACRO tk_exd_tsk, KNL_HOK_TKFN_EXD_TSK

    KNL_HOK_TKWRAPER_MACRO tk_ext_tsk, KNL_HOK_TKFN_EXT_TSK

    KNL_HOK_TKWRAPER_MACRO tk_frsm_tsk, KNL_HOK_TKFN_FRSM_TSK

    KNL_HOK_TKWRAPER_MACRO tk_get_reg, KNL_HOK_TKFN_GET_REG

    KNL_HOK_TKWRAPER_MACRO tk_ref_tsk, KNL_HOK_TKFN_REF_TSK

    KNL_HOK_TKWRAPER_MACRO tk_rel_wai, KNL_HOK_TKFN_REL_WAI

    KNL_HOK_TKWRAPER_MACRO tk_rsm_tsk, KNL_HOK_TKFN_RSM_TSK

    KNL_HOK_TKWRAPER_MACRO tk_set_reg, KNL_HOK_TKFN_SET_REG

    KNL_HOK_TKWRAPER_MACRO tk_slp_tsk, KNL_HOK_TKFN_SLP_TSK

    KNL_HOK_TKWRAPER_MACRO tk_sta_tsk, KNL_HOK_TKFN_STA_TSK

    KNL_HOK_TKWRAPER_MACRO tk_sus_tsk, KNL_HOK_TKFN_SUS_TSK

    KNL_HOK_TKWRAPER_MACRO tk_ter_tsk, KNL_HOK_TKFN_TER_TSK

    KNL_HOK_TKWRAPER_MACRO tk_wup_tsk, KNL_HOK_TKFN_WUP_TSK

    KNL_HOK_TDWRAPER_MACRO td_get_reg, KNL_HOK_TDFN_GET_REG
   
    KNL_HOK_TDWRAPER_MACRO td_inf_tsk, KNL_HOK_TDFN_INF_TSK

    KNL_HOK_TDWRAPER_MACRO td_lst_tsk, KNL_HOK_TDFN_LST_TSK

    KNL_HOK_TDWRAPER_MACRO td_rdy_que, KNL_HOK_TDFN_RDY_QUE

    KNL_HOK_TDWRAPER_MACRO td_ref_tsk, KNL_HOK_TDFN_REF_TSK

    KNL_HOK_TDWRAPER_MACRO td_set_reg, KNL_HOK_TDFN_SET_REG

    /* 
     * Semaphore SVC interface
     */
    KNL_HOK_TKWRAPER_MACRO tk_cre_sem, KNL_HOK_TKFN_CRE_SEM

    KNL_HOK_TKWRAPER_MACRO tk_del_sem, KNL_HOK_TKFN_DEL_SEM

    KNL_HOK_TKWRAPER_MACRO tk_ref_sem, KNL_HOK_TKFN_REF_SEM

    KNL_HOK_TKWRAPER_MACRO tk_sig_sem, KNL_HOK_TKFN_SIG_SEM

    KNL_HOK_TKWRAPER_MACRO tk_wai_sem, KNL_HOK_TKFN_WAI_SEM

    KNL_HOK_TDWRAPER_MACRO td_lst_sem, KNL_HOK_TDFN_LST_SEM

    KNL_HOK_TDWRAPER_MACRO td_ref_sem, KNL_HOK_TDFN_REF_SEM

    KNL_HOK_TDWRAPER_MACRO td_sem_que, KNL_HOK_TDFN_SEM_QUE


    /* 
     * Event flag SVC interface
     */
    KNL_HOK_TKWRAPER_MACRO tk_clr_flg, KNL_HOK_TKFN_CLR_FLG

    KNL_HOK_TKWRAPER_MACRO tk_cre_flg, KNL_HOK_TKFN_CRE_FLG

    KNL_HOK_TKWRAPER_MACRO tk_del_flg, KNL_HOK_TKFN_DEL_FLG

    KNL_HOK_TKWRAPER_MACRO tk_ref_flg, KNL_HOK_TKFN_REF_FLG

    KNL_HOK_TKWRAPER_MACRO tk_set_flg, KNL_HOK_TKFN_SET_FLG

    KNL_HOK_TKWRAPER_MACRO tk_wai_flg, KNL_HOK_TKFN_WAI_FLG

    KNL_HOK_TDWRAPER_MACRO td_flg_que, KNL_HOK_TDFN_FLG_QUE

    KNL_HOK_TDWRAPER_MACRO td_lst_flg, KNL_HOK_TDFN_LST_FLG

    KNL_HOK_TDWRAPER_MACRO td_ref_flg, KNL_HOK_TDFN_REF_FLG

    /* 
     * Mail box SVC interface
     */
    KNL_HOK_TKWRAPER_MACRO tk_cre_mbx, KNL_HOK_TKFN_CRE_MBX

    KNL_HOK_TKWRAPER_MACRO tk_del_mbx, KNL_HOK_TKFN_DEL_MBX

    KNL_HOK_TKWRAPER_MACRO tk_rcv_mbx, KNL_HOK_TKFN_RCV_MBX

    KNL_HOK_TKWRAPER_MACRO tk_ref_mbx, KNL_HOK_TKFN_REF_MBX

    KNL_HOK_TKWRAPER_MACRO tk_snd_mbx, KNL_HOK_TKFN_SND_MBX

    KNL_HOK_TDWRAPER_MACRO td_lst_mbx, KNL_HOK_TDFN_LST_MBX

    KNL_HOK_TDWRAPER_MACRO td_mbx_que, KNL_HOK_TDFN_MBX_QUE

    KNL_HOK_TDWRAPER_MACRO td_ref_mbx, KNL_HOK_TDFN_REF_MBX     

    /* 
     * Mutex SVC interface
     */
    KNL_HOK_TKWRAPER_MACRO tk_cre_mtx, KNL_HOK_TKFN_CRE_MTX

    KNL_HOK_TKWRAPER_MACRO tk_del_mtx, KNL_HOK_TKFN_DEL_MTX

    KNL_HOK_TKWRAPER_MACRO tk_loc_mtx, KNL_HOK_TKFN_LOC_MTX

    KNL_HOK_TKWRAPER_MACRO tk_ref_mtx, KNL_HOK_TKFN_REF_MPL

    KNL_HOK_TKWRAPER_MACRO tk_unl_mtx, KNL_HOK_TKFN_UNL_MTX

    KNL_HOK_TDWRAPER_MACRO td_lst_mtx, KNL_HOK_TDFN_LST_MTX

    KNL_HOK_TDWRAPER_MACRO td_mtx_que, KNL_HOK_TDFN_MTX_QUE

    KNL_HOK_TDWRAPER_MACRO td_ref_mtx, KNL_HOK_TDFN_REF_MTX     
     
    /* 
     * Message buffer SVC interface
     */
    KNL_HOK_TKWRAPER_MACRO tk_cre_mbf, KNL_HOK_TKFN_CRE_MBF

    KNL_HOK_TKWRAPER_MACRO tk_del_mbf, KNL_HOK_TKFN_DEL_MBF

    KNL_HOK_TKWRAPER_MACRO tk_rcv_mbf, KNL_HOK_TKFN_RCV_MBF

    KNL_HOK_TKWRAPER_MACRO tk_ref_mbf, KNL_HOK_TKFN_REF_MBF

    KNL_HOK_TKWRAPER_MACRO tk_snd_mbf, KNL_HOK_TKFN_SND_MBF

    KNL_HOK_TDWRAPER_MACRO td_lst_mbf, KNL_HOK_TDFN_LST_MBF

    KNL_HOK_TDWRAPER_MACRO td_ref_mbf, KNL_HOK_TDFN_REF_MBF     
     
    KNL_HOK_TDWRAPER_MACRO td_rmbf_que, KNL_HOK_TDFN_RMBF_QUE

    KNL_HOK_TDWRAPER_MACRO td_smbf_que, KNL_HOK_TDFN_SMBF_QUE

    /* 
     * Rendezvous port SVC interface
     */
    KNL_HOK_TKWRAPER_MACRO tk_acp_por, KNL_HOK_TKFN_ACP_POR

    KNL_HOK_TKWRAPER_MACRO tk_cal_por, KNL_HOK_TKFN_CAL_POR

    KNL_HOK_TKWRAPER_MACRO tk_cre_por, KNL_HOK_TKFN_CRE_POR

    KNL_HOK_TKWRAPER_MACRO tk_del_por, KNL_HOK_TKFN_DEL_POR

    KNL_HOK_TKWRAPER_MACRO tk_fwd_por, KNL_HOK_TKFN_CRE_SEM

    KNL_HOK_TKWRAPER_MACRO tk_ref_por, KNL_HOK_TKFN_REF_POR

    KNL_HOK_TKWRAPER_MACRO tk_rpl_rdv, KNL_HOK_TKFN_RPL_RDV

    KNL_HOK_TDWRAPER_MACRO td_acp_que, KNL_HOK_TDFN_ACP_QUE

    KNL_HOK_TDWRAPER_MACRO td_cal_que, KNL_HOK_TDFN_CAL_QUE

    KNL_HOK_TDWRAPER_MACRO td_lst_por, KNL_HOK_TDFN_LST_POR

    KNL_HOK_TDWRAPER_MACRO td_ref_por, KNL_HOK_TDFN_REF_POR     
     
    /* 
     * Fixed Size Memory Pool SVC interface
     */
    KNL_HOK_TKWRAPER_MACRO tk_cre_mpf, KNL_HOK_TKFN_CRE_MPF

    KNL_HOK_TKWRAPER_MACRO tk_del_mpf, KNL_HOK_TKFN_DEL_MPF

    KNL_HOK_TKWRAPER_MACRO tk_get_mpf, KNL_HOK_TKFN_GET_MPF

    KNL_HOK_TKWRAPER_MACRO tk_ref_mpf, KNL_HOK_TKFN_REF_MPF

    KNL_HOK_TKWRAPER_MACRO tk_rel_mpf, KNL_HOK_TKFN_REL_MPF

    KNL_HOK_TDWRAPER_MACRO td_lst_mpf, KNL_HOK_TDFN_LST_MPF

    KNL_HOK_TDWRAPER_MACRO td_mpf_que, KNL_HOK_TDFN_MPF_QUE

    KNL_HOK_TDWRAPER_MACRO td_ref_mpf, KNL_HOK_TDFN_REF_MPF     
 
    /* 
     * Variable Size Memory Pool SVC interface
     */
    KNL_HOK_TKWRAPER_MACRO tk_cre_mpl, KNL_HOK_TKFN_CRE_MPL

    KNL_HOK_TKWRAPER_MACRO tk_del_mpl, KNL_HOK_TKFN_DEL_MPL

    KNL_HOK_TKWRAPER_MACRO tk_get_mpl, KNL_HOK_TKFN_GET_MPL

    KNL_HOK_TKWRAPER_MACRO tk_ref_mpl, KNL_HOK_TKFN_REF_MPL

    KNL_HOK_TKWRAPER_MACRO tk_rel_mpl, KNL_HOK_TKFN_REL_MPL

    KNL_HOK_TDWRAPER_MACRO td_lst_mpl, KNL_HOK_TDFN_LST_MPL

    KNL_HOK_TDWRAPER_MACRO td_mpl_que, KNL_HOK_TDFN_MPL_QUE

    KNL_HOK_TDWRAPER_MACRO td_ref_mpl, KNL_HOK_TDFN_REF_MPL     

    /* 
     * Time Management SVC interface
     */
    KNL_HOK_TKWRAPER_MACRO tk_get_etm, KNL_HOK_TKFN_GET_ETM

    KNL_HOK_TKWRAPER_MACRO tk_get_otm, KNL_HOK_TKFN_GET_OTM

    KNL_HOK_TKWRAPER_MACRO tk_get_tim, KNL_HOK_TKFN_GET_TIM

    KNL_HOK_TKWRAPER_MACRO tk_set_tim, KNL_HOK_TKFN_SET_TIM

    KNL_HOK_TDWRAPER_MACRO td_get_otm, KNL_HOK_TDFN_GET_OTM     
     
    KNL_HOK_TDWRAPER_MACRO td_get_tim, KNL_HOK_TDFN_GET_TIM
   
    /* 
     * Cyclic Handler SVC interface
     */
    KNL_HOK_TKWRAPER_MACRO tk_cre_cyc, KNL_HOK_TKFN_CRE_CYC

    KNL_HOK_TKWRAPER_MACRO tk_del_cyc, KNL_HOK_TKFN_DEL_CYC

    KNL_HOK_TKWRAPER_MACRO tk_ref_cyc, KNL_HOK_TKFN_REF_CYC
    
    KNL_HOK_TKWRAPER_MACRO tk_sta_cyc, KNL_HOK_TKFN_STA_CYC

    KNL_HOK_TKWRAPER_MACRO tk_stp_cyc, KNL_HOK_TKFN_STP_CYC

    KNL_HOK_TDWRAPER_MACRO td_lst_cyc, KNL_HOK_TDFN_LST_CYC

    KNL_HOK_TDWRAPER_MACRO td_ref_cyc, KNL_HOK_TDFN_REF_CYC
    
    /* 
     * Alarm Handler SVC interface
     */
    KNL_HOK_TKWRAPER_MACRO tk_cre_alm, KNL_HOK_TKFN_CRE_ALM

    KNL_HOK_TKWRAPER_MACRO tk_del_alm, KNL_HOK_TKFN_DEL_ALM

    KNL_HOK_TKWRAPER_MACRO tk_sta_alm, KNL_HOK_TKFN_STA_ALM

    KNL_HOK_TKWRAPER_MACRO tk_stp_alm, KNL_HOK_TKFN_STP_ALM

    KNL_HOK_TKWRAPER_MACRO tk_ref_alm, KNL_HOK_TKFN_REF_ALM

    KNL_HOK_TDWRAPER_MACRO td_lst_alm, KNL_HOK_TDFN_LST_ALM

    KNL_HOK_TDWRAPER_MACRO td_ref_alm, KNL_HOK_TDFN_REF_ALM
    
    /* 
     * Interrupt Management SVC interface
     */    
    KNL_HOK_TKWRAPER_MACRO tk_def_int, KNL_HOK_TKFN_DEF_INT

    KNL_HOK_TKWRAPER_MACRO tk_ret_int, KNL_HOK_TKFN_RET_INT

    /* 
     * System Management SVC interface
     */  
    KNL_HOK_TKWRAPER_MACRO tk_rot_rdq, KNL_HOK_TKFN_ROT_RDQ

    KNL_HOK_TKWRAPER_MACRO tk_dis_dsp, KNL_HOK_TKFN_DIS_DSP

    KNL_HOK_TKWRAPER_MACRO tk_ena_dsp, KNL_HOK_TKFN_ENA_DSP

    KNL_HOK_TKWRAPER_MACRO tk_get_tid, KNL_HOK_TKFN_GET_TID

    KNL_HOK_TDWRAPER_MACRO tk_set_pow, KNL_HOK_TKFN_SET_POW

    KNL_HOK_TKWRAPER_MACRO tk_ref_sys, KNL_HOK_TKFN_REF_SYS

    KNL_HOK_TKWRAPER_MACRO tk_ref_ver, KNL_HOK_TKFN_REF_VER

    KNL_HOK_TDWRAPER_MACRO td_ref_dsname, KNL_HOK_TDFN_REF_DSNAME

    KNL_HOK_TDWRAPER_MACRO td_ref_sys, KNL_HOK_TDFN_REF_SYS
 
    KNL_HOK_TDWRAPER_MACRO td_set_dsname, KNL_HOK_TDFN_SET_DSNAME
    
    /* 
     * Hook Trace SVC interface
     */       
    KNL_HOK_TDWRAPER_MACRO td_hok_dsp, KNL_HOK_TDFN_HOK_DSP

    KNL_HOK_TDWRAPER_MACRO td_hok_int, KNL_HOK_TDFN_HOK_INT

    KNL_HOK_TDWRAPER_MACRO td_hok_svc, KNL_HOK_TDFN_HOK_SVC
     
#endif

    End
