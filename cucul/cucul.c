/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
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

#if !defined(__KERNEL__)
#   include <stdlib.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

static void cucul_read_environment(cucul_t *);

/** \brief Initialise a \e libcucul canvas.
 *
 *  This function initialises internal \e libcucul structures and the backend
 *  that will be used for subsequent graphical operations. It must be the
 *  first \e libcucul function to be called in a function. cucul_free() should
 *  be called at the end of the program to free all allocated resources.
 *
 *  If one of the desired canvas coordinates is zero, a default canvas size
 *  of 80x32 is used instead.
 *
 *  \param width The desired canvas width
 *  \param height The desired canvas height
 *  \return 0 upon success, a non-zero value if an error occurs.
 */
cucul_t * cucul_create(unsigned int width, unsigned int height)
{
    cucul_t *qq = malloc(sizeof(cucul_t));

    cucul_read_environment(qq);

    qq->refcount = 0;

    qq->fgcolor = CUCUL_COLOR_LIGHTGRAY;
    qq->bgcolor = CUCUL_COLOR_BLACK;

    qq->width = qq->width = 0;
    qq->chars = NULL;
    qq->attr = NULL;
    qq->empty_line = qq->scratch_line = NULL;

    /* Initialise to a default size. 80x32 is arbitrary but matches AAlib's
     * default X11 window. When a graphic driver attaches to us, it can set
     * a different size. */
    if(width && height)
        _cucul_set_size(qq, width, height);
    else
        _cucul_set_size(qq, 80, 32);

    if(_cucul_init_bitmap())
    {
        free(qq);
        return NULL;
    }

    return qq;
}

cucul_t *cucul_load(void *data, unsigned int size)
{
    cucul_t *qq;
    uint8_t *buf = (uint8_t *)data;
    unsigned int width, height, n;

    if(size < 12)
        return NULL;

    if(buf[0] != 'C' || buf[1] != 'A' || buf[2] != 'C' || buf[3] != 'A')
        return NULL;

    width = ((uint32_t)buf[4] << 24) | ((uint32_t)buf[5] << 16)
          | ((uint32_t)buf[6] << 8) | (uint32_t)buf[7];
    height = ((uint32_t)buf[8] << 24) | ((uint32_t)buf[9] << 16)
           | ((uint32_t)buf[10] << 8) | (uint32_t)buf[11];

    if(!width || !height)
        return NULL;

    if(size != 12 + width * height * 5 + 4)
        return NULL;

    qq = cucul_create(width, height);

    if(!qq)
        return NULL;

    for(n = height * width; n--; )
    {
        qq->chars[n] = ((uint32_t)buf[12 + 5 * n] << 24)
                     | ((uint32_t)buf[13 + 5 * n] << 16)
                     | ((uint32_t)buf[14 + 5 * n] << 8)
                     | (uint32_t)buf[15 + 5 * n];
        qq->attr[n] = buf[16 + 5 * n];
    }

    return qq;
}

/** \brief Resize a canvas.
 *
 *  This function sets the canvas width and height, in character cells.
 *
 *  The contents of the canvas are preserved to the extent of the new
 *  canvas size. Newly allocated character cells at the right and/or at
 *  the bottom of the canvas are filled with spaces.
 *
 *  It is an error to try to resize the canvas if an output driver has
 *  been attached to the canvas using caca_attach(). You need to remove
 *  the output driver using caca_detach() before you can change the
 *  canvas size again. However, the caca output driver can cause a canvas
 *  resize through user interaction. See the caca_event() documentation
 *  for more about this.
 *
 *  \param width The desired canvas width
 *  \param height The desired canvas height
 */
void cucul_set_size(cucul_t *qq, unsigned int width, unsigned int height)
{
    if(qq->refcount)
        return;

    _cucul_set_size(qq, width, height);
}

/** \brief Get the canvas width.
 *
 *  This function returns the current canvas width, in character cells.
 *
 *  \return The canvas width.
 */
unsigned int cucul_get_width(cucul_t *qq)
{
    return qq->width;
}

/** \brief Get the canvas height.
 *
 *  This function returns the current canvas height, in character cells.
 *
 *  \return The canvas height.
 */
unsigned int cucul_get_height(cucul_t *qq)
{
    return qq->height;
}

/** \brief Translate a colour index into the colour's name.
 *
 *  This function translates a cucul_color enum into a human-readable
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
 *  This function retrieves the value of an internal \e libcucul feature. A
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
 *  This function sets an internal \e libcucul feature such as the antialiasing
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
 *  This function frees all resources allocated by cucul_create(). After
 *  cucul_free() has been called, no other \e libcucul functions may be used
 *  unless a new call to cucul_create() is done.
 */
void cucul_free(cucul_t *qq)
{
    _cucul_end_bitmap();

    free(qq->empty_line);
    free(qq->scratch_line);

    free(qq->chars);
    free(qq->attr);

    free(qq);
}

struct cucul_export * cucul_create_export(cucul_t *qq, char const *format)
{
    struct cucul_export *ex;

    ex = malloc(sizeof(struct cucul_export));

    if(!strcasecmp("ansi", format))
        _cucul_get_ansi(qq, ex);
    else if(!strcasecmp("html", format))
        _cucul_get_html(qq, ex);
    else if(!strcasecmp("html3", format))
        _cucul_get_html3(qq, ex);
    else if(!strcasecmp("irc", format))
        _cucul_get_irc(qq, ex);
    else if(!strcasecmp("ps", format))
        _cucul_get_ps(qq, ex);
    else if(!strcasecmp("svg", format))
        _cucul_get_svg(qq, ex);
    else
    {
        free(ex);
        return NULL;
    }

    return ex;
}

/**
 * \brief Get available export formats
 *
 * Return a list of available export formats. The list is a NULL-terminated
 * array of strings, interleaving a string containing the internal value for
 * the export format, to be used with \e cucul_export(), and a string
 * containing the natural language description for that export format.
 *
 * \return An array of strings.
 */
char const * const * cucul_get_export_list(void)
{
    static char const * const list[] =
    {
        "ansi", "ANSI",
        "html", "HTML",
        "html3", "backwards-compatible HTML",
        "irc", "IRC (mIRC colours)",
        "ps", "PostScript",
        "svg", "SVG",
        NULL, NULL
    };

    return list;
}

void cucul_free_export(struct cucul_export *ex)
{
    free(ex->buffer);
    free(ex);
}

/*
 * XXX: The following functions are local.
 */

static void cucul_read_environment(cucul_t * qq)
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

void _cucul_set_size(cucul_t *qq, unsigned int width, unsigned int height)
{
    unsigned int x, y, old_width, old_height, new_size, old_size;

    old_width = qq->width;
    old_height = qq->height;
    old_size = old_width * old_height;

    qq->width = width;
    qq->height = height;
    new_size = width * height;

    /* Step 1: if new area is bigger, resize the memory area now. */
    if(new_size > old_size)
    {
        qq->chars = realloc(qq->chars, new_size * sizeof(uint32_t));
        qq->attr = realloc(qq->attr, new_size * sizeof(uint8_t));
    }

    /* Step 2: move line data if necessary. */
    if(width == old_width)
    {
        /* Width did not change, which means we do not need to move data. */
        ;
    }
    else if(width > old_width)
    {
        /* New width is bigger than old width, which means we need to
         * copy lines starting from the bottom of the screen otherwise
         * we will overwrite information. */
        for(y = height < old_height ? height : old_height; y--; )
        {
            for(x = old_width; x--; )
            {
                qq->chars[y * width + x] = qq->chars[y * old_width + x];
                qq->attr[y * width + x] = qq->attr[y * old_width + x];
            }

            /* Zero the end of the line */
            for(x = width - old_width; x--; )
                qq->chars[y * width + old_width + x] = (uint32_t)' ';
            memset(qq->attr + y * width + old_width, 0,
                   width - old_width);
        }
    }
    else
    {
        /* New width is smaller. Copy as many lines as possible. Ignore
         * the first line, it is already in place. */
        unsigned int lines = height < old_height ? height : old_height;

        for(y = 1; y < lines; y++)
        {
            for(x = 0; x < width; x++)
            {
                qq->chars[y * width + x] = qq->chars[y * old_width + x];
                qq->attr[y * width + x] = qq->attr[y * old_width + x];
            }
        }
    }

    /* Step 3: fill the bottom of the new screen if necessary. */
    if(height > old_height)
    {
        /* Zero the bottom of the screen */
        for(x = (height - old_height) * width; x--; )
            qq->chars[old_height * width + x] = (uint32_t)' ';
        memset(qq->attr + old_height * width, 0,
               (height - old_height) * width);
    }

    /* Step 4: if new area is smaller, resize memory area now. */
    if(new_size <= old_size)
    {
        qq->chars = realloc(qq->chars, new_size * sizeof(uint32_t));
        qq->attr = realloc(qq->attr, new_size * sizeof(uint8_t));
    }

    /* Recompute the scratch line and the empty line */
    if(width != old_width)
    {
        qq->empty_line = realloc(qq->empty_line, width + 1);
        memset(qq->empty_line, ' ', width);
        qq->empty_line[width] = '\0';

        qq->scratch_line = realloc(qq->scratch_line, width + 1);
    }
}

