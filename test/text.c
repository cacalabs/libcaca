/*
 *  text          canvas text import/export
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
#   include <string.h>
#   include <stdlib.h>
#endif
#include "cucul.h"

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
    cucul_canvas_t *cv, *pig;
    void *buffer;
    unsigned long int len;
    unsigned int i, j;

    pig = cucul_create_canvas(0, 0);
    cucul_import_memory(pig, STRING, strlen(STRING), "text");

    cv = cucul_create_canvas(cucul_get_canvas_width(pig) * 2,
                             cucul_get_canvas_height(pig) * 2);

    if(cv == NULL || pig == NULL)
    {
        printf("Can't created canvas\n");
        return -1;
    }

    cucul_blit(cv, 0, 0, pig, NULL);
    cucul_flip(pig);
    cucul_blit(cv, cucul_get_canvas_width(pig), 0, pig, NULL);
    cucul_flip(pig);
    cucul_flop(pig);
    cucul_blit(cv, 0, cucul_get_canvas_height(pig), pig, NULL);
    cucul_flop(pig);
    cucul_rotate_180(pig);
    cucul_blit(cv, cucul_get_canvas_width(pig),
                   cucul_get_canvas_height(pig), pig, NULL);

    for(j = 0; j < cucul_get_canvas_height(cv); j++)
    {
        for(i = 0; i < cucul_get_canvas_width(cv); i += 2)
        {
            unsigned long int a;
            cucul_set_color_ansi(cv, CUCUL_LIGHTBLUE + (i + j) % 6,
                                 CUCUL_DEFAULT);
            a = cucul_get_attr(cv, -1, -1);
            cucul_put_attr(cv, i, j, a);
            cucul_put_attr(cv, i + 1, j, a);
        }
    }

    buffer = cucul_export_memory(cv, "utf8", &len);
    fwrite(buffer, len, 1, stdout);
    free(buffer);

    cucul_rotate_left_wide(cv);
    buffer = cucul_export_memory(cv, "utf8", &len);
    fwrite(buffer, len, 1, stdout);
    free(buffer);

    cucul_free_canvas(pig);
    cucul_free_canvas(cv);

    return 0;
}

