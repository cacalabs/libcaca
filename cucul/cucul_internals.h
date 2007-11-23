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

#if defined(HAVE_INTTYPES_H) && !defined(__KERNEL__)
#   include <inttypes.h>
#endif

struct cucul_frame
{
    /* Frame size */
    unsigned int width, height;

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
    /* Frame information */
    unsigned int frame, framecount;
    struct cucul_frame *frames;

    unsigned int refcount;
    unsigned int autoinc;

    /* Shortcut to the active frame information */
    unsigned int width, height;
    uint32_t *chars;
    uint32_t *attrs;
    uint32_t curattr;
};

struct cucul_buffer
{
    unsigned long int size;
    char *data;
    int user_data;
};

/* Canvas functions */
extern int __cucul_set_canvas_size(cucul_canvas_t *,
                                   unsigned int, unsigned int);

/* Colour functions */
extern uint16_t _cucul_attr_to_rgb12fg(uint32_t);
extern uint16_t _cucul_attr_to_rgb12bg(uint32_t);
extern uint32_t _cucul_attr_to_rgb24fg(uint32_t);
extern uint32_t _cucul_attr_to_rgb24bg(uint32_t);
extern void _cucul_attr_to_argb4(uint32_t, uint8_t[8]);

/* Frames functions */
extern void _cucul_save_frame_info(cucul_canvas_t *);
extern void _cucul_load_frame_info(cucul_canvas_t *);

#endif /* __CUCUL_INTERNALS_H__ */
