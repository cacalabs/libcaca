/*
 *  cacaball      metaballs effect for libcaca
 *  Copyright (c) 2003-2004 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  $Id$
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "caca.h"

/* Virtual buffer size */
#define XSIZ 256
#define YSIZ 256

#define METASIZE 128

static void create_ball(void);
static void draw_ball(unsigned int, unsigned int);

static unsigned char pixels[XSIZ * YSIZ];
static unsigned char metaball[METASIZE * METASIZE];

int main(int argc, char **argv)
{
    struct caca_bitmap *caca_bitmap;
    float i = 0, j = 0, k = 0;
    int p;
    unsigned int x[5], y[5];
    int r[256], g[256], b[256], a[256];

    if(caca_init())
        return 1;

    caca_set_delay(0);

    /* Make the palette eatable by libcaca */
    for(p = 0; p < 256; p++)
    {
        r[p] = p < 0xc0 ? 0 : (p - 0xc0) * 0x40;
        g[p] = p < 0x40 ? 0 : p < 0xc0 ? (p - 0x40) * 0x20 : 0xfff;
        b[p] = p < 0x40 ? p * 0x40 : 0xfff;
        a[p] = 0x0;
    }

    /* Crop the palette */
    for(p = 0; p < 150; p++)
        r[p] = g[p] = b[p] = a[p] = 0x0;

    /* Create the bitmap */
    caca_bitmap = caca_create_bitmap(8, XSIZ, YSIZ, XSIZ, 0, 0, 0, 0);

    /* Set the palette */
    caca_set_bitmap_palette(caca_bitmap, r, g, b, a);

    /* Generate ball sprite */
    create_ball();

    /* Go ! */
    while(!caca_get_event(CACA_EVENT_KEY_PRESS))
    {
        /* Silly paths for our balls */
        x[0] = (1 + sin(i + k)) * (XSIZ-METASIZE) / 2;
        y[0] = (1 + cos(1 + j)) * (YSIZ-METASIZE) / 2;
        x[1] = (1 + cos(2 + j * 2 + k)) * (XSIZ-METASIZE) / 2;
        y[1] = (1 + cos(3 + i / 2 + j)) * (YSIZ-METASIZE) / 2;
        x[2] = (1 + cos(4 + k * 2)) * (XSIZ-METASIZE) / 2;
        y[2] = (1 + cos(i + j / 2)) * (YSIZ-METASIZE) / 2;
        x[3] = (1 + sin(6 + j * 2)) * (XSIZ-METASIZE) / 2;
        y[3] = (1 + cos(7 + i / 2)) * (YSIZ-METASIZE) / 2;
        x[4] = (1 + cos(i - k / 2)) * (XSIZ-METASIZE) / 2;
        y[4] = (1 + sin(i + k / 2)) * (YSIZ-METASIZE) / 2;

        i += 0.011;
        j += 0.021;
        k += 0.029;

        memset(pixels, 0, XSIZ * YSIZ);

        /* Here is all the trick. Maybe if you're that
         * clever you'll understand. */
        for(p = 0; p < 5; p++)
            draw_ball(x[p], y[p]);

        /* Draw our virtual buffer to screen, letting libcaca resize it */
        caca_draw_bitmap(-10, -10, caca_get_width() + 9, caca_get_height() + 9,
                         caca_bitmap, pixels);
        caca_refresh();
    }

    /* End, bye folks */
    caca_end();

    return 0;
}

/* Generate ball sprite
 * You should read the comments, I already wrote that before ... */
static void create_ball(void)
{
    int x, y;
    float distance;

    for(y = 0; y < METASIZE; y++)
        for(x = 0; x < METASIZE; x++)
    {
	distance = ((METASIZE/2) - x) * ((METASIZE/2) - x)
                 + ((METASIZE/2) - y) * ((METASIZE/2) - y);
        distance = sqrt(distance) * 64 / METASIZE;
	metaball[x + y * METASIZE] = distance > 15 ? 0 : (255 - distance) * 15;
    }
}

/* You missed the trick ? */
static void draw_ball(unsigned int bx, unsigned int by)
{
    unsigned int color;
    unsigned int i, e = 0;
    unsigned int b = (by * XSIZ) + bx;

    for(i = 0; i < METASIZE * METASIZE; i++)
    {
        color = pixels[b] + metaball[i];

        if(color > 255)
            color = 255;

        pixels[b] = color;
        if(e == METASIZE)
        {
            e = 0;
            b += XSIZ - METASIZE;
        }
        b++;
        e++;
    }
}

