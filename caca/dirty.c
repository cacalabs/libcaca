/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2010 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *  This file contains the dirty rectangle handling functions.
 *
 *
 *  About dirty rectangles:
 *
 *  * Dirty rectangles MUST NOT be larger than the canvas. If the user
 *  provides a large rectangle through caca_add_dirty_rect(), or if the
 *  canvas changes size to become smaller, all dirty rectangles MUST
 *  immediately be clipped to the canvas size.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <string.h>
#endif

#include "caca.h"
#include "caca_internals.h"

static void merge_new_rect(caca_canvas_t *cv, int n);

/** \brief Disable dirty rectangles.
 *
 *  Disable dirty rectangle handling for all \e libcaca graphic calls. This
 *  is handy when the calling application needs to do slow operations within
 *  a known area. Just call caca_add_dirty_rect() afterwards.
 *
 *  This function is recursive. Dirty rectangles are only reenabled when
 *  caca_enable_dirty_rect() is called as many times.
 *
 *  This function never fails.
 *
 *  \param cv A libcaca canvas.
 *  \return This function always returns 0.
 */
int caca_disable_dirty_rect(caca_canvas_t *cv)
{
    cv->dirty_disabled++;

    return 0;
}

/** \brief Enable dirty rectangles.
 *
 *  This function can only be called after caca_disable_dirty_rect() was
 *  called.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL Dirty rectangles were not disabled.
 *
 *  \param cv A libcaca canvas.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_enable_dirty_rect(caca_canvas_t *cv)
{
    if(cv->dirty_disabled <= 0)
    {
        seterrno(EINVAL);
        return -1;
    }

    cv->dirty_disabled--;

    return 0;
}

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
int caca_get_dirty_rect_count(caca_canvas_t *cv)
{
    return cv->ndirty;
}

/** \brief Get a canvas's dirty rectangle.
 *
 *  Get the canvas's given dirty rectangle coordinates. The index must be
 *  within the dirty rectangle count. See caca_get_dirty_rect_count()
 *  for how to compute this count.
 *
 *  If an error occurs, no coordinates are written in the pointer arguments,
 *  -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL Specified rectangle index is out of bounds.
 *
 *  \param cv A libcaca canvas.
 *  \param r The requested rectangle index.
 *  \param x A pointer to an integer where the leftmost edge of the
 *           dirty rectangle will be stored.
 *  \param y A pointer to an integer where the topmost edge of the
 *           dirty rectangle will be stored.
 *  \param width A pointer to an integer where the width of the
 *               dirty rectangle will be stored.
 *  \param height A pointer to an integer where the height of the
 *                dirty rectangle will be stored.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_get_dirty_rect(caca_canvas_t *cv, int r,
                        int *x, int *y, int *width, int *height)
{
    if(r < 0 || r >= cv->ndirty)
    {
        seterrno(EINVAL);
        return -1;
    }

    *x = cv->dirty[r].xmin;
    *y = cv->dirty[r].ymin;
    *width = cv->dirty[r].xmax - cv->dirty[r].xmin + 1;
    *height = cv->dirty[r].ymax - cv->dirty[r].ymin + 1;

    debug("dirty #%i: %ix%i at (%i,%i)", r, *width, *height, *x, *y);

    return 0;
}

/** \brief Add an area to the canvas's dirty rectangle list.
 *
 *  Add an invalidating zone to the canvas's dirty rectangle list. For more
 *  information about the dirty rectangles, see caca_get_dirty_rect().
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
 *  \param x The leftmost edge of the additional dirty rectangle.
 *  \param y The topmost edge of the additional dirty rectangle.
 *  \param width The width of the additional dirty rectangle.
 *  \param height The height of the additional dirty rectangle.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_add_dirty_rect(caca_canvas_t *cv, int x, int y, int width, int height)
{
    debug("new dirty: %ix%i at (%i,%i)", width, height, x, y);

    /* Clip arguments to canvas */
    if(x < 0) { width += x; x = 0; }

    if(x + width > cv->width)
        width = cv->width - x;

    if(y < 0) { height += y; y = 0; }

    if(y + height > cv->height)
        height = cv->height - y;

    /* Ignore empty and out-of-canvas rectangles */
    if(width <= 0 || height <= 0)
    {
        seterrno(EINVAL);
        return -1;
    }

    /* Add the new rectangle to the list; it works even if cv->ndirty
     * is MAX_DIRTY_COUNT because there's an extra cell in the array. */
    cv->dirty[cv->ndirty].xmin = x;
    cv->dirty[cv->ndirty].ymin = y;
    cv->dirty[cv->ndirty].xmax = x + width - 1;
    cv->dirty[cv->ndirty].ymax = y + height - 1;
    cv->ndirty++;

    /* Try to merge the new rectangle with existing ones. This also ensures
     * that cv->ndirty is brought back below MAX_DIRTY_COUNT. */
    merge_new_rect(cv, cv->ndirty - 1);

    return 0;
}

/** \brief Remove an area from the dirty rectangle list.
 *
 *  Mark a cell area in the canvas as not dirty. For more information about
 *  the dirty rectangles, see caca_get_dirty_rect().
 *
 *  Values such that \b xmin > \b xmax or \b ymin > \b ymax indicate that
 *  the dirty rectangle is empty. They will be silently ignored.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL Specified rectangle coordinates are out of bounds.
 *
 *  \param cv A libcaca canvas.
 *  \param x The leftmost edge of the clean rectangle.
 *  \param y The topmost edge of the clean rectangle.
 *  \param width The width of the clean rectangle.
 *  \param height The height of the clean rectangle.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_remove_dirty_rect(caca_canvas_t *cv, int x, int y,
                           int width, int height)
{
    /* Clip arguments to canvas size */
    if(x < 0) { width += x; x = 0; }

    if(x + width > cv->width)
        width = cv->width - x;

    if(y < 0) { height += y; y = 0; }

    if(y + height > cv->height)
        height = cv->height - y;

    /* Ignore empty and out-of-canvas rectangles */
    if(width <= 0 || height <= 0)
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
int caca_clear_dirty_rect_list(caca_canvas_t *cv)
{
    cv->ndirty = 0;

    return 0;
}

/*
 * XXX: the following functions are local.
 */

static inline int int_min(int a, int b) { return a < b ? a : b; }
static inline int int_max(int a, int b) { return a > b ? a : b; }

/* Merge a newly added rectangle, if necessary. */
static void merge_new_rect(caca_canvas_t *cv, int n)
{
    int wasted[MAX_DIRTY_COUNT + 1];
    int i, sn, best, best_score;

    best = -1;
    best_score = cv->width * cv->height;

    sn = (cv->dirty[n].xmax - cv->dirty[n].xmin + 1)
           * (cv->dirty[n].ymax - cv->dirty[n].ymin + 1);

    /* Check whether the new rectangle can be merged with an existing one. */
    for(i = 0; i < cv->ndirty; i++)
    {
        int si, sf, xmin, ymin, xmax, ymax;

        if(i == n)
            continue;

        xmin = int_min(cv->dirty[i].xmin, cv->dirty[n].xmin);
        ymin = int_min(cv->dirty[i].ymin, cv->dirty[n].ymin);
        xmax = int_max(cv->dirty[i].xmax, cv->dirty[n].xmax);
        ymax = int_max(cv->dirty[i].ymax, cv->dirty[n].ymax);

        sf = (xmax - xmin + 1) * (ymax - ymin + 1);

        /* Shortcut: if the current rectangle is inside the new rectangle,
         * we remove the current rectangle and continue trying merges. */
        if(sf == sn)
        {
            memmove(&cv->dirty[i], &cv->dirty[i + 1],
                    (cv->ndirty - i) * sizeof(cv->dirty[0]));
            cv->ndirty--;

            if(i < n)
                n--;
            else
                i--;

            continue;
        }

        si = (cv->dirty[i].xmax - cv->dirty[i].xmin + 1)
               * (cv->dirty[i].ymax - cv->dirty[i].ymin + 1);

        /* Shortcut: if the new rectangle is inside the current rectangle,
         * we get rid of the new rectangle and bail out. */
        if(sf == si)
        {
            cv->ndirty--;
            memmove(&cv->dirty[n], &cv->dirty[n + 1],
                    (cv->ndirty - n) * sizeof(cv->dirty[0]));
            return;
        }

        /* We store approximately how many bytes were wasted. FIXME: this is
         * not an exact computation, we need to be more precise. */
        wasted[i] = sf - si - sn;

        if(wasted[i] < best_score)
        {
             best = i;
             best_score = wasted[i];
        }
    }

    /* FIXME: we only try to merge the current rectangle, ignoring
     * potentially better merges. */

    /* If no acceptable score was found and the dirty rectangle list is
     * not full, we bail out. */
    if(best_score > 0 && cv->ndirty < MAX_DIRTY_COUNT)
        return;

    /* Otherwise, merge the rectangle with the best candidate */
    cv->dirty[best].xmin = int_min(cv->dirty[best].xmin, cv->dirty[n].xmin);
    cv->dirty[best].ymin = int_min(cv->dirty[best].ymin, cv->dirty[n].ymin);
    cv->dirty[best].xmax = int_max(cv->dirty[best].xmax, cv->dirty[n].xmax);
    cv->dirty[best].ymax = int_max(cv->dirty[best].ymax, cv->dirty[n].ymax);

    memmove(&cv->dirty[n], &cv->dirty[n + 1],
            (cv->ndirty - n) * sizeof(cv->dirty[0]));
    cv->ndirty--;

    if(best < n)
        merge_new_rect(cv, best);
    else
        merge_new_rect(cv, best - 1);
}

/* Clip all dirty rectangles in case they're larger than the canvas */
void _caca_clip_dirty_rect_list(caca_canvas_t *cv)
{
    int i;

    for(i = 0; i < cv->ndirty; i++)
    {
        if(cv->dirty[i].xmin < 0)
            cv->dirty[i].xmin = 0;

        if(cv->dirty[i].ymin < 0)
            cv->dirty[i].ymin = 0;

        if(cv->dirty[i].xmax >= cv->width)
            cv->dirty[i].xmax = cv->width - 1;

        if(cv->dirty[i].ymax >= cv->height)
            cv->dirty[i].ymax = cv->height - 1;
    }
}

