/*
 *  libcaca       ASCII-Art library
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file driver_vga.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief VGA driver
 *
 *  This file contains the libcaca VGA input and output driver
 */

#include "config.h"

#if defined(USE_VGA)

#include "caca.h"
#include "caca_internals.h"
#include "cucul.h"
#include "cucul_internals.h"

static uint8_t vga_utf32_to_cp437(uint32_t);

/* Address of the VGA screen */
#define VGA_SCREEN ((char *)(intptr_t)0x000b8000)

static uint8_t const vga_colors[][4] =
{
    /* Colour values range from 0x00 to 0x3f */
    { 0,      0x00, 0x00, 0x00 },
    { 1,      0x00, 0x00, 0x1f },
    { 2,      0x00, 0x1f, 0x00 },
    { 3,      0x00, 0x1f, 0x1f },
    { 4,      0x1f, 0x00, 0x00 },
    { 5,      0x1f, 0x00, 0x1f },
    { 0x14,   0x1f, 0x1f, 0x00 },
    { 7,      0x1f, 0x1f, 0x1f },

    { 0x38,   0x0f, 0x0f, 0x0f },
    { 0x39,   0x0f, 0x0f, 0x3f },
    { 0x3a,   0x0f, 0x3f, 0x0f },
    { 0x3b,   0x0f, 0x3f, 0x3f },
    { 0x3c,   0x3f, 0x0f, 0x0f },
    { 0x3d,   0x3f, 0x0f, 0x3f },
    { 0x3e,   0x3f, 0x3f, 0x0f },
    { 0x3f,   0x3f, 0x3f, 0x3f },
};

static int vga_init_graphics(caca_t *kk)
{
    int i;
    uint8_t tmp;

    /* Blank screen */
    memset(VGA_SCREEN, 0, 80 * 25 * 2);

    /* Fill VGA palette */
    for(i = 0; i < 16; i++)
    {
        outb(vga_colors[i][0], 0x3c8);
        outb(vga_colors[i][1], 0x3c9);
        outb(vga_colors[i][2], 0x3c9);
        outb(vga_colors[i][3], 0x3c9);
    }

    /* Hide cursor */
    outb(0x0a, 0x3d4);
    tmp = inb(0x3d5);
    tmp |= 0x20;
    outb(0x0a, 0x3d4);
    outb(tmp, 0x3d5);

    /* We don't have much choice */
    _cucul_set_size(kk->qq, 80, 25);

    return 0;
}

static int vga_end_graphics(caca_t *kk)
{
    uint8_t tmp;

    /* Show cursor again */
    outb(0x0a, 0x3d4);
    tmp = inb(0x3d5);
    tmp &= 0xdf;
    outb(0x0a, 0x3d4);
    outb(tmp, 0x3d5);

    return 0;
}

static int vga_set_window_title(caca_t *kk, char const *title)
{
    /* Unsupported, of course. */
    return 0;
}

static unsigned int vga_get_window_width(caca_t *kk)
{
    /* Fallback to a 320x200 screen */
    return 320;
}

static unsigned int vga_get_window_height(caca_t *kk)
{
    /* Fallback to a 320x200 screen */
    return 200;
}

static void vga_display(caca_t *kk)
{
    char *screen = (char *)(intptr_t)0x000b8000;
    uint8_t *attr = kk->qq->attr;
    uint32_t *chars = kk->qq->chars;
    int n;

    for(n = kk->qq->height * kk->qq->width; n--; )
    {
        *screen++ = vga_utf32_to_cp437(*chars++);
        *screen++ = *attr++;
    }
}

static void vga_handle_resize(caca_t *kk)
{
    /* We know nothing about our window */
    kk->resize.w = kk->qq->width;
    kk->resize.h = kk->qq->height;
}

static unsigned int vga_get_event(caca_t *kk)
{
    /* FIXME */
    return CACA_EVENT_NONE;
}

/*
 * XXX: following functions are local
 */

static uint8_t vga_utf32_to_cp437(uint32_t c)
{
    static uint32_t const lookup1[] =
    {
        /* 0x01 - 0x0f: ☺ ☻ ♥ ♦ ♣ ♠ • ◘ ○ ◙ ♂ ♀ ♪ ♫ ☼ */
                0x263a, 0x263b, 0x2665, 0x2666, 0x2663, 0x2660, 0x2022,
        0x25d8, 0x25cb, 0x25d9, 0x2642, 0x2640, 0x266a, 0x266b, 0x263c,
        /* 0x10 - 0x1f: ► ◄ ↕ ‼ ¶ § ▬ ↨ ↑ ↓ → ← ∟ ↔ ▲ ▼ */
        0x25ba, 0x25c4, 0x2195, 0x203c, 0xb6, 0xa7, 0x25ac, 0x21a8,
        0x2191, 0x2193, 0x2192, 0x2190, 0x221f, 0x2194, 0x25b2, 0x25bc
    };

    static uint32_t const lookup2[] =
    {
        /* 0x7f: ⌂ */
        0x2302,
        /* 0x80 - 0x8f: Ç ü é â ä à å ç ê ë è ï î ì Ä Å */
        0xc7, 0xfc, 0xe9, 0xe2, 0xe4, 0xe0, 0xe5, 0xe7,
        0xea, 0xeb, 0xe8, 0xef, 0xee, 0xec, 0xc4, 0xc5,
        /* 0x90 - 0x9f: É æ Æ ô ö ò û ù ÿ Ö Ü ¢ £ ¥ ₧ ƒ */
        0xc9, 0xe6, 0xc6, 0xf4, 0xf6, 0xf2, 0xfb, 0xf9,
        0xff, 0xd6, 0xdc, 0xa2, 0xa3, 0xa5, 0x20a7, 0x192,
        /* 0xa0 - 0xaf: á í ó ú ñ Ñ ª º ¿ ⌐ ¬ ½ ¼ ¡ « » */
        0xe1, 0xed, 0xf3, 0xfa, 0xf1, 0xd1, 0xaa, 0xba,
        0xbf, 0x2310, 0xac, 0xbd, 0xbc, 0xa1, 0xab, 0xbb,
        /* 0xb0 - 0xbf: ░ ▒ ▓ │ ┤ ╡ ╢ ╖ ╕ ╣ ║ ╗ ╝ ╜ ╛ ┐ */
        0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
        0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510,
        /* 0xc0 - 0xcf: └ ┴ ┬ ├ ─ ┼ ╞ ╟ ╚ ╔ ╩ ╦ ╠ ═ ╬ ╧ */
        0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f,
        0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567,
        /* 0xd0 - 0xdf: ╨ ╤ ╥ ╙ ╘ ╒ ╓ ╫ ╪ ┘ ┌ █ ▄ ▌ ▐ ▀ */
        0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b,
        0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580,
        /* 0xe0 - 0xef: α ß Γ π Σ σ µ τ Φ Θ Ω δ ∞ φ ε ∩ */
        0x3b1, 0xdf, 0x393, 0x3c0, 0x3a3, 0x3c3, 0xb5, 0x3c4,
        0x3a6, 0x398, 0x3a9, 0x3b4, 0x221e, 0x3c6, 0x3b5, 0x2229,
        /* 0xf0 - 0xff: ≡ ± ≥ ≤ ⌠ ⌡ ÷ ≈ ° ∙ · √ ⁿ ² ■ <nbsp> */
        0x2261, 0xb1, 0x2265, 0x2264, 0x2320, 0x2321, 0xf7, 0x2248,
        0xb0, 0x2219, 0xb7, 0x221a, 0x207f, 0xb2, 0x25a0, 0xa0
    };

    unsigned int i;

    if(c < 0x00000020)
        return '?';

    if(c < 0x00000080)
        return c;

    for(i = 0; i < sizeof(lookup1) / sizeof(*lookup1); i++)
        if(lookup1[i] == c)
            return 0x01 + i;

    for(i = 0; i < sizeof(lookup2) / sizeof(*lookup2); i++)
        if(lookup2[i] == c)
            return 0x7f + i;

    return '?';
}

/*
 * Driver initialisation
 */

void vga_init_driver(caca_t *kk)
{
    kk->drv.driver = CACA_DRIVER_VGA;

    kk->drv.init_graphics = vga_init_graphics;
    kk->drv.end_graphics = vga_end_graphics;
    kk->drv.set_window_title = vga_set_window_title;
    kk->drv.get_window_width = vga_get_window_width;
    kk->drv.get_window_height = vga_get_window_height;
    kk->drv.display = vga_display;
    kk->drv.handle_resize = vga_handle_resize;
    kk->drv.get_event = vga_get_event;
}

#endif /* USE_VGA */

