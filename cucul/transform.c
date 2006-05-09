/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
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
 *  This function inverts a canvas' colours (black becomes white, red
 *  becomes cyan, etc.) without changing the characters in it.
 *
 *  This function never fails.
 *
 *  \param cv The canvas to invert.
 *  \return This function always returns 0.
 */
int cucul_invert(cucul_canvas_t *cv)
{
    uint32_t *attr = cv->attr;
    unsigned int i;

    for(i = cv->height * cv->width; i--; )
    {
        *attr = *attr ^ 0x000f000f;
        attr++;
    }

    return 0;
}

/** \brief Flip a canvas horizontally.
 *
 *  This function flips a canvas horizontally, choosing characters that
 *  look like the mirrored version wherever possible.
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
        uint32_t *aleft = cv->attr + y * cv->width;
        uint32_t *aright = aleft + cv->width - 1;

        while(cleft < cright)
        {
            uint32_t ch;
            uint32_t attr;

            /* Swap attributes */
            attr = *aright; *aright = *aleft; *aleft = attr;

            /* Swap characters */
            ch = *cright; *cright = flipchar(*cleft); *cleft = flipchar(ch);

            cleft++; cright--; aleft++; aright--;
        }

        if(cleft == cright)
            *cleft = flipchar(*cleft);
    }

    return 0;
}

/** \brief Flip a canvas vertically.
 *
 *  This function flips a canvas vertically, choosing characters that
 *  look like the mirrored version wherever possible.
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
        uint32_t *atop = cv->attr + x;
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
 *  This function applies a 180 degrees transformation to a canvas,
 *  choosing characters that look like the mirrored version wherever
 *  possible.
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
    uint32_t *abegin = cv->attr;
    uint32_t *aend = abegin + cv->width * cv->height - 1;

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

    return 0;
}

static uint32_t flipchar(uint32_t ch)
{
    int i;

    static uint32_t const noflip[] =
    {
         /* ASCII */
         (uint32_t)' ', (uint32_t)'"', (uint32_t)'#', (uint32_t)'\'',
         (uint32_t)'-', (uint32_t)'.', (uint32_t)'*', (uint32_t)'+',
         (uint32_t)':', (uint32_t)'=',
         (uint32_t)'0', (uint32_t)'8', (uint32_t)'A', (uint32_t)'H',
         (uint32_t)'I', (uint32_t)'M', (uint32_t)'O', (uint32_t)'T',
         (uint32_t)'U', (uint32_t)'V', (uint32_t)'W', (uint32_t)'X',
         (uint32_t)'Y', (uint32_t)'^', (uint32_t)'_', (uint32_t)'i',
         (uint32_t)'o', (uint32_t)'v', (uint32_t)'w', (uint32_t)'x',
         (uint32_t)'|',
         /* CP437 */
         0x2591, 0x2592, 0x2593, 0x2588, 0x2584, 0x2580, /* ░ ▒ ▓ █ ▄ ▀ */
         0
    };

    static uint32_t const pairs[] =
    {
         /* ASCII */
         (uint32_t)'(', (uint32_t)')',
         (uint32_t)'/', (uint32_t)'\\',
         (uint32_t)'<', (uint32_t)'>',
         (uint32_t)'[', (uint32_t)']',
         (uint32_t)'b', (uint32_t)'d',
         (uint32_t)'p', (uint32_t)'q',
         (uint32_t)'{', (uint32_t)'}',
         /* ASCII-Unicode */
         (uint32_t)';', 0x204f, /* ; ⁏ */
         (uint32_t)'`', 0x00b4, /* ` ´ */
         (uint32_t)',', 0x02ce, /* , ˎ */
         (uint32_t)'C', 0x03fd, /* C Ͻ */
         (uint32_t)'E', 0x018e, /* E Ǝ */
         (uint32_t)'N', 0x0418, /* N И */
         (uint32_t)'R', 0x042f, /* R Я */
         (uint32_t)'S', 0x01a7, /* S Ƨ */
         (uint32_t)'c', 0x0254, /* c ɔ */
         (uint32_t)'e', 0x0258, /* e ɘ */
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
         (uint32_t)' ', (uint32_t)'(', (uint32_t)')', (uint32_t)'*',
         (uint32_t)'+', (uint32_t)'-',
         (uint32_t)'0', (uint32_t)'3', (uint32_t)'8', (uint32_t)':',
         (uint32_t)'<', (uint32_t)'=', (uint32_t)'>', (uint32_t)'B',
         (uint32_t)'C', (uint32_t)'D', (uint32_t)'E', (uint32_t)'H',
         (uint32_t)'I', (uint32_t)'K', (uint32_t)'O', (uint32_t)'X',
         (uint32_t)'[', (uint32_t)']', (uint32_t)'c', (uint32_t)'o',
         (uint32_t)'{', (uint32_t)'|', (uint32_t)'}',
         /* CP437 */
         0x2591, 0x2592, 0x2593, 0x2588, 0x258c, 0x2590, /* ░ ▒ ▓ █ ▌ ▐ */
         0
    };

    static uint32_t const pairs[] =
    {
         /* ASCII */
         (uint32_t)'/', (uint32_t)'\\',
         (uint32_t)'M', (uint32_t)'W',
         (uint32_t)',', (uint32_t)'`',
         (uint32_t)'b', (uint32_t)'p',
         (uint32_t)'d', (uint32_t)'q',
         (uint32_t)'p', (uint32_t)'q',
         (uint32_t)'f', (uint32_t)'t',
         (uint32_t)'.', (uint32_t)'\'',
         /* ASCII-Unicode */
         (uint32_t)'_', 0x203e, /* _ ‾ */
         (uint32_t)'!', 0x00a1, /* ! ¡ */
         (uint32_t)'L', 0x0413, /* L Г */
         (uint32_t)'N', 0x0418, /* N И */
         (uint32_t)'P', 0x042c, /* P Ь */
         (uint32_t)'R', 0x0281, /* R ʁ */
         (uint32_t)'S', 0x01a7, /* S Ƨ */
         (uint32_t)'U', 0x0548, /* U Ո */
         (uint32_t)'V', 0x039b, /* V Λ */
         (uint32_t)'h', 0x03bc, /* h μ */
         (uint32_t)'i', 0x1d09, /* i ᴉ */
         (uint32_t)'v', 0x028c, /* v ʌ */
         (uint32_t)'w', 0x028d, /* w ʍ */
         (uint32_t)'y', 0x03bb, /* y λ */
         /* Not perfect, but better than nothing */
         (uint32_t)'m', 0x026f, /* m ɯ */
         (uint32_t)'n', (uint32_t)'u',
         /* CP437 */
         0x2584, 0x2580, /* ▄ ▀ */
         0x2596, 0x2598, /* ▖ ▘ */
         0x2597, 0x259d, /* ▗ ▝ */
         0x2599, 0x259b, /* ▙ ▛ */
         0x259f, 0x259c, /* ▟ ▜ */
         0x259a, 0x259e, /* ▚ ▞ */
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
         /* ASCII - FIXME: a lot are missing */
         (uint32_t)' ', (uint32_t)'*', (uint32_t)'+', (uint32_t)'-',
         (uint32_t)'0', (uint32_t)'8', (uint32_t)':', (uint32_t)'=',
         /* Unicode */
         0x2591, 0x2592, 0x2593, 0x2588, 0x259a, 0x259e, /* ░ ▒ ▓ █ ▚ ▞ */
         0
    };

    static uint32_t const pairs[] =
    {
         /* ASCII */
         (uint32_t)'(', (uint32_t)')',
         (uint32_t)'<', (uint32_t)'>',
         (uint32_t)'[', (uint32_t)']',
         (uint32_t)'{', (uint32_t)'}',
         (uint32_t)'.', (uint32_t)'\'',
         (uint32_t)'6', (uint32_t)'9',
         (uint32_t)'M', (uint32_t)'W',
         (uint32_t)'b', (uint32_t)'q',
         (uint32_t)'d', (uint32_t)'p',
         (uint32_t)'n', (uint32_t)'u',
         /* ASCII-Unicode */
         (uint32_t)'_', 0x203e, /* _ ‾ */
         (uint32_t)',', 0x00b4, /* , ´ */
         (uint32_t)'`', 0x02ce, /* ` ˎ */
         (uint32_t)'!', 0x00a1, /* ! ¡ */
         (uint32_t)'?', 0x00bf, /* ? ¿ */
         (uint32_t)'C', 0x03fd, /* C Ͻ */
         (uint32_t)'E', 0x018e, /* E Ǝ */
         (uint32_t)'F', 0x2132, /* F Ⅎ */
         (uint32_t)'U', 0x0548, /* U Ո */
         (uint32_t)'V', 0x039b, /* V Λ */
         (uint32_t)'a', 0x0250, /* a ɐ */
         (uint32_t)'c', 0x0254, /* c ɔ */
         (uint32_t)'e', 0x0259, /* e ə */
         (uint32_t)'f', 0x025f, /* f ɟ */
         (uint32_t)'g', 0x1d77, /* g ᵷ */
         (uint32_t)'h', 0x0265, /* h ɥ */
         (uint32_t)'i', 0x1d09, /* i ᴉ */
         (uint32_t)'k', 0x029e, /* k ʞ */
         (uint32_t)'m', 0x026f, /* m ɯ */
         (uint32_t)'r', 0x0279, /* r ɹ */
         (uint32_t)'t', 0x0287, /* t ʇ */
         (uint32_t)'v', 0x028c, /* v ʌ */
         (uint32_t)'w', 0x028d, /* w ʍ */
         (uint32_t)'y', 0x028e, /* y ʎ */
         /* CP437 */
         0x258c, 0x2590, /* ▌ ▐ */
         0x2584, 0x2580, /* ▄ ▀ */
         0x2596, 0x259d, /* ▖ ▝ */
         0x2597, 0x2598, /* ▗ ▘ */
         0x2599, 0x259c, /* ▙ ▜ */
         0x259f, 0x259b, /* ▟ ▛ */
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

