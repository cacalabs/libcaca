/*
 *   view          image viewer for libcaca
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

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>

#if defined(HAVE_IMLIB2_H)
#   include <Imlib2.h>
#else
#   include <stdio.h>
#endif

#include "caca.h"

/* Local macros */
#define STATUS_DITHERING 1
#define STATUS_ANTIALIASING 2
#define STATUS_BACKGROUND 3

/* Local functions */
static void load_image(const char *);
static void unload_image(void);
static void draw_checkers(unsigned int, unsigned int,
                          unsigned int, unsigned int);
#if !defined(HAVE_IMLIB2_H)
static int freadint(FILE *);
static int freadshort(FILE *);
static int freadchar(FILE *);
#endif

/* Local variables */
#if defined(HAVE_IMLIB2_H)
Imlib_Image image = NULL;
#endif
char *pixels = NULL;
struct caca_bitmap *bitmap = NULL;
int x, y;
unsigned int w, h, depth, bpp, rmask, gmask, bmask, amask;
unsigned int red[256], green[256], blue[256], alpha[256];

int main(int argc, char **argv)
{
    int quit = 0, update = 1, help = 0, fullscreen = 0, status = 0;
    int reload = 0, zoom = 0;

    char **list = NULL;
    int current = 0, items = 0, opts = 1;
    int i;

    /* Initialise libcaca */
    if(caca_init())
    {
        fprintf(stderr, "%s: unable to initialise libcaca\n", argv[0]);
        return 1;
    }

    /* Load items into playlist */
    for(i = 1; i < argc; i++)
    {
        /* Skip options except after `--' */
        if(opts && argv[i][0] == '-')
        {
            if(argv[i][1] == '-' && argv[i][2] == '\0')
                opts = 0;
            continue;
        }

        /* Add argv[i] to the list */
        if(items)
            list = realloc(list, (items + 1) * sizeof(char *));
        else
            list = malloc(sizeof(char *));
        list[items] = argv[i];
        items++;

        reload = 1;
    }

    /* Go ! */
    while(!quit)
    {
        int ww = caca_get_width();
        int wh = caca_get_height();

        int event, new_status = 0, new_help = 0;

        while((event = caca_get_event()))
        {
            switch(event)
            {
            case CACA_EVENT_KEY_PRESS | 'n':
            case CACA_EVENT_KEY_PRESS | 'N':
                if(items) current = (current + 1) % items;
                reload = 1;
                break;
            case CACA_EVENT_KEY_PRESS | 'p':
            case CACA_EVENT_KEY_PRESS | 'P':
                if(items) current = (items + current - 1) % items;
                reload = 1;
                break;
            case CACA_EVENT_KEY_PRESS | 'f':
            case CACA_EVENT_KEY_PRESS | 'F':
                fullscreen = ~fullscreen;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | 'b':
                i = 1 + caca_get_feature(CACA_BACKGROUND);
                if(i > CACA_BACKGROUND_MAX) i = CACA_BACKGROUND_MIN;
                caca_set_feature(i);
                new_status = STATUS_BACKGROUND;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | 'B':
                i = -1 + caca_get_feature(CACA_BACKGROUND);
                if(i < CACA_BACKGROUND_MIN) i = CACA_BACKGROUND_MAX;
                caca_set_feature(i);
                new_status = STATUS_BACKGROUND;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | 'a':
                i = 1 + caca_get_feature(CACA_ANTIALIASING);
                if(i > CACA_ANTIALIASING_MAX) i = CACA_ANTIALIASING_MIN;
                caca_set_feature(i);
                new_status = STATUS_ANTIALIASING;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | 'A':
                i = -1 + caca_get_feature(CACA_ANTIALIASING);
                if(i < CACA_ANTIALIASING_MIN) i = CACA_ANTIALIASING_MAX;
                caca_set_feature(i);
                new_status = STATUS_ANTIALIASING;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | 'd':
                i = 1 + caca_get_feature(CACA_DITHERING);
                if(i > CACA_DITHERING_MAX) i = CACA_DITHERING_MIN;
                caca_set_feature(i);
                new_status = STATUS_DITHERING;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | 'D':
                i = -1 + caca_get_feature(CACA_DITHERING);
                if(i < CACA_DITHERING_MIN) i = CACA_DITHERING_MAX;
                caca_set_feature(i);
                new_status = STATUS_DITHERING;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | '+':
                zoom++;
                if(zoom > 48) zoom = 48; else update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | '-':
                zoom--;
                if(zoom < -48) zoom = -48; else update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | 'x':
            case CACA_EVENT_KEY_PRESS | 'X':
                zoom = 0;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | 'k':
            case CACA_EVENT_KEY_PRESS | 'K':
            case CACA_EVENT_KEY_PRESS | CACA_KEY_UP:
                if(zoom > 0) y -= 1 + h / (2 + zoom) / 8;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | 'j':
            case CACA_EVENT_KEY_PRESS | 'J':
            case CACA_EVENT_KEY_PRESS | CACA_KEY_DOWN:
                if(zoom > 0) y += 1 + h / (2 + zoom) / 8;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | 'h':
            case CACA_EVENT_KEY_PRESS | 'H':
            case CACA_EVENT_KEY_PRESS | CACA_KEY_LEFT:
                if(zoom > 0) x -= 1 + w / (2 + zoom) / 8;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | 'l':
            case CACA_EVENT_KEY_PRESS | 'L':
            case CACA_EVENT_KEY_PRESS | CACA_KEY_RIGHT:
                if(zoom > 0) x += 1 + w / (2 + zoom) / 8;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | '?':
                new_help = !help;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | 'q':
            case CACA_EVENT_KEY_PRESS | 'Q':
                quit = 1;
                break;
            }

            if(status || new_status)
                status = new_status;

            if(help || new_help)
                help = new_help;
        }

        if(items && reload)
        {
            char *buffer = malloc(ww + 1);

            /* Reset image-specific runtime variables */
            zoom = 0;

            snprintf(buffer, ww, " Loading `%s'... ", list[current]);
            buffer[ww] = '\0';
            caca_set_color(CACA_COLOR_WHITE, CACA_COLOR_BLUE);
            caca_putstr((ww - strlen(buffer)) / 2, wh / 2, buffer);
            caca_refresh();

            unload_image();
            load_image(list[current]);
            reload = 0;
            update = 1;

            free(buffer);
        }

        if(!update)
        {
            usleep(10000);
            continue;
        }

        caca_clear();

        if(!items)
        {
            caca_set_color(CACA_COLOR_WHITE, CACA_COLOR_BLUE);
            caca_printf(ww / 2 - 5, wh / 2, " No image. ");
        }
        else if(!pixels)
        {
            char *buffer = malloc(ww + 1);
            snprintf(buffer, ww, " Error loading `%s'. ", list[current]);
            buffer[ww] = '\0';
            caca_set_color(CACA_COLOR_WHITE, CACA_COLOR_BLUE);
            caca_putstr((ww - strlen(buffer)) / 2, wh / 2, buffer);
            free(buffer);
        }
        else if(zoom < 0)
        {
            int xo = (ww - 1) / 2;
            int yo = (wh - 1) / 2;
            int xn = (ww - 1) / (2 - zoom);
            int yn = (wh - 1) / (2 - zoom);
            draw_checkers(xo - xn, yo - yn, xo + xn, yo + yn);
            caca_draw_bitmap(xo - xn, yo - yn, xo + xn, yo + yn,
                             bitmap, pixels);
        }
        else if(zoom > 0)
        {
            struct caca_bitmap *newbitmap;
            int xn = w / (2 + zoom);
            int yn = h / (2 + zoom);
            if(x < xn) x = xn;
            if(y < yn) y = yn;
            if(xn + x > (int)w) x = w - xn;
            if(yn + y > (int)h) y = h - yn;
            newbitmap = caca_create_bitmap(bpp, 2 * xn, 2 * yn, depth * w,
                                           rmask, gmask, bmask, amask);
            if(bpp == 8)
                caca_set_bitmap_palette(newbitmap, red, green, blue, alpha);
            draw_checkers(0, fullscreen ? 0 : 1,
                          ww - 1, fullscreen ? wh - 1 : wh - 3);
            caca_draw_bitmap(0, fullscreen ? 0 : 1,
                             ww - 1, fullscreen ? wh - 1 : wh - 3,
                             newbitmap,
                             pixels + depth * (x - xn) + depth * w * (y - yn));
            caca_free_bitmap(newbitmap);
        }
        else
        {
            draw_checkers(0, fullscreen ? 0 : 1,
                          ww - 1, fullscreen ? wh - 1 : wh - 3);
            caca_draw_bitmap(0, fullscreen ? 0 : 1,
                             ww - 1, fullscreen ? wh - 1 : wh - 3,
                             bitmap, pixels);
        }

        if(!fullscreen)
        {
            caca_set_color(CACA_COLOR_WHITE, CACA_COLOR_BLUE);
            caca_draw_line(0, 0, ww - 1, 0, ' ');
            caca_draw_line(0, wh - 2, ww - 1, wh - 2, '-');
            caca_putstr(0, 0, "q:Quit  np:Next/Prev  +-x:Zoom  "
                              "hjkl:Move  d:Dithering  a:Antialias");
            caca_putstr(ww - strlen("?:Help"), 0, "?:Help");
            caca_printf(3, wh - 2, "cacaview %s", VERSION);
            caca_printf(ww - 14, wh - 2,
                        "(zoom: %s%i)", zoom > 0 ? "+" : "", zoom);

            caca_set_color(CACA_COLOR_LIGHTRED, CACA_COLOR_BLACK);
            caca_draw_line(0, wh - 1, ww - 1, wh - 1, ' ');
            switch(status)
            {
                case STATUS_ANTIALIASING:
                    caca_printf(0, wh - 1, "Antialiasing: %s",
                  caca_get_feature_name(caca_get_feature(CACA_ANTIALIASING)));
                    break;
                case STATUS_DITHERING:
                    caca_printf(0, wh - 1, "Dithering: %s",
                  caca_get_feature_name(caca_get_feature(CACA_DITHERING)));
                    break;
                case STATUS_BACKGROUND:
                    caca_printf(0, wh - 1, "Background: %s",
                  caca_get_feature_name(caca_get_feature(CACA_BACKGROUND)));
                    break;
            }
        }

        if(help)
        {
            caca_set_color(CACA_COLOR_WHITE, CACA_COLOR_BLUE);
            caca_putstr(ww - 25, 2,  " +: zoom in             ");
            caca_putstr(ww - 25, 3,  " -: zoom out            ");
            caca_putstr(ww - 25, 4,  " x: reset zoom          ");
            caca_putstr(ww - 25, 5,  " ---------------------- ");
            caca_putstr(ww - 25, 6,  " hjkl: move view        ");
            caca_putstr(ww - 25, 7,  " arrows: move view      ");
            caca_putstr(ww - 25, 8,  " ---------------------- ");
            caca_putstr(ww - 25, 9,  " a: antialiasing method ");
            caca_putstr(ww - 25, 10, " d: dithering method    ");
            caca_putstr(ww - 25, 11, " b: background mode     ");
            caca_putstr(ww - 25, 12, " ---------------------- ");
            caca_putstr(ww - 25, 13, " ?: help                ");
            caca_putstr(ww - 25, 14, " q: quit                ");
        }

        caca_refresh();
        update = 0;
    }

    /* Clean up */
    unload_image();
    caca_end();

    return 0;
}

static void unload_image(void)
{
#ifdef HAVE_IMLIB2_H
    if(image)
        imlib_free_image();
    image = NULL;
    pixels = NULL;
#else
    if(pixels)
        free(pixels);
    pixels = NULL;
#endif
    if(bitmap)
        caca_free_bitmap(bitmap);
    bitmap = NULL;
}

static void load_image(const char *name)
{
#ifdef HAVE_IMLIB2_H
    /* Load the new image */
    image = imlib_load_image(name);

    if(!image)
        return;

    imlib_context_set_image(image);
    pixels = (char *)imlib_image_get_data_for_reading_only();
    w = imlib_image_get_width();
    h = imlib_image_get_height();
    rmask = 0x00ff0000;
    gmask = 0x0000ff00;
    bmask = 0x000000ff;
    amask = 0xff000000;
    bpp = 32;
    depth = 4;

    /* Create the libcaca bitmap */
    bitmap = caca_create_bitmap(bpp, w, h, depth * w,
                                rmask, gmask, bmask, amask);
    if(!bitmap)
    {
        imlib_free_image();
        image = NULL;
    }

#else
    /* Try to load a BMP file */
    FILE *fp;
    unsigned int i, colors, offset, tmp, planes;

    fp = fopen(name, "rb");
    if(!fp)
        return;

    if(freadshort(fp) != 0x4d42)
    {
        fclose(fp);
        return;
    }

    freadint(fp); /* size */
    freadshort(fp); /* reserved 1 */
    freadshort(fp); /* reserved 2 */

    offset = freadint(fp);

    tmp = freadint(fp); /* header size */
    if(tmp == 40)
    {
        w = freadint(fp);
        h = freadint(fp);
        planes = freadshort(fp);
        bpp = freadshort(fp);

        tmp = freadint(fp); /* compression */
        if(tmp != 0)
        {
            fclose(fp);
            return;
        }

        freadint(fp); /* sizeimage */
        freadint(fp); /* xpelspermeter */
        freadint(fp); /* ypelspermeter */
        freadint(fp); /* biclrused */
        freadint(fp); /* biclrimportantn */

        colors = (offset - 54) / 4;
        for(i = 0; i < colors && i < 256; i++)
        {
            blue[i] = freadchar(fp) * 16;
            green[i] = freadchar(fp) * 16;
            red[i] = freadchar(fp) * 16;
            alpha[i] = 0;
            freadchar(fp);
        }
    }
    else if(tmp == 12)
    {
        w = freadint(fp);
        h = freadint(fp);
        planes = freadshort(fp);
        bpp = freadshort(fp);

        colors = (offset - 26) / 3;
        for(i = 0; i < colors && i < 256; i++)
        {
            blue[i] = freadchar(fp);
            green[i] = freadchar(fp);
            red[i] = freadchar(fp);
            alpha[i] = 0;
        }
    }
    else
    {
        fclose(fp);
        return;
    }

    /* Fill the rest of the palette */
    for(i = colors; i < 256; i++)
        blue[i] = green[i] = red[i] = alpha[i] = 0;

    depth = (bpp + 7) / 8;

    /* Sanity check */
    if(!w || w > 0x10000 || !h || h > 0x10000 || planes != 1 /*|| bpp != 24*/)
    {
        fclose(fp);
        return;
    }

    /* Allocate the pixel buffer */
    pixels = malloc(w * h * depth);
    if(!pixels)
    {
        fclose(fp);
        return;
    }

    memset(pixels, 0, w * h * depth);

    /* Read the bitmap data */
    for(i = h; i--; )
    {
        unsigned int j, k, bits = 0;

        switch(bpp)
        {
            case 1:
                for(j = 0; j < w; j++)
                {
                    k = j % 32;
                    if(k == 0)
                        bits = freadint(fp);
                    pixels[w * i * depth + j] =
                        (bits >> ((k & ~0xf) + 0xf - (k & 0xf))) & 0xf;
                }
                break;
            case 4:
                for(j = 0; j < w; j++)
                {
                    k = j % 8;
                    if(k == 0)
                        bits = freadint(fp);
                    pixels[w * i * depth + j] =
                        (bits >> (4 * ((k & ~0x1) + 0x1 - (k & 0x1)))) & 0xf;
                }
                break;
            default:
                fread(pixels + w * i * depth, w * depth, 1, fp);
                /* Pad reads to 4 bytes */
                tmp = (4 - (w * depth) % 4) % 4;
                while(tmp--)
                    freadchar(fp);
                break;
        }
    }

    switch(depth)
    {
    case 3:
        rmask = 0xff0000;
        gmask = 0x00ff00;
        bmask = 0x0000ff;
        amask = 0x000000;
        break;
    case 2: /* XXX: those are the 16 bits values */
        rmask = 0x7c00;
        gmask = 0x03e0;
        bmask = 0x001f;
        amask = 0x0000;
        break;
    case 1:
    default:
        bpp = 8;
        rmask = gmask = bmask = amask = 0;
        break;
    }

    fclose(fp);

    /* Create the libcaca bitmap */
    bitmap = caca_create_bitmap(bpp, w, h, depth * w,
                                rmask, gmask, bmask, amask);
    if(!bitmap)
    {
        free(pixels);
        pixels = NULL;
        return;
    }

    if(bpp == 8)
        caca_set_bitmap_palette(bitmap, red, green, blue, alpha);
#endif

    x = w / 2;
    y = h / 2;
}

static void draw_checkers(unsigned int x1, unsigned int y1,
                          unsigned int x2, unsigned int y2)
{
    unsigned int xn, yn;

    for(yn = y1; yn <= y2; yn++)
        for(xn = x1; xn <= x2; xn++)
    {
        if((((xn - x1) / 5) ^ ((yn - y1) / 3)) & 1)
            caca_set_color(CACA_COLOR_LIGHTGRAY, CACA_COLOR_DARKGRAY);
        else
            caca_set_color(CACA_COLOR_DARKGRAY, CACA_COLOR_LIGHTGRAY);
        caca_putchar(xn, yn, ' ');
    }
}

#if !defined(HAVE_IMLIB2_H)
static int freadint(FILE *fp)
{
    unsigned char buffer[4];
    fread(buffer, 4, 1, fp);
    return ((int)buffer[3] << 24) | ((int)buffer[2] << 16)
             | ((int)buffer[1] << 8) | ((int)buffer[0]);
}

static int freadshort(FILE *fp)
{
    unsigned char buffer[2];
    fread(buffer, 2, 1, fp);
    return ((int)buffer[1] << 8) | ((int)buffer[0]);
}

static int freadchar(FILE *fp)
{
    unsigned char buffer;
    fread(&buffer, 1, 1, fp);
    return (int)buffer;
}
#endif

