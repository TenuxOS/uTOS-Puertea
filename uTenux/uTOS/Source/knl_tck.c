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
 *    File Name      : knl_tck.c
 *    Create Date    : 2012/9/4-2014/5/31
 *    Author         : wangshb
 *    Description    : Kernel Tick routine function.
 *----------------------------------------------------------------------------------------
 */

#include "knl_tmr.h"
#include "knl_tck.h"

/*
 * Current time (Software clock)
 *  'current_time' shows the total operation time since
 *  operating system Starts. 'real_time_ofs' shows difference
 *  between the current time and the operating system clock
 *  (current_time). Do not change 'current_time' when setting
 *  time by 'set_tim()'. Set 'real_time_ofs' with the time   
 *  difference between 'current_time' and setup time.
 *  Therefore 'current_time' does not affect with time change
 *  and it increases simply.
 */
Zeroinit LSYSTIM knl_tck_current_time;           /* System operation time */
Zeroinit LSYSTIM knl_tck_realtime_ofs;           /* Actual time - System operation time */

/* 
 * Tick event queue
 */
Zeroinit QUEUE knl_tck_queue;

/*
 *    Function Name : knl_tck_insque
 *    Create Date   : 2012/9/4-2014/2/1
 *    Author        : wangshb
 *    Description   : Insert tick event to tick event queue
 *    Param         : TMEB *event : tick event
 *    Return Code   : void
 */
static void knl_tck_insque( TMEB *event )
{
    QUEUE  *q;

    for ( q = knl_tck_queue.next; q != &knl_tck_queue; q = q->next ) {
        if ( ll_cmp(event->time, ((TMEB*)q)->time) < 0) {
            break;
        }
    }
    knl_que_insert(&event->queue, q);
}

/*
 *    Function Name : knl_tck_handler
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Tick interrupt handler
 *                     Tick interrupt handler starts every TIMER_PERIOD millisecond
 *                     interval by hardware timer. Update the software clock and start 
 *                     the tick event upon arriving at start time.
 *    Param         : void
 *    Return Code   : void
 */
void knl_tck_handler( void )
{
    TMEB  *event;

    knl_tmr_clrint();                            /* Clear timer interrupt */

    KNL_TSK_BEGIN_CRITICAL_SECTION
    knl_tck_current_time = ll_add(knl_tck_current_time, uitoll(KNL_CFG_TIMER_PERIOD));

#if TK_USE_DBGSPT && defined(USE_FUNC_TD_INF_TSK)
    if ( knl_tsk_execution != NULL ) {
        /* Task at execution */
        if ( knl_tsk_execution->sysmode > 0 ) {
            knl_tsk_execution->stime += KNL_CFG_TIMER_PERIOD;
        } else {
            knl_tsk_execution->utime += KNL_CFG_TIMER_PERIOD;
        }
    }
#endif

    /* Execute event that passed occurring time. */
    while ( !knl_que_isempty(&knl_tck_queue) ) {
        event = (TMEB*)knl_tck_queue.next;

        if ( ll_cmp(event->time, knl_tck_current_time) > 0 ) {
            break;
        }

        knl_que_remove(&event->queue);
        if ( event->callback != NULL ) {
            (*event->callback)(event->arg);
        }
    }

    KNL_TSK_END_CRITICAL_SECTION

    knl_tmr_endofint();                          /* Clear timer interrupt */
}

/*
 *    Function Name : knl_tck_init
 *    Create Date   : 2012/9/4-2014/2/1
 *    Author        : wangshb
 *    Description   : Initialization of tick module
 *    Param         : void
 *    Return Code   : ER : Error code
 */
ER knl_tck_init( void )
{
    if ( KNL_CFG_TIMER_PERIOD < KNL_TMR_PERIOD_MIN
      || KNL_CFG_TIMER_PERIOD > KNL_TMR_PERIOD_MAX ) {
        return E_SYS;
    }

    knl_tck_current_time = knl_tck_realtime_ofs = ltoll(0);
    knl_que_init(&knl_tck_queue);

    /* Start timer interrupt */
    knl_tmr_start();

    return E_OK;
}

/*
 *    Function Name : knl_tck_insert
 *    Create Date   : 2012/9/4-2014/2/1
 *    Author        : wangshb
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
 *    Return Code   : void
 */
void knl_tck_insert( TMEB *event, TMO tmout, CBACK callback, VP arg )
{
    event->callback = callback;
    event->arg = arg;

    if ( tmout == TMO_FEVR ) {
        knl_que_init(&event->queue);
    } else {
        /* To guarantee longer wait time specified by 'tmout', 
           add tick period on wait time */
        event->time = ll_add( ll_add(knl_tck_current_time, ltoll(tmout)),
                              uitoll(KNL_CFG_TIMER_PERIOD) );
        knl_tck_insque(event);
    }
}

/*
 *    Function Name : knl_tck_insabs
 *    Create Date   : 2012/9/4-2014/2/1
 *    Author        : wangshb
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
void knl_tck_insabs( TMEB *event, LSYSTIM time, CBACK callback, VP arg )
{
    event->callback = callback;
    event->arg = arg;
    event->time = time;
    knl_tck_insque(event);
}

/*
 *    Function Name : knl_tck_insrel
 *    Create Date   : 2012/9/4-2014/2/1
 *    Author        : wangshb
 *    Description   : Register tick event onto tick queue
 *                      Register the tick event 'event' onto the tick queue to start 
 *                      after the timeout 'tmout'. At timeout, start with the argument 
 *                      'arg' on the callback function 'callback'.
 *    Param         : TMEB *event : tick event
 *                    RELTIM tmout : timeout
 *                    CBACK callback : call back function
 *                    VP arg : argument of call back function
 *    Return Code   : void
 */
void knl_tck_insrel( TMEB *event, RELTIM tmout, CBACK callback, VP arg )
{
    event->callback = callback;
    event->arg = arg;

    /* To guarantee longer wait time specified by 'tmout',
       add TIMER_PERIOD on wait time */
    event->time = ll_add( ll_add(knl_tck_current_time, ultoll(tmout)),
                  uitoll(KNL_CFG_TIMER_PERIOD) );
    knl_tck_insque(event);
}

/*
 *    Function Name : knl_tck_restart
 *    Create Date   : 2014/5/4-2014/5/6
 *    Author        : wangshb
 *    Description   : Get Next Tick Event Relative Time to Caculate,then Reload timer
 *    Param         : void
 *    Return Code   : void
 */
void knl_tck_restart( void )
{
    RELTIM rtim;
    
    /* Get next tick event relative time */
    rtim = ll_lo( ll_sub( ll_sub( ((TMEB *)knl_tck_queue.next)->time,  knl_tck_current_time),
                  uitoll(KNL_CFG_TIMER_PERIOD) ) );
    knl_tmr_restart( rtim );
}

/*
 *    Function Name : knl_tck_restore
 *    Create Date   : 2014/5/4-2014/5/6
 *    Author        : wangshb
 *    Description   : Get Next Tick Event Relative Time to Caculate,then Reload timer
 *    Param         : void
 *    Return Code   : void
 */
void knl_tck_restore(void )
{
    RELTIM rtim;

    rtim = knl_tmr_restore();

    /* Add relative time from last tick interrupt to this tick or other interrupt */
    knl_tck_current_time = ll_add( knl_tck_current_time, uitoll(rtim) );
}

#if KNL_CFG_USE_CLEANUP
/*
 *    Function Name : knl_tck_shutdown
 *    Create Date   : 2012/9/4-2014/2/1
 *    Author        : wangshb
 *    Description   : Stop tick
 *    Param         : void
 *    Return Code   : void
 */
void knl_tck_shutdown( void )
{
    knl_tmr_terminate();
}
#endif /* KNL_CFG_USE_CLEANUP */
