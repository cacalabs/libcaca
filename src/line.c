/*
 *  libcaca       ASCII-Art library
 *  Copyright (c) 2002, 2003 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA
 */

/** \file line.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Line drawing
 *
 *  This file contains line and polyline drawing functions, with both thin
 *  and thick styles.
 */

#include "config.h"

#if defined(HAVE_INTTYPES_H) || defined(_DOXYGEN_SKIP_ME)
#   include <inttypes.h>
#else
typedef unsigned char uint8_t;
#endif

#include <stdlib.h>

#include "caca.h"
#include "caca_internals.h"

#if !defined(_DOXYGEN_SKIP_ME)
struct line
{
    int x1, y1;
    int x2, y2;
    char c;
    void (*draw) (struct line*);
};
#endif

static void clip_line(struct line*);
static uint8_t clip_bits(int, int);
static void draw_solid_line(struct line*);
static void draw_thin_line(struct line*);

/**
 * \brief Draw a line on the screen using the given character.
 *
 * \param x1 X coordinate of the first point.
 * \param y1 Y coordinate of the first point.
 * \param x2 X coordinate of the second point.
 * \param y2 Y coordinate of the second point.
 * \param c Character to draw the line with.
 * \return void
 */
void caca_draw_line(int x1, int y1, int x2, int y2, char c)
{
    struct line s;
    s.x1 = x1;
    s.y1 = y1;
    s.x2 = x2;
    s.y2 = y2;
    s.c = c;
    s.draw = draw_solid_line;
    clip_line(&s);
}

/**
 * \brief Draw a polyline on the screen using the given character and
 *       coordinate arrays. The first and last points are not connected,
 *       so in order to draw a polygon you need to specify the starting
 *       point at the end of the list as well.
 *
 * \param x Array of X coordinates. Must have \p n + 1 elements.
 * \param y Array of Y coordinates. Must have \p n + 1 elements.
 * \param n Number of lines to draw.
 * \param c Character to draw the lines with.
 * \return void
 */
void caca_draw_polyline(int const x[], int const y[], int n, char c)
{
    int i;
    struct line s;
    s.c = c;
    s.draw = draw_solid_line;

    for(i = 0; i < n; i++)
    {
        s.x1 = x[i];
        s.y1 = y[i];
        s.x2 = x[i+1];
        s.y2 = y[i+1];
        clip_line(&s);
    }
}

/**
 * \brief Draw a thin line on the screen, using ASCII art.
 *
 * \param x1 X coordinate of the first point.
 * \param y1 Y coordinate of the first point.
 * \param x2 X coordinate of the second point.
 * \param y2 Y coordinate of the second point.
 * \return void
 */
void caca_draw_thin_line(int x1, int y1, int x2, int y2)
{
    struct line s;
    s.x1 = x1;
    s.y1 = y1;
    s.x2 = x2;
    s.y2 = y2;
    s.draw = draw_thin_line;
    clip_line(&s);
}

/**
 * \brief Draw a thin polyline on the screen using the given coordinate
 *       arrays and with ASCII art. The first and last points are not
 *       connected, so in order to draw a polygon you need to specify the
 *       starting point at the end of the list as well.
 *
 * \param x Array of X coordinates. Must have \p n + 1 elements.
 * \param y Array of Y coordinates. Must have \p n + 1 elements.
 * \param n Number of lines to draw.
 * \return void
 */
void caca_draw_thin_polyline(int const x[], int const y[], int n)
{
    int i;
    struct line s;
    s.draw = draw_thin_line;

    for(i = 0; i < n; i++)
    {
        s.x1 = x[i];
        s.y1 = y[i];
        s.x2 = x[i+1];
        s.y2 = y[i+1];
        clip_line(&s);
    }
}

/*
 * XXX: The following functions are local.
 */

/**
 * \brief Generic Cohen-Sutherland line clipping function.
 *
 * \param s a line structure
 * \return void
 */
static void clip_line(struct line* s)
{
    uint8_t bits1, bits2;

    bits1 = clip_bits(s->x1, s->y1);
    bits2 = clip_bits(s->x2, s->y2);

    if(bits1 & bits2)
        return;

    if(bits1 == 0)
    {
        if(bits2 == 0)
            s->draw(s);
        else
        {
            int tmp;
            tmp = s->x1; s->x1 = s->x2; s->x2 = tmp;
            tmp = s->y1; s->y1 = s->y2; s->y2 = tmp;
            clip_line(s);
        }

        return;
    }

    if(bits1 & (1<<0))
    {
        s->y1 = s->y2 - (s->x2 - 0) * (s->y2 - s->y1) / (s->x2 - s->x1);
        s->x1 = 0;
    }
    else if(bits1 & (1<<1))
    {
        int xmax = _caca_width - 1;
        s->y1 = s->y2 - (s->x2 - xmax) * (s->y2 - s->y1) / (s->x2 - s->x1);
        s->x1 = xmax;
    }
    else if(bits1 & (1<<2))
    {
        s->x1 = s->x2 - (s->y2 - 0) * (s->x2 - s->x1) / (s->y2 - s->y1);
        s->y1 = 0;
    }
    else if(bits1 & (1<<3))
    {
        int ymax = _caca_height - 1;
        s->x1 = s->x2 - (s->y2 - ymax) * (s->x2 - s->x1) / (s->y2 - s->y1);
        s->y1 = ymax;
    }

    clip_line(s);
}

/**
 * \brief Helper function for clip_line().
 *
 * \param x X coordinate of the point.
 * \param y Y coordinate of the point.
 * \return The clipping bits for the given point.
 */
static uint8_t clip_bits(int x, int y)
{
    uint8_t b = 0;

    if(x < 0)
        b |= (1<<0);
    else if(x >= (int)_caca_width)
        b |= (1<<1);

    if(y < 0)
        b |= (1<<2);
    else if(y >= (int)_caca_height)
        b |= (1<<3);

    return b;
}

/**
 * \brief Solid line drawing function, using Bresenham's mid-point line
 *       scan-conversion algorithm.
 *
 * \param s a line structure
 * \return void
 */
static void draw_solid_line(struct line* s)
{
    int x1, y1, x2, y2;
    int dx, dy;
    int xinc, yinc;

    x1 = s->x1; y1 = s->y1; x2 = s->x2; y2 = s->y2;

    dx = abs(x2 - x1);
    dy = abs(y2 - y1);

    xinc = (x1 > x2) ? -1 : 1;
    yinc = (y1 > y2) ? -1 : 1;

    if(dx >= dy)
    {
        int dpr = dy << 1;
        int dpru = dpr - (dx << 1);
        int delta = dpr - dx;

        for(; dx>=0; dx--)
        {
            caca_putchar(x1, y1, s->c);
            if(delta > 0)
            {
                x1 += xinc;
                y1 += yinc;
                delta += dpru;
            }
            else
            {
                x1 += xinc;
                delta += dpr;
            }
        }
    }
    else
    {
        int dpr = dx << 1;
        int dpru = dpr - (dy << 1);
        int delta = dpr - dy;

        for(; dy >= 0; dy--)
        {
            caca_putchar(x1, y1, s->c);
            if(delta > 0)
            {
                x1 += xinc;
                y1 += yinc;
                delta += dpru;
            }
            else
            {
                y1 += yinc;
                delta += dpr;
            }
        }
    }
}

/**
 * \brief Thin line drawing function, using Bresenham's mid-point line
 *        scan-conversion algorithm and ASCII art graphics.
 *
 * \param s a line structure
 * \return void
 */
static void draw_thin_line(struct line* s)
{
    char *charmapx, *charmapy;
    int x1, y1, x2, y2;
    int dx, dy;
    int yinc;

    if(s->x2 >= s->x1)
    {
        if(s->y1 > s->y2)
            charmapx = ",'";
        else
            charmapx = "`.";
        x1 = s->x1; y1 = s->y1; x2 = s->x2; y2 = s->y2;
    }
    else
    {
        if(s->y1 > s->y2)
            charmapx = "`.";
        else
            charmapx = ",'";
        x2 = s->x1; y2 = s->y1; x1 = s->x2; y1 = s->y2;
    }

    dx = abs(x2 - x1);
    dy = abs(y2 - y1);

    if(y1 > y2)
    {
        charmapy = ",'";
        yinc = -1;
    }
    else
    {
        yinc = 1;
        charmapy = "`.";
    }

    if(dx >= dy)
    {
        int dpr = dy << 1;
        int dpru = dpr - (dx << 1);
        int delta = dpr - dx;
        int prev = 0;

        for(; dx>=0; dx--)
        {
            if(delta > 0)
            {
                caca_putchar(x1, y1, charmapy[1]);
                x1++;
                y1 += yinc;
                delta += dpru;
                prev = 1;
            }
            else
            {
                if(prev)
                    caca_putchar(x1, y1, charmapy[0]);
                else
                    caca_putchar(x1, y1, '-');
                x1++;
                delta += dpr;
                prev = 0;
            }
        }
    }
    else
    {
        int dpr = dx << 1;
        int dpru = dpr - (dy << 1);
        int delta = dpr - dy;

        for(; dy >= 0; dy--)
        {
            if(delta > 0)
            {
                caca_putchar(x1, y1, charmapx[0]);
                caca_putchar(x1 + 1, y1, charmapx[1]);
                x1++;
                y1 += yinc;
                delta += dpru;
            }
            else
            {
                caca_putchar(x1, y1, '|');
                y1 += yinc;
                delta += dpr;
            }
        }
    }
}

