/*
 *  cacaplay      caca file player
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

#include <stdio.h>
#include <stdlib.h>

#include "cucul.h"
#include "caca.h"

int main(int argc, char **argv)
{
    caca_event_t ev;
    cucul_buffer_t *b;
    cucul_canvas_t *cv;
    caca_display_t *dp;

    if(argc < 2)
    {
        fprintf(stderr, "%s: missing argument (filename).\n", argv[0]);
        return 1;
    }

    b = cucul_load_file(argv[1]);
    if(!b)
    {
        fprintf(stderr, "%s: could not open %s.\n", argv[0], argv[1]);
        return 1;
    }

    cv = cucul_import_canvas(b, "caca");
    if(!cv)
    {
        fprintf(stderr, "%s: invalid caca file %s.\n", argv[0], argv[1]);
        cucul_free_buffer(b);
        return 1;
    }

    cucul_free_buffer(b);

    dp = caca_create_display(cv);

    caca_refresh_display(dp);

    while(caca_get_event(dp, CACA_EVENT_KEY_PRESS, &ev, -1))
    {
        if(ev.data.key.ch == CACA_KEY_ESCAPE)
            break;
    }

    /* Clean up */
    caca_free_display(dp);
    cucul_free_canvas(cv);

    return 0;
}

