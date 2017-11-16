/*
 *----------------------------------------------------------------------------------------
 *    App
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
 *    File Name      : usermain.c
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : Wangshb
 *    Description    : Entry module for the user application.
 *----------------------------------------------------------------------------------------
 */

#include <tm_bsp.h>
#include <tk_kernel.h>
#include <tm_monitor.h>

/*
 * Declaration of local function prototype  
 */
static void inittask(int32_t stacd,VP exinf);
static int32_t usermain( void );

/*
 *    Function Name : main
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Entry for initialize bsp and starting uT/OS.
 *                    Called by Cortex M boot code for example startup.S.
 *    Param         : none
 *    Return Code   : int32_t
 */
int32_t main( void )
{
    T_CTSK initctsk;

    /* Initialize bsp sequence */
    tm_initbsp();

    /* Initial task define parameter */
    initctsk.exinf   = NULL;
    initctsk.tskatr  = TA_HLNG | TA_RNG0;
    initctsk.task    = (FP)&inittask;
    initctsk.itskpri = TK_MAX_PRI-2u;
    initctsk.stksz   = 512;
    initctsk.bufptr  = NULL;
#if ( TK_USE_OBJ_NAME && ( TK_OBJ_NAME_LENGTH >= 8u ) )
    initctsk.tskatr  |= TA_DSNAME;
    strcpy((char*)&initctsk.dsname, "inittsk");
#endif
    
    /* Start uT/OS and Never return*/
    tk_sta_sys((T_CTSK *)&initctsk);

    return 0;
}

/*
 *    Function Name : inittask
 *    Create Date   : 2012/7/9-2013/12/22
 *    Author        : wangshb
 *    Description   : Entry for initialize task.
 *                    call usermain, stop uT/OS etc.
 *    Param         : int32_t stacd: task start code
 *                    VP exinf: task extend information
 *    Return Code   : none
 */
static void inittask(int32_t stacd,VP exinf)
{
    /* Perform user main */
    usermain();

    /* Stop uT/OS */
    tk_ext_sys();
}

/*
 *    Function Name : usermain
 *    Create Date   : 2012/7/9-2014/4/1
 *    Author        : wangshb
 *    Description   : Entry routine for the user application.
 *                    At this point, Initialize and start the user application.
 *                    Entry routine is called from the initial task for Kernel,
 *                    so system call for stopping the task should not be issued
 *                    from the contexts of entry routine.
 *                    We recommend that:
 *                    (1)'usermain()' only generates the user initial task.
 *                    (2)initialize and start the user application by the user initial task.
 *    Param         : none
 *    Return Code   : int32_t
 */
static int32_t usermain( void )
{
    ER ercd = E_OK;

#if TK_USE_MESSAGE
    tm_putstring((uint8_t*)&"Push any key to shutdown the micro Tenux.\n");
    tm_getchar(-1);
#endif

    return ercd;
}
