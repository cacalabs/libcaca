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
#include "common.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cucul.h"
#include "caca.h"

static int refresh_screen(void);

static cucul_canvas_t *cv, *image;
static caca_display_t *dp;
static int x = 0, y = 0;

int main(int argc, char **argv)
{
    cucul_buffer_t *b;
    int refresh = 1;
    unsigned int iw, ih;

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
    caca_set_display_title(dp, argv[1]);

    b = cucul_load_file(argv[1]);
    if(!b)
    {
        fprintf(stderr, "%s: could not open `%s'.\n", argv[0], argv[1]);
        return 1;
    }

    image = cucul_import_canvas(b, "");
    if(!image)
    {
        fprintf(stderr, "%s: could not import `%s'.\n", argv[0], argv[1]);
        return 1;
    }

    cucul_free_buffer(b);

    ih = cucul_get_canvas_height(image);
    iw = cucul_get_canvas_width(image);

    for(;;)
    {
        caca_event_t ev;
        unsigned int w, h;

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
                    case CACA_KEY_LEFT: x -= 2; refresh = 1; goto stopevents;
                    case CACA_KEY_RIGHT: x += 2; refresh = 1; goto stopevents;
                    case CACA_KEY_UP: y--; refresh = 1; goto stopevents;
                    case CACA_KEY_DOWN: y++; refresh = 1; goto stopevents;
                    case CACA_KEY_PAGEUP: y-=25; refresh = 1; goto stopevents;
                    case CACA_KEY_PAGEDOWN: y+=25; refresh = 1; goto stopevents;
                    case CACA_KEY_ESCAPE:
                    case 'q':
                        goto quit;
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
        if(x < 0) x = 0; else if(x + w > iw) x = iw > w ? iw - w : 0;
        if(y < 0) y = 0; else if(y + h > ih) y = ih > h ? ih - h : 0;
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
    cucul_set_color(cv, CUCUL_COLOR_DEFAULT, CUCUL_COLOR_DEFAULT);
    cucul_clear_canvas(cv);

    cucul_blit(cv, - x, - y, image, NULL);

    caca_refresh_display(dp);

    return 0;
}

