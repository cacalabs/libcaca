/*
 *  import       libcaca importers test program
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

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "cucul.h"
#include "caca.h"

int main(int argc, char *argv[])
{
    cucul_canvas_t *cv;
    caca_display_t *dp;
    FILE *fp;
    unsigned char *buffer;
    int size=0;

    if(argc < 2)
    {
        fprintf(stderr, "%s: missing argument (filename).\n", argv[0]);
        return 1;
    }

    fp = fopen(argv[1], "rb");

    if(!fp)
    {
	fprintf(stderr, "%s: could not open `%s'.\n", argv[0], argv[1]);
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = malloc(sizeof(unsigned char) * size);
    if(!buffer)
    {
	fprintf(stderr, "%s: Can't allocate memory (%d bytes)\n", argv[0], size);
        return 1;
    }
    if(!fread(buffer, size, 1, fp))
    {
	fprintf(stderr, "%s: Can't read %s\n", argv[0], argv[1]);
        return 1;
    }



    cv = cucul_import_canvas(buffer, size, "");

    if(cv==NULL)
    {
	fprintf(stderr, "%s: Can't load %s, unknow reason.\n", argv[0], argv[1]);
        return 1;
    }

    dp = caca_create_display(cv);

    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    caca_free_display(dp);
    cucul_free_canvas(cv);


    return 0;
}

