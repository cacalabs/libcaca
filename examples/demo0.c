/*
 *  demo          demo for libcaca version 0.9
 *  Copyright (c) 2003-2010 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <math.h>
#   include <string.h>
#   include <stdio.h>
#endif

#include "caca0.h"

static void display_menu(void);

static void demo_all(void);

static void demo_color(void);
static void demo_dots(void);
static void demo_lines(void);
static void demo_boxes(void);
static void demo_ellipses(void);
static void demo_triangles(void);
#if 0
static void demo_sprites(void);
#endif
static void demo_render(void);

int bounds = 0;
int outline = 0;
int dithering = 0;
struct caca_sprite *sprite = NULL;

int main(int argc, char **argv)
{
    void (*demo)(void) = NULL;
    int quit = 0;

    if(caca_init())
        return 1;

    caca_set_delay(40000);

    /* Initialize data */
#if 0
    sprite = caca_load_sprite("caca.txt");
    if(!sprite)
        sprite = caca_load_sprite("examples/caca.txt");
#endif

    /* Main menu */
    display_menu();
    caca_refresh();

    /* Go ! */
    while(!quit)
    {
        int menu = 0, mouse = 0, xmouse = 0, ymouse = 0;
        int event;

        while((event = caca_get_event(CACA_EVENT_ANY)))
        {
            if(demo && (event & CACA_EVENT_KEY_PRESS))
            {
                menu = 1;
                demo = NULL;
            }
            else if(event & CACA_EVENT_KEY_PRESS)
            {
                switch(event & 0xffff)
                {
                case 'q':
                case 'Q':
                case CACA_KEY_ESCAPE:
                    demo = NULL;
                    quit = 1;
                    break;
                case 'o':
                case 'O':
                    outline = (outline + 1) % 3;
                    display_menu();
                    caca_refresh();
                    break;
                case 'b':
                case 'B':
                    bounds = (bounds + 1) % 2;
                    display_menu();
                    caca_refresh();
                    break;
                case 'd':
                case 'D':
                    dithering = (dithering + 1) % 5;
                    caca_set_dithering(CACA_DITHERING_NONE + dithering);
                    display_menu();
                    caca_refresh();
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
#if 0
                case 's':
                case 'S':
                    if(sprite)
                        demo = demo_sprites;
                    break;
#endif
                case 'r':
                case 'R':
                    demo = demo_render;
                    break;
                }

                if(demo)
                    caca_clear();
            }
            else if(event & CACA_EVENT_MOUSE_MOTION)
            {
                mouse = 1;
                xmouse = (event & 0xfff000) >> 12;
                ymouse = event & 0xfff;
            }
        }

        if(menu || (mouse && !demo))
        {
            display_menu();
            if(mouse && !demo)
            {
                caca_set_color(CACA_COLOR_RED, CACA_COLOR_BLACK);
                caca_putstr(xmouse, ymouse, "|\\");
            }
            caca_refresh();
            mouse = menu = 0;
        }
        else if(demo)
        {
            demo();

            caca_set_color(CACA_COLOR_LIGHTGRAY, CACA_COLOR_BLACK);
            caca_draw_thin_box(1, 1, caca_get_width() - 2, caca_get_height() - 2);
            caca_printf(4, 1, "[%i.%i fps]----",
                            1000000 / caca_get_rendertime(),
                            (10000000 / caca_get_rendertime()) % 10);
            caca_refresh();
        }
    }

    /* Clean up */
#if 0
    caca_free_sprite(sprite);
#endif
    caca_end();

    return 0;
}

static void display_menu(void)
{
    int xo = caca_get_width() - 2;
    int yo = caca_get_height() - 2;

    caca_clear();
    caca_set_color(CACA_COLOR_LIGHTGRAY, CACA_COLOR_BLACK);
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
    caca_putstr(4, 13, "'c': colour");
    caca_putstr(4, 14, "'r': render");
    if(sprite)
        caca_putstr(4, 15, "'s': sprites");

    caca_putstr(4, 16, "settings:");
    caca_printf(4, 17, "'o': outline: %s",
              outline == 0 ? "none" : outline == 1 ? "solid" : "thin");
    caca_printf(4, 18, "'b': drawing boundaries: %s",
              bounds == 0 ? "screen" : "infinite");
    caca_printf(4, 19, "'d': dithering (%s)",
                caca_get_dithering_name(CACA_DITHERING_NONE + dithering));

    caca_putstr(4, yo - 2, "'q': quit");
}

static void demo_all(void)
{
    static int i = 0;

    int j, xo, yo, xa, ya, xb, yb, xc, yc;

    i++;

    caca_clear();

    /* Draw the sun */
    caca_set_color(CACA_COLOR_YELLOW, CACA_COLOR_BLACK);
    xo = caca_get_width() / 4;
    yo = caca_get_height() / 4 + 5 * sin(0.03*i);

    for(j = 0; j < 16; j++)
    {
        xa = xo - (30 + sin(0.03*i) * 8) * sin(0.03*i + M_PI*j/8);
        ya = yo + (15 + sin(0.03*i) * 4) * cos(0.03*i + M_PI*j/8);
        caca_draw_thin_line(xo, yo, xa, ya);
    }

    j = 15 + sin(0.03*i) * 8;
    caca_set_color(CACA_COLOR_WHITE, CACA_COLOR_BLACK);
    caca_fill_ellipse(xo, yo, j, j / 2, '#');
    caca_set_color(CACA_COLOR_YELLOW, CACA_COLOR_BLACK);
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

    caca_set_color(CACA_COLOR_GREEN, CACA_COLOR_BLACK);
    caca_fill_triangle(xo, yo, xb, yb, xa, ya, '%');
    caca_set_color(CACA_COLOR_YELLOW, CACA_COLOR_BLACK);
    caca_draw_thin_triangle(xo, yo, xb, yb, xa, ya);

    caca_set_color(CACA_COLOR_RED, CACA_COLOR_BLACK);
    caca_fill_triangle(xa, ya, xb, yb, xc, yc, '#');
    caca_set_color(CACA_COLOR_YELLOW, CACA_COLOR_BLACK);
    caca_draw_thin_triangle(xa, ya, xb, yb, xc, yc);

    caca_set_color(CACA_COLOR_BLUE, CACA_COLOR_BLACK);
    caca_fill_triangle(xo, yo, xb, yb, xc, yc, '%');
    caca_set_color(CACA_COLOR_YELLOW, CACA_COLOR_BLACK);
    caca_draw_thin_triangle(xo, yo, xb, yb, xc, yc);

    /* Draw a background triangle */
    xa = 2;
    ya = 2;

    xb = caca_get_width() - 3;
    yb = caca_get_height() / 2;

    xc = caca_get_width() / 3;
    yc = caca_get_height() - 3;

    caca_set_color(CACA_COLOR_CYAN, CACA_COLOR_BLACK);
    caca_draw_thin_triangle(xa, ya, xb, yb, xc, yc);

    xo = caca_get_width() / 2 + cos(0.027*i) * caca_get_width() / 3;
    yo = caca_get_height() / 2 - sin(0.027*i) * caca_get_height() / 2;

    caca_draw_thin_line(xa, ya, xo, yo);
    caca_draw_thin_line(xb, yb, xo, yo);
    caca_draw_thin_line(xc, yc, xo, yo);

    /* Draw a sprite on the pyramid */
#if 0
    caca_draw_sprite(xo, yo, sprite, 0);
#endif

    /* Draw a trail behind the foreground sprite */
    for(j = i - 60; j < i; j++)
    {
        int delta = caca_rand(-5, 5);
        caca_set_color(caca_rand(0, 15), caca_rand(0, 15));
        caca_putchar(caca_get_width() / 2
                    + cos(0.02*j) * (delta + caca_get_width() / 4),
                   caca_get_height() / 2
                    + sin(0.02*j) * (delta + caca_get_height() / 3),
                   '#');
    }

    /* Draw foreground sprite */
#if 0
    caca_draw_sprite(caca_get_width() / 2 + cos(0.02*i) * caca_get_width() / 4,
                   caca_get_height() / 2 + sin(0.02*i) * caca_get_height() / 3,
                   sprite, 0);
#endif
}

static void demo_dots(void)
{
    int xmax = caca_get_width() - 1;
    int ymax = caca_get_height() - 1;
    int i;
    static char chars[10] =
    {
        '+', '-', '*', '#', 'X', '@', '%', '$', 'M', 'W'
    };

    for(i = 1000; i--;)
    {
        /* Putpixel */
        caca_set_color(caca_rand(0, 15), caca_rand(0, 15));
        caca_putchar(caca_rand(0, xmax), caca_rand(0, ymax),
                     chars[caca_rand(0, 9)]);
    }
}

static void demo_color(void)
{
    int i, j;
    char buf[BUFSIZ];

    caca_clear();
    for(i = 0; i < 16; i++)
    {
        sprintf(buf, "'%c': %i (%s)", 'a' + i, i, caca_get_color_name(i));
        caca_set_color(CACA_COLOR_LIGHTGRAY, CACA_COLOR_BLACK);
        caca_putstr(4, i + (i >= 8 ? 4 : 3), buf);
        for(j = 0; j < 16; j++)
        {
            caca_set_color(i, j);
            caca_putstr((j >= 8 ? 41 : 40) + j * 2, i + (i >= 8 ? 4 : 3), "# ");
        }
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

    caca_set_color(caca_rand(0, 15), CACA_COLOR_BLACK);
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

    caca_set_color(caca_rand(0, 15), caca_rand(0, 15));
    caca_fill_box(xa, ya, xb, yb, '#');

    caca_set_color(caca_rand(0, 15), CACA_COLOR_BLACK);
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

    caca_set_color(caca_rand(0, 15), caca_rand(0, 15));
    caca_fill_ellipse(x, y, a, b, '#');

    caca_set_color(caca_rand(0, 15), CACA_COLOR_BLACK);
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

    caca_set_color(caca_rand(0, 15), caca_rand(0, 15));
    caca_fill_triangle(xa, ya, xb, yb, xc, yc, '#');

    caca_set_color(caca_rand(0, 15), CACA_COLOR_BLACK);
    if(outline == 2)
        caca_draw_thin_triangle(xa, ya, xb, yb, xc, yc);
    else if(outline == 1)
        caca_draw_triangle(xa, ya, xb, yb, xc, yc, '#');
}

#if 0
static void demo_sprites(void)
{
    caca_draw_sprite(caca_rand(0, caca_get_width() - 1),
                   caca_rand(0, caca_get_height() - 1), sprite, 0);
}
#endif

#if 0
static void demo_render(void)
{
    struct caca_bitmap *bitmap;
    //short buffer[256*256];
    //short *dest = buffer;
    int buffer[256*256];
    int *dest = buffer;
    int x, y, z;
    static int i = 0;

    i = (i + 1) % 512;
    z = i < 256 ? i : 511 - i;

    for(x = 0; x < 256; x++)
        for(y = 0; y < 256; y++)
    {
        //*dest++ = ((x >> 3) << 11) | ((y >> 2) << 5) | ((z >> 3));
        *dest++ = (x << 16) | (y << 8) | (z);
    }

    //bitmap = caca_create_bitmap(16, 256, 256, 2 * 256, 0xf800, 0x07e0, 0x001f, 0x0000);
    bitmap = caca_create_bitmap(32, 256, 256, 4 * 256, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    caca_draw_bitmap(0, 0, caca_get_width() - 1, caca_get_height() - 1,
                     bitmap, buffer);
    caca_free_bitmap(bitmap);
}
#endif

static void draw_circle(int *buffer, int xo, int yo, int r, int mask, int val);

static void demo_render(void)
{
    struct caca_bitmap *bitmap;
    int buffer[256*256];
    int *dest;
    int x, y, z, xo, yo;
    static int i = 0;

    i++;

    dest = buffer;
    for(x = 0; x < 256; x++)
        for(y = 0; y < 256; y++)
    {
        *dest++ = 0xff000000;
    }

    /* red */
    xo = 128 + 48 * sin(0.02 * i);
    yo = 128 + 48 * cos(0.03 * i);
    for(z = 0; z < 240; z++)
        draw_circle(buffer, xo, yo, z, 0x00ff0000, 200 << 16);

    /* green */
    xo = 128 + 48 * sin(2 + 0.06 * i);
    yo = 128 + 48 * cos(2 + 0.05 * i);
    for(z = 0; z < 240; z++)
        draw_circle(buffer, xo, yo, z, 0x0000ff00, 200 << 8);

    /* blue */
    xo = 128 + 48 * sin(1 + 0.04 * i);
    yo = 128 + 48 * cos(1 + 0.03 * i);
    for(z = 0; z < 240; z++)
        draw_circle(buffer, xo, yo, z, 0x000000ff, 200);

    bitmap = caca_create_bitmap(32, 256, 256, 4 * 256, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    caca_draw_bitmap(0, 0, caca_get_width() - 1, caca_get_height() - 1,
                     bitmap, (char *)buffer);
    caca_free_bitmap(bitmap);
}

static void draw_circle(int *buffer, int x, int y, int r, int mask, int val)
{
    int t, dx, dy;

#define POINT(X,Y) \
    buffer[(X) + 256 * (Y)] = 0xff000000 | (buffer[(X) + 256 * (Y)] & ~mask) | val;

    for(t = 0, dx = 0, dy = r; dx <= dy; dx++)
    {
        POINT(x - dx / 3, y - dy / 3);
        POINT(x + dx / 3, y - dy / 3);
        POINT(x - dx / 3, y + dy / 3);
        POINT(x + dx / 3, y + dy / 3);

        POINT(x - dy / 3, y - dx / 3);
        POINT(x + dy / 3, y - dx / 3);
        POINT(x - dy / 3, y + dx / 3);
        POINT(x + dy / 3, y + dx / 3);

        t += t > 0 ? dx - dy-- : dx;
    }
}

