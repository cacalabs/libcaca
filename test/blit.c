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
    cucul_canvas_t *cv, *sprite;
    caca_display_t *dp;

    cv = cucul_create_canvas(0, 0);
    dp = caca_create_display(cv);

    sprite = cucul_create_canvas(0, 0);
    cucul_set_color_ansi(sprite, CUCUL_LIGHTRED, CUCUL_BLACK);
    cucul_import_memory(sprite, pig, strlen(pig), "text");
    cucul_set_canvas_handle(sprite, cucul_get_canvas_width(sprite) / 2,
                                    cucul_get_canvas_height(sprite) / 2);

    cucul_set_color_ansi(cv, CUCUL_WHITE, CUCUL_BLUE);
    cucul_putstr(cv, 0, 0, "Centered sprite");

    cucul_blit(cv, cucul_get_canvas_width(cv) / 2,
                   cucul_get_canvas_height(cv) / 2, sprite, NULL);

    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    caca_free_display(dp);
    cucul_free_canvas(sprite);
    cucul_free_canvas(cv);

    return 0;
}

