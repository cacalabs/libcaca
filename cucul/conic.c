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
 *  This file contains ellipse and circle drawing functions, both filled
 *  and outline.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

static void ellipsepoints(cucul_canvas_t *, int, int, int, int, uint32_t);

/** \brief Draw a circle on the canvas using the given character.
 *
 *  \param c The handle to the libcucul canvas.
 *  \param x Center X coordinate.
 *  \param y Center Y coordinate.
 *  \param r Circle radius.
 *  \param str UTF-8 string representing the character that should be used
 *         to draw the circle outline.
 *  \return void
 */
void cucul_draw_circle(cucul_canvas_t *c, int x, int y, int r, char const *str)
{
    int test, dx, dy;
    uint32_t ch = _cucul_utf8_to_utf32(str);

    /* Optimized Bresenham. Kick ass. */
    for(test = 0, dx = 0, dy = r ; dx <= dy ; dx++)
    {
        ellipsepoints(c, x, y, dx, dy, ch);
        ellipsepoints(c, x, y, dy, dx, ch);

        test += test > 0 ? dx - dy-- : dx;
    }
}

/** \brief Fill an ellipse on the canvas using the given character.
 *
 *  \param c The handle to the libcucul canvas.
 *  \param xo Center X coordinate.
 *  \param yo Center Y coordinate.
 *  \param a Ellipse X radius.
 *  \param b Ellipse Y radius.
 *  \param str UTF-8 string representing the character that should be used
 *         to fill the ellipse.
 *  \return void
 */
void cucul_fill_ellipse(cucul_canvas_t *c, int xo, int yo, int a, int b,
                        char const *str)
{
    int d2;
    int x = 0;
    int y = b;
    int d1 = b*b - (a*a*b) + (a*a/4);

    while(a*a*y - a*a/2 > b*b*(x+1))
    {
        if(d1 < 0)
        {
            d1 += b*b*(2*x+1); /* XXX: "Computer Graphics" has + 3 here. */
        }
        else
        {
            d1 += b*b*(2*x*1) + a*a*(-2*y+2);
            cucul_draw_line(c, xo - x, yo - y, xo + x, yo - y, str);
            cucul_draw_line(c, xo - x, yo + y, xo + x, yo + y, str);
            y--;
        }
        x++;
    }

    cucul_draw_line(c, xo - x, yo - y, xo + x, yo - y, str);
    cucul_draw_line(c, xo - x, yo + y, xo + x, yo + y, str);

    d2 = b*b*(x+0.5)*(x+0.5) + a*a*(y-1)*(y-1) - a*a*b*b;
    while(y > 0)
    {
        if(d2 < 0)
        {
            d2 += b*b*(2*x+2) + a*a*(-2*y+3);
            x++;
        }
        else
        {
            d2 += a*a*(-2*y+3);
        }

        y--;
        cucul_draw_line(c, xo - x, yo - y, xo + x, yo - y, str);
        cucul_draw_line(c, xo - x, yo + y, xo + x, yo + y, str);
    }
}

/** \brief Draw an ellipse on the canvas using the given character.
 *
 *  \param c The handle to the libcucul canvas.
 *  \param xo Center X coordinate.
 *  \param yo Center Y coordinate.
 *  \param a Ellipse X radius.
 *  \param b Ellipse Y radius.
 *  \param str UTF-8 string representing the character that should be used
 *         to draw the ellipse outline.
 *  \return void
 */
void cucul_draw_ellipse(cucul_canvas_t *c, int xo, int yo, int a, int b,
                        char const *str)
{
    int d2;
    int x = 0;
    int y = b;
    int d1 = b*b - (a*a*b) + (a*a/4);
    uint32_t ch = _cucul_utf8_to_utf32(str);

    ellipsepoints(c, xo, yo, x, y, ch);

    while(a*a*y - a*a/2 > b*b*(x+1))
    {
        if(d1 < 0)
        {
            d1 += b*b*(2*x+1); /* XXX: "Computer Graphics" has + 3 here. */
        }
        else
        {
            d1 += b*b*(2*x*1) + a*a*(-2*y+2);
            y--;
        }
        x++;
        ellipsepoints(c, xo, yo, x, y, ch);
    }

    d2 = b*b*(x+0.5)*(x+0.5) + a*a*(y-1)*(y-1) - a*a*b*b;
    while(y > 0)
    {
        if(d2 < 0)
        {
            d2 += b*b*(2*x+2) + a*a*(-2*y+3);
            x++;
        }
        else
        {
            d2 += a*a*(-2*y+3);
        }

        y--;
        ellipsepoints(c, xo, yo, x, y, ch);
    }
}

/** \brief Draw a thin ellipse on the canvas.
 *
 *  \param c The handle to the libcucul canvas.
 *  \param xo Center X coordinate.
 *  \param yo Center Y coordinate.
 *  \param a Ellipse X radius.
 *  \param b Ellipse Y radius.
 *  \return void
 */
void cucul_draw_thin_ellipse(cucul_canvas_t *c, int xo, int yo, int a, int b)
{
    /* FIXME: this is not correct */
    int d2;
    int x = 0;
    int y = b;
    int d1 = b*b - (a*a*b) + (a*a/4);

    ellipsepoints(c, xo, yo, x, y, '-');

    while(a*a*y - a*a/2 > b*b*(x+1))
    {
        if(d1 < 0)
        {
            d1 += b*b*(2*x+1); /* XXX: "Computer Graphics" has + 3 here. */
        }
        else
        {
            d1 += b*b*(2*x*1) + a*a*(-2*y+2);
            y--;
        }
        x++;
        ellipsepoints(c, xo, yo, x, y, '-');
    }

    d2 = b*b*(x+0.5)*(x+0.5) + a*a*(y-1)*(y-1) - a*a*b*b;
    while(y > 0)
    {
        if(d2 < 0)
        {
            d2 += b*b*(2*x+2) + a*a*(-2*y+3);
            x++;
        }
        else
        {
            d2 += a*a*(-2*y+3);
        }

        y--;
        ellipsepoints(c, xo, yo, x, y, '|');
    }
}

static void ellipsepoints(cucul_canvas_t *c, int xo, int yo, int x, int y,
                          uint32_t ch)
{
    uint8_t b = 0;

    if(xo + x >= 0 && xo + x < (int)c->width)
        b |= 0x1;
    if(xo - x >= 0 && xo - x < (int)c->width)
        b |= 0x2;
    if(yo + y >= 0 && yo + y < (int)c->height)
        b |= 0x4;
    if(yo - y >= 0 && yo - y < (int)c->height)
        b |= 0x8;

    if((b & (0x1|0x4)) == (0x1|0x4))
        _cucul_putchar32(c, xo + x, yo + y, ch);

    if((b & (0x2|0x4)) == (0x2|0x4))
        _cucul_putchar32(c, xo - x, yo + y, ch);

    if((b & (0x1|0x8)) == (0x1|0x8))
        _cucul_putchar32(c, xo + x, yo - y, ch);

    if((b & (0x2|0x8)) == (0x2|0x8))
        _cucul_putchar32(c, xo - x, yo - y, ch);
}

