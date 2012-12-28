/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains glue code for applications using the pre-1.0
 *  libcaca API.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#endif

#include "caca0.h"

/* These variables are needed to emulate old non-thread safe behaviour */
caca_canvas_t *__caca0_cv = NULL;
caca_display_t *__caca0_dp = NULL;
unsigned char __caca0_fg = CACA_LIGHTGRAY;
unsigned char __caca0_bg = CACA_BLACK;
char __caca0_utf8[2] = " ";

/* These functions are needed, too */
int __caca0_init(void);
void __caca0_end(void);
unsigned int __caca0_get_event(unsigned int, int);
unsigned int __caca0_sqrt(unsigned int);
int __caca0_get_feature(int);
void __caca0_set_feature(int);
char const *__caca0_get_feature_name(int);
caca_canvas_t *__caca0_load_sprite(char const *);
caca_dither_t *__caca0_create_bitmap(unsigned int, unsigned int,
          unsigned int, unsigned int, unsigned long int, unsigned long int,
          unsigned long int, unsigned long int);
void __caca0_free_bitmap(caca_dither_t *);
extern char const *__caca0_get_color_name(unsigned char);

/* Emulation functions */
int __caca0_init(void)
{
    __caca0_cv = caca_create_canvas(0, 0);
    if(!__caca0_cv)
        return -1;
    __caca0_dp = caca_create_display(__caca0_cv);
    if(!__caca0_dp)
    {
        caca_free_canvas(__caca0_cv);
        __caca0_cv = NULL;
        return -1;
    }
    __caca0_fg = CACA_LIGHTGRAY;
    __caca0_bg = CACA_BLACK;
    return 0;
}

void __caca0_end(void)
{
    caca_free_display(__caca0_dp);
    __caca0_dp = NULL;
    caca_free_canvas(__caca0_cv);
    __caca0_cv = NULL;
}

unsigned int __caca0_get_event(unsigned int m, int t)
{
    caca_event_t ev;
    int ret;

    ret = caca_get_event(__caca0_dp, (m >> 24) & 0x7f, &ev, t);
    if(!ret)
        return 0x00000000;

    switch(caca_get_event_type(&ev))
    {
        case CACA_EVENT_KEY_PRESS:
            return 0x01000000 | caca_get_event_key_ch(&ev);
        case CACA_EVENT_KEY_RELEASE:
            return 0x02000000 | caca_get_event_key_ch(&ev);
        case CACA_EVENT_MOUSE_PRESS:
            return 0x04000000 | caca_get_event_mouse_button(&ev);
        case CACA_EVENT_MOUSE_RELEASE:
            return 0x08000000 | caca_get_event_mouse_button(&ev);
        case CACA_EVENT_MOUSE_MOTION:
            return 0x10000000 | ((caca_get_event_mouse_x(&ev) & 0xfff) << 12)
                              | (caca_get_event_mouse_y(&ev) & 0xfff);
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

static caca_dither_t **bitmaps = NULL;
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
                caca_set_dither_color(bitmaps[i], features[feature]);
            break;

        case 0x20: feature = 0x22; /* CACA_ANTIALIASING_PREFILTER */
        case 0x21: case 0x22:
            antialiasing = feature;
            for(i = 0; i < nbitmaps; i++)
                caca_set_dither_antialias(bitmaps[i], features[feature]);
            break;

        case 0x30: feature = 0x33; /* CACA_DITHERING_ORDERED4 */
        case 0x31: case 0x32: case 0x33: case 0x34: case 0x35:
            dithering = feature;
            for(i = 0; i < nbitmaps; i++)
                caca_set_dither_algorithm(bitmaps[i], features[feature]);
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

caca_canvas_t *__caca0_load_sprite(char const *file)
{
    caca_canvas_t *cv;

    cv = caca_create_canvas(0, 0);;
    if(caca_import_canvas_from_file(cv, file, "") < 0)
    {
        caca_free_canvas(cv);
        return NULL;
    }

    return cv;
}

caca_dither_t *__caca0_create_bitmap(unsigned int bpp, unsigned int w,
                                      unsigned int h, unsigned int pitch,
                                      unsigned long int r, unsigned long int g,
                                      unsigned long int b, unsigned long int a)
{
    caca_dither_t *d;

    d = caca_create_dither(bpp, w, h, pitch, r, g, b, a);
    if(!d)
        return NULL;

    caca_set_dither_color(d, features[background]);
    caca_set_dither_antialias(d, features[antialiasing]);
    caca_set_dither_algorithm(d, features[dithering]);

    /* Store bitmap in our list */
    nbitmaps++;
    bitmaps = realloc(bitmaps, nbitmaps * (sizeof(caca_dither_t *)));
    bitmaps[nbitmaps - 1] = d;

    return d;
}

void __caca0_free_bitmap(caca_dither_t *d)
{
    unsigned int i, found = 0;

    caca_free_dither(d);

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

