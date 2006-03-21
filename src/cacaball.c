/*
 *  cacaball      metaballs effect for libcaca
 *  Copyright (c) 2003-2004 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <string.h>
#   include <math.h>
#   ifndef M_PI
#       define M_PI 3.14159265358979323846
#   endif
#endif

#include "cucul.h"
#include "caca.h"

/* Virtual buffer size */
#define XSIZ 256
#define YSIZ 256

#define METASIZE 128
#define METABALLS 12

/* Colour index where to crop balls */
#define CROPBALL 200

static void create_ball(void);
static void draw_ball(unsigned int, unsigned int);

static unsigned char pixels[XSIZ * YSIZ];
static unsigned char metaball[METASIZE * METASIZE];

int main(int argc, char **argv)
{
    cucul_t *qq; caca_t *kk;
    unsigned int r[256], g[256], b[256], a[256];
    float d[METABALLS], di[METABALLS], dj[METABALLS], dk[METABALLS];
    unsigned int x[METABALLS], y[METABALLS];
    struct cucul_bitmap *cucul_bitmap;
    float i = 10.0, j = 17.0, k = 11.0;
    int p, frame = 0, pause = 0;
    double frameOffset[360 + 80];
    double frameOffset40[360];
    double frameOffset80[360];

    qq = cucul_init();
    if(!qq)
        return 1;
    kk = caca_attach(qq);
    if(!kk)
        return 1;

    caca_set_delay(kk, 20000);

    /* Make the palette eatable by libcaca */
    for(p = 0; p < 256; p++)
        r[p] = g[p] = b[p] = a[p] = 0x0;
    r[255] = g[255] = b[255] = 0xfff;

    /* Create a libcucul bitmap smaller than our pixel buffer, so that we
     * display only the interesting part of it */
    cucul_bitmap = cucul_create_bitmap(qq, 8, XSIZ - METASIZE, YSIZ - METASIZE,
                                       XSIZ, 0, 0, 0, 0);

    /* Generate ball sprite */
    create_ball();

    for(p = 0; p < METABALLS; p++)
    {
        d[p] = cucul_rand(0, 100);
        di[p] = (float)cucul_rand(500, 4000) / 6000.0;
        dj[p] = (float)cucul_rand(500, 4000) / 6000.0;
        dk[p] = (float)cucul_rand(500, 4000) / 6000.0;
    }

    for(frame = 0; frame < 360; frame++) {
        frameOffset[frame] = frame * M_PI / 60;
        frameOffset40[frame] = (frame + 40) * M_PI / 60;
        frameOffset80[frame] = (frame + 80) * M_PI / 60;
    }

    /* Go ! */
    for(;;)
    {
        switch(caca_get_event(kk, CACA_EVENT_KEY_PRESS))
        {
            case CACA_EVENT_KEY_PRESS | CACA_KEY_ESCAPE: goto end;
            case CACA_EVENT_KEY_PRESS | ' ': pause = !pause;
        }

        if(pause)
            goto paused;

        frame++;
        if(frame >= 360)
            frame = 0;

        /* Crop the palette */
        for(p = CROPBALL; p < 255; p++)
        {
            int t1, t2, t3;
            double c1 = 1.0 + sin((double)frameOffset[frame]);
            double c2 = 1.0 + sin((double)frameOffset40[frame]);
            double c3 = 1.0 + sin((double)frameOffset80[frame]);

            t1 = p < 0x40 ? 0 : p < 0xc0 ? (p - 0x40) * 0x20 : 0xfff;
            t2 = p < 0xe0 ? 0 : (p - 0xe0) * 0x80;
            t3 = p < 0x40 ? p * 0x40 : 0xfff;

            r[p] = (c1 * t1 + c2 * t2 + c3 * t3) / 4;
            g[p] = (c1 * t2 + c2 * t3 + c3 * t1) / 4;
            b[p] = (c1 * t3 + c2 * t1 + c3 * t2) / 4;
        }

        /* Set the palette */
        cucul_set_bitmap_palette(cucul_bitmap, r, g, b, a);

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
        /* Draw our virtual buffer to screen, letting libcucul resize it */
        cucul_draw_bitmap(qq, 0, 0,
                          cucul_get_width(qq) - 1, cucul_get_height(qq) - 1,
                          cucul_bitmap, pixels + (METASIZE / 2) * (1 + XSIZ));
        caca_display(kk);
    }

    /* End, bye folks */
end:
    cucul_free_bitmap(cucul_bitmap);
    caca_detach(kk);
    cucul_end(qq);

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

