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
 *  This file contains box drawing functions, both filled and outline.
 */

#include "config.h"
#include "common.h"

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
 *  \param x1 X coordinate of the upper-left corner of the box.
 *  \param y1 Y coordinate of the upper-left corner of the box.
 *  \param x2 X coordinate of the lower-right corner of the box.
 *  \param y2 Y coordinate of the lower-right corner of the box.
 *  \param str UTF-8 string containing the character to use to draw the box.
 *  \return This function always returns 0.
 */
int cucul_draw_box(cucul_canvas_t *cv, int x1, int y1, int x2, int y2, 
                   char const *str)
{
    cucul_draw_line(cv, x1, y1, x1, y2, str);
    cucul_draw_line(cv, x1, y2, x2, y2, str);
    cucul_draw_line(cv, x2, y2, x2, y1, str);
    cucul_draw_line(cv, x2, y1, x1, y1, str);

    return 0;
}

/** \brief Draw a thin box on the canvas.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcucul canvas.
 *  \param x1 X coordinate of the upper-left corner of the box.
 *  \param y1 Y coordinate of the upper-left corner of the box.
 *  \param x2 X coordinate of the lower-right corner of the box.
 *  \param y2 Y coordinate of the lower-right corner of the box.
 *  \return This function always returns 0.
 */
int cucul_draw_thin_box(cucul_canvas_t *cv, int x1, int y1, int x2, int y2)
{
    int x, y, xmax, ymax;

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
            cucul_putchar(cv, x, y1, '-');

    if(y2 <= ymax)
        for(x = x1 < 0 ? 1 : x1 + 1; x < x2 && x < xmax; x++)
            cucul_putchar(cv, x, y2, '-');

    if(x1 >= 0)
        for(y = y1 < 0 ? 1 : y1 + 1; y < y2 && y < ymax; y++)
            cucul_putchar(cv, x1, y, '|');

    if(x2 <= xmax)
        for(y = y1 < 0 ? 1 : y1 + 1; y < y2 && y < ymax; y++)
            cucul_putchar(cv, x2, y, '|');

    /* Draw corners */
    if(x1 >= 0 && y1 >= 0)
        cucul_putchar(cv, x1, y1, ',');

    if(x1 >= 0 && y2 <= ymax)
        cucul_putchar(cv, x1, y2, '`');

    if(x2 <= xmax && y1 >= 0)
        cucul_putchar(cv, x2, y1, '.');

    if(x2 <= xmax && y2 <= ymax)
        cucul_putchar(cv, x2, y2, '\'');

    return 0;
}

/** \brief Fill a box on the canvas using the given character.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcucul canvas.
 *  \param x1 X coordinate of the upper-left corner of the box.
 *  \param y1 Y coordinate of the upper-left corner of the box.
 *  \param x2 X coordinate of the lower-right corner of the box.
 *  \param y2 Y coordinate of the lower-right corner of the box.
 *  \param str UTF-8 string containing the character to fill the box with.
 *  \return This function always returns 0.
 */
int cucul_fill_box(cucul_canvas_t *cv, int x1, int y1, int x2, int y2,
                   char const *str)
{
    int x, y, xmax, ymax;
    uint32_t ch;

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

    ch = _cucul_utf8_to_utf32(str);

    for(y = y1; y <= y2; y++)
        for(x = x1; x <= x2; x++)
            cucul_putchar(cv, x, y, ch);

    return 0;
}

