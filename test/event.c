/*
 *  event         event lister for libcaca
 *  Copyright (c) 2004 Sam Hocevar <sam@zoy.org>
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cucul.h"
#include "caca.h"

static cucul_t *qq;
static caca_t *kk;

static void print_event(int, int, struct caca_event *);

int main(int argc, char **argv)
{
    struct caca_event *events;
    int i, h, quit;

    qq = cucul_create(0, 0);
    if(!qq)
        return 1;
    kk = caca_attach(qq);
    if(!kk)
        return 1;

    h = cucul_get_height(qq) - 1;

    cucul_set_color(qq, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
    cucul_draw_line(qq, 0, 0, cucul_get_width(qq) - 1, 0, " ");

    cucul_draw_line(qq, 0, h, cucul_get_width(qq) - 1, h, " ");
    cucul_putstr(qq, 0, h, "type \"quit\" to exit");

    caca_display(kk);

    events = malloc(h * sizeof(struct caca_event));
    memset(events, 0, h * sizeof(struct caca_event));

    for(quit = 0; quit < 4; )
    {
        struct caca_event ev;
        static char const * quit_string[] = { "", "q", "qu", "qui", "quit" };
        int ret = caca_wait_event(kk, CACA_EVENT_ANY, &ev);

        if(!ret)
            continue;

        do
        {
            /* "quit" quits */
            if(ev.type & CACA_EVENT_KEY_PRESS)
            {
                int key = ev.data.key.c;
                if((key == 'q' && quit == 0) || (key == 'u' && quit == 1)
                    || (key == 'i' && quit == 2) || (key == 't' && quit == 3))
                    quit++;
                else if(key == 'q')
                    quit = 1;
                else
                    quit = 0;
            }

            memmove(events + 1, events, (h - 1) * sizeof(struct caca_event));
            events[0] = ev;

            ret = caca_get_event(kk, CACA_EVENT_ANY, &ev);
        }
        while(ret);

        cucul_clear(qq);

        /* Print current event */
        cucul_set_color(qq, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
        cucul_draw_line(qq, 0, 0, cucul_get_width(qq) - 1, 0, " ");
        print_event(0, 0, events);

        cucul_draw_line(qq, 0, h, cucul_get_width(qq) - 1, h, " ");
        cucul_printf(qq, 0, h, "type \"quit\" to exit: %s", quit_string[quit]);

        /* Print previous events */
        cucul_set_color(qq, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLACK);
        for(i = 1; i < h && events[i].type; i++)
            print_event(0, i, events + i);

        caca_display(kk);
    }

    /* Clean up */
    caca_detach(kk);
    cucul_free(qq);

    return 0;
}

static void print_event(int x, int y, struct caca_event *ev)
{
    int character;

    switch(ev->type)
    {
    case CACA_EVENT_NONE:
        cucul_printf(qq, x, y, "CACA_EVENT_NONE");
        break;
    case CACA_EVENT_KEY_PRESS:
        character = ev->data.key.c;
        cucul_printf(qq, x, y, "CACA_EVENT_KEY_PRESS 0x%02x (%c)", character,
                     (character > 0x20 && character < 0x80) ? character : '?');
        break;
    case CACA_EVENT_KEY_RELEASE:
        character = ev->data.key.c;
        cucul_printf(qq, x, y, "CACA_EVENT_KEY_RELEASE 0x%02x (%c)", character,
                     (character > 0x20 && character < 0x80) ? character : '?');
        break;
    case CACA_EVENT_MOUSE_MOTION:
        cucul_printf(qq, x, y, "CACA_EVENT_MOUSE_MOTION %u %u",
                     ev->data.mouse.x, ev->data.mouse.y);
        break;
    case CACA_EVENT_MOUSE_PRESS:
        cucul_printf(qq, x, y, "CACA_EVENT_MOUSE_PRESS %u",
                     ev->data.mouse.button);
        break;
    case CACA_EVENT_MOUSE_RELEASE:
        cucul_printf(qq, x, y, "CACA_EVENT_MOUSE_RELEASE %u",
                     ev->data.mouse.button);
        break;
    case CACA_EVENT_RESIZE:
        cucul_printf(qq, x, y, "CACA_EVENT_RESIZE %u %u",
                     ev->data.resize.w, ev->data.resize.h);
        break;
    default:
        cucul_printf(qq, x, y, "CACA_EVENT_UNKNOWN");
    }
}

