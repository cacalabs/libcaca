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
    cucul_t *qq; caca_t *kk;
    unsigned int red[256], green[256], blue[256], alpha[256];
    struct cucul_bitmap *bitmap;
    int i, x, y, frame = 0, pause = 0;

    qq = cucul_create(0, 0);
    if(!qq)
        return 1;
    kk = caca_attach(qq);
    if(!kk)
        return 1;

    caca_set_delay(kk, 20000);

    /* Fill various tables */
    for(i = 0 ; i < 256; i++)
        red[i] = green[i] = blue[i] = alpha[i] = 0;

    red[0] = green[0] = blue[0] = 0x777;
    red[1] = green[1] = blue[1] = 0xfff;

    /* Fill the circle */
    for(i = DISCSIZ * 2; i > 0; i -= DISCTHICKNESS)
        draw_disc(i, (i / DISCTHICKNESS) % 2);

    /* Create a libcucul bitmap */
    bitmap = cucul_create_bitmap(8, XSIZ, YSIZ, XSIZ, 0, 0, 0, 0);

    /* Main loop */
    for(;;)
    {
        switch(caca_get_event(kk, CACA_EVENT_KEY_PRESS))
        {
            case CACA_EVENT_KEY_PRESS | CACA_KEY_ESCAPE: goto end;
            case CACA_EVENT_KEY_PRESS | ' ': pause = !pause;
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

        cucul_set_bitmap_palette(bitmap, red, green, blue, alpha);

        /* Draw circles */
        x = cos(0.07 * frame + 5.0) * 128.0 + (XSIZ / 2);
        y = sin(0.11 * frame) * 128.0 + (YSIZ / 2);
        put_disc(x, y);

        x = cos(0.13 * frame + 2.0) * 64.0 + (XSIZ / 2);
        y = sin(0.09 * frame + 1.0) * 64.0 + (YSIZ / 2);
        put_disc(x, y);

        frame++;

paused:
        cucul_draw_bitmap(qq, 0, 0,
                          cucul_get_width(qq) - 1, cucul_get_height(qq) - 1,
                          bitmap, screen);
        caca_display(kk);
    }

end:
    cucul_free_bitmap(bitmap);
    caca_detach(kk);
    cucul_free(qq);

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

