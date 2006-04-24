/*
 *  text          canvas text import/export
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
#include <string.h>

#include "cucul.h"

#define STRING \
  "Hello world!\n" \
  "  _,----._  \n" \
  " (/ @  @ \\) \n" \
  "  |  OO  |  \n" \
  "  \\ `--' /  \n" \
  "   `----'   \n"

int main(void)
{
    cucul_canvas_t *cv;
    cucul_buffer_t *buffer;

    cv = cucul_import_canvas(STRING, strlen(STRING), "text");

    buffer = cucul_export_canvas(cv, "ansi");
    fwrite(cucul_get_buffer_data(buffer),
           cucul_get_buffer_size(buffer), 1, stdout);
    cucul_free_buffer(buffer);

    cucul_rotate(cv);

    buffer = cucul_export_canvas(cv, "ansi");
    fwrite(cucul_get_buffer_data(buffer),
           cucul_get_buffer_size(buffer), 1, stdout);
    cucul_free_buffer(buffer);

    cucul_free_canvas(cv);

    return 0;
}

