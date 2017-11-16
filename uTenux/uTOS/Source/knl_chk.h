/*
 *----------------------------------------------------------------------------------------
 *    Kernel/chk
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
 *    File Name      : knl_chk.h
 *    Create Date    : 2012/9/4-2014/4/1
 *    Author         : wangshb
 *    Description    : Macro for Error Check head file.
 *----------------------------------------------------------------------------------------
 */

#ifndef _KNL_CHK_H__
#define _KNL_CHK_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef Assembler

/*
 * Check object ID range (E_ID)
 */
#if KNL_CFG_CHK_ID
#define KNL_CHK_TSKID(tskid) {                                                           \
    if ( ( !KNL_TSK_IN_INDP() ) && ( (tskid) == TSK_SELF) ) {                                   \
        return E_OBJ;                                                                    \
    } else if (!KNL_CFG_CHK_TSKID(tskid)) {                                              \
        return E_ID;                                                                     \
    }                                                                                    \
}
#define KNL_CHK_TSKID_SELF(tskid) {                                                      \
    if ( !( ( ( !KNL_TSK_IN_INDP() ) && ( (tskid) == TSK_SELF ) )                        \
            || KNL_CFG_CHK_TSKID(tskid) ) ) {                                            \
        return E_ID;                                                                     \
    }                                                                                    \
}
#define KNL_CHK_SEMID(semid) {                                                           \
    if ( !KNL_CFG_CHK_SEMID(semid) ) {                                                   \
        return E_ID;                                                                     \
    }                                                                                    \
}
#define KNL_CHK_FLGID(flgid) {                                                           \
    if ( !KNL_CFG_CHK_FLGID(flgid) ) {                                                   \
        return E_ID;                                                                     \
    }                                                                                    \
}
#define KNL_CHK_MBXID(mbxid) {                                                           \
    if ( !KNL_CFG_CHK_MBXID(mbxid) ) {                                                   \
        return E_ID;                                                                     \
    }                                                                                    \
}
#define KNL_CHK_MBFID(mbfid) {                                                           \
    if ( !KNL_CFG_CHK_MBFID(mbfid) ) {                                                   \
        return E_ID;                                                                     \
    }                                                                                    \
}
#define KNL_CHK_PORID(porid) {                                                           \
    if ( !KNL_CFG_CHK_PORID(porid) ) {                                                   \
        return E_ID;                                                                     \
    }                                                                                    \
}
#define KNL_CHK_MTXID(pisid) {                                                           \
    if ( !KNL_CFG_CHK_MTXID(pisid) ) {                                                   \
        return E_ID;                                                                     \
    }                                                                                    \
}
#define KNL_CHK_MPLID(mplid) {                                                           \
    if ( !KNL_CFG_CHK_MPLID(mplid) ) {                                                   \
        return E_ID;                                                                     \
    }                                                                                    \
}
#define KNL_CHK_MPFID(mpfid) {                                                           \
    if ( !KNL_CFG_CHK_MPFID(mpfid) ) {                                                   \
        return E_ID;                                                                     \
    }                                                                                    \
}
#define KNL_CHK_CYCID(cycid) {                                                           \
    if ( !KNL_CFG_CHK_CYCID(cycid) ) {                                                   \
        return E_ID;                                                                     \
    }                                                                                    \
}
#define KNL_CHK_ALMID(almid) {                                                           \
    if ( !KNL_CFG_CHK_ALMID(almid) ) {                                                   \
        return E_ID;                                                                     \
    }                                                                                    \
}
#else /* KNL_CFG_CHK_ID */
#define KNL_CHK_TSKID(tskid)
#define KNL_CHK_TSKID_SELF(tskid)
#define KNL_CHK_SEMID(semid)
#define KNL_CHK_FLGID(flgid)
#define KNL_CHK_MBXID(mbxid)
#define KNL_CHK_MBFID(mbfid)
#define KNL_CHK_PORID(porid)
#define KNL_CHK_MTXID(pisid)
#define KNL_CHK_MPLID(mplid)
#define KNL_CHK_MPFID(mpfid)
#define KNL_CHK_CYCID(cycid)
#define KNL_CHK_ALMID(almid)
#endif /* KNL_CFG_CHK_ID */

/*
 * Check whether its own task is specified (E_OBJ)
 */
#if KNL_CFG_CHK_SELF
#define KNL_CHK_NONSELF(tid) {                                                           \
    if ( ( !KNL_TSK_IN_INDP() ) && ( (tid) == knl_tsk_execution->tskid ) ) {             \
        return E_OBJ;                                                                    \
    }                                                                                    \
}
#else /* KNL_CFG_CHK_SELF */
#define KNL_CHK_NONSELF(tid)
#endif /* KNL_CFG_CHK_SELF */

/*
 * Check task priority value (E_PAR)
 */
#if KNL_CFG_CHK_PAR
#define KNL_CHK_PRI(pri) {                                                               \
    if ( !KNL_CFG_CHK_PRI(pri) ) {                                                       \
        return E_PAR;                                                                    \
    }                                                                                    \
}
#define KNL_CHK_PRI_INI(pri) {                                                           \
    if ( ( (pri) != TPRI_INI ) && ( !KNL_CFG_CHK_PRI(pri) ) ) {                          \
        return E_PAR;                                                                    \
    }                                                                                    \
}
#define KNL_CHK_PRI_RUN(pri) {                                                           \
    if ( ( (pri) != TPRI_RUN ) && ( !KNL_CFG_CHK_PRI(pri) ) ) {                          \
        return E_PAR;                                                                    \
    }                                                                                    \
}
#else /* KNL_CFG_CHK_PAR */
#define KNL_CHK_PRI(pri)
#define KNL_CHK_PRI_INI(pri)
#define KNL_CHK_PRI_RUN(pri)
#endif /* KNL_CFG_CHK_PAR */

/*
 * Check timeout specification value (E_PAR)
 */
#if KNL_CFG_CHK_PAR
#define KNL_CHK_TMOUT(tmout) {                                                           \
    if ( !( (tmout) >= TMO_FEVR ) ) {                                                    \
        return E_PAR;                                                                    \
    }                                                                                    \
}
#else /* KNL_CFG_CHK_PAR */
#define KNL_CHK_TMOUT(tmout)
#endif /* KNL_CFG_CHK_PAR */

/*
 * Check other parameter errors (E_PAR)
 */
#if KNL_CFG_CHK_PAR
#define KNL_CHK_PAR(exp) {                                                               \
    if ( !(exp) ) {                                                                      \
        return E_PAR;                                                                    \
    }                                                                                    \
}
#else /* KNL_CFG_CHK_PAR */
#define KNL_CHK_PAR(exp)
#endif /* KNL_CFG_CHK_PAR */

/*
 * Check reservation attribute error (E_RSATR)
 */
#if KNL_CFG_CHK_RSATR
#define KNL_CHK_RSATR(atr, maxatr) {                                                     \
    if ( ( (atr) & ~(maxatr) ) != 0u ) {                                                 \
            return E_RSATR;                                                              \
    }                                                                                    \
}
#else /* KNL_CFG_CHK_RSATR */
#define KNL_CHK_RSATR(atr, maxatr)
#endif /* KNL_CFG_CHK_RSATR */

/*
 * Check unsupported function (E_NOSPT)
 */
#if KNL_CFG_CHK_NOSPT
#define KNL_CHK_NOSPT(exp) {                                                             \
    if ( !(exp) ) {                                                                      \
        return E_NOSPT;                                                                  \
    }                                                                                    \
}
#else /* KNL_CFG_CHK_NOSPT */
#define KNL_CHK_NOSPT(exp)
#endif /* KNL_CFG_CHK_NOSPT */

/*
 * Check whether task-independent part is running (E_CTX)
 */
#if KNL_CFG_CHK_CTX
#define KNL_CHK_INTSK() {                                                                \
    if ( KNL_TSK_IN_INDP() ) {                                                           \
        return E_CTX;                                                                    \
    }                                                                                    \
}
#else /* KNL_CFG_CHK_CTX */
#define KNL_CHK_INTSK()
#endif /* KNL_CFG_CHK_CTX */

/*
 * Check whether dispatch is in disabled state (E_CTX)
 */
#if KNL_CFG_CHK_CTX
#define KNL_CHK_DISPATCH() {                                                             \
    if ( KNL_TSK_IN_DDSP() ) {                                                           \
        return E_CTX;                                                                    \
    }                                                                                    \
}
#define KNL_CHK_DISPATCH_POL(tmout) {                                                    \
    if ( KNL_TSK_IN_DDSP() && ( (tmout) != TMO_POL ) ) {                                 \
        return E_CTX;                                                                    \
    }                                                                                    \
}
#else /* KNL_CFG_CHK_CTX */
#define KNL_CHK_DISPATCH()
#define KNL_CHK_DISPATCH_POL(tmout)
#endif /* KNL_CFG_CHK_CTX */

/*
 * Check other context errors (E_CTX)
 */
#if KNL_CFG_CHK_CTX
#define KNL_CHK_CTX(exp) {                                                               \
    if ( !(exp) ) {                                                                      \
        return E_CTX;                                                                    \
    }                                                                                    \
}
#else /* KNL_CFG_CHK_CTX */
#define KNL_CHK_CTX(exp)
#endif /* KNL_CFG_CHK_CTX */

#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* _KNL_CHK_H__ */
