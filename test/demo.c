/*
 *   demo          demo using libee
 *   Copyright (c) 2003 Sam Hocevar <sam@zoy.org>
 *                 All Rights Reserved
 *
 *   $Id$
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "config.h"

#include <math.h>
#include <string.h>

#include "ee.h"

static void display_menu(void);

static void demo_all(void);

static void demo_dots(void);
static void demo_lines(void);
static void demo_thin_lines(void);
static void demo_circles(void);
static void demo_ellipses(void);
static void demo_triangles(void);
static void demo_outlined_triangles(void);
static void demo_sprites(void);

int force_clipping = 0;
struct ee_sprite *sprite = NULL;

int main(int argc, char **argv)
{
    void (*demo)(void) = NULL;
    int quit = 0;

    if(ee_init())
    {
        return 1;
    }

    /* Initialize data */
    sprite = ee_load_sprite("data/bar_boss");

    /* Main menu */
    display_menu();

    /* Go ! */
    while(!quit)
    {
        char key = ee_get_key();
        if(key && demo)
        {
            display_menu();
            demo = NULL;
        }
        else if(key)
        {
            switch(key)
            {
            case 'q':
                demo = NULL;
                quit = 1;
                break;
            case '0':
                ee_clear();
                demo = demo_all;
                break;
            case '1':
                ee_clear();
                demo = demo_dots;
                break;
            case '2':
                ee_clear();
                demo = demo_lines;
                break;
            case '3':
                ee_clear();
                demo = demo_thin_lines;
                break;
            case '4':
                ee_clear();
                demo = demo_circles;
                break;
            case '5':
                ee_clear();
                demo = demo_ellipses;
                break;
            case '6':
                ee_clear();
                demo = demo_triangles;
                break;
            case '7':
                ee_clear();
                demo = demo_outlined_triangles;
                break;
            case '8':
                ee_clear();
                demo = demo_sprites;
                break;
            }
        }

        if(demo)
            demo();
    }

    /* Clean up */
    ee_free_sprite(sprite);
    ee_end();

    return 0;
}

static void display_menu(void)
{
    int xo = ee_get_width() - 2;
    int yo = ee_get_height() - 2;

    ee_clear();
    ee_color(EE_WHITE);
    ee_draw_line(xo, yo, 1, yo, '.');
    ee_draw_line(1, yo, 1, 1, ':');
    ee_draw_line(xo, 1, xo, yo, ':');
    ee_draw_line(1, 1, xo, 1, '.');

    ee_putstr((xo - strlen("libee demo")) / 2, 3, "libee demo");
    ee_putstr((xo - strlen("============")) / 2, 4, "============");

    ee_putstr(4, 6, "0: complete demo");
    ee_putstr(4, 7, "1: dots demo");
    ee_putstr(4, 8, "2: lines demo");
    ee_putstr(4, 9, "3: thin lines demo");
    ee_putstr(4, 10, "4: circles demo");
    ee_putstr(4, 11, "5: ellipses demo");
    ee_putstr(4, 12, "6: triangles demo");
    ee_putstr(4, 13, "7: outlined triangles demo");
    ee_putstr(4, 14, "8: sprites demo");

    ee_putstr(4, yo - 2, "q: quit");

    ee_refresh();
}

static void demo_all(void)
{
    static int i = 0;

    int j, xo, yo, x1, y1, x2, y2, x3, y3;

    i++;

    ee_clear();

    /* Draw the sun */
    ee_color(EE_YELLOW);
    xo = ee_get_width() / 4;
    yo = ee_get_height() / 4 + 5 * sin(0.03*i);

    for(j = 0; j < 16; j++)
    {
        x1 = xo - (30 + sin(0.03*i) * 8) * sin(0.03*i + M_PI*j/8);
        y1 = yo + (15 + sin(0.03*i) * 4) * cos(0.03*i + M_PI*j/8);
        ee_draw_thin_line(xo, yo, x1, y1);
    }

    ee_color(EE_WHITE);
    for(j = 15 + sin(0.03*i) * 8; j--;)
    {
        ee_draw_circle(xo, yo, j, '#');
    }

    ee_color(EE_YELLOW);
    ee_draw_circle(xo, yo, 15 + sin(0.03*i) * 8, '#');

    /* Draw the pyramid */
    xo = ee_get_width() * 5 / 8;
    yo = 2;

    x1 = ee_get_width() / 8 + sin(0.03*i) * 5;
    y1 = ee_get_height() / 2 + cos(0.03*i) * 5;

    x2 = ee_get_width() - 10 - cos(0.02*i) * 10;
    y2 = ee_get_height() * 3 / 4 - 5 + sin(0.02*i) * 5;

    x3 = ee_get_width() / 4 - sin(0.02*i) * 5;
    y3 = ee_get_height() * 3 / 4 + cos(0.02*i) * 5;

    ee_color(EE_GREEN);
    ee_fill_triangle(xo, yo, x2, y2, x1, y1, '%');
    ee_color(EE_YELLOW);
    ee_draw_thin_line(xo, yo, x2, y2);
    ee_draw_thin_line(x2, y2, x1, y1);
    ee_draw_thin_line(x1, y1, xo, yo);

    ee_color(EE_RED);
    ee_fill_triangle(x1, y1, x2, y2, x3, y3, '#');
    ee_color(EE_YELLOW);
    ee_draw_thin_line(x1, y1, x2, y2);
    ee_draw_thin_line(x2, y2, x3, y3);
    ee_draw_thin_line(x3, y3, x1, y1);

    ee_color(EE_BLUE);
    ee_fill_triangle(xo, yo, x2, y2, x3, y3, '%');
    ee_color(EE_YELLOW);
    ee_draw_thin_line(xo, yo, x2, y2);
    ee_draw_thin_line(x2, y2, x3, y3);
    ee_draw_thin_line(x3, y3, xo, yo);

    /* Draw a background triangle */
    x1 = 2;
    y1 = 2;

    x2 = ee_get_width() - 3;
    y2 = ee_get_height() / 2;

    x3 = ee_get_width() / 3;
    y3 = ee_get_height() - 3;

    ee_color(EE_BLUE);
//    ee_fill_triangle(x1, y1, x2, y2, x3, y3, '.');
    ee_color(EE_CYAN);
    ee_draw_thin_line(x1, y1, x3, y3);
    ee_draw_thin_line(x2, y2, x1, y1);
    ee_draw_thin_line(x3, y3, x2, y2);

    xo = ee_get_width() / 2 + cos(0.027*i) * ee_get_width() / 3;
    yo = ee_get_height() / 2 - sin(0.027*i) * ee_get_height() / 2;

    ee_draw_thin_line(x1, y1, xo, yo);
    ee_draw_thin_line(x2, y2, xo, yo);
    ee_draw_thin_line(x3, y3, xo, yo);

    /* Draw a sprite behind the pyramid */
    ee_draw_sprite(xo, yo, sprite);

    /* Draw a trail behind the foreground sprite */
    for(j = i - 60; j < i; j++)
    {
        int delta = ee_rand(-5, 5);
        ee_color(ee_rand(1, 10));
        ee_putchar(ee_get_width() / 2
                    + cos(0.02*j) * (delta + ee_get_width() / 4),
                   ee_get_height() / 2
                    + sin(0.02*j) * (delta + ee_get_height() / 3),
                   '#');
    }

    /* Draw foreground sprite */
    ee_draw_sprite(ee_get_width() / 2 + cos(0.02*i) * ee_get_width() / 4,
                   ee_get_height() / 2 + sin(0.02*i) * ee_get_height() / 3,
                   sprite);

    ee_refresh();
}

static void demo_dots(void)
{
    int xmax = ee_get_width() - 1;
    int ymax = ee_get_height() - 1;
    int i;

    for(i = 1000; i--;)
    {
        /* Putpixel */
        ee_color(ee_rand(1, 10));
        ee_putchar(ee_rand(0, xmax), ee_rand(0, ymax), '#');
    }
    ee_refresh();
}

static void demo_lines(void)
{
    int w = ee_get_width();
    int h = ee_get_height();

    /* Draw lines */
    ee_color(ee_rand(1, 10));
    if(force_clipping)
    {
        ee_draw_line(ee_rand(- w, 2 * w), ee_rand(- h, 2 * h),
                     ee_rand(- w, 2 * w), ee_rand(- h, 2 * h), '#');
    }
    else
    {
        ee_draw_line(ee_rand(0, w - 1), ee_rand(0, h - 1),
                     ee_rand(0, w - 1), ee_rand(0, h - 1), '#');
    }
    ee_refresh();
}

static void demo_thin_lines(void)
{
    int w = ee_get_width();
    int h = ee_get_height();

    /* Draw lines */
    ee_color(ee_rand(1, 10));
    if(force_clipping)
    {
        ee_draw_thin_line(ee_rand(- w, 2 * w), ee_rand(- h, 2 * h),
                          ee_rand(- w, 2 * w), ee_rand(- h, 2 * h));
    }
    else
    {
        ee_draw_thin_line(ee_rand(0, w), ee_rand(0, h),
                          ee_rand(0, w), ee_rand(0, h));
    }
    ee_refresh();
}

static void demo_circles(void)
{
    int w = ee_get_width();
    int h = ee_get_height();

    /* Draw circles */
    if(force_clipping)
    {
        ee_color(ee_rand(1, 10));
        ee_draw_circle(ee_rand(- w, 2 * w),
                       ee_rand(- h, 2 * h),
                       ee_rand(0, (w + h) / 2),
                       '#');
    }
    else
    {
        int x = ee_rand(0, w);
        int y = ee_rand(0, h);
        int r = ee_rand(0, (w + h) / 2);

        if(x - r < 0 || x + r >= w || y - r < 0 || y + r >= h)
        {
            demo_circles();
            return;
        }

        ee_color(ee_rand(1, 10));
        ee_draw_circle(x, y, r, '#');
    }

    ee_refresh();
}

static void demo_ellipses(void)
{
    int w = ee_get_width();
    int h = ee_get_height();

    /* Draw circles */
    if(force_clipping)
    {
        ee_color(ee_rand(1, 10));
        ee_draw_ellipse(ee_rand(- w, 2 * w),
                        ee_rand(- h, 2 * h),
                        ee_rand(0, w),
                        ee_rand(0, h),
                        '#');
    }
    else
    {
        int x = ee_rand(0, w);
        int y = ee_rand(0, h);
        int a = ee_rand(0, w);
        int b = ee_rand(0, h);

        if(x - a < 0 || x + a >= w || y - b < 0 || y + b >= h)
        {
            demo_ellipses();
            return;
        }

        ee_color(ee_rand(1, 10));
        ee_draw_ellipse(x, y, a, b, '#');
    }

    ee_refresh();
}

static void demo_triangles(void)
{
    int w = ee_get_width();
    int h = ee_get_height();

    /* Draw lines */
    ee_color(ee_rand(1, 10));
    if(force_clipping)
    {
        ee_fill_triangle(ee_rand(- w, 2 * w), ee_rand(- h, 2 * h),
                         ee_rand(- w, 2 * w), ee_rand(- h, 2 * h),
                         ee_rand(- w, 2 * w), ee_rand(- h, 2 * h), '#');
    }
    else
    {
        ee_fill_triangle(ee_rand(0, w - 1), ee_rand(0, h - 1),
                         ee_rand(0, w - 1), ee_rand(0, h - 1),
                         ee_rand(0, w - 1), ee_rand(0, h - 1), '#');
    }
    ee_refresh();
}

static void demo_outlined_triangles(void)
{
    int w = ee_get_width();
    int h = ee_get_height();
    int x1, y1, x2, y2, x3, y3;

    /* Draw lines */
    ee_color(ee_rand(1, 10));
    if(force_clipping)
    {
        x1 = ee_rand(- w, 2 * w);
        y1 = ee_rand(- h, 2 * h);
        x2 = ee_rand(- w, 2 * w);
        y2 = ee_rand(- h, 2 * h);
        x3 = ee_rand(- w, 2 * w);
        y3 = ee_rand(- h, 2 * h);
    }
    else
    {

        x1 = ee_rand(0, w - 1);
        y1 = ee_rand(0, h - 1);
        x2 = ee_rand(0, w - 1);
        y2 = ee_rand(0, h - 1);
        x3 = ee_rand(0, w - 1);
        y3 = ee_rand(0, h - 1);
    }

    ee_fill_triangle(x1, y1, x2, y2, x3, y3, '#');

    ee_color(ee_rand(1, 10));
    ee_draw_thin_line(x1, y1, x2, y2);
    ee_draw_thin_line(x2, y2, x3, y3);
    ee_draw_thin_line(x3, y3, x1, y1);

    ee_refresh();
}

static void demo_sprites(void)
{
    ee_draw_sprite(ee_rand(0, ee_get_width() - 1),
                   ee_rand(0, ee_get_height() - 1), sprite);
    ee_refresh();
}

static void demo_pyramid(void)
{
    static int i = 0;

    int xo, yo, x1, y1, x2, y2, x3, y3;

    i++;

    xo = ee_get_width() * 5 / 8;
    yo = 2;

    x1 = ee_get_width() / 8 + sin(0.03*i) * 5;
    y1 = ee_get_height() / 2 + cos(0.03*i) * 5;

    x2 = ee_get_width() - 10 - cos(0.02*i) * 10;
    y2 = ee_get_height() - 5 + sin(0.02*i) * 5;

    x3 = ee_get_width() / 4 - sin(0.02*i) * 5;
    y3 = ee_get_height() + cos(0.02*i) * 5;

    ee_clear();

    ee_color(EE_GREEN);
    ee_fill_triangle(xo, yo, x2, y2, x1, y1, '%');
    ee_color(EE_YELLOW);
    ee_draw_thin_line(xo, yo, x2, y2);
    ee_draw_thin_line(x2, y2, x1, y1);
    ee_draw_thin_line(x1, y1, xo, yo);

    ee_color(EE_RED);
    ee_fill_triangle(x1, y1, x2, y2, x3, y3, '#');
    ee_color(EE_YELLOW);
    ee_draw_thin_line(x1, y1, x2, y2);
    ee_draw_thin_line(x2, y2, x3, y3);
    ee_draw_thin_line(x3, y3, x1, y1);

    ee_color(EE_BLUE);
    ee_fill_triangle(xo, yo, x2, y2, x3, y3, '%');
    ee_color(EE_YELLOW);
    ee_draw_thin_line(xo, yo, x2, y2);
    ee_draw_thin_line(x2, y2, x3, y3);
    ee_draw_thin_line(x3, y3, xo, yo);

    ee_refresh();
}

