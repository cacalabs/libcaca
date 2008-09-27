/*
 *  libcaca       Colour ASCII-Art library
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
 *  This file contains box drawing functions, both filled and outline.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#endif

#include "caca.h"
#include "caca_internals.h"

/** \brief Draw a box on the canvas using the given character.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcaca canvas.
 *  \param x X coordinate of the upper-left corner of the box.
 *  \param y Y coordinate of the upper-left corner of the box.
 *  \param w Width of the box.
 *  \param h Height of the box.
 *  \param ch UTF-32 character to be used to draw the box.
 *  \return This function always returns 0.
 */
int caca_draw_box(caca_canvas_t *cv, int x, int y, int w, int h, uint32_t ch)
{
    int x2 = x + w - 1;
    int y2 = y + h - 1;

    caca_draw_line(cv,  x,  y,  x, y2, ch);
    caca_draw_line(cv,  x, y2, x2, y2, ch);
    caca_draw_line(cv, x2, y2, x2,  y, ch);
    caca_draw_line(cv, x2,  y,  x,  y, ch);

    return 0;
}

/** \brief Draw a thin box on the canvas.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcaca canvas.
 *  \param x X coordinate of the upper-left corner of the box.
 *  \param y Y coordinate of the upper-left corner of the box.
 *  \param w Width of the box.
 *  \param h Height of the box.
 *  \return This function always returns 0.
 */
int caca_draw_thin_box(caca_canvas_t *cv, int x, int y, int w, int h)
{
    int i, j, xmax, ymax;

    int x2 = x + w - 1;
    int y2 = y + h - 1;

    if(x > x2)
    {
        int tmp = x;
        x = x2; x2 = tmp;
    }

    if(y > y2)
    {
        int tmp = y;
        y = y2; y2 = tmp;
    }

    xmax = cv->width - 1;
    ymax = cv->height - 1;

    if(x2 < 0 || y2 < 0 || x > xmax || y > ymax)
        return 0;

    /* Draw edges */
    if(y >= 0)
        for(i = x < 0 ? 1 : x + 1; i < x2 && i < xmax; i++)
            caca_put_char(cv, i, y, '-');

    if(y2 <= ymax)
        for(i = x < 0 ? 1 : x + 1; i < x2 && i < xmax; i++)
            caca_put_char(cv, i, y2, '-');

    if(x >= 0)
        for(j = y < 0 ? 1 : y + 1; j < y2 && j < ymax; j++)
            caca_put_char(cv, x, j, '|');

    if(x2 <= xmax)
        for(j = y < 0 ? 1 : y + 1; j < y2 && j < ymax; j++)
            caca_put_char(cv, x2, j, '|');

    /* Draw corners */
    caca_put_char(cv, x, y, ',');
    caca_put_char(cv, x, y2, '`');
    caca_put_char(cv, x2, y, '.');
    caca_put_char(cv, x2, y2, '\'');

    return 0;
}

/** \brief Draw a box on the canvas using CP437 characters.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcaca canvas.
 *  \param x X coordinate of the upper-left corner of the box.
 *  \param y Y coordinate of the upper-left corner of the box.
 *  \param w Width of the box.
 *  \param h Height of the box.
 *  \return This function always returns 0.
 */
int caca_draw_cp437_box(caca_canvas_t *cv, int x, int y, int w, int h)
{
    int i, j, xmax, ymax;

    int x2 = x + w - 1;
    int y2 = y + h - 1;

    if(x > x2)
    {
        int tmp = x;
        x = x2; x2 = tmp;
    }

    if(y > y2)
    {
        int tmp = y;
        y = y2; y2 = tmp;
    }

    xmax = cv->width - 1;
    ymax = cv->height - 1;

    if(x2 < 0 || y2 < 0 || x > xmax || y > ymax)
        return 0;

    /* Draw edges */
    if(y >= 0)
        for(i = x < 0 ? 1 : x + 1; i < x2 && i < xmax; i++)
            caca_put_char(cv, i, y, 0x2500); /* ─ */

    if(y2 <= ymax)
        for(i = x < 0 ? 1 : x + 1; i < x2 && i < xmax; i++)
            caca_put_char(cv, i, y2, 0x2500); /* ─ */

    if(x >= 0)
        for(j = y < 0 ? 1 : y + 1; j < y2 && j < ymax; j++)
            caca_put_char(cv, x, j, 0x2502); /* │ */

    if(x2 <= xmax)
        for(j = y < 0 ? 1 : y + 1; j < y2 && j < ymax; j++)
            caca_put_char(cv, x2, j, 0x2502); /* │ */

    /* Draw corners */
    caca_put_char(cv, x, y, 0x250c); /* ┌ */
    caca_put_char(cv, x, y2, 0x2514); /* └ */
    caca_put_char(cv, x2, y, 0x2510); /* ┐ */
    caca_put_char(cv, x2, y2, 0x2518); /* ┘ */

    return 0;
}

/** \brief Fill a box on the canvas using the given character.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcaca canvas.
 *  \param x X coordinate of the upper-left corner of the box.
 *  \param y Y coordinate of the upper-left corner of the box.
 *  \param w Width of the box.
 *  \param h Height of the box.
 *  \param ch UTF-32 character to be used to draw the box.
 *  \return This function always returns 0.
 */
int caca_fill_box(caca_canvas_t *cv, int x, int y, int w, int h,
                   uint32_t ch)
{
    int i, j, xmax, ymax;

    int x2 = x + w - 1;
    int y2 = y + h - 1;

    if(x > x2)
    {
        int tmp = x;
        x = x2; x2 = tmp;
    }

    if(y > y2)
    {
        int tmp = y;
        y = y2; y2 = tmp;
    }

    xmax = cv->width - 1;
    ymax = cv->height - 1;

    if(x2 < 0 || y2 < 0 || x > xmax || y > ymax)
        return 0;

    if(x < 0) x = 0;
    if(y < 0) y = 0;
    if(x2 > xmax) x2 = xmax;
    if(y2 > ymax) y2 = ymax;

    for(j = y; j <= y2; j++)
        for(i = x; i <= x2; i++)
            caca_put_char(cv, i, j, ch);

    return 0;
}

/*
 * XXX: The following functions are aliases.
 */

int cucul_draw_box(cucul_canvas_t *, int, int, int, int, uint32_t)
         CACA_ALIAS(caca_draw_box);
int cucul_draw_thin_box(cucul_canvas_t *, int, int, int, int)
         CACA_ALIAS(caca_draw_thin_box);
int cucul_draw_cp437_box(cucul_canvas_t *, int, int, int, int)
         CACA_ALIAS(caca_draw_cp437_box);
int cucul_fill_box(cucul_canvas_t *, int, int, int, int, uint32_t)
         CACA_ALIAS(caca_fill_box);

