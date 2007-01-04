/*
 *  libcaca       Colour ASCII-Art library
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
 *  This file contains event handling functions for keyboard and mouse input.
 */

#include "config.h"
#include "common.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"
#include "caca.h"
#include "caca_internals.h"

static int _get_next_event(caca_display_t *, caca_event_t *);
static int _lowlevel_event(caca_display_t *, caca_event_t *);

#if !defined(_DOXYGEN_SKIP_ME)
/* If no new key was pressed after AUTOREPEAT_THRESHOLD usec, assume the
 * key was released */
#define AUTOREPEAT_THRESHOLD 100000
/* Start repeating key after AUTOREPEAT_TRIGGER usec and send keypress
 * events every AUTOREPEAT_RATE usec. */
#define AUTOREPEAT_TRIGGER 300000
#define AUTOREPEAT_RATE 100000
#endif

/** \brief Get the next mouse or keyboard input event.
 *
 *  Poll the event queue for mouse or keyboard events matching the event
 *  mask and return the first matching event. Non-matching events are
 *  discarded. If \c event_mask is zero, the function returns immediately.
 *
 *  The timeout value tells how long this function needs to wait for an
 *  event. A value of zero returns immediately and the function returns zero
 *  if no more events are pending in the queue. A negative value causes the
 *  function to wait indefinitely until a matching event is received.
 *
 *  If not null, \c ev will be filled with information about the event
 *  received. If null, the function will return but no information about
 *  the event will be sent.
 *
 *  This function never fails.
 *
 *  \param dp The libcaca graphical context.
 *  \param event_mask Bitmask of requested events.
 *  \param timeout A timeout value in microseconds, -1 for blocking behaviour
 *  \param ev A pointer to a caca_event structure, or NULL.
 *  \return 1 if a matching event was received, or 0 if the wait timeouted.
 */
int caca_get_event(caca_display_t *dp, unsigned int event_mask,
                   caca_event_t *ev, int timeout)
{
    caca_event_t dummy_event;
    caca_timer_t timer;
    int usec = 0;

    if(!event_mask)
        return 0;

    if(timeout > 0)
        _caca_getticks(&timer);

    if(ev == NULL)
        ev = &dummy_event;

    for( ; ; )
    {
        int ret = _get_next_event(dp, ev);

        /* If we got the event we wanted, return */
        if(ev->type & event_mask)
            return ret;

        /* If there is no timeout, sleep and try again. */
        if(timeout < 0)
        {
            _caca_sleep(10000);
            continue;
        }

        /* If we timeouted, return an empty event */
        if(usec >= timeout)
        {
            ev->type = CACA_EVENT_NONE;
            return 0;
        }

        /* Otherwise sleep a bit. Our granularity is far too high for values
         * below 10000 microseconds so we cheat a bit. */
        if(usec > 10000)
            _caca_sleep(10000);
        else
            _caca_sleep(1000);

        usec += _caca_getticks(&timer);
    }
}

/** \brief Return the X mouse coordinate.
 *
 *  Return the X coordinate of the mouse position last time
 *  it was detected. This function is not reliable if the ncurses or S-Lang
 *  drivers are being used, because mouse position is only detected when
 *  the mouse is clicked. Other drivers such as X11 work well.
 *
 *  This function never fails.
 *
 *  \param dp The libcaca graphical context.
 *  \return The X mouse coordinate.
 */
unsigned int caca_get_mouse_x(caca_display_t *dp)
{
    if(dp->mouse.x >= dp->cv->width)
        dp->mouse.x = dp->cv->width - 1;

    return dp->mouse.x;
}

/** \brief Return the Y mouse coordinate.
 *
 *  Return the Y coordinate of the mouse position last time
 *  it was detected. This function is not reliable if the ncurses or S-Lang
 *  drivers are being used, because mouse position is only detected when
 *  the mouse is clicked. Other drivers such as X11 work well.
 *
 *  This function never fails.
 *
 *  \param dp The libcaca graphical context.
 *  \return The Y mouse coordinate.
 */
unsigned int caca_get_mouse_y(caca_display_t *dp)
{
    if(dp->mouse.y >= dp->cv->height)
        dp->mouse.y = dp->cv->height - 1;

    return dp->mouse.y;
}

/*
 * XXX: The following functions are local.
 */

static int _get_next_event(caca_display_t *dp, caca_event_t *ev)
{
#if defined(USE_SLANG) || defined(USE_NCURSES)
    unsigned int ticks;
#endif
    int ret;

    /* If we are about to return a resize event, acknowledge it */
    if(dp->resize.resized)
    {
        dp->resize.resized = 0;
        _caca_handle_resize(dp);
        ev->type = CACA_EVENT_RESIZE;
        ev->data.resize.w = dp->cv->width;
        ev->data.resize.h = dp->cv->height;
        return 1;
    }

    ret = _lowlevel_event(dp, ev);

#if defined(USE_SLANG)
    if(dp->drv.driver != CACA_DRIVER_SLANG)
#endif
#if defined(USE_NCURSES)
    if(dp->drv.driver != CACA_DRIVER_NCURSES)
#endif
    return ret;

#if defined(USE_SLANG) || defined(USE_NCURSES)
    /* Simulate long keypresses using autorepeat features */
    ticks = _caca_getticks(&dp->events.key_timer);
    dp->events.last_key_ticks += ticks;
    dp->events.autorepeat_ticks += ticks;

    /* Handle autorepeat */
    if(dp->events.last_key_event.type
           && dp->events.autorepeat_ticks > AUTOREPEAT_TRIGGER
           && dp->events.autorepeat_ticks > AUTOREPEAT_THRESHOLD
           && dp->events.autorepeat_ticks > AUTOREPEAT_RATE)
    {
        _push_event(dp, ev);
        dp->events.autorepeat_ticks -= AUTOREPEAT_RATE;
        *ev = dp->events.last_key_event;
        return 1;
    }

    /* We are in autorepeat mode and the same key was just pressed, ignore
     * this event and return the next one by calling ourselves. */
    if(ev->type == CACA_EVENT_KEY_PRESS
        && dp->events.last_key_event.type
        && ev->data.key.ch == dp->events.last_key_event.data.key.ch
        && ev->data.key.utf32 == dp->events.last_key_event.data.key.utf32)
    {
        dp->events.last_key_ticks = 0;
        return _get_next_event(dp, ev);
    }

    /* We are in autorepeat mode, but key has expired or a new key was
     * pressed - store our event and return a key release event first */
    if(dp->events.last_key_event.type
          && (dp->events.last_key_ticks > AUTOREPEAT_THRESHOLD
               || (ev->type & CACA_EVENT_KEY_PRESS)))
    {
        _push_event(dp, ev);
        *ev = dp->events.last_key_event;
        ev->type = CACA_EVENT_KEY_RELEASE;
        dp->events.last_key_event.type = CACA_EVENT_NONE;
        return 1;
    }

    /* A new key was pressed, enter autorepeat mode */
    if(ev->type & CACA_EVENT_KEY_PRESS)
    {
        dp->events.last_key_ticks = 0;
        dp->events.autorepeat_ticks = 0;
        dp->events.last_key_event = *ev;
    }

    return ev->type ? 1 : 0;
#endif
}

static int _lowlevel_event(caca_display_t *dp, caca_event_t *ev)
{
#if defined(USE_SLANG) || defined(USE_NCURSES) || defined(USE_CONIO)
    int ret = _pop_event(dp, ev);

    if(ret)
        return ret;
#endif

    return dp->drv.get_event(dp, ev);
}

#if defined(USE_SLANG) || defined(USE_NCURSES) || defined(USE_CONIO) || defined(USE_GL)
void _push_event(caca_display_t *dp, caca_event_t *ev)
{
    if(!ev->type || dp->events.queue == EVENTBUF_LEN)
        return;
    dp->events.buf[dp->events.queue] = *ev;
    dp->events.queue++;
}

int _pop_event(caca_display_t *dp, caca_event_t *ev)
{
    int i;

    if(dp->events.queue == 0)
        return 0;

    *ev = dp->events.buf[0];
    for(i = 1; i < dp->events.queue; i++)
        dp->events.buf[i - 1] = dp->events.buf[i];
    dp->events.queue--;

    return 1;
}
#endif

