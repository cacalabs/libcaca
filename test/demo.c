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

static void demo_dots(void);
static void demo_lines(void);
static void demo_thin_lines(void);
static void demo_circles(void);
static void demo_triangle(void);
static void demo_radar(void);

int clipping = 0;

int main(int argc, char **argv)
{
    void (*demo)(void) = NULL;
    int quit = 0;

    if(ee_init())
    {
        return 1;
    }

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
                demo = demo_triangle;
                break;
            case '6':
                ee_clear();
                demo = demo_radar;
                break;
            }
        }

        if(demo)
            demo();
    }

    /* Clean up */
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

    ee_goto((xo - strlen("libee demo")) / 2, 3);
    ee_putstr("libee demo");
    ee_goto((xo - strlen("============")) / 2, 4);
    ee_putstr("============");

    ee_goto(4, 6);
    ee_putstr("1: dots demo");
    ee_goto(4, 7);
    ee_putstr("2: lines demo");
    ee_goto(4, 8);
    ee_putstr("3: thin lines demo");
    ee_goto(4, 9);
    ee_putstr("4: circles demo");
    ee_goto(4, 10);
    ee_putstr("5: triangle demo");
    ee_goto(4, 11);
    ee_putstr("6: radar demo");

    ee_goto(4, yo - 2);
    ee_putstr("q: quit");

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
        ee_goto(ee_rand(0, xmax), ee_rand(0, ymax));
        ee_putchar('#');
    }
    ee_refresh();
}

static void demo_lines(void)
{
    int w = ee_get_width();
    int h = ee_get_height();

    /* Draw lines */
    ee_color(ee_rand(1, 10));
    if(clipping)
    {
        ee_draw_line(ee_rand(- w, 2 * w), ee_rand(- h, 2 * h),
                     ee_rand(- w, 2 * w), ee_rand(- h, 2 * h), '#');
    }
    else
    {
        ee_draw_line(ee_rand(0, w - 1), ee_rand(0, h - 1),
                     ee_rand(0, w - 1), ee_rand(0, h - 1), '*');
    }
    ee_refresh();
}

static void demo_thin_lines(void)
{
    int w = ee_get_width();
    int h = ee_get_height();

    /* Draw lines */
    ee_color(ee_rand(1, 10));
    if(clipping)
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
    if(clipping)
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
        ee_draw_circle(x, y, r, '*');
    }

    ee_refresh();
}

static void demo_triangle(void)
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

static void demo_radar(void)
{
    static int i = 0;

    int xo = ee_get_width() / 2;
    int yo = ee_get_height() / 2;
    int l = ee_get_height() + ee_get_width();

    i++;

    ee_color(EE_BLUE);
    ee_draw_line(xo,yo,xo+(sin(0.03*(i-30))*l*2),yo+(cos(0.03*(i-30))*l),'.');
    ee_color(EE_CYAN);
    ee_draw_line(xo,yo,xo+(sin(0.03*(i-2))*l*2),yo+(cos(0.03*(i-2))*l),':');
    ee_color(EE_WHITE);
    ee_draw_line(xo,yo,xo+(sin(0.03*(i-1))*l*2),yo+(cos(0.03*(i-1))*l),':');
    ee_color(EE_WHITE);
    ee_draw_line(xo,yo,xo+(sin(0.03*i)*l*2),yo+(cos(0.03*i)*l),'#');

    ee_refresh();
}

