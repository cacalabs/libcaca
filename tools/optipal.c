/*
 *  optipal       S-Lang optimised palette generator for libcaca
 *  Copyright (c) 2003-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
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

#include "caca.h" /* Only necessary for CACA_* macros */

static void base_colors(void);
static void emulated_colors(void);
static void unused_colors(void);

static int slang_assoc[16*16], palette[16*16];

/* 6 colours in hue order */
static unsigned int const hue_list[] =
{
    CACA_RED, CACA_BROWN, CACA_GREEN,
    CACA_CYAN, CACA_BLUE, CACA_MAGENTA
};

#define SETPAIR(_fg, _bg, _n) \
    do \
    { \
        int fg = _fg, bg = _bg, n = _n; \
        slang_assoc[fg + 16 * bg] = n; \
        palette[n] = fg + 16 * bg; \
    } \
    while(0);

int main(int argc, char *argv[])
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
 *  256 character pairs are definable, but only 128 can be used. This is
 *  because slsmg.c's This_Color variable uses its 8th bit to indicate an
 *  alternate character set. Replacing a few 0x7F with 0xFF in sldisply.c
 *  works around the problem but gets rid of the alternate charset.
 *
 *  We can work around this problem. See this usage grid:
 *
 *                  bg                         1 1 1 1 1 1
 *    fg                  0 1 2 3 4 5 6 7  8 9 0 1 2 3 4 5
 *
 *    0 (black)             C C C C C C F  B c c c c c c F
 *    1 (blue)            A   h D h D i f  C C h E h E k g
 *    2 (green)           A h   D h i D f  C h C E h k E g
 *    3 (cyan)            A D D   i h h f  C E E C k h h g
 *    4 (red)             A h h i   D D f  C h h k C E E g
 *    5 (magenta)         A D i h D   h f  C E k h E C h g
 *    6 (brown)           A i D h D h   f  C k E h E h C g
 *    7 (light gray)      A F a a a a a    B C C C C C C B
 *
 *    8 (dark gray)       A C C C C C C B    d d d d d d F
 *    9 (light blue)      A C h E h E j C  e   h D h D l C
 *    10 (light green)    A h C E h j E C  e h   D h l D C
 *    11 (light cyan)     A E E C j h h C  e D D   l h h C
 *    12 (light red)      A h h j C E E C  e h h l   D D C
 *    13 (light magenta)  A E j h E C h C  e D l h D   h C
 *    14 (yellow)         A j E h E h C C  e l D h D h   C
 *    15 (white)          A F b b b b b B  F C C C C C C
 *
 *  ' ': useless colour pairs that can be emulated by printing a space in
 *       any other colour pair that has the same background
 *  'A': black background colour pairs that are needed for the old renderer
 *  'B': gray combinations used for grayscale dithering
 *  'C': white/light, light/dark, lightgray/light, darkgray/dark, dark/black
 *       combinations often used for saturation/value dithering (the two
 *       other possible combinations, lightgray/dark and darkgray/light, are
 *       not considered here)
 *  'D': next colour combinations for hue dithering (magenta/blue, blue/green
 *       and so on)
 *  'E': next colour combinations for hue/value dithering (blue/lightgreen,
 *       green/lightblue and so on)
 *  'F': black on light gray, black on white, white on dark gray, dark gray
 *       on white, white on blue, light gray on blue (chosen arbitrarily)
 *
 *  'A': 15 colour pairs
 *  'A'+'B': 20 colour pairs
 *  'A'+'B'+'C': 74 colour pairs
 *  'A'+'B'+'C'+'D': 98 colour pairs
 *  'A'+'B'+'C'+'D'+'E': 122 colour pairs
 *  'A'+'B'+'C'+'D'+'E'+'F': 128 colour pairs
 *
 *  The remaining slightly important colour pairs are:
 *
 *  'a': light gray on dark colour: emulate with light colour on dark colour
 *  'b': white on dark colour: emulate with light gray on light colour
 *  'c': black on light colour: emulate with dark gray on dark colour
 *  'd': dark gray on light colour: emulate with dark colour on light colour
 *  'e': light colour on dark gray: emulate with dark colour on dark gray
 *  'f': dark colour on light gray: emulate with light colour on light gray
 *  'g': dark colour on white: emulate with light colour on white
 *
 *  And now the seldom used pairs:
 *
 *  'h': 120 degree hue pairs can be emulated as well; for instance blue on
 *       red can be emulated using magenta on red, and blue on green using
 *       cyan on green
 *
 *  And the almost never used pairs:
 *
 *  'i': dark opposite on dark: emulate with dark opposite on black
 *  'j': light opposite on dark: emulate with light opposite on black
 *  'k': dark opposite on light: emulate with black on dark
 *  'l': light opposite on light: emulate with white on light
 */

static void base_colors(void)
{
    int i, cur = 0;

    /* black background colour pairs that are needed for the old renderer */
    for(i = 1; i < 16; i++)
        SETPAIR(i, CACA_BLACK, cur++);

    /* gray combinations used for grayscale dithering */
    SETPAIR(CACA_BLACK, CACA_DARKGRAY, cur++);
    SETPAIR(CACA_DARKGRAY, CACA_LIGHTGRAY, cur++);
    SETPAIR(CACA_LIGHTGRAY, CACA_DARKGRAY, cur++);
    SETPAIR(CACA_WHITE, CACA_LIGHTGRAY, cur++);
    SETPAIR(CACA_LIGHTGRAY, CACA_WHITE, cur++);

    /* white/light, light/dark, lightgray/light, darkgray/dark, dark/black
     * combinations often used for saturation/value dithering (the two
     * other possible combinations, lightgray/dark and darkgray/light, are
     * not considered here) */
    for(i = 1; i < 7; i++)
    {
        SETPAIR(CACA_WHITE, i + 8, cur++);
        SETPAIR(i + 8, CACA_WHITE, cur++);
        SETPAIR(i, i + 8, cur++);
        SETPAIR(i + 8, i, cur++);
        SETPAIR(CACA_LIGHTGRAY, i + 8, cur++);
        SETPAIR(i + 8, CACA_LIGHTGRAY, cur++);
        SETPAIR(CACA_DARKGRAY, i, cur++);
        SETPAIR(i, CACA_DARKGRAY, cur++);
        SETPAIR(CACA_BLACK, i, cur++);
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
    SETPAIR(CACA_BLACK, CACA_LIGHTGRAY, cur++);
    SETPAIR(CACA_BLACK, CACA_WHITE, cur++);
    SETPAIR(CACA_WHITE, CACA_DARKGRAY, cur++);
    SETPAIR(CACA_DARKGRAY, CACA_WHITE, cur++);
    SETPAIR(CACA_WHITE, CACA_BLUE, cur++);
    SETPAIR(CACA_LIGHTGRAY, CACA_BLUE, cur++);
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
        if(i != CACA_BLUE)
        {
            SETPAIR(CACA_LIGHTGRAY, i, 128 +
                    slang_assoc[i + 8 + 16 * i]);
            SETPAIR(CACA_WHITE, i, 128 +
                    slang_assoc[CACA_LIGHTGRAY + 16 * (i + 8)]);
        }
        SETPAIR(CACA_BLACK, i + 8,
                128 + slang_assoc[CACA_DARKGRAY + 16 * i]);
        SETPAIR(CACA_DARKGRAY, i + 8,
                128 + slang_assoc[i + 16 * (i + 8)]);
        SETPAIR(i + 8, CACA_DARKGRAY,
                128 + slang_assoc[i + 16 * CACA_DARKGRAY]);
        SETPAIR(i, CACA_LIGHTGRAY,
                128 + slang_assoc[i + 8 + 16 * CACA_LIGHTGRAY]);
        SETPAIR(i, CACA_WHITE,
                128 + slang_assoc[i + 8 + 16 * CACA_WHITE]);
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
                128 + slang_assoc[hue_list[i] + 16 * CACA_BLACK]);
        SETPAIR(hue_list[i] + 8, hue_list[(i + 3) % 6],
                128 + slang_assoc[hue_list[i] + 8 + 16 * CACA_BLACK]);
        SETPAIR(hue_list[(i + 3) % 6], hue_list[i] + 8,
                128 + slang_assoc[CACA_BLACK + 16 * hue_list[i]]);
        SETPAIR(hue_list[(i + 3) % 6] + 8, hue_list[i] + 8,
                128 + slang_assoc[CACA_WHITE + 16 * (hue_list[i] + 8)]);
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

