/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file event.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Event handling
 *
 *  This file contains event handling functions for keyboard and mouse input.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"
#include "caca.h"
#include "caca_internals.h"

static int _get_next_event(caca_t *, struct caca_event *);
static int _lowlevel_event(caca_t *, struct caca_event *);

#if !defined(_DOXYGEN_SKIP_ME)
/* If no new key was pressed after AUTOREPEAT_THRESHOLD usec, assume the
 * key was released */
#define AUTOREPEAT_THRESHOLD 200000
/* Start repeating key after AUTOREPEAT_TRIGGER usec and send keypress
 * events every AUTOREPEAT_RATE usec. */
#define AUTOREPEAT_TRIGGER 300000
#define AUTOREPEAT_RATE 100000
#endif

/** \brief Get the next mouse or keyboard input event.
 *
 *  This function polls the event queue for mouse or keyboard events matching
 *  the event mask and returns the first matching event. Non-matching events
 *  are discarded. \c event_mask must have a non-zero value.
 *
 *  The timeout value tells how long this function needs to wait for an
 *  event. A value of zero returns immediately and the function returns zero
 *  if no more events are pending in the queue. A negative value causes the
 *  function to wait indefinitely until a matching event is received.
 *
 * \param event_mask Bitmask of requested events.
 * \param timeout A timeout value in microseconds
 * \return The next matching event in the queue, or 0 if no event is pending.
 */
int caca_get_event(caca_t *kk, unsigned int event_mask,
                   struct caca_event *ev, int timeout)
{
    struct caca_timer timer;
    int usec = 0;

    if(!event_mask)
        return 0;

    if(timeout > 0)
        _caca_getticks(&timer);

    for( ; ; )
    {
        int ret = _get_next_event(kk, ev);

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
 *  This function returns the X coordinate of the mouse position last time
 *  it was detected. This function is not reliable if the ncurses or S-Lang
 *  drivers are being used, because mouse position is only detected when
 *  the mouse is clicked. Other drivers such as X11 work well.
 *
 *  \return The X mouse coordinate.
 */
unsigned int caca_get_mouse_x(caca_t *kk)
{
    if(kk->mouse.x >= kk->qq->width)
        kk->mouse.x = kk->qq->width - 1;

    return kk->mouse.x;
}

/** \brief Return the Y mouse coordinate.
 *
 *  This function returns the Y coordinate of the mouse position last time
 *  it was detected. This function is not reliable if the ncurses or S-Lang
 *  drivers are being used, because mouse position is only detected when
 *  the mouse is clicked. Other drivers such as X11 work well.
 *
 *  \return The Y mouse coordinate.
 */
unsigned int caca_get_mouse_y(caca_t *kk)
{
    if(kk->mouse.y >= kk->qq->height)
        kk->mouse.y = kk->qq->height - 1;

    return kk->mouse.y;
}

/*
 * XXX: The following functions are local.
 */

static int _get_next_event(caca_t *kk, struct caca_event *ev)
{
#if defined(USE_SLANG) || defined(USE_NCURSES)
    unsigned int ticks;
#endif
    int ret;

    /* If we are about to return a resize event, acknowledge it */
    if(kk->resize.resized)
    {
        kk->resize.resized = 0;
        _caca_handle_resize(kk);
        ev->type = CACA_EVENT_RESIZE;
        ev->data.resize.w = kk->qq->width;
        ev->data.resize.h = kk->qq->height;
        return 1;
    }

    ret = _lowlevel_event(kk, ev);

#if defined(USE_SLANG)
    if(kk->drv.driver != CACA_DRIVER_SLANG)
#endif
#if defined(USE_NCURSES)
    if(kk->drv.driver != CACA_DRIVER_NCURSES)
#endif
    return ret;

#if defined(USE_SLANG) || defined(USE_NCURSES)
    /* Simulate long keypresses using autorepeat features */
    ticks = _caca_getticks(&kk->events.key_timer);
    kk->events.last_key_ticks += ticks;
    kk->events.autorepeat_ticks += ticks;

    /* Handle autorepeat */
    if(kk->events.last_key_event.type
           && kk->events.autorepeat_ticks > AUTOREPEAT_TRIGGER
           && kk->events.autorepeat_ticks > AUTOREPEAT_THRESHOLD
           && kk->events.autorepeat_ticks > AUTOREPEAT_RATE)
    {
        _push_event(kk, ev);
        kk->events.autorepeat_ticks -= AUTOREPEAT_RATE;
        *ev = kk->events.last_key_event;
        return 1;
    }

    /* We are in autorepeat mode and the same key was just pressed, ignore
     * this event and return the next one by calling ourselves. */
    if(ev->type == CACA_EVENT_KEY_PRESS
        && kk->events.last_key_event.type
        && ev->data.key.c == kk->events.last_key_event.data.key.c
        && ev->data.key.ucs4 == kk->events.last_key_event.data.key.ucs4)
    {
        kk->events.last_key_ticks = 0;
        return _get_next_event(kk, ev);
    }

    /* We are in autorepeat mode, but key has expired or a new key was
     * pressed - store our event and return a key release event first */
    if(kk->events.last_key_event.type
          && (kk->events.last_key_ticks > AUTOREPEAT_THRESHOLD
               || (ev->type & CACA_EVENT_KEY_PRESS)))
    {
        _push_event(kk, ev);
        *ev = kk->events.last_key_event;
        ev->type = CACA_EVENT_KEY_RELEASE;
        kk->events.last_key_event.type = CACA_EVENT_NONE;
        return 1;
    }

    /* A new key was pressed, enter autorepeat mode */
    if(ev->type & CACA_EVENT_KEY_PRESS)
    {
        kk->events.last_key_ticks = 0;
        kk->events.autorepeat_ticks = 0;
        kk->events.last_key_event = *ev;
    }

    return ev->type ? 1 : 0;
#endif
}

static int _lowlevel_event(caca_t *kk, struct caca_event *ev)
{
#if defined(USE_SLANG) || defined(USE_NCURSES) || defined(USE_CONIO)
    int ret = _pop_event(kk, ev);

    if(ret)
        return ret;
#endif

    return kk->drv.get_event(kk, ev);
}

#if defined(USE_SLANG) || defined(USE_NCURSES) || defined(USE_CONIO) || defined(USE_GL)
void _push_event(caca_t *kk, struct caca_event *ev)
{
    if(!ev->type || kk->events.queue == EVENTBUF_LEN)
        return;
    kk->events.buf[kk->events.queue] = *ev;
    kk->events.queue++;
}

int _pop_event(caca_t *kk, struct caca_event *ev)
{
    int i;

    if(kk->events.queue == 0)
        return 0;

    *ev = kk->events.buf[0];
    for(i = 1; i < kk->events.queue; i++)
        kk->events.buf[i - 1] = kk->events.buf[i];
    kk->events.queue--;

    return 1;
}
#endif

