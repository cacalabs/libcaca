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
    int i, h, quit;

    if(caca_init())
        return 1;

    h = caca_get_height() - 1;

    caca_set_color(CACA_COLOR_WHITE, CACA_COLOR_BLUE);
    caca_draw_line(0, 0, caca_get_width() - 1, 0, ' ');

    caca_draw_line(0, h, caca_get_width() - 1, h, ' ');
    caca_putstr(0, h, "type \"quit\" to exit");

    caca_refresh();

    events = malloc(h * sizeof(int));
    memset(events, 0, h * sizeof(int));

    for(quit = 0; quit < 4; )
    {
        static char const * quit_string[] = { "", "q", "qu", "qui", "quit" };
        unsigned int event = caca_wait_event(CACA_EVENT_ANY);

        if(!event)
            continue;

        do
        {
            /* "quit" quits */
            if(event & CACA_EVENT_KEY_PRESS)
            {
                int key = event & ~CACA_EVENT_KEY_PRESS;
                if((key == 'q' && quit == 0) || (key == 'u' && quit == 1)
                    || (key == 'i' && quit == 2) || (key == 't' && quit == 3))
                    quit++;
                else if(key == 'q')
                    quit = 1;
                else
                    quit = 0;
            }

            memmove(events + 1, events, (h - 1) * sizeof(int));
            events[0] = event;

            event = caca_get_event(CACA_EVENT_ANY);
        }
        while(event);

        caca_clear();

        /* Print current event */
        caca_set_color(CACA_COLOR_WHITE, CACA_COLOR_BLUE);
        caca_draw_line(0, 0, caca_get_width() - 1, 0, ' ');
        print_event(0, 0, events[0]);

        caca_draw_line(0, h, caca_get_width() - 1, h, ' ');
        caca_printf(0, h, "type \"quit\" to exit: %s", quit_string[quit]);

        /* Print previous events */
        caca_set_color(CACA_COLOR_WHITE, CACA_COLOR_BLACK);
        for(i = 1; i < h && events[i]; i++)
            print_event(0, i, events[i]);

        caca_refresh();
    }

    /* Clean up */
    caca_end();

    return 0;
}

static void print_event(int x, int y, unsigned int event)
{
    int character;

    switch(event & 0xff000000)
    {
    case CACA_EVENT_NONE:
        caca_printf(x, y, "CACA_EVENT_NONE");
        break;
    case CACA_EVENT_KEY_PRESS:
        character = event & 0x00ffffff;
        caca_printf(x, y, "CACA_EVENT_KEY_PRESS 0x%02x (%c)", character,
                    (character > 0x20 && character < 0x80) ? character : '?');
        break;
    case CACA_EVENT_KEY_RELEASE:
        character = event & 0x00ffffff;
        caca_printf(x, y, "CACA_EVENT_KEY_RELEASE 0x%02x (%c)", character,
                    (character > 0x20 && character < 0x80) ? character : '?');
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

