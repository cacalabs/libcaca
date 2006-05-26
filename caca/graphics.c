/*
 *  libcaca       Colour ASCII-Art library
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
 *  This file contains character and string drawing functions.
 */

#include "config.h"
#include "common.h"

#include "caca.h"
#include "caca_internals.h"
#include "cucul.h"
#include "cucul_internals.h"

/** \brief Set the display title.
 *
 *  If libcaca runs in a window, try to change its title. This works with
 *  the X11 and Win32 drivers.
 *
 *  \param dp The libcaca display context.
 *  \param title The desired display title.
 *  \return 0 upon success, a non-zero value if an error occurs.
 */
int caca_set_display_title(caca_display_t *dp, char const *title)
{
    return dp->drv.set_display_title(dp, title);
}

/** \brief Get the display width.
 *
 *  If libcaca runs in a window, get the usable window width. This value can
 *  be used for aspect ratio calculation. If libcaca does not run in a window
 *  or if there is no way to know the font size, most drivers will assume a
 *  6x10 font is being used. Note that the units are not necessarily pixels.
 *
 *  \param dp The libcaca display context.
 *  \return The display width.
 */
unsigned int caca_get_display_width(caca_display_t *dp)
{
    return dp->drv.get_display_width(dp);
}

/** \brief Get the display height.
 *
 *  If libcaca runs in a window, get the usable window height. This value can
 *  be used for aspect ratio calculation. If libcaca does not run in a window
 *  or if there is no way to know the font size, assume a 6x10 font is being
 *  used. Note that the units are not necessarily pixels.
 *
 *  \param dp The libcaca display context.
 *  \return The display height.
 */
unsigned int caca_get_display_height(caca_display_t *dp)
{
    return dp->drv.get_display_height(dp);
}

/** \brief Set the refresh delay.
 *
 *  This function sets the refresh delay in microseconds. The refresh delay
 *  is used by caca_refresh_display() to achieve constant framerate. See the
 *  caca_refresh_display() documentation for more details.
 *
 *  If the argument is zero, constant framerate is disabled. This is the
 *  default behaviour.
 *
 *  \param dp The libcaca display context.
 *  \param usec The refresh delay in microseconds.
 */
void caca_set_display_time(caca_display_t *dp, unsigned int usec)
{
    dp->delay = usec;
}

/** \brief Get the display's average rendering time.
 *
 *  This function returns the average rendering time, which is the average
 *  measured time between two caca_refresh_display() calls, in microseconds. If
 *  constant framerate was activated by calling caca_set_display_time(), the
 *  average rendering time will not be considerably shorter than the requested
 *  delay even if the real rendering time was shorter.
 *
 *  \param dp The libcaca display context.
 *  \return The render time in microseconds.
 */
unsigned int caca_get_display_time(caca_display_t *dp)
{
    return dp->rendertime;
}

/** \brief Flush pending changes and redraw the screen.
 *
 *  This function flushes all graphical operations and prints them to the
 *  screen. Nothing will show on the screen until caca_refresh_display() is
 *  called.
 *
 *  If caca_set_display_time() was called with a non-zero value,
 *  caca_refresh_display() will use that value to achieve constant
 *  framerate: if two consecutive calls to caca_refresh_display() are within
 *  a time range shorter than the value set with caca_set_display_time(),
 *  the second call will be delayed before performing the screen refresh.
 *
 *  \param dp The libcaca display context.
 */
void caca_refresh_display(caca_display_t *dp)
{
#if !defined(_DOXYGEN_SKIP_ME)
#define IDLE_USEC 5000
#endif
    int ticks = dp->lastticks + _caca_getticks(&dp->timer);

    dp->drv.display(dp);

    /* Once the display is finished, we can ack resizes */
    if(dp->resize.resized)
    {
        dp->resize.resized = 0;
        _caca_handle_resize(dp);
    }

    /* Wait until dp->delay + time of last call */
    ticks += _caca_getticks(&dp->timer);
    for(ticks += _caca_getticks(&dp->timer);
        ticks + IDLE_USEC < (int)dp->delay;
        ticks += _caca_getticks(&dp->timer))
    {
        _caca_sleep(IDLE_USEC);
    }

    /* Update the sliding mean of the render time */
    dp->rendertime = (7 * dp->rendertime + ticks) / 8;

    dp->lastticks = ticks - dp->delay;

    /* If we drifted too much, it's bad, bad, bad. */
    if(dp->lastticks > (int)dp->delay)
        dp->lastticks = 0;
}

/** \brief Show or hide the mouse pointer.
 *
 *  This function shows or hides the mouse pointer, for devices that
 *  support it.
 *
 *  \param dp The libcaca display context.
 *  \param flag 0 hides the pointer, 1 shows the system's default pointer
 *              (usually an arrow). Other values are reserved for future use.
 */
void caca_set_mouse(caca_display_t *dp, int flag)
{
    if(dp->drv.set_mouse)
        dp->drv.set_mouse(dp, flag);
}

/*
 * XXX: following functions are local
 */

void _caca_handle_resize(caca_display_t *dp)
{
    dp->drv.handle_resize(dp);

    /* Tell libcucul we changed size */
    if(dp->resize.w != dp->cv->width || dp->resize.h != dp->cv->height)
        _cucul_set_canvas_size(dp->cv, dp->resize.w, dp->resize.h);
}

