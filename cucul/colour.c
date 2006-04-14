/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
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
 *  This file contains functions for converting colour values between
 *  various colourspaces.
 */

#include "config.h"

#include "cucul.h"
#include "cucul_internals.h"

static const uint16_t ansitab[16] =
{
    0xf000, 0xf008, 0xf080, 0xf088, 0xf800, 0xf808, 0xf880, 0xf888,
    0xf444, 0xf44f, 0xf4f4, 0xf4ff, 0xff44, 0xff4f, 0xfff4, 0xffff,
};

static uint8_t nearest_ansi(uint16_t argb16, uint8_t def)
{
    unsigned int i, best, dist;

    if(argb16 < CUCUL_COLOR_DEFAULT)
        return argb16;

    if(argb16 == CUCUL_COLOR_DEFAULT || argb16 == CUCUL_COLOR_TRANSPARENT)
        return def;

    if(argb16 < 0x5fff) /* too transparent, return default colour */
        return def;

    best = def;
    dist = 0xffff;
    for(i = 0; i < 16; i++)
    {
        unsigned int d = 0;
        int a, b;

        a = (ansitab[i] >> 8) & 0xf;
        b = (argb16 >> 8) & 0xf;
        d += (a - b) * (a - b);

        a = (ansitab[i] >> 4) & 0xf;
        b = (argb16 >> 4) & 0xf;
        d += (a - b) * (a - b);

        a = ansitab[i] & 0xf;
        b = argb16 & 0xf;
        d += (a - b) * (a - b);

        if(d < dist)
        {
            dist = d;
            best = i;
        }
    }

    return best;
}

uint8_t _cucul_argb32_to_ansi8(uint32_t c)
{
    uint16_t fg = c & 0xffff;
    uint16_t bg = c >> 16;

    return nearest_ansi(fg, CUCUL_COLOR_LIGHTGRAY)
            | (nearest_ansi(bg, CUCUL_COLOR_BLACK) << 4);
}

uint8_t _cucul_argb32_to_ansi4fg(uint32_t c)
{
    return nearest_ansi(c & 0xffff, CUCUL_COLOR_LIGHTGRAY);
}

uint8_t _cucul_argb32_to_ansi4bg(uint32_t c)
{
    return nearest_ansi(c >> 16, CUCUL_COLOR_BLACK);
}

void _cucul_argb32_to_argb4(uint32_t c, uint8_t argb[8])
{
    uint16_t fg = c & 0xffff;
    uint16_t bg = c >> 16;

    if(fg < CUCUL_COLOR_DEFAULT)
        fg = ansitab[fg];
    else if(fg == CUCUL_COLOR_DEFAULT)
        fg = ansitab[CUCUL_COLOR_LIGHTGRAY];
    else if(fg == CUCUL_COLOR_TRANSPARENT)
        fg = 0x0fff;

    if(bg < CUCUL_COLOR_DEFAULT)
        bg = ansitab[bg];
    else if(bg == CUCUL_COLOR_DEFAULT)
        bg = ansitab[CUCUL_COLOR_BLACK];
    else if(bg == CUCUL_COLOR_TRANSPARENT)
        bg = 0x0fff;

    argb[0] = bg >> 12;
    argb[1] = (bg >> 8) & 0xf;
    argb[2] = (bg >> 4) & 0xf;
    argb[3] = bg & 0xf;

    argb[4] = fg >> 12;
    argb[5] = (fg >> 8) & 0xf;
    argb[6] = (fg >> 4) & 0xf;
    argb[7] = fg & 0xf;
}

