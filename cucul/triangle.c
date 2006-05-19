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
 *  This file contains triangle drawing functions, both filled and outline.
 */

#include "config.h"
#include "common.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

/** \brief Draw a triangle on the canvas using the given character.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcucul canvas.
 *  \param x1 X coordinate of the first point.
 *  \param y1 Y coordinate of the first point.
 *  \param x2 X coordinate of the second point.
 *  \param y2 Y coordinate of the second point.
 *  \param x3 X coordinate of the third point.
 *  \param y3 Y coordinate of the third point.
 *  \param str UTF-8 string representing the character that should be used
 *         to draw the triangle outline.
 *  \return This function always returns 0.
 */
int cucul_draw_triangle(cucul_canvas_t *cv, int x1, int y1, int x2, int y2,
                        int x3, int y3, char const *str)
{
    cucul_draw_line(cv, x1, y1, x2, y2, str);
    cucul_draw_line(cv, x2, y2, x3, y3, str);
    cucul_draw_line(cv, x3, y3, x1, y1, str);

    return 0;
}

/** \brief Draw a thin triangle on the canvas.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcucul canvas.
 *  \param x1 X coordinate of the first point.
 *  \param y1 Y coordinate of the first point.
 *  \param x2 X coordinate of the second point.
 *  \param y2 Y coordinate of the second point.
 *  \param x3 X coordinate of the third point.
 *  \param y3 Y coordinate of the third point.
 *  \return This function always returns 0.
 */
int cucul_draw_thin_triangle(cucul_canvas_t *cv, int x1, int y1,
                             int x2, int y2, int x3, int y3)
{
    cucul_draw_thin_line(cv, x1, y1, x2, y2);
    cucul_draw_thin_line(cv, x2, y2, x3, y3);
    cucul_draw_thin_line(cv, x3, y3, x1, y1);

    return 0;
}

/** \brief Fill a triangle on the canvas using the given character.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcucul canvas.
 *  \param x1 X coordinate of the first point.
 *  \param y1 Y coordinate of the first point.
 *  \param x2 X coordinate of the second point.
 *  \param y2 Y coordinate of the second point.
 *  \param x3 X coordinate of the third point.
 *  \param y3 Y coordinate of the third point.
 *  \param str UTF-8 string representing the character that should be used
 *         to fill the triangle.
 *  \return This function always returns 0.
 */
int cucul_fill_triangle(cucul_canvas_t *cv, int x1, int y1, int x2, int y2,
                        int x3, int y3, char const *str)
{
    int x, y, xa, xb, xmax, ymax;
    uint32_t ch;

    /* Bubble-sort y1 <= y2 <= y3 */
    if(y1 > y2)
        return cucul_fill_triangle(cv, x2, y2, x1, y1, x3, y3, str);

    if(y2 > y3)
        return cucul_fill_triangle(cv, x1, y1, x3, y3, x2, y2, str);

    /* Promote precision */
    x1 *= 4;
    x2 *= 4;
    x3 *= 4;

    xmax = cv->width - 1;
    ymax = cv->height - 1;

    ch = cucul_utf8_to_utf32(str);

    /* Rasterize our triangle */
    for(y = y1 < 0 ? 0 : y1; y <= y3 && y <= ymax; y++)
    {
        if(y <= y2)
        {
            xa = (y1 == y2) ? x2 : x1 + (x2 - x1) * (y - y1) / (y2 - y1);
            xb = (y1 == y3) ? x3 : x1 + (x3 - x1) * (y - y1) / (y3 - y1);
        }
        else
        {
            xa = (y3 == y2) ? x2 : x3 + (x2 - x3) * (y - y3) / (y2 - y3);
            xb = (y3 == y1) ? x1 : x3 + (x1 - x3) * (y - y3) / (y1 - y3);
        }

        if(xb < xa)
        {
            int tmp = xb;
            xb = xa; xa = tmp;
        }

        /* Rescale xa and xb, slightly cropping */
        xa = (xa + 2) / 4;
        xb = (xb - 2) / 4;

        if(xb < 0) continue;
        if(xa > xmax) continue;
        if(xa < 0) xa = 0;
        if(xb > xmax) xb = xmax;

        for(x = xa; x <= xb; x++)
            cucul_putchar(cv, x, y, ch);
    }

    return 0;
}

