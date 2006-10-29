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
 *  This file contains a small framework for canvas frame management.
 */

#include "config.h"
#include "common.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#   if defined(HAVE_ERRNO_H)
#       include <errno.h>
#   endif
#endif

#include "cucul.h"
#include "cucul_internals.h"

/** \brief Get the number of frames in a canvas.
 *
 *  Return the current canvas' frame count.
 *
 *  This function never fails.
 *
 *  \param cv A libcucul canvas
 *  \return The frame count
 */
unsigned int cucul_get_canvas_frame_count(cucul_canvas_t *cv)
{
    return cv->framecount;
}

/** \brief Activate a given canvas frame.
 *
 *  Set the active canvas frame. All subsequent drawing operations will
 *  be performed on that frame. The current painting context set by
 *  cucul_set_attr() is inherited.
 *
 *  If the frame index is outside the canvas' frame range, nothing happens.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL Requested frame is out of range.
 *
 *  \param cv A libcucul canvas
 *  \param frame The canvas frame to activate
 *  \return 0 in case of success, -1 if an error occurred.
 */
int cucul_set_canvas_frame(cucul_canvas_t *cv, unsigned int frame)
{
    if(frame >= cv->framecount)
    {
#if defined(HAVE_ERRNO_H)
        errno = EINVAL;
#endif
        return -1;
    }

    cv->frame = frame;

    cv->chars = cv->allchars[cv->frame];
    cv->attrs = cv->allattrs[cv->frame];

    return 0;
}

/** \brief Add a frame to a canvas.
 *
 *  Create a new frame within the given canvas. Its contents are copied
 *  from the currently active frame.
 *
 *  The frame index indicates where the frame should be inserted. Valid
 *  values range from 0 to the current canvas frame count. If the frame
 *  index is greater the or equals the current canvas frame count, the new
 *  frame is appended at the end of the canvas.
 *
 *  The active frame does not change, but its index may be renumbered due
 *  to the insertion.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c ENOMEM Not enough memory to allocate new frame.
 *
 *  \param cv A libcucul canvas
 *  \param frame The index where to insert the new frame
 *  \return 0 in case of success, -1 if an error occurred.
 */
int cucul_create_canvas_frame(cucul_canvas_t *cv, unsigned int frame)
{
    unsigned int size = cv->width * cv->height * sizeof(uint32_t);
    unsigned int f;

    if(frame > cv->framecount)
        frame = cv->framecount;

    cv->framecount++;
    cv->allchars = realloc(cv->allchars, sizeof(uint32_t *) * cv->framecount);
    cv->allattrs = realloc(cv->allattrs, sizeof(uint32_t *) * cv->framecount);

    for(f = cv->framecount - 1; f > frame; f--)
    {
        cv->allchars[f] = cv->allchars[f - 1];
        cv->allattrs[f] = cv->allattrs[f - 1];
    }

    cv->allchars[frame] = malloc(size);
    memcpy(cv->allchars[frame], cv->chars, size);
    cv->allattrs[frame] = malloc(size);
    memcpy(cv->allattrs[frame], cv->attrs, size);

    if(cv->frame >= frame)
        cv->frame++;

    cv->chars = cv->allchars[cv->frame];
    cv->attrs = cv->allattrs[cv->frame];

    return 0;
}

/** \brief Remove a frame from a canvas.
 *
 *  Delete a frame from a given canvas.
 *
 *  The frame index indicates the frame to delete. Valid values range from
 *  0 to the current canvas frame count minus 1. If the frame index is
 *  greater the or equals the current canvas frame count, the last frame
 *  is deleted.
 *
 *  If the active frame is deleted, frame 0 becomes the new active frame.
 *  Otherwise, the active frame does not change, but its index may be
 *  renumbered due to the deletion.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL Requested frame is out of range, or attempt to delete the
 *    last frame of the canvas.
 *
 *  \param cv A libcucul canvas
 *  \param frame The index of the frame to delete
 *  \return 0 in case of success, -1 if an error occurred.
 */
int cucul_free_canvas_frame(cucul_canvas_t *cv, unsigned int frame)
{
    unsigned int f;

    if(frame >= cv->framecount)
    {
#if defined(HAVE_ERRNO_H)
        errno = EINVAL;
#endif
        return -1;
    }

    if(cv->framecount == 1)
    {
#if defined(HAVE_ERRNO_H)
        errno = EINVAL;
#endif
        return -1;
    }

    free(cv->allchars[frame]);
    free(cv->allattrs[frame]);

    for(f = frame + 1; f < cv->framecount; f++)
    {
        cv->allchars[f - 1] = cv->allchars[f];
        cv->allattrs[f - 1] = cv->allattrs[f];
    }

    cv->framecount--;
    cv->allchars = realloc(cv->allchars, sizeof(uint32_t *) * cv->framecount);
    cv->allattrs = realloc(cv->allattrs, sizeof(uint32_t *) * cv->framecount);

    if(cv->frame > frame)
        cv->frame--;
    else if(cv->frame == frame)
        cv->frame = 0;

    cv->chars = cv->allchars[cv->frame];
    cv->attrs = cv->allattrs[cv->frame];

    return 0;
}

