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

/** \file driver_conio.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief DOS/conio.h driver
 *
 *  This file contains the libcaca DOS/conio.h input and output driver
 */

#include "config.h"

#if defined(USE_CONIO)

#include <dos.h>
#include <conio.h>
#if defined(SCREENUPDATE_IN_PC_H)
#   include <pc.h>
#endif

#include <stdlib.h>

#include "caca.h"
#include "caca_internals.h"
#include "cucul.h"
#include "cucul_internals.h"

static uint8_t conio_utf32_to_cp437(uint32_t);

struct driver_private
{
    struct text_info ti;
    char *screen;
};

static int conio_init_graphics(caca_t *kk)
{
    kk->drv.p = malloc(sizeof(struct driver_private));

    _wscroll = 0;
    _setcursortype(_NOCURSOR);
    clrscr();

    gettextinfo(&kk->drv.p->ti);
    kk->drv.p->screen = malloc(2 * kk->drv.p->ti.screenwidth
                                 * kk->drv.p->ti.screenheight * sizeof(char));
    if(kk->drv.p->screen == NULL)
        return -1;
#   if defined(SCREENUPDATE_IN_PC_H)
    ScreenRetrieve(kk->drv.p->screen);
#   else
    /* FIXME */
#   endif
    _cucul_set_size(kk->qq, kk->drv.p->ti.screenwidth,
                            kk->drv.p->ti.screenheight);
    return 0;
}

static int conio_end_graphics(caca_t *kk)
{
    _wscroll = 1;
    textcolor((enum COLORS)WHITE);
    textbackground((enum COLORS)BLACK);
    gotoxy(kk->qq->width, kk->qq->height);
    cputs("\r\n");
    _setcursortype(_NORMALCURSOR);

    free(kk->drv.p->screen);
    free(kk->drv.p);

    return 0;
}

static int conio_set_window_title(caca_t *kk, char const *title)
{
    return 0;
}

static unsigned int conio_get_window_width(caca_t *kk)
{
    /* Fallback to a 6x10 font */
    return kk->qq->width * 6;
}

static unsigned int conio_get_window_height(caca_t *kk)
{
    /* Fallback to a 6x10 font */
    return kk->qq->height * 10;
}

static void conio_display(caca_t *kk)
{
    char *screen = kk->drv.p->screen;
    uint8_t *attr = kk->qq->attr;
    uint32_t *chars = kk->qq->chars;
    int n;

    for(n = kk->qq->height * kk->qq->width; n--; )
    {
        *screen++ = conio_utf32_to_cp437(*chars++);
        *screen++ = *attr++;
    }
#   if defined(SCREENUPDATE_IN_PC_H)
    ScreenUpdate(kk->drv.p->screen);
#   else
    /* FIXME */
#   endif
}

static void conio_handle_resize(caca_t *kk)
{
    /* We know nothing about our window */
    kk->resize.w = kk->qq->width;
    kk->resize.h = kk->qq->height;
}

static unsigned int conio_get_event(caca_t *kk)
{
    unsigned int event;

    if(!_conio_kbhit())
        return CACA_EVENT_NONE;

    event = getch();
    _push_event(kk, CACA_EVENT_KEY_RELEASE | event);
    return CACA_EVENT_KEY_PRESS | event;
}

/*
 * XXX: following functions are local
 */

static uint8_t conio_utf32_to_cp437(uint32_t c)
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

void conio_init_driver(caca_t *kk)
{
    kk->drv.driver = CACA_DRIVER_CONIO;

    kk->drv.init_graphics = conio_init_graphics;
    kk->drv.end_graphics = conio_end_graphics;
    kk->drv.set_window_title = conio_set_window_title;
    kk->drv.get_window_width = conio_get_window_width;
    kk->drv.get_window_height = conio_get_window_height;
    kk->drv.display = conio_display;
    kk->drv.handle_resize = conio_handle_resize;
    kk->drv.get_event = conio_get_event;
}

#endif /* USE_CONIO */

