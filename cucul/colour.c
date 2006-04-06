/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file colour.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Colour handling
 *
 *  This file contains functions for converting colour values between
 *  various colourspaces.
 */

#include "config.h"

#include "cucul.h"
#include "cucul_internals.h"

/* FIXME: could this be inlined? */

uint8_t _cucul_rgba32_to_ansi8(uint32_t c)
{
    /* FIXME: we need nearest colour handling for non-ANSI */
    return (c & 0x0000000f) | ((c & 0x000f0000) >> 12);
}

uint8_t _cucul_rgba32_to_ansi4fg(uint32_t c)
{
    /* FIXME: we need nearest colour handling for non-ANSI */
    return c & 0x0000000f;
}

uint8_t _cucul_rgba32_to_ansi4bg(uint32_t c)
{
    /* FIXME: we need nearest colour handling for non-ANSI */
    return (c & 0x000f0000) >> 16;
}

