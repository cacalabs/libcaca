/*
 *  Imaging tools for cacaview and img2irc
 *  Copyright © 2003—2018 Sam Hocevar <sam@hocevar.net>
 *              All Rights Reserved
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by the WTFPL Task Force.
 *  See http://www.wtfpl.net/ for more details.
 */

struct image
{
    char *pixels;
    size_t w, h;
    struct caca_dither *dither;
    void *priv;
};

/* Local functions */
extern struct image * load_image(char const *);
extern void unload_image(struct image *);

