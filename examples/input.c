/*
 *  input         libcaca text input test program
 *  Copyright (c) 2006-2012 Sam Hocevar <sam@hocevar.net>
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
#   include <string.h>
#   include <stdio.h>
#endif

#include "caca.h"

#define BUFFER_SIZE 75
#define TEXT_ENTRIES 5

typedef struct textentry
{
    uint32_t buffer[BUFFER_SIZE + 1];
    unsigned int size, cursor, changed;
} textentry;

int main(int argc, char *argv[])
{
    textentry entries[TEXT_ENTRIES];
    caca_canvas_t *cv;
    caca_display_t *dp;
    unsigned int i, e = 0, running = 1;

    cv = caca_create_canvas(0, 0);
    if(cv == NULL)
    {
        printf("Can't create canvas\n");
        return -1;
    }
    dp = caca_create_display(cv);
    if(dp == NULL)
    {
        printf("Can't create display\n");
        return -1;
    }
    caca_set_cursor(dp, 1);

    caca_set_color_ansi(cv, CACA_WHITE, CACA_BLUE);
    caca_put_str(cv, 1, 1, "Text entries - press tab to cycle");

    for(i = 0; i < TEXT_ENTRIES; i++)
    {
        entries[i].buffer[0] = 0;
        entries[i].size = 0;
        entries[i].cursor = 0;
        entries[i].changed = 1;
        caca_printf(cv, 3, 3 * i + 4, "[entry %i]", i + 1);
    }

    /* Put Unicode crap in the last text entry */
    entries[TEXT_ENTRIES - 1].buffer[0] = 'A';
    entries[TEXT_ENTRIES - 1].buffer[1] = 'b';
    entries[TEXT_ENTRIES - 1].buffer[2] = caca_utf8_to_utf32("Ç", NULL);
    entries[TEXT_ENTRIES - 1].buffer[3] = caca_utf8_to_utf32("đ", NULL);
    entries[TEXT_ENTRIES - 1].buffer[4] = caca_utf8_to_utf32("ボ", NULL);
    entries[TEXT_ENTRIES - 1].buffer[5] = CACA_MAGIC_FULLWIDTH;
    entries[TEXT_ENTRIES - 1].buffer[6] = caca_utf8_to_utf32("♥", NULL);
    entries[TEXT_ENTRIES - 1].size = 7;

    while(running)
    {
        caca_event_t ev;

        for(i = 0; i < TEXT_ENTRIES; i++)
        {
            unsigned int j, start, size;

            if(!entries[i].changed)
                continue;

            caca_set_color_ansi(cv, CACA_BLACK, CACA_LIGHTGRAY);
            caca_fill_box(cv, 2, 3 * i + 5, BUFFER_SIZE + 1, 1, ' ');

            start = 0;
            size = entries[i].size;

            for(j = 0; j < size; j++)
            {
                caca_put_char(cv, 2 + j, 3 * i + 5,
                              entries[i].buffer[start + j]);
            }

            entries[i].changed = 0;
        }

        /* Put the cursor on the active textentry */
        caca_gotoxy(cv, 2 + entries[e].cursor, 3 * e + 5);

        caca_refresh_display(dp);

        if(caca_get_event(dp, CACA_EVENT_KEY_PRESS, &ev, -1) == 0)
            continue;

        switch(caca_get_event_key_ch(&ev))
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
                    entries[e].changed = 1;
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
                    entries[e].changed = 1;
                }
                break;
            default:
                if(entries[e].size < BUFFER_SIZE)
                {
                    memmove(entries[e].buffer + entries[e].cursor + 1,
                            entries[e].buffer + entries[e].cursor,
                            (entries[e].size - entries[e].cursor) * 4);
                    entries[e].buffer[entries[e].cursor] =
                                              caca_get_event_key_utf32(&ev);
                    entries[e].size++;
                    entries[e].cursor++;
                    entries[e].changed = 1;
                }
                break;
        }
    }

    caca_free_display(dp);
    caca_free_canvas(cv);

    return 0;
}

