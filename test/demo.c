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
#include <stdio.h>

#include <gdk/gdk.h>
#include <gdk/gdkpixbuf.h>

#include "ee.h"

static void display_menu(void);

static void demo_all(void);

static void demo_color(void);
static void demo_dots(void);
static void demo_lines(void);
static void demo_boxes(void);
static void demo_ellipses(void);
static void demo_triangles(void);
static void demo_sprites(void);
static void demo_blit(void);

int bounds = 0;
int outline = 0;
struct ee_sprite *sprite = NULL;

GdkPixbuf *pixbuf;
char *pixels;
int bufx, bufy, bufpitch;

int main(int argc, char **argv)
{
    void (*demo)(void) = NULL;
    int quit = 0;

    if(ee_init())
    {
        return 1;
    }

    ee_set_delay(40000);

    /* Initialize data */
    sprite = ee_load_sprite("data/barboss.txt");

gdk_init (&argc, &argv);
    //pixbuf = gdk_pixbuf_new_from_file("/home/sam/pix/gally4.jpeg", NULL);
    //pixbuf = gdk_pixbuf_new_from_file("/home/sam/pix/badge1.jpeg", NULL);
    //pixbuf = gdk_pixbuf_new_from_file("/home/sam/pix/union.png", NULL);
    pixbuf = gdk_pixbuf_new_from_file("/home/sam/pix/pikachu.jpeg", NULL);
if(!pixbuf) return -2;
    pixels = gdk_pixbuf_get_pixels(pixbuf);
    bufx = gdk_pixbuf_get_width(pixbuf);
    bufy = gdk_pixbuf_get_height(pixbuf);
    bufpitch = gdk_pixbuf_get_rowstride(pixbuf);
fprintf(stderr, "bits: %i\n", gdk_pixbuf_get_bits_per_sample(pixbuf));
fprintf(stderr, "w %i, h %i, stride %i\n", bufx, bufy, bufpitch);

    /* Main menu */
    display_menu();
    ee_refresh();

    /* Go ! */
    while(!quit)
    {
        char key = ee_get_key();

        if(key && demo)
        {
            display_menu();
            ee_refresh();
            demo = NULL;
        }
        else if(key)
        {
        handle_key:
            switch(key)
            {
            case 'q':
            case 'Q':
                demo = NULL;
                quit = 1;
                break;
            case 'o':
            case 'O':
                outline = (outline + 1) % 3;
                display_menu();
                break;
            case 'b':
            case 'B':
                bounds = (bounds + 1) % 2;
                display_menu();
                break;
            case 'c':
                demo = demo_color;
                break;
            case 'f':
            case 'F':
                demo = demo_all;
                break;
            case '1':
                demo = demo_dots;
                break;
            case '2':
                demo = demo_lines;
                break;
            case '3':
                demo = demo_boxes;
                break;
            case '4':
                demo = demo_triangles;
                break;
            case '5':
                demo = demo_ellipses;
                break;
            case 's':
            case 'S':
                demo = demo_sprites;
                break;
            case 'i':
            case 'I':
                demo = demo_blit;
                break;
            }

            if(demo)
                ee_clear();

            key = ee_get_key();
            if(key)
                goto handle_key;

            ee_refresh();
        }

        if(demo)
        {
            demo();

            ee_set_color(EE_WHITE);
            ee_draw_thin_box(1, 1, ee_get_width() - 2, ee_get_height() - 2);
            ee_printf(4, 1, "[%i.%i fps]----",
                            1000000 / ee_get_rendertime(),
                            (10000000 / ee_get_rendertime()) % 10);
            ee_refresh();
        }
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
    ee_draw_thin_box(1, 1, xo, yo);

    ee_putstr((xo - strlen("libee demo")) / 2, 3, "libee demo");
    ee_putstr((xo - strlen("============")) / 2, 4, "============");

    ee_putstr(4, 6, "demos:");
    ee_putstr(4, 7, "'f': full");
    ee_putstr(4, 8, "'1': dots");
    ee_putstr(4, 9, "'2': lines");
    ee_putstr(4, 10, "'3': boxes");
    ee_putstr(4, 11, "'4': triangles");
    ee_putstr(4, 12, "'5': ellipses");
    ee_putstr(4, 13, "'s': sprites");
    ee_putstr(4, 14, "'c': color");
    ee_putstr(4, 15, "'i': image blit");

    ee_putstr(4, 17, "settings:");
    ee_printf(4, 18, "'o': outline: %s",
              outline == 0 ? "none" : outline == 1 ? "solid" : "thin");
    ee_printf(4, 19, "'b': drawing boundaries: %s",
              bounds == 0 ? "screen" : "infinite");

    ee_putstr(4, yo - 2, "'q': quit");
}

static void demo_all(void)
{
    static int i = 0;

    int j, xo, yo, xa, ya, xb, yb, xc, yc;

    i++;

    ee_clear();

    /* Draw the sun */
    ee_set_color(EE_YELLOW);
    xo = ee_get_width() / 4;
    yo = ee_get_height() / 4 + 5 * sin(0.03*i);

    for(j = 0; j < 16; j++)
    {
        xa = xo - (30 + sin(0.03*i) * 8) * sin(0.03*i + M_PI*j/8);
        ya = yo + (15 + sin(0.03*i) * 4) * cos(0.03*i + M_PI*j/8);
        ee_draw_thin_line(xo, yo, xa, ya);
    }

    j = 15 + sin(0.03*i) * 8;
    ee_set_color(EE_WHITE);
    ee_fill_ellipse(xo, yo, j, j / 2, '#');
    ee_set_color(EE_YELLOW);
    ee_draw_ellipse(xo, yo, j, j / 2, '#');

    /* Draw the pyramid */
    xo = ee_get_width() * 5 / 8;
    yo = 2;

    xa = ee_get_width() / 8 + sin(0.03*i) * 5;
    ya = ee_get_height() / 2 + cos(0.03*i) * 5;

    xb = ee_get_width() - 10 - cos(0.02*i) * 10;
    yb = ee_get_height() * 3 / 4 - 5 + sin(0.02*i) * 5;

    xc = ee_get_width() / 4 - sin(0.02*i) * 5;
    yc = ee_get_height() * 3 / 4 + cos(0.02*i) * 5;

    ee_set_color(EE_GREEN);
    ee_fill_triangle(xo, yo, xb, yb, xa, ya, '%');
    ee_set_color(EE_YELLOW);
    ee_draw_thin_triangle(xo, yo, xb, yb, xa, ya);

    ee_set_color(EE_RED);
    ee_fill_triangle(xa, ya, xb, yb, xc, yc, '#');
    ee_set_color(EE_YELLOW);
    ee_draw_thin_triangle(xa, ya, xb, yb, xc, yc);

    ee_set_color(EE_BLUE);
    ee_fill_triangle(xo, yo, xb, yb, xc, yc, '%');
    ee_set_color(EE_YELLOW);
    ee_draw_thin_triangle(xo, yo, xb, yb, xc, yc);

    /* Draw a background triangle */
    xa = 2;
    ya = 2;

    xb = ee_get_width() - 3;
    yb = ee_get_height() / 2;

    xc = ee_get_width() / 3;
    yc = ee_get_height() - 3;

    ee_set_color(EE_CYAN);
    ee_draw_thin_triangle(xa, ya, xb, yb, xc, yc);

    xo = ee_get_width() / 2 + cos(0.027*i) * ee_get_width() / 3;
    yo = ee_get_height() / 2 - sin(0.027*i) * ee_get_height() / 2;

    ee_draw_thin_line(xa, ya, xo, yo);
    ee_draw_thin_line(xb, yb, xo, yo);
    ee_draw_thin_line(xc, yc, xo, yo);

    /* Draw a sprite on the pyramid */
    ee_draw_sprite(xo, yo, sprite, 0);

    /* Draw a trail behind the foreground sprite */
    for(j = i - 60; j < i; j++)
    {
        int delta = ee_rand(-5, 5);
        ee_set_color(ee_rand(0, 15));
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
}

static void demo_dots(void)
{
    int xmax = ee_get_width() - 1;
    int ymax = ee_get_height() - 1;
    int i;

    for(i = 1000; i--;)
    {
        /* Putpixel */
        ee_set_color(ee_rand(0, 15));
        ee_putchar(ee_rand(0, xmax), ee_rand(0, ymax), '#');
    }
}

static void demo_color(void)
{
    int i;
    char buf[BUFSIZ];

    ee_clear();
    for(i = 0; i < 16; i++)
    {
        sprintf(buf, "'%c': %i (%s)", 'a' + i, i, ee_get_color_name(i));
        ee_set_color(EE_WHITE);
        ee_putstr(4, i + 3, buf);
        ee_set_color(i);
        ee_putstr(40, i + 3, "XXXXXXXXXX-XX--X----------");
    }
}

static void demo_lines(void)
{
    int w = ee_get_width();
    int h = ee_get_height();
    int xa, ya, xb, yb;

    if(bounds)
    {
        xa = ee_rand(- w, 2 * w); ya = ee_rand(- h, 2 * h);
        xb = ee_rand(- w, 2 * w); yb = ee_rand(- h, 2 * h);
    }
    else
    {
        xa = ee_rand(0, w - 1); ya = ee_rand(0, h - 1);
        xb = ee_rand(0, w - 1); yb = ee_rand(0, h - 1);
    }

    ee_set_color(ee_rand(0, 15));
    if(outline > 1)
        ee_draw_thin_line(xa, ya, xb, yb);
    else
        ee_draw_line(xa, ya, xb, yb, '#');
}

static void demo_boxes(void)
{
    int w = ee_get_width();
    int h = ee_get_height();
    int xa, ya, xb, yb;

    if(bounds)
    {
        xa = ee_rand(- w, 2 * w); ya = ee_rand(- h, 2 * h);
        xb = ee_rand(- w, 2 * w); yb = ee_rand(- h, 2 * h);
    }
    else
    {
        xa = ee_rand(0, w - 1); ya = ee_rand(0, h - 1);
        xb = ee_rand(0, w - 1); yb = ee_rand(0, h - 1);
    }

    ee_set_color(ee_rand(0, 15));
    ee_fill_box(xa, ya, xb, yb, '#');

    ee_set_color(ee_rand(0, 15));
    if(outline == 2)
        ee_draw_thin_box(xa, ya, xb, yb);
    else if(outline == 1)
        ee_draw_box(xa, ya, xb, yb, '#');
}

static void demo_ellipses(void)
{
    int w = ee_get_width();
    int h = ee_get_height();
    int x, y, a, b;

    if(bounds)
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

    ee_set_color(ee_rand(0, 15));
    ee_fill_ellipse(x, y, a, b, '#');

    ee_set_color(ee_rand(0, 15));
    if(outline == 2)
        ee_draw_thin_ellipse(x, y, a, b);
    else if(outline == 1)
        ee_draw_ellipse(x, y, a, b, '#');
}

static void demo_triangles(void)
{
    int w = ee_get_width();
    int h = ee_get_height();
    int xa, ya, xb, yb, xc, yc;

    if(bounds)
    {
        xa = ee_rand(- w, 2 * w); ya = ee_rand(- h, 2 * h);
        xb = ee_rand(- w, 2 * w); yb = ee_rand(- h, 2 * h);
        xc = ee_rand(- w, 2 * w); yc = ee_rand(- h, 2 * h);
    }
    else
    {

        xa = ee_rand(0, w - 1); ya = ee_rand(0, h - 1);
        xb = ee_rand(0, w - 1); yb = ee_rand(0, h - 1);
        xc = ee_rand(0, w - 1); yc = ee_rand(0, h - 1);
    }

    ee_set_color(ee_rand(0, 15));
    ee_fill_triangle(xa, ya, xb, yb, xc, yc, '#');

    ee_set_color(ee_rand(0, 15));
    if(outline == 2)
        ee_draw_thin_triangle(xa, ya, xb, yb, xc, yc);
    else if(outline == 1)
        ee_draw_triangle(xa, ya, xb, yb, xc, yc, '#');
}

static void demo_sprites(void)
{
    ee_draw_sprite(ee_rand(0, ee_get_width() - 1),
                   ee_rand(0, ee_get_height() - 1), sprite, 0);
}

static void demo_blit(void)
{
ee_set_color(EE_LIGHTGRAY);
    ee_blit(6, 4, ee_get_width() - 6, ee_get_height() - 4, pixels, bufx, bufy);
}

