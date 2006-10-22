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
 *  If a fullwidth Unicode character gets overwritten, its remaining parts
 *  are replaced with spaces. If the canvas' boundaries would split the
 *  fullwidth character in two, a space is printed instead.
 *
 *  The behaviour when printing non-printable characters or invalid UTF-32
 *  characters is undefined. To print a sequence of bytes forming an UTF-8
 *  character instead of an UTF-32 character, use the cucul_putstr() function.
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
    uint32_t *curchar, *curattr, attr;
    int fullwidth;

    if(x >= (int)cv->width || y < 0 || y >= (int)cv->height)
        return 0;

    if(ch == CUCUL_MAGIC_FULLWIDTH)
        return 0;

    fullwidth = cucul_utf32_is_fullwidth(ch);

    if(x == -1 && fullwidth)
    {
        x = 0;
        ch = ' ';
        fullwidth = 0;
    }
    else if(x < 0)
        return 0;

    curchar = cv->chars + x + y * cv->width;
    curattr = cv->attr + x + y * cv->width;
    attr = (cv->bgcolor << 16) | cv->fgcolor;

    /* When overwriting the right part of a fullwidth character,
     * replace its left part with a space. */
    if(x && curchar[0] == CUCUL_MAGIC_FULLWIDTH)
        curchar[-1] = ' ';

    if(fullwidth)
    {
        if(x + 1 == (int)cv->width)
            ch = ' ';
        else
        {
            /* When overwriting the left part of a fullwidth character,
             * replace its right part with a space. */
            if(x + 2 < (int)cv->width && curchar[2] == CUCUL_MAGIC_FULLWIDTH)
                curchar[2] = ' ';

            curchar[1] = CUCUL_MAGIC_FULLWIDTH;
            curattr[1] = attr;
        }
    }
    else
    {
        /* When overwriting the left part of a fullwidth character,
         * replace its right part with a space. */
        if(x + 1 != (int)cv->width && curchar[1] == CUCUL_MAGIC_FULLWIDTH)
            curchar[1] = ' ';
    }

    curchar[0] = ch;
    curattr[0] = attr;

    return 0;
}

/** \brief Get the Unicode character at the given coordinates.
 *
 *  This function gets the ASCII or Unicode value of the character at
 *  the given coordinates. If the value is less or equal to 127 (0x7f),
 *  the character can be printed as ASCII. Otherise, it must be handled
 *  as a UTF-32 value.
 *
 *  If the coordinates are outside the canvas boundaries, a space (0x20)
 *  is returned.
 *
 *  A special exception is when CUCUL_MAGIC_FULLWIDTH is returned. This
 *  value is guaranteed not to be a valid Unicode character, and indicates
 *  that the character at the left of the requested one is a fullwidth
 *  character.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcucul canvas.
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param ch The requested character value.
 *  \return The character always returns 0.
 */
unsigned long int cucul_getchar(cucul_canvas_t *cv, int x, int y)
{
    if(x < 0 || x >= (int)cv->width || y < 0 || y >= (int)cv->height)
        return ' ';

    return (unsigned long int)cv->chars[x + y * cv->width];
}

/** \brief Print a string.
 *
 *  This function prints an UTF-8 string at the given coordinates, using the
 *  default foreground and background values. The coordinates may be outside
 *  the canvas boundaries (eg. a negative Y coordinate) and the string will
 *  be cropped accordingly if it is too long.
 *
 *  See cucul_putchar() for more information on how fullwidth characters
 *  are handled when overwriting each other or at the canvas' boundaries.
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
    unsigned int read;

    if(y < 0 || y >= (int)cv->height || x >= (int)cv->width)
        return 0;

    while(*s && x < -1)
    {
        x += cucul_utf32_is_fullwidth(cucul_utf8_to_utf32(s, &read)) ? 2 : 1;
        s += read;
    }

    while(*s && x < (int)cv->width)
    {
        uint32_t ch = cucul_utf8_to_utf32(s, &read);
        cucul_putchar(cv, x, y, ch);
        x += cucul_utf32_is_fullwidth(ch) ? 2 : 1;
        s += read;
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

    if((unsigned int)starti > src->width || (unsigned int)startj > src->height
        || starti >= endi || startj >= endj)
        return 0;

    for(j = startj; j < endj; j++)
    {
        unsigned int dstix = (j + y) * dst->width + starti + x;
        unsigned int srcix = j * src->width + starti;
        int stride = endi - starti;

        /* FIXME: we are ignoring the mask here */
        if((starti + x) && dst->chars[dstix] == CUCUL_MAGIC_FULLWIDTH)
            dst->chars[dstix - 1] = ' ';

        if((unsigned int)(endi + x) < dst->width
                && dst->chars[dstix + stride] == CUCUL_MAGIC_FULLWIDTH)
            dst->chars[dstix + stride] = ' ';

        if(mask)
        {
            for(i = 0; i < stride; i++)
            {
                if(mask->chars[srcix + i] == (uint32_t)' ')
                    continue;

                dst->chars[dstix + i] = src->chars[srcix + i];
                dst->attr[dstix + i] = src->attr[srcix + i];
            }
        }
        else
        {
            memcpy(dst->chars + dstix, src->chars + srcix, (stride) * 4);
            memcpy(dst->attr + dstix, src->attr + srcix, (stride) * 4);
        }

        /* Fix split fullwidth chars */
        if(src->chars[srcix] == CUCUL_MAGIC_FULLWIDTH)
            dst->chars[dstix] = ' ';

        if((unsigned int)endi < src->width
                && src->chars[endi] == CUCUL_MAGIC_FULLWIDTH)
            dst->chars[dstix + stride - 1] = ' ';
    }

    return 0;
}

/** \brief Set a canvas' new boundaries.
 *
 *  This function sets new boundaries for a canvas. It can be used to crop a
 *  canvas, to expand it or for combinations of both actions.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EBUSY The canvas is in use by a display driver and cannot be resized.
 *  - \c ENOMEM Not enough memory for the requested canvas size. If this
 *    happens, the canvas handle becomes invalid and should not be used.
 *
 *  \param cv The canvas to crop.
 *  \param x X coordinate of the top-left corner.
 *  \param y Y coordinate of the top-left corner.
 *  \param w The width of the cropped area.
 *  \param h The height of the cropped area.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int cucul_set_canvas_boundaries(cucul_canvas_t *cv, int x, int y,
                                unsigned int w, unsigned int h)
{
    cucul_canvas_t *new;
    unsigned int f, saved_f, framecount;

    if(cv->refcount)
    {
#if defined(HAVE_ERRNO_H)
        errno = EBUSY;
#endif
        return -1;
    }

    new = cucul_create_canvas(w, h);

    framecount = cucul_get_canvas_frame_count(cv);
    saved_f = cv->frame;

    for(f = 0; f < framecount; f++)
    {
        if(f)
            cucul_create_canvas_frame(new, framecount);

        cucul_set_canvas_frame(cv, f);
        cucul_set_canvas_frame(new, f);
        cucul_blit(new, -x, -y, cv, NULL);

        free(cv->allchars[f]);
        free(cv->allattr[f]);
    }
    free(cv->allchars);
    free(cv->allattr);

    memcpy(cv, new, sizeof(cucul_canvas_t));
    free(new);

    cucul_set_canvas_frame(cv, saved_f);

    return 0;
}

