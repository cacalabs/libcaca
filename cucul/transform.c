/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software. It commes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *  This file contains horizontal and vertical flipping routines.
 */

#include "config.h"
#include "common.h"

#if !defined(__KERNEL__)
#endif

#include "cucul.h"
#include "cucul_internals.h"

static uint32_t flipchar(uint32_t ch);
static uint32_t flopchar(uint32_t ch);
static uint32_t rotatechar(uint32_t ch);

/** \brief Invert a canvas' colours.
 *
 *  Invert a canvas' colours (black becomes white, red becomes cyan, etc.)
 *  without changing the characters in it.
 *
 *  This function never fails.
 *
 *  \param cv The canvas to invert.
 *  \return This function always returns 0.
 */
int cucul_invert(cucul_canvas_t *cv)
{
    uint32_t *attrs = cv->attrs;
    unsigned int i;

    for(i = cv->height * cv->width; i--; )
    {
        *attrs = *attrs ^ 0x000f000f;
        attrs++;
    }

    return 0;
}

/** \brief Flip a canvas horizontally.
 *
 *  Flip a canvas horizontally, choosing characters that look like the
 *  mirrored version wherever possible. Some characters will stay
 *  unchanged by the process, but the operation is guaranteed to be
 *  involutive: performing it again gives back the original canvas.
 *
 *  This function never fails.
 *
 *  \param cv The canvas to flip.
 *  \return This function always returns 0.
 */
int cucul_flip(cucul_canvas_t *cv)
{
    unsigned int y;

    for(y = 0; y < cv->height; y++)
    {
        uint32_t *cleft = cv->chars + y * cv->width;
        uint32_t *cright = cleft + cv->width - 1;
        uint32_t *aleft = cv->attrs + y * cv->width;
        uint32_t *aright = aleft + cv->width - 1;

        while(cleft < cright)
        {
            uint32_t ch;
            uint32_t attr;

            /* Swap attributes */
            attr = *aright;
            *aright-- = *aleft;
            *aleft++ = attr;

            /* Swap characters */
            ch = *cright;
            *cright-- = flipchar(*cleft);
            *cleft++ = flipchar(ch);
        }

        if(cleft == cright)
            *cleft = flipchar(*cleft);

        /* Fix fullwidth characters. Could it be done in one loop? */
        cleft = cv->chars + y * cv->width;
        cright = cleft + cv->width - 1;
        for( ; cleft < cright; cleft++)
        {
            if(cleft[0] == CUCUL_MAGIC_FULLWIDTH)
            {
                cleft[0] = cleft[1];
                cleft[1] = CUCUL_MAGIC_FULLWIDTH;
                cleft++;
            }
        }
    }

    return 0;
}

/** \brief Flip a canvas vertically.
 *
 *  Flip a canvas vertically, choosing characters that look like the
 *  mirrored version wherever possible. Some characters will stay
 *  unchanged by the process, but the operation is guaranteed to be
 *  involutive: performing it again gives back the original canvas.
 *
 *  This function never fails.
 *
 *  \param cv The canvas to flop.
 *  \return This function always returns 0.
 */
int cucul_flop(cucul_canvas_t *cv)
{
    unsigned int x;

    for(x = 0; x < cv->width; x++)
    {
        uint32_t *ctop = cv->chars + x;
        uint32_t *cbottom = ctop + cv->width * (cv->height - 1);
        uint32_t *atop = cv->attrs + x;
        uint32_t *abottom = atop + cv->width * (cv->height - 1);

        while(ctop < cbottom)
        {
            uint32_t ch;
            uint32_t attr;

            /* Swap attributes */
            attr = *abottom; *abottom = *atop; *atop = attr;

            /* Swap characters */
            ch = *cbottom; *cbottom = flopchar(*ctop); *ctop = flopchar(ch);

            ctop += cv->width; cbottom -= cv->width;
            atop += cv->width; abottom -= cv->width;
        }

        if(ctop == cbottom)
            *ctop = flopchar(*ctop);
    }

    return 0;
}

/** \brief Rotate a canvas.
 *
 *  Apply a 180-degree transformation to a canvas, choosing characters
 *  that look like the upside-down version wherever possible. Some
 *  characters will stay unchanged by the process, but the operation is
 *  guaranteed to be involutive: performing it again gives back the
 *  original canvas.
 *
 *  This function never fails.
 *
 *  \param cv The canvas to rotate.
 *  \return This function always returns 0.
 */
int cucul_rotate(cucul_canvas_t *cv)
{
    uint32_t *cbegin = cv->chars;
    uint32_t *cend = cbegin + cv->width * cv->height - 1;
    uint32_t *abegin = cv->attrs;
    uint32_t *aend = abegin + cv->width * cv->height - 1;
    unsigned int y;

    while(cbegin < cend)
    {
        uint32_t ch;
        uint32_t attr;

        /* Swap attributes */
        attr = *aend; *aend = *abegin; *abegin = attr;

        /* Swap characters */
        ch = *cend; *cend = rotatechar(*cbegin); *cbegin = rotatechar(ch);

        cbegin++; cend--; abegin++; aend--;
    }

    if(cbegin == cend)
        *cbegin = rotatechar(*cbegin);

    /* Fix fullwidth characters. Could it be done in one loop? */
    for(y = 0; y < cv->height; y++)
    {
        cbegin = cv->chars + y * cv->width;
        cend = cbegin + cv->width - 1;
        for( ; cbegin < cend; cbegin++)
        {
            if(cbegin[0] == CUCUL_MAGIC_FULLWIDTH)
            {
                cbegin[0] = cbegin[1];
                cbegin[1] = CUCUL_MAGIC_FULLWIDTH;
                cbegin++;
            }
        }
    }

    return 0;
}

/* FIXME: as the lookup tables grow bigger, use a log(n) lookup instead
 * of linear lookup. */
static uint32_t flipchar(uint32_t ch)
{
    int i;

    static uint32_t const noflip[] =
    {
         /* ASCII */
         ' ', '"', '#', '\'', '-', '.', '*', '+', ':', '=', '0', '8',
         'A', 'H', 'I', 'M', 'O', 'T', 'U', 'V', 'W', 'X', 'Y', '^',
         '_', 'i', 'o', 'v', 'w', 'x', '|',
         /* CP437 and box drawing */
         0x2591, 0x2592, 0x2593, 0x2588, 0x2584, 0x2580, /* ░ ▒ ▓ █ ▄ ▀ */
         0x2500, 0x2501, 0x2503, 0x2503, 0x253c, 0x254b, /* ─ ━ │ ┃ ┼ ╋ */
         0x252c, 0x2534, 0x2533, 0x253b, 0x2566, 0x2569, /* ┬ ┴ ┳ ┻ ╦ ╩ */
         0x2550, 0x2551, 0x256c, /* ═ ║ ╬ */
         0x2575, 0x2577, 0x2579, 0x257b, /* ╵ ╷ ╹ ╻ */
         0
    };

    static uint32_t const pairs[] =
    {
         /* ASCII */
         '(', ')',
         '/', '\\',
         '<', '>',
         '[', ']',
         'b', 'd',
         'p', 'q',
         '{', '}',
         /* ASCII-Unicode */
         ';', 0x204f, /* ; ⁏ */
         '`', 0x00b4, /* ` ´ */
         ',', 0x02ce, /* , ˎ */
         'C', 0x03fd, /* C Ͻ */
         'E', 0x018e, /* E Ǝ */
         'N', 0x0418, /* N И */
         'R', 0x042f, /* R Я */
         'S', 0x01a7, /* S Ƨ */
         'c', 0x0254, /* c ɔ */
         'e', 0x0258, /* e ɘ */
         /* CP437 */
         0x258c, 0x2590, /* ▌ ▐ */
         0x2596, 0x2597, /* ▖ ▗ */
         0x2598, 0x259d, /* ▘ ▝ */
         0x2599, 0x259f, /* ▙ ▟ */
         0x259a, 0x259e, /* ▚ ▞ */
         0x259b, 0x259c, /* ▛ ▜ */
         0x25ba, 0x25c4, /* ► ◄ */
         0x2192, 0x2190, /* → ← */
         0x2310, 0xac,   /* ⌐ ¬ */
         /* Box drawing */
         0x250c, 0x2510, /* ┌ ┐ */
         0x2514, 0x2518, /* └ ┘ */
         0x251c, 0x2524, /* ├ ┤ */
         0x250f, 0x2513, /* ┏ ┓ */
         0x2517, 0x251b, /* ┗ ┛ */
         0x2523, 0x252b, /* ┣ ┫ */
         0x2554, 0x2557, /* ╔ ╗ */
         0x255a, 0x255d, /* ╚ ╝ */
         0x2560, 0x2563, /* ╠ ╣ */
         0x2574, 0x2576, /* ╴ ╶ */
         0x2578, 0x257a, /* ╸ ╺ */
         0
    };

    for(i = 0; noflip[i]; i++)
        if(ch == noflip[i])
            return ch;

    for(i = 0; pairs[i]; i++)
        if(ch == pairs[i])
            return pairs[i ^ 1];

    return ch;
}

static uint32_t flopchar(uint32_t ch)
{
    int i;

    static uint32_t const noflop[] =
    {
         /* ASCII */
         ' ', '(', ')', '*', '+', '-', '0', '3', '8', ':', '<', '=',
         '>', 'B', 'C', 'D', 'E', 'H', 'I', 'K', 'O', 'X', '[', ']',
         'c', 'o', '{', '|', '}',
         /* CP437 and box drawing */
         0x2591, 0x2592, 0x2593, 0x2588, 0x258c, 0x2590, /* ░ ▒ ▓ █ ▌ ▐ */
         0x2500, 0x2501, 0x2503, 0x2503, 0x253c, 0x254b, /* ─ ━ │ ┃ ┼ ╋ */
         0x251c, 0x2524, 0x2523, 0x252b, 0x2560, 0x2563, /* ├ ┤ ┣ ┫ ╠ ╣ */
         0x2550, 0x2551, 0x256c, /* ═ ║ ╬ */
         0x2574, 0x2576, 0x2578, 0x257a, /* ╴ ╶ ╸ ╺ */
         0
    };

    static uint32_t const pairs[] =
    {
         /* ASCII */
         '/', '\\',
         'M', 'W',
         ',', '`',
         'b', 'p',
         'd', 'q',
         'p', 'q',
         'f', 't',
         '.', '\'',
         /* ASCII-Unicode */
         '_', 0x203e, /* _ ‾ */
         '!', 0x00a1, /* ! ¡ */
         'L', 0x0413, /* L Г */
         'N', 0x0418, /* N И */
         'P', 0x042c, /* P Ь */
         'R', 0x0281, /* R ʁ */
         'S', 0x01a7, /* S Ƨ */
         'U', 0x0548, /* U Ո */
         'V', 0x039b, /* V Λ */
         'h', 0x03bc, /* h μ */
         'i', 0x1d09, /* i ᴉ */
         'v', 0x028c, /* v ʌ */
         'w', 0x028d, /* w ʍ */
         'y', 0x03bb, /* y λ */
         /* Not perfect, but better than nothing */
         '"', 0x201e, /* " „ */
         'm', 0x026f, /* m ɯ */
         'n', 'u',
         /* CP437 */
         0x2584, 0x2580, /* ▄ ▀ */
         0x2596, 0x2598, /* ▖ ▘ */
         0x2597, 0x259d, /* ▗ ▝ */
         0x2599, 0x259b, /* ▙ ▛ */
         0x259f, 0x259c, /* ▟ ▜ */
         0x259a, 0x259e, /* ▚ ▞ */
         /* Box drawing */
         0x250c, 0x2514, /* ┌ └ */
         0x2510, 0x2518, /* ┐ ┘ */
         0x252c, 0x2534, /* ┬ ┴ */
         0x250f, 0x2517, /* ┏ ┗ */
         0x2513, 0x251b, /* ┓ ┛ */
         0x2533, 0x253b, /* ┳ ┻ */
         0x2554, 0x255a, /* ╔ ╚ */
         0x2557, 0x255d, /* ╗ ╝ */
         0x2566, 0x2569, /* ╦ ╩ */
         0x2575, 0x2577, /* ╵ ╷ */
         0x2579, 0x257b, /* ╹ ╻ */
         0
    };

    for(i = 0; noflop[i]; i++)
        if(ch == noflop[i])
            return ch;

    for(i = 0; pairs[i]; i++)
        if(ch == pairs[i])
            return pairs[i ^ 1];

    return ch;
}

static uint32_t rotatechar(uint32_t ch)
{
    int i;

    static uint32_t const norotate[] =
    {
         /* ASCII */
         ' ', '*', '+', '-', '/', '0', '8', ':', '=', 'H', 'I', 'N',
         'O', 'S', 'X', 'Z', '\\', 'l', 'o', 's', 'x', 'z', '|',
         /* Unicode */
         0x2591, 0x2592, 0x2593, 0x2588, 0x259a, 0x259e, /* ░ ▒ ▓ █ ▚ ▞ */
         0x2500, 0x2501, 0x2503, 0x2503, 0x253c, 0x254b, /* ─ ━ │ ┃ ┼ ╋ */
         0x2550, 0x2551, 0x256c, /* ═ ║ ╬ */
         0
    };

    static uint32_t const pairs[] =
    {
         /* ASCII */
         '(', ')',
         '<', '>',
         '[', ']',
         '{', '}',
         '.', '\'',
         '6', '9',
         'M', 'W',
         'b', 'q',
         'd', 'p',
         'n', 'u',
         /* ASCII-Unicode */
         '_', 0x203e, /* _ ‾ */
         ',', 0x00b4, /* , ´ */
         '`', 0x02ce, /* ` ˎ */
         '&', 0x214b, /* & ⅋ */
         '!', 0x00a1, /* ! ¡ */
         '?', 0x00bf, /* ? ¿ */
         'C', 0x03fd, /* C Ͻ */
         'E', 0x018e, /* E Ǝ */
         'F', 0x2132, /* F Ⅎ */
         'U', 0x0548, /* U Ո */
         'V', 0x039b, /* V Λ */
         'a', 0x0250, /* a ɐ */
         'c', 0x0254, /* c ɔ */
         'e', 0x0259, /* e ə */
         'f', 0x025f, /* f ɟ */
         'g', 0x1d77, /* g ᵷ */
         'h', 0x0265, /* h ɥ */
         'i', 0x1d09, /* i ᴉ */
         'k', 0x029e, /* k ʞ */
         'm', 0x026f, /* m ɯ */
         'r', 0x0279, /* r ɹ */
         't', 0x0287, /* t ʇ */
         'v', 0x028c, /* v ʌ */
         'w', 0x028d, /* w ʍ */
         'y', 0x028e, /* y ʎ */
         /* Not perfect, but better than nothing */
         '"', 0x201e, /* " „ */
         /* CP437 */
         0x258c, 0x2590, /* ▌ ▐ */
         0x2584, 0x2580, /* ▄ ▀ */
         0x2596, 0x259d, /* ▖ ▝ */
         0x2597, 0x2598, /* ▗ ▘ */
         0x2599, 0x259c, /* ▙ ▜ */
         0x259f, 0x259b, /* ▟ ▛ */
         /* Box drawing */
         0x250c, 0x2518, /* ┌ ┘ */
         0x2510, 0x2514, /* ┐ └ */
         0x251c, 0x2524, /* ├ ┤ */
         0x252c, 0x2534, /* ┬ ┴ */
         0x250f, 0x251b, /* ┏ ┛ */
         0x2513, 0x2517, /* ┓ ┗ */
         0x2523, 0x252b, /* ┣ ┫ */
         0x2533, 0x253b, /* ┳ ┻ */
         0x2554, 0x255d, /* ╔ ╝ */
         0x2557, 0x255a, /* ╗ ╚ */
         0x2560, 0x2563, /* ╠ ╣ */
         0x2566, 0x2569, /* ╦ ╩ */
         0x2574, 0x2576, /* ╴ ╶ */
         0x2575, 0x2577, /* ╵ ╷ */
         0x2578, 0x257a, /* ╸ ╺ */
         0x2579, 0x257b, /* ╹ ╻ */
         0
    };

    for(i = 0; norotate[i]; i++)
        if(ch == norotate[i])
            return ch;

    for(i = 0; pairs[i]; i++)
        if(ch == pairs[i])
            return pairs[i ^ 1];

    return ch;
}

