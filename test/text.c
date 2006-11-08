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
  "Hello world!\n" \
  "  _,----._  \n" \
  " (/ @  @ \\) \n" \
  "  |  OO  |  \n" \
  "  \\ `--' /  \n" \
  "   `----'   \n"

int main(int argc, char *argv[])
{
    cucul_canvas_t *cv;
    void *buffer;
    unsigned long int len;

    cv = cucul_create_canvas(0, 0);

    cucul_import_memory(cv, STRING, strlen(STRING), "text");

    buffer = cucul_export_memory(cv, "utf8", &len);
    fwrite(buffer, len, 1, stdout);
    free(buffer);

    cucul_rotate(cv);

    buffer = cucul_export_memory(cv, "utf8", &len);
    fwrite(buffer, len, 1, stdout);
    free(buffer);

    cucul_free_canvas(cv);

    return 0;
}

