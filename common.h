/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *  This file contains replacements for commonly found object types and
 *  function prototypes that are sometimes missing.
 */

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#else
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed long int int32_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long int uint32_t;

typedef long int intptr_t;
typedef unsigned long int uintptr_t;
#endif

#if !defined(HAVE_HTONS) && !defined(HAVE_NETINET_IN_H)
#   if defined(HAVE_ENDIAN_H)
#       include <endian.h>
#   endif
static inline uint16_t htons(uint16_t x)
{
#if defined(HAVE_ENDIAN_H)
    if(__BYTE_ORDER == __BIG_ENDIAN)
#else
    /* This is compile-time optimised with at least -O1 or -Os */
    uint32_t const dummy = 0x12345678;
    if(*(uint8_t const *)&dummy == 0x12)
#endif
        return x;
    else
        return (x >> 8) | (x << 8);
}

static inline uint32_t htonl(uint32_t x)
{
#if defined(HAVE_ENDIAN_H)
    if(__BYTE_ORDER == __BIG_ENDIAN)
#else
    /* This is compile-time optimised with at least -O1 or -Os */
    uint32_t const dummy = 0x12345678;
    if(*(uint8_t const *)&dummy == 0x12)
#endif
        return x;
    else
        return (x >> 24) | ((x >> 8) & 0x0000ff00)
                | ((x << 8) & 0x00ff0000) | (x << 24);
}
#endif

