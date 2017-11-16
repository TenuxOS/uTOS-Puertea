/*
 *----------------------------------------------------------------------------------------
 *    Kernel/tck
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
 *    File Name      : knl_tick.h
 *    Create Date    : 2012/9/4-2014/5/4
 *    Author         : wangshb
 *    Description    : Kernel Tick routine head file.
 *----------------------------------------------------------------------------------------
 */

#ifndef _KNL_TCK_H__
#define _KNL_TCK_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef Assembler

                                                 /* Actual time */
#define KNL_TCK_REAL_TIME    ( ll_add(knl_tck_current_time, knl_tck_realtime_ofs) )  

typedef int64_t              LSYSTIM;            /* SYSTIM int. expression */

typedef void                 (*CBACK)(VP exinf); /* Type of callback function */

/*
 * Definition of tick management event block
 */
typedef struct tick_event_block {
        QUEUE                queue;              /* Tick event queue */
        LSYSTIM              time;               /* Event time */
        CBACK                callback;           /* Callback function */
        VP                   arg;                /* Argument to be sent to callback func*/
}                            TMEB;

/*
 * Current time (Software clock)
 */
extern LSYSTIM knl_tck_current_time;             /* System operation time */
extern LSYSTIM knl_tck_realtime_ofs;             /* Difference from actual time */

/*
 * Tick event queue
 */
extern QUEUE knl_tck_queue;

/*
 *    Description   : Delete from timer queue
 *    Param         : TMEB *event : tick event
 *    Return Code   : void
 */
Inline void knl_tck_delete( TMEB *event )
{
    knl_que_remove(&event->queue);
}

/*
 *    Description   : Get Next Tick Event Relative Time
 *    Param         : void
 *    Return Code   : RELTIM : next tick event relative time
 */
Inline RELTIM knl_tck_getetim( void )
{
    RELTIM rtim;
    
    rtim = ll_lo(ll_sub( ((TMEB *)knl_tck_queue.next)->time,  knl_tck_current_time));
    return rtim;
}

/*
 *    Description   : Conversion from SYSTIM to LSYSTIM
 *    Param         : SYSTIM stime : SYSTIM expression
 *    Return Code   : LSYSTIM : LSYSTIM expression
 */
Inline LSYSTIM knl_tck_tolsystim( SYSTIM *stime )
{
    LSYSTIM ltime;

    ltime = ll_hilo(stime->hi, stime->lo);

    return ltime;
}

/*
 *    Description   : Conversion from LSYSTIM to SYSTIM
 *    Param         : LSYSTIM ltime : LSYSTIM expression
 *    Return Code   : SYSTIM : SYSTIM expression
 */
Inline SYSTIM knl_tck_tosystim( LSYSTIM ltime )
{
    SYSTIM stime;

    stime.hi = ll_hi(ltime);
    stime.lo = ll_lo(ltime);
    
    return stime;
}

/*
 *    Description   : Tick interrupt handler
 *                     Tick interrupt handler starts every TIMER_PERIOD millisecond
 *                     interval by hardware timer. Update the software clock and start 
 *                     the tick event upon arriving at start time.
 *    Param         : void
 *    Return Code   : void
 */
extern void knl_tck_handler( void );

/*
 *    Description   : Initialization of tick module
 *    Param         : void
 *    Return Code   : ER : Error code
 */
extern ER knl_tck_init( void );

/*
 *    Description   : Register tick event onto tick queue
 *                     Register the tick event 'event' onto the tick queue to start 
 *                     after the timeout 'tmout'. At timeout, start with the argument 
 *                     'arg' on the callback function 'callback'.
 *                     When 'tmout' is TMO_FEVR, do not register onto the tick queue, 
 *                     but initialize queue area in case 'tick_delete' is called later.
 *    Param         : TMEB *event : tick event
 *                    TMO tmout : timeout
 *                    CBACK callback : call back function
 *                    VP arg : argument of call back function
 *    Return Code   : 
 */
extern void knl_tck_insert( TMEB *event, TMO tmout, CBACK callback, VP arg );

/*
 *    Description   : Register tick event onto tick queue
 *                     Register the tick event 'event' onto the tick queue to start 
 *                     at the (absolute) time 'time'. At the time, start with the argument 
 *                     'arg' on the callback function 'callback'.
 *    Param         : TMEB *event : tick event
 *                    LSYSTIM time : system operation time, not an actual time.
 *                    CBACK callback : call back function
 *                    VP arg : argument of call back function
 *    Return Code   : void
 */
extern void knl_tck_insabs( TMEB *event, LSYSTIM time, CBACK callback, VP arg );

/*
 *    Description   : Register tick event onto tick queue
 *                     Register the tick event 'event' onto the tick queue to start 
 *                     after the timeout 'tmout'. At timeout, start with the argument 
 *                     'arg' on the callback function 'callback'.
 *    Param         : TMEB *event : tick event
 *                    RELTIM tmout : timeout
 *                    CBACK callback : call back function
 *                    VP arg : argument of call back function
 *    Return Code   : void
 */
extern void knl_tck_insrel( TMEB *event, RELTIM tmout, CBACK callback, VP arg );

/*
 *    Description   : Get Next Tick Event Relative Time to Caculate,then Reload timer
 *    Param         : void
 *    Return Code   : void
 */
extern void knl_tck_restart( void );

/*
 *    Description   : Get Next Tick Event Relative Time to Caculate,then Reload timer
 *    Param         : void
 *    Return Code   : void
 */
extern void knl_tck_restore(void );

/*
 *    Description   : Stop tick
 *    Param         : void
 *    Return Code   : void
 */
extern void knl_tck_shutdown( void );

#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* _KNL_TCK_H__ */
