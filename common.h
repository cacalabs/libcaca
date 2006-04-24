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

#if !defined(HAVE_INTTYPES_H)
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed long int int32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long int uint32_t;
#endif

