/*
 *  optipal       S-Lang optimised palette generator for libcaca
 *  Copyright (c) 2003 Sam Hocevar <sam@zoy.org>
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

#include <stdio.h>

#include "caca.h"

static void base_colors(void);
static void emulated_colors(void);
static void unused_colors(void);

static int slang_assoc[16*16], palette[16*16];

/* 6 colours in hue order */
static enum caca_color const hue_list[] =
{
    CACA_COLOR_RED,
    CACA_COLOR_BROWN,
    CACA_COLOR_GREEN,
    CACA_COLOR_CYAN,
    CACA_COLOR_BLUE,
    CACA_COLOR_MAGENTA
};

#define SETPAIR(_fg, _bg, _n) \
    do \
    { \
        int fg = _fg, bg = _bg, n = _n; \
        slang_assoc[fg + 16 * bg] = n; \
        palette[n] = fg + 16 * bg; \
    } \
    while(0);

int main(void)
{
    int i;

    for(i = 0; i < 16 * 16; i++)
    {
        slang_assoc[i] = -1;
        palette[i] = -1;
    }

    /* The base colour pairs (0-127) */
    base_colors();

    /* Now the less important pairs that we can afford to emulate using
     * previously defined colour pairs. */
    emulated_colors();

    /* Fill the rest of the palette with equal colour pairs such as black
     * on black. They will never be used, but nevermind. */
    unused_colors();

    /* Output the palette */
    printf("static int const slang_palette[2*16*16] =\n{\n");
    for(i = 0; i < 16 * 16; i++)
    {
        if((i % 8) == 0) printf("    ");
        printf("%2i, %2i,  ", palette[i] % 16, palette[i] / 16);
        if((i % 8) == 7) printf("\n");
    }
    printf("};\n\n");

    /* Output the association table */
    printf("static int const slang_assoc[16*16] =\n{\n");
    for(i = 0; i < 16 * 16; i++)
    {
        if((i % 16) == 0) printf("    ");
        printf("%i, ", slang_assoc[i]);
        if((i % 16) == 15) printf("\n");
    }
    printf("};\n");

    return 0;
}

/*
 * XXX: See the NOTES file for what follows
 */

static void base_colors(void)
{
    int i, cur = 0;

    /* black background colour pairs that are needed for the old renderer */
    for(i = 1; i < 16; i++)
        SETPAIR(i, CACA_COLOR_BLACK, cur++);

    /* gray combinations used for grayscale dithering */
    SETPAIR(CACA_COLOR_BLACK, CACA_COLOR_DARKGRAY, cur++);
    SETPAIR(CACA_COLOR_DARKGRAY, CACA_COLOR_LIGHTGRAY, cur++);
    SETPAIR(CACA_COLOR_LIGHTGRAY, CACA_COLOR_DARKGRAY, cur++);
    SETPAIR(CACA_COLOR_WHITE, CACA_COLOR_LIGHTGRAY, cur++);
    SETPAIR(CACA_COLOR_LIGHTGRAY, CACA_COLOR_WHITE, cur++);

    /* white/light, light/dark, lightgray/light, darkgray/dark, dark/black
     * combinations often used for saturation/value dithering (the two
     * other possible combinations, lightgray/dark and darkgray/light, are
     * not considered here) */
    for(i = 1; i < 7; i++)
    {
        SETPAIR(CACA_COLOR_WHITE, i + 8, cur++);
        SETPAIR(i + 8, CACA_COLOR_WHITE, cur++);
        SETPAIR(i, i + 8, cur++);
        SETPAIR(i + 8, i, cur++);
        SETPAIR(CACA_COLOR_LIGHTGRAY, i + 8, cur++);
        SETPAIR(i + 8, CACA_COLOR_LIGHTGRAY, cur++);
        SETPAIR(CACA_COLOR_DARKGRAY, i, cur++);
        SETPAIR(i, CACA_COLOR_DARKGRAY, cur++);
        SETPAIR(CACA_COLOR_BLACK, i, cur++);
    }

    /* next colour combinations for hue dithering (magenta/blue, blue/green
     * and so on) */
    for(i = 0; i < 6; i++)
    {
        SETPAIR(hue_list[i], hue_list[(i + 1) % 6], cur++);
        SETPAIR(hue_list[(i + 1) % 6], hue_list[i], cur++);
        SETPAIR(hue_list[i] + 8, hue_list[(i + 1) % 6] + 8, cur++);
        SETPAIR(hue_list[(i + 1) % 6] + 8, hue_list[i] + 8, cur++);
    }

    /* next colour combinations for hue/value dithering (blue/lightgreen,
     * green/lightblue and so on) */
    for(i = 0; i < 6; i++)
    {
        SETPAIR(hue_list[i], hue_list[(i + 1) % 6] + 8, cur++);
        SETPAIR(hue_list[(i + 1) % 6], hue_list[i] + 8, cur++);
        SETPAIR(hue_list[i] + 8, hue_list[(i + 1) % 6], cur++);
        SETPAIR(hue_list[(i + 1) % 6] + 8, hue_list[i], cur++);
    }

    /* black on light gray, black on white, white on dark gray, dark gray
     * on white, white on blue, light gray on blue (chosen arbitrarily) */
    SETPAIR(CACA_COLOR_BLACK, CACA_COLOR_LIGHTGRAY, cur++);
    SETPAIR(CACA_COLOR_BLACK, CACA_COLOR_WHITE, cur++);
    SETPAIR(CACA_COLOR_WHITE, CACA_COLOR_DARKGRAY, cur++);
    SETPAIR(CACA_COLOR_DARKGRAY, CACA_COLOR_WHITE, cur++);
    SETPAIR(CACA_COLOR_WHITE, CACA_COLOR_BLUE, cur++);
    SETPAIR(CACA_COLOR_LIGHTGRAY, CACA_COLOR_BLUE, cur++);
}

static void emulated_colors(void)
{
    int i;

    /* light gray on dark colour: emulate with light colour on dark colour
     * white on dark colour: emulate with light gray on light colour
     * black on light colour: emulate with dark gray on dark colour
     * dark gray on light colour: emulate with dark colour on light colour
     * light colour on dark gray: emulate with dark colour on dark gray
     * dark colour on light gray: emulate with light colour on light gray
     * dark colour on white: emulate with light colour on white */
    for(i = 1; i < 7; i++)
    {
        if(i != CACA_COLOR_BLUE)
        {
            SETPAIR(CACA_COLOR_LIGHTGRAY, i, 128 +
                    slang_assoc[i + 8 + 16 * i]);
            SETPAIR(CACA_COLOR_WHITE, i, 128 +
                    slang_assoc[CACA_COLOR_LIGHTGRAY + 16 * (i + 8)]);
        }
        SETPAIR(CACA_COLOR_BLACK, i + 8,
                128 + slang_assoc[CACA_COLOR_DARKGRAY + 16 * i]);
        SETPAIR(CACA_COLOR_DARKGRAY, i + 8,
                128 + slang_assoc[i + 16 * (i + 8)]);
        SETPAIR(i + 8, CACA_COLOR_DARKGRAY,
                128 + slang_assoc[i + 16 * CACA_COLOR_DARKGRAY]);
        SETPAIR(i, CACA_COLOR_LIGHTGRAY,
                128 + slang_assoc[i + 8 + 16 * CACA_COLOR_LIGHTGRAY]);
        SETPAIR(i, CACA_COLOR_WHITE,
                128 + slang_assoc[i + 8 + 16 * CACA_COLOR_WHITE]);
    }

    /* 120 degree hue pairs can be emulated as well; for instance blue on
     * red can be emulated using magenta on red, and blue on green using
     * cyan on green */
    for(i = 0; i < 6; i++)
    {
        SETPAIR(hue_list[(i + 2) % 6], hue_list[i],
            128 + slang_assoc[hue_list[(i + 1) % 6] + 16 * hue_list[i]]);
        SETPAIR(hue_list[(i + 2) % 6] + 8, hue_list[i] + 8,
            128 + slang_assoc[hue_list[(i + 1) % 6] + 16 * hue_list[i] + 136]);
        SETPAIR(hue_list[(i + 2) % 6] + 8, hue_list[i],
            128 + slang_assoc[hue_list[(i + 1) % 6] + 16 * hue_list[i] + 8]);
        SETPAIR(hue_list[(i + 2) % 6], hue_list[i] + 8,
            128 + slang_assoc[hue_list[(i + 1) % 6] + 16 * hue_list[i] + 128]);

        SETPAIR(hue_list[(i + 4) % 6], hue_list[i],
            128 + slang_assoc[hue_list[(i + 5) % 6] + 16 * hue_list[i]]);
        SETPAIR(hue_list[(i + 4) % 6] + 8, hue_list[i] + 8,
            128 + slang_assoc[hue_list[(i + 5) % 6] + 16 * hue_list[i] + 136]);
        SETPAIR(hue_list[(i + 4) % 6] + 8, hue_list[i],
            128 + slang_assoc[hue_list[(i + 5) % 6] + 16 * hue_list[i] + 8]);
        SETPAIR(hue_list[(i + 4) % 6], hue_list[i] + 8,
            128 + slang_assoc[hue_list[(i + 5) % 6] + 16 * hue_list[i] + 128]);
    }

    /* dark opposite on dark: emulate with dark opposite on black
     * light opposite on dark: emulate with light opposite on black
     * dark opposite on light: emulate with black on dark
     * light opposite on light: emulate with white on light */
    for(i = 0; i < 6; i++)
    {
        SETPAIR(hue_list[i], hue_list[(i + 3) % 6],
                128 + slang_assoc[hue_list[i] + 16 * CACA_COLOR_BLACK]);
        SETPAIR(hue_list[i] + 8, hue_list[(i + 3) % 6],
                128 + slang_assoc[hue_list[i] + 8 + 16 * CACA_COLOR_BLACK]);
        SETPAIR(hue_list[(i + 3) % 6], hue_list[i] + 8,
                128 + slang_assoc[CACA_COLOR_BLACK + 16 * hue_list[i]]);
        SETPAIR(hue_list[(i + 3) % 6] + 8, hue_list[i] + 8,
                128 + slang_assoc[CACA_COLOR_WHITE + 16 * (hue_list[i] + 8)]);
    }
}

static void unused_colors(void)
{
    int i, j;

    for(i = 0, j = 0; i < 16 * 16; i++)
    {
        if(palette[i] == -1)
        {
            SETPAIR(j, j, i);
            j++;
        }
    }
}

