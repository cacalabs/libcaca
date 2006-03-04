/*
 *  hsv           libcaca HSV rendering test program
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

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#else
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#endif

#include "caca.h"

uint32_t buffer[256*256];

int main(void)
{
    struct caca_bitmap *bitmap;
    int x, y;

    caca_init();

    for(y = 0; y < 256; y++)
        for(x = 0; x < 256; x++)
    {
        buffer[y * 256 + x] = ((y * x / 256) << 16) | ((y * x / 256) << 8) | (x<< 0);
    }

    bitmap = caca_create_bitmap(32, 256, 256, 4 * 256,
                                0x00ff0000, 0x0000ff00, 0x000000ff, 0x0);
    caca_draw_bitmap(0, 0, caca_get_width() - 1, caca_get_height() - 1,
                     bitmap, buffer);
    caca_free_bitmap(bitmap);

    caca_refresh();

    while(!caca_get_event(CACA_EVENT_KEY_PRESS));

    caca_end();

    return 0;
}

