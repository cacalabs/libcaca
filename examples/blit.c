/*
 *  blit          libcaca blit test program
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
    "  ,__         __,\n"
    "   \\)`\\_..._/`(/\n"
    "   .'  _   _  '.\n"
    "  /    o\\ /o   \\\n"
    "  |    .-.-.    |  _\n"
    "  |   /() ()\\   | (,`)\n"
    " / \\  '-----'  / \\ .'\n"
    "|   '-..___..-'   |\n"
    "|                 |\n"
    "|                 |\n"
    ";                 ;\n"
    " \\      / \\      /\n"
    "  \\-..-/'-'\\-..-/\n"
    "jgs\\/\\/     \\/\\/\n";

int main(int argc, char *argv[])
{
    caca_canvas_t *cv, *sprite;
    caca_display_t *dp;

    cv = caca_create_canvas(80, 24);
    if(cv == NULL)
    {
        printf("Failed to create canvas\n");
        return 1;
    }

    dp = caca_create_display(cv);
    if(dp == NULL)
    {
        printf("Failed to create display\n");
        return 1;
    }

    sprite = caca_create_canvas(0, 0);
    caca_set_color_ansi(sprite, CACA_LIGHTRED, CACA_BLACK);
    caca_import_canvas_from_memory(sprite, pig, strlen(pig), "text");
    caca_set_canvas_handle(sprite, caca_get_canvas_width(sprite) / 2,
                                    caca_get_canvas_height(sprite) / 2);

    caca_set_color_ansi(cv, CACA_WHITE, CACA_BLUE);
    caca_put_str(cv, 0, 0, "Centered sprite");

    caca_blit(cv, caca_get_canvas_width(cv) / 2,
                   caca_get_canvas_height(cv) / 2, sprite, NULL);

    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    caca_free_display(dp);
    caca_free_canvas(sprite);
    caca_free_canvas(cv);

    return 0;
}

