/*
 *  import        libcaca importers test program
 *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
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
#   include <stdlib.h>
#endif


#include "cucul.h"
#include "caca.h"

int main(int argc, char *argv[])
{
    cucul_canvas_t *cv;
    caca_display_t *dp;

    if(argc < 2)
    {
        fprintf(stderr, "%s: missing argument (filename).\n", argv[0]);
        fprintf(stderr, "usage: %s <filename> [<format>]\n", argv[0]);
        return 1;
    }

    cv = cucul_create_canvas(0, 0);
    if(cucul_import_file(cv, argv[1], argc >= 3 ? argv[2] : "") < 0)
    {
	fprintf(stderr, "%s: could not open `%s'.\n", argv[0], argv[1]);
        cucul_free_canvas(cv);
        return 1;
    }

    dp = caca_create_display(cv);

    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    caca_free_display(dp);
    cucul_free_canvas(cv);

    return 0;
}

