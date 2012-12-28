/*
 *  fullwidth     libcaca fullwidth Unicode characters test program
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
#endif

#include "caca.h"

#define CACA "쫊쫊쫊쫊쫊쫊쫊쫊쫊쫊쫊쫊쫊쫊쫊"

int main(int argc, char *argv[])
{
    caca_canvas_t *cv, *caca, *line;
    caca_display_t *dp;

    unsigned int i;

    cv = caca_create_canvas(0, 0);
    if(cv == NULL)
    {
        printf("Can't created canvas\n");
        return -1;
    }
    dp = caca_create_display(cv);
    if(dp == NULL)
    {
        printf("Can't create display\n");
        return -1;
    }

    caca = caca_create_canvas(6, 10);
    line = caca_create_canvas(2, 1);

    /* Line of x's */
    for(i = 0; i < 10; i++)
    {
        caca_set_color_ansi(caca, CACA_WHITE, CACA_BLUE);
        caca_put_str(caca, 0, i, CACA);
        caca_set_color_ansi(caca, CACA_WHITE, CACA_RED);
        caca_put_char(caca, i - 2, i, 'x');
    }

    caca_blit(cv, 1, 1, caca, NULL);

    /* Line of ホ's */
    for(i = 0; i < 10; i++)
    {
        caca_set_color_ansi(caca, CACA_WHITE, CACA_BLUE);
        caca_put_str(caca, 0, i, CACA);
        caca_set_color_ansi(caca, CACA_WHITE, CACA_GREEN);
        caca_put_str(caca, i - 2, i, "ホ");
    }

    caca_blit(cv, 15, 1, caca, NULL);

    /* Line of canvas */
    caca_set_color_ansi(line, CACA_WHITE, CACA_MAGENTA);
    caca_put_str(line, 0, 0, "ほ");
    for(i = 0; i < 10; i++)
    {
        caca_set_color_ansi(caca, CACA_WHITE, CACA_BLUE);
        caca_put_str(caca, 0, i, CACA);
        caca_blit(caca, i - 2, i, line, NULL);
    }

    caca_blit(cv, 29, 1, caca, NULL);

    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    caca_free_display(dp);

    caca_free_canvas(line);
    caca_free_canvas(caca);
    caca_free_canvas(cv);

    return 0;
}

