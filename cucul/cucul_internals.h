/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#ifndef __CUCUL_INTERNALS_H__
#define __CUCUL_INTERNALS_H__

#include "stubs.h"

typedef struct cucul_figfont cucul_figfont_t;

struct cucul_frame
{
    /* Frame size */
    int width, height;

    /* Cell information */
    uint32_t *chars;
    uint32_t *attrs;

    /* Painting context */
    int x, y;
    int handlex, handley;
    uint32_t curattr;

    /* Frame name */
    char *name;
};

struct cucul_canvas
{
    /* XXX: look at cucul_set_canvas_boundaries() before adding anything
     * to this structure. The function is quite hacky. */

    /* Frame information */
    int frame, framecount;
    struct cucul_frame *frames;

    /* Canvas management */
    int refcount;
    int autoinc;
    int (*resize_callback)(void *);
    void *resize_data;

    /* Shortcut to the active frame information */
    int width, height;
    uint32_t *chars;
    uint32_t *attrs;
    uint32_t curattr;

    /* FIGfont management */
    cucul_figfont_t *ff;
};

struct cucul_buffer
{
    size_t size;
    char *data;
    int user_data;
};

/* Colour functions */
extern uint32_t _cucul_attr_to_rgb24fg(uint32_t);
extern uint32_t _cucul_attr_to_rgb24bg(uint32_t);

/* Frames functions */
extern void _cucul_save_frame_info(cucul_canvas_t *);
extern void _cucul_load_frame_info(cucul_canvas_t *);

#endif /* __CUCUL_INTERNALS_H__ */
