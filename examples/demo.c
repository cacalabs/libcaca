/*
 *   demo          demo using libcaca
 *   Copyright (c) 2003 Sam Hocevar <sam@zoy.org>
 *                 All Rights Reserved
 *
 *   $Id$
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *   02111-1307  USA
 */

#include "config.h"

#include <math.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdk.h>
#include <gdk/gdkpixbuf.h>

#include "caca.h"

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
int dithering = 0;
struct caca_sprite *sprite = NULL;

GdkPixbuf *pixbuf;
char *pixels;
int bufx, bufy, bufpitch;

int main(int argc, char **argv)
{
    void (*demo)(void) = NULL;
    int quit = 0;

    if(caca_init())
    {
        return 1;
    }

    caca_set_delay(40000);

    /* Initialize data */
    sprite = caca_load_sprite(DATADIR "/caca.txt");
    if(!sprite)
        sprite = caca_load_sprite("caca.txt");
    if(!sprite)
        sprite = caca_load_sprite("examples/caca.txt");

gdk_init (&argc, &argv);
    //pixbuf = gdk_pixbuf_new_from_file("/home/sam/pix/gally4.jpeg", NULL);
    //pixbuf = gdk_pixbuf_new_from_file("/home/sam/pix/badge1.jpeg", NULL);
    //pixbuf = gdk_pixbuf_new_from_file("/home/sam/pix/union.png", NULL);
    //pixbuf = gdk_pixbuf_new_from_file("/home/sam/pix/pikachu.jpeg", NULL);
    //pixbuf = gdk_pixbuf_new_from_file("/home/sam/pix/gradient.png", NULL);
    //pixbuf = gdk_pixbuf_new_from_file("/home/sam/pix/beastie.png", NULL);
    //pixbuf = gdk_pixbuf_new_from_file("/home/sam/pix/stitch.jpg", NULL);
    //pixbuf = gdk_pixbuf_new_from_file("/home/sam/pix/caca.jpg", NULL);
    //pixbuf = gdk_pixbuf_new_from_file("/home/sam/pix/dranac.jpeg", NULL);
    //pixbuf = gdk_pixbuf_new_from_file("/home/sam/artwork/aboire.png", NULL);
    //pixbuf = gdk_pixbuf_new_from_file("/home/sam/web/sam.zoy.org/artwork/goret.png", NULL);
    //pixbuf = gdk_pixbuf_new_from_file("/home/sam/lilkim02.jpg", NULL);
    //pixbuf = gdk_pixbuf_new_from_file("/home/sam/etw.bmp", NULL);
    pixbuf = gdk_pixbuf_new_from_file("/home/sam/pix/lena_std.png", NULL);
if(!pixbuf) return -2;
    pixels = gdk_pixbuf_get_pixels(pixbuf);
    bufx = gdk_pixbuf_get_width(pixbuf);
    bufy = gdk_pixbuf_get_height(pixbuf);
    bufpitch = gdk_pixbuf_get_rowstride(pixbuf);
fprintf(stderr, "bits: %i\n", gdk_pixbuf_get_bits_per_sample(pixbuf));
fprintf(stderr, "w %i, h %i, stride %i\n", bufx, bufy, bufpitch);

    /* Main menu */
    display_menu();
    caca_refresh();

    /* Go ! */
    while(!quit)
    {
        int event = caca_get_event();

        if(event && demo)
        {
            display_menu();
            caca_refresh();
            demo = NULL;
        }
        else if(event & CACA_EVENT_KEY_PRESS)
        {
        handle_key:
            switch(event & 0xff)
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
            case 'd':
            case 'D':
                dithering = (dithering + 1) % 3;
                caca_set_dithering(dithering == 0 ? CACA_DITHER_NONE :
                                   dithering == 1 ? CACA_DITHER_ORDERED :
                                                    CACA_DITHER_RANDOM);
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
                caca_clear();

        handle_event:
            event = caca_get_event();
            if(event & CACA_EVENT_KEY_PRESS)
                goto handle_key;
            else if(event)
                goto handle_event;

            caca_refresh();
        }

        if(demo)
        {
            demo();

            caca_set_color(CACA_COLOR_WHITE);
            caca_draw_thin_box(1, 1, caca_get_width() - 2, caca_get_height() - 2);
            caca_printf(4, 1, "[%i.%i fps]----",
                            1000000 / caca_get_rendertime(),
                            (10000000 / caca_get_rendertime()) % 10);
            caca_refresh();
        }
    }

    /* Clean up */
    caca_free_sprite(sprite);
    caca_end();

    return 0;
}

static void display_menu(void)
{
    int xo = caca_get_width() - 2;
    int yo = caca_get_height() - 2;

    caca_clear();
    caca_set_color(CACA_COLOR_WHITE);
    caca_draw_thin_box(1, 1, xo, yo);

    caca_putstr((xo - strlen("libcaca demo")) / 2, 3, "libcaca demo");
    caca_putstr((xo - strlen("==============")) / 2, 4, "==============");

    caca_putstr(4, 6, "demos:");
    caca_putstr(4, 7, "'f': full");
    caca_putstr(4, 8, "'1': dots");
    caca_putstr(4, 9, "'2': lines");
    caca_putstr(4, 10, "'3': boxes");
    caca_putstr(4, 11, "'4': triangles");
    caca_putstr(4, 12, "'5': ellipses");
    caca_putstr(4, 13, "'s': sprites");
    caca_putstr(4, 14, "'c': color");
    caca_putstr(4, 15, "'i': image blit");

    caca_putstr(4, 17, "settings:");
    caca_printf(4, 18, "'o': outline: %s",
              outline == 0 ? "none" : outline == 1 ? "solid" : "thin");
    caca_printf(4, 19, "'b': drawing boundaries: %s",
              bounds == 0 ? "screen" : "infinite");
    caca_printf(4, 20, "'d': %s dithering",
              dithering == 0 ? "no" : dithering == 1 ? "ordered" : "random");

    caca_putstr(4, yo - 2, "'q': quit");
}

static void demo_all(void)
{
    static int i = 0;

    int j, xo, yo, xa, ya, xb, yb, xc, yc;

    i++;

    caca_clear();

    /* Draw the sun */
    caca_set_color(CACA_COLOR_YELLOW);
    xo = caca_get_width() / 4;
    yo = caca_get_height() / 4 + 5 * sin(0.03*i);

    for(j = 0; j < 16; j++)
    {
        xa = xo - (30 + sin(0.03*i) * 8) * sin(0.03*i + M_PI*j/8);
        ya = yo + (15 + sin(0.03*i) * 4) * cos(0.03*i + M_PI*j/8);
        caca_draw_thin_line(xo, yo, xa, ya);
    }

    j = 15 + sin(0.03*i) * 8;
    caca_set_color(CACA_COLOR_WHITE);
    caca_fill_ellipse(xo, yo, j, j / 2, '#');
    caca_set_color(CACA_COLOR_YELLOW);
    caca_draw_ellipse(xo, yo, j, j / 2, '#');

    /* Draw the pyramid */
    xo = caca_get_width() * 5 / 8;
    yo = 2;

    xa = caca_get_width() / 8 + sin(0.03*i) * 5;
    ya = caca_get_height() / 2 + cos(0.03*i) * 5;

    xb = caca_get_width() - 10 - cos(0.02*i) * 10;
    yb = caca_get_height() * 3 / 4 - 5 + sin(0.02*i) * 5;

    xc = caca_get_width() / 4 - sin(0.02*i) * 5;
    yc = caca_get_height() * 3 / 4 + cos(0.02*i) * 5;

    caca_set_color(CACA_COLOR_GREEN);
    caca_fill_triangle(xo, yo, xb, yb, xa, ya, '%');
    caca_set_color(CACA_COLOR_YELLOW);
    caca_draw_thin_triangle(xo, yo, xb, yb, xa, ya);

    caca_set_color(CACA_COLOR_RED);
    caca_fill_triangle(xa, ya, xb, yb, xc, yc, '#');
    caca_set_color(CACA_COLOR_YELLOW);
    caca_draw_thin_triangle(xa, ya, xb, yb, xc, yc);

    caca_set_color(CACA_COLOR_BLUE);
    caca_fill_triangle(xo, yo, xb, yb, xc, yc, '%');
    caca_set_color(CACA_COLOR_YELLOW);
    caca_draw_thin_triangle(xo, yo, xb, yb, xc, yc);

    /* Draw a background triangle */
    xa = 2;
    ya = 2;

    xb = caca_get_width() - 3;
    yb = caca_get_height() / 2;

    xc = caca_get_width() / 3;
    yc = caca_get_height() - 3;

    caca_set_color(CACA_COLOR_CYAN);
    caca_draw_thin_triangle(xa, ya, xb, yb, xc, yc);

    xo = caca_get_width() / 2 + cos(0.027*i) * caca_get_width() / 3;
    yo = caca_get_height() / 2 - sin(0.027*i) * caca_get_height() / 2;

    caca_draw_thin_line(xa, ya, xo, yo);
    caca_draw_thin_line(xb, yb, xo, yo);
    caca_draw_thin_line(xc, yc, xo, yo);

    /* Draw a sprite on the pyramid */
    caca_draw_sprite(xo, yo, sprite, 0);

    /* Draw a trail behind the foreground sprite */
    for(j = i - 60; j < i; j++)
    {
        int delta = caca_rand(-5, 5);
        caca_set_color(caca_rand(0, 15));
        caca_putchar(caca_get_width() / 2
                    + cos(0.02*j) * (delta + caca_get_width() / 4),
                   caca_get_height() / 2
                    + sin(0.02*j) * (delta + caca_get_height() / 3),
                   '#');
    }

    /* Draw foreground sprite */
    caca_draw_sprite(caca_get_width() / 2 + cos(0.02*i) * caca_get_width() / 4,
                   caca_get_height() / 2 + sin(0.02*i) * caca_get_height() / 3,
                   sprite, 0);
}

static void demo_dots(void)
{
    int xmax = caca_get_width() - 1;
    int ymax = caca_get_height() - 1;
    int i;

    for(i = 1000; i--;)
    {
        /* Putpixel */
        caca_set_color(caca_rand(0, 15));
        caca_putchar(caca_rand(0, xmax), caca_rand(0, ymax), '#');
    }
}

static void demo_color(void)
{
    int i;
    char buf[BUFSIZ];

    caca_clear();
    for(i = 0; i < 16; i++)
    {
        sprintf(buf, "'%c': %i (%s)", 'a' + i, i, caca_get_color_name(i));
        caca_set_color(CACA_COLOR_WHITE);
        caca_putstr(4, i + 3, buf);
        caca_set_color(i);
        caca_putstr(40, i + 3, "XXXXXXXXXX-XX--X----------");
    }
}

static void demo_lines(void)
{
    int w = caca_get_width();
    int h = caca_get_height();
    int xa, ya, xb, yb;

    if(bounds)
    {
        xa = caca_rand(- w, 2 * w); ya = caca_rand(- h, 2 * h);
        xb = caca_rand(- w, 2 * w); yb = caca_rand(- h, 2 * h);
    }
    else
    {
        xa = caca_rand(0, w - 1); ya = caca_rand(0, h - 1);
        xb = caca_rand(0, w - 1); yb = caca_rand(0, h - 1);
    }

    caca_set_color(caca_rand(0, 15));
    if(outline > 1)
        caca_draw_thin_line(xa, ya, xb, yb);
    else
        caca_draw_line(xa, ya, xb, yb, '#');
}

static void demo_boxes(void)
{
    int w = caca_get_width();
    int h = caca_get_height();
    int xa, ya, xb, yb;

    if(bounds)
    {
        xa = caca_rand(- w, 2 * w); ya = caca_rand(- h, 2 * h);
        xb = caca_rand(- w, 2 * w); yb = caca_rand(- h, 2 * h);
    }
    else
    {
        xa = caca_rand(0, w - 1); ya = caca_rand(0, h - 1);
        xb = caca_rand(0, w - 1); yb = caca_rand(0, h - 1);
    }

    caca_set_color(caca_rand(0, 15));
    caca_fill_box(xa, ya, xb, yb, '#');

    caca_set_color(caca_rand(0, 15));
    if(outline == 2)
        caca_draw_thin_box(xa, ya, xb, yb);
    else if(outline == 1)
        caca_draw_box(xa, ya, xb, yb, '#');
}

static void demo_ellipses(void)
{
    int w = caca_get_width();
    int h = caca_get_height();
    int x, y, a, b;

    if(bounds)
    {
        x = caca_rand(- w, 2 * w); y = caca_rand(- h, 2 * h);
        a = caca_rand(0, w); b = caca_rand(0, h);
    }
    else
    {
        do
        {
            x = caca_rand(0, w); y = caca_rand(0, h);
            a = caca_rand(0, w); b = caca_rand(0, h);

        } while(x - a < 0 || x + a >= w || y - b < 0 || y + b >= h);
    }

    caca_set_color(caca_rand(0, 15));
    caca_fill_ellipse(x, y, a, b, '#');

    caca_set_color(caca_rand(0, 15));
    if(outline == 2)
        caca_draw_thin_ellipse(x, y, a, b);
    else if(outline == 1)
        caca_draw_ellipse(x, y, a, b, '#');
}

static void demo_triangles(void)
{
    int w = caca_get_width();
    int h = caca_get_height();
    int xa, ya, xb, yb, xc, yc;

    if(bounds)
    {
        xa = caca_rand(- w, 2 * w); ya = caca_rand(- h, 2 * h);
        xb = caca_rand(- w, 2 * w); yb = caca_rand(- h, 2 * h);
        xc = caca_rand(- w, 2 * w); yc = caca_rand(- h, 2 * h);
    }
    else
    {

        xa = caca_rand(0, w - 1); ya = caca_rand(0, h - 1);
        xb = caca_rand(0, w - 1); yb = caca_rand(0, h - 1);
        xc = caca_rand(0, w - 1); yc = caca_rand(0, h - 1);
    }

    caca_set_color(caca_rand(0, 15));
    caca_fill_triangle(xa, ya, xb, yb, xc, yc, '#');

    caca_set_color(caca_rand(0, 15));
    if(outline == 2)
        caca_draw_thin_triangle(xa, ya, xb, yb, xc, yc);
    else if(outline == 1)
        caca_draw_triangle(xa, ya, xb, yb, xc, yc, '#');
}

static void demo_sprites(void)
{
    caca_draw_sprite(caca_rand(0, caca_get_width() - 1),
                   caca_rand(0, caca_get_height() - 1), sprite, 0);
}

static void demo_blit(void)
{
    caca_blit(6, 4, caca_get_width() - 6, caca_get_height() - 4,
              pixels, bufx, bufy);
}

