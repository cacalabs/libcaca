/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
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

#include "caca.h"
#include "caca_internals.h"

#if defined _WIN32 && defined __GNUC__ && __GNUC__ >= 3
int vsnprintf_s(char *s, size_t n, size_t c,
                const char *fmt, va_list ap) CACA_WEAK;
int vsnprintf(char *s, size_t n, const char *fmt, va_list ap) CACA_WEAK;
#endif

/** \brief Set cursor position.
 *
 *  Put the cursor at the given coordinates. Functions making use of the
 *  cursor will use the new values. Setting the cursor position outside the
 *  canvas is legal but the cursor will not be shown.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcaca canvas.
 *  \param x X cursor coordinate.
 *  \param y Y cursor coordinate.
 *  \return This function always returns 0.
 */
int caca_gotoxy(caca_canvas_t *cv, int x, int y)
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
 *  \param cv A handle to the libcaca canvas.
 *  \return The cursor's X coordinate.
 */
int caca_wherex(caca_canvas_t const *cv)
{
    return cv->frames[cv->frame].x;
}

/** \brief Get Y cursor position.
 *
 *  Retrieve the Y coordinate of the cursor's position.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcaca canvas.
 *  \return The cursor's Y coordinate.
 */
int caca_wherey(caca_canvas_t const *cv)
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
 *  character instead of an UTF-32 character, use the caca_put_str() function.
 *
 *  This function returns the width of the printed character. If it is a
 *  fullwidth character, 2 is returned. Otherwise, 1 is returned.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcaca canvas.
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param ch The character to print.
 *  \return The width of the printed character: 2 for a fullwidth character,
 *          1 otherwise.
 */
int caca_put_char(caca_canvas_t *cv, int x, int y, uint32_t ch)
{
    uint32_t *curchar, *curattr, attr;
    int fullwidth, xmin, xmax, ret;

    if(ch == CACA_MAGIC_FULLWIDTH)
        return 1;

    fullwidth = caca_utf32_is_fullwidth(ch);
    ret = fullwidth ? 2 : 1;

    if(x >= (int)cv->width || y < 0 || y >= (int)cv->height)
        return ret;

    if(x == -1 && fullwidth)
    {
        x = 0;
        ch = ' ';
        fullwidth = 0;
    }
    else if(x < 0)
        return ret;

    curchar = cv->chars + x + y * cv->width;
    curattr = cv->attrs + x + y * cv->width;
    attr = cv->curattr;

    xmin = xmax = x;

    /* When overwriting the right part of a fullwidth character,
     * replace its left part with a space. */
    if(x && curchar[0] == CACA_MAGIC_FULLWIDTH)
    {
        curchar[-1] = ' ';
        xmin--;
    }

    if(fullwidth)
    {
        if(x + 1 == (int)cv->width)
            ch = ' ';
        else
        {
            xmax++;

            /* When overwriting the left part of a fullwidth character,
             * replace its right part with a space. */
            if(x + 2 < (int)cv->width && curchar[2] == CACA_MAGIC_FULLWIDTH)
            {
                curchar[2] = ' ';
                xmax++;
            }

            curchar[1] = CACA_MAGIC_FULLWIDTH;
            curattr[1] = attr;
        }
    }
    else
    {
        /* When overwriting the left part of a fullwidth character,
         * replace its right part with a space. */
        if(x + 1 != (int)cv->width && curchar[1] == CACA_MAGIC_FULLWIDTH)
        {
            curchar[1] = ' ';
            xmax++;
        }
    }

    /* Only add a dirty rectangle if we are pasting a different character
     * or attribute at that place. This does not account for inconsistencies
     * in the canvas, ie. if CACA_MAGIC_FULLWIDTH lies at illegal places,
     * but it's the caller's responsibility not to corrupt the contents. */
    if(!cv->dirty_disabled
        && (curchar[0] != ch || curattr[0] != attr))
        caca_add_dirty_rect(cv, xmin, y, xmax - xmin + 1, 1);

    curchar[0] = ch;
    curattr[0] = attr;

    return ret;
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
 *  A special exception is when CACA_MAGIC_FULLWIDTH is returned. This
 *  value is guaranteed not to be a valid Unicode character, and indicates
 *  that the character at the left of the requested one is a fullwidth
 *  character.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcaca canvas.
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \return The Unicode character at the given coordinates.
 */
uint32_t caca_get_char(caca_canvas_t const *cv, int x, int y)
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
 *  See caca_put_char() for more information on how fullwidth characters
 *  are handled when overwriting each other or at the canvas' boundaries.
 *
 *  This function returns the number of cells printed by the string. It is
 *  not the number of characters printed, because fullwidth characters
 *  account for two cells.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcaca canvas.
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param s The string to print.
 *  \return The number of cells printed.
 */
int caca_put_str(caca_canvas_t *cv, int x, int y, char const *s)
{
    size_t rd;
    int len = 0;

    if (y < 0 || y >= (int)cv->height || x >= (int)cv->width)
    {
        while (*s)
        {
            len += caca_utf32_is_fullwidth(caca_utf8_to_utf32(s, &rd)) ? 2 : 1;
            s += rd ? rd : 1;
        }
        return len;
    }

    while (*s)
    {
        uint32_t ch = caca_utf8_to_utf32(s, &rd);

        if (x + len >= -1 && x + len < (int)cv->width)
            caca_put_char(cv, x + len, y, ch);

        len += caca_utf32_is_fullwidth(ch) ? 2 : 1;
        s += rd ? rd : 1;
    }

    return len;
}

/** \brief Print a formated string.
 *
 *  Format a string at the given coordinates, using the default foreground
 *  and background values. The coordinates may be outside the canvas
 *  boundaries (eg. a negative Y coordinate) and the string will be cropped
 *  accordingly if it is too long. The syntax of the format string is the
 *  same as for the C printf() function.
 *
 *  This function returns the number of cells printed by the string. It is
 *  not the number of characters printed, because fullwidth characters
 *  account for two cells.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcaca canvas.
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param format The format string to print.
 *  \param ... Arguments to the format string.
 *  \return The number of cells printed.
 */
int caca_printf(caca_canvas_t *cv, int x, int y, char const *format, ...)
{
    va_list args;
    int ret;
    va_start(args, format);
    ret = caca_vprintf(cv, x, y, format, args);
    va_end(args);
    return ret;
}

/** \brief Print a formated string (va_list version).
 *
 *  Format a string at the given coordinates, using the default foreground
 *  and background values. The coordinates may be outside the canvas
 *  boundaries (eg. a negative X coordinate) and the string will be cropped
 *  accordingly if it is too long. The syntax of the format string is the
 *  same as for the C vprintf() function.
 *
 *  This function returns the number of cells printed by the string. It is
 *  not the number of characters printed, because fullwidth characters
 *  account for two cells.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcaca canvas.
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param format The format string to print.
 *  \param args A va_list containting the arguments to the format string.
 *  \return The number of cells printed.
 */
int caca_vprintf(caca_canvas_t *cv, int x, int y, char const *format,
                 va_list args)
{
    char tmp[BUFSIZ];
    char *buf = tmp;
    int bufsize = BUFSIZ, ret;

    if(cv->width - x + 1 > BUFSIZ)
    {
        bufsize = cv->width - x + 1;
        buf = malloc(bufsize);
    }

#if defined(HAVE_VSNPRINTF_S)
    vsnprintf_s(buf, bufsize, _TRUNCATE, format, args);
#elif defined(HAVE_VSNPRINTF)
    vsnprintf(buf, bufsize, format, args);
#else
    vsprintf(buf, format, args);
#endif
    buf[bufsize - 1] = '\0';

    ret = caca_put_str(cv, x, y, buf);

    if(buf != tmp)
        free(buf);

    return ret;
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
int caca_clear_canvas(caca_canvas_t *cv)
{
    uint32_t attr = cv->curattr;
    int n;

    for(n = cv->width * cv->height; n--; )
    {
        cv->chars[n] = (uint32_t)' ';
        cv->attrs[n] = attr;
    }

    if(!cv->dirty_disabled)
        caca_add_dirty_rect(cv, 0, 0, cv->width, cv->height);

    return 0;
}

/** \brief Set cursor handle.
 *
 *  Set the canvas' handle. Blitting functions will use the handle value
 *  to put the canvas at the proper coordinates.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcaca canvas.
 *  \param x X handle coordinate.
 *  \param y Y handle coordinate.
 *  \return This function always returns 0.
 */
int caca_set_canvas_handle(caca_canvas_t *cv, int x, int y)
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
 *  \param cv A handle to the libcaca canvas.
 *  \return The canvas' handle's X coordinate.
 */
int caca_get_canvas_handle_x(caca_canvas_t const *cv)
{
    return cv->frames[cv->frame].handlex;
}

/** \brief Get Y handle position.
 *
 *  Retrieve the Y coordinate of the canvas' handle.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcaca canvas.
 *  \return The canvas' handle's Y coordinate.
 */
int caca_get_canvas_handle_y(caca_canvas_t const *cv)
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
int caca_blit(caca_canvas_t *dst, int x, int y,
              caca_canvas_t const *src, caca_canvas_t const *mask)
{
    int i, j, starti, startj, endi, endj, stride, bleed_left, bleed_right;

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
    stride = endi - starti;

    if(starti > src->width || startj > src->height
        || starti >= endi || startj >= endj)
        return 0;

    bleed_left = bleed_right = 0;

    for(j = startj; j < endj; j++)
    {
        int dstix = (j + y) * dst->width + starti + x;
        int srcix = j * src->width + starti;

        /* FIXME: we are ignoring the mask here */
        if((starti + x) && dst->chars[dstix] == CACA_MAGIC_FULLWIDTH)
        {
            dst->chars[dstix - 1] = ' ';
            bleed_left = 1;
        }

        if(endi + x < dst->width
                && dst->chars[dstix + stride] == CACA_MAGIC_FULLWIDTH)
        {
            dst->chars[dstix + stride] = ' ';
            bleed_right = 1;
        }

        if(mask)
        {
            for(i = 0; i < stride; i++)
            {
                if(mask->chars[srcix + i] == (uint32_t)' ')
                    continue;

                if(dst->chars[dstix + i] != src->chars[srcix + i] ||
                   dst->attrs[dstix + i] != src->attrs[srcix + i])
                {
                    dst->chars[dstix + i] = src->chars[srcix + i];
                    dst->attrs[dstix + i] = src->attrs[srcix + i];
                    if(!dst->dirty_disabled)
                        caca_add_dirty_rect(dst, x + starti + i, y + j, 1, 1);
                }
            }
        }
        else
        {
            if(memcmp(dst->chars + dstix, src->chars + srcix, stride * 4) ||
               memcmp(dst->attrs + dstix, src->attrs + srcix, stride * 4))
            {
                /* FIXME be more precise ? */
                memcpy(dst->chars + dstix, src->chars + srcix, stride * 4);
                memcpy(dst->attrs + dstix, src->attrs + srcix, stride * 4);
                if(!dst->dirty_disabled)
                    caca_add_dirty_rect(dst, x + starti, y + j, stride, 1);
            }
        }

        /* Fix split fullwidth chars */
        if(src->chars[srcix] == CACA_MAGIC_FULLWIDTH)
            dst->chars[dstix] = ' ';

        if(endi < src->width && src->chars[endi] == CACA_MAGIC_FULLWIDTH)
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
 *  - \c EINVAL Specified width or height is invalid.
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
int caca_set_canvas_boundaries(caca_canvas_t *cv, int x, int y, int w, int h)
{
    caca_canvas_t *new;
    int f, saved_f, framecount;

    if(cv->refcount)
    {
        seterrno(EBUSY);
        return -1;
    }

    if(w < 0 || h < 0)
    {
        seterrno(EINVAL);
        return -1;
    }

    new = caca_create_canvas(w, h);

    framecount = caca_get_frame_count(cv);
    saved_f = cv->frame;

    for(f = 0; f < framecount; f++)
    {
        if(f)
            caca_create_frame(new, framecount);

        caca_set_frame(cv, f);
        caca_set_frame(new, f);
        caca_blit(new, -x, -y, cv, NULL);
        free(cv->frames[f].chars);
        free(cv->frames[f].attrs);
    }
    free(cv->frames);

    cv->frames = new->frames;
    free(new);

    caca_set_frame(cv, saved_f);
    _caca_load_frame_info(cv);

    /* FIXME: this may be optimised somewhat */
    if(!cv->dirty_disabled)
        caca_add_dirty_rect(cv, 0, 0, cv->width, cv->height);

    return 0;
}

/*
 * Functions for the mingw32 runtime
 */

#if defined _WIN32 && defined __GNUC__ && __GNUC__ >= 3
int vsnprintf_s(char *s, size_t n, size_t c, const char *fmt, va_list ap)
{
    return vsnprintf(s, n, fmt, ap);
}

int vsnprintf(char *s, size_t n, const char *fmt, va_list ap)
{
    return 0;
}
#endif

/*
 * XXX: The following functions are aliases.
 */

int cucul_gotoxy(cucul_canvas_t *, int, int) CACA_ALIAS(caca_gotoxy);
int cucul_get_cursor_x(cucul_canvas_t const *) CACA_ALIAS(caca_wherex);
int cucul_get_cursor_y(cucul_canvas_t const *) CACA_ALIAS(caca_wherey);
int caca_get_cursor_x(caca_canvas_t const *) CACA_ALIAS(caca_wherex);
int caca_get_cursor_y(caca_canvas_t const *) CACA_ALIAS(caca_wherey);
int cucul_put_char(cucul_canvas_t *, int, int, uint32_t)
         CACA_ALIAS(caca_put_char);
uint32_t cucul_get_char(cucul_canvas_t const *, int, int)
         CACA_ALIAS(caca_get_char);
int cucul_put_str(cucul_canvas_t *, int, int, char const *)
         CACA_ALIAS(caca_put_str);
int cucul_printf(cucul_canvas_t *, int, int, char const *, ...)
         CACA_ALIAS(caca_printf);
int cucul_clear_canvas(cucul_canvas_t *) CACA_ALIAS(caca_clear_canvas);
int cucul_set_canvas_handle(cucul_canvas_t *, int, int)
         CACA_ALIAS(caca_set_canvas_handle);
int cucul_get_canvas_handle_x(cucul_canvas_t const *)
         CACA_ALIAS(caca_get_canvas_handle_x);
int cucul_get_canvas_handle_y(cucul_canvas_t const *)
         CACA_ALIAS(caca_get_canvas_handle_y);
int cucul_blit(cucul_canvas_t *, int, int, cucul_canvas_t const *,
                        cucul_canvas_t const *) CACA_ALIAS(caca_blit);
int cucul_set_canvas_boundaries(cucul_canvas_t *, int, int, int, int)
         CACA_ALIAS(caca_set_canvas_boundaries);

