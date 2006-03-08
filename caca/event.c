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

/** \file event.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Event handling
 *
 *  This file contains event handling functions for keyboard and mouse input.
 */

#include "config.h"

#include "cucul.h"
#include "cucul_internals.h"
#include "caca.h"
#include "caca_internals.h"

static unsigned int _get_next_event(caca_t *);
static unsigned int _lowlevel_event(caca_t *);
#if defined(USE_SLANG) || defined(USE_NCURSES) || defined(USE_CONIO)
void _push_event(caca_t *, unsigned int);
unsigned int _pop_event(caca_t *);
#endif

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
 *  are discarded. \c event_mask must have a non-zero value. This function is
 *  non-blocking and returns zero if no more events are pending in the queue.
 *  See also caca_wait_event() for a blocking version of this function.
 *
 * \param event_mask Bitmask of requested events.
 * \return The next matching event in the queue, or 0 if no event is pending.
 */
unsigned int caca_get_event(caca_t *kk, unsigned int event_mask)
{
    if(!event_mask)
        return CACA_EVENT_NONE;

    for( ; ; )
    {
        unsigned int event = _get_next_event(kk);

        if(!event || event & event_mask)
            return event;
    }
}

/** \brief Wait for the next mouse or keyboard input event.
 *
 *  This function returns the first mouse or keyboard event in the queue
 *  that matches the event mask. If no event is pending, it blocks until a
 *  matching event is received. \c event_mask must have a non-zero value.
 *  See also caca_get_event() for a non-blocking version of this function.
 *
 *  \param event_mask Bitmask of requested events.
 *  \return The next event in the queue.
 */
unsigned int caca_wait_event(caca_t *kk, unsigned int event_mask)
{
    if(!event_mask)
        return CACA_EVENT_NONE;

    for( ; ; )
    {
        unsigned int event = _get_next_event(kk);

        if(event & event_mask)
            return event;

        _caca_sleep(10000);
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

static unsigned int _get_next_event(caca_t *kk)
{
#if defined(USE_SLANG) || defined(USE_NCURSES)
    unsigned int ticks;
#endif
    unsigned int event;

    /* If we are about to return a resize event, acknowledge it */
    if(kk->resize.resized)
    {
        kk->resize.resized = 0;
        _caca_handle_resize(kk);
        return CACA_EVENT_RESIZE;
    }

    event = _lowlevel_event(kk);

#if defined(USE_SLANG)
    if(kk->drv.driver != CACA_DRIVER_SLANG)
#endif
#if defined(USE_NCURSES)
    if(kk->drv.driver != CACA_DRIVER_NCURSES)
#endif
    return event;

#if defined(USE_SLANG) || defined(USE_NCURSES)
    /* Simulate long keypresses using autorepeat features */
    ticks = _caca_getticks(&kk->events.key_timer);
    kk->events.last_key_ticks += ticks;
    kk->events.autorepeat_ticks += ticks;

    /* Handle autorepeat */
    if(kk->events.last_key
           && kk->events.autorepeat_ticks > AUTOREPEAT_TRIGGER
           && kk->events.autorepeat_ticks > AUTOREPEAT_THRESHOLD
           && kk->events.autorepeat_ticks > AUTOREPEAT_RATE)
    {
        _push_event(kk, event);
        kk->events.autorepeat_ticks -= AUTOREPEAT_RATE;
        return CACA_EVENT_KEY_PRESS | kk->events.last_key;
    }

    /* We are in autorepeat mode and the same key was just pressed, ignore
     * this event and return the next one by calling ourselves. */
    if(event == (CACA_EVENT_KEY_PRESS | kk->events.last_key))
    {
        kk->events.last_key_ticks = 0;
        return _get_next_event(kk);
    }

    /* We are in autorepeat mode, but key has expired or a new key was
     * pressed - store our event and return a key release event first */
    if(kk->events.last_key
          && (kk->events.last_key_ticks > AUTOREPEAT_THRESHOLD
               || (event & CACA_EVENT_KEY_PRESS)))
    {
        _push_event(kk, event);
        event = CACA_EVENT_KEY_RELEASE | kk->events.last_key;
        kk->events.last_key = 0;
        return event;
    }

    /* A new key was pressed, enter autorepeat mode */
    if(event & CACA_EVENT_KEY_PRESS)
    {
        kk->events.last_key_ticks = 0;
        kk->events.autorepeat_ticks = 0;
        kk->events.last_key = event & 0x00ffffff;
    }

    return event;
#endif
}

static unsigned int _lowlevel_event(caca_t *kk)
{
#if defined(USE_SLANG) || defined(USE_NCURSES) || defined(USE_CONIO)
    unsigned int event = _pop_event(kk);

    if(event)
        return event;
#endif

    return kk->drv.get_event(kk);
}

#if defined(USE_SLANG) || defined(USE_NCURSES) || defined(USE_CONIO)
void _push_event(caca_t *kk, unsigned int event)
{
    if(!event || kk->events.queue == EVENTBUF_LEN)
        return;
    kk->events.buf[kk->events.queue] = event;
    kk->events.queue++;
}

unsigned int _pop_event(caca_t *kk)
{
    int i;
    unsigned int event;

    if(kk->events.queue == 0)
        return CACA_EVENT_NONE;

    event = kk->events.buf[0];
    for(i = 1; i < kk->events.queue; i++)
        kk->events.buf[i - 1] = kk->events.buf[i];
    kk->events.queue--;

    return event;
}
#endif

