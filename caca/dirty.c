/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2009 Sam Hocevar <sam@hocevar.net>
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
 *  This file contains the dirty rectangle handling functions.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#endif

#include "caca.h"
#include "caca_internals.h"

/** \brief Get the number of dirty rectangles in the canvas.
 *
 *  Get the number of dirty rectangles in a canvas. Dirty rectangles are
 *  areas that contain cells that have changed since the last reset.
 *
 *  The dirty rectangles are used internally by display drivers to optimise
 *  rendering by avoiding to redraw the whole screen. Once the display driver
 *  has rendered the canvas, it resets the dirty rectangle list.
 *
 *  Dirty rectangles are guaranteed not to overlap.
 *
 *  This function never fails.
 *
 *  \param cv A libcaca canvas.
 *  \return The number of dirty rectangles in the given canvas.
 */
int caca_get_dirty_rectangle_count(caca_canvas_t *cv)
{
    return cv->ndirty;
}

/** \brief Get a canvas's dirty rectangle.
 *
 *  Get the canvas's given dirty rectangle coordinates. The index must be
 *  within the dirty rectangle count. See caca_get_dirty_rectangle_count()
 *  for how to compute this count.
 *
 *  If an error occurs, no coordinates are written in the pointer arguments,
 *  -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL Specified rectangle index is out of bounds.
 *
 *  \param cv A libcaca canvas.
 *  \param index The requested rectangle index.
 *  \param xmin A pointer to an integer where the leftmost edge of the
 *              dirty rectangle will be stored.
 *  \param ymin A pointer to an integer where the topmost edge of the
 *              dirty rectangle will be stored.
 *  \param xmax A pointer to an integer where the rightmost edge of the
 *              dirty rectangle will be stored.
 *  \param ymax A pointer to an integer where the bottommost edge of the
 *              dirty rectangle will be stored.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_get_dirty_rectangle(caca_canvas_t *cv, int index,
                             int *xmin, int *ymin, int *xmax, int *ymax)
{
    if(index < 0 || index >= cv->ndirty)
    {
        seterrno(EINVAL);
        return -1;
    }

    /* Normalise dirty rectangle so that the values can be directly used. */
    if(cv->dirty_xmin < 0)
        cv->dirty_xmin = 0;

    if(cv->dirty_xmax > cv->width - 1)
        cv->dirty_xmax = cv->width - 1;

    if(cv->dirty_ymin < 0)
        cv->dirty_ymin = 0;

    if(cv->dirty_ymax > cv->height - 1)
        cv->dirty_ymax = cv->height - 1;

    *xmin = cv->dirty_xmin;
    *xmax = cv->dirty_xmax;
    *ymin = cv->dirty_ymin;
    *ymax = cv->dirty_ymax;

    return 0;
}

/** \brief Add an area to the canvas's dirty rectangle list.
 *
 *  Add an invalidating zone to the canvas's dirty rectangle list. For more
 *  information about the dirty rectangles, see caca_get_dirty_rectangle().
 *
 *  This function may be useful to force refresh of a given zone of the
 *  canvas even if the dirty rectangle tracking indicates that it is
 *  unchanged. This may happen if the canvas contents were somewhat
 *  directly modified.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL Specified rectangle coordinates are out of bounds.
 *
 *  \param cv A libcaca canvas.
 *  \param xmin The leftmost edge of the additional dirty rectangle.
 *  \param ymin The topmost edge of the additional dirty rectangle.
 *  \param xmax The rightmost edge of the additional dirty rectangle.
 *  \param ymax The bottommost edge of the additional dirty rectangle.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_add_dirty_rectangle(caca_canvas_t *cv, int xmin, int ymin,
                             int xmax, int ymax)
{
    /* Ignore empty and out-of-bounds rectangles */
    if(xmin > xmax || ymin > ymax
        || xmax < 0 || xmin >= cv->width || ymax < 0 || ymin >= cv->height)
    {
        seterrno(EINVAL);
        return -1;
    }

    if(cv->ndirty == 0)
    {
        cv->ndirty = 1;
        cv->dirty_xmin = xmin;
        cv->dirty_xmax = xmax;
        cv->dirty_ymin = ymin;
        cv->dirty_ymax = ymax;
    }
    else
    {
        if(xmin < cv->dirty_xmin)
            cv->dirty_xmin = xmin;
        if(xmax > cv->dirty_xmax)
            cv->dirty_xmax = xmax;
        if(ymin < cv->dirty_ymin)
            cv->dirty_ymin = ymin;
        if(ymax > cv->dirty_ymax)
            cv->dirty_ymax = ymax;
    }

    return 0;
}

/** \brief Remove an area from the dirty rectangle list.
 *
 *  Mark a cell area in the canvas as not dirty. For more information about
 *  the dirty rectangles, see caca_get_dirty_rectangle().
 *
 *  Values such that \b xmin > \b xmax or \b ymin > \b ymax indicate that
 *  the dirty rectangle is empty. They will be silently ignored.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL Specified rectangle coordinates are out of bounds.
 *
 *  \param cv A libcaca canvas.
 *  \param xmin The leftmost edge of the clean rectangle.
 *  \param ymin The topmost edge of the clean rectangle.
 *  \param xmax The rightmost edge of the clean rectangle.
 *  \param ymax The bottommost edge of the clean rectangle.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_remove_dirty_rectangle(caca_canvas_t *cv, int xmin, int ymin,
                                int xmax, int ymax)
{
    /* Ignore empty and out-of-bounds rectangles */
    if(xmin > xmax || ymin > ymax
        || xmax < 0 || xmin >= cv->width || ymax < 0 || ymin >= cv->height)
    {
        seterrno(EINVAL);
        return -1;
    }

    /* FIXME: implement this function. It's OK to have it do nothing,
     * since we take a conservative approach in dirty rectangle handling,
     * but we ought to help the rendering eventually. */

    return 0;
}

/** \brief Clear a canvas's dirty rectangle list.
 *
 *  Empty the canvas's dirty rectangle list.
 *
 *  This function never fails.
 *
 *  \param cv A libcaca canvas.
 *  \return This function always returns 0.
 */
int caca_clear_dirty_rectangle_list(caca_canvas_t *cv)
{
    cv->ndirty = 0;

    return 0;
}

