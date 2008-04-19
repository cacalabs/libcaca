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
 *  This file contains box drawing functions, both filled and outline.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

/** \brief Draw a box on the canvas using the given character.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcucul canvas.
 *  \param x X coordinate of the upper-left corner of the box.
 *  \param y Y coordinate of the upper-left corner of the box.
 *  \param w Width of the box.
 *  \param h Height of the box.
 *  \param ch UTF-32 character to be used to draw the box.
 *  \return This function always returns 0.
 */
int cucul_draw_box(cucul_canvas_t *cv, int x1, int y1, int w, int h, 
                   uint32_t ch)
{
    int x2 = x1 + w - 1;
    int y2 = y1 + h - 1;

    cucul_draw_line(cv, x1, y1, x1, y2, ch);
    cucul_draw_line(cv, x1, y2, x2, y2, ch);
    cucul_draw_line(cv, x2, y2, x2, y1, ch);
    cucul_draw_line(cv, x2, y1, x1, y1, ch);

    return 0;
}

/** \brief Draw a thin box on the canvas.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcucul canvas.
 *  \param x X coordinate of the upper-left corner of the box.
 *  \param y Y coordinate of the upper-left corner of the box.
 *  \param w Width of the box.
 *  \param h Height of the box.
 *  \return This function always returns 0.
 */
int cucul_draw_thin_box(cucul_canvas_t *cv, int x1, int y1, int w, int h)
{
    int x, y, xmax, ymax;

    int x2 = x1 + w - 1;
    int y2 = y1 + h - 1;

    if(x1 > x2)
    {
        int tmp = x1;
        x1 = x2; x2 = tmp;
    }

    if(y1 > y2)
    {
        int tmp = y1;
        y1 = y2; y2 = tmp;
    }

    xmax = cv->width - 1;
    ymax = cv->height - 1;

    if(x2 < 0 || y2 < 0 || x1 > xmax || y1 > ymax)
        return 0;

    /* Draw edges */
    if(y1 >= 0)
        for(x = x1 < 0 ? 1 : x1 + 1; x < x2 && x < xmax; x++)
            cucul_put_char(cv, x, y1, '-');

    if(y2 <= ymax)
        for(x = x1 < 0 ? 1 : x1 + 1; x < x2 && x < xmax; x++)
            cucul_put_char(cv, x, y2, '-');

    if(x1 >= 0)
        for(y = y1 < 0 ? 1 : y1 + 1; y < y2 && y < ymax; y++)
            cucul_put_char(cv, x1, y, '|');

    if(x2 <= xmax)
        for(y = y1 < 0 ? 1 : y1 + 1; y < y2 && y < ymax; y++)
            cucul_put_char(cv, x2, y, '|');

    /* Draw corners */
    cucul_put_char(cv, x1, y1, ',');
    cucul_put_char(cv, x1, y2, '`');
    cucul_put_char(cv, x2, y1, '.');
    cucul_put_char(cv, x2, y2, '\'');

    return 0;
}

/** \brief Draw a box on the canvas using CP437 characters.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcucul canvas.
 *  \param x X coordinate of the upper-left corner of the box.
 *  \param y Y coordinate of the upper-left corner of the box.
 *  \param w Width of the box.
 *  \param h Height of the box.
 *  \return This function always returns 0.
 */
int cucul_draw_cp437_box(cucul_canvas_t *cv, int x1, int y1, int w, int h)
{
    int x, y, xmax, ymax;

    int x2 = x1 + w - 1;
    int y2 = y1 + h - 1;

    if(x1 > x2)
    {
        int tmp = x1;
        x1 = x2; x2 = tmp;
    }

    if(y1 > y2)
    {
        int tmp = y1;
        y1 = y2; y2 = tmp;
    }

    xmax = cv->width - 1;
    ymax = cv->height - 1;

    if(x2 < 0 || y2 < 0 || x1 > xmax || y1 > ymax)
        return 0;

    /* Draw edges */
    if(y1 >= 0)
        for(x = x1 < 0 ? 1 : x1 + 1; x < x2 && x < xmax; x++)
            cucul_put_char(cv, x, y1, 0x2500); /* ─ */

    if(y2 <= ymax)
        for(x = x1 < 0 ? 1 : x1 + 1; x < x2 && x < xmax; x++)
            cucul_put_char(cv, x, y2, 0x2500); /* ─ */

    if(x1 >= 0)
        for(y = y1 < 0 ? 1 : y1 + 1; y < y2 && y < ymax; y++)
            cucul_put_char(cv, x1, y, 0x2502); /* │ */

    if(x2 <= xmax)
        for(y = y1 < 0 ? 1 : y1 + 1; y < y2 && y < ymax; y++)
            cucul_put_char(cv, x2, y, 0x2502); /* │ */

    /* Draw corners */
    cucul_put_char(cv, x1, y1, 0x250c); /* ┌ */
    cucul_put_char(cv, x1, y2, 0x2514); /* └ */
    cucul_put_char(cv, x2, y1, 0x2510); /* ┐ */
    cucul_put_char(cv, x2, y2, 0x2518); /* ┘ */

    return 0;
}

/** \brief Fill a box on the canvas using the given character.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcucul canvas.
 *  \param x X coordinate of the upper-left corner of the box.
 *  \param y Y coordinate of the upper-left corner of the box.
 *  \param w Width of the box.
 *  \param h Height of the box.
 *  \param ch UTF-32 character to be used to draw the box.
 *  \return This function always returns 0.
 */
int cucul_fill_box(cucul_canvas_t *cv, int x1, int y1, int w, int h,
                   uint32_t ch)
{
    int x, y, xmax, ymax;

    int x2 = x1 + w - 1;
    int y2 = y1 + h - 1;

    if(x1 > x2)
    {
        int tmp = x1;
        x1 = x2; x2 = tmp;
    }

    if(y1 > y2)
    {
        int tmp = y1;
        y1 = y2; y2 = tmp;
    }

    xmax = cv->width - 1;
    ymax = cv->height - 1;

    if(x2 < 0 || y2 < 0 || x1 > xmax || y1 > ymax)
        return 0;

    if(x1 < 0) x1 = 0;
    if(y1 < 0) y1 = 0;
    if(x2 > xmax) x2 = xmax;
    if(y2 > ymax) y2 = ymax;

    for(y = y1; y <= y2; y++)
        for(x = x1; x <= x2; x++)
            cucul_put_char(cv, x, y, ch);

    return 0;
}

