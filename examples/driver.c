/*
 *  driver        libcaca Unicode rendering test program
 *  Copyright (c) 2007-2012 Sam Hocevar <sam@hocevar.net>
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

int main(int argc, char *argv[])
{
    char const * const *list;
    caca_display_t *dp;
    caca_canvas_t *cv;

    list = caca_get_display_driver_list();

    dp = caca_create_display(NULL);
    if(dp == NULL)
    {
        printf("cannot create display\n");
        return -1;
    }

    cv = caca_get_canvas(dp);
    caca_set_color_ansi(cv, CACA_WHITE, CACA_BLACK);

    while(1)
    {
        char const *driver;
        int i, cur = 0;

        caca_put_str(cv, 1, 0, "Available drivers:");

        driver = caca_get_display_driver(dp);

        for(i = 0; list[i]; i += 2)
        {
            int match = !strcmp(list[i], driver);

            if(match)
                cur = i;
            caca_draw_line(cv, 0, i + 2, 9999, i + 2, ' ');
            caca_printf(cv, 2, i + 2, "%c %s (%s)",
                         match ? '*' : ' ', list[i], list[i + 1]);
        }

        caca_put_str(cv, 1, i + 2, "Switching driver in 5 seconds");

        caca_refresh_display(dp);

        if(caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, 5000000))
            break;

        do
        {
            cur += 2;
            if(list[cur] && !strcmp(list[cur], "raw"))
                cur += 2;
            if(!list[cur])
                cur = 0;
        }
        while(caca_set_display_driver(dp, list[cur]));
    }

    caca_free_display(dp);

    return 0;
}

