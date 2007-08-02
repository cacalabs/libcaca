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

/*
 *  This file contains the main functions used by \e libcucul applications
 *  to initialise a drawing context.
 */

#include "config.h"
#include "common.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#   include <time.h>
#   include <sys/types.h>
#   if defined(HAVE_UNISTD_H)
#       include <unistd.h>
#   endif
#endif

#include "cucul.h"
#include "cucul_internals.h"

/** \brief Initialise a \e libcucul canvas.
 *
 *  Initialise internal \e libcucul structures and the backend that will
 *  be used for subsequent graphical operations. It must be the first
 *  \e libcucul function to be called in a function. cucul_free_canvas()
 *  should be called at the end of the program to free all allocated resources.
 *
 *  Both the cursor and the canvas' handle are initialised at the top-left
 *  corner.
 *
 *  If an error occurs, NULL is returned and \b errno is set accordingly:
 *  - \c ENOMEM Not enough memory for the requested canvas size.
 *
 *  \param width The desired canvas width
 *  \param height The desired canvas height
 *  \return A libcucul canvas handle upon success, NULL if an error occurred.
 */
cucul_canvas_t * cucul_create_canvas(unsigned int width, unsigned int height)
{
    cucul_canvas_t *cv = malloc(sizeof(cucul_canvas_t));

    if(!cv)
        goto nomem;

    cv->refcount = 0;
    cv->autoinc = 0;

    cv->frame = 0;
    cv->framecount = 1;
    cv->frames = malloc(sizeof(struct cucul_frame));
    if(!cv->frames)
    {
        free(cv);
        goto nomem;
    }

    cv->frames[0].width = cv->frames[0].height = 0;
    cv->frames[0].chars = NULL;
    cv->frames[0].attrs = NULL;
    cv->frames[0].x = cv->frames[0].y = 0;
    cv->frames[0].handlex = cv->frames[0].handley = 0;
    cv->frames[0].curattr = 0;
    cv->frames[0].name = strdup("frame#00000000");

    _cucul_load_frame_info(cv);
    cucul_set_color_ansi(cv, CUCUL_DEFAULT, CUCUL_TRANSPARENT);

    if(_cucul_set_canvas_size(cv, width, height) < 0)
    {
        int saved_errno = geterrno();
        free(cv->frames[0].name);
        free(cv->frames);
        free(cv);
        seterrno(saved_errno);
        return NULL;
    }

    return cv;

nomem:
    seterrno(ENOMEM);
    return NULL;
}

/** \brief Resize a canvas.
 *
 *  Set the canvas' width and height, in character cells.
 *
 *  The contents of the canvas are preserved to the extent of the new
 *  canvas size. Newly allocated character cells at the right and/or at
 *  the bottom of the canvas are filled with spaces.
 *
 *  If as a result of the resize the cursor coordinates fall outside the
 *  new canvas boundaries, they are readjusted. For instance, if the
 *  current X cursor coordinate is 11 and the requested width is 10, the
 *  new X cursor coordinate will be 10.
 *
 *  It is an error to try to resize the canvas if an output driver has
 *  been attached to the canvas using caca_create_display(). You need to
 *  remove the output driver using caca_free_display() before you can change
 *  the canvas size again. However, the caca output driver can cause a
 *  canvas resize through user interaction. See the caca_event() documentation
 *  for more about this.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EBUSY The canvas is in use by a display driver and cannot be resized.
 *  - \c ENOMEM Not enough memory for the requested canvas size. If this
 *    happens, the canvas handle becomes invalid and should not be used.
 *
 *  \param cv A libcucul canvas
 *  \param width The desired canvas width
 *  \param height The desired canvas height
 *  \return 0 in case of success, -1 if an error occurred.
 */
int cucul_set_canvas_size(cucul_canvas_t *cv, unsigned int width,
                                              unsigned int height)
{
    if(cv->refcount)
    {
        seterrno(EBUSY);
        return -1;
    }

    return _cucul_set_canvas_size(cv, width, height);
}

/** \brief Get the canvas width.
 *
 *  Return the current canvas' width, in character cells.
 *
 *  This function never fails.
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
 *  Returns the current canvas' height, in character cells.
 *
 *  This function never fails.
 *
 *  \param cv A libcucul canvas
 *  \return The canvas height.
 */
unsigned int cucul_get_canvas_height(cucul_canvas_t *cv)
{
    return cv->height;
}

/** \brief Uninitialise \e libcucul.
 *
 *  Free all resources allocated by cucul_create_canvas(). After
 *  this function has been called, no other \e libcucul functions may be
 *  used unless a new call to cucul_create_canvas() is done.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EBUSY The canvas is in use by a display driver and cannot be freed.
 *
 *  \param cv A libcucul canvas
 *  \return 0 in case of success, -1 if an error occurred.
 */
int cucul_free_canvas(cucul_canvas_t *cv)
{
    unsigned int f;

    if(cv->refcount)
    {
        seterrno(EBUSY);
        return -1;
    }

    for(f = 0; f < cv->framecount; f++)
    {
        free(cv->frames[f].chars);
        free(cv->frames[f].attrs);
        free(cv->frames[f].name);
    }

    free(cv->frames);
    free(cv);

    return 0;
}

/** \brief Generate a random integer within a range.
 *
 *  Generate a random integer within the given range.
 *
 *  This function never fails.
 *
 *  \param min The lower bound of the integer range.
 *  \param max The upper bound of the integer range.
 *  \return A random integer comprised between \p min  and \p max - 1
 *  (inclusive).
 */
int cucul_rand(int min, int max)
{
    static int need_init = 1;

    if(need_init)
    {
        srand(getpid() + time(NULL));
        need_init = 0;
    }

    return min + (int)((1.0 * (max - min)) * rand() / (RAND_MAX + 1.0));
}

/*
 * XXX: The following functions are local.
 */

int _cucul_set_canvas_size(cucul_canvas_t *cv, unsigned int width,
                                               unsigned int height)
{
    unsigned int x, y, f, old_width, old_height, new_size, old_size;

    old_width = cv->width;
    old_height = cv->height;
    old_size = old_width * old_height;

    _cucul_save_frame_info(cv);

    cv->width = width;
    cv->height = height;
    new_size = width * height;

    /* Step 1: if new area is bigger, resize the memory area now. */
    if(new_size > old_size)
    {
        for(f = 0; f < cv->framecount; f++)
        {
            cv->frames[f].chars = realloc(cv->frames[f].chars,
                                          new_size * sizeof(uint32_t));
            cv->frames[f].attrs = realloc(cv->frames[f].attrs,
                                          new_size * sizeof(uint32_t));
            if(new_size && (!cv->frames[f].chars || !cv->frames[f].attrs))
            {
                seterrno(ENOMEM);
                return -1;
            }
        }
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
        for(f = 0; f < cv->framecount; f++)
        {
            uint32_t *chars = cv->frames[f].chars;
            uint32_t *attrs = cv->frames[f].attrs;

            for(y = height < old_height ? height : old_height; y--; )
            {
                uint32_t attr = cv->frames[f].curattr;

                for(x = old_width; x--; )
                {
                    chars[y * width + x] = chars[y * old_width + x];
                    attrs[y * width + x] = attrs[y * old_width + x];
                }

                /* Zero the end of the line */
                for(x = width - old_width; x--; )
                {
                    chars[y * width + old_width + x] = (uint32_t)' ';
                    attrs[y * width + old_width + x] = attr;
                }
            }
        }
    }
    else
    {
        /* New width is smaller. Copy as many lines as possible. Ignore
         * the first line, it is already in place. */
        unsigned int lines = height < old_height ? height : old_height;

        for(f = 0; f < cv->framecount; f++)
        {
            uint32_t *chars = cv->frames[f].chars;
            uint32_t *attrs = cv->frames[f].attrs;

            for(y = 1; y < lines; y++)
            {
                for(x = 0; x < width; x++)
                {
                    chars[y * width + x] = chars[y * old_width + x];
                    attrs[y * width + x] = attrs[y * old_width + x];
                }
            }
        }
    }

    /* Step 3: fill the bottom of the new screen if necessary. */
    if(height > old_height)
    {
        for(f = 0; f < cv->framecount; f++)
        {
            uint32_t *chars = cv->frames[f].chars;
            uint32_t *attrs = cv->frames[f].attrs;
            uint32_t attr = cv->frames[f].curattr;

            /* Zero the bottom of the screen */
            for(x = (height - old_height) * width; x--; )
            {
                chars[old_height * width + x] = (uint32_t)' ';
                attrs[old_height * width + x] = attr;
            }
        }
    }

    /* Step 4: if new area is smaller, resize memory area now. */
    if(new_size < old_size)
    {
        for(f = 0; f < cv->framecount; f++)
        {
            cv->frames[f].chars = realloc(cv->frames[f].chars,
                                          new_size * sizeof(uint32_t));
            cv->frames[f].attrs = realloc(cv->frames[f].attrs,
                                          new_size * sizeof(uint32_t));
            if(new_size && (!cv->frames[f].chars || !cv->frames[f].attrs))
            {
                seterrno(ENOMEM);
                return -1;
            }
        }
    }

    /* Set new size */
    for(f = 0; f < cv->framecount; f++)
    {
        if(cv->frames[f].x > (int)width)
            cv->frames[f].x = width;
        if(cv->frames[f].y > (int)height)
            cv->frames[f].y = height;

        cv->frames[f].width = width;
        cv->frames[f].height = height;
    }

    /* Reset the current frame shortcuts */
    _cucul_load_frame_info(cv);

    return 0;
}

