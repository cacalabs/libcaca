/*
 *  mouse         libcaca mouse events
 *  Copyright (c) 2006-2012 Jean-Yves Lamoureux <jylam@lnxscene.org>
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
#   include <math.h>
#   include <string.h>
#   include <stdio.h>
#endif

#include "caca.h"


int main(int argc, char *argv[])
{
    int quit = 0;
    int x = 0, y = 0, p = 0, b = 0;
    caca_canvas_t *cv;
    caca_display_t *dp;

    cv = caca_create_canvas(80, 24);
    if (cv == NULL)
    {
        printf("Failed to create canvas\n");
        return 1;
    }

    dp = caca_create_display(cv);
    if (dp == NULL)
    {
        printf("Failed to create display\n");
        return 1;
    }

    caca_set_display_time(dp, 40000);
    caca_set_cursor(dp, 0);

    while (!quit)
    {
        caca_event_t ev;
        while (caca_get_event(dp, CACA_EVENT_ANY, &ev, 0))
        {
            if ((caca_get_event_type(&ev) & CACA_EVENT_KEY_PRESS))
            {
                quit = 1;
            }
            if (caca_get_event_type(&ev) & CACA_EVENT_MOUSE_MOTION)
            {
                x = caca_get_event_mouse_x(&ev);
                y = caca_get_event_mouse_y(&ev);
            }
            if (caca_get_event_type(&ev) & CACA_EVENT_MOUSE_PRESS)
            {
                p = 1;
                b = caca_get_event_mouse_button(&ev);
            }
            else if (caca_get_event_type(&ev) & CACA_EVENT_MOUSE_RELEASE)
            {
                p = 0;
                b = caca_get_event_mouse_button(&ev);
            }
        }
        caca_printf(cv, 0, 0, "%d,%d", x, y);
        if (b)
        {
            caca_printf(cv, 0, 1, "Mouse button %d %s", b,
                        p == 1 ? "pressed" : "released");
        }

        caca_printf(cv, x - 2, y - 1, "  |");
        caca_printf(cv, x - 2, y, "--|--");
        caca_printf(cv, x - 2, y + 1, "  |");
        caca_refresh_display(dp);
        caca_clear_canvas(cv);
    }

    caca_free_display(dp);
    caca_free_canvas(cv);

    return 0;
}
