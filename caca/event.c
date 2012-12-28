/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains event handling functions for keyboard and mouse input.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <string.h>
#endif

#include "caca.h"
#include "caca_internals.h"

static int _get_next_event(caca_display_t *, caca_privevent_t *);
static int _lowlevel_event(caca_display_t *, caca_privevent_t *);

#if !defined(_DOXYGEN_SKIP_ME)
/* If no new key was pressed after AUTOREPEAT_THRESHOLD usec, assume the
 * key was released */
#define AUTOREPEAT_THRESHOLD 100000
/* Start repeating key after AUTOREPEAT_TRIGGER usec and send keypress
 * events every AUTOREPEAT_RATE usec. */
#define AUTOREPEAT_TRIGGER 300000
#define AUTOREPEAT_RATE 20000
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
int caca_get_event(caca_display_t *dp, int event_mask,
                   caca_event_t *ev, int timeout)
{
    caca_privevent_t privevent;
    caca_timer_t timer = {0, 0};
#if defined PROF
    caca_timer_t proftimer = {0, 0};
    int profsys = 0, profwait = 0;
#endif
    int ret = 0, usec = 0;

#if defined PROF
    _caca_getticks(&proftimer);
#endif

    if(!event_mask)
        goto end;

    if(timeout > 0)
        _caca_getticks(&timer);

    for( ; ; )
    {
#if defined PROF
        profwait += _caca_getticks(&proftimer);
#endif
        ret = _get_next_event(dp, &privevent);
#if defined PROF
        profsys += _caca_getticks(&proftimer);
#endif

        /* If we got the event we wanted, return */
        if(privevent.type & event_mask)
        {
            if(ev)
                memcpy(ev, &privevent, sizeof(privevent));
            goto end;
        }

        /* If there is no timeout, sleep and try again. */
        if(timeout < 0)
        {
            _caca_sleep(1000);
            continue;
        }

        /* If we timeouted, return an empty event */
        if(usec >= timeout)
        {
            privevent.type = CACA_EVENT_NONE;
            if(ev)
                memcpy(ev, &privevent, sizeof(privevent));
            ret = 0;
            goto end;
        }

        /* Otherwise sleep a bit. Our granularity is far too high for values
         * below 10000 microseconds so we cheat a bit. */
        _caca_sleep(usec > 10000 ? 10000 : 1000);

        usec += _caca_getticks(&timer);
    }

end:
#if defined PROF
    profwait += _caca_getticks(&proftimer);
    STAT_IADD(&dp->ev_sys_stat, profsys);
    STAT_IADD(&dp->ev_wait_stat, profwait);
#endif

    return ret;
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
int caca_get_mouse_x(caca_display_t const *dp)
{
    int width = caca_get_canvas_width(dp->cv);

    if(dp->mouse.x >= width)
        return width - 1;

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
int caca_get_mouse_y(caca_display_t const *dp)
{
    int height = caca_get_canvas_height(dp->cv);

    if(dp->mouse.y >= height)
        return height - 1;

    return dp->mouse.y;
}

/** \brief Return an event's type.
 *
 *  Return the type of an event. This function may always be called on an
 *  event after caca_get_event() was called, and its return value indicates
 *  which other functions may be called:
 *  - \c CACA_EVENT_NONE: no other function may be called.
 *  - \c CACA_EVENT_KEY_PRESS, \c CACA_EVENT_KEY_RELEASE:
 *  caca_get_event_key_ch(), caca_get_event_key_utf32() and
 *  caca_get_event_key_utf8() may be called.
 *  - \c CACA_EVENT_MOUSE_PRESS, \c CACA_EVENT_MOUSE_RELEASE:
 *  caca_get_event_mouse_button() may be called.
 *  - \c CACA_EVENT_MOUSE_MOTION: caca_get_event_mouse_x() and
 *  caca_get_event_mouse_y() may be called.
 *  - \c CACA_EVENT_RESIZE: caca_get_event_resize_width() and
 *  caca_get_event_resize_height() may be called.
 *  - \c CACA_EVENT_QUIT: no other function may be called.
 *
 *  This function never fails.
 *
 *  \param ev The libcaca event.
 *  \return The event's type.
 */
enum caca_event_type caca_get_event_type(caca_event_t const *ev)
{
    return ((caca_privevent_t const *)ev)->type;
}

/** \brief Return a key press or key release event's value
 *
 *  Return either the ASCII value for an event's key, or if the key is not
 *  an ASCII character, an appropriate \e enum \e caca_key value.
 *
 *  This function never fails, but must only be called with a valid event of
 *  type \c CACA_EVENT_KEY_PRESS or \c CACA_EVENT_KEY_RELEASE, or the results
 *  will be undefined. See caca_get_event_type() for more information.
 *
 *  \param ev The libcaca event.
 *  \return The key value.
 */
int caca_get_event_key_ch(caca_event_t const *ev)
{
    return ((caca_privevent_t const *)ev)->data.key.ch;
}

/** \brief Return a key press or key release event's Unicode value
 *
 *  Return the UTF-32/UCS-4 value for an event's key if it resolves to a
 *  printable character.
 *
 *  This function never fails, but must only be called with a valid event of
 *  type \c CACA_EVENT_KEY_PRESS or \c CACA_EVENT_KEY_RELEASE, or the results
 *  will be undefined. See caca_get_event_type() for more information.
 *
 *  \param ev The libcaca event.
 *  \return The key's Unicode value.
 */
uint32_t caca_get_event_key_utf32(caca_event_t const *ev)
{
    return ((caca_privevent_t const *)ev)->data.key.utf32;
}

/** \brief Return a key press or key release event's UTF-8 value
 *
 *  Write the UTF-8 value for an event's key if it resolves to a printable
 *  character. Up to 6 UTF-8 bytes and a null termination are written.
 *
 *  This function never fails, but must only be called with a valid event of
 *  type \c CACA_EVENT_KEY_PRESS or \c CACA_EVENT_KEY_RELEASE, or the results
 *  will be undefined. See caca_get_event_type() for more information.
 *
 *  \param ev The libcaca event.
 *  \param utf8 A string buffer with enough bytes to hold the pressed
 *              key value in UTF-8. Though fewer bytes may be written to
 *              it, 7 bytes is the minimum safe size.
 *  \return This function always returns 0.
 */
int caca_get_event_key_utf8(caca_event_t const *ev, char *utf8)
{
    memcpy(utf8, ((caca_privevent_t const *)ev)->data.key.utf8, 8);
    return 0;
}

/** \brief Return a mouse press or mouse release event's button
 *
 *  Return the mouse button index for an event.
 *
 *  This function never fails, but must only be called with a valid event of
 *  type \c CACA_EVENT_MOUSE_PRESS or \c CACA_EVENT_MOUSE_RELEASE, or the
 *  results will be undefined. See caca_get_event_type() for more information.
 *
 *  This function returns 1 for the left mouse button, 2 for the right mouse
 *  button, and 3 for the middle mouse button.
 *
 *  \param ev The libcaca event.
 *  \return The event's mouse button.
 */
int caca_get_event_mouse_button(caca_event_t const *ev)
{
    return ((caca_privevent_t const *)ev)->data.mouse.button;
}

/** \brief Return a mouse motion event's X coordinate.
 *
 *  Return the X coordinate for a mouse motion event.
 *
 *  This function never fails, but must only be called with a valid event of
 *  type \c CACA_EVENT_MOUSE_MOTION, or the results will be undefined. See
 *  caca_get_event_type() for more information.
 *
 *  \param ev The libcaca event.
 *  \return The event's X mouse coordinate.
 */
int caca_get_event_mouse_x(caca_event_t const *ev)
{
    return ((caca_privevent_t const *)ev)->data.mouse.x;
}

/** \brief Return a mouse motion event's Y coordinate.
 *
 *  Return the Y coordinate for a mouse motion event.
 *
 *  This function never fails, but must only be called with a valid event of
 *  type \c CACA_EVENT_MOUSE_MOTION, or the results will be undefined. See
 *  caca_get_event_type() for more information.
 *
 *  \param ev The libcaca event.
 *  \return The event's Y mouse coordinate.
 */
int caca_get_event_mouse_y(caca_event_t const *ev)
{
    return ((caca_privevent_t const *)ev)->data.mouse.y;
}

/** \brief Return a resize event's display width value.
 *
 *  Return the width value for a display resize event.
 *
 *  This function never fails, but must only be called with a valid event of
 *  type \c CACA_EVENT_RESIZE, or the results will be undefined. See
 *  caca_get_event_type() for more information.
 *
 *  \param ev The libcaca event.
 *  \return The event's new display width value.
 */
int caca_get_event_resize_width(caca_event_t const *ev)
{
    return ((caca_privevent_t const *)ev)->data.resize.w;
}

/** \brief Return a resize event's display height value.
 *
 *  Return the height value for a display resize event.
 *
 *  This function never fails, but must only be called with a valid event of
 *  type \c CACA_EVENT_RESIZE, or the results will be undefined. See
 *  caca_get_event_type() for more information.
 *
 *  \param ev The libcaca event.
 *  \return The event's new display height value.
 */
int caca_get_event_resize_height(caca_event_t const *ev)
{
    return ((caca_privevent_t const *)ev)->data.resize.h;
}

/*
 * XXX: The following functions are local.
 */

static int _get_next_event(caca_display_t *dp, caca_privevent_t *ev)
{
#if defined(USE_SLANG) || defined(USE_NCURSES)
    int ticks;
#endif
    int ret;

    /* If we are about to return a resize event, acknowledge it */
    if(dp->resize.resized)
    {
        dp->resize.resized = 0;
        _caca_handle_resize(dp);
        ev->type = CACA_EVENT_RESIZE;
        ev->data.resize.w = caca_get_canvas_width(dp->cv);
        ev->data.resize.h = caca_get_canvas_height(dp->cv);
        return 1;
    }

    ret = _lowlevel_event(dp, ev);

#if defined(USE_SLANG)
    if(dp->drv.id != CACA_DRIVER_SLANG)
#endif
#if defined(USE_NCURSES)
    if(dp->drv.id != CACA_DRIVER_NCURSES)
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

static int _lowlevel_event(caca_display_t *dp, caca_privevent_t *ev)
{
#if defined(USE_SLANG) || defined(USE_NCURSES) || defined(USE_CONIO)
    int ret = _pop_event(dp, ev);

    if(ret)
        return ret;
#endif

    return dp->drv.get_event(dp, ev);
}

#if defined(USE_SLANG) || defined(USE_NCURSES) || defined(USE_CONIO) || defined(USE_GL)
void _push_event(caca_display_t *dp, caca_privevent_t *ev)
{
    if(!ev->type || dp->events.queue == EVENTBUF_LEN)
        return;
    dp->events.buf[dp->events.queue] = *ev;
    dp->events.queue++;
}

int _pop_event(caca_display_t *dp, caca_privevent_t *ev)
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

