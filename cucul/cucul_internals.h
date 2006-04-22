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
#elif !defined(CUSTOM_INTTYPES) && !defined(_DOXYGEN_SKIP_ME)
#   define CUSTOM_INTTYPES
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long int uint32_t;
typedef long int intptr_t;
typedef long unsigned int uintptr_t;
#endif

struct cucul_canvas
{
    /* Canvas size */
    unsigned int width, height;

    /* Shortcut to the active frame */
    uint32_t *chars;
    uint32_t *attr;

    /* Frame information */
    unsigned int frame, framecount;
    uint32_t **allchars;
    uint32_t **allattr;

    /* Painting context */
    uint16_t fgcolor;
    uint16_t bgcolor;

    unsigned int refcount;
};

struct cucul_buffer
{
    unsigned long int size;
    char *data;
};

/* Bitmap functions */
extern int _cucul_init_dither(void);
extern int _cucul_end_dither(void);

/* Canvas functions */
extern void _cucul_set_canvas_size(cucul_canvas_t *, unsigned int, unsigned int);
extern void _cucul_putchar32(cucul_canvas_t *, int, int, uint32_t);

/* Charset functions */
extern unsigned int _cucul_strlen_utf8(char const *);
extern char const *_cucul_skip_utf8(char const *, unsigned int);
extern uint32_t _cucul_utf8_to_utf32(char const *);
extern uint8_t _cucul_utf32_to_cp437(uint32_t);
extern uint32_t _cucul_cp437_to_utf32(uint8_t);

/* Colour functions */
uint8_t _cucul_argb32_to_ansi8(uint32_t);
uint8_t _cucul_argb32_to_ansi4fg(uint32_t);
uint8_t _cucul_argb32_to_ansi4bg(uint32_t);
uint16_t _cucul_argb32_to_rgb12fg(uint32_t);
uint16_t _cucul_argb32_to_rgb12bg(uint32_t);
uint32_t _cucul_argb32_to_rgb24fg(uint32_t);
uint32_t _cucul_argb32_to_rgb24bg(uint32_t);
void _cucul_argb32_to_argb4(uint32_t, uint8_t[8]);

#endif /* __CUCUL_INTERNALS_H__ */
