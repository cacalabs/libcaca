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
#include "common.h"
#if !defined(__KERNEL__)
#   if defined(HAVE_INTTYPES_H)
#      include <inttypes.h>
#   endif
#   include <stdio.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "caca.h"

static char const pig[] =
    ",--.   ,--.\n"
    "\\  /-~-\\  /\n"
    " )' o O `(\n"
    "(  ,---.  )\n"
    " `(_o_o_)'\n"
    "   )`-'(\n";

static char const duck[] =
    "                ,~~.\n"
    "    __     ,   (  O )>\n"
    "___( o)>   )`~~'   (\n"
    "\\ <_. )   (  .__)   )\n"
    " `---'     `-.____,'\n";

int main(int argc, char *argv[])
{
    cucul_canvas_t *cv, *image, *tmp, *sprite;
    caca_display_t *dp;

    cv = cucul_create_canvas(0, 0);
    dp = caca_create_display(cv);

    image = cucul_create_canvas(70, 6);
    tmp = cucul_create_canvas(70, 6);
    sprite = cucul_create_canvas(0, 0);

    cucul_set_color_ansi(sprite, CUCUL_LIGHTMAGENTA, CUCUL_BLACK);
    cucul_import_memory(sprite, pig, strlen(pig), "text");
    cucul_blit(image, 55, 0, sprite, NULL);

    cucul_set_color_ansi(sprite, CUCUL_LIGHTGREEN, CUCUL_BLACK);
    cucul_import_memory(sprite, duck, strlen(duck), "text");
    cucul_blit(image, 30, 1, sprite, NULL);

    cucul_set_color_ansi(image, CUCUL_LIGHTCYAN, CUCUL_BLACK);
    cucul_putstr(image, 1, 1, "hahaha mais vieux porc immonde !! [⽼ ⾗]");
    cucul_set_color_ansi(image, CUCUL_LIGHTRED, CUCUL_BLACK);
    cucul_putchar(image, 38, 1, '|');

    cucul_set_color_ansi(image, CUCUL_YELLOW, CUCUL_BLACK);
    cucul_putstr(image, 4, 2, "\\o\\ \\o| _o/ \\o_ |o/ /o/");

    cucul_set_color_ansi(image, CUCUL_WHITE, CUCUL_LIGHTRED);
    cucul_putstr(image, 7, 3, "▙▘▌▙▘▞▖▞▖▌ ▞▖▌ ▌▌");
    cucul_putstr(image, 7, 4, "▛▖▌▛▖▚▘▚▘▚▖▚▘▚▖▖▖");
    cucul_set_color_ansi(image, CUCUL_BLACK, CUCUL_LIGHTRED);
    cucul_putstr(image, 4, 3, "▓▒░");
    cucul_putstr(image, 4, 4, "▓▒░");
    cucul_putstr(image, 24, 3, "░▒▓");
    cucul_putstr(image, 24, 4, "░▒▓");

    /* Blit the transformed canvas onto the main canvas */
    cucul_set_color_ansi(cv, CUCUL_WHITE, CUCUL_BLUE);
    cucul_putstr(cv, 0, 0, "normal");
    cucul_blit(cv, 10, 0, image, NULL);

    cucul_putstr(cv, 0, 6, "flip");
    cucul_blit(tmp, 0, 0, image, NULL);
    cucul_flip(tmp);
    cucul_blit(cv, 10, 6, tmp, NULL);

    cucul_putstr(cv, 0, 12, "flop");
    cucul_blit(tmp, 0, 0, image, NULL);
    cucul_flop(tmp);
    cucul_blit(cv, 10, 12, tmp, NULL);

    cucul_putstr(cv, 0, 18, "rotate");
    cucul_blit(tmp, 0, 0, image, NULL);
    cucul_rotate(tmp);
    cucul_blit(cv, 10, 18, tmp, NULL);

    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    caca_free_display(dp);
    cucul_free_canvas(tmp);
    cucul_free_canvas(image);
    cucul_free_canvas(cv);

    return 0;
}

