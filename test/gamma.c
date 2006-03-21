/*
 *  gamma         libcucul gamma test program
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

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#else
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#endif

#include "cucul.h"
#include "caca.h"

uint32_t buffer[256];

int main(void)
{
    cucul_t *qq;
    caca_t *kk;

    struct cucul_bitmap *left, *right;
    int x;

    qq = cucul_init();
    kk = caca_attach(qq);

    for(x = 0; x < 256; x++)
        buffer[x] = (x << 16) | (x << 8) | (x<< 0);

    left = cucul_create_bitmap(qq, 32, 256, 1, 4 * 256,
                               0x00ff0000, 0x0000ff00, 0x000000ff, 0x0);
    right = cucul_create_bitmap(qq, 32, 256, 1, 4 * 256,
                                0x00ff0000, 0x0000ff00, 0x000000ff, 0x0);
    cucul_set_bitmap_gamma(right, 0.5);

    cucul_draw_bitmap(qq, 0, 0,
                      cucul_get_width(qq) - 1, cucul_get_height(qq) / 2 - 1,
                      right, buffer);
    cucul_draw_bitmap(qq, 0, cucul_get_height(qq) / 2,
                      cucul_get_width(qq) - 1, cucul_get_height(qq) - 1,
                      left, buffer);
    cucul_free_bitmap(left);
    cucul_free_bitmap(right);

    caca_display(kk);

    while(!caca_get_event(kk, CACA_EVENT_KEY_PRESS));

    caca_detach(kk);
    cucul_end(qq);

    return 0;
}

