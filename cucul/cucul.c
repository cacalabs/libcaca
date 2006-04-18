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

/*
 *  This file contains the main functions used by \e libcucul applications
 *  to initialise a drawing context.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

/** \brief Initialise a \e libcucul canvas.
 *
 *  This function initialises internal \e libcucul structures and the backend
 *  that will be used for subsequent graphical operations. It must be the
 *  first \e libcucul function to be called in a function. cucul_free_canvas()
 *  should be called at the end of the program to free all allocated resources.
 *
 *  If one of the desired canvas coordinates is zero, a default canvas size
 *  of 80x32 is used instead.
 *
 *  \param width The desired canvas width
 *  \param height The desired canvas height
 *  \return A libcucul canvas handle upon success, NULL if an error occurred.
 */
cucul_canvas_t * cucul_create_canvas(unsigned int width, unsigned int height)
{
    cucul_canvas_t *cv = malloc(sizeof(cucul_canvas_t));

    cv->refcount = 0;

    cv->fgcolor = CUCUL_COLOR_LIGHTGRAY;
    cv->bgcolor = CUCUL_COLOR_BLACK;

    cv->width = cv->height = 0;
    cv->chars = NULL;
    cv->attr = NULL;
    cv->empty_line = cv->scratch_line = NULL;

    /* Initialise to a default size. 80x32 is arbitrary but matches AAlib's
     * default X11 window. When a graphic driver attaches to us, it can set
     * a different size. */
    if(width && height)
        _cucul_set_canvas_size(cv, width, height);
    else
        _cucul_set_canvas_size(cv, 80, 32);

    if(_cucul_init_dither())
    {
        free(cv);
        return NULL;
    }

    return cv;
}

/** \brief Load a memory area into a canvas.
 *
 *  This function loads a memory area containing an exported canvas into
 *  a new \e libcucul canvas.
 *
 *  \param data The memory area to be loaded into a canvas.
 *  \param size The length of the memory area.
 *  \return A libcucul canvas, or NULL in case of error.
 */
cucul_canvas_t *cucul_load_canvas(void *data, unsigned int size)
{
    cucul_canvas_t *cv;
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

    if(size != 12 + width * height * 8 + 4)
        return NULL;

    if(buf[size - 4] != 'A' || buf[size - 3] != 'C'
        || buf[size - 2] != 'A' || buf[size - 1] != 'C')
        return NULL;

    cv = cucul_create_canvas(width, height);

    if(!cv)
        return NULL;

    for(n = height * width; n--; )
    {
        cv->chars[n] = ((uint32_t)buf[12 + 8 * n] << 24)
                     | ((uint32_t)buf[13 + 8 * n] << 16)
                     | ((uint32_t)buf[14 + 8 * n] << 8)
                     | (uint32_t)buf[15 + 8 * n];
        cv->attr[n] = ((uint32_t)buf[16 + 8 * n] << 24)
                    | ((uint32_t)buf[17 + 8 * n] << 16)
                    | ((uint32_t)buf[18 + 8 * n] << 8)
                    | (uint32_t)buf[19 + 8 * n];
    }

    return cv;
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
 *  \param cv A libcucul canvas
 *  \param width The desired canvas width
 *  \param height The desired canvas height
 */
void cucul_set_canvas_size(cucul_canvas_t *cv, unsigned int width,
                                               unsigned int height)
{
    if(cv->refcount)
        return;

    _cucul_set_canvas_size(cv, width, height);
}

/** \brief Get the canvas width.
 *
 *  This function returns the current canvas width, in character cells.
 *
 *  \param cv A libcucul canvas
 *  \return The canvas width.
 */
unsigned int cucul_get_canvas_width(cucul_canvas_t *cv)
{
    return cv->width;
}

/** \brief Get the canvas height.
 *
 *  This function returns the current canvas height, in character cells.
 *
 *  \param cv A libcucul canvas
 *  \return The canvas height.
 */
unsigned int cucul_get_canvas_height(cucul_canvas_t *cv)
{
    return cv->height;
}

/** \brief Translate a colour index into the colour's name.
 *
 *  This function translates a cucul_color enum into a human-readable
 *  description string of the associated colour.
 *
 *  \param color The colour value.
 *  \return A static string containing the colour's name.
 */
char const *cucul_get_color_name(unsigned int color)
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

/** \brief Uninitialise \e libcucul.
 *
 *  This function frees all resources allocated by cucul_create_canvas(). After
 *  cucul_free_canvas() has been called, no other \e libcucul functions may be
 *  used unless a new call to cucul_create_canvas() is done.
 *
 *  \param cv A libcucul canvas
 */
void cucul_free_canvas(cucul_canvas_t *cv)
{
    _cucul_end_dither();

    free(cv->empty_line);
    free(cv->scratch_line);

    free(cv->chars);
    free(cv->attr);

    free(cv);
}

/** \brief Generate a random integer within a range.
 *
 *  \param min The lower bound of the integer range.
 *  \param max The upper bound of the integer range.
 *  \return A random integer comprised between \p min  and \p max - 1
 *  (inclusive).
 */
int cucul_rand(int min, int max)
{
    return min + (int)((1.0 * (max - min)) * rand() / (RAND_MAX + 1.0));
}

/*
 * XXX: The following functions are local.
 */

void _cucul_set_canvas_size(cucul_canvas_t *cv, unsigned int width,
                                                unsigned int height)
{
    unsigned int x, y, old_width, old_height, new_size, old_size;

    old_width = cv->width;
    old_height = cv->height;
    old_size = old_width * old_height;

    cv->width = width;
    cv->height = height;
    new_size = width * height;

    /* Step 1: if new area is bigger, resize the memory area now. */
    if(new_size > old_size)
    {
        cv->chars = realloc(cv->chars, new_size * sizeof(uint32_t));
        cv->attr = realloc(cv->attr, new_size * sizeof(uint32_t));
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
                cv->chars[y * width + x] = cv->chars[y * old_width + x];
                cv->attr[y * width + x] = cv->attr[y * old_width + x];
            }

            /* Zero the end of the line */
            for(x = width - old_width; x--; )
                cv->chars[y * width + old_width + x] = (uint32_t)' ';
            memset(cv->attr + y * width + old_width, 0,
                   (width - old_width) * 4);
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
                cv->chars[y * width + x] = cv->chars[y * old_width + x];
                cv->attr[y * width + x] = cv->attr[y * old_width + x];
            }
        }
    }

    /* Step 3: fill the bottom of the new screen if necessary. */
    if(height > old_height)
    {
        /* Zero the bottom of the screen */
        for(x = (height - old_height) * width; x--; )
            cv->chars[old_height * width + x] = (uint32_t)' ';
        memset(cv->attr + old_height * width, 0,
               (height - old_height) * width * 4);
    }

    /* Step 4: if new area is smaller, resize memory area now. */
    if(new_size <= old_size)
    {
        cv->chars = realloc(cv->chars, new_size * sizeof(uint32_t));
        cv->attr = realloc(cv->attr, new_size * sizeof(uint32_t));
    }

    /* Recompute the scratch line and the empty line */
    if(width != old_width)
    {
        cv->empty_line = realloc(cv->empty_line, width + 1);
        memset(cv->empty_line, ' ', width);
        cv->empty_line[width] = '\0';

        cv->scratch_line = realloc(cv->scratch_line, width + 1);
    }
}

