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
#   include <stdlib.h>
#endif

#include "caca.h"
#include "caca_internals.h"

/* These variables are needed to emulate old non-thread safe behaviour */
cucul_canvas_t *__caca0_cv = NULL;
caca_display_t *__caca0_dp = NULL;
unsigned char __caca0_fg = CUCUL_LIGHTGRAY;
unsigned char __caca0_bg = CUCUL_BLACK;
char __caca0_utf8[2] = " ";

/* These functions are needed, too */
int __caca0_init(void);
void __caca0_end(void);
unsigned int __caca0_get_event(unsigned int, int);
unsigned int __caca0_sqrt(unsigned int);
int __caca0_get_feature(int);
void __caca0_set_feature(int);
char const *__caca0_get_feature_name(int);
cucul_canvas_t *__caca0_load_sprite(char const *);
cucul_dither_t *__caca0_create_bitmap(unsigned int, unsigned int,
          unsigned int, unsigned int, unsigned long int, unsigned long int,
          unsigned long int, unsigned long int);
void __caca0_free_bitmap(cucul_dither_t *);

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
    __caca0_fg = CUCUL_LIGHTGRAY;
    __caca0_bg = CUCUL_BLACK;
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

unsigned int __caca0_sqrt(unsigned int a)
{
    if(a == 0)
        return 0;

    if(a < 1000000000)
    {
        unsigned int x = a < 10 ? 1
                       : a < 1000 ? 10
                       : a < 100000 ? 100
                       : a < 10000000 ? 1000
                       : 10000;

        /* Newton's method. Three iterations would be more than enough. */
        x = (x * x + a) / x / 2;
        x = (x * x + a) / x / 2;
        x = (x * x + a) / x / 2;
        x = (x * x + a) / x / 2;

        return x;
    }

    return 2 * __caca0_sqrt(a / 4);
}

static char const *features[] =
{
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

    NULL, "16", "full16", NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

    NULL, "none", "prefilter", NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

    NULL, "none", "ordered2", "ordered4", "ordered8", "random"
};

static cucul_dither_t **bitmaps = NULL;
static unsigned int nbitmaps = 0;

static int background = 0x12;
static int antialiasing = 0x22;
static int dithering = 0x33;

int __caca0_get_feature(int feature)
{
    if(feature == 0x10)
        return background;
    if(feature == 0x20)
        return antialiasing;
    if(feature == 0x30)
        return dithering;
    return 0xffff; /* CACA_FEATURE_UNKNOWN */
}

void __caca0_set_feature(int feature)
{
    unsigned int i;

    switch(feature)
    {
        case 0x10: feature = 0x12; /* CACA_BACKGROUND_SOLID */
        case 0x11: case 0x12:
            background = feature;
            for(i = 0; i < nbitmaps; i++)
                cucul_set_dither_color(bitmaps[i], features[feature]);
            break;

        case 0x20: feature = 0x22; /* CACA_ANTIALIASING_PREFILTER */
        case 0x21: case 0x22:
            antialiasing = feature;
            for(i = 0; i < nbitmaps; i++)
                cucul_set_dither_antialias(bitmaps[i], features[feature]);
            break;

        case 0x30: feature = 0x33; /* CACA_DITHERING_ORDERED4 */
        case 0x31: case 0x32: case 0x33: case 0x34: case 0x35:
            dithering = feature;
            for(i = 0; i < nbitmaps; i++)
                cucul_set_dither_mode(bitmaps[i], features[feature]);
            break;
    }
}

char const *__caca0_get_feature_name(int feature)
{
    switch(feature)
    {
        case 0x11: return "black background";
        case 0x12: return "solid background";

        case 0x21: return "no antialiasing";
        case 0x22: return "prefilter antialiasing";

        case 0x31: return "no dithering";
        case 0x32: return "2x2 ordered dithering";
        case 0x33: return "4x4 ordered dithering";
        case 0x34: return "8x8 ordered dithering";
        case 0x35: return "random dithering";

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

cucul_dither_t *__caca0_create_bitmap(unsigned int bpp, unsigned int w,
                                      unsigned int h, unsigned int pitch,
                                      unsigned long int r, unsigned long int g,
                                      unsigned long int b, unsigned long int a)
{
    cucul_dither_t *d;

    d = cucul_create_dither(bpp, w, h, pitch, r, g, b, a);
    if(!d)
        return NULL;

    cucul_set_dither_color(d, features[background]);
    cucul_set_dither_antialias(d, features[antialiasing]);
    cucul_set_dither_mode(d, features[dithering]);

    /* Store bitmap in our list */
    nbitmaps++;
    bitmaps = realloc(bitmaps, nbitmaps * (sizeof(cucul_dither_t *)));
    bitmaps[nbitmaps - 1] = d;

    return d;
}

void __caca0_free_bitmap(cucul_dither_t *d)
{
    unsigned int i, found = 0;

    cucul_free_dither(d);

    /* Remove bitmap from our list */
    for(i = 0; i + 1 < nbitmaps; i++)
    {
        if(bitmaps[i] == d)
            found = 1;
        if(found)
            bitmaps[i] = bitmaps[i + 1];
    }

    nbitmaps--;
}

char const *__caca0_get_color_name(unsigned char color)
{
    static char const *color_names[] =
    {
        "black",
        "blue",
        "green",
        "cyan",
        "red",
        "magenta",
        "brown",
        "light gray",
        "dark gray",
        "light blue",
        "light green",
        "light cyan",
        "light red",
        "light magenta",
        "yellow",
        "white",
    };

    if(color > 15)
        return "unknown";

    return color_names[(unsigned int)color];
}

