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
 *  This file contains various canvas handling functions such as character
 *  and string drawing.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h> /* BUFSIZ */
#   include <string.h>
#   include <stdlib.h>
#   include <stdarg.h>
#   if defined(HAVE_UNISTD_H)
#       include <unistd.h>
#   endif
#   if defined(HAVE_SIGNAL_H)
#       include <signal.h>
#   endif
#   if defined(HAVE_SYS_IOCTL_H)
#       include <sys/ioctl.h>
#   endif
#endif

#include "cucul.h"
#include "cucul_internals.h"

/** \brief Print an ASCII character.
 *
 *  This function prints an ASCII character at the given coordinates, using
 *  the default foreground and background values. If the coordinates are
 *  outside the canvas boundaries, nothing is printed. If the character
 *  value is a non-printable character or is outside the ASCII range, it is
 *  replaced with a space. To print a sequence of bytes forming an UTF-8
 *  character, use cucul_putstr() instead.
 *
 *  \param c A handle to the libcucul canvas.
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param ch The character to print.
 */
void cucul_putchar(cucul_canvas_t *c, int x, int y, char ch)
{
    if(x < 0 || x >= (int)c->width ||
       y < 0 || y >= (int)c->height)
        return;

    if((unsigned char)ch < 0x20 || (unsigned char)ch > 0x7f)
        ch = 0x20;

    c->chars[x + y * c->width] = ch;
    c->attr[x + y * c->width] = (c->bgcolor << 16) | c->fgcolor;
}

/** \brief Print a string.
 *
 *  This function prints an UTF-8 string at the given coordinates, using the
 *  default foreground and background values. The coordinates may be outside
 *  the canvas boundaries (eg. a negative Y coordinate) and the string will
 *  be cropped accordingly if it is too long.
 *
 *  \param c A handle to the libcucul canvas.
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param s The string to print.
 */
void cucul_putstr(cucul_canvas_t *c, int x, int y, char const *s)
{
    uint32_t *chars, *attr;
    unsigned int len;

    if(y < 0 || y >= (int)c->height || x >= (int)c->width)
        return;

    len = _cucul_strlen_utf8(s);

    if(x < 0)
    {
        if(len < (unsigned int)-x)
            return;
        len -= -x;
        s = _cucul_skip_utf8(s, -x);
        x = 0;
    }

    chars = c->chars + x + y * c->width;
    attr = c->attr + x + y * c->width;

    if(x + len >= c->width)
        len = c->width - x;

    while(len)
    {
        *chars++ = _cucul_utf8_to_utf32(s);
        *attr++ = (c->bgcolor << 16) | c->fgcolor;

        s = _cucul_skip_utf8(s, 1);
        len--;
    }
}

/** \brief Print a formated string.
 *
 *  This function formats a string at the given coordinates, using the
 *  default foreground and background values. The coordinates may be outside
 *  the canvas boundaries (eg. a negative Y coordinate) and the string will
 *  be cropped accordingly if it is too long. The syntax of the format
 *  string is the same as for the C printf() function.
 *
 *  \param c A handle to the libcucul canvas.
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param format The format string to print.
 *  \param ... Arguments to the format string.
 */
void cucul_printf(cucul_canvas_t *c, int x, int y, char const *format, ...)
{
    char tmp[BUFSIZ];
    char *buf = tmp;
    va_list args;

    if(y < 0 || y >= (int)c->height || x >= (int)c->width)
        return;

    if(c->width - x + 1 > BUFSIZ)
        buf = malloc(c->width - x + 1);

    va_start(args, format);
#if defined(HAVE_VSNPRINTF)
    vsnprintf(buf, c->width - x + 1, format, args);
#else
    vsprintf(buf, format, args);
#endif
    buf[c->width - x] = '\0';
    va_end(args);

    cucul_putstr(c, x, y, buf);

    if(buf != tmp)
        free(buf);
}

/** \brief Clear the canvas.
 *
 *  This function clears the canvas using a black background.
 */
void cucul_clear(cucul_canvas_t *c)
{
    uint16_t oldfg = c->fgcolor;
    uint16_t oldbg = c->bgcolor;
    int y = c->height;

    cucul_set_color(c, CUCUL_COLOR_LIGHTGRAY, CUCUL_COLOR_BLACK);

    /* We could use SLsmg_cls() etc., but drawing empty lines is much faster */
    while(y--)
        cucul_putstr(c, 0, y, c->empty_line);

    c->fgcolor = oldfg;
    c->bgcolor = oldbg;
}

/** \brief Blit a canvas onto another one.
 *
 *  This function blits a canvas onto another one at the given coordinates.
 *  An optional mask canvas can be used.
 *
 *  \param dst The destination canvas.
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param src The source canvas.
 *  \param mask The mask canvas.
 */
void cucul_blit(cucul_canvas_t *dst, int x, int y,
                cucul_canvas_t const *src, cucul_canvas_t const *mask)
{
    int i, j, starti, startj, endi, endj;

    if(mask && (src->width != mask->width || src->height != mask->height))
        return;

    starti = x < 0 ? -x : 0;
    startj = y < 0 ? -y : 0;
    endi = (x + src->width >= dst->width) ? dst->width - x : src->width;
    endj = (y + src->height >= dst->height) ? dst->height - y : src->height;

    if(starti >= endi || startj >= endj)
        return;

    for(j = startj; j < endj; j++)
    {
        if(mask)
        {
            for(i = starti; i < endi; i++)
            {
                if(mask->chars[j * src->width + i] == (uint32_t)' ')
                    continue;

                dst->chars[(j + y) * dst->width + (i + x)]
                                             = src->chars[j * src->width + i];
                dst->attr[(j + y) * dst->width + (i + x)]
                                             = src->attr[j * src->width + i];
            }
        }
        else
        {
            memcpy(dst->chars + (j + y) * dst->width + starti + x,
                   src->chars + j * src->width + starti,
                   (endi - starti) * 4);
            memcpy(dst->attr + (j + y) * dst->width + starti + x,
                   src->attr + j * src->width + starti,
                   (endi - starti) * 4);
        }
    }
}

/*
 * XXX: The following functions are not exported
 */

void _cucul_putchar32(cucul_canvas_t *c, int x, int y, uint32_t ch)
{
    if(x < 0 || x >= (int)c->width ||
       y < 0 || y >= (int)c->height)
        return;

    c->chars[x + y * c->width] = ch;
    c->attr[x + y * c->width] = (c->bgcolor << 16) | c->fgcolor;
}

