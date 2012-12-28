/*
 *  spritedit     sprite editor for libcaca
 *  Copyright (c) 2003-2012 Sam Hocevar <sam@hocevar.net>
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
    caca_canvas_t *sprite;
    size_t len;
    void *buffer;
    int i;

    /* Create a canvas with 4 frames */
    sprite = caca_create_canvas(0, 0);
    for(i = 0; i < 3; i++)
        caca_create_frame(sprite, 0);

    /* Load stuff in all 4 frames */
    for(i = 0; i < 4; i++)
    {
        caca_set_frame(sprite, i);
        caca_import_canvas_from_memory(sprite, guy[i], strlen(guy[i]), "utf8");
    }

    /* Export our sprite in a memory buffer. We could save this to
     * disk afterwards. */
    buffer = caca_export_canvas_to_memory(sprite, "caca", &len);

    /* Free our sprite and reload it from the memory buffer. We could
     * load this from disk, too. */
    caca_free_canvas(sprite);
    sprite = caca_create_canvas(0, 0);
    caca_import_canvas_from_memory(sprite, buffer, len, "caca");
    free(buffer);

    /* Print each sprite frame to stdout */
    for(i = 0; i < 4; i++)
    {
        caca_set_frame(sprite, i);
        printf("Frame #%i\n", i);
        buffer = caca_export_canvas_to_memory(sprite, "utf8", &len);
        fwrite(buffer, len, 1, stdout);
        free(buffer);
    }

    /* Free our sprite */
    caca_free_canvas(sprite);

    return 0;
}

