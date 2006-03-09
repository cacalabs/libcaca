/*
 *  libcaca       ASCII-Art library
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file graphics.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Character drawing
 *
 *  This file contains character and string drawing functions.
 */

#include "config.h"

#include "caca.h"
#include "caca_internals.h"
#include "cucul.h"
#include "cucul_internals.h"

/*
 * Local functions
 */
void _caca_handle_resize(caca_t *kk);

/** \brief Set the window title.
 *
 *  If libcaca runs in a window, try to change its title. This works with
 *  the X11 and Win32 drivers.
 *
 *  \param title The desired window title.
 *  \return 0 upon success, a non-zero value if an error occurs.
 */
int caca_set_window_title(caca_t *kk, char const *title)
{
    return kk->drv.set_window_title(kk, title);
}

/** \brief Get the window width.
 *
 *  If libcaca runs in a window, get the usable window width. This value can
 *  be used for aspect ratio calculation. If libcaca does not run in a window
 *  or if there is no way to know the font size, assume a 6x10 font is being
 *  used. Note that the units are not necessarily pixels.
 *
 *  \return The window width.
 */
unsigned int caca_get_window_width(caca_t *kk)
{
    return kk->drv.get_window_width(kk);
}

/** \brief Get the window height.
 *
 *  If libcaca runs in a window, get the usable window height. This value can
 *  be used for aspect ratio calculation. If libcaca does not run in a window
 *  or if there is no way to know the font size, assume a 6x10 font is being
 *  used. Note that the units are not necessarily pixels.
 *
 *  \return The window height.
 */
unsigned int caca_get_window_height(caca_t *kk)
{
    return kk->drv.get_window_height(kk);
}

/** \brief Set the refresh delay.
 *
 *  This function sets the refresh delay in microseconds. The refresh delay
 *  is used by caca_display() to achieve constant framerate. See the
 *  caca_display() documentation for more details.
 *
 *  If the argument is zero, constant framerate is disabled. This is the
 *  default behaviour.
 *
 *  \param usec The refresh delay in microseconds.
 */
void caca_set_delay(caca_t *kk, unsigned int usec)
{
    kk->delay = usec;
}

/** \brief Get the average rendering time.
 *
 *  This function returns the average rendering time, which is the average
 *  measured time between two caca_display() calls, in microseconds. If
 *  constant framerate was activated by calling caca_set_delay(), the average
 *  rendering time will not be considerably shorter than the requested delay
 *  even if the real rendering time was shorter.
 *
 *  \return The render time in microseconds.
 */
unsigned int caca_get_rendertime(caca_t *kk)
{
    return kk->rendertime;
}

/** \brief Flush pending changes and redraw the screen.
 *
 *  This function flushes all graphical operations and prints them to the
 *  screen. Nothing will show on the screen until caca_display() is
 *  called.
 *
 *  If caca_set_delay() was called with a non-zero value, caca_display()
 *  will use that value to achieve constant framerate: if two consecutive
 *  calls to caca_display() are within a time range shorter than the value
 *  set with caca_set_delay(), the second call will wait a bit before
 *  performing the screen refresh.
 */
void caca_display(caca_t *kk)
{
#if !defined(_DOXYGEN_SKIP_ME)
#define IDLE_USEC 10000
#endif
    int ticks = kk->lastticks + _caca_getticks(&kk->timer);

    kk->drv.display(kk);

    /* Once the display is finished, we can ack resizes */
    if(kk->resize.resized)
    {
        kk->resize.resized = 0;
        _caca_handle_resize(kk);
    }

    /* Wait until kk->delay + time of last call */
    ticks += _caca_getticks(&kk->timer);
    for(ticks += _caca_getticks(&kk->timer);
        ticks + IDLE_USEC < (int)kk->delay;
        ticks += _caca_getticks(&kk->timer))
    {
        _caca_sleep(IDLE_USEC);
    }

    /* Update the sliding mean of the render time */
    kk->rendertime = (7 * kk->rendertime + ticks) / 8;

    kk->lastticks = ticks - kk->delay;

    /* If we drifted too much, it's bad, bad, bad. */
    if(kk->lastticks > (int)kk->delay)
        kk->lastticks = 0;
}

/*
 * XXX: following functions are local
 */

void _caca_handle_resize(caca_t *kk)
{
    kk->drv.handle_resize(kk);

    /* Tell libcucul we changed size */
    if(kk->resize.w != kk->qq->width || kk->resize.h != kk->qq->height)
        _cucul_set_size(kk->qq, kk->resize.w, kk->resize.h);
}

