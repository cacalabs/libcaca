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

#define X_DISPLAY_MISSING 1
#include <Imlib2.h>

#include "caca.h"

Imlib_Image image = NULL;
char *pixels = NULL;
struct caca_bitmap *bitmap = NULL;

int dithering = 1;
const enum caca_dithering dithering_list[] =
    { CACA_DITHER_NONE, CACA_DITHER_ORDERED, CACA_DITHER_RANDOM };

int main(int argc, char **argv)
{
    int quit = 0, update = 1;
    int x, y, w, h, zoom = 0;

    if(argc != 2)
    {
        fprintf(stderr, "usage: %s <filename>\n", argv[0]);
        return 1;
    }

    image = imlib_load_image(argv[1]);

    if(!image)
    {
        fprintf(stderr, "%s: unable to open `%s'\n", argv[0], argv[1]);
        return 1;
    }

    imlib_context_set_image(image);
    pixels = (char *)imlib_image_get_data_for_reading_only();
    w = imlib_image_get_width();
    h = imlib_image_get_height();
    x = w / 2;
    y = h / 2;

    if(caca_init())
    {
        fprintf(stderr, "%s: unable to initialise libcaca\n", argv[0]);
        return 1;
    }

    /* Create the libcaca bitmap */
    bitmap = caca_create_bitmap(32, w, h, 4 * w,
                                0x00ff0000, 0x0000ff00, 0x000000ff);

    if(!image)
    {
        fprintf(stderr, "%s: unable to create libcaca bitmap\n", argv[0]);
        caca_end();
        return 1;
    }

    /* Go ! */
    while(!quit)
    {
        int event;

        while((event = caca_get_event()))
        {
            switch(event)
            {
            case CACA_EVENT_KEY_PRESS | 'd':
            case CACA_EVENT_KEY_PRESS | 'D':
                dithering = (dithering + 1) % 3;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | '+':
                zoom++;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | '-':
                zoom--;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | 'x':
            case CACA_EVENT_KEY_PRESS | 'X':
                zoom = 0;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | CACA_KEY_UP:
                if(zoom > 0) y -= 1 + h / (2 + zoom) / 8;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | CACA_KEY_DOWN:
                if(zoom > 0) y += 1 + h / (2 + zoom) / 8;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | CACA_KEY_LEFT:
                if(zoom > 0) x -= 1 + w / (2 + zoom) / 8;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | CACA_KEY_RIGHT:
                if(zoom > 0) x += 1 + w / (2 + zoom) / 8;
                update = 1;
                break;
            case CACA_EVENT_KEY_PRESS | 'q':
            case CACA_EVENT_KEY_PRESS | 'Q':
                quit = 1;
                break;
            }
        }

        if(update)
        {
            caca_clear();
            caca_set_dithering(dithering_list[dithering]);
            if(zoom < 0)
            {
                int xo = (caca_get_width() - 1) / 2;
                int yo = (caca_get_height() - 1) / 2;
                int xn = (caca_get_width() - 1) / (2 - zoom);
                int yn = (caca_get_height() - 1) / (2 - zoom);
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
                if(xn + x > w) x = w - xn;
                if(yn + y > h) y = h - yn;
                newbitmap = caca_create_bitmap(32, 2 * xn, 2 * yn, 4 * w,
                                            0x00ff0000, 0x0000ff00, 0x000000ff);
                caca_draw_bitmap(0, 0, caca_get_width() - 1,
                                 caca_get_height() - 1, newbitmap,
                                 pixels + 4 * (x - xn) + 4 * w * (y - yn));
                caca_free_bitmap(newbitmap);
            }
            else
            {
                caca_draw_bitmap(0, 0, caca_get_width() - 1,
                                 caca_get_height() - 1, bitmap, pixels);
            }
            caca_refresh();
            update = 0;
        }
    }

    caca_free_bitmap(bitmap);
    imlib_free_image();

    /* Clean up */
    caca_end();

    return 0;
}

