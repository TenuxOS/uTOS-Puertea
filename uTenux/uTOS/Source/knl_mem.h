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
 *    File Name      : knl_mem.h
 *    Create Date    : 2012/9/4-2014/4/1
 *    Author         : wangshb
 *    Description    : Memory allocation management routine head file.
 *----------------------------------------------------------------------------------------
 */

#ifndef _KNL_MEM_H__
#define _KNL_MEM_H__

#include "knl_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef Assembler

/*
 *  Memory allocation control block
 *
 *  Order of members must not be changed because members are used with casting from MPLCB.
 *
 *  AreaQue for connecting each area where reserved pages are divided Sort in ascending
 *  order of addresses in a page.Do not sort between pages.
 *  FreeQue for connecting unused area in reserved pages Sort from small to large spaces.
 */
typedef struct {
        int32_t              memsz;
        QUEUE                areaque;
        QUEUE                freeque;
}                            IMACB;

/*
 * Compensation for aligning "&areaque" position to 2 bytes border
 */
#define KNL_MEM_ALIGNIMACB(imacb) ( (IMACB*)((uint32_t)(imacb) & ~0x00000001u) )

/* Calculate Area size */
#define KNL_MEM_AREASIZE(aq) ( (uint8_t*)(aq)->next - (uint8_t*)((aq) + 1) )

/* Calculate Free size */
#define KNL_MEM_FREESIZE(fq) ( (int32_t)((fq) + 1)->prev )

/* Minimum fragment size */
#define KNL_MEM_MIN_FRAGMENT ( sizeof(QUEUE) * 2u )

/* Flag that uses the lower bits of areaQue's 'prev' */
#define KNL_MEM_AREA_USE     0x00000001UL        /* In-use */
#define KNL_MEM_AREA_MASK    0x00000001UL

/*
 * Minimum unit of subdivision
 *  The lower 1 bit of address is always 0
 *  because memory is allocated by KNL_MEM_ROUNDUNIT.
 *  areaque uses the lower 1 bit for flag.
 */
#define KNL_MEM_ROUNDUNIT    ( sizeof(QUEUE) )   /* 8 bytes */
#define KNL_MEM_ROUND(sz)    ( ((uint32_t)(sz) + (uint32_t)(KNL_MEM_ROUNDUNIT - 1u)) & \
                               ~(uint32_t)(KNL_MEM_ROUNDUNIT-1u) )

/*
 * System management memory area definition
 */
extern VP knl_mem_sysareatop;                    /* Top of area (Low address) */
extern VP knl_mem_sysareaend;                    /* End of area (High address) */

/*
 *    Description   : Check prev pointer flag in areaQue of IMACB
 *    Param         : QUEUE* que: current areaQue's pointer
 *                    uint32_t f: flag to check
 *    Return Code   : bool8_t : if prev pointer flag bit is set, return true
 */
Inline bool8_t knl_mem_chkareaflag( QUEUE* que,  uint32_t f )
{
    int8_t  b;
    uint32_t  q;

    q = (uint32_t)((que)->prev) & f;
    b = ( q != 0u )? true: false;
    return (bool8_t)b;
}

/*
 *    Description   : Clear prev pointer flag in areaQue of IMACB
 *    Param         : QUEUE* que: current areaQue's pointer
 *                    uint32_t f: flag to clear
 *    Return Code   : void
 */
Inline void knl_mem_clrareaflag( QUEUE* que,  uint32_t f )
{
    uint32_t  q;
    
    q = (uint32_t)((que)->prev) & ~f;
    que->prev = (QUEUE*)q;
}

/*
 *    Description   : Set prev pointer flag in areaQue of IMACB
 *    Param         : QUEUE* que: current areaQue's pointer
 *                    uint32_t f: flag to set
 *    Return Code   : void
 */
Inline void knl_mem_setareaflag( QUEUE* que,  uint32_t f )
{
    uint32_t  q;
    
    q = (uint32_t)((que)->prev) | f;
    que->prev = (QUEUE*)q;
}

/*
 *    Description   : Assign in-use state flag in areaQue's prev pointer of IMACB
 *    Param         : QUEUE* xq: current areaQue's prev pointer
 *                    QUEUE* xq: another areaQue's prev pointer
 *    Return Code   : QUEUE*: areaQue's prev pointer
 */
Inline QUEUE* knl_mem_assign( QUEUE* xq,  QUEUE* yq )
{
    uint32_t  q;
    
    q = ( (uint32_t)xq & KNL_MEM_AREA_MASK ) | (uint32_t)yq;
    return (QUEUE*)q;
}

/*
 *    Description   : Clear in-use state flag in areaQue's prev pointer of IMACB
 *    Param         : QUEUE* que: areaQue's prev pointer with in-use state flag
 *    Return Code   : QUEUE*: areaQue's prev pointer without in-use state flag
 */
Inline QUEUE* knl_mem_mask( QUEUE* que )
{
    uint32_t  q;
    
    q = (uint32_t)(que) & ~KNL_MEM_AREA_MASK;
    return (QUEUE*)q;
}

/*
 *    Description   : Adjusting the size which can be allocated
 *    Param         : int32_t sz: memory block size
 *    Return Code   : int32_t: adjusted size
 */
Inline int32_t knl_mem_roundsize( int32_t sz )
{
    uint32_t adjsz;
    
    if ( sz < (int32_t)KNL_MEM_MIN_FRAGMENT ) {
        sz = (int32_t)KNL_MEM_MIN_FRAGMENT;
    }
    
    adjsz = KNL_MEM_ROUND(sz);
    return (int32_t)(adjsz);
}

/*
 *    Description   : Registration of free area on FreeQue
 *    Param         : IMACB *imacb : Memory allocation control block
 *                    QUEUE *aq : area
 *    Return Code   : void 
 */
extern void knl_mem_appendfreearea( IMACB *imacb, QUEUE *aq );

/*
 *    Description   : Register area, Insert 'ent' just after 'que.'
 *    Param         : QUEUE *que  : area 
 *                    QUEUE *ent : area to insert
 *    Return Code   : void 
 */
extern void knl_mem_insertareaque( QUEUE *que, QUEUE *ent );

/*
 *    Description   : Delete area
 *    Param         : QUEUE *fq  : area to delete
 *    Return Code   : void 
 */
extern void knl_mem_removeareaque( QUEUE *aq );

/*
 *    Description   : Delete from FreeQue
 *    Param         : QUEUE *fq  : free area to delete
 *    Return Code   : void 
 */
extern void knl_mem_removefreeque( QUEUE *fq );

/*
 *    Description   : FreeQue search
 *    Param         : IMACB *imacb : Memory allocation control block
 *                    int32_t blksz : block size to search
 *    Return Code   : QUEUE* : free area 
 */
extern QUEUE* knl_mem_searchfreearea( IMACB *imacb, int32_t blksz );

/*
 *    Description   : Get nmemb*size bytes memory block from free area queue
 *    Param         : size_t nmemb: bytes per unit
 *                    size_t size:  unit count
 *    Return Code   : void*: pointer to memory block
 */
extern void* knl_mem_icalloc( size_t nmemb, size_t size );

/*
 *    Description   : Free memory,It may be called during interrupt disable. In this case,
 *                    need to wait until interrupt is enabled and until free.
 *    Param         : void *ptr: pointer to memory block
 *    Return Code   : void*:
 */
extern void  knl_mem_ifree( void *ptr );

/*
 *    Description   : Get memory from free area queue
 *    Param         : size_t size: memory block size
 *    Return Code   : void*: pointer to memory block
 */
extern void* knl_mem_imalloc( size_t size );

/*
 *    Description   : Initial system memroy control block-IMACB
 *    Param         : none
 *    Return Code   : ER ercd: E_OK if success
 */
extern ER knl_mem_init( void );

#endif /* Assembler */

#ifdef __cplusplus
}
#endif

#endif /* _KNL_MEM_H__ */
