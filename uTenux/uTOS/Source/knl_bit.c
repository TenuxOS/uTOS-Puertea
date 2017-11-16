/*
 *----------------------------------------------------------------------------------------
 *    Kernel/bit
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
 *    File Name      : knl_bit.c
 *    Create Date    : 2013/2/14-2014/4/1
 *    Author         : wangshb
 *    Description    : bit operation routine.
 *----------------------------------------------------------------------------------------
 */

#include "knl_bit.h"

/* bit operation macro */
#if TK_BIGENDIAN
#define KNL_BIT_SET_N(n)     ( 0x80u >> ( ((uint8_t)(n)) & 7u) )
#define KNL_BIT_SHIFT(n)     ( ((uint8_t)(n)) >> 1u )
#else
#define KNL_BIT_SET_N(n)     ( 0x01u << (((uint8_t)(n)) & 7u) )
#define KNL_BIT_SHIFT(n)     ( ((uint8_t)(n)) << 1u )
#endif

/*
 *    Function Name : knl_bit_clr
 *    Create Date   : 2013/2/14-2014/4/1
 *    Author        : wangshb
 *    Description   : clear specified bit
 *    Param         : VP base: pointer to source
 *                    int32_t offset: specified bit offset
 *    Return Code   : none
 */
void knl_bit_clr( VP base, int32_t offset )
{
    register uint8_t *cp, mask;

    if (offset < 0) {
        return;
    }

    cp = (uint8_t*)base;
    cp += offset / 8;

    mask = KNL_BIT_SET_N(offset);

    *cp &= (uint8_t)~mask;
}

/*
 *    Function Name : knl_bit_set
 *    Create Date   : 2013/2/14-2014/2/1
 *    Author        : wangshb
 *    Description   : set specified bit
 *    Param         : VP base: pointer to source
 *                    int32_t offset: specified bit offset
 *    Return Code   : none
 */
void knl_bit_set( VP base, int32_t offset )
{
    register uint8_t *cp, mask;

    if (offset < 0) {
        return;
    }

    cp = (uint8_t*)base;
    cp += offset / 8;

    mask = KNL_BIT_SET_N(offset);

    *cp |= mask;
}

/*
 *    Function Name : knl_bit_search
 *    Create Date   : 2013/2/14-2014/4/1
 *    Author        : wangshb
 *    Description   : Perform 1 search on bit string
 *    Param         : VP base:  pointer to source
 *                    int32_t offset: specified bit offset
 *                    int32_t width:  max search bit number
 *    Return Code   : int32_t position: position of bit '1'
 */
int32_t knl_bit_search( VP base, int32_t offset, int32_t width )
{
    register uint8_t *cp, mask;
    register int32_t position;
    register uint32_t tmp;
    
    if ((offset < 0) || (width < 0)) {
        return -1;
    }

    cp = (uint8_t*)base;
    cp += offset / 8;

    position = 0;
    mask = KNL_BIT_SET_N(offset);

    while (position < width) {
        if (*cp) {                               /* includes 1 --> search bit of 1 */
            while (1) {
                if ( ( *cp & mask ) != 0u ) {
                    if (position < width) {
                        return position;
                    } else {
                        return -1;
                    }
                }
                mask = KNL_BIT_SHIFT(mask);
                ++position;
            }
        } else {                                 /* all bits are 0 --> 1 Byte skip */
            if (position) {
                position += 8;
            } else {
                tmp = 8u - ( (uint32_t)offset & 7u );
                position = (int32_t)tmp;
                mask = KNL_BIT_SET_N(0);
            }
            cp++;
        }
    }

    return -1;
}
