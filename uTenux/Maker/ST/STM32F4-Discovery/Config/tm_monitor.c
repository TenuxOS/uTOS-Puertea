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
 *    CPU:        Cortex M
 *    RTOS:       uT/OS
 *    Version:    2.0.00
 *    Released by Tenux Open Source Society
 *                  (http://www.tenux.org).
 *
 *    File Name      : tm_monitor.c
 *    Create Date    : 2013/2/14-2014/4/1
 *    Author         : wangshb
 *    Description    : tm monitor communication routine.
 *----------------------------------------------------------------------------------------
 */

#include <tm_bsp.h>
#include <tm_monitor.h>

#if TK_USE_TM_MONITOR

/* Definition of normal control char */
#define TM_CHAR_NUL          0x00U               /* null character */
#define TM_CHAR_ETX          0x03U               /* end of text character */
#define TM_CHAR_CR           0x0DU               /* carriage return character */
#define TM_CHAR_LF           0x0AU               /* line feed character */

/*
 *    Function Name : tm_getchar
 *    Create Date   : 2013/2/18-2014/4/1
 *    Author        : wangshb
 *    Description   : disable interrupt,then receive char from console,util success
 *                    supported only on wait != 0 (polling not supported)
 *    Param         : int32_t wait: no used
 *    Return Code   : int32_t c: char to receive
 */
int32_t tm_getchar( int32_t wait )
{
    uint8_t c;
    uint32_t imask;

    /* Disable interrupt */
    imask = __get_PRIMASK();
    __set_PRIMASK( 0x1u );

    c = tm_recvuart();

    /* Enable interrupt */
    __set_PRIMASK( imask );

    return (int32_t)c;
}

/*
 *    Function Name : tm_getline
 *    Create Date   : 2013/2/18-2014/4/1
 *    Author        : wangshb
 *    Description   : disable interrupt,then receive a line from console,util success
 *                    special key is not supported
 *    Param         : uint8_t *buff: buffer to save one line chars
 *    Return Code   : int32_t len: line length to receive
 */
int32_t tm_getline( uint8_t *buff )
{
    int32_t len = 0;
    uint32_t imask;

    /* Disable interrupt */
    imask = __get_PRIMASK();
    __set_PRIMASK( 0x1u );

    for (;;) {
        *buff = tm_recvuart();
        tm_senduart(*buff);                      /* echo back */
        if ( *buff == TM_CHAR_CR ) {
            tm_senduart(TM_CHAR_LF);
            *buff = TM_CHAR_NUL;
        } else if ( *buff == TM_CHAR_ETX ) {
            *buff = TM_CHAR_NUL;
            len = -1;
        }
        if ( *buff == TM_CHAR_NUL ) {
            break;
        }
        len++;
        buff++;
    }

    /* Enable interrupt */
    __set_PRIMASK( imask );

    return len;
}
#endif /* TK_USE_TM_MONITOR */

/*
 *    Function Name : tm_monitor
 *    Create Date   : 2013/2/18-2013/4/26
 *    Author        : wangshb
 *    Description   : Empty loop
 *    Param         : none
 *    Return Code   : none
 */
void tm_monitor( void )
{
    for(;;) {
        ;
    }
}

#if TK_USE_TM_MONITOR
/*
 *    Function Name : tm_putchar
 *    Create Date   : 2013/2/18-2014/4/1
 *    Author        : wangshb
 *    Description   : send a char to console util success
 *                    Ctrl-C is not supported
 *    Param         : int32_t c: char to send
 *    Return Code   : int32_t len: not used
 */
int32_t tm_putchar( int32_t c )
{
    uint8_t buf = (uint8_t)c;
    uint32_t imask;

    /* Disable interrupt */
    imask = __get_PRIMASK();
    __set_PRIMASK( 0x1u );

    if (buf == TM_CHAR_LF) {
        tm_senduart(TM_CHAR_CR);
    }
    tm_senduart(buf);

    /* Enable interrupt */
    __set_PRIMASK( imask );

    return 0;
}

/*
 *    Function Name : tm_putstring
 *    Create Date   : 2013/2/18-2014/4/1
 *    Author        : wangshb
 *    Description   : send a string to console util success
 *                    Ctrl-C is not supported
 *    Param         : const uint8_t *buff: string to send
 *    Return Code   : int32_t len: not used
 */
int32_t tm_putstring( const uint8_t *buff )
{
    uint32_t imask;

    /* Disable interrupt */
    imask = __get_PRIMASK();
    __set_PRIMASK( 0x1u );

    while ( *buff != TM_CHAR_NUL ) {
        if (*buff == TM_CHAR_LF) {
            tm_senduart(TM_CHAR_CR);
        } 
        tm_senduart(*buff);
        buff++;
    }

    /* Enable interrupt */
    __set_PRIMASK( imask );

    return 0;
}
#endif /* TK_USE_TM_MONITOR */
