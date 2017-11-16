/*
 *----------------------------------------------------------------------------------------
 *    TM
 *
 *    Copyright (C) 2008-2013 by Dalian uLoong Co.,Ltd. All rights reserved.
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
 *    CPU:        STM32L152RB
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tm_pow.h
 *    Create Date    : 2014/05/07-2014/05/25
 *    Author         : Cuihl-wangshb
 *    Description    : Power entry Definition about chip.
 *----------------------------------------------------------------------------------------
 */

#ifndef __TM_POW_H__
#define __TM_POW_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 *    Description   : enter sleep mode preprocess function.
 *    Param         : none
 *    Return Code   : none
 */
extern void tm_pow_lowenter(void);

/*
 *    Description   : leave sleep mode postprocess function.
 *    Param         : none
 *    Return Code   : none
 */
extern void tm_pow_lowleave(void);
    
/*
 *    Description   : enter stop mode preprocess function.
 *    Param         : none
 *    Return Code   : none
 */
extern void tm_pow_offenter(void);

/*
 *    Description   : leave stop mode postprocess function.
 *    Param         : none
 *    Return Code   : none
 */
extern void tm_pow_offleave(void);

#ifdef __cplusplus
}
#endif

#endif /* __TM_POW_H__ */
