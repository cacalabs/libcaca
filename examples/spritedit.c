/*
 *  spritedit     sprite editor for libcaca
 *  Copyright (c) 2003 Sam Hocevar <sam@zoy.org>
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
#   include <stdio.h>
#   include <string.h>
#   include <stdlib.h>
#endif

#include "cucul.h"
#include "caca.h"

/* Courtesy of Zashi */
char *guy[] = {
  "  O_,= \n"
  "  |    \n"
  "  /\\   \n"
  " / /   \n",

  "  O_,= \n"
  "  |    \n"
  "  /|   \n"
  " / |   \n",

  "  O_,= \n"
  "  |    \n"
  "  |\\   \n"
  "  |/   \n",

  "  O_,= \n"
  "  |    \n"
  "  |\\   \n"
  "  | \\  \n",
};

int main(int argc, char **argv)
{
    cucul_canvas_t *sprite;
    unsigned long int len;
    void *buffer;
    int i;

    /* Create a canvas with 4 frames */
    sprite = cucul_create_canvas(0, 0);
    for(i = 0; i < 3; i++)
        cucul_create_frame(sprite, 0);

    /* Load stuff in all 4 frames */
    for(i = 0; i < 4; i++)
    {
        cucul_set_frame(sprite, i);
        cucul_import_memory(sprite, guy[i], strlen(guy[i]), "utf8");
    }

    /* Export our sprite in a memory buffer. We could save this to
     * disk afterwards. */
    buffer = cucul_export_memory(sprite, "caca", &len);

    /* Free our sprite and reload it from the memory buffer. We could
     * load this from disk, too. */
    cucul_free_canvas(sprite);
    sprite = cucul_create_canvas(0, 0);
    cucul_import_memory(sprite, buffer, len, "caca");
    free(buffer);

    /* Print each sprite frame to stdout */
    for(i = 0; i < 4; i++)
    {
        cucul_set_frame(sprite, i);
        printf("Frame #%i\n", i);
        buffer = cucul_export_memory(sprite, "utf8", &len);
        fwrite(buffer, len, 1, stdout);
        free(buffer);
    }

    /* Free our sprite */
    cucul_free_canvas(sprite);

    return 0;
}

