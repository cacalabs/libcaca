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

int main(void)
{
    cucul_canvas_t *cv;
    caca_display_t *dp;

    cv = cucul_create_canvas(0, 0);
    dp = caca_create_display(cv);

    cucul_set_color(cv, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
    cucul_putstr(cv, 1, 1, "Basic Unicode support");

    cucul_set_color(cv, CUCUL_COLOR_LIGHTGRAY, CUCUL_COLOR_BLACK);
    cucul_putstr(cv, 1, 2, "This is ASCII:    | abc DEF 123 !@# |");
    cucul_putstr(cv, 1, 3, "This is Unicode:  | äßç δεφ ☺♥♀ ╞╬╗ |");
    cucul_putstr(cv, 1, 4, "And this is, too: | ἀβϛ ΔЗҒ ᚴᛒᛯ ♩♔✈ |");

    cucul_putstr(cv, 1, 5, "If the three lines do not have the same length, there is a bug somewhere.");

    cucul_set_color(cv, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
    cucul_putstr(cv, 1, 7, "Gradient glyphs");

    cucul_set_color(cv, CUCUL_COLOR_LIGHTGRAY, CUCUL_COLOR_BLACK);
    cucul_putstr(cv, 31,  8, "  0%");
    cucul_putstr(cv, 31,  9, " 25%");
    cucul_putstr(cv, 31, 10, " 50%");
    cucul_putstr(cv, 31, 11, " 75%");
    cucul_putstr(cv, 31, 12, "100%");

    cucul_set_color(cv, CUCUL_COLOR_LIGHTRED, CUCUL_COLOR_LIGHTGREEN);
    cucul_putstr(cv, 1,  8, "                             ");
    cucul_putstr(cv, 1,  9, "░░░░░░░░░░░░░░░░░░░░░░░░░░░░░");
    cucul_putstr(cv, 1, 10, "▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒");
    cucul_putstr(cv, 1, 11, "▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓");
    cucul_putstr(cv, 1, 12, "█████████████████████████████");

    cucul_set_color(cv, CUCUL_COLOR_LIGHTGREEN, CUCUL_COLOR_LIGHTRED);
    cucul_putstr(cv, 36,  8, "█████████████████████████████");
    cucul_putstr(cv, 36,  9, "▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓");
    cucul_putstr(cv, 36, 10, "▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒");
    cucul_putstr(cv, 36, 11, "░░░░░░░░░░░░░░░░░░░░░░░░░░░░░");
    cucul_putstr(cv, 36, 12, "                             ");

    cucul_set_color(cv, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
    cucul_putstr(cv, 1, 14, "Double width characters");

    cucul_set_color(cv, CUCUL_COLOR_LIGHTRED, CUCUL_COLOR_BLACK);
    cucul_putstr(cv, 1, 15, "| ドラゴン ボーレ |");
    cucul_set_color(cv, CUCUL_COLOR_LIGHTGRAY, CUCUL_COLOR_BLACK);
    cucul_putstr(cv, 1, 16, "| ()()()() ()()() |");
    cucul_set_color(cv, CUCUL_COLOR_YELLOW, CUCUL_COLOR_BLACK);
    cucul_putstr(cv, 1, 17, "| ドラゴン");
    cucul_putstr(cv, 10, 17, "ボーレ |");

    cucul_set_color(cv, CUCUL_COLOR_LIGHTGRAY, CUCUL_COLOR_BLACK);
    cucul_putstr(cv, 1, 18, "If the three lines do not have the same length, there is a bug somewhere.");

    cucul_putstr(cv, 1, 20, "CP437 glyphs: ☺ ☻ ♥ ♦ ♣ ♠ • ◘ ○ ◙ ♂ ♀ ♪ ♫ ☼ ► ◄ ↕ ‼ ¶ § ▬ ↨ ↑ ↓ → ← ∟ ↔ ▲ ▼");
    cucul_putstr(cv, 1, 21, "more CP437: α ß Γ π Σ σ µ τ Φ Θ Ω δ ∞ φ ε ∩ ≡ ± ≥ ≤ ⌠ ⌡ ÷ ≈ ° ∙ · √ ⁿ ² ■");
    cucul_putstr(cv, 1, 22, "drawing blocks: ███ ▓▓▓ ▒▒▒ ░░░ ▀ ▄ ▌ ▐ █ ▖ ▗ ▘ ▝ ▚ ▞ ▙ ▛ ▜ ▟");

    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    caca_free_display(dp);
    cucul_free_canvas(cv);

    return 0;
}

