/*
 *  unicode       libcaca Unicode rendering test program
 *  Copyright (c) 2006-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <string.h>
#endif

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
    caca_canvas_t *cv, *image, *tmp, *sprite;
    caca_display_t *dp;

    cv = caca_create_canvas(0, 0);
    if(cv == NULL)
    {
        printf("Can't created canvas\n");
        return -1;
    }
    dp = caca_create_display(cv);
    if(dp == NULL)
    {
        printf("Can't create display\n");
        return -1;
    }

    image = caca_create_canvas(70, 6);
    tmp = caca_create_canvas(70, 6);
    sprite = caca_create_canvas(0, 0);

    caca_set_color_ansi(sprite, CACA_LIGHTMAGENTA, CACA_BLACK);
    caca_import_canvas_from_memory(sprite, pig, strlen(pig), "text");
    caca_blit(image, 55, 0, sprite, NULL);

    caca_set_color_ansi(sprite, CACA_LIGHTGREEN, CACA_BLACK);
    caca_import_canvas_from_memory(sprite, duck, strlen(duck), "text");
    caca_blit(image, 30, 1, sprite, NULL);

    caca_set_color_ansi(image, CACA_LIGHTCYAN, CACA_BLACK);
    caca_put_str(image, 1, 1, "hahaha mais vieux porc immonde !! [⽼ ⾗]");
    caca_set_color_ansi(image, CACA_LIGHTRED, CACA_BLACK);
    caca_put_char(image, 38, 1, '|');

    caca_set_color_ansi(image, CACA_YELLOW, CACA_BLACK);
    caca_put_str(image, 4, 2, "\\o\\ \\o| _o/ \\o_ |o/ /o/");

    caca_set_color_ansi(image, CACA_WHITE, CACA_LIGHTRED);
    caca_put_str(image, 7, 3, "▙▘▌▙▘▞▖▞▖▌ ▞▖▌ ▌▌");
    caca_put_str(image, 7, 4, "▛▖▌▛▖▚▘▚▘▚▖▚▘▚▖▖▖");
    caca_set_color_ansi(image, CACA_BLACK, CACA_LIGHTRED);
    caca_put_str(image, 4, 3, "▓▒░");
    caca_put_str(image, 4, 4, "▓▒░");
    caca_put_str(image, 24, 3, "░▒▓");
    caca_put_str(image, 24, 4, "░▒▓");

    /* Blit the transformed canvas onto the main canvas */
    caca_set_color_ansi(cv, CACA_WHITE, CACA_BLUE);
    caca_put_str(cv, 0, 0, "normal");
    caca_blit(cv, 10, 0, image, NULL);

    caca_put_str(cv, 0, 6, "flip");
    caca_blit(tmp, 0, 0, image, NULL);
    caca_flip(tmp);
    caca_blit(cv, 10, 6, tmp, NULL);

    caca_put_str(cv, 0, 12, "flop");
    caca_blit(tmp, 0, 0, image, NULL);
    caca_flop(tmp);
    caca_blit(cv, 10, 12, tmp, NULL);

    caca_put_str(cv, 0, 18, "rotate");
    caca_blit(tmp, 0, 0, image, NULL);
    caca_rotate_180(tmp);
    caca_blit(cv, 10, 18, tmp, NULL);

    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    caca_free_display(dp);
    caca_free_canvas(tmp);
    caca_free_canvas(sprite);
    caca_free_canvas(image);
    caca_free_canvas(cv);

    return 0;
}

