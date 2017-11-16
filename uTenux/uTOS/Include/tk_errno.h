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
 *    File Name      : tk_errno.h
 *    Create Date    : 2013/2/16-2013/12/22
 *    Author         : WangShb
 *    Description    : uT/OS Standard Error Code definitions.
 *----------------------------------------------------------------------------------------
 */

#ifndef __TK_ERRNO_H__
#define __TK_ERRNO_H__

#ifdef __cplusplus
extern "C" {
#endif

#define E_OK                 (0)                 /* Completed successfully */

#define E_SYS                (-5)                /* System error */
#define E_NOCOP              (-6)                /* Coprocessor disable */
#define E_NOSPT              (-9)                /* Unsupported function */
#define E_RSFN               (-10)               /* Reserved function code number */
#define E_RSATR              (-11)               /* Reserved attribute */
#define E_PAR                (-17)               /* Parameter error */
#define E_ID                 (-18)               /* Incorrect ID number */
#define E_CTX                (-25)               /* Context error */
#define E_MACV               (-26)               /* Inaccessible memory/access violation*/
#define E_OACV               (-27)               /* Object access violation */
#define E_ILUSE              (-28)               /* Incorrect system call use */
#define E_NOMEM              (-33)               /* Insufficient memory */
#define E_LIMIT              (-34)               /* Exceed system limits */
#define E_OBJ                (-41)               /* Incorrect object state */
#define E_NOEXS              (-42)               /* Object does not exist */
#define E_QOVR               (-43)               /* Queuing overflow */
#define E_RLWAI              (-49)               /* Forcibly release wait state */
#define E_TMOUT              (-50)               /* Polling fail/time out */
#define E_DLT                (-51)               /* Waited object was deleted */
#define E_DISWAI             (-52)               /* Release wait caused by wait disable */

#define E_IO                 (-57)               /* Output/input error */
#define E_NOMDA              (-58)               /* No media */
#define E_BUSY               (-65)               /* Busy state */
#define E_ABORT              (-66)               /* Aborted */
#define E_RONLY              (-67)               /* Write protected */

#ifdef __cplusplus
}
#endif

#endif /* __TK_ERRNO_H__ */
