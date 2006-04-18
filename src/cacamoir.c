/*
 *  cacamoir      moiré circles effect for libcaca
 *  Copyright (c) 2004 Sam Hocevar <sam@zoy.org>
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
#   include <math.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "caca.h"

/* Virtual buffer size */
#define XSIZ 256
#define YSIZ 256

#define DISCSIZ 512
#define DISCTHICKNESS 96

static unsigned char screen[XSIZ * YSIZ];
static unsigned char disc[DISCSIZ * DISCSIZ];

static void put_disc(int, int);
static void draw_disc(int, char);
static void draw_line(int, int, char);

int main (int argc, char **argv)
{
    cucul_canvas_t *cv; caca_display_t *dp;
    unsigned int red[256], green[256], blue[256], alpha[256];
    cucul_dither_t *dither;
    int i, x, y, frame = 0, pause = 0;

    cv = cucul_create_canvas(0, 0);
    if(!cv)
        return 1;
    dp = caca_attach(cv);
    if(!dp)
        return 1;

    caca_set_delay(dp, 20000);

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
        caca_display(dp);
    }

end:
    cucul_free_dither(dither);
    caca_detach(dp);
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

