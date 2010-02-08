/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2009 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *  This file contains the main functions used by \e libcaca applications
 *  to initialise a drawing context.
 */

#include "config.h"

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

#include "caca.h"
#include "caca_internals.h"

static int caca_resize(caca_canvas_t *, int, int);

/** \brief Initialise a \e libcaca canvas.
 *
 *  Initialise internal \e libcaca structures and the backend that will
 *  be used for subsequent graphical operations. It must be the first
 *  \e libcaca function to be called in a function. caca_free_canvas()
 *  should be called at the end of the program to free all allocated resources.
 *
 *  Both the cursor and the canvas' handle are initialised at the top-left
 *  corner.
 *
 *  If an error occurs, NULL is returned and \b errno is set accordingly:
 *  - \c EINVAL Specified width or height is invalid.
 *  - \c ENOMEM Not enough memory for the requested canvas size.
 *
 *  \param width The desired canvas width
 *  \param height The desired canvas height
 *  \return A libcaca canvas handle upon success, NULL if an error occurred.
 */
caca_canvas_t * caca_create_canvas(int width, int height)
{
    caca_canvas_t *cv;

    if(width < 0 || height < 0)
    {
        seterrno(EINVAL);
        return NULL;
    }

    cv = malloc(sizeof(caca_canvas_t));

    if(!cv)
        goto nomem;

    cv->refcount = 0;
    cv->autoinc = 0;
    cv->resize_callback = NULL;
    cv->resize_data = NULL;

    cv->frame = 0;
    cv->framecount = 1;
    cv->frames = malloc(sizeof(struct caca_frame));
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

    _caca_load_frame_info(cv);
    caca_set_color_ansi(cv, CACA_DEFAULT, CACA_TRANSPARENT);

    cv->ndirty = 0;
    cv->dirty_disabled = 0;
    cv->ff = NULL;

    if(caca_resize(cv, width, height) < 0)
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

/** \brief Manage a canvas.
 *
 *  Lock a canvas to prevent it from being resized. If non-NULL,
 *  the \e callback function pointer will be called upon each
 *  \e caca_set_canvas_size call and if the returned value is zero, the
 *  canvas resize request will be denied.
 *
 *  This function is only useful for display drivers such as the \e libcaca
 *  library.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EBUSY The canvas is already being managed.
 *
 *  \param cv A libcaca canvas.
 *  \param callback An optional callback function pointer.
 *  \param p The argument to be passed to \e callback.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_manage_canvas(caca_canvas_t *cv, int (*callback)(void *), void *p)
{
    if(cv->refcount)
    {
        seterrno(EBUSY);
        return -1;
    }

    cv->resize_callback = callback;
    cv->resize_data = p;
    cv->refcount = 1;

    return 0;
}

/** \brief unmanage a canvas.
 *
 *  unlock a canvas previously locked by caca_manage_canvas(). for safety
 *  reasons, the callback and callback data arguments must be the same as for
 *  the caca_manage_canvas() call.
 *
 *  this function is only useful for display drivers such as the \e libcaca
 *  library.
 *
 *  if an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c einval the canvas is not managed, or the callback arguments do
 *              not match.
 *
 *  \param cv a libcaca canvas.
 *  \param callback the \e callback argument previously passed to
 *                  caca_manage_canvas().
 *  \param p the \e p argument previously passed to caca_manage_canvas().
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_unmanage_canvas(caca_canvas_t *cv, int (*callback)(void *), void *p)
{
    if(!cv->refcount
        || cv->resize_callback != callback || cv->resize_data != p)
    {
        seterrno(EINVAL);
        return -1;
    }

    cv->refcount = 0;

    return 0;
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
 *  - \c EINVAL Specified width or height is invalid.
 *  - \c EBUSY The canvas is in use by a display driver and cannot be resized.
 *  - \c ENOMEM Not enough memory for the requested canvas size. If this
 *    happens, the canvas handle becomes invalid and should not be used.
 *
 *  \param cv A libcaca canvas.
 *  \param width The desired canvas width.
 *  \param height The desired canvas height.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_set_canvas_size(caca_canvas_t *cv, int width, int height)
{
    if(width < 0 || height < 0)
    {
        seterrno(EINVAL);
        return -1;
    }

    if(cv->refcount && cv->resize_callback
        && !cv->resize_callback(cv->resize_data))
    {
        seterrno(EBUSY);
        return -1;
    }

    return caca_resize(cv, width, height);
}

/** \brief Get the canvas width.
 *
 *  Return the current canvas' width, in character cells.
 *
 *  This function never fails.
 *
 *  \param cv A libcaca canvas.
 *  \return The canvas width.
 */
int caca_get_canvas_width(caca_canvas_t const *cv)
{
    return cv->width;
}

/** \brief Get the canvas height.
 *
 *  Returns the current canvas' height, in character cells.
 *
 *  This function never fails.
 *
 *  \param cv A libcaca canvas.
 *  \return The canvas height.
 */
int caca_get_canvas_height(caca_canvas_t const *cv)
{
    return cv->height;
}

/** \brief Get the canvas character array.
 *
 *  Return the current canvas' internal character array. The array elements
 *  consist in native endian 32-bit Unicode values as returned by
 *  caca_get_char().
 *
 *  This function is probably only useful for \e libcaca 's internal display
 *  drivers.
 *
 *  This function never fails.
 *
 *  \param cv A libcaca canvas.
 *  \return The canvas character array.
 */
uint32_t const * caca_get_canvas_chars(caca_canvas_t const *cv)
{
    return (uint32_t const *)cv->chars;
}

/** \brief Get the canvas attribute array.
 *
 *  Returns the current canvas' internal attribute array. The array elements
 *  consist in native endian 32-bit attribute values as returned by
 *  caca_get_attr().
 *
 *  This function is probably only useful for \e libcaca 's internal display
 *  drivers.
 *
 *  This function never fails.
 *
 *  \param cv A libcaca canvas.
 *  \return The canvas attribute array.
 */
uint32_t const * caca_get_canvas_attrs(caca_canvas_t const *cv)
{
    return (uint32_t const *)cv->attrs;
}

/** \brief Free a \e libcaca canvas.
 *
 *  Free all resources allocated by caca_create_canvas(). The canvas
 *  pointer becomes invalid and must no longer be used unless a new call
 *  to caca_create_canvas() is made.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EBUSY The canvas is in use by a display driver and cannot be freed.
 *
 *  \param cv A libcaca canvas.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_free_canvas(caca_canvas_t *cv)
{
    int f;

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

    caca_canvas_set_figfont(cv, NULL);

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
int caca_rand(int min, int max)
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

int caca_resize(caca_canvas_t *cv, int width, int height)
{
    int x, y, f, old_width, old_height, new_size, old_size;

    old_width = cv->width;
    old_height = cv->height;
    old_size = old_width * old_height;

    _caca_save_frame_info(cv);

    /* Preload new width and height values into the canvas to optimise
     * dirty rectangle handling */
    cv->width = width;
    cv->height = height;
    new_size = width * height;

    /* If width or height is smaller (or both), we have the opportunity to
     * reduce or even remove dirty rectangles */
    if(width < old_width || height < old_height)
        _caca_clip_dirty_rect_list(cv);

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

        if(!cv->dirty_disabled)
            caca_add_dirty_rect(cv, old_width, 0,
                                width - old_width, old_height);
    }
    else
    {
        /* New width is smaller. Copy as many lines as possible. Ignore
         * the first line, it is already in place. */
        int lines = height < old_height ? height : old_height;

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

        if(!cv->dirty_disabled)
            caca_add_dirty_rect(cv, 0, old_height,
                                old_width, height - old_height);
    }

    /* If both width and height are larger, there is a new dirty rectangle
     * that needs to be created in the lower right corner. */
    if(!cv->dirty_disabled &&
        width > old_width && height > old_height)
        caca_add_dirty_rect(cv, old_width, old_height,
                            width - old_width, height - old_height);

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
    _caca_load_frame_info(cv);

    return 0;
}

/*
 * XXX: The following functions are aliases.
 */

cucul_canvas_t * cucul_create_canvas(int, int) CACA_ALIAS(caca_create_canvas);
int cucul_manage_canvas(cucul_canvas_t *, int (*)(void *), void *)
    CACA_ALIAS(caca_manage_canvas);
int cucul_unmanage_canvas(cucul_canvas_t *, int (*)(void *), void *)
    CACA_ALIAS(caca_unmanage_canvas);
int cucul_set_canvas_size(cucul_canvas_t *, int, int)
    CACA_ALIAS(caca_set_canvas_size);
int cucul_get_canvas_width(cucul_canvas_t const *)
    CACA_ALIAS(caca_get_canvas_width);
int cucul_get_canvas_height(cucul_canvas_t const *)
    CACA_ALIAS(caca_get_canvas_height);
uint32_t const * cucul_get_canvas_chars(cucul_canvas_t const *)
    CACA_ALIAS(caca_get_canvas_chars);
uint32_t const * cucul_get_canvas_attrs(cucul_canvas_t const *)
    CACA_ALIAS(caca_get_canvas_attrs);
int cucul_free_canvas(cucul_canvas_t *) CACA_ALIAS(caca_free_canvas);
int cucul_rand(int, int) CACA_ALIAS(caca_rand);

