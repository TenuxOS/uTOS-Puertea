/*
 *----------------------------------------------------------------------------------------
 *    Kernel/mem
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
 *    File Name      : knl_mem.c
 *    Create Date    : 2012/9/4-2014/4/1
 *    Author         : wangshb
 *    Description    : Memory allocation management routine function.
 *----------------------------------------------------------------------------------------
 */

#include "knl_mem.h"

#if TK_USE_MALLOC
/*
 * System management memory area definition
 */
Zeroinit VP knl_mem_sysareatop;                  /* Top of area (Low address) */
Zeroinit VP knl_mem_sysareaend;                  /* End of area (High address) */

/*
 * Memory allocation control block pointer definition
 */
#ifdef KNL_CFG_USE_FUNC_KNL_MEM_IMACB
static IMACB *knl_mem_imacb = NULL;
#endif /* KNL_CFG_USE_FUNC_KNL_MEM_IMACB */
#endif

#ifdef KNL_CFG_USE_FUNC_KNL_MEM_APPENDFREEAREA
/*
 *    Function Name : knl_mem_appendfreearea
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Registration of free area on FreeQue
 *                     FreeQue is composed of 2 types: Queue that links the
 *                     different size of areas by size and queue that links the
 *                     same size of areas.
 *
 *                     freeque
 *                     |
 *                     |   +-----------------------+     +-----------------------+
 *                     |   | AreaQue               |     | AreaQue               |
 *                     |   +-----------------------+     +-----------------------+
 *                     *---> FreeQue Size order    |  *----> FreeQue Same size   ----->
 *                     |   | FreeQue Same size   -----*  | EmptyQue              |
 *                     |   |                       |     |                       |
 *                     |   |                       |     |                       |
 *                     |   +-----------------------+     +-----------------------+
 *                     |   | AreaQue               |     | AreaQue               |
 *                     |   +-----------------------+     +-----------------------+
 *    Param         : IMACB *imacb : Memory allocation control block
 *                    QUEUE *aq : area
 *    Return Code   : void 
 */
void knl_mem_appendfreearea( IMACB *imacb, QUEUE *aq )
{
    QUEUE *fq;
    int32_t size = KNL_MEM_AREASIZE(aq);

    /* Registration position search */
    /*  Search the free area whose size is equal to 'blksz',
     *  or larger than 'blksz' but closest.
     *  If it does not exist, return '&imacb->freeque'.
     */
    fq = knl_mem_searchfreearea(imacb, size);

    /* Register */
    knl_mem_clrareaflag(aq, KNL_MEM_AREA_USE);
    if ( ( fq != &imacb->freeque ) && ( KNL_MEM_FREESIZE(fq) == size ) ) {
        (aq + 2)->next = (fq + 1)->next;
        (fq + 1)->next = aq + 2;
        (aq + 2)->prev = fq + 1;
        (aq + 2)->next->prev = aq + 2;
        (aq + 1)->next = NULL;
    } else {
        knl_que_insert(aq + 1, fq);
        (aq + 2)->next = NULL;
        (aq + 2)->prev = (QUEUE*)size;
    }
}
#endif /* KNL_CFG_USE_FUNC_KNL_MEM_APPENDFREEAREA */

#ifdef KNL_CFG_USE_FUNC_KNL_MEM_INSERTAREAQUE
/*
 *    Function Name : knl_mem_insertareaque
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Register area, Insert 'ent' just after 'que.'
 *    Param         : QUEUE *que  : area 
 *                    QUEUE *ent : area to insert
 *    Return Code   : void 
 */
void knl_mem_insertareaque( QUEUE *que, QUEUE *ent )
{
    ent->prev = que;
    ent->next = que->next;
    que->next->prev = knl_mem_assign(que->next->prev, ent);
    que->next = ent;
}
#endif /* KNL_CFG_USE_FUNC_KNL_MEM_INSERTAREAQUE */

#ifdef KNL_CFG_USE_FUNC_KNL_MEM_AREAQUEREMOVE
/*
 *    Function Name : knl_mem_removeareaque
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Delete area
 *    Param         : QUEUE *fq  : area to delete
 *    Return Code   : void 
 */
void knl_mem_removeareaque( QUEUE *aq )
{
    knl_mem_mask(aq->prev)->next = aq->next;
    aq->next->prev = knl_mem_assign(aq->next->prev, knl_mem_mask(aq->prev));
}
#endif /* KNL_CFG_USE_FUNC_KNL_MEM_AREAQUEREMOVE */

#ifdef KNL_CFG_USE_FUNC_KNL_MEM_REMOVEFREEQUE
/*
 *    Function Name : knl_mem_removefreeque
 *    Create Date   : 2012/9/4-2014/2/1
 *    Author        : wangshb
 *    Description   : Delete from FreeQue
 *    Param         : QUEUE *fq  : free area to delete
 *    Return Code   : void 
 */
void knl_mem_removefreeque( QUEUE *fq )
{
    if ( fq->next == NULL ) {
        (fq + 1)->prev->next = (fq + 1)->next;
        if ( (fq + 1)->next != NULL ) {
            (fq + 1)->next->prev = (fq + 1)->prev;
        }
    } else {
        if ( (fq + 1)->next != NULL ) {
            knl_que_insert((fq + 1)->next - 1, fq);
            (fq + 1)->next->prev = (fq + 1)->prev;
        }
        knl_que_remove(fq);
    }
}
#endif /* KNL_CFG_USE_FUNC_KNL_MEM_REMOVEFREEQUE */

#ifdef KNL_CFG_USE_FUNC_KNL_MEM_SEARCHFREEAREA
/*
 *    Function Name : knl_mem_searchfreearea
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : FreeQue search
 *                      Search the free area whose size is equal to 'blksz',
 *                      or larger than 'blksz' but closest.
 *                      If it does not exist, return '&imacb->freeque'.
 *    Param         : IMACB *imacb : Memory allocation control block
 *                    int32_t blksz : block size to search
 *    Return Code   : QUEUE* : free area 
 */
QUEUE* knl_mem_searchfreearea( IMACB *imacb, int32_t blksz )
{
    int32_t fsz = 0;
    QUEUE *q = &imacb->freeque;

    /* For area whose memory pool size is less than 1/4,
       search from smaller size.
       Otherwise, search from larger size. */
    if ( blksz > ( imacb->memsz / 4 ) ) {
        /* Search from larger size. */
        while ( (q = q->prev) != &imacb->freeque ) {
            fsz = KNL_MEM_FREESIZE(q);
            if ( fsz <= blksz ) {
                return ( fsz < blksz )? q->next: q;
            }
        }
        return ( fsz >= blksz )? q->next: q;
    } else {
        /* Search from smaller size. */
        while ( (q = q->next) != &imacb->freeque ) {
            fsz = KNL_MEM_FREESIZE(q);
            if ( fsz >= blksz ) {
                break;
            }
        }
        return q;
    }
}
#endif /* KNL_CFG_USE_FUNC_KNL_MEM_SEARCHFREEAREA */

#if TK_USE_MALLOC

#ifdef KNL_CFG_USE_FUNC_KNL_MEM_ICALLOC
/*
 *    Function Name : knl_mem_icalloc
 *    Create Date   : 2012/9/4-2014/2/1
 *    Author        : wangshb
 *    Description   : Get nmemb*size bytes memory block from free area queue
 *    Param         : size_t nmemb: bytes per unit
 *                    size_t size:  unit count
 *    Return Code   : void*: pointer to memory block
 */
void* knl_mem_icalloc( size_t nmemb, size_t size )
{
    void *mem;
    size_t sz = nmemb * size;

    mem = knl_mem_imalloc(sz);
    if ( mem == NULL ) {
        return NULL;
    }

    memset(mem, 0, sz);

    return mem;
}
#endif /* KNL_CFG_USE_FUNC_KNL_MEM_ICALLOC */

#ifdef KNL_CFG_USE_FUNC_KNL_MEM_IFREE
/*
 *    Function Name : knl_mem_ifree
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Free memory,It may be called during interrupt disable. In this case,
 *                    need to wait until interrupt is enabled and until free.
 *    Param         : void *ptr: pointer to memory block
 *    Return Code   : void
 */
void  knl_mem_ifree( void *ptr )
{
    QUEUE *aq;
    uint32_t imask;

    DI(imask);                                   /* Exclusive by interrupt disable */

    aq = (QUEUE*)ptr - 1;
    knl_mem_clrareaflag(aq, KNL_MEM_AREA_USE);

    if ( !knl_mem_chkareaflag(aq->next, KNL_MEM_AREA_USE) ) {
        /* Merge with free area in after location */
        knl_mem_removefreeque(aq->next + 1);
        knl_mem_removeareaque(aq->next);
    }

    if ( !knl_mem_chkareaflag(aq->prev, KNL_MEM_AREA_USE) ) {
        /* Merge with free area in front location */
        aq = aq->prev;
        knl_mem_removefreeque(aq + 1);
        knl_mem_removeareaque(aq->next);
    }

    knl_mem_appendfreearea(knl_mem_imacb, aq);

    EI(imask);
}
#endif /* KNL_CFG_USE_FUNC_KNL_MEM_IFREE */

#ifdef KNL_CFG_USE_FUNC_KNL_MEM_IMALLOC
/*
 *    Function Name : knl_mem_imalloc
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Get memory from free area queue
 *    Param         : size_t size: memory block size
 *    Return Code   : void*: pointer to memory block
 */
void* knl_mem_imalloc( size_t size )
{
    QUEUE *q, *aq, *aq2;
    uint32_t imask;

    /* If it is smaller than the minimum fragment size,allocate the minimum size to it. */
    if ( size < KNL_MEM_MIN_FRAGMENT ) {
        size = KNL_MEM_MIN_FRAGMENT;
    } else {
        size = KNL_MEM_ROUND(size);
    }

    DI(imask);                                   /* Exclusive by interrupt disable */

    /* Search FreeQue */
    q = knl_mem_searchfreearea(knl_mem_imacb, (int32_t)size);
    if ( q == &(knl_mem_imacb->freeque) ) {
        q = NULL;                                /* Insufficient memory */
        goto error_exit;
    }

    /* There is free area: Split from FreeQue once */
    knl_mem_removefreeque(q);

    aq = q - 1;

    /* If there are fragments smaller than the minimum fragment size,allocate them also */
    if ( (uint32_t)KNL_MEM_FREESIZE(q) - size >= KNL_MEM_MIN_FRAGMENT + sizeof(QUEUE) ) {

        /* Divide area into 2 */
        aq2 = (QUEUE*)((uint8_t*)(aq + 1) + size);
        knl_mem_insertareaque(aq, aq2);

        /* Register remaining area to FreeQue */
        knl_mem_appendfreearea(knl_mem_imacb, aq2);
    }
    knl_mem_setareaflag(aq, KNL_MEM_AREA_USE);

error_exit:
    EI(imask);

    return (VP)q;
}
#endif /* KNL_CFG_USE_FUNC_KNL_MEM_IMALLOC */

#ifdef KNL_CFG_USE_FUNC_KNL_MEM_INIT
/*
 *    Function Name : knl_mem_init
 *    Create Date   : 2012/9/4-2014/4/1
 *    Author        : wangshb
 *    Description   : Initial system memroy control block-IMACB
 *    Param         : void
 *    Return Code   : ER ercd: E_OK if success
 */
ER knl_mem_init( void )
{
    uint32_t tmp;
    VP  memend;
    QUEUE  *aqtop, *aqend;

    /* Acquire system configuration definition information */
    memend = (VP)TK_RAM_TOTALAREA_END;
    if ( (uint32_t)memend > (uint32_t)knl_mem_sysareaend ) {
        memend = knl_mem_sysareaend;
    }

    /* Align top with 8 byte unit alignment */
    tmp = ((uint32_t)knl_mem_sysareatop + 7u) & ~0x00000007u;
    knl_mem_sysareatop = (VP)tmp;
    
    knl_mem_imacb = (IMACB*)knl_mem_sysareatop;
    
    tmp = (uint32_t)knl_mem_sysareatop + sizeof(IMACB);
    knl_mem_sysareatop = (VP)tmp;
    
    aqtop = (QUEUE*)knl_mem_sysareatop;
    
    tmp = (uint32_t)memend - (uint32_t)knl_mem_sysareatop - sizeof(QUEUE) * 2u;
    knl_mem_imacb->memsz = (int32_t)tmp;

    knl_mem_sysareatop = memend;                 /* Update memory free space */

    knl_que_init(&(knl_mem_imacb->areaque));
    knl_que_init(&(knl_mem_imacb->freeque));

    /* Register on AreaQue */
    aqend = (QUEUE*)((uint8_t*)aqtop + knl_mem_imacb->memsz) + 1;
    knl_mem_insertareaque(&knl_mem_imacb->areaque, aqend);
    knl_mem_insertareaque(&knl_mem_imacb->areaque, aqtop);
    knl_mem_setareaflag(aqend, KNL_MEM_AREA_USE);
    knl_mem_setareaflag(&knl_mem_imacb->areaque, KNL_MEM_AREA_USE);

    knl_mem_appendfreearea(knl_mem_imacb, aqtop);

    return E_OK;
}
#endif /* KNL_CFG_USE_FUNC_KNL_MEM_INIT */

#endif /* TK_USE_MALLOC */
