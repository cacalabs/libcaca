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
#include "common.h"

#if defined(HAVE_ERRNO_H)
#   include <errno.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

/* RGB colours for the ANSI palette. There is no real standard, so we
 * use the same values as gnome-terminal. The 7th colour (brown) is a bit
 * special. */
static const uint16_t ansitab[16] =
{
    0xf000, 0xf00a, 0xf0a0, 0xf0aa, 0xfa00, 0xfa0a, 0xfa50, 0xfaaa,
    0xf555, 0xf55f, 0xf5f5, 0xf5ff, 0xff55, 0xff5f, 0xfff5, 0xffff,
};

/** \brief Set the default colour pair.
 *
 *  Set the default ANSI colour pair for drawing. String functions such as
 *  caca_printf() and graphical primitive functions such as caca_draw_line()
 *  will use these colours.
 *
 *  Color values are those defined in cucul.h, such as CUCUL_COLOR_RED
 *  or CUCUL_COLOR_TRANSPARENT.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL At least one of the colour values is invalid.
 *
 *  \param cv A handle to the libcucul canvas.
 *  \param fg The requested foreground colour.
 *  \param bg The requested background colour.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int cucul_set_color(cucul_canvas_t *cv, unsigned char fg, unsigned char bg)
{
    if(fg > 0x20 || bg > 0x20)
    {
#if defined(HAVE_ERRNO_H)
        errno = EINVAL;
#endif
        return -1;
    }

    cv->fgcolor = fg;
    cv->bgcolor = bg;

    return 0;
}

/** \brief Set the default colour pair (truecolor version).
 *
 *  Set the default colour pair for drawing. String functions such as
 *  caca_printf() and graphical primitive functions such as caca_draw_line()
 *  will use these colours.
 *
 *  Colors are 16-bit ARGB values, each component being coded on 4 bits. For
 *  instance, 0xf088 is solid dark cyan (A=15 R=0 G=8 B=8), and 0x8fff is
 *  white with 50% alpha (A=8 R=15 G=15 B=15).
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL At least one of the colour values is invalid.
 *
 *  \param cv A handle to the libcucul canvas.
 *  \param fg The requested foreground colour.
 *  \param bg The requested background colour.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int cucul_set_truecolor(cucul_canvas_t *cv, unsigned int fg, unsigned int bg)
{
    if(fg > 0xffff || bg > 0xffff)
    {
#if defined(HAVE_ERRNO_H)
        errno = EINVAL;
#endif
        return -1;
    }

    if(fg < 0x100)
        fg += 0x100;

    if(bg < 0x100)
        bg += 0x100;

    cv->fgcolor = fg;
    cv->bgcolor = bg;

    return 0;
}

/** \brief Get the colour pair at the given coordinates.
 *
 *  Get the internal \e libcucul colour pair value of the character at the
 *  given coordinates. The colour pair value has 32 significant bits: the
 *  lower 16 bits are for the foreground colour, the higher 16 bits are for
 *  the background.
 *
 *  If the coordinates are outside the canvas boundaries, the current colour
 *  pair is returned.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcucul canvas.
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param ch The requested colour pair value.
 *  \return The character always returns 0.
 */
unsigned long int cucul_get_color(cucul_canvas_t *cv, int x, int y)
{
    if(x < 0 || x >= (int)cv->width || y < 0 || y >= (int)cv->height)
        return ((uint32_t)cv->bgcolor << 16) | (uint32_t)cv->fgcolor;

    return (unsigned long int)cv->attr[x + y * cv->width];
}


/*
 * XXX: the following functions are local
 */

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

uint8_t _cucul_argb32_to_ansi8(uint32_t ch)
{
    uint16_t fg = ch & 0xffff;
    uint16_t bg = ch >> 16;

    return nearest_ansi(fg, CUCUL_COLOR_LIGHTGRAY)
            | (nearest_ansi(bg, CUCUL_COLOR_BLACK) << 4);
}

uint8_t _cucul_argb32_to_ansi4fg(uint32_t ch)
{
    return nearest_ansi(ch & 0xffff, CUCUL_COLOR_LIGHTGRAY);
}

uint8_t _cucul_argb32_to_ansi4bg(uint32_t ch)
{
    return nearest_ansi(ch >> 16, CUCUL_COLOR_BLACK);
}

uint16_t _cucul_argb32_to_rgb12fg(uint32_t ch)
{
    uint16_t fg = ch & 0xffff;

    if(fg < CUCUL_COLOR_DEFAULT)
        return ansitab[fg] & 0x0fff;

    if(fg == CUCUL_COLOR_DEFAULT)
        return ansitab[CUCUL_COLOR_LIGHTGRAY] & 0x0fff;

    if(fg == CUCUL_COLOR_TRANSPARENT)
        return ansitab[CUCUL_COLOR_LIGHTGRAY] & 0x0fff;

    return fg & 0x0fff;
}

uint16_t _cucul_argb32_to_rgb12bg(uint32_t ch)
{
    uint16_t bg = ch >> 16;

    if(bg < CUCUL_COLOR_DEFAULT)
        return ansitab[bg] & 0x0fff;

    if(bg == CUCUL_COLOR_DEFAULT)
        return ansitab[CUCUL_COLOR_BLACK] & 0x0fff;

    if(bg == CUCUL_COLOR_TRANSPARENT)
        return ansitab[CUCUL_COLOR_BLACK] & 0x0fff;

    return bg & 0x0fff;
}

#define RGB12TO24(i) \
   (((uint32_t)((i & 0xf00) >> 8) * 0x110000) \
  | ((uint32_t)((i & 0x0f0) >> 4) * 0x001100) \
  | ((uint32_t)(i & 0x00f) * 0x000011))

uint32_t _cucul_argb32_to_rgb24fg(uint32_t ch)
{
    return RGB12TO24(_cucul_argb32_to_rgb12fg(ch));
}

uint32_t _cucul_argb32_to_rgb24bg(uint32_t ch)
{
    return RGB12TO24(_cucul_argb32_to_rgb12bg(ch));
}

void _cucul_argb32_to_argb4(uint32_t ch, uint8_t argb[8])
{
    uint16_t fg = ch & 0xffff;
    uint16_t bg = ch >> 16;

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

