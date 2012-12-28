/*
 *  event         event lister for libcaca
 *  Copyright (c) 2004-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <string.h>
#   include <stdlib.h>
#endif

#include "caca.h"

static caca_canvas_t *cv;
static caca_display_t *dp;

static void print_event(int, int, caca_event_t *);

int main(int argc, char **argv)
{
    caca_event_t *events;
    int i, h, quit;

    cv = caca_create_canvas(80, 24);
    if(cv == NULL)
    {
        printf("Failed to create canvas\n");
        return 1;
    }

    dp = caca_create_display(cv);
    if(dp == NULL)
    {
        printf("Failed to create display\n");
        return 1;
    }

    h = caca_get_canvas_height(cv) - 1;

    caca_set_color_ansi(cv, CACA_WHITE, CACA_BLUE);
    caca_draw_line(cv, 0, 0, caca_get_canvas_width(cv) - 1, 0, ' ');

    caca_draw_line(cv, 0, h, caca_get_canvas_width(cv) - 1, h, ' ');
    caca_put_str(cv, 0, h, "type \"quit\" to exit");

    caca_refresh_display(dp);

    events = malloc(h * sizeof(caca_event_t));
    memset(events, 0, h * sizeof(caca_event_t));

    for(quit = 0; quit < 4; )
    {
        caca_event_t ev;
        static char const * quit_string[] = { "", "q", "qu", "qui", "quit" };
        int ret = caca_get_event(dp, CACA_EVENT_ANY, &ev, -1);

        if(!ret)
            continue;

        do
        {
            /* "quit" quits */
            if(caca_get_event_type(&ev) & CACA_EVENT_KEY_PRESS)
            {
                int key = caca_get_event_key_ch(&ev);
                if((key == 'q' && quit == 0) || (key == 'u' && quit == 1)
                    || (key == 'i' && quit == 2) || (key == 't' && quit == 3))
                    quit++;
                else if(key == 'q')
                    quit = 1;
                else
                    quit = 0;
            }

            memmove(events + 1, events, (h - 1) * sizeof(caca_event_t));
            events[0] = ev;

            ret = caca_get_event(dp, CACA_EVENT_ANY, &ev, 0);
        }
        while(ret);

        caca_set_color_ansi(cv, CACA_LIGHTGRAY, CACA_BLACK);
        caca_clear_canvas(cv);

        /* Print current event */
        caca_set_color_ansi(cv, CACA_WHITE, CACA_BLUE);
        caca_draw_line(cv, 0, 0, caca_get_canvas_width(cv) - 1, 0, ' ');
        print_event(0, 0, events);

        caca_draw_line(cv, 0, h, caca_get_canvas_width(cv) - 1, h, ' ');
        caca_printf(cv, 0, h, "type \"quit\" to exit: %s", quit_string[quit]);

        /* Print previous events */
        caca_set_color_ansi(cv, CACA_WHITE, CACA_BLACK);
        for(i = 1; i < h && caca_get_event_type(&events[i]); i++)
            print_event(0, i, events + i);

        caca_refresh_display(dp);
    }

    /* Clean up */
    free(events);
    caca_free_display(dp);
    caca_free_canvas(cv);

    return 0;
}

static void print_event(int x, int y, caca_event_t *ev)
{
    int character;

    switch(caca_get_event_type(ev))
    {
    case CACA_EVENT_NONE:
        caca_printf(cv, x, y, "CACA_EVENT_NONE");
        break;
    case CACA_EVENT_KEY_PRESS:
        character = caca_get_event_key_ch(ev);
        caca_printf(cv, x, y, "CACA_EVENT_KEY_PRESS 0x%02x (%c)", character,
                     (character > 0x1f && character < 0x80) ? character : '?');
        break;
    case CACA_EVENT_KEY_RELEASE:
        character = caca_get_event_key_ch(ev);
        caca_printf(cv, x, y, "CACA_EVENT_KEY_RELEASE 0x%02x (%c)", character,
                     (character > 0x1f && character < 0x80) ? character : '?');
        break;
    case CACA_EVENT_MOUSE_MOTION:
        caca_printf(cv, x, y, "CACA_EVENT_MOUSE_MOTION %u %u",
                     caca_get_event_mouse_x(ev), caca_get_event_mouse_y(ev));
        break;
    case CACA_EVENT_MOUSE_PRESS:
        caca_printf(cv, x, y, "CACA_EVENT_MOUSE_PRESS %u",
                     caca_get_event_mouse_button(ev));
        break;
    case CACA_EVENT_MOUSE_RELEASE:
        caca_printf(cv, x, y, "CACA_EVENT_MOUSE_RELEASE %u",
                     caca_get_event_mouse_button(ev));
        break;
    case CACA_EVENT_RESIZE:
        caca_printf(cv, x, y, "CACA_EVENT_RESIZE %u %u",
                     caca_get_event_resize_width(ev),
                     caca_get_event_resize_height(ev));
        break;
    case CACA_EVENT_QUIT:
        caca_printf(cv, x, y, "CACA_EVENT_QUIT");
        break;
    default:
        caca_printf(cv, x, y, "CACA_EVENT_UNKNOWN");
    }
}

