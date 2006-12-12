/*
 *  fullwidth     libcaca fullwidth Unicode characters test program
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
#   if defined(HAVE_INTTYPES_H)
#      include <inttypes.h>
#   endif
#   include <stdio.h>
#endif

#include "cucul.h"
#include "caca.h"

#define CACA "쫊쫊쫊쫊쫊쫊쫊쫊쫊쫊쫊쫊쫊쫊쫊"

int main(int argc, char *argv[])
{
    cucul_canvas_t *cv, *caca, *line;
    caca_display_t *dp;

    unsigned int i;

    cv = cucul_create_canvas(0, 0);
    dp = caca_create_display(cv);

    caca = cucul_create_canvas(6, 10);
    line = cucul_create_canvas(2, 1);

    /* Line of x's */
    for(i = 0; i < 10; i++)
    {
        cucul_set_color_ansi(caca, CUCUL_WHITE, CUCUL_BLUE);
        cucul_put_str(caca, 0, i, CACA);
        cucul_set_color_ansi(caca, CUCUL_WHITE, CUCUL_RED);
        cucul_put_char(caca, i - 2, i, 'x');
    }

    cucul_blit(cv, 1, 1, caca, NULL);

    /* Line of ホ's */
    for(i = 0; i < 10; i++)
    {
        cucul_set_color_ansi(caca, CUCUL_WHITE, CUCUL_BLUE);
        cucul_put_str(caca, 0, i, CACA);
        cucul_set_color_ansi(caca, CUCUL_WHITE, CUCUL_GREEN);
        cucul_put_str(caca, i - 2, i, "ホ");
    }

    cucul_blit(cv, 15, 1, caca, NULL);

    /* Line of canvas */
    cucul_set_color_ansi(line, CUCUL_WHITE, CUCUL_MAGENTA);
    cucul_put_str(line, 0, 0, "ほ");
    for(i = 0; i < 10; i++)
    {
        cucul_set_color_ansi(caca, CUCUL_WHITE, CUCUL_BLUE);
        cucul_put_str(caca, 0, i, CACA);
        cucul_blit(caca, i - 2, i, line, NULL);
    }

    cucul_blit(cv, 29, 1, caca, NULL);

    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    caca_free_display(dp);

    cucul_free_canvas(line);
    cucul_free_canvas(caca);
    cucul_free_canvas(cv);

    return 0;
}

