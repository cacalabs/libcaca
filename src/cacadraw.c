/*
 *  event         event lister for libcaca
 *  Copyright (c) 2004 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software. It commes without any warranty, to
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

static int refresh_screen(void);

static cucul_canvas_t *cv, *image;
static caca_display_t *dp;
static int x = 0, y = 0;

int main(int argc, char **argv)
{
    int refresh = 1, file = 1;
    unsigned int iw = 0, ih = 0;

    if(argc < 2)
    {
        fprintf(stderr, "%s: missing argument (filename).\n", argv[0]);
        return 1;
    }

    cv = cucul_create_canvas(0, 0);
    if(!cv)
        return 1;
    dp = caca_create_display(cv);
    if(!dp)
        return 1;

    for(;;)
    {
        caca_event_t ev;
        unsigned int w, h;
        int dx = 0, dy = 0;

        if(!image)
        {
            image = cucul_create_canvas(0, 0);
            if(cucul_import_file(image, argv[file], "ansi") < 0)
            {
                fprintf(stderr, "%s: invalid file `%s'.\n", argv[0], argv[1]);
                return 1;
            }

            ih = cucul_get_canvas_height(image);
            iw = cucul_get_canvas_width(image);
            x = y = 0;

            caca_set_display_title(dp, argv[file]);

            refresh = 1;
        }

        if(refresh)
        {
            refresh_screen();
            refresh = 0;
        }

        while(caca_get_event(dp, CACA_EVENT_ANY, &ev, -1))
        {
            switch(ev.type)
            {
                case CACA_EVENT_QUIT:
                    goto quit;
                case CACA_EVENT_KEY_PRESS:
                    switch(ev.data.key.ch)
                    {
                    case CACA_KEY_LEFT: dx -= 2; break;
                    case CACA_KEY_RIGHT: dx += 2; break;
                    case CACA_KEY_UP: dy--; break;
                    case CACA_KEY_DOWN: dy++; break;
                    case CACA_KEY_PAGEUP: dy -= 12; break;
                    case CACA_KEY_PAGEDOWN: dy += 12; break;
                    case CACA_KEY_ESCAPE:
                    case 'q':
                        goto quit;
                    case 'n':
                        file = file + 1 < argc ? file + 1 : 1;
                        cucul_free_canvas(image);
                        image = NULL;
                        goto stopevents;
                    case 'p':
                        file = file > 1 ? file - 1 : argc - 1;
                        cucul_free_canvas(image);
                        image = NULL;
                        goto stopevents;
                    default:
                        break;
                    }
                case CACA_EVENT_RESIZE:
                    refresh = 1;
                    goto stopevents;
                default:
                    break;
            }
        }
stopevents:

        w = cucul_get_canvas_width(cv);
        h = cucul_get_canvas_height(cv);

        if(dx | dy)
        {
            refresh = 1;
            x += dx;
            y += dy;

            if(x < 0) x = 0; else if(x + w > iw) x = iw > w ? iw - w : 0;
            if(y < 0) y = 0; else if(y + h > ih) y = ih > h ? ih - h : 0;
        }
    }
quit:

    /* Clean up */
    caca_free_display(dp);
    cucul_free_canvas(image);
    cucul_free_canvas(cv);

    return 0;
}

static int refresh_screen(void)
{
    cucul_set_color_ansi(cv, CUCUL_DEFAULT, CUCUL_DEFAULT);
    cucul_clear_canvas(cv);

    cucul_blit(cv, - x, - y, image, NULL);

    caca_refresh_display(dp);

    return 0;
}

