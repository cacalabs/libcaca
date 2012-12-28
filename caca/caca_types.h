/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2008-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains definitions for required C99 integer types.
 */

#ifndef __CACA_TYPES_H__
#define __CACA_TYPES_H__

#if !defined _MSC_VER
    /* FIXME: detect platforms with <inttypes.h> and no <stdint.h> */
#   include <stdint.h>
#   include <unistd.h>

#elif defined _MSC_STDINT_H_ /* msinttypes */ \
   || defined NM_DEFINED_FIXED_WIDTH_TYPEDEFS /* Morpheme */
    /* Someone already defined things for us: do nothing */

#else
#   if _MSC_VER >= 1600
        /* Visual Studio 2010 and later */
#       include <stdint.h>
#   else
#       include <intsafe.h>
#       if _MSC_VER >= 1300
            typedef signed __int8 int8_t;
            typedef signed __int16 int16_t;
            typedef signed __int32 int32_t;
            typedef signed __int64 int64_t;
            typedef unsigned __int8 uint8_t;
            typedef unsigned __int16 uint16_t;
            typedef unsigned __int32 uint32_t;
            typedef unsigned __int64 uint64_t;
#       else
            typedef signed char int8_t;
            typedef signed short int16_t;
            typedef signed int int32_t;
            typedef signed long long int int64_t;
            typedef unsigned char uint8_t;
            typedef unsigned short uint16_t;
            typedef unsigned int uint32_t;
            typedef unsigned long long int uint64_t;
#       endif
#   endif
#   if defined _WIN64
        /* Win64, (u)intptr_t and size_t are present */
        typedef int ssize_t;
#   else
        /* Win32, only (u)intptr_t is present */
        typedef int ssize_t;
        typedef unsigned int size_t;
#   endif
#endif

#if 0
/* fallback: nothing is known, we assume the platform is 32-bit and
 * sizeof(long) == sizeof(void *). We don't typedef directly because we
 * have no idea what other typedefs have already been made. */
typedef signed char _caca_int8_t;
typedef signed short _caca_int16_t;
typedef signed long int _caca_int32_t;
typedef signed long long int _caca_int64_t;
#   undef int8_t
#   define int8_t _caca_int8_t
#   undef int16_t
#   define int16_t _caca_int16_t
#   undef int32_t
#   define int32_t _caca_int32_t
#   undef int64_t
#   define int64_t _caca_int64_t
typedef unsigned char _caca_uint8_t;
typedef unsigned short _caca_uint16_t;
typedef unsigned long int _caca_uint32_t;
typedef unsigned long long int _caca_uint64_t;
#   undef uint8_t
#   define uint8_t _caca_uint8_t
#   undef uint16_t
#   define uint16_t _caca_uint16_t
#   undef uint32_t
#   define uint32_t _caca_uint32_t
#   undef uint64_t
#   define uint64_t _caca_uint64_t
typedef long int _caca_intptr_t;
typedef unsigned long int _caca_uintptr_t;
#   undef intptr_t
#   define intptr_t _caca_intptr_t
#   undef uintptr_t
#   define uintptr_t _caca_uintptr_t
typedef int _caca_ssize_t;
typedef unsigned int _caca_size_t;
#   undef ssize_t
#   define ssize_t _caca_ssize_t
#   undef size_t
#   define size_t _caca_size_t
#endif

#endif /* __CACA_TYPES_H__ */

