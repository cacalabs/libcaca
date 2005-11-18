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

#ifndef M_PI
#   define M_PI 3.14159265358979323846
#endif

#include "caca.h"

/* Virtual buffer size */
#define XSIZ 256
#define YSIZ 256

#define METASIZE 100
#define METABALLS 16

/* Colour index where to crop balls */
#define CROPBALL 160

static void create_ball(void);
static void draw_ball(unsigned int, unsigned int);

static unsigned char pixels[XSIZ * YSIZ];
static unsigned char metaball[METASIZE * METASIZE];

int main(int argc, char **argv)
{
    unsigned int r[256], g[256], b[256], a[256];
    float d[METABALLS], di[METABALLS], dj[METABALLS], dk[METABALLS];
    unsigned int x[METABALLS], y[METABALLS];
    struct caca_bitmap *caca_bitmap;
    float i = 10.0, j = 17.0, k = 11.0;
    int p, frame = 0, pause = 0;

    if(caca_init())
        return 1;

    caca_set_delay(20000);

    /* Make the palette eatable by libcaca */
    for(p = 0; p < 256; p++)
        r[p] = g[p] = b[p] = a[p] = 0x0;
    r[255] = g[255] = b[255] = 0xfff;

    /* Create a libcaca bitmap smaller than our pixel buffer, so that we
     * display only the interesting part of it */
    caca_bitmap = caca_create_bitmap(8, XSIZ - METASIZE, YSIZ - METASIZE,
                                     XSIZ, 0, 0, 0, 0);

    /* Generate ball sprite */
    create_ball();

    for(p = 0; p < METABALLS; p++)
    {
        d[p] = caca_rand(0, 100);
        di[p] = (float)caca_rand(500, 4000) / 6000.0;
        dj[p] = (float)caca_rand(500, 4000) / 6000.0;
        dk[p] = (float)caca_rand(500, 4000) / 6000.0;
    }

    /* Go ! */
    for(;;)
    {
        switch(caca_get_event(CACA_EVENT_KEY_PRESS))
        {
            case CACA_EVENT_KEY_PRESS | CACA_KEY_ESCAPE: goto end;
            case CACA_EVENT_KEY_PRESS | ' ': pause = !pause;
        }

        if(pause)
            goto paused;

        frame++;

        /* Crop the palette */
        for(p = CROPBALL; p < 255; p++)
        {
            int t1, t2, t3;
            t1 = p < 0x40 ? 0 : p < 0xc0 ? (p - 0x40) * 0x20 : 0xfff;
            t2 = p < 0xe0 ? 0 : (p - 0xe0) * 0x80;
            t3 = p < 0x40 ? p * 0x40 : 0xfff;

            r[p] = (1.0 + sin((double)frame * M_PI / 60)) * t1 / 4
                 + (1.0 + sin((double)(frame + 40) * M_PI / 60)) * t2 / 4
                 + (1.0 + sin((double)(frame + 80) * M_PI / 60)) * t3 / 4;
            g[p] = (1.0 + sin((double)frame * M_PI / 60)) * t2 / 4
                 + (1.0 + sin((double)(frame + 40) * M_PI / 60)) * t3 / 4
                 + (1.0 + sin((double)(frame + 80) * M_PI / 60)) * t1 / 4;
            b[p] = (1.0 + sin((double)frame * M_PI / 60)) * t3 / 4
                 + (1.0 + sin((double)(frame + 40) * M_PI / 60)) * t1 / 4
                 + (1.0 + sin((double)(frame + 80) * M_PI / 60)) * t2 / 4;
        }

        /* Set the palette */
        caca_set_bitmap_palette(caca_bitmap, r, g, b, a);

        /* Silly paths for our balls */
        for(p = 0; p < METABALLS; p++)
        {
            float u = di[p] * i + dj[p] * j + dk[p] * sin(di[p] * k);
            float v = d[p] + di[p] * j + dj[p] * k + dk[p] * sin(dk[p] * i);
            u = sin(i + u * 2.1) * (1.0 + sin(u));
            v = sin(j + v * 1.9) * (1.0 + sin(v));
            x[p] = (XSIZ - METASIZE) / 2 + u * (XSIZ - METASIZE) / 4;
            y[p] = (YSIZ - METASIZE) / 2 + v * (YSIZ - METASIZE) / 4;
        }

        i += 0.011;
        j += 0.017;
        k += 0.019;

        memset(pixels, 0, XSIZ * YSIZ);

        /* Here is all the trick. Maybe if you're that
         * clever you'll understand. */
        for(p = 0; p < METABALLS; p++)
            draw_ball(x[p], y[p]);

paused:
        /* Draw our virtual buffer to screen, letting libcaca resize it */
        caca_draw_bitmap(0, 0, caca_get_width() - 1, caca_get_height() - 1,
                         caca_bitmap, pixels + (METASIZE / 2) * (1 + XSIZ));
        caca_refresh();
    }

    /* End, bye folks */
end:
    caca_free_bitmap(caca_bitmap);
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

