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

/** \file caca.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Main \e libcaca functions
 *
 *  This file contains the main functions used by \e libcaca applications to
 *  initialise the library, get the screen properties, set the framerate and
 *  so on.
 */

#include "config.h"

#if defined(USE_SLANG)
#   include <slang.h>
#endif
#if defined(USE_NCURSES)
#   include <curses.h>
#endif
#if defined(USE_CONIO)
#   include <dos.h>
#   include <conio.h>
#endif
#if defined(USE_X11)
#   include <X11/Xlib.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "caca.h"
#include "caca_internals.h"

static void caca_init_driver(void);
static void caca_init_features(void);
static void caca_init_terminal(void);

enum caca_driver _caca_driver;

#if defined(USE_NCURSES)
static mmask_t oldmask;
#endif

/** \brief Initialise \e libcaca.
 *
 *  This function initialises internal \e libcaca structures and the backend
 *  that will be used for subsequent graphical operations. It must be the
 *  first \e libcaca function to be called in a function. caca_end() should
 *  be called at the end of the program to free all allocated resources.
 *
 *  \return 0 upon success, a non-zero value if an error occurs.
 */
int caca_init(void)
{
#if defined(USE_NCURSES)
    mmask_t newmask;
#endif

    caca_init_driver();

    if(_caca_driver == CACA_DRIVER_NONE)
        return -1;

    caca_init_features();
    caca_init_terminal();

#if defined(USE_SLANG)
    if(_caca_driver == CACA_DRIVER_SLANG)
    {
        /* Initialise slang library */
        SLsig_block_signals();
        SLtt_get_terminfo();

        if(SLkp_init() == -1)
        {
            SLsig_unblock_signals();
            return -1;
        }

        SLang_init_tty(-1, 0, 1);

        if(SLsmg_init_smg() == -1)
        {
            SLsig_unblock_signals();
            return -1;
        }

        SLsig_unblock_signals();

        SLsmg_cls();
        SLtt_set_cursor_visibility(0);
        SLkp_define_keysym("\e[M", 1001);
        SLtt_set_mouse_mode(1, 0);
        SLsmg_refresh();

        /* Disable scrolling so that hashmap scrolling optimization code
         * does not cause ugly refreshes due to slow terminals */
        SLtt_Term_Cannot_Scroll = 1;
    }
    else
#endif
#if defined(USE_NCURSES)
    if(_caca_driver == CACA_DRIVER_NCURSES)
    {
        initscr();
        keypad(stdscr, TRUE);
        nonl();
        cbreak();
        noecho();
        nodelay(stdscr, TRUE);
        curs_set(0);

        /* Activate mouse */
        newmask = ALL_MOUSE_EVENTS;
        mousemask(newmask, &oldmask);
    }
    else
#endif
#if defined(USE_CONIO)
    if(_caca_driver == CACA_DRIVER_CONIO)
    {
        _wscroll = 0;
        _setcursortype(_NOCURSOR);
        clrscr();
    }
    else
#endif
#if defined(USE_X11)
    /* Nothing to do */
#endif
    {
        /* Dummy */
    }

    if(_caca_init_graphics())
        return -1;

    return 0;
}

/** \brief Get the screen width.
 *
 *  This function returns the current screen width, in character cells.
 *
 *  \return The screen width.
 */
unsigned int caca_get_width(void)
{
    return _caca_width;
}

/** \brief Get the screen height.
 *
 *  This function returns the current screen height, in character cells.
 *
 *  \return The screen height.
 */
unsigned int caca_get_height(void)
{
    return _caca_height;
}

/** \brief Translate a colour index into the colour's name.
 *
 *  This function translates a caca_color enum into a human-readable
 *  description string of the associated colour.
 *
 *  \param color The colour value.
 *  \return A static string containing the colour's name.
 */
char const *caca_get_color_name(enum caca_color color)
{
    static char const *color_names[] =
    {
        "black",
        "blue",
        "green",
        "cyan",
        "red",
        "magenta",
        "brown",
        "light gray",
        "dark gray",
        "light blue",
        "light green",
        "light cyan",
        "light red",
        "light magenta",
        "yellow",
        "white",
    };

    if(color < 0 || color > 15)
        return "unknown";

    return color_names[color];
}

/** \brief Get the current value of a feature.
 *
 *  This function retrieves the value of an internal \e libcaca feature. A
 *  generic feature value is expected, such as CACA_ANTIALIASING.
 *
 *  \param feature The requested feature.
 *  \return The current value of the feature or CACA_UNKNOWN_FEATURE if an
 *          error occurred..
 */
enum caca_feature caca_get_feature(enum caca_feature feature)
{
    switch(feature)
    {
        case CACA_BACKGROUND:
            return _caca_background;
        case CACA_ANTIALIASING:
            return _caca_antialiasing;
        case CACA_DITHERING:
            return _caca_dithering;

        default:
            return CACA_UNKNOWN_FEATURE;
    }
}

/** \brief Set a feature.
 *
 *  This function sets an internal \e libcaca feature such as the antialiasing
 *  or dithering modes. If a specific feature such as CACA_DITHERING_RANDOM,
 *  caca_set_feature() will set it immediately. If a generic feature is given
 *  instead, such as CACA_DITHERING, the default value will be used instead.
 *
 *  \param feature The requested feature.
 */
void caca_set_feature(enum caca_feature feature)
{
    switch(feature)
    {
        case CACA_BACKGROUND:
            feature = CACA_BACKGROUND_SOLID;
        case CACA_BACKGROUND_BLACK:
        case CACA_BACKGROUND_SOLID:
            _caca_background = feature;
            break;

        case CACA_ANTIALIASING:
            feature = CACA_ANTIALIASING_PREFILTER;
        case CACA_ANTIALIASING_NONE:
        case CACA_ANTIALIASING_PREFILTER:
            _caca_antialiasing = feature;
            break;

        case CACA_DITHERING:
            feature = CACA_DITHERING_ORDERED4;
        case CACA_DITHERING_NONE:
        case CACA_DITHERING_ORDERED2:
        case CACA_DITHERING_ORDERED4:
        case CACA_DITHERING_ORDERED8:
        case CACA_DITHERING_RANDOM:
            _caca_dithering = feature;
            break;

        case CACA_UNKNOWN_FEATURE:
            break;
    }
}

/** \brief Translate a feature value into the feature's name.
 *
 *  This function translates a caca_feature enum into a human-readable
 *  description string of the associated feature.
 *
 *  \param feature The feature value.
 *  \return A static string containing the feature's name.
 */
char const *caca_get_feature_name(enum caca_feature feature)
{
    switch(feature)
    {
        case CACA_BACKGROUND_BLACK: return "black background";
        case CACA_BACKGROUND_SOLID: return "solid background";

        case CACA_ANTIALIASING_NONE:      return "no antialiasing";
        case CACA_ANTIALIASING_PREFILTER: return "prefilter antialiasing";

        case CACA_DITHERING_NONE:     return "no dithering";
        case CACA_DITHERING_ORDERED2: return "2x2 ordered dithering";
        case CACA_DITHERING_ORDERED4: return "4x4 ordered dithering";
        case CACA_DITHERING_ORDERED8: return "8x8 ordered dithering";
        case CACA_DITHERING_RANDOM:   return "random dithering";

        default: return "unknown";
    }
}

/** \brief Uninitialise \e libcaca.
 *
 *  This function frees all resources allocated by caca_init(). After
 *  caca_end() has been called, no other \e libcaca functions may be used
 *  unless a new call to caca_init() is done.
 */
void caca_end(void)
{
    _caca_end_graphics();

#if defined(USE_SLANG)
    if(_caca_driver == CACA_DRIVER_SLANG)
    {
        SLtt_set_mouse_mode(0, 0);
        SLtt_set_cursor_visibility(1);
        SLang_reset_tty();
        SLsmg_reset_smg();
    }
    else
#endif
#if defined(USE_NCURSES)
    if(_caca_driver == CACA_DRIVER_NCURSES)
    {
        mousemask(oldmask, NULL);
        curs_set(1);
        endwin();
    }
    else
#endif
#if defined(USE_CONIO)
    if(_caca_driver == CACA_DRIVER_CONIO)
    {
        _wscroll = 1;
        textcolor((enum COLORS)WHITE);
        textbackground((enum COLORS)BLACK);
        gotoxy(_caca_width, _caca_height);
        cputs("\r\n");
        _setcursortype(_NORMALCURSOR);
    }
    else
#endif
#if defined(USE_X11)
    if(_caca_driver == CACA_DRIVER_X11)
    {
        /* Nothing to do */
    }
    else
#endif
    {
        /* Dummy */
    }
}

/*
 * XXX: The following functions are local.
 */

static void caca_init_driver(void)
{
#if defined(HAVE_GETENV) && defined(HAVE_STRCASECMP)
    char *var = getenv("CACA_DRIVER");

    /* If the environment variable was set, use it */
    if(var && *var)
    {
#if defined(USE_CONIO)
        if(!strcasecmp(var, "conio"))
            _caca_driver = CACA_DRIVER_CONIO;
        else
#endif
#if defined(USE_X11)
        if(!strcasecmp(var, "x11"))
            _caca_driver = CACA_DRIVER_X11;
        else
#endif
#if defined(USE_SLANG)
        if(!strcasecmp(var, "slang"))
            _caca_driver = CACA_DRIVER_SLANG;
        else
#endif
#if defined(USE_NCURSES)
        if(!strcasecmp(var, "ncurses"))
            _caca_driver = CACA_DRIVER_NCURSES;
        else
#endif
            _caca_driver = CACA_DRIVER_NONE;

        return;
    }
#endif

#if defined(USE_CONIO)
    _caca_driver = CACA_DRIVER_CONIO;
    return;
#endif
#if defined(USE_X11)
#if defined(HAVE_GETENV)
    if(getenv("DISPLAY") && *(getenv("DISPLAY")))
#endif
    {
        _caca_driver = CACA_DRIVER_X11;
        return;
    }
#endif
#if defined(USE_SLANG)
    _caca_driver = CACA_DRIVER_SLANG;
    return;
#endif
#if defined(USE_NCURSES)
    _caca_driver = CACA_DRIVER_NCURSES;
    return;
#endif
    _caca_driver = CACA_DRIVER_NONE;
    return;
}

static void caca_init_features(void)
{
    /* FIXME: if strcasecmp isn't available, use strcmp */
#if defined(HAVE_GETENV) && defined(HAVE_STRCASECMP)
    char *var;
#endif

    caca_set_feature(CACA_BACKGROUND);
    caca_set_feature(CACA_ANTIALIASING);
    caca_set_feature(CACA_DITHERING);

#if defined(HAVE_GETENV) && defined(HAVE_STRCASECMP)
    if((var = getenv("CACA_BACKGROUND")) && *var)
    {
        if(!strcasecmp("black", var))
            caca_set_feature(CACA_BACKGROUND_BLACK);
        else if(!strcasecmp("solid", var))
            caca_set_feature(CACA_BACKGROUND_SOLID);
    }

    if((var = getenv("CACA_ANTIALIASING")) && *var)
    {
        if(!strcasecmp("none", var))
            caca_set_feature(CACA_ANTIALIASING_NONE);
        else if(!strcasecmp("prefilter", var))
            caca_set_feature(CACA_ANTIALIASING_PREFILTER);
    }

    if((var = getenv("CACA_DITHERING")) && *var)
    {
        if(!strcasecmp("none", var))
            caca_set_feature(CACA_DITHERING_NONE);
        else if(!strcasecmp("ordered2", var))
            caca_set_feature(CACA_DITHERING_ORDERED2);
        else if(!strcasecmp("ordered4", var))
            caca_set_feature(CACA_DITHERING_ORDERED4);
        else if(!strcasecmp("ordered8", var))
            caca_set_feature(CACA_DITHERING_ORDERED8);
        else if(!strcasecmp("random", var))
            caca_set_feature(CACA_DITHERING_RANDOM);
    }
#endif
}

static void caca_init_terminal(void)
{
#if defined(HAVE_GETENV) && defined(HAVE_PUTENV)
    char *term, *colorterm, *other;

#if defined(USE_SLANG)
    if(_caca_driver != CACA_DRIVER_SLANG)
#endif
#if defined(USE_NCURSES)
    if(_caca_driver != CACA_DRIVER_NCURSES)
#endif
    return;

    term = getenv("TERM");
    colorterm = getenv("COLORTERM");

    if(term && !strcmp(term, "xterm"))
    {
        /* If we are using gnome-terminal, it's really a 16 colour terminal */
        if(colorterm && !strcmp(colorterm, "gnome-terminal"))
        {
#if defined(USE_NCURSES)
            if(_caca_driver == CACA_DRIVER_NCURSES)
            {
                SCREEN *screen;
                screen = newterm("xterm-16color", stdout, stdin);
                if(screen == NULL)
                    return;
                endwin();
            }
#endif
            (void)putenv("TERM=xterm-16color");
            return;
        }

        /* Ditto if we are using Konsole */
        other = getenv("KONSOLE_DCOP_SESSION");
        if(other)
        {
#if defined(USE_NCURSES)
            if(_caca_driver == CACA_DRIVER_NCURSES)
            {
                SCREEN *screen;
                screen = newterm("xterm-16color", stdout, stdin);
                if(screen == NULL)
                    return;
                endwin();
            }
#endif
            (void)putenv("TERM=xterm-16color");
            return;
        }
    }
#endif
}

