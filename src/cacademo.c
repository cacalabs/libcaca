/*
 *  cacademo      various demo effects for libcaca
 *  Copyright (c) 1998 Michele Bini <mibin@tin.it>
 *                2004 Sam Hocevar <sam@zoy.org>
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
#include "common.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <string.h>
#   include <math.h>
#   ifndef M_PI
#       define M_PI 3.14159265358979323846
#   endif
#endif

#include "cucul.h"
#include "caca.h"

/* Virtual buffer size for dither-based demos */
#define XSIZ 256
#define YSIZ 256
static unsigned char screen[XSIZ * YSIZ];

/* The plasma effect */
#define TABLEX (XSIZ * 2)
#define TABLEY (YSIZ * 2)

static unsigned char table[TABLEX * TABLEY];

static int main_plasma(int, char **);
static void do_plasma(unsigned char *,
                      double, double, double, double, double, double);

/* The metaball effect */
#define METASIZE 128
#define METABALLS 12
#define CROPBALL 200 /* Colour index where to crop balls */

static int main_ball(int, char **);
static void create_ball(void);
static void draw_ball(unsigned int, unsigned int);

static unsigned char metaball[METASIZE * METASIZE];

/* The moiré effect */
#define DISCSIZ 512
#define DISCTHICKNESS 96

static unsigned char disc[DISCSIZ * DISCSIZ];

static int main_moir(int, char **);
static void put_disc(int, int);
static void draw_disc(int, char);
static void draw_line(int, int, char);

int main(int argc, char **argv)
{
    switch(cucul_rand(0, 3))
    {
        case 0:
            return main_plasma(argc, argv);
        case 1:
            return main_ball(argc, argv);
        case 2:
            return main_moir(argc, argv);
    }

    return -1;
}

static int main_plasma(int argc, char **argv)
{
    cucul_canvas_t *cv, *c2, *mask; caca_display_t *dp;
    unsigned int red[256], green[256], blue[256], alpha[256];
    double r[3], R[6];
    cucul_dither_t *dither;
    int i, x, y, frame = 0, pause = 0;

    cv = cucul_create_canvas(0, 0);
    if(!cv)
        return 1;
    dp = caca_create_display(cv);
    if(!dp)
        return 1;

    caca_set_display_time(dp, 20000);

    c2 = cucul_create_canvas(cucul_get_canvas_width(cv),
                             cucul_get_canvas_height(cv));
    mask = cucul_create_canvas(cucul_get_canvas_width(cv),
                               cucul_get_canvas_height(cv));

    /* Fill various tables */
    for(i = 0 ; i < 256; i++)
        red[i] = green[i] = blue[i] = alpha[i] = 0;

    for(i = 0; i < 3; i++)
        r[i] = (double)(cucul_rand(1, 1000)) / 60000 * M_PI;

    for(i = 0; i < 6; i++)
        R[i] = (double)(cucul_rand(1, 1000)) / 10000;

    for(y = 0 ; y < TABLEY ; y++)
        for(x = 0 ; x < TABLEX ; x++)
    {
        double tmp = (((double)((x - (TABLEX / 2)) * (x - (TABLEX / 2))
                              + (y - (TABLEX / 2)) * (y - (TABLEX / 2))))
                      * (M_PI / (TABLEX * TABLEX + TABLEY * TABLEY)));

        table[x + y * TABLEX] = (1.0 + sin(12.0 * sqrt(tmp))) * 256 / 6;
    }

    /* Create a libcucul dither */
    dither = cucul_create_dither(8, XSIZ, YSIZ, XSIZ, 0, 0, 0, 0);

    /* Main loop */
    for(;;)
    {
        caca_event_t ev;
        if(caca_get_event(dp, CACA_EVENT_KEY_PRESS, &ev, 0))
        {
            switch(ev.data.key.ch)
            {
                case CACA_KEY_ESCAPE: goto end;
                case ' ': pause = !pause;
            }
        }

        if(pause)
            goto paused;

        for(i = 0 ; i < 256; i++)
        {
            double z = ((double)i) / 256 * 6 * M_PI;

            red[i] = (1.0 + sin(z + r[1] * frame)) / 2 * 0xfff;
            blue[i] = (1.0 + cos(z + r[0] * frame)) / 2 * 0xfff;
            green[i] = (1.0 + cos(z + r[2] * frame)) / 2 * 0xfff;
        }

        /* Set the palette */
        cucul_set_dither_palette(dither, red, green, blue, alpha);

        do_plasma(screen,
                  (1.0 + sin(((double)frame) * R[0])) / 2,
                  (1.0 + sin(((double)frame) * R[1])) / 2,
                  (1.0 + sin(((double)frame) * R[2])) / 2,
                  (1.0 + sin(((double)frame) * R[3])) / 2,
                  (1.0 + sin(((double)frame) * R[4])) / 2,
                  (1.0 + sin(((double)frame) * R[5])) / 2);
        frame++;

paused:
        cucul_dither_bitmap(cv, 0, 0,
                            cucul_get_canvas_width(cv),
                            cucul_get_canvas_height(cv),
                            dither, screen);

        cucul_blit(c2, 0, 0, cv, NULL);
        cucul_invert(c2);


        cucul_blit(cv, 0, 0, c2, mask);

        cucul_set_color(cv, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
        cucul_putstr(cv, cucul_get_canvas_width(cv) - 30,
                         cucul_get_canvas_height(cv) - 2,
                         " -=[ Powered by libcaca ]=- ");
        caca_refresh_display(dp);
    }

end:
    cucul_free_dither(dither);
    caca_free_display(dp);
    cucul_free_canvas(cv);

    return 0;
}

static void do_plasma(unsigned char *pixels, double x_1, double y_1,
                      double x_2, double y_2, double x_3, double y_3)
{
    unsigned int X1 = x_1 * (TABLEX / 2),
                 Y1 = y_1 * (TABLEY / 2),
                 X2 = x_2 * (TABLEX / 2),
                 Y2 = y_2 * (TABLEY / 2),
                 X3 = x_3 * (TABLEX / 2),
                 Y3 = y_3 * (TABLEY / 2);
    unsigned int y;
    unsigned char * t1 = table + X1 + Y1 * TABLEX,
                  * t2 = table + X2 + Y2 * TABLEX,
                  * t3 = table + X3 + Y3 * TABLEX;

    for(y = 0; y < YSIZ; y++)
    {
        unsigned int x;
        unsigned char * tmp = pixels + y * YSIZ;
        unsigned int ty = y * TABLEX, tmax = ty + XSIZ;
        for(x = 0; ty < tmax; ty++, tmp++)
            tmp[0] = t1[ty] + t2[ty] + t3[ty];
    }
}

static int main_ball(int argc, char **argv)
{
    cucul_canvas_t *cv; caca_display_t *dp;
    unsigned int r[256], g[256], b[256], a[256];
    float dd[METABALLS], di[METABALLS], dj[METABALLS], dk[METABALLS];
    unsigned int x[METABALLS], y[METABALLS];
    cucul_dither_t *cucul_dither;
    float i = 10.0, j = 17.0, k = 11.0;
    int p, frame = 0, pause = 0;
    double frameOffset[360 + 80];

    cv = cucul_create_canvas(0, 0);
    if(!cv)
        return 1;
    dp = caca_create_display(cv);
    if(!dp)
        return 1;

    caca_set_display_time(dp, 20000);

    /* Make the palette eatable by libcaca */
    for(p = 0; p < 256; p++)
        r[p] = g[p] = b[p] = a[p] = 0x0;
    r[255] = g[255] = b[255] = 0xfff;

    /* Create a libcucul dither smaller than our pixel buffer, so that we
     * display only the interesting part of it */
    cucul_dither = cucul_create_dither(8, XSIZ - METASIZE, YSIZ - METASIZE,
                                       XSIZ, 0, 0, 0, 0);

    /* Generate ball sprite */
    create_ball();

    for(p = 0; p < METABALLS; p++)
    {
        dd[p] = cucul_rand(0, 100);
        di[p] = (float)cucul_rand(500, 4000) / 6000.0;
        dj[p] = (float)cucul_rand(500, 4000) / 6000.0;
        dk[p] = (float)cucul_rand(500, 4000) / 6000.0;
    }

    for(frame = 0; frame < 360 + 80; frame++)
        frameOffset[frame] = 1.0 + sin((double)(frame * M_PI / 60));

    /* Go ! */
    for(;;)
    {
        caca_event_t ev;
        if(caca_get_event(dp, CACA_EVENT_KEY_PRESS, &ev, 0))
        {
            switch(ev.data.key.ch)
            {
                case CACA_KEY_ESCAPE: goto end;
                case ' ': pause = !pause;
            }
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
            double c1 = frameOffset[frame];
            double c2 = frameOffset[frame+40];
            double c3 = frameOffset[frame+80];

            t1 = p < 0x40 ? 0 : p < 0xc0 ? (p - 0x40) * 0x20 : 0xfff;
            t2 = p < 0xe0 ? 0 : (p - 0xe0) * 0x80;
            t3 = p < 0x40 ? p * 0x40 : 0xfff;

            r[p] = (c1 * t1 + c2 * t2 + c3 * t3) / 4;
            g[p] = (c1 * t2 + c2 * t3 + c3 * t1) / 4;
            b[p] = (c1 * t3 + c2 * t1 + c3 * t2) / 4;
        }

        /* Set the palette */
        cucul_set_dither_palette(cucul_dither, r, g, b, a);

        /* Silly paths for our balls */
        for(p = 0; p < METABALLS; p++)
        {
            float u = di[p] * i + dj[p] * j + dk[p] * sin(di[p] * k);
            float v = dd[p] + di[p] * j + dj[p] * k + dk[p] * sin(dk[p] * i);
            u = sin(i + u * 2.1) * (1.0 + sin(u));
            v = sin(j + v * 1.9) * (1.0 + sin(v));
            x[p] = (XSIZ - METASIZE) / 2 + u * (XSIZ - METASIZE) / 4;
            y[p] = (YSIZ - METASIZE) / 2 + v * (YSIZ - METASIZE) / 4;
        }

        i += 0.011;
        j += 0.017;
        k += 0.019;

        memset(screen, 0, XSIZ * YSIZ);

        /* Here is all the trick. Maybe if you're that
         * clever you'll understand. */
        for(p = 0; p < METABALLS; p++)
            draw_ball(x[p], y[p]);

paused:
        /* Draw our virtual buffer to screen, letting libcucul resize it */
        cucul_dither_bitmap(cv, 0, 0,
                          cucul_get_canvas_width(cv),
                          cucul_get_canvas_height(cv),
                          cucul_dither, screen + (METASIZE / 2) * (1 + XSIZ));
        cucul_set_color(cv, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
        cucul_putstr(cv, cucul_get_canvas_width(cv) - 30,
                         cucul_get_canvas_height(cv) - 2,
                         " -=[ Powered by libcaca ]=- ");

        caca_refresh_display(dp);
    }

    /* End, bye folks */
end:
    cucul_free_dither(cucul_dither);
    caca_free_display(dp);
    cucul_free_canvas(cv);

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
        color = screen[b] + metaball[i];

        if(color > 255)
            color = 255;

        screen[b] = color;
        if(e == METASIZE)
        {
            e = 0;
            b += XSIZ - METASIZE;
        }
        b++;
        e++;
    }
}

static int main_moir(int argc, char **argv)
{
    cucul_canvas_t *cv; caca_display_t *dp;
    unsigned int red[256], green[256], blue[256], alpha[256];
    cucul_dither_t *dither;
    int i, x, y, frame = 0, pause = 0;

    cv = cucul_create_canvas(0, 0);
    if(!cv)
        return 1;
    dp = caca_create_display(cv);
    if(!dp)
        return 1;

    caca_set_display_time(dp, 20000);

    /* Fill various tables */
    for(i = 0 ; i < 256; i++)
        red[i] = green[i] = blue[i] = alpha[i] = 0;

    red[0] = green[0] = blue[0] = 0x777;
    red[1] = green[1] = blue[1] = 0xfff;

    /* Fill the circle */
    for(i = DISCSIZ * 2; i > 0; i -= DISCTHICKNESS)
        draw_disc(i, (i / DISCTHICKNESS) % 2);

    /* Create a libcucul dither */
    dither = cucul_create_dither(8, XSIZ, YSIZ, XSIZ, 0, 0, 0, 0);

    /* Main loop */
    for(;;)
    {
        caca_event_t ev;
        if(caca_get_event(dp, CACA_EVENT_KEY_PRESS, &ev, 0))
        {
            switch(ev.data.key.ch)
            {
                case CACA_KEY_ESCAPE: goto end;
                case ' ': pause = !pause;
            }
        }

        if(pause)
            goto paused;

        memset(screen, 0, XSIZ * YSIZ);

        /* Set the palette */
        red[0] = 0.5 * (1 + sin(0.05 * frame)) * 0xfff;
        green[0] = 0.5 * (1 + cos(0.07 * frame)) * 0xfff;
        blue[0] = 0.5 * (1 + cos(0.06 * frame)) * 0xfff;

        red[1] = 0.5 * (1 + sin(0.07 * frame + 5.0)) * 0xfff;
        green[1] = 0.5 * (1 + cos(0.06 * frame + 5.0)) * 0xfff;
        blue[1] = 0.5 * (1 + cos(0.05 * frame + 5.0)) * 0xfff;

        cucul_set_dither_palette(dither, red, green, blue, alpha);

        /* Draw circles */
        x = cos(0.07 * frame + 5.0) * 128.0 + (XSIZ / 2);
        y = sin(0.11 * frame) * 128.0 + (YSIZ / 2);
        put_disc(x, y);

        x = cos(0.13 * frame + 2.0) * 64.0 + (XSIZ / 2);
        y = sin(0.09 * frame + 1.0) * 64.0 + (YSIZ / 2);
        put_disc(x, y);

        frame++;

paused:
        cucul_dither_bitmap(cv, 0, 0,
                            cucul_get_canvas_width(cv),
                            cucul_get_canvas_height(cv),
                            dither, screen);
        cucul_set_color(cv, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
        cucul_putstr(cv, cucul_get_canvas_width(cv) - 30,
                         cucul_get_canvas_height(cv) - 2,
                         " -=[ Powered by libcaca ]=- ");
        caca_refresh_display(dp);
    }

end:
    cucul_free_dither(dither);
    caca_free_display(dp);
    cucul_free_canvas(cv);

    return 0;
}

static void put_disc(int x, int y)
{
    char *src = ((char*)disc) + (DISCSIZ / 2 - x) + (DISCSIZ / 2 - y) * DISCSIZ;
    int i, j;

    for(j = 0; j < YSIZ; j++)
        for(i = 0; i < XSIZ; i++)
    {
        screen[i + XSIZ * j] ^= src[i + DISCSIZ * j];
    }
}

static void draw_disc(int r, char color)
{
    int t, dx, dy;

    for(t = 0, dx = 0, dy = r; dx <= dy; dx++)
    {
        draw_line(dx / 3,   dy / 3, color);
        draw_line(dy / 3,   dx / 3, color);

        t += t > 0 ? dx - dy-- : dx;
    }
}

static void draw_line(int x, int y, char color)
{
    if(x == 0 || y == 0 || y > DISCSIZ / 2)
        return;

    if(x > DISCSIZ / 2)
        x = DISCSIZ / 2;

    memset(disc + (DISCSIZ / 2) - x + DISCSIZ * ((DISCSIZ / 2) - y),
           color, 2 * x - 1);
    memset(disc + (DISCSIZ / 2) - x + DISCSIZ * ((DISCSIZ / 2) + y - 1),
           color, 2 * x - 1);
}

