/*
 *  input         libcaca text input test program
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
#include "common.h"

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#endif

#include <string.h>

#include "cucul.h"
#include "caca.h"

#define BUFFER_SIZE 75
#define TEXT_ENTRIES 5

typedef struct textentry
{
    uint32_t buffer[BUFFER_SIZE + 1];
    unsigned int size, cursor;
} textentry;

int main(void)
{
    textentry entries[TEXT_ENTRIES];
    cucul_canvas_t *cv;
    caca_display_t *dp;
    unsigned int i, e = 0, running = 1;

    cv = cucul_create_canvas(0, 0);
    dp = caca_create_display(cv);

    cucul_set_color(cv, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
    cucul_putstr(cv, 1, 1, "Text entries - press tab to cycle");

    for(i = 0; i < TEXT_ENTRIES; i++)
    {
        entries[i].buffer[0] = 0;
        entries[i].size = 0;
        entries[i].cursor = 0;
    }

    while(running)
    {
        caca_event_t ev;

        for(i = 0; i < TEXT_ENTRIES; i++)
        {
            unsigned int j, start, size;

            cucul_set_color(cv, CUCUL_COLOR_BLACK, CUCUL_COLOR_LIGHTGRAY);
            cucul_fill_box(cv, 2, 3 * i + 4, 2 + BUFFER_SIZE, 3 * i + 4, " ");

            start = 0;
            size = entries[i].size;

            for(j = 0; j < size; j++)
            {
                cucul_putchar(cv, 2 + j, 3 * i + 4,
                              entries[i].buffer[start + j]);
            }
        }

        /* Put the cursor on the active textentry */
        cucul_set_color(cv, CUCUL_COLOR_WHITE, CUCUL_COLOR_WHITE);
        cucul_putchar(cv, 2 + entries[e].cursor, 3 * e + 4, ' ');

        caca_refresh_display(dp);

        if(caca_get_event(dp, CACA_EVENT_KEY_PRESS, &ev, -1) == 0)
            continue;

        switch(ev.data.key.ch)
        {
            case CACA_KEY_ESCAPE:
                running = 0;
                break;
            case CACA_KEY_TAB:
            case CACA_KEY_RETURN:
                e = (e + 1) % TEXT_ENTRIES;
                break;
            case CACA_KEY_HOME:
                entries[e].cursor = 0;
                break;
            case CACA_KEY_END:
                entries[e].cursor = entries[e].size;
                break;
            case CACA_KEY_LEFT:
                if(entries[e].cursor)
                    entries[e].cursor--;
                break;
            case CACA_KEY_RIGHT:
                if(entries[e].cursor < entries[e].size)
                    entries[e].cursor++;
                break;
            case CACA_KEY_DELETE:
                if(entries[e].cursor < entries[e].size)
                {
                    memmove(entries[e].buffer + entries[e].cursor,
                            entries[e].buffer + entries[e].cursor + 1,
                            (entries[e].size - entries[e].cursor + 1) * 4);
                    entries[e].size--;
                }
                break;
            case CACA_KEY_BACKSPACE:
                if(entries[e].cursor)
                {
                    memmove(entries[e].buffer + entries[e].cursor - 1,
                            entries[e].buffer + entries[e].cursor,
                            (entries[e].size - entries[e].cursor) * 4);
                    entries[e].size--;
                    entries[e].cursor--;
                }
                break;
            default:
                if(entries[e].size < BUFFER_SIZE)
                {
                    memmove(entries[e].buffer + entries[e].cursor + 1,
                            entries[e].buffer + entries[e].cursor,
                            (entries[e].size - entries[e].cursor) * 4);
                    entries[e].buffer[entries[e].cursor] = ev.data.key.ucs4;
                    entries[e].size++;
                    entries[e].cursor++;
                }
                break;
        }
    }

    caca_free_display(dp);
    cucul_free_canvas(cv);

    return 0;
}

