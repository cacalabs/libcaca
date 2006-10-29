/*
 *  font2tga      libcucul font test program
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
#   include <stdlib.h>
#endif

#include "cucul.h"

#define WIDTH 64

/* Copied from makefont.c */
static unsigned int const blocklist[] =
{
    0x0000, 0x0080, /* Basic latin: A, B, C, a, b, c */
    0x0080, 0x0100, /* Latin-1 Supplement: Ä, Ç, å, ß */
    0x0100, 0x0180, /* Latin Extended-A: Ā č Ō œ */
    0x0180, 0x0250, /* Latin Extended-B: Ǝ Ƹ */
    0x0250, 0x02b0, /* IPA Extensions: ɐ ɔ ɘ ʌ ʍ */
    0x0370, 0x0400, /* Greek and Coptic: Λ α β */
    0x0400, 0x0500, /* Cyrillic: И Я */
    0x0530, 0x0590, /* Armenian: Ո */
    0x1d00, 0x1d80, /* Phonetic Extensions: ᴉ ᵷ */
    0x2000, 0x2070, /* General Punctuation: ‘’ “” */
    0x2100, 0x2150, /* Letterlike Symbols: Ⅎ */
    0x2200, 0x2300, /* Mathematical Operators: √ ∞ ∙ */
    0x2300, 0x2400, /* Miscellaneous Technical: ⌐ ⌂ ⌠ ⌡ */
    0x2500, 0x2580, /* Box Drawing: ═ ║ ╗ ╔ ╩ */
    0x2580, 0x25a0, /* Block Elements: ▛ ▞ ░ ▒ ▓ */
    0, 0
};

int main(int argc, char *argv[])
{
    cucul_canvas_t *cv;
    cucul_buffer_t *buffer;
    unsigned int i, j, x, y, glyphs;

    for(i = 0, glyphs = 0; blocklist[i + 1]; i += 2)
        glyphs += blocklist[i + 1] - blocklist[i];

    /* Create a canvas */
    cv = cucul_create_canvas(WIDTH, (glyphs + WIDTH - 1) / WIDTH);
    cucul_set_attr_ansi(cv, CUCUL_COLOR_BLACK, CUCUL_COLOR_WHITE, 0);

    /* Put all glyphs on the canvas */
    x = y = 0;

    for(i = 0; blocklist[i + 1]; i += 2)
    {
        for(j = blocklist[i]; j < blocklist[i + 1]; j++)
        {
            cucul_putchar(cv, x, y, j);

            if(++x == WIDTH)
            {
                x = 0;
                y++;
            }
        }
    }

    buffer = cucul_export_canvas(cv, "tga");
    fwrite(cucul_get_buffer_data(buffer),
           cucul_get_buffer_size(buffer), 1, stdout);
    cucul_free_buffer(buffer);

    /* Free everything */
    cucul_free_canvas(cv);

    return 0;
}

