/*
 *  event         event lister for libcaca
 *  Copyright (c) 2004 Sam Hocevar <sam@zoy.org>
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

#include <stdio.h>
#include <string.h>

#include "caca.h"

static void print_event(int, int, unsigned int);

int main(int argc, char **argv)
{
    int *events;
    int i, h;

    if(caca_init())
        return 1;

    caca_set_color(CACA_COLOR_WHITE, CACA_COLOR_BLUE);
    caca_draw_line(0, 0, caca_get_width() - 1, 0, ' ');

    caca_refresh();

    h = caca_get_height();

    events = malloc(h * sizeof(int));
    memset(events, 0, h * sizeof(int));

    for( ; ; )
    {
        unsigned int event = caca_wait_event(CACA_EVENT_ANY);

        if(!event)
            continue;

        memmove(events + 1, events, (h - 1) * sizeof(int));
        events[0] = event;

        caca_clear();

        /* Print current event */
        caca_set_color(CACA_COLOR_WHITE, CACA_COLOR_BLUE);
        caca_draw_line(0, 0, caca_get_width() - 1, 0, ' ');
        print_event(0, 0, event);

        /* Print previous events */
        caca_set_color(CACA_COLOR_WHITE, CACA_COLOR_BLACK);
        for(i = 1; i < h && events[i]; i++)
            print_event(0, i, events[i]);

        /* q quits */
        if(event == (CACA_EVENT_KEY_PRESS | 'q'))
            break;

        caca_refresh();
    }

    /* Clean up */
    caca_end();

    return 0;
}

static void print_event(int x, int y, unsigned int event)
{
    switch(event & 0xff000000)
    {
    case CACA_EVENT_NONE:
        caca_printf(x, y, "CACA_EVENT_NONE");
        break;
    case CACA_EVENT_KEY_PRESS:
        caca_printf(x, y, "CACA_EVENT_KEY_PRESS 0x%02x",
                    event & 0x00ffffff);
        break;
    case CACA_EVENT_KEY_RELEASE:
        caca_printf(x, y, "CACA_EVENT_KEY_RELEASE 0x2%x",
                    event & 0x00ffffff);
        break;
    case CACA_EVENT_MOUSE_MOTION:
        caca_printf(x, y, "CACA_EVENT_MOUSE_MOTION %u %u",
                    (event & 0x00fff000) >> 12, event & 0x00000fff);
        break;
    case CACA_EVENT_MOUSE_PRESS:
        caca_printf(x, y, "CACA_EVENT_MOUSE_PRESS %u",
                    event & 0x00ffffff);
        break;
    case CACA_EVENT_MOUSE_RELEASE:
        caca_printf(x, y, "CACA_EVENT_MOUSE_RELEASE %u",
                    event & 0x00ffffff);
        break;
    default:
        caca_printf(x, y, "CACA_EVENT_UNKNOWN");
    }
}

