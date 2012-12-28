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
#endif

#include "caca.h"

int main(int argc, char *argv[])
{
    caca_canvas_t *cv;
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
    caca_set_color_ansi(cv, CACA_WHITE, CACA_BLUE);
    caca_put_str(cv, 1, 1, "Basic Unicode support");

    caca_set_color_ansi(cv, CACA_DEFAULT, CACA_TRANSPARENT);
    caca_put_str(cv, 1, 2, "This is ASCII:    | abc DEF 123 !@# |");
    caca_put_str(cv, 1, 3, "This is Unicode:  | äßç δεφ ☺♥♀ ╞╬╗ |");
    caca_put_str(cv, 1, 4, "And this is, too: | ἀβϛ ΔЗҒ ᚴᛒᛯ ♩♔✈ |");
    caca_put_str(cv, 41, 4, "Size test: 018adxmygWX'_ÍçÕĔŷ ﻙ が本");

    caca_put_str(cv, 1, 5, "If the three lines do not have the same length, there is a bug somewhere.");

    caca_set_color_ansi(cv, CACA_WHITE, CACA_BLUE);
    caca_put_str(cv, 1, 7, "Gradient glyphs");

    caca_set_color_ansi(cv, CACA_DEFAULT, CACA_TRANSPARENT);
    caca_put_str(cv, 31,  8, "  0%");
    caca_put_str(cv, 31,  9, " 25%");
    caca_put_str(cv, 31, 10, " 50%");
    caca_put_str(cv, 31, 11, " 75%");
    caca_put_str(cv, 31, 12, "100%");

    caca_set_color_ansi(cv, CACA_LIGHTRED, CACA_LIGHTGREEN);
    caca_put_str(cv, 1,  8, "                             ");
    caca_put_str(cv, 1,  9, "░░░░░░░░░░░░░░░░░░░░░░░░░░░░░");
    caca_put_str(cv, 1, 10, "▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒");
    caca_put_str(cv, 1, 11, "▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓");
    caca_put_str(cv, 1, 12, "█████████████████████████████");

    caca_set_color_ansi(cv, CACA_LIGHTGREEN, CACA_LIGHTRED);
    caca_put_str(cv, 36,  8, "█████████████████████████████");
    caca_put_str(cv, 36,  9, "▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓");
    caca_put_str(cv, 36, 10, "▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒");
    caca_put_str(cv, 36, 11, "░░░░░░░░░░░░░░░░░░░░░░░░░░░░░");
    caca_put_str(cv, 36, 12, "                             ");

    caca_set_color_ansi(cv, CACA_WHITE, CACA_BLUE);
    caca_put_str(cv, 1, 14, "Double width characters");

    caca_set_color_ansi(cv, CACA_LIGHTRED, CACA_TRANSPARENT);
    caca_put_str(cv, 1, 15, "| ドラゴン ボーレ |");
    caca_set_color_ansi(cv, CACA_DEFAULT, CACA_TRANSPARENT);
    caca_put_str(cv, 1, 16, "| ()()()() ()()() |");
    caca_set_color_ansi(cv, CACA_YELLOW, CACA_TRANSPARENT);
    caca_put_str(cv, 1, 17, "| ドラゴン");
    caca_put_str(cv, 12, 17, "ボーレ |");

    caca_set_color_ansi(cv, CACA_DEFAULT, CACA_TRANSPARENT);
    caca_put_str(cv, 1, 18, "If the three lines do not have the same length, there is a bug somewhere.");

    caca_put_str(cv, 1, 20, "CP437 glyphs: ☺ ☻ ♥ ♦ ♣ ♠ • ◘ ○ ◙ ♂ ♀ ♪ ♫ ☼ ► ◄ ↕ ‼ ¶ § ▬ ↨ ↑ ↓ → ← ∟ ↔ ▲ ▼");
    caca_put_str(cv, 1, 21, "more CP437: α ß Γ π Σ σ µ τ Φ Θ Ω δ ∞ φ ε ∩ ≡ ± ≥ ≤ ⌠ ⌡ ÷ ≈ ° ∙ · √ ⁿ ² ■");
    caca_put_str(cv, 1, 22, "drawing blocks: ███ ▓▓▓ ▒▒▒ ░░░ ▀ ▄ ▌ ▐ █ ▖ ▗ ▘ ▝ ▚ ▞ ▙ ▛ ▜ ▟ ─ │ ┌ ┐ └ ┘ ├ ┤");
    caca_put_str(cv, 1, 23, "more drawing: ┬ ┴ ┼ ═ ║ ╒ ╓ ╔ ╕ ╖ ╗ ╘ ╙ ╚ ╛ ╜ ╝ ╞ ╟ ╠ ╡ ╢ ╣ ╤ ╥ ╦ ╧ ╨ ╩ ╪ ╫ ╬");
    caca_put_str(cv, 1, 24, "misc Unicode: ● ☭ ☮ ☯ ♔ ♛ ♙ ♞ ⚒ ⚓ ⚠");

    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    caca_free_display(dp);
    caca_free_canvas(cv);

    return 0;
}

