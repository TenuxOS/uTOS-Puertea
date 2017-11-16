/*
 *----------------------------------------------------------------------------------------
 *    TM
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
 *    File Name      : tm_monitor.h
 *    Create Date    : 2013/2/9-2014/2/28
 *    Author         : WangShb
 *    Description    : T-Monitor equivalent headfile.
 *----------------------------------------------------------------------------------------
 */

#ifndef __TM_MONITOR_H__
#define __TM_MONITOR_H__

#include <tk_kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef Assembler

/*
 * Monitor service function
 */

/*
 *    Description   : disable interrupt,then receive char from console,util success
 *                    supported only on wait != 0 (polling not supported)
 *    Param         : int32_t wait: no used
 *    Return Code   : int32_t c: char to receive
 */
extern int32_t tm_getchar( int32_t wait );

/*
 *    Description   : disable interrupt,then receive a line from console,util success
 *                    special key is not supported
 *    Param         : uint8_t *buff: buffer to save one line chars
 *    Return Code   : int32_t len: line length to receive
 */
extern int32_t tm_getline( uint8_t *buff );

/*
 *    Description   : Empty loop
 *    Param         : none
 *    Return Code   : none
 */
extern void tm_monitor( void );

/*
 *    Description   : send a char to console util success
 *                    Ctrl-C is not supported
 *    Param         : int32_t c: char to send
 *    Return Code   : int32_t len: not used
 */
extern int32_t tm_putchar( int32_t c );

/*
 *    Description   : send a string to console util success
 *                    Ctrl-C is not supported
 *    Param         : const uint8_t *buff: string to send
 *    Return Code   : int32_t len: not used
 */
extern int32_t tm_putstring( const uint8_t *buff );

#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* __TM_MONITOR_H__ */
