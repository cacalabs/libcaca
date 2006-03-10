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

#include "cucul.h"
#include "caca.h"

int main(void)
{
    cucul_t *qq;
    caca_t *kk;

    qq = cucul_init();
    kk = caca_attach(qq);

    cucul_set_color(qq, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
    cucul_putstr(qq, 1, 1, "Basic Unicode support");

    cucul_set_color(qq, CUCUL_COLOR_LIGHTGRAY, CUCUL_COLOR_BLACK);
    cucul_putstr(qq, 1, 2, "This is ASCII:   [ abc DEF 123 !@# ]");
    cucul_putstr(qq, 1, 3, "This is Unicode: [ äβç ΔЗҒ ░▒▓ ♩♔✈ ]");

    cucul_putstr(qq, 1, 4, "If the two lines do not have the same length, there is a bug somewhere.");

    cucul_set_color(qq, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
    cucul_putstr(qq, 1, 6, "Gradient glyphs");

    cucul_set_color(qq, CUCUL_COLOR_LIGHTGRAY, CUCUL_COLOR_BLACK);
    cucul_putstr(qq, 31,  7, "  0%");
    cucul_putstr(qq, 31,  8, " 25%");
    cucul_putstr(qq, 31,  9, " 50%");
    cucul_putstr(qq, 31, 10, " 75%");
    cucul_putstr(qq, 31, 11, "100%");

    cucul_set_color(qq, CUCUL_COLOR_RED, CUCUL_COLOR_CYAN);
    cucul_putstr(qq, 1,  7, "                             ");
    cucul_putstr(qq, 1,  8, "░░░░░░░░░░░░░░░░░░░░░░░░░░░░░");
    cucul_putstr(qq, 1,  9, "▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒");
    cucul_putstr(qq, 1, 10, "▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓");
    cucul_putstr(qq, 1, 11, "█████████████████████████████");

    cucul_set_color(qq, CUCUL_COLOR_CYAN, CUCUL_COLOR_RED);
    cucul_putstr(qq, 36,  7, "█████████████████████████████");
    cucul_putstr(qq, 36,  8, "▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓");
    cucul_putstr(qq, 36,  9, "▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒");
    cucul_putstr(qq, 36, 10, "░░░░░░░░░░░░░░░░░░░░░░░░░░░░░");
    cucul_putstr(qq, 36, 11, "                             ");

    cucul_set_color(qq, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
    cucul_putstr(qq, 1, 13, "Double width characters");

    cucul_set_color(qq, CUCUL_COLOR_LIGHTRED, CUCUL_COLOR_BLACK);
    cucul_putstr(qq, 1, 14, "ドラゴン ボーレ |");
    cucul_set_color(qq, CUCUL_COLOR_LIGHTGRAY, CUCUL_COLOR_BLACK);
    cucul_putstr(qq, 1, 15, "()()()() ()()() |");
    cucul_set_color(qq, CUCUL_COLOR_YELLOW, CUCUL_COLOR_BLACK);
    cucul_putstr(qq, 1, 16, "ドラゴン");
    cucul_putstr(qq, 10, 16, "ボーレ |");

    cucul_set_color(qq, CUCUL_COLOR_LIGHTGRAY, CUCUL_COLOR_BLACK);
    cucul_putstr(qq, 1, 17, "If the three lines do not have the same length, there is a bug somewhere.");

    caca_display(kk);

    while(!caca_get_event(kk, CACA_EVENT_KEY_PRESS));

    caca_detach(kk);
    cucul_end(qq);

    return 0;
}

