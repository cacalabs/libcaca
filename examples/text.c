/*
 *  text          canvas text import/export
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
#   include <string.h>
#   include <stdlib.h>
#endif

#include "caca.h"

#define STRING \
  "              |_| \n" \
  "   _,----._   | | \n" \
  "  (/ @  @ \\)   __ \n" \
  "   |  OO  |   |_  \n" \
  "   \\ `--' /   |__ \n" \
  "    `----'        \n" \
  "              |_| \n" \
  " Hello world!  |  \n" \
  "                  \n"

int main(int argc, char *argv[])
{
    caca_canvas_t *cv, *pig;
    void *buffer;
    size_t len;
    int i, j;

    pig = caca_create_canvas(0, 0);
    caca_import_canvas_from_memory(pig, STRING, strlen(STRING), "text");

    cv = caca_create_canvas(caca_get_canvas_width(pig) * 2,
                             caca_get_canvas_height(pig) * 2);

    if(cv == NULL || pig == NULL)
    {
        printf("Can't created canvas\n");
        return -1;
    }

    caca_blit(cv, 0, 0, pig, NULL);
    caca_flip(pig);
    caca_blit(cv, caca_get_canvas_width(pig), 0, pig, NULL);
    caca_flip(pig);
    caca_flop(pig);
    caca_blit(cv, 0, caca_get_canvas_height(pig), pig, NULL);
    caca_flop(pig);
    caca_rotate_180(pig);
    caca_blit(cv, caca_get_canvas_width(pig),
                   caca_get_canvas_height(pig), pig, NULL);

    for(j = 0; j < caca_get_canvas_height(cv); j++)
    {
        for(i = 0; i < caca_get_canvas_width(cv); i += 2)
        {
            unsigned long int a;
            caca_set_color_ansi(cv, CACA_LIGHTBLUE + (i + j) % 6,
                                 CACA_DEFAULT);
            a = caca_get_attr(cv, -1, -1);
            caca_put_attr(cv, i, j, a);
            caca_put_attr(cv, i + 1, j, a);
        }
    }

    buffer = caca_export_canvas_to_memory(cv, "utf8", &len);
    fwrite(buffer, len, 1, stdout);
    free(buffer);

    caca_rotate_left(cv);
    buffer = caca_export_canvas_to_memory(cv, "utf8", &len);
    fwrite(buffer, len, 1, stdout);
    free(buffer);

    caca_free_canvas(pig);
    caca_free_canvas(cv);

    return 0;
}

