/*
 *----------------------------------------------------------------------------------------
 *    Kernel/que
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
 *    File Name      : knl_que.h
 *    Create Date    : 2012/7/9-2014/4/1
 *    Author         : WangShb
 *    Description    : Queuing operation routine head file.
 *----------------------------------------------------------------------------------------
 */

#ifndef __KNL_QUE_H__
#define __KNL_QUE_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef Assembler

/*
 * Double-link queue (ring)
 */
typedef struct ring_queue {
        struct ring_queue *  next;
        struct ring_queue *  prev;
}                            QUEUE;

/*
 *    Description   : Queue initialization.
 *    Param         : QUEUE *que: queue pointer to init
 *    Return Code   : void
 */
Inline void knl_que_init( QUEUE *que )
{
    que->next = que;
    que->prev = que;
}

/*
 *    Description   : Check queue state
 *    Param         : QUEUE *que: queue pointer to judge
*     Return Code   : bool8_t: true if the queue is empty.
 */
Inline bool8_t knl_que_isempty( QUEUE *que )
{
    int8_t  b;
    
    b = ( que->next == que )? true: false;
    return (bool8_t)b;
}

/*
 *    Description   : Inserts entry directly prior to que.
 *    Param         : QUEUE *entry: queue pointer to insert
 *                    QUEUE *que: queue pointer be inserted
 *    Return Code   : void
 */
Inline void knl_que_insert( QUEUE *entry, QUEUE *que )
{
    entry->prev = que->prev;
    entry->next = que;
    que->prev->next = entry;
    que->prev = entry;
}

/*
 *    Description   : Deletes entry from queue.
 *                    No action is performed if entry is empty.
 *    Param         : QUEUE *entry: entry pointer to delete
 *    Return Code   : void
 */
Inline void knl_que_remove( QUEUE *entry )
{
    if ( entry->next != entry ) {
        entry->prev->next = entry->next;
        entry->next->prev = entry->prev;
    }
}

/*
 *    Description   : Deletes the entry directly after que from the queue.
 *                    and returns the deleted entry.
 *    Param         : QUEUE *que: queue pointer
 *    Return Code   : QUEUE *entry: deleted entry pointer,NULL if que is empty.
 */
Inline QUEUE* knl_que_removenext( QUEUE *que )
{
    QUEUE *entry;

    if ( que->next == que ) {
        return NULL;
    }

    entry = que->next;
    que->next = entry->next;
    entry->next->prev = que;

    return entry;
}

#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* __KNL_QUE_H__ */
