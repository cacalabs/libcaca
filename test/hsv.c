/*
 *  hsv           libcaca HSV rendering test program
 *  Copyright (c) 2003 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA
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

    while((caca_get_event() & 0xff000000) != CACA_EVENT_KEY_PRESS);

    caca_end();

    return 0;
}

