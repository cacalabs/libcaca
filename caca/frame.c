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
 *  This file contains a small framework for canvas frame management.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#endif

#include "caca.h"
#include "caca_internals.h"

/** \brief Get the number of frames in a canvas.
 *
 *  Return the current canvas' frame count.
 *
 *  This function never fails.
 *
 *  \param cv A libcaca canvas
 *  \return The frame count
 */
int caca_get_frame_count(caca_canvas_t const *cv)
{
    return cv->framecount;
}

/** \brief Activate a given canvas frame.
 *
 *  Set the active canvas frame. All subsequent drawing operations will
 *  be performed on that frame. The current painting context set by
 *  caca_set_attr() is inherited.
 *
 *  If the frame index is outside the canvas' frame range, nothing happens.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL Requested frame is out of range.
 *
 *  \param cv A libcaca canvas
 *  \param id The canvas frame to activate
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_set_frame(caca_canvas_t *cv, int id)
{
    if(id < 0 || id >= cv->framecount)
    {
        seterrno(EINVAL);
        return -1;
    }

    /* Bail out if no operation is required */
    if(id == cv->frame)
        return 0;

    _caca_save_frame_info(cv);
    cv->frame = id;
    _caca_load_frame_info(cv);

    if(!cv->dirty_disabled)
        caca_add_dirty_rect(cv, 0, 0, cv->width, cv->height);

    return 0;
}

/** \brief Get the current frame's name.
 *
 *  Return the current frame's name. The returned string is valid until
 *  the frame is deleted or caca_set_frame_name() is called to change
 *  the frame name again.
 *
 *  This function never fails.
 *
 *  \param cv A libcaca canvas.
 *  \return The current frame's name.
 */
char const *caca_get_frame_name(caca_canvas_t const *cv)
{
    return cv->frames[cv->frame].name;
}

/** \brief Set the current frame's name.
 *
 *  Set the current frame's name. Upon creation, a frame has a default name
 *  of \c "frame#xxxxxxxx" where \c xxxxxxxx is a self-incrementing
 *  hexadecimal number.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c ENOMEM Not enough memory to allocate new frame.
 *
 *  \param cv A libcaca canvas.
 *  \param name The name to give to the current frame.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_set_frame_name(caca_canvas_t *cv, char const *name)
{
    char *newname = strdup(name);

    if(!newname)
    {
        seterrno(ENOMEM);
        return -1;
    }

    free(cv->frames[cv->frame].name);
    cv->frames[cv->frame].name = newname;

    return 0;
}

/** \brief Add a frame to a canvas.
 *
 *  Create a new frame within the given canvas. Its contents and attributes
 *  are copied from the currently active frame.
 *
 *  The frame index indicates where the frame should be inserted. Valid
 *  values range from 0 to the current canvas frame count. If the frame
 *  index is greater than or equals the current canvas frame count, the new
 *  frame is appended at the end of the canvas. If the frame index is less
 *  than zero, the new frame is inserted at index 0.
 *
 *  The active frame does not change, but its index may be renumbered due
 *  to the insertion.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c ENOMEM Not enough memory to allocate new frame.
 *
 *  \param cv A libcaca canvas
 *  \param id The index where to insert the new frame
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_create_frame(caca_canvas_t *cv, int id)
{
    int size = cv->width * cv->height;
    int f;

    if(id < 0)
        id = 0;
    else if(id > cv->framecount)
        id = cv->framecount;

    cv->framecount++;
    cv->frames = realloc(cv->frames,
                         sizeof(struct caca_frame) * cv->framecount);

    for(f = cv->framecount - 1; f > id; f--)
        cv->frames[f] = cv->frames[f - 1];

    if(cv->frame >= id)
        cv->frame++;

    cv->frames[id].width = cv->width;
    cv->frames[id].height = cv->height;
    cv->frames[id].chars = malloc(size * sizeof(uint32_t));
    memcpy(cv->frames[id].chars, cv->chars, size * sizeof(uint32_t));
    cv->frames[id].attrs = malloc(size * sizeof(uint32_t));
    memcpy(cv->frames[id].attrs, cv->attrs, size * sizeof(uint32_t));
    cv->frames[id].curattr = cv->curattr;

    cv->frames[id].x = cv->frames[cv->frame].x;
    cv->frames[id].y = cv->frames[cv->frame].y;
    cv->frames[id].handlex = cv->frames[cv->frame].handlex;
    cv->frames[id].handley = cv->frames[cv->frame].handley;

    cv->frames[id].name = strdup("frame#--------");
    sprintf(cv->frames[id].name + 6, "%.08x", ++cv->autoinc);

    return 0;
}

/** \brief Remove a frame from a canvas.
 *
 *  Delete a frame from a given canvas.
 *
 *  The frame index indicates the frame to delete. Valid values range from
 *  0 to the current canvas frame count minus 1. If the frame index is
 *  greater than or equals the current canvas frame count, the last frame
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
 *  \param cv A libcaca canvas
 *  \param id The index of the frame to delete
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_free_frame(caca_canvas_t *cv, int id)
{
    int f;

    if(id < 0 || id >= cv->framecount)
    {
        seterrno(EINVAL);
        return -1;
    }

    if(cv->framecount == 1)
    {
        seterrno(EINVAL);
        return -1;
    }

    free(cv->frames[id].chars);
    free(cv->frames[id].attrs);
    free(cv->frames[id].name);

    for(f = id + 1; f < cv->framecount; f++)
        cv->frames[f - 1] = cv->frames[f];

    cv->framecount--;
    cv->frames = realloc(cv->frames,
                         sizeof(struct caca_frame) * cv->framecount);

    if(cv->frame > id)
        cv->frame--;
    else if(cv->frame == id)
    {
        cv->frame = 0;
        _caca_load_frame_info(cv);
        if(!cv->dirty_disabled)
            caca_add_dirty_rect(cv, 0, 0, cv->width, cv->height);
    }

    return 0;
}

/*
 * XXX: the following functions are local.
 */

void _caca_save_frame_info(caca_canvas_t *cv)
{
    cv->frames[cv->frame].width = cv->width;
    cv->frames[cv->frame].height = cv->height;

    cv->frames[cv->frame].curattr = cv->curattr;
}

void _caca_load_frame_info(caca_canvas_t *cv)
{
    cv->width = cv->frames[cv->frame].width;
    cv->height = cv->frames[cv->frame].height;

    cv->chars = cv->frames[cv->frame].chars;
    cv->attrs = cv->frames[cv->frame].attrs;

    cv->curattr = cv->frames[cv->frame].curattr;
}

/*
 * XXX: The following functions are aliases.
 */

int cucul_get_frame_count(cucul_canvas_t const *)
         CACA_ALIAS(caca_get_frame_count);
int cucul_set_frame(cucul_canvas_t *, int) CACA_ALIAS(caca_set_frame);
char const *cucul_get_frame_name(cucul_canvas_t const *)
         CACA_ALIAS(caca_get_frame_name);
int cucul_set_frame_name(cucul_canvas_t *, char const *)
         CACA_ALIAS(caca_set_frame_name);
int cucul_create_frame(cucul_canvas_t *, int) CACA_ALIAS(caca_create_frame);
int cucul_free_frame(cucul_canvas_t *, int) CACA_ALIAS(caca_free_frame);

