/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains the libcaca VGA input and output driver
 */

#include "config.h"

#if defined(USE_VGA)

#include "caca.h"
#include "caca_internals.h"

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

static int vga_init_graphics(caca_display_t *dp)
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

    caca_add_dirty_rect(dp->cv, 0, 0, dp->cv->width, dp->cv->height);
    dp->resize.allow = 1;
    caca_set_canvas_size(dp->cv, 80, 25); /* We don't have much choice */
    dp->resize.allow = 0;

    return 0;
}

static int vga_end_graphics(caca_display_t *dp)
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

static int vga_set_display_title(caca_display_t *dp, char const *title)
{
    /* Unsupported, of course. */
    return -1;
}

static int vga_get_display_width(caca_display_t const *dp)
{
    /* Fallback to a 320x200 screen */
    return 320;
}

static int vga_get_display_height(caca_display_t const *dp)
{
    /* Fallback to a 320x200 screen */
    return 200;
}

static void vga_display(caca_display_t *dp)
{
    int x, y, i;

    for(i = 0; i < caca_get_dirty_rect_count(dp->cv); i++)
    {
        char *screen = (char *)(intptr_t)0x000b8000;
        uint32_t const *cvchars, *cvattrs;
        int dx, dy, dw, dh;

        caca_get_dirty_rect(dp->cv, i, &dx, &dy, &dw, &dh);

        cvchars = caca_get_canvas_chars(dp->cv) + dx + dy * dp->cv->width;
        cvattrs = caca_get_canvas_attrs(dp->cv) + dx + dy * dp->cv->width;

        screen += dy * dp->cv->width + dx;

        for(y = dy; y < dy + dh; y++)
        {
            for(x = dx; x < dx + dw; x++)
            {
                char ch = caca_utf32_to_cp437(*cvchars++);
                if(x < dx + dw - 1 && *cvchars == CACA_MAGIC_FULLWIDTH)
                {
                    *screen++ = '[';
                    *screen++ = caca_attr_to_ansi(*cvattrs++);
                    ch = ']';
                    cvchars++;
                    x++;
                }
                *screen++ = ch;
                *screen++ = caca_attr_to_ansi(*cvattrs++);
            }

            cvchars += dp->cv->width - dw;
            cvattrs += dp->cv->width - dw;
            screen += 2 * (dp->cv->width - dw);
        }
    }
}

static void vga_handle_resize(caca_display_t *dp)
{
    /* We know nothing about our window */
    dp->resize.w = caca_get_canvas_width(dp->cv);
    dp->resize.h = caca_get_canvas_height(dp->cv);
}

static int vga_get_event(caca_display_t *dp, caca_privevent_t *ev)
{
    /* FIXME */
    ev->type = CACA_EVENT_NONE;
    return 0;
}

/*
 * Driver initialisation
 */

int vga_install(caca_display_t *dp)
{
    dp->drv.id = CACA_DRIVER_VGA;
    dp->drv.driver = "vga";

    dp->drv.init_graphics = vga_init_graphics;
    dp->drv.end_graphics = vga_end_graphics;
    dp->drv.set_display_title = vga_set_display_title;
    dp->drv.get_display_width = vga_get_display_width;
    dp->drv.get_display_height = vga_get_display_height;
    dp->drv.display = vga_display;
    dp->drv.handle_resize = vga_handle_resize;
    dp->drv.get_event = vga_get_event;
    dp->drv.set_mouse = NULL;
    dp->drv.set_cursor = NULL;

    return 0;
}

#endif /* USE_VGA */

