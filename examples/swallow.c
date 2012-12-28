/*
 *  swallow       swallow another libcaca application
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
#   include <stdio.h>
#   include <string.h>
#   include <stdlib.h>
#endif

#include "caca.h"

int main(int argc, char **argv)
{
    char cmd[BUFSIZ];
    static caca_canvas_t *cv, *app;
    static caca_display_t *dp;
    uint8_t *buf[4];
    long int bytes[4], total[4];
    FILE *f[4];
    int w, h, i;

    if(argc < 5)
    {
        fprintf(stderr, "usage: %s <cmd1> <cmd2> <cmd3> <cmd4>\n", argv[0]);
        return 1;
    }

    cv = caca_create_canvas(0, 0);
    app = caca_create_canvas(0, 0);
    dp = caca_create_display(cv);

    if(cv == NULL || app == NULL )
    {
        printf("Can't created canvas\n");
        return -1;
    }
    if(dp == NULL)
    {
        printf("Can't create display\n");
        return -1;
    }

    w = (caca_get_canvas_width(cv) - 4) / 2;
    h = (caca_get_canvas_height(cv) - 6) / 2;

    if(w < 0 || h < 0)
        return 1;

    caca_set_color_ansi(cv, CACA_WHITE, CACA_BLUE);
    caca_draw_line(cv, 0, 0, caca_get_canvas_width(cv) - 1, 0, ' ');
    caca_printf(cv, caca_get_canvas_width(cv) / 2 - 10, 0,
                 "libcaca multiplexer");

    for(i = 0; i < 4; i++)
    {
        buf[i] = NULL;
        total[i] = bytes[i] = 0;
        sprintf(cmd, "CACA_DRIVER=raw CACA_GEOMETRY=%ix%i %s",
                     w, h, argv[i + 1]);
        f[i] = popen(cmd, "r");
        if(!f[i])
            return 1;
        caca_printf(cv, (w + 2) * (i / 2) + 1,
                         (h + 2) * ((i % 2) + 1), "%s", argv[i + 1]);
    }

    for(;;)
    {
        caca_event_t ev;
        int ret = caca_get_event(dp, CACA_EVENT_ANY, &ev, 0);

        if(ret && caca_get_event_type(&ev) & CACA_EVENT_KEY_PRESS)
            break;

        for(i = 0; i < 4; i++)
        {
            bytes[i] = caca_import_canvas_from_memory(app, buf[i],
                                                      total[i], "caca");

            if(bytes[i] > 0)
            {
                total[i] -= bytes[i];
                memmove(buf[i], buf[i] + bytes[i], total[i]);

                caca_blit(cv, (w + 2) * (i / 2) + 1,
                               (h + 2) * (i % 2) + 2, app, NULL);
                caca_refresh_display(dp);
            }
            else if(bytes[i] == 0)
            {
                buf[i] = realloc(buf[i], total[i] + 128);
                fread(buf[i] + total[i], 128, 1, f[i]);
                total[i] += 128;
            }
            else
            {
                fprintf(stderr, "%s: corrupted input %i\n", argv[0], i);
                return -1;
            }
        }
    }

    /* Clean up */
    caca_free_display(dp);
    caca_free_canvas(cv);
    caca_free_canvas(app);

    return 0;
}

