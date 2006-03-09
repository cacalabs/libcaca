/*
 *  libcucul      Unicode canvas library
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file conic.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Ellipse and circle drawing
 *
 *  This file contains ellipse and circle drawing functions, both filled
 *  and outline.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

static void ellipsepoints(cucul_t *, int, int, int, int, char);

/**
 * \brief Draw a circle on the screen using the given character.
 *
 * \param x Center X coordinate.
 * \param y Center Y coordinate.
 * \param r Circle radius.
 * \param c Character to draw the circle outline with.
 * \return void
 */
void cucul_draw_circle(cucul_t *qq, int x, int y, int r, char c)
{
    int test, dx, dy;

    /* Optimized Bresenham. Kick ass. */
    for(test = 0, dx = 0, dy = r ; dx <= dy ; dx++)
    {
        ellipsepoints(qq, x, y, dx, dy, c);
        ellipsepoints(qq, x, y, dy, dx, c);

        test += test > 0 ? dx - dy-- : dx;
    }
}

/**
 * \brief Fill an ellipse on the screen using the given character.
 *
 * \param xo Center X coordinate.
 * \param yo Center Y coordinate.
 * \param a Ellipse X radius.
 * \param b Ellipse Y radius.
 * \param c Character to fill the ellipse with.
 * \return void
 */
void cucul_fill_ellipse(cucul_t *qq, int xo, int yo, int a, int b, char c)
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
            cucul_draw_line(qq, xo - x, yo - y, xo + x, yo - y, c);
            cucul_draw_line(qq, xo - x, yo + y, xo + x, yo + y, c);
            y--;
        }
        x++;
    }

    cucul_draw_line(qq, xo - x, yo - y, xo + x, yo - y, c);
    cucul_draw_line(qq, xo - x, yo + y, xo + x, yo + y, c);

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
        cucul_draw_line(qq, xo - x, yo - y, xo + x, yo - y, c);
        cucul_draw_line(qq, xo - x, yo + y, xo + x, yo + y, c);
    }
}

/**
 * \brief Draw an ellipse on the screen using the given character.
 *
 * \param xo Center X coordinate.
 * \param yo Center Y coordinate.
 * \param a Ellipse X radius.
 * \param b Ellipse Y radius.
 * \param c Character to draw the ellipse outline with.
 * \return void
 */
void cucul_draw_ellipse(cucul_t *qq, int xo, int yo, int a, int b, char c)
{
    int d2;
    int x = 0;
    int y = b;
    int d1 = b*b - (a*a*b) + (a*a/4);

    ellipsepoints(qq, xo, yo, x, y, c);

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
        ellipsepoints(qq, xo, yo, x, y, c);
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
        ellipsepoints(qq, xo, yo, x, y, c);
    }
}

/**
 * \brief Draw a thin ellipse on the screen.
 *
 * \param xo Center X coordinate.
 * \param yo Center Y coordinate.
 * \param a Ellipse X radius.
 * \param b Ellipse Y radius.
 * \return void
 */
void cucul_draw_thin_ellipse(cucul_t *qq, int xo, int yo, int a, int b)
{
    /* FIXME: this is not correct */
    int d2;
    int x = 0;
    int y = b;
    int d1 = b*b - (a*a*b) + (a*a/4);

    ellipsepoints(qq, xo, yo, x, y, '-');

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
        ellipsepoints(qq, xo, yo, x, y, '-');
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
        ellipsepoints(qq, xo, yo, x, y, '|');
    }
}

static void ellipsepoints(cucul_t *qq, int xo, int yo, int x, int y, char c)
{
    uint8_t b = 0;

    if(xo + x >= 0 && xo + x < (int)qq->width)
        b |= 0x1;
    if(xo - x >= 0 && xo - x < (int)qq->width)
        b |= 0x2;
    if(yo + y >= 0 && yo + y < (int)qq->height)
        b |= 0x4;
    if(yo - y >= 0 && yo - y < (int)qq->height)
        b |= 0x8;

    if((b & (0x1|0x4)) == (0x1|0x4))
        cucul_putchar(qq, xo + x, yo + y, c);

    if((b & (0x2|0x4)) == (0x2|0x4))
        cucul_putchar(qq, xo - x, yo + y, c);

    if((b & (0x1|0x8)) == (0x1|0x8))
        cucul_putchar(qq, xo + x, yo - y, c);

    if((b & (0x2|0x8)) == (0x2|0x8))
        cucul_putchar(qq, xo - x, yo - y, c);
}

