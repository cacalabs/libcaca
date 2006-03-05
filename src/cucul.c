/*
 *  libcucul      Unicode canvas library
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file cucul.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Main \e libcucul functions
 *
 *  This file contains the main functions used by \e libcucul applications
 *  to initialise a drawing context.
 */

#include "config.h"

#if defined(HAVE_INTTYPES_H) || defined(_DOXYGEN_SKIP_ME)
#   include <inttypes.h>
#else
typedef unsigned char uint8_t;
#endif

#include <stdlib.h>
#include <string.h>

#include "cucul.h"
#include "cucul_internals.h"

static void cucul_init_features(cucul_t *qq);

/** \brief Initialise \e libcucul.
 *
 *  This function initialises internal \e libcucul structures and the backend
 *  that will be used for subsequent graphical operations. It must be the
 *  first \e libcucul function to be called in a function. cucul_end() should
 *  be called at the end of the program to free all allocated resources.
 *
 *  \return 0 upon success, a non-zero value if an error occurs.
 */
cucul_t * cucul_init(void)
{
    cucul_t *qq = malloc(sizeof(cucul_t));

    cucul_init_features(qq);

    qq->fgcolor = CUCUL_COLOR_LIGHTGRAY;
    qq->bgcolor = CUCUL_COLOR_BLACK;
#if defined(OPTIMISE_SLANG_PALETTE)
    qq->fgisbg = 0;
#endif

    /* Initialise to a default size. If a graphic driver attaches to
     * us before cucul_set_size is called, we'll adapt. */
    qq->width = 80;
    qq->height = 32;
    qq->size_set = 0;

    qq->chars = malloc(qq->width * qq->height * sizeof(uint8_t));
    qq->attr = malloc(qq->width * qq->height * sizeof(uint8_t));

    memset(qq->chars, 0, qq->width * qq->height * sizeof(uint8_t));
    memset(qq->attr, 0, qq->width * qq->height * sizeof(uint8_t));

    qq->empty_line = malloc(qq->width + 1);
    memset(qq->empty_line, ' ', qq->width);
    qq->empty_line[qq->width] = '\0';

    qq->scratch_line = malloc(qq->width + 1);

    if(_cucul_init_bitmap())
    {
        free(qq);
        return NULL;
    }

    return qq;
}

/** \brief Set the screen size.
 *
 *  This function sets the screen width and height, in character cells.
 *
 *  \param width The desired screen width
 *  \param height The desired screen height
 */
void cucul_set_size(cucul_t *qq, unsigned int width, unsigned int height)
{
    qq->width = width;
    qq->height = height;
    qq->size_set = 1;
}

/** \brief Get the screen width.
 *
 *  This function returns the current screen width, in character cells.
 *
 *  \return The screen width.
 */
unsigned int cucul_get_width(cucul_t *qq)
{
    return qq->width;
}

/** \brief Get the screen height.
 *
 *  This function returns the current screen height, in character cells.
 *
 *  \return The screen height.
 */
unsigned int cucul_get_height(cucul_t *qq)
{
    return qq->width;
}

/** \brief Translate a colour index into the colour's name.
 *
 *  This function translates a caca_color enum into a human-readable
 *  description string of the associated colour.
 *
 *  \param color The colour value.
 *  \return A static string containing the colour's name.
 */
char const *cucul_get_color_name(enum cucul_color color)
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

    if(color < 0 || color > 15)
        return "unknown";

    return color_names[color];
}

/** \brief Get the current value of a feature.
 *
 *  This function retrieves the value of an internal \e libcaca feature. A
 *  generic feature value is expected, such as CUCUL_ANTIALIASING.
 *
 *  \param feature The requested feature.
 *  \return The current value of the feature or CUCUL_FEATURE_UNKNOWN if an
 *          error occurred..
 */
enum cucul_feature cucul_get_feature(cucul_t *qq, enum cucul_feature feature)
{
    switch(feature)
    {
        case CUCUL_BACKGROUND:
            return qq->background;
        case CUCUL_ANTIALIASING:
            return qq->antialiasing;
        case CUCUL_DITHERING:
            return qq->dithering;

        default:
            return CUCUL_FEATURE_UNKNOWN;
    }
}

/** \brief Set a feature.
 *
 *  This function sets an internal \e libcaca feature such as the antialiasing
 *  or dithering modes. If a specific feature such as CUCUL_DITHERING_RANDOM,
 *  cucul_set_feature() will set it immediately. If a generic feature is given
 *  instead, such as CUCUL_DITHERING, the default value will be used instead.
 *
 *  \param feature The requested feature.
 */
void cucul_set_feature(cucul_t *qq, enum cucul_feature feature)
{
    switch(feature)
    {
        case CUCUL_BACKGROUND:
            feature = CUCUL_BACKGROUND_SOLID;
        case CUCUL_BACKGROUND_BLACK:
        case CUCUL_BACKGROUND_SOLID:
            qq->background = feature;
            break;

        case CUCUL_ANTIALIASING:
            feature = CUCUL_ANTIALIASING_PREFILTER;
        case CUCUL_ANTIALIASING_NONE:
        case CUCUL_ANTIALIASING_PREFILTER:
            qq->antialiasing = feature;
            break;

        case CUCUL_DITHERING:
            feature = CUCUL_DITHERING_FSTEIN;
        case CUCUL_DITHERING_NONE:
        case CUCUL_DITHERING_ORDERED2:
        case CUCUL_DITHERING_ORDERED4:
        case CUCUL_DITHERING_ORDERED8:
        case CUCUL_DITHERING_RANDOM:
        case CUCUL_DITHERING_FSTEIN:
            qq->dithering = feature;
            break;

        case CUCUL_FEATURE_UNKNOWN:
            break;
    }
}

/** \brief Translate a feature value into the feature's name.
 *
 *  This function translates a cucul_feature enum into a human-readable
 *  description string of the associated feature.
 *
 *  \param feature The feature value.
 *  \return A static string containing the feature's name.
 */
char const *cucul_get_feature_name(enum cucul_feature feature)
{
    switch(feature)
    {
        case CUCUL_BACKGROUND_BLACK: return "black background";
        case CUCUL_BACKGROUND_SOLID: return "solid background";

        case CUCUL_ANTIALIASING_NONE:      return "no antialiasing";
        case CUCUL_ANTIALIASING_PREFILTER: return "prefilter antialiasing";

        case CUCUL_DITHERING_NONE:     return "no dithering";
        case CUCUL_DITHERING_ORDERED2: return "2x2 ordered dithering";
        case CUCUL_DITHERING_ORDERED4: return "4x4 ordered dithering";
        case CUCUL_DITHERING_ORDERED8: return "8x8 ordered dithering";
        case CUCUL_DITHERING_RANDOM:   return "random dithering";
        case CUCUL_DITHERING_FSTEIN:   return "Floyd-Steinberg dithering";

        default: return "unknown";
    }
}

/** \brief Uninitialise \e libcucul.
 *
 *  This function frees all resources allocated by cucul_init(). After
 *  cucul_end() has been called, no other \e libcucul functions may be used
 *  unless a new call to cucul_init() is done.
 */
void cucul_end(cucul_t *qq)
{
    _cucul_end_bitmap();

    free(qq->empty_line);
    free(qq->scratch_line);

    free(qq->chars);
    free(qq->attr);

    free(qq);
}

/*
 * XXX: The following functions are local.
 */

static void cucul_init_features(cucul_t * qq)
{
    /* FIXME: if strcasecmp isn't available, use strcmp */
#if defined(HAVE_GETENV) && defined(HAVE_STRCASECMP)
    char *var;
#endif

    cucul_set_feature(qq, CUCUL_BACKGROUND);
    cucul_set_feature(qq, CUCUL_ANTIALIASING);
    cucul_set_feature(qq, CUCUL_DITHERING);

#if defined(HAVE_GETENV) && defined(HAVE_STRCASECMP)
    if((var = getenv("CUCUL_BACKGROUND")) && *var)
    {
        if(!strcasecmp("black", var))
            cucul_set_feature(qq, CUCUL_BACKGROUND_BLACK);
        else if(!strcasecmp("solid", var))
            cucul_set_feature(qq, CUCUL_BACKGROUND_SOLID);
    }

    if((var = getenv("CUCUL_ANTIALIASING")) && *var)
    {
        if(!strcasecmp("none", var))
            cucul_set_feature(qq, CUCUL_ANTIALIASING_NONE);
        else if(!strcasecmp("prefilter", var))
            cucul_set_feature(qq, CUCUL_ANTIALIASING_PREFILTER);
    }

    if((var = getenv("CUCUL_DITHERING")) && *var)
    {
        if(!strcasecmp("none", var))
            cucul_set_feature(qq, CUCUL_DITHERING_NONE);
        else if(!strcasecmp("ordered2", var))
            cucul_set_feature(qq, CUCUL_DITHERING_ORDERED2);
        else if(!strcasecmp("ordered4", var))
            cucul_set_feature(qq, CUCUL_DITHERING_ORDERED4);
        else if(!strcasecmp("ordered8", var))
            cucul_set_feature(qq, CUCUL_DITHERING_ORDERED8);
        else if(!strcasecmp("random", var))
            cucul_set_feature(qq, CUCUL_DITHERING_RANDOM);
        else if(!strcasecmp("fstein", var))
            cucul_set_feature(qq, CUCUL_DITHERING_FSTEIN);
    }
#endif
}

