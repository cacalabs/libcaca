/*
 *  import        libcaca importers test program
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
#   include <stdio.h>
#   include <stdlib.h>
#endif

#include "caca.h"

int main(int argc, char *argv[])
{
    caca_canvas_t *cv;
    caca_display_t *dp;

    if(argc < 2)
    {
        fprintf(stderr, "%s: missing argument (filename).\n", argv[0]);
        fprintf(stderr, "usage: %s <filename> [<format>]\n", argv[0]);
        return 1;
    }

    cv = caca_create_canvas(0, 0);
    if(cv == NULL)
    {
        printf("Can't create canvas\n");
        return -1;
    }

    if(caca_import_canvas_from_file(cv, argv[1], argc >= 3 ? argv[2] : "") < 0)
    {
        fprintf(stderr, "%s: could not open `%s'.\n", argv[0], argv[1]);
        caca_free_canvas(cv);
        return 1;
    }

    dp = caca_create_display(cv);
    if(dp == NULL)
    {
        printf("Can't create display\n");
        return -1;
    }

    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    caca_free_display(dp);
    caca_free_canvas(cv);

    return 0;
}

