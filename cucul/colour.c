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

/* Legacy stuff from old versions */
int cucul_set_color(cucul_canvas_t *, unsigned char, unsigned char);
int cucul_set_truecolor(cucul_canvas_t *, unsigned int, unsigned int);

/* RGB colours for the ANSI palette. There is no real standard, so we
 * use the same values as gnome-terminal. The 7th colour (brown) is a bit
 * special: 0xfa50 instead of 0xfaa0. */
static const uint16_t ansitab[16] =
{
    0xf000, 0xf00a, 0xf0a0, 0xf0aa, 0xfa00, 0xfa0a, 0xfa50, 0xfaaa,
    0xf555, 0xf55f, 0xf5f5, 0xf5ff, 0xff55, 0xff5f, 0xfff5, 0xffff,
};

/** \brief Set the default character attribute.
 *
 *  Set the default character attribute for drawing. Attributes define
 *  foreground and background colour, transparency, bold, italics and
 *  underline styles, as well as blink. String functions such as
 *  caca_printf() and graphical primitive functions such as caca_draw_line()
 *  will use this attribute.
 *
 *  The attribute value is a 32-bit integer as returned by cucul_get_attr().
 *  For more user-friendly versions of this function, see cucul_set_attr_ansi()
 *  and cucul_set_attr_argb().
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL The attribute value is out of the 32-bit range.
 *
 *  \param cv A handle to the libcucul canvas.
 *  \param attr The requested attribute value.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int cucul_set_attr(cucul_canvas_t *cv, unsigned long int attr)
{
    if(sizeof(unsigned long int) > sizeof(uint32_t) && attr > 0xffffffff)
    {
#if defined(HAVE_ERRNO_H)
        errno = EINVAL;
#endif
        return -1;
    }

    cv->curattr = attr;

    return 0;
}

/** \brief Set the default colour pair and text style (ANSI version).
 *
 *  Set the default ANSI colour pair and text style for drawing. String
 *  functions such as caca_printf() and graphical primitive functions such as
 *  caca_draw_line() will use these attributes.
 *
 *  Color values are those defined in cucul.h, such as CUCUL_COLOR_RED
 *  or CUCUL_COLOR_TRANSPARENT.
 *
 *  Style values are those defined in cucul.h, such as CUCUL_STYLE_UNDERLINE
 *  or CUCUL_STYLE_BLINK. The values can be ORed to set several styles at
 *  the same time.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL The colour values and/or the style mask are invalid.
 *
 *  \param cv A handle to the libcucul canvas.
 *  \param fg The requested foreground colour.
 *  \param bg The requested background colour.
 *  \param style The requested text styles.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int cucul_set_attr_ansi(cucul_canvas_t *cv, unsigned char fg, unsigned char bg,
                        unsigned char style)
{
    uint32_t attr;

    if(fg > 0x20 || bg > 0x20 || style > 0x0f)
    {
#if defined(HAVE_ERRNO_H)
        errno = EINVAL;
#endif
        return -1;
    }

    attr = ((uint32_t)bg << 20) | ((uint32_t)fg << 4);

    if(style)
        attr |= (0x02004801 * style) & 0x10011001;

    cv->curattr = attr;

    return 0;
}

/* Legacy function for old programs */
int cucul_set_color(cucul_canvas_t *cv, unsigned char fg, unsigned char bg)
{
    return cucul_set_attr_ansi(cv, fg, bg, 0);
}

/** \brief Set the default colour pair and text style (truecolor version).
 *
 *  Set the default colour pair and text style for drawing. String
 *  functions such as caca_printf() and graphical primitive functions such as
 *  caca_draw_line() will use these attributes.
 *
 *  Colors are 16-bit ARGB values, each component being coded on 4 bits. For
 *  instance, 0xf088 is solid dark cyan (A=15 R=0 G=8 B=8), and 0x8fff is
 *  white with 50% alpha (A=8 R=15 G=15 B=15).
 *
 *  Style values are those defined in cucul.h, such as CUCUL_STYLE_UNDERLINE
 *  or CUCUL_STYLE_BLINK. The values can be ORed to set several styles at
 *  the same time.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL At least one of the colour values is invalid.
 *
 *  \param cv A handle to the libcucul canvas.
 *  \param fg The requested foreground colour.
 *  \param bg The requested background colour.
 *  \param style The requested text styles.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int cucul_set_attr_argb(cucul_canvas_t *cv, unsigned int fg, unsigned int bg,
                        unsigned char style)
{
    uint32_t attr;

    if(fg > 0xffff || bg > 0xffff || style > 0x0f)
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

    attr = (((uint32_t)bg << 16) | (uint32_t)fg) & 0xeffeeffe;

    if(style)
        attr |= (0x02004801 * style) & 0x10011001;

    cv->curattr = attr;

    return 0;
}

/* Legacy function for old programs */
int cucul_set_truecolor(cucul_canvas_t *cv, unsigned int fg, unsigned int bg)
{
    return cucul_set_attr_argb(cv, fg, bg, 0);
}

/** \brief Get the text attribute at the given coordinates.
 *
 *  Get the internal \e libcucul attribute value of the character at the
 *  given coordinates. The attribute value has 32 significant bits,
 *  organised as follows from MSB to LSB:
 *  - 3 bits for the background alpha
 *  - 1 bit for the blink flag
 *  - 4 bits for the background red component
 *  - 4 bits for the background green component
 *  - 3 bits for the background blue component
 *  - 1 bit for the underline flag
 *  - 3 bits for the foreground alpha
 *  - 1 bit for the italics flag
 *  - 4 bits for the foreground red component
 *  - 4 bits for the foreground green component
 *  - 3 bits for the foreground blue component
 *  - 1 bit for the bold flag
 *
 *  If the coordinates are outside the canvas boundaries, the current
 *  attribute is returned.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcucul canvas.
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \return The requested attribute.
 */
unsigned long int cucul_get_attr(cucul_canvas_t *cv, int x, int y)
{
    if(x < 0 || x >= (int)cv->width || y < 0 || y >= (int)cv->height)
        return (unsigned long int)cv->curattr;

    return (unsigned long int)cv->attrs[x + y * cv->width];
}

/*
 * XXX: the following functions are local
 */

static uint8_t nearest_ansi(uint16_t argb16, uint8_t def)
{
    unsigned int i, best, dist;

    if(argb16 == (argb16 & 0x00f0))
        return argb16 >> 4;

    if(argb16 == (CUCUL_COLOR_DEFAULT << 4)
       || argb16 == (CUCUL_COLOR_TRANSPARENT << 4))
        return def;

    if(argb16 < 0x6fff) /* too transparent, return default colour */
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

uint8_t _cucul_attr_to_ansi8(uint32_t attr)
{
    uint16_t fg = attr & 0xeffe;
    uint16_t bg = (attr >> 16) & 0xeffe;

    return nearest_ansi(fg, CUCUL_COLOR_LIGHTGRAY)
            | (nearest_ansi(bg, CUCUL_COLOR_BLACK) << 4);
}

uint8_t _cucul_attr_to_ansi4fg(uint32_t attr)
{
    return nearest_ansi(attr & 0xeffe, CUCUL_COLOR_LIGHTGRAY);
}

uint8_t _cucul_attr_to_ansi4bg(uint32_t attr)
{
    return nearest_ansi((attr >> 16) & 0xeffe, CUCUL_COLOR_BLACK);
}

uint16_t _cucul_attr_to_rgb12fg(uint32_t attr)
{
    uint16_t fg = attr & 0xeffe;

    if(fg == (fg & 0x00f0))
        return ansitab[fg >> 4] & 0x0fff;

    if(fg == (CUCUL_COLOR_DEFAULT << 4))
        return ansitab[CUCUL_COLOR_LIGHTGRAY] & 0x0fff;

    if(fg == (CUCUL_COLOR_TRANSPARENT << 4))
        return ansitab[CUCUL_COLOR_LIGHTGRAY] & 0x0fff;

    return fg & 0x0fff;
}

uint16_t _cucul_attr_to_rgb12bg(uint32_t attr)
{
    uint16_t bg = (attr >> 16) & 0xeffe;

    if(bg == (bg & 0x00f0))
        return ansitab[bg >> 4] & 0x0fff;

    if(bg == (CUCUL_COLOR_DEFAULT << 4))
        return ansitab[CUCUL_COLOR_BLACK] & 0x0fff;

    if(bg == (CUCUL_COLOR_TRANSPARENT << 4))
        return ansitab[CUCUL_COLOR_BLACK] & 0x0fff;

    return bg & 0x0fff;
}

#define RGB12TO24(i) \
   (((uint32_t)((i & 0xf00) >> 8) * 0x110000) \
  | ((uint32_t)((i & 0x0f0) >> 4) * 0x001100) \
  | ((uint32_t)(i & 0x00f) * 0x000011))

uint32_t _cucul_attr_to_rgb24fg(uint32_t attr)
{
    return RGB12TO24(_cucul_attr_to_rgb12fg(attr));
}

uint32_t _cucul_attr_to_rgb24bg(uint32_t attr)
{
    return RGB12TO24(_cucul_attr_to_rgb12bg(attr));
}

void _cucul_attr_to_argb4(uint32_t attr, uint8_t argb[8])
{
    uint16_t fg = attr & 0xeffe;
    uint16_t bg = (attr >> 16) & 0xeffe;

    if(fg == (fg & 0x00f0))
        fg = ansitab[fg >> 4];
    else if(fg == (CUCUL_COLOR_DEFAULT << 4))
        fg = ansitab[CUCUL_COLOR_LIGHTGRAY];
    else if(fg == (CUCUL_COLOR_TRANSPARENT << 4))
        fg = 0x0fff;

    if(bg == (bg & 0x00f0))
        bg = ansitab[bg >> 4];
    else if(bg == (CUCUL_COLOR_DEFAULT << 4))
        bg = ansitab[CUCUL_COLOR_BLACK];
    else if(bg == (CUCUL_COLOR_TRANSPARENT << 4))
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

