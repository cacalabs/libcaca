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
static void demo_boxes(void);
static void demo_ellipses(void);
static void demo_triangles(void);
static void demo_sprites(void);

int force_clipping = 0;
int outline = 0;
int thin = 0;
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
                thin = 0;
                break;
            case '3':
                ee_clear();
                demo = demo_lines;
                thin = 1;
                break;
            case '4':
                ee_clear();
                demo = demo_boxes;
                outline = 0;
                break;
            case '5':
                ee_clear();
                demo = demo_boxes;
                outline = 1;
                break;
            case '6':
                ee_clear();
                demo = demo_ellipses;
                break;
            case '7':
                ee_clear();
                demo = demo_triangles;
                outline = 0;
                break;
            case '8':
                ee_clear();
                demo = demo_triangles;
                outline = 1;
                break;
            case '9':
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
    ee_set_color(EE_WHITE);
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
    ee_putstr(4, 10, "4: boxes demo");
    ee_putstr(4, 11, "5: outlined boxes demo");
    ee_putstr(4, 12, "6: ellipses demo");
    ee_putstr(4, 13, "7: triangles demo");
    ee_putstr(4, 14, "8: outlined triangles demo");
    ee_putstr(4, 15, "9: sprites demo");

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
    ee_set_color(EE_YELLOW);
    xo = ee_get_width() / 4;
    yo = ee_get_height() / 4 + 5 * sin(0.03*i);

    for(j = 0; j < 16; j++)
    {
        x1 = xo - (30 + sin(0.03*i) * 8) * sin(0.03*i + M_PI*j/8);
        y1 = yo + (15 + sin(0.03*i) * 4) * cos(0.03*i + M_PI*j/8);
        ee_draw_thin_line(xo, yo, x1, y1);
    }

    j = 15 + sin(0.03*i) * 8;
    ee_set_color(EE_WHITE);
    ee_fill_ellipse(xo, yo, j, j / 2, '#');
    ee_set_color(EE_YELLOW);
    ee_draw_ellipse(xo, yo, j, j / 2, '#');

    /* Draw the pyramid */
    xo = ee_get_width() * 5 / 8;
    yo = 2;

    x1 = ee_get_width() / 8 + sin(0.03*i) * 5;
    y1 = ee_get_height() / 2 + cos(0.03*i) * 5;

    x2 = ee_get_width() - 10 - cos(0.02*i) * 10;
    y2 = ee_get_height() * 3 / 4 - 5 + sin(0.02*i) * 5;

    x3 = ee_get_width() / 4 - sin(0.02*i) * 5;
    y3 = ee_get_height() * 3 / 4 + cos(0.02*i) * 5;

    ee_set_color(EE_GREEN);
    ee_fill_triangle(xo, yo, x2, y2, x1, y1, '%');
    ee_set_color(EE_YELLOW);
    ee_draw_thin_triangle(xo, yo, x2, y2, x1, y1);

    ee_set_color(EE_RED);
    ee_fill_triangle(x1, y1, x2, y2, x3, y3, '#');
    ee_set_color(EE_YELLOW);
    ee_draw_thin_triangle(x1, y1, x2, y2, x3, y3);

    ee_set_color(EE_BLUE);
    ee_fill_triangle(xo, yo, x2, y2, x3, y3, '%');
    ee_set_color(EE_YELLOW);
    ee_draw_thin_triangle(xo, yo, x2, y2, x3, y3);

    /* Draw a background triangle */
    x1 = 2;
    y1 = 2;

    x2 = ee_get_width() - 3;
    y2 = ee_get_height() / 2;

    x3 = ee_get_width() / 3;
    y3 = ee_get_height() - 3;

    ee_set_color(EE_CYAN);
    ee_draw_thin_triangle(x1, y1, x2, y2, x3, y3);

    xo = ee_get_width() / 2 + cos(0.027*i) * ee_get_width() / 3;
    yo = ee_get_height() / 2 - sin(0.027*i) * ee_get_height() / 2;

    ee_draw_thin_line(x1, y1, xo, yo);
    ee_draw_thin_line(x2, y2, xo, yo);
    ee_draw_thin_line(x3, y3, xo, yo);

    /* Draw a sprite on the pyramid */
    ee_draw_sprite(xo, yo, sprite, 0);

    /* Draw a trail behind the foreground sprite */
    for(j = i - 60; j < i; j++)
    {
        int delta = ee_rand(-5, 5);
        ee_set_color(ee_rand(1, 10));
        ee_putchar(ee_get_width() / 2
                    + cos(0.02*j) * (delta + ee_get_width() / 4),
                   ee_get_height() / 2
                    + sin(0.02*j) * (delta + ee_get_height() / 3),
                   '#');
    }

    /* Draw foreground sprite */
    ee_draw_sprite(ee_get_width() / 2 + cos(0.02*i) * ee_get_width() / 4,
                   ee_get_height() / 2 + sin(0.02*i) * ee_get_height() / 3,
                   sprite, 0);

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
        ee_set_color(ee_rand(1, 10));
        ee_putchar(ee_rand(0, xmax), ee_rand(0, ymax), '#');
    }
    ee_refresh();
}

static void demo_lines(void)
{
    int w = ee_get_width();
    int h = ee_get_height();
    int x1, y1, x2, y2;

    if(force_clipping)
    {
        x1 = ee_rand(- w, 2 * w); y1 = ee_rand(- h, 2 * h);
        x2 = ee_rand(- w, 2 * w); y2 = ee_rand(- h, 2 * h);
    }
    else
    {
        x1 = ee_rand(0, w - 1); y1 = ee_rand(0, h - 1);
        x2 = ee_rand(0, w - 1); y2 = ee_rand(0, h - 1);
    }

    ee_set_color(ee_rand(1, 10));
    if(thin)
        ee_draw_thin_line(x1, y1, x2, y2);
    else
        ee_draw_line(x1, y1, x2, y2, '#');

    ee_refresh();
}

static void demo_boxes(void)
{
    int w = ee_get_width();
    int h = ee_get_height();
    int x1, y1, x2, y2;

    if(force_clipping)
    {
        x1 = ee_rand(- w, 2 * w); y1 = ee_rand(- h, 2 * h);
        x2 = ee_rand(- w, 2 * w); y2 = ee_rand(- h, 2 * h);
    }
    else
    {
        x1 = ee_rand(0, w - 1); y1 = ee_rand(0, h - 1);
        x2 = ee_rand(0, w - 1); y2 = ee_rand(0, h - 1);
    }

    ee_set_color(ee_rand(1, 10));
    ee_fill_box(x1, y1, x2, y2, '#');

    if(outline)
    {
        ee_set_color(ee_rand(1, 10));
        ee_draw_thin_box(x1, y1, x2, y2);
    }

    ee_refresh();
}

static void demo_ellipses(void)
{
    int w = ee_get_width();
    int h = ee_get_height();
    int x, y, a, b;

    if(force_clipping)
    {
        x = ee_rand(- w, 2 * w); y = ee_rand(- h, 2 * h);
        a = ee_rand(0, w); b = ee_rand(0, h);
    }
    else
    {
        do
        {
            x = ee_rand(0, w); y = ee_rand(0, h);
            a = ee_rand(0, w); b = ee_rand(0, h);

        } while(x - a < 0 || x + a >= w || y - b < 0 || y + b >= h);
    }

    ee_set_color(ee_rand(1, 10));
    ee_fill_ellipse(x, y, a, b, '#');

    if(outline)
    {
        ee_set_color(ee_rand(1, 10));
        ee_draw_thin_ellipse(x, y, a, b);
    }

    ee_refresh();
}

static void demo_triangles(void)
{
    int w = ee_get_width();
    int h = ee_get_height();
    int x1, y1, x2, y2, x3, y3;

    if(force_clipping)
    {
        x1 = ee_rand(- w, 2 * w); y1 = ee_rand(- h, 2 * h);
        x2 = ee_rand(- w, 2 * w); y2 = ee_rand(- h, 2 * h);
        x3 = ee_rand(- w, 2 * w); y3 = ee_rand(- h, 2 * h);
    }
    else
    {

        x1 = ee_rand(0, w - 1); y1 = ee_rand(0, h - 1);
        x2 = ee_rand(0, w - 1); y2 = ee_rand(0, h - 1);
        x3 = ee_rand(0, w - 1); y3 = ee_rand(0, h - 1);
    }

    ee_set_color(ee_rand(1, 10));
    ee_fill_triangle(x1, y1, x2, y2, x3, y3, '#');

    if(outline)
    {
        ee_set_color(ee_rand(1, 10));
        ee_draw_thin_triangle(x1, y1, x2, y2, x3, y3);
    }

    ee_refresh();
}

static void demo_sprites(void)
{
    ee_draw_sprite(ee_rand(0, ee_get_width() - 1),
                   ee_rand(0, ee_get_height() - 1), sprite, 0);
    ee_refresh();
}

