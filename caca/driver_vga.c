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

/* Address of the VGA screen */
#define VGA_SCREEN ((char *)(intptr_t)0x000b8000)

static uint8_t const vga_colors[][4] =
{
    { 0,      0x20, 0x20, 0x20 },
    { 1,      0x20, 0x20, 0x7f },
    { 2,      0x20, 0x7f, 0x20 },
    { 3,      0x20, 0x7f, 0x7f },
    { 4,      0x7f, 0x20, 0x20 },
    { 5,      0x7f, 0x20, 0x7f },
    { 0x14,   0x7f, 0x7f, 0x20 },
    { 7,      0x7f, 0x7f, 0x7f },

    { 0x38,   0x40, 0x40, 0x40 },
    { 0x39,   0x40, 0x40, 0xff },
    { 0x3a,   0x40, 0xff, 0x40 },
    { 0x3b,   0x40, 0xff, 0xff },
    { 0x3c,   0xff, 0x40, 0x40 },
    { 0x3d,   0xff, 0x40, 0xff },
    { 0x3e,   0xff, 0xff, 0x40 },
    { 0x3f,   0xff, 0xff, 0xff }
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
    /* Fallback to a 6x10 font */
    return kk->qq->width * 6;
}

static unsigned int vga_get_window_height(caca_t *kk)
{
    /* Fallback to a 6x10 font */
    return kk->qq->height * 10;
}

static void vga_display(caca_t *kk)
{
    char *screen = (char *)(intptr_t)0x000b8000;
    uint8_t *attr = kk->qq->attr;
    uint32_t *chars = kk->qq->chars;
    int n;

    for(n = kk->qq->height * kk->qq->width; n--; )
    {
        uint32_t c = *chars++;

        if(c > 0x00000020 && c < 0x00000080)
            *screen++ = (char)c;
        else
            *screen++ = ' ';

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

