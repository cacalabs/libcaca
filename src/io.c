/*
 *  libcaca       ASCII-Art library
 *  Copyright (c) 2002, 2003 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA
 */

/** \file io.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Event handling
 *
 *  This file contains event handling functions for keyboard and mouse input.
 */

#include "config.h"

#if defined(USE_SLANG)
#   if defined(HAVE_SLANG_SLANG_H)
#       include <slang/slang.h>
#   else
#       include <slang.h>
#   endif
#endif
#if defined(USE_NCURSES)
#   include <curses.h>
#endif
#if defined(USE_CONIO)
#   include <conio.h>
#endif
#if defined(USE_X11)
#   include <X11/Xlib.h>
#   include <X11/Xutil.h>
#   include <X11/keysym.h>
#endif

#include <unistd.h>

#include "caca.h"
#include "caca_internals.h"

static unsigned int _get_next_event(void);
static void _push_key(unsigned int);
static unsigned int _pop_key(void);
static unsigned int _read_key(void);

#if !defined(_DOXYGEN_SKIP_ME)
#define KEY_BUFLEN 10
#endif
static unsigned int keybuf[KEY_BUFLEN + 1]; /* zero-terminated */
static int keys = 0;

/** \brief Get the next mouse or keyboard input event.
 *
 *  This function polls the event queue for mouse or keyboard events matching
 *  the event mask and returns the first matching event. Non-matching events
 *  are discarded. It is non-blocking and returns zero if no more events are
 *  pending in the queue. See also caca_wait_event() for a blocking version
 *  of this function.
 *
 * \param event_mask Bitmask of requested events.
 * \return The next matching event in the queue, or 0 if no event is pending.
 */
unsigned int caca_get_event(unsigned int event_mask)
{
    for( ; ; )
    {
        unsigned int event = _get_next_event();

        if(!event || event & event_mask)
            return event;
    }
}

/** \brief Wait for the next mouse or keyboard input event.
 *
 *  This function returns the first mouse or keyboard event in the queue
 *  that matches the event mask. If no event is pending, it blocks until a
 *  matching event is received. See also caca_get_event() for a non-blocking
 *  version of this function.
 *
 *  \param event_mask Bitmask of requested events.
 *  \return The next event in the queue.
 */
unsigned int caca_wait_event(unsigned int event_mask)
{
    for( ; ; )
    {
        unsigned int event = _get_next_event();

        if(event & event_mask)
            return event;

        usleep(1000);
    }
}

/*
 * XXX: The following functions are local.
 */

static unsigned int _get_next_event(void)
{
    unsigned int event = 0;

#if defined(USE_X11)
    /* The X11 event check routine */
    if(_caca_driver == CACA_DRIVER_X11)
    {
        XEvent xevent;
        static unsigned int x11_x = 0, x11_y = 0;
        long int xevent_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask
                                | ButtonReleaseMask | PointerMotionMask;
        char key;

        while(XCheckWindowEvent(x11_dpy, x11_window, xevent_mask, &xevent)
               == True)
        {
            KeySym keysym;

            /* Check for mouse motion events */
            if(xevent.type == MotionNotify)
            {
                unsigned int newx = xevent.xmotion.x / x11_font_width;
                unsigned int newy = xevent.xmotion.y / x11_font_height;

                if(x11_x == newx && x11_y == newy)
                    continue;

                if(newx >= _caca_width)
                    newx = _caca_width - 1;
                if(newy >= _caca_height)
                    newy = _caca_height - 1;

                x11_x = newx & 0xfff;
                x11_y = newy & 0xfff;

                return CACA_EVENT_MOUSE_MOTION | (newx << 12) | (newy << 0);
            }

            /* Check for mouse press and release events */
            if(xevent.type == ButtonPress)
                return CACA_EVENT_MOUSE_PRESS
                        | ((XButtonEvent *)&xevent)->button;

            if(xevent.type == ButtonRelease)
                return CACA_EVENT_MOUSE_RELEASE
                        | ((XButtonEvent *)&xevent)->button;

            /* Check for key press and release events */
            if(xevent.type == KeyPress)
                event |= CACA_EVENT_KEY_PRESS;
            else if(xevent.type == KeyRelease)
                event |= CACA_EVENT_KEY_RELEASE;
            else
                continue;

            if(XLookupString(&xevent.xkey, &key, 1, NULL, NULL))
                return event | key;

            keysym = XKeycodeToKeysym(x11_dpy, xevent.xkey.keycode, 0);
            switch(keysym)
            {
            case XK_F1:    return event | CACA_KEY_F1;
            case XK_F2:    return event | CACA_KEY_F2;
            case XK_F3:    return event | CACA_KEY_F3;
            case XK_F4:    return event | CACA_KEY_F4;
            case XK_F5:    return event | CACA_KEY_F5;
            case XK_F6:    return event | CACA_KEY_F6;
            case XK_F7:    return event | CACA_KEY_F7;
            case XK_F8:    return event | CACA_KEY_F8;
            case XK_F9:    return event | CACA_KEY_F9;
            case XK_F10:   return event | CACA_KEY_F10;
            case XK_F11:   return event | CACA_KEY_F11;
            case XK_F12:   return event | CACA_KEY_F12;
            case XK_F13:   return event | CACA_KEY_F13;
            case XK_F14:   return event | CACA_KEY_F14;
            case XK_F15:   return event | CACA_KEY_F15;
            case XK_Left:  return event | CACA_KEY_LEFT;
            case XK_Right: return event | CACA_KEY_RIGHT;
            case XK_Up:    return event | CACA_KEY_UP;
            case XK_Down:  return event | CACA_KEY_DOWN;
            default:       return 0;
            }
        }

        return 0;
    }
#endif

    /* Read all available key events and push them into the buffer */
    while(keys < KEY_BUFLEN)
    {
        unsigned int key = _read_key();
        if(!key)
            break;
        _push_key(key);
    }

    /* If no keys were read, return */
    if(!keys)
        return 0;

#if defined(USE_NCURSES)
    if(_caca_driver == CACA_DRIVER_NCURSES)
    {
        if(keybuf[0] == KEY_MOUSE)
        {
            MEVENT mevent;
            _pop_key();
            getmouse(&mevent);

            switch(mevent.bstate)
            {
                case BUTTON1_PRESSED:
                    _push_key(CACA_EVENT_MOUSE_PRESS | 1);
                    break;
                case BUTTON1_RELEASED:
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 1);
                    break;
                case BUTTON1_CLICKED:
                    _push_key(CACA_EVENT_MOUSE_PRESS | 1);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 1);
                    break;
                case BUTTON1_DOUBLE_CLICKED:
                    _push_key(CACA_EVENT_MOUSE_PRESS | 1);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 1);
                    _push_key(CACA_EVENT_MOUSE_PRESS | 1);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 1);
                    break;
                case BUTTON1_TRIPLE_CLICKED:
                    _push_key(CACA_EVENT_MOUSE_PRESS | 1);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 1);
                    _push_key(CACA_EVENT_MOUSE_PRESS | 1);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 1);
                    _push_key(CACA_EVENT_MOUSE_PRESS | 1);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 1);
                    break;
                case BUTTON1_RESERVED_EVENT:
                    break;

                case BUTTON2_PRESSED:
                    _push_key(CACA_EVENT_MOUSE_PRESS | 2);
                    break;
                case BUTTON2_RELEASED:
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 2);
                    break;
                case BUTTON2_CLICKED:
                    _push_key(CACA_EVENT_MOUSE_PRESS | 2);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 2);
                    break;
                case BUTTON2_DOUBLE_CLICKED:
                    _push_key(CACA_EVENT_MOUSE_PRESS | 2);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 2);
                    _push_key(CACA_EVENT_MOUSE_PRESS | 2);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 2);
                    break;
                case BUTTON2_TRIPLE_CLICKED:
                    _push_key(CACA_EVENT_MOUSE_PRESS | 2);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 2);
                    _push_key(CACA_EVENT_MOUSE_PRESS | 2);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 2);
                    _push_key(CACA_EVENT_MOUSE_PRESS | 2);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 2);
                    break;
                case BUTTON2_RESERVED_EVENT:
                    break;

                case BUTTON3_PRESSED:
                    _push_key(CACA_EVENT_MOUSE_PRESS | 3);
                    break;
                case BUTTON3_RELEASED:
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 3);
                    break;
                case BUTTON3_CLICKED:
                    _push_key(CACA_EVENT_MOUSE_PRESS | 3);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 3);
                    break;
                case BUTTON3_DOUBLE_CLICKED:
                    _push_key(CACA_EVENT_MOUSE_PRESS | 3);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 3);
                    _push_key(CACA_EVENT_MOUSE_PRESS | 3);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 3);
                    break;
                case BUTTON3_TRIPLE_CLICKED:
                    _push_key(CACA_EVENT_MOUSE_PRESS | 3);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 3);
                    _push_key(CACA_EVENT_MOUSE_PRESS | 3);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 3);
                    _push_key(CACA_EVENT_MOUSE_PRESS | 3);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 3);
                    break;
                case BUTTON3_RESERVED_EVENT:
                    break;

                case BUTTON4_PRESSED:
                    _push_key(CACA_EVENT_MOUSE_PRESS | 4);
                    break;
                case BUTTON4_RELEASED:
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 4);
                    break;
                case BUTTON4_CLICKED:
                    _push_key(CACA_EVENT_MOUSE_PRESS | 4);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 4);
                    break;
                case BUTTON4_DOUBLE_CLICKED:
                    _push_key(CACA_EVENT_MOUSE_PRESS | 4);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 4);
                    _push_key(CACA_EVENT_MOUSE_PRESS | 4);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 4);
                    break;
                case BUTTON4_TRIPLE_CLICKED:
                    _push_key(CACA_EVENT_MOUSE_PRESS | 4);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 4);
                    _push_key(CACA_EVENT_MOUSE_PRESS | 4);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 4);
                    _push_key(CACA_EVENT_MOUSE_PRESS | 4);
                    _push_key(CACA_EVENT_MOUSE_RELEASE | 4);
                    break;
                case BUTTON4_RESERVED_EVENT:
                    break;

                default:
                    break;
            }

            return CACA_EVENT_MOUSE_MOTION | (mevent.x << 12) | mevent.y;
        }

        switch(keybuf[0])
        {
            case KEY_UP: event = CACA_EVENT_KEY_PRESS | CACA_KEY_UP; break;
            case KEY_DOWN: event = CACA_EVENT_KEY_PRESS | CACA_KEY_DOWN; break;
            case KEY_LEFT: event = CACA_EVENT_KEY_PRESS | CACA_KEY_LEFT; break;
            case KEY_RIGHT: event = CACA_EVENT_KEY_PRESS | CACA_KEY_RIGHT; break;

            case KEY_F(1): event = CACA_EVENT_KEY_PRESS | CACA_KEY_F1; break;
            case KEY_F(2): event = CACA_EVENT_KEY_PRESS | CACA_KEY_F2; break;
            case KEY_F(3): event = CACA_EVENT_KEY_PRESS | CACA_KEY_F3; break;
            case KEY_F(4): event = CACA_EVENT_KEY_PRESS | CACA_KEY_F4; break;
            case KEY_F(5): event = CACA_EVENT_KEY_PRESS | CACA_KEY_F5; break;
            case KEY_F(6): event = CACA_EVENT_KEY_PRESS | CACA_KEY_F6; break;
            case KEY_F(7): event = CACA_EVENT_KEY_PRESS | CACA_KEY_F7; break;
            case KEY_F(8): event = CACA_EVENT_KEY_PRESS | CACA_KEY_F8; break;
            case KEY_F(9): event = CACA_EVENT_KEY_PRESS | CACA_KEY_F9; break;
            case KEY_F(10): event = CACA_EVENT_KEY_PRESS | CACA_KEY_F10; break;
            case KEY_F(11): event = CACA_EVENT_KEY_PRESS | CACA_KEY_F11; break;
            case KEY_F(12): event = CACA_EVENT_KEY_PRESS | CACA_KEY_F12; break;
        }

        if(event)
        {
            _pop_key();
            if(event & CACA_EVENT_KEY_PRESS)
                _push_key(CACA_EVENT_KEY_RELEASE
                           | (event & ~CACA_EVENT_KEY_PRESS));
            return event;
        }
    }
#endif

    /* If it's already a special event, return it */
    if((keybuf[0] & ~0xff) != 0)
        return _pop_key();

    /* If it's not an escape sequence, return the key */
    if(keybuf[0] != '\x1b')
    {
        event = _pop_key();
        _push_key(CACA_EVENT_KEY_RELEASE | event);
        return CACA_EVENT_KEY_PRESS | event;
    }

    /*
     * Handle known escape sequences
     */

    _pop_key();

    if(keybuf[0] == 'O' && keybuf[1] >= 'P' && keybuf[1] <= 'S')
    {
        /* ^[OP ^[OQ ^[OR ^[OS */
        static unsigned int keylist[] =
            { CACA_KEY_F1, CACA_KEY_F2, CACA_KEY_F3, CACA_KEY_F4 };
        _pop_key();
        event = keylist[_pop_key() - 'P'];
        _push_key(CACA_EVENT_KEY_RELEASE | event);
        return CACA_EVENT_KEY_PRESS | event;
    }
    else if(keybuf[0] == '[' && keybuf[1] >= 'A' && keybuf[1] <= 'D')
    {
        /* ^[[A ^[[B ^[[C ^[[D */
        static unsigned int keylist[] =
            { CACA_KEY_UP, CACA_KEY_DOWN, CACA_KEY_RIGHT, CACA_KEY_LEFT };
        _pop_key();
        event = keylist[_pop_key() - 'A'];
        _push_key(CACA_EVENT_KEY_RELEASE | event);
        return CACA_EVENT_KEY_PRESS | event;
    }
    else if(keybuf[0] == '[' && keybuf[1] == 'M' &&
            keybuf[2] && keybuf[3] && keybuf[3])
    {
        int button;

        /* ^[[Mxxx */
        _pop_key();
        _pop_key();
        button = (1 + _pop_key() - ' ') & 0xf;
        _push_key(CACA_EVENT_MOUSE_PRESS | button);
        _push_key(CACA_EVENT_MOUSE_RELEASE | button);
        return CACA_EVENT_MOUSE_MOTION
                | ((_pop_key() - '!') << 12) | ((_pop_key() - '!') << 0);
    }
    else if(keybuf[0] == '[' && keybuf[1] == '1' && keybuf[3] == '~' &&
            keybuf[2] >= '5' && keybuf[2] != '6' && keybuf[2] <= '9')
    {
        /* ^[[15~ ^[[17~ ^[[18~ ^[[19~ */
        static unsigned int keylist[] =
            { CACA_KEY_F5, 0, CACA_KEY_F6, CACA_KEY_F7, CACA_KEY_F8 };
        _pop_key();
        _pop_key();
        event = keylist[_pop_key() - '5'];
        _pop_key();
        _push_key(CACA_EVENT_KEY_RELEASE | event);
        return CACA_EVENT_KEY_PRESS | event;
    }
    else if(keybuf[0] == '[' && keybuf[1] == '2' && keybuf[3] == '~' &&
            keybuf[2] >= '0' && keybuf[2] != '2' && keybuf[2] <= '4')
    {
        /* ^[[20~ ^[[21~ ^[[23~ ^[[24~ */
        static unsigned int keylist[] =
            { CACA_KEY_F9, CACA_KEY_F10, 0, CACA_KEY_F11, CACA_KEY_F12 };
        _pop_key();
        _pop_key();
        event = keylist[_pop_key() - '0'];
        _pop_key();
        _push_key(CACA_EVENT_KEY_RELEASE | event);
        return CACA_EVENT_KEY_PRESS | event;
    }

    /* Unknown escape sequence: return the ESC key */
    _push_key(CACA_EVENT_KEY_RELEASE | '\x1b');
    return CACA_EVENT_KEY_PRESS | '\x1b';
}

static void _push_key(unsigned int key)
{
    if(keys == KEY_BUFLEN)
        return;
    keybuf[keys] = key;
    keys++;
    keybuf[keys] = 0;
}

static unsigned int _pop_key(void)
{
    int i;
    unsigned int key = keybuf[0];
    keys--;
    for(i = 0; i < keys; i++)
        keybuf[i] = keybuf[i + 1];
    keybuf[keys] = 0;

    return key;
}

static unsigned int _read_key(void)
{
#if defined(USE_NCURSES)
    int intkey;
#endif
#if defined(USE_X11)
#endif

    switch(_caca_driver)
    {
#if defined(USE_SLANG)
    case CACA_DRIVER_SLANG:
        return SLang_input_pending(0) ? SLang_getkey() : 0;
#endif
#if defined(USE_NCURSES)
    case CACA_DRIVER_NCURSES:
        intkey = getch();
        return (intkey == ERR) ? 0 : intkey;
#endif
#if defined(USE_CONIO)
    case CACA_DRIVER_CONIO:
        return _conio_kbhit() ? getch() : 0;
#endif
#if defined(USE_X11)
    case CACA_DRIVER_X11:
#endif
    default:
        break;
    }

    return 0;
}

