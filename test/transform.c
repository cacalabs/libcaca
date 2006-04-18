/*
 *  unicode       libcaca Unicode rendering test program
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
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

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#else
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#endif

#include <stdio.h>

#include "cucul.h"
#include "caca.h"

static char const *pig[] =
{
    ",--.   ,--.",
    "\\  /-~-\\  /",
    " )' o O `(",
    "(  ,---.  )",
    " `(_o_o_)'",
    "   )`-'(",
    NULL
};

static char const *duck[] =
{
    "                ,~~.",
    "    __     ,   (  O )>",
    "___( o)>   )`~~'   (",
    "\\ <_. )   (  .__)   )",
    " `---'     `-.____,'",
    NULL
};

int main(void)
{
    caca_event_t ev;
    cucul_canvas_t *cv, *normal, *flip, *flop, *rotate;
    caca_display_t *dp;
    int i;

    cv = cucul_create(0, 0);
    dp = caca_attach(cv);

    normal = cucul_create(70, 6);
    flip = cucul_create(70, 6);
    flop = cucul_create(70, 6);
    rotate = cucul_create(70, 6);

    cucul_set_color(normal, CUCUL_COLOR_LIGHTMAGENTA, CUCUL_COLOR_BLACK);
    for(i = 0; pig[i]; i++)
        cucul_putstr(normal, 55, i, pig[i]);

    cucul_set_color(normal, CUCUL_COLOR_LIGHTGREEN, CUCUL_COLOR_BLACK);
    for(i = 0; duck[i]; i++)
        cucul_putstr(normal, 30, 1 + i, duck[i]);

    cucul_set_color(normal, CUCUL_COLOR_LIGHTCYAN, CUCUL_COLOR_BLACK);
    cucul_putstr(normal, 1, 1, "hahaha mais vieux porc immonde !!");

    cucul_set_color(normal, CUCUL_COLOR_YELLOW, CUCUL_COLOR_BLACK);
    cucul_putstr(normal, 4, 2, "\\o\\ \\o| _o/ \\o_ |o/ /o/");

    cucul_set_color(normal, CUCUL_COLOR_WHITE, CUCUL_COLOR_LIGHTRED);
    cucul_putstr(normal, 7, 3, "▙▘▌▙▘▞▖▞▖▌ ▞▖▌ ▌▌");
    cucul_putstr(normal, 7, 4, "▛▖▌▛▖▚▘▚▘▚▖▚▘▚▖▖▖");
    cucul_set_color(normal, CUCUL_COLOR_BLACK, CUCUL_COLOR_LIGHTRED);
    cucul_putstr(normal, 4, 3, "▓▒░");
    cucul_putstr(normal, 4, 4, "▓▒░");
    cucul_putstr(normal, 24, 3, "░▒▓");
    cucul_putstr(normal, 24, 4, "░▒▓");

    /* Flip, flop and rotate our working canvas */
    cucul_blit(flip, 0, 0, normal, NULL);
    cucul_flip(flip);

    cucul_blit(flop, 0, 0, normal, NULL);
    cucul_flop(flop);

    cucul_blit(rotate, 0, 0, normal, NULL);
    cucul_rotate(rotate);

    /* Blit the transformed canvas onto the main canvas */
    cucul_set_color(cv, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
    cucul_putstr(cv, 0, 0, "normal");
    cucul_blit(cv, 10, 0, normal, NULL);
    cucul_putstr(cv, 0, 6, "flip");
    cucul_blit(cv, 10, 6, flip, NULL);
    cucul_putstr(cv, 0, 12, "flop");
    cucul_blit(cv, 10, 12, flop, NULL);
    cucul_putstr(cv, 0, 18, "rotate");
    cucul_blit(cv, 10, 18, rotate, NULL);

    caca_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, &ev, -1);

    caca_detach(dp);
    cucul_free(rotate);
    cucul_free(flop);
    cucul_free(flip);
    cucul_free(normal);
    cucul_free(cv);

    return 0;
}

