/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
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

/** \brief Set cursor position.
 *
 *  Put the cursor at the given coordinates. Functions making use of the
 *  cursor will use the new values. Setting the cursor position outside the
 *  canvas is legal but the cursor will not be shown.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcucul canvas.
 *  \param x X cursor coordinate.
 *  \param y Y cursor coordinate.
 *  \return This function always returns 0.
 */
int cucul_gotoxy(cucul_canvas_t *cv, int x, int y)
{
    cv->frames[cv->frame].x = x;
    cv->frames[cv->frame].y = y;

    return 0;
}

/** \brief Get X cursor position.
 *
 *  Retrieve the X coordinate of the cursor's position.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcucul canvas.
 *  \return The cursor's X coordinate.
 */
int cucul_get_cursor_x(cucul_canvas_t const *cv)
{
    return cv->frames[cv->frame].x;
}

/** \brief Get Y cursor position.
 *
 *  Retrieve the Y coordinate of the cursor's position.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcucul canvas.
 *  \return The cursor's Y coordinate.
 */
int cucul_get_cursor_y(cucul_canvas_t const *cv)
{
    return cv->frames[cv->frame].y;
}

/** \brief Print an ASCII or Unicode character.
 *
 *  Print an ASCII or Unicode character at the given coordinates, using
 *  the default foreground and background colour values.
 *
 *  If the coordinates are outside the canvas boundaries, nothing is printed.
 *  If a fullwidth Unicode character gets overwritten, its remaining visible
 *  parts are replaced with spaces. If the canvas' boundaries would split the
 *  fullwidth character in two, a space is printed instead.
 *
 *  The behaviour when printing non-printable characters or invalid UTF-32
 *  characters is undefined. To print a sequence of bytes forming an UTF-8
 *  character instead of an UTF-32 character, use the cucul_put_str() function.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcucul canvas.
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param ch The character to print.
 *  \return This function always returns 0.
 */
int cucul_put_char(cucul_canvas_t *cv, int x, int y, uint32_t ch)
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
    curattr = cv->attrs + x + y * cv->width;
    attr = cv->curattr;

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
 *  Get the ASCII or Unicode value of the character at the given
 *  coordinates. If the value is less or equal to 127 (0x7f),
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
 *  \return This function always returns 0.
 */
uint32_t cucul_get_char(cucul_canvas_t const *cv, int x, int y)
{
    if(x < 0 || x >= (int)cv->width || y < 0 || y >= (int)cv->height)
        return ' ';

    return cv->chars[x + y * cv->width];
}

/** \brief Print a string.
 *
 *  Print an UTF-8 string at the given coordinates, using the default
 *  foreground and background values. The coordinates may be outside the
 *  canvas boundaries (eg. a negative Y coordinate) and the string will
 *  be cropped accordingly if it is too long.
 *
 *  See cucul_put_char() for more information on how fullwidth characters
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
int cucul_put_str(cucul_canvas_t *cv, int x, int y, char const *s)
{
    size_t rd;

    if(y < 0 || y >= (int)cv->height || x >= (int)cv->width)
        return 0;

    while(*s && x < -1)
    {
        x += cucul_utf32_is_fullwidth(cucul_utf8_to_utf32(s, &rd)) ? 2 : 1;
        s += rd;
    }

    while(*s && x < (int)cv->width)
    {
        uint32_t ch = cucul_utf8_to_utf32(s, &rd);
        cucul_put_char(cv, x, y, ch);
        x += cucul_utf32_is_fullwidth(ch) ? 2 : 1;
        s += rd;
    }

    return 0;
}

/** \brief Print a formated string.
 *
 *  Format a string at the given coordinates, using the default foreground
 *  and background values. The coordinates may be outside the canvas
 *  boundaries (eg. a negative Y coordinate) and the string will be cropped
 *  accordingly if it is too long. The syntax of the format string is the
 *  same as for the C printf() function.
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

    cucul_put_str(cv, x, y, buf);

    if(buf != tmp)
        free(buf);

    return 0;
}

/** \brief Clear the canvas.
 *
 *  Clear the canvas using the current foreground and background colours.
 *
 *  This function never fails.
 *
 *  \param cv The canvas to clear.
 *  \return This function always returns 0.
 */
int cucul_clear_canvas(cucul_canvas_t *cv)
{
    uint32_t attr = cv->curattr;
    unsigned int n;

    for(n = cv->width * cv->height; n--; )
    {
        cv->chars[n] = (uint32_t)' ';
        cv->attrs[n] = attr;
    }

    return 0;
}

/** \brief Set cursor handle.
 *
 *  Set the canvas' handle. Blitting functions will use the handle value
 *  to put the canvas at the proper coordinates.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcucul canvas.
 *  \param x X handle coordinate.
 *  \param y Y handle coordinate.
 *  \return This function always returns 0.
 */
int cucul_set_canvas_handle(cucul_canvas_t *cv, int x, int y)
{
    cv->frames[cv->frame].handlex = x;
    cv->frames[cv->frame].handley = y;

    return 0;
}

/** \brief Get X handle position.
 *
 *  Retrieve the X coordinate of the canvas' handle.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcucul canvas.
 *  \return The canvas' handle's X coordinate.
 */
int cucul_get_canvas_handle_x(cucul_canvas_t const *cv)
{
    return cv->frames[cv->frame].handlex;
}

/** \brief Get Y handle position.
 *
 *  Retrieve the Y coordinate of the canvas' handle.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcucul canvas.
 *  \return The canvas' handle's Y coordinate.
 */
int cucul_get_canvas_handle_y(cucul_canvas_t const *cv)
{
    return cv->frames[cv->frame].handley;
}

/** \brief Blit a canvas onto another one.
 *
 *  Blit a canvas onto another one at the given coordinates.
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
        seterrno(EINVAL);
        return -1;
    }

    x -= src->frames[src->frame].handlex;
    y -= src->frames[src->frame].handley;

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
                dst->attrs[dstix + i] = src->attrs[srcix + i];
            }
        }
        else
        {
            memcpy(dst->chars + dstix, src->chars + srcix, stride * 4);
            memcpy(dst->attrs + dstix, src->attrs + srcix, stride * 4);
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
 *  Set new boundaries for a canvas. This function can be used to crop a
 *  canvas, to expand it or for combinations of both actions. All frames
 *  are affected by this function.
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
        seterrno(EBUSY);
        return -1;
    }

    new = cucul_create_canvas(w, h);

    framecount = cucul_get_frame_count(cv);
    saved_f = cv->frame;

    for(f = 0; f < framecount; f++)
    {
        if(f)
            cucul_create_frame(new, framecount);

        cucul_set_frame(cv, f);
        cucul_set_frame(new, f);
        cucul_blit(new, -x, -y, cv, NULL);
        free(cv->frames[f].chars);
        free(cv->frames[f].attrs);
    }
    free(cv->frames);

    cv->frames = new->frames;
    free(new);

    cucul_set_frame(cv, saved_f); 
    _cucul_load_frame_info(cv);
    
    return 0;
}

