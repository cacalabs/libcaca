/*
 *  libcaca       Colour ASCII-Art library
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

/*
 *  This file contains glue code for applications using the pre-1.0
 *  libcaca API.
 */

#include "config.h"
#include "common.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#endif

#include "caca.h"
#include "caca_internals.h"

/* These variables are needed to emulate old non-thread safe behaviour */
cucul_canvas_t *__caca0_cv = NULL;
caca_display_t *__caca0_dp = NULL;
unsigned char __caca0_fg;
unsigned char __caca0_bg;
char __caca0_utf8[2] = " ";

/* These functions are needed, too */
int __caca0_init(void);
void __caca0_end(void);
unsigned int __caca0_get_event(unsigned int, int);
int __caca0_get_feature(int);
void __caca0_set_feature(int);
char const *__caca0_get_feature_name(int);
cucul_canvas_t *__caca0_load_sprite(char const *);

/* Emulation functions */
int __caca0_init(void)
{
    __caca0_cv = cucul_create_canvas(0, 0);
    if(!__caca0_cv)
        return -1;
    __caca0_dp = caca_create_display(__caca0_cv);
    if(!__caca0_dp)
    {
        cucul_free_canvas(__caca0_cv);
        __caca0_cv = NULL;
        return -1;
    }
    __caca0_fg = CUCUL_COLOR_LIGHTGRAY;
    __caca0_bg = CUCUL_COLOR_BLACK;
    return 0;
}

void __caca0_end(void)
{
    caca_free_display(__caca0_dp);
    __caca0_dp = NULL;
    cucul_free_canvas(__caca0_cv);
    __caca0_cv = NULL;
}

unsigned int __caca0_get_event(unsigned int m, int t)
{
    caca_event_t ev;
    int ret;

    ret = caca_get_event(__caca0_dp, (m >> 24) & 0x7f, &ev, t);
    if(!ret)
        return 0x00000000;

    switch(ev.type)
    {
        case CACA_EVENT_KEY_PRESS:
            return 0x01000000 | ev.data.key.ch;
        case CACA_EVENT_KEY_RELEASE:
            return 0x02000000 | ev.data.key.ch;
        case CACA_EVENT_MOUSE_PRESS:
            return 0x04000000 | ev.data.mouse.button;
        case CACA_EVENT_MOUSE_RELEASE:
            return 0x08000000 | ev.data.mouse.button;
        case CACA_EVENT_MOUSE_MOTION:
            return 0x10000000 | ((ev.data.mouse.x & 0xfff) << 12)
                              | (ev.data.mouse.y & 0xfff);
        case CACA_EVENT_RESIZE:
            return 0x20000000;
        default:
            break;
    }

    return 0x00000000;
}

enum caca_feature
{
    CACA_BACKGROUND       = 0x10,
    CACA_BACKGROUND_BLACK = 0x11,
    CACA_BACKGROUND_SOLID = 0x12,
#define CACA_BACKGROUND_MIN 0x11
#define CACA_BACKGROUND_MAX 0x12
    CACA_ANTIALIASING           = 0x20,
    CACA_ANTIALIASING_NONE      = 0x21,
    CACA_ANTIALIASING_PREFILTER = 0x22,
#define CACA_ANTIALIASING_MIN     0x21
#define CACA_ANTIALIASING_MAX     0x22
    CACA_DITHERING          = 0x30,
    CACA_DITHERING_NONE     = 0x31,
    CACA_DITHERING_ORDERED2 = 0x32,
    CACA_DITHERING_ORDERED4 = 0x33,
    CACA_DITHERING_ORDERED8 = 0x34,
    CACA_DITHERING_RANDOM   = 0x35,
#define CACA_DITHERING_MIN    0x31
#define CACA_DITHERING_MAX    0x35
    CACA_FEATURE_UNKNOWN = 0xffff
};

int __caca0_get_feature(int feature)
{
    return feature;
}

void __caca0_set_feature(int feature)
{
    switch(feature)
    {
        case CACA_BACKGROUND:
            feature = CACA_BACKGROUND_SOLID;
        case CACA_BACKGROUND_BLACK:
        case CACA_BACKGROUND_SOLID:
            //_caca_background = feature;
            break;

        case CACA_ANTIALIASING:
            feature = CACA_ANTIALIASING_PREFILTER;
        case CACA_ANTIALIASING_NONE:
        case CACA_ANTIALIASING_PREFILTER:
            //_caca_antialiasing = feature;
            break;

        case CACA_DITHERING:
            feature = CACA_DITHERING_ORDERED4;
        case CACA_DITHERING_NONE:
        case CACA_DITHERING_ORDERED2:
        case CACA_DITHERING_ORDERED4:
        case CACA_DITHERING_ORDERED8:
        case CACA_DITHERING_RANDOM:
            //_caca_dithering = feature;
            break;
    }
}

char const *__caca0_get_feature_name(int feature)
{
    switch(feature)
    {
        case CACA_BACKGROUND_BLACK: return "black background";
        case CACA_BACKGROUND_SOLID: return "solid background";

        case CACA_ANTIALIASING_NONE:      return "no antialiasing";
        case CACA_ANTIALIASING_PREFILTER: return "prefilter antialiasing";

        case CACA_DITHERING_NONE:     return "no dithering";
        case CACA_DITHERING_ORDERED2: return "2x2 ordered dithering";
        case CACA_DITHERING_ORDERED4: return "4x4 ordered dithering";
        case CACA_DITHERING_ORDERED8: return "8x8 ordered dithering";
        case CACA_DITHERING_RANDOM:   return "random dithering";

        default: return "unknown";
    }
}

cucul_canvas_t *__caca0_load_sprite(char const *file)
{
    cucul_buffer_t *buf;
    cucul_canvas_t *cv;

    buf = cucul_load_file(file);
    if(!buf)
        return NULL;
    cv = cucul_import_canvas(buf, "");
    cucul_free_buffer(buf);
    if(!cv)
        return NULL;

    return cv;
}

