/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#ifndef __CUCUL_INTERNALS_H__
#define __CUCUL_INTERNALS_H__

#if defined(HAVE_INTTYPES_H) && !defined(__KERNEL__)
#   include <inttypes.h>
#endif

struct cucul_canvas
{
    /* Canvas size */
    unsigned int width, height;

    /* Shortcut to the active frame */
    uint32_t *chars;
    uint32_t *attrs;

    /* Frame information */
    unsigned int frame, framecount;
    uint32_t **allchars;
    uint32_t **allattrs;

    /* Painting context */
    uint32_t curattr;

    unsigned int refcount;
};

struct cucul_buffer
{
    unsigned long int size;
    char *data;
    int user_data;
};

/* Canvas functions */
extern int _cucul_set_canvas_size(cucul_canvas_t *, unsigned int, unsigned int);

/* Colour functions */
extern uint16_t _cucul_attr_to_rgb12fg(uint32_t);
extern uint16_t _cucul_attr_to_rgb12bg(uint32_t);
extern uint32_t _cucul_attr_to_rgb24fg(uint32_t);
extern uint32_t _cucul_attr_to_rgb24bg(uint32_t);
extern void _cucul_attr_to_argb4(uint32_t, uint8_t[8]);

#endif /* __CUCUL_INTERNALS_H__ */
