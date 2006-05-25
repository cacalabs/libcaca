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
#include "common.h"

#if !defined(__KERNEL__)
#   include <stdio.h> /* BUFSIZ */
#   include <string.h>
#   include <stdlib.h>
#   include <stdarg.h>
#   if defined(HAVE_ERRNO_H)
#       include <errno.h>
#   endif
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

/** \brief Print an ASCII or Unicode character.
 *
 *  This function prints an ASCII or Unicode character at the given
 *  coordinates, using the default foreground and background values.
 *
 *  If the coordinates are outside the canvas boundaries, nothing is printed.
 *  If the character value is a non-printable character or is outside the
 *  UTF-32 range, it is replaced with a space. To print a sequence of bytes
 *  forming an UTF-8 character instead of an UTF-32 character, use the
 *  cucul_putstr() function instead.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcucul canvas.
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param ch The character to print.
 *  \return This function always returns 0.
 */
int cucul_putchar(cucul_canvas_t *cv, int x, int y, unsigned long int ch)
{
    if(x < 0 || x >= (int)cv->width || y < 0 || y >= (int)cv->height)
        return 0;

    if((unsigned char)ch < 0x20)
        ch = 0x20;

    cv->chars[x + y * cv->width] = ch;
    cv->attr[x + y * cv->width] = (cv->bgcolor << 16) | cv->fgcolor;

    return 0;
}

/** \brief Print a string.
 *
 *  This function prints an UTF-8 string at the given coordinates, using the
 *  default foreground and background values. The coordinates may be outside
 *  the canvas boundaries (eg. a negative Y coordinate) and the string will
 *  be cropped accordingly if it is too long.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcucul canvas.
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param s The string to print.
 *  \return This function always returns 0.
 */
int cucul_putstr(cucul_canvas_t *cv, int x, int y, char const *s)
{
    uint32_t *chars, *attr;
    unsigned int len;

    if(y < 0 || y >= (int)cv->height || x >= (int)cv->width)
        return 0;

    len = _cucul_strlen_utf8(s);

    if(x < 0)
    {
        if(len < (unsigned int)-x)
            return 0;
        len -= -x;
        s = _cucul_skip_utf8(s, -x);
        x = 0;
    }

    chars = cv->chars + x + y * cv->width;
    attr = cv->attr + x + y * cv->width;

    if(x + len >= cv->width)
        len = cv->width - x;

    while(len)
    {
        *chars++ = cucul_utf8_to_utf32(s, NULL);
        *attr++ = (cv->bgcolor << 16) | cv->fgcolor;

        s = _cucul_skip_utf8(s, 1);
        len--;
    }

    return 0;
}

/** \brief Print a formated string.
 *
 *  This function formats a string at the given coordinates, using the
 *  default foreground and background values. The coordinates may be outside
 *  the canvas boundaries (eg. a negative Y coordinate) and the string will
 *  be cropped accordingly if it is too long. The syntax of the format
 *  string is the same as for the C printf() function.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcucul canvas.
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param format The format string to print.
 *  \param ... Arguments to the format string.
 *  \return This function always returns 0.
 */
int cucul_printf(cucul_canvas_t *cv, int x, int y, char const *format, ...)
{
    char tmp[BUFSIZ];
    char *buf = tmp;
    va_list args;

    if(y < 0 || y >= (int)cv->height || x >= (int)cv->width)
        return 0;

    if(cv->width - x + 1 > BUFSIZ)
        buf = malloc(cv->width - x + 1);

    va_start(args, format);
#if defined(HAVE_VSNPRINTF)
    vsnprintf(buf, cv->width - x + 1, format, args);
#else
    vsprintf(buf, format, args);
#endif
    buf[cv->width - x] = '\0';
    va_end(args);

    cucul_putstr(cv, x, y, buf);

    if(buf != tmp)
        free(buf);

    return 0;
}

/** \brief Clear the canvas.
 *
 *  This function clears the canvas using the current background colour.
 *
 *  This function never fails.
 *
 *  \param cv The canvas to clear.
 *  \return This function always returns 0.
 */
int cucul_clear_canvas(cucul_canvas_t *cv)
{
    uint32_t color = (cv->bgcolor << 16) | cv->fgcolor;
    unsigned int n;

    /* We could use SLsmg_cls() etc., but drawing empty lines is much faster */
    for(n = cv->width * cv->height; n--; )
    {
        cv->chars[n] = (uint32_t)' ';
        cv->attr[n] = color;
    }

    return 0;
}

/** \brief Blit a canvas onto another one.
 *
 *  This function blits a canvas onto another one at the given coordinates.
 *  An optional mask canvas can be used.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL A mask was specified but the mask size and source canvas
 *    size do not match.
 *
 *  \param dst The destination canvas.
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param src The source canvas.
 *  \param mask The mask canvas.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int cucul_blit(cucul_canvas_t *dst, int x, int y,
               cucul_canvas_t const *src, cucul_canvas_t const *mask)
{
    int i, j, starti, startj, endi, endj;

    if(mask && (src->width != mask->width || src->height != mask->height))
    {
#if defined(HAVE_ERRNO_H)
        errno = EINVAL;
#endif
        return -1;
    }

    starti = x < 0 ? -x : 0;
    startj = y < 0 ? -y : 0;
    endi = (x + src->width >= dst->width) ? dst->width - x : src->width;
    endj = (y + src->height >= dst->height) ? dst->height - y : src->height;

    if(starti >= endi || startj >= endj)
        return 0;

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

    return 0;
}

