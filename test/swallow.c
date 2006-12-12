/*
 *  swallow       swallow another libcaca application
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "config.h"
#include "common.h"
#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <string.h>
#   include <stdlib.h>
#endif

#include "cucul.h"
#include "caca.h"

int main(int argc, char **argv)
{
    char cmd[BUFSIZ];
    static cucul_canvas_t *cv, *app;
    static caca_display_t *dp;
    unsigned char *buf[4];
    long int bytes[4], total[4];
    FILE *f[4];
    int w, h, i;

    if(argc < 5)
    {
        fprintf(stderr, "usage: %s <cmd1> <cmd2> <cmd3> <cmd4>\n", argv[0]);
        return 1;
    }

    cv = cucul_create_canvas(0, 0);
    app = cucul_create_canvas(0, 0);
    dp = caca_create_display(cv);
    if(!dp)
        return 1;

    w = (cucul_get_canvas_width(cv) - 4) / 2;
    h = (cucul_get_canvas_height(cv) - 6) / 2;

    if(w < 0 || h < 0)
        return 1;

    cucul_set_color_ansi(cv, CUCUL_WHITE, CUCUL_BLUE);
    cucul_draw_line(cv, 0, 0, cucul_get_canvas_width(cv) - 1, 0, ' ');
    cucul_printf(cv, cucul_get_canvas_width(cv) / 2 - 10, 0,
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
        cucul_printf(cv, (w + 2) * (i / 2) + 1,
                         (h + 2) * ((i % 2) + 1), "%s", argv[i + 1]);
    }

    for(;;)
    {
        caca_event_t ev;
        int ret = caca_get_event(dp, CACA_EVENT_ANY, &ev, 0);

        if(ret && ev.type & CACA_EVENT_KEY_PRESS)
            break;

        for(i = 0; i < 4; i++)
        {
            bytes[i] = cucul_import_memory(app, buf[i], total[i], "caca");

            if(bytes[i] > 0)
            {
                total[i] -= bytes[i];
                memmove(buf[i], buf[i] + bytes[i], total[i]);

                cucul_blit(cv, (w + 2) * (i / 2) + 1,
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
    cucul_free_canvas(cv);
    cucul_free_canvas(app);

    return 0;
}

