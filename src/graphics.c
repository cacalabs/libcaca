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

/** \file graphics.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Character drawing functions
 *
 *  This file contains character and string drawing functions.
 */

#include "config.h"

#if defined(USE_SLANG)
#   include <slang.h>
#endif
#if defined(USE_NCURSES)
#   include <curses.h>
#endif
#if defined(USE_CONIO)
#   include <conio.h>
#   if defined(SCREENUPDATE_IN_PC_H)
#       include <pc.h>
#   endif
#endif
#if defined(USE_X11)
#   include <X11/Xlib.h>
#endif

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#endif

#include <stdio.h> /* BUFSIZ */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/time.h>
#include <time.h>

#include "caca.h"
#include "caca_internals.h"

/*
 * Global variables
 */
unsigned int _caca_width = 0;
unsigned int _caca_height = 0;

/*
 * Local variables
 */
#if defined(USE_NCURSES)
static int ncurses_attr[16*16];
#endif

#if defined(USE_SLANG)
static int slang_assoc[16*16];
#endif

#if defined(USE_CONIO)
static struct text_info conio_ti;
static char *conio_screen;
#endif

#if defined(USE_X11)
Display *x11_dpy;
Window x11_window;
int x11_font_width, x11_font_height;
static GC x11_gc;
static Pixmap x11_pixmap;
static int *x11_screen;
static int x11_colors[16];
static Font x11_font;
static XFontStruct *x11_font_struct;
static int x11_font_offset;
#endif

static char *_caca_empty_line;
static char *_caca_scratch_line;

static unsigned int _caca_delay;
static unsigned int _caca_rendertime;

#if defined(OPTIMISE_SLANG_PALETTE)
static int _caca_fgisbg = 0;
#endif
static enum caca_color _caca_fgcolor = CACA_COLOR_LIGHTGRAY;
static enum caca_color _caca_bgcolor = CACA_COLOR_BLACK;

/*
 * Local functions
 */
#if defined(USE_SLANG)
static void slang_init_palette(void);
#endif

static unsigned int _caca_getticks(void);

/** \brief Set the default colour pair.
 *
 *  This function sets the default colour pair. String functions such as
 *  caca_printf() and graphical primitive functions such as caca_draw_line()
 *  will use these colour pairs.
 *
 *  \param fgcolor The requested foreground colour.
 *  \param bgcolor The requested background colour.
 */
void caca_set_color(enum caca_color fgcolor, enum caca_color bgcolor)
{
    if(fgcolor < 0 || fgcolor > 15 || bgcolor < 0 || bgcolor > 15)
        return;

    _caca_fgcolor = fgcolor;
    _caca_bgcolor = bgcolor;

    switch(_caca_driver)
    {
#if defined(USE_SLANG)
    case CACA_DRIVER_SLANG:

#if defined(OPTIMISE_SLANG_PALETTE)
        /* If foreground == background, discard this colour pair. Functions
         * such as caca_putchar will print spaces instead of characters */
        if(fgcolor != bgcolor)
            _caca_fgisbg = 0;
        else
        {
            _caca_fgisbg = 1;
            switch(fgcolor)
            {
            case CACA_COLOR_BLACK:
                fgcolor = CACA_COLOR_WHITE;
                break;
            case CACA_COLOR_WHITE:
                fgcolor = CACA_COLOR_BLACK;
                break;
            default:
                if(fgcolor <= CACA_COLOR_LIGHTGRAY)
                    fgcolor = CACA_COLOR_BLACK;
                else
                    fgcolor = CACA_COLOR_WHITE;
            }
        }
#endif

#if defined(OPTIMISE_SLANG_PALETTE)
        SLsmg_set_color(slang_assoc[fgcolor + 16 * bgcolor]);
#else
        SLsmg_set_color(fgcolor + 16 * bgcolor);
#endif
        break;
#endif
#if defined(USE_NCURSES)
    case CACA_DRIVER_NCURSES:
        attrset(ncurses_attr[fgcolor + 16 * bgcolor]);
        break;
#endif
#if defined(USE_CONIO)
    case CACA_DRIVER_CONIO:
        textbackground(bgcolor);
        textcolor(fgcolor);
        break;
#endif
#if defined(USE_X11)
    case CACA_DRIVER_X11:
        /* FIXME */
        break;
#endif
    default:
        break;
    }
}

/** \brief Get the current foreground colour.
 *
 *  This function returns the current foreground colour that was set with
 *  caca_set_color().
 *
 *  \return The current foreground colour.
 */
enum caca_color caca_get_fg_color(void)
{
    return _caca_fgcolor;
}

/** \brief Get the current background colour.
 *
 *  This function returns the current background colour that was set with
 *  caca_set_color().
 *
 *  \return The current background colour.
 */
enum caca_color caca_get_bg_color(void)
{
    return _caca_bgcolor;
}

/** \brief Print a character.
 *
 *  This function prints a character at the given coordinates, using the
 *  default foreground and background values. If the coordinates are outside
 *  the screen boundaries, nothing is printed.
 *
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param c The character to print.
 */
void caca_putchar(int x, int y, char c)
{
#if defined(USE_CONIO)
    char *data;
#endif
    if(x < 0 || x >= (int)_caca_width ||
       y < 0 || y >= (int)_caca_height)
        return;

    switch(_caca_driver)
    {
#if defined(USE_SLANG)
    case CACA_DRIVER_SLANG:
        SLsmg_gotorc(y, x);
#if defined(OPTIMISE_SLANG_PALETTE)
        if(_caca_fgisbg)
            SLsmg_write_char(' ');
        else
#endif
            SLsmg_write_char(c);
        break;
#endif
#if defined(USE_NCURSES)
    case CACA_DRIVER_NCURSES:
        move(y, x);
        addch(c);
        break;
#endif
#if defined(USE_CONIO)
    case CACA_DRIVER_CONIO:
        data = conio_screen + 2 * (x + y * _caca_width);
        data[0] = c;
        data[1] = (_caca_bgcolor << 4) | _caca_fgcolor;
//        gotoxy(x + 1, y + 1);
//        putch(c);
        break;
#endif
#if defined(USE_X11)
    case CACA_DRIVER_X11:
        x11_screen[x + y * _caca_width] =
            ((int)c << 8) | ((int)_caca_bgcolor << 4) | (int)_caca_fgcolor;
        break;
#endif
    default:
        break;
    }
}

/** \brief Print a string.
 *
 *  This function prints a string at the given coordinates, using the
 *  default foreground and background values. The coordinates may be outside
 *  the screen boundaries (eg. a negative Y coordinate) and the string will
 *  be cropped accordingly if it is too long.
 *
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param s The string to print.
 */
void caca_putstr(int x, int y, const char *s)
{
#if defined(USE_CONIO)
    char *charbuf;
#endif
#if defined(USE_X11)
    int *intbuf;
#endif
    unsigned int len;

    if(y < 0 || y >= (int)_caca_height || x >= (int)_caca_width)
        return;

    len = strlen(s);

    if(x < 0)
    {
        len -= -x;
        if(len < 0)
            return;
        s += -x;
        x = 0;
    }

    if(x + len >= _caca_width)
    {
        len = _caca_width - x;
        memcpy(_caca_scratch_line, s, len);
        _caca_scratch_line[len] = '\0';
        s = _caca_scratch_line;
    }

    switch(_caca_driver)
    {
#if defined(USE_SLANG)
    case CACA_DRIVER_SLANG:
        SLsmg_gotorc(y, x);
#if defined(OPTIMISE_SLANG_PALETTE)
        if(_caca_fgisbg)
            SLsmg_write_string(_caca_empty_line + _caca_width - len);
        else
#endif
            SLsmg_write_string((char *)(intptr_t)s);
        break;
#endif
#if defined(USE_NCURSES)
    case CACA_DRIVER_NCURSES:
        move(y, x);
        addstr(s);
        break;
#endif
#if defined(USE_CONIO)
    case CACA_DRIVER_CONIO:
        charbuf = conio_screen + 2 * (x + y * _caca_width);
        while(*s)
        {
            *charbuf++ = *s++;
            *charbuf++ = (_caca_bgcolor << 4) | _caca_fgcolor;
        }
//        gotoxy(x + 1, y + 1);
//        cputs(s);
        break;
#endif
#if defined(USE_X11)
    case CACA_DRIVER_X11:
        intbuf = x11_screen + x + y * _caca_width;
        while(*s)
            *intbuf++ = ((int)*s++ << 8)
                         | ((int)_caca_bgcolor << 4) | (int)_caca_fgcolor;
        break;
#endif
    default:
        break;
    }
}

/** \brief Format a string.
 *
 *  This function formats a string at the given coordinates, using the
 *  default foreground and background values. The coordinates may be outside
 *  the screen boundaries (eg. a negative Y coordinate) and the string will
 *  be cropped accordingly if it is too long. The syntax of the format
 *  string is the same as for the C printf() function.
 *
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param format The format string to print.
 *  \param ... Arguments to the format string.
 */
void caca_printf(int x, int y, const char *format, ...)
{
    char tmp[BUFSIZ];
    char *buf = tmp;
    va_list args;

    if(y < 0 || y >= (int)_caca_height || x >= (int)_caca_width)
        return;

    if(_caca_width - x + 1 > BUFSIZ)
        buf = malloc(_caca_width - x + 1);

    va_start(args, format);
#if defined(HAVE_VSNPRINTF)
    vsnprintf(buf, _caca_width - x + 1, format, args);
#else
    vsprintf(buf, format, args);
#endif
    buf[_caca_width - x] = '\0';
    va_end(args);

    caca_putstr(x, y, buf);

    if(buf != tmp)
        free(buf);
}

/** \brief Clear the screen.
 *
 *  This function clears the screen using a black background.
 */
void caca_clear(void)
{
    enum caca_color oldfg = caca_get_fg_color();
    enum caca_color oldbg = caca_get_bg_color();
    int y = _caca_height;

    caca_set_color(CACA_COLOR_LIGHTGRAY, CACA_COLOR_BLACK);

    /* We could use SLsmg_cls() etc., but drawing empty lines is much faster */
    while(y--)
        caca_putstr(0, y, _caca_empty_line);

    caca_set_color(oldfg, oldbg);
}

int _caca_init_graphics(void)
{
#if defined(USE_SLANG)
    if(_caca_driver == CACA_DRIVER_SLANG)
    {
        slang_init_palette();

        /* Disable alt charset support so that we get all 256 colour pairs */
        SLtt_Has_Alt_Charset = 0;

        _caca_width = SLtt_Screen_Cols;
        _caca_height = SLtt_Screen_Rows;
    }
    else
#endif
#if defined(USE_NCURSES)
    if(_caca_driver == CACA_DRIVER_NCURSES)
    {
        static int curses_colors[] =
        {
            /* Standard curses colours */
            COLOR_BLACK,
            COLOR_BLUE,
            COLOR_GREEN,
            COLOR_CYAN,
            COLOR_RED,
            COLOR_MAGENTA,
            COLOR_YELLOW,
            COLOR_WHITE,
            /* Extra values for xterm-16color */
            COLOR_BLACK + 8,
            COLOR_BLUE + 8,
            COLOR_GREEN + 8,
            COLOR_CYAN + 8,
            COLOR_RED + 8,
            COLOR_MAGENTA + 8,
            COLOR_YELLOW + 8,
            COLOR_WHITE + 8
        };

        int fg, bg, max;

        /* Activate colour */
        start_color();

        /* If COLORS == 16, it means the terminal supports full bright colours
         * using setab and setaf (will use \e[90m \e[91m etc. for colours >= 8),
         * we can build 16*16 colour pairs.
         * If COLORS == 8, it means the terminal does not know about bright
         * colours and we need to get them through A_BOLD and A_BLINK (\e[1m
         * and \e[5m). We can only build 8*8 colour pairs. */
        max = COLORS >= 16 ? 16 : 8;

        for(bg = 0; bg < max; bg++)
            for(fg = 0; fg < max; fg++)
            {
                /* Use ((max + 7 - fg) % max) instead of fg so that colour 0
                 * is light gray on black, since some terminals don't like
                 * this colour pair to be redefined. */
                int col = ((max + 7 - fg) % max) + max * bg;
                init_pair(col, curses_colors[fg], curses_colors[bg]);
                ncurses_attr[fg + 16 * bg] = COLOR_PAIR(col);

                if(max == 8)
                {
                    /* Bright fg on simple bg */
                    ncurses_attr[fg + 8 + 16 * bg] = A_BOLD | COLOR_PAIR(col);
                    /* Simple fg on bright bg */
                    ncurses_attr[fg + 16 * (bg + 8)] = A_BLINK
                                                        | COLOR_PAIR(col);
                    /* Bright fg on bright bg */
                    ncurses_attr[fg + 8 + 16 * (bg + 8)] = A_BLINK | A_BOLD
                                                            | COLOR_PAIR(col);
                }
            }

        _caca_width = COLS;
        _caca_height = LINES;
    }
    else
#endif
#if defined(USE_CONIO)
    if(_caca_driver == CACA_DRIVER_CONIO)
    {
        gettextinfo(&conio_ti);
        conio_screen = malloc(2 * conio_ti.screenwidth
                                 * conio_ti.screenheight * sizeof(char));
        if(conio_screen == NULL)
            return -1;
#   if defined(SCREENUPDATE_IN_PC_H)
        ScreenRetrieve(conio_screen);
#   else
        /* FIXME */
#   endif
        _caca_width = conio_ti.screenwidth;
        _caca_height = conio_ti.screenheight;
    }
    else
#endif
#if defined(USE_X11)
    if(_caca_driver == CACA_DRIVER_X11)
    {
        static int x11_palette[] =
        {
            /* Standard curses colours */
            0, 0, 0,
            0, 0, 32768,
            0, 32768, 0,
            0, 32768, 32768,
            32768, 0, 0,
            32768, 0, 32768,
            32768, 32768, 0,
            32768, 32768, 32768,
            /* Extra values for xterm-16color */
            16384, 16384, 16384,
            16384, 16384, 65535,
            16384, 65535, 16384,
            16384, 65535, 65535,
            65535, 16384, 16384,
            65535, 16384, 65535,
            65535, 65535, 16384,
            65535, 65535, 65535,
        };

        Colormap colormap;
        XSetWindowAttributes x11_attr;
        const char *font_name = "8x13bold";
        int i;

        if(getenv("CACA_GEOMETRY"))
            sscanf(getenv("CACA_GEOMETRY"),
                   "%ux%u", &_caca_width, &_caca_height);

        if(!_caca_width)
            _caca_width = 80;
        if(!_caca_height)
            _caca_height = 32;

        x11_screen = malloc(_caca_width * _caca_height * sizeof(int));
        if(x11_screen == NULL)
            return -1;

        x11_dpy = XOpenDisplay(NULL);
        if(x11_dpy == NULL)
        {
            free(x11_screen);
            return -1;
        }

        if(getenv("CACA_FONT"))
            font_name = getenv("CACA_FONT");

        x11_font = XLoadFont(x11_dpy, font_name);
        if(!x11_font)
        {
            XCloseDisplay(x11_dpy);
            free(x11_screen);
            return -1;
        }

        x11_font_struct = XQueryFont(x11_dpy, x11_font);
        if(!x11_font_struct)
        {
            XUnloadFont(x11_dpy, x11_font);
            XCloseDisplay(x11_dpy);
            free(x11_screen);
            return -1;
        }

        x11_font_width = x11_font_struct->max_bounds.width;
        x11_font_height = x11_font_struct->max_bounds.ascent
                             + x11_font_struct->max_bounds.descent;
        x11_font_offset = x11_font_struct->max_bounds.descent;

        colormap = DefaultColormap(x11_dpy, DefaultScreen(x11_dpy));
        for(i = 0; i < 16; i++)
        {
            XColor color;
            color.red = x11_palette[i * 3];
            color.green = x11_palette[i * 3 + 1];
            color.blue = x11_palette[i * 3 + 2];
            XAllocColor(x11_dpy, colormap, &color);
            x11_colors[i] = color.pixel;
        }

        x11_attr.backing_store = Always;
        x11_attr.background_pixel = x11_colors[0];
        x11_attr.event_mask = ExposureMask | StructureNotifyMask;

        x11_window = XCreateWindow(x11_dpy, DefaultRootWindow(x11_dpy), 0, 0,
                                   _caca_width * x11_font_width,
                                   _caca_height * x11_font_height,
                                   0, 0, InputOutput, 0,
                                   CWBackingStore | CWBackPixel | CWEventMask,
                                   &x11_attr);

        XStoreName(x11_dpy, x11_window, "caca for X");

        XSelectInput(x11_dpy, x11_window, StructureNotifyMask);
        XMapWindow(x11_dpy, x11_window);

        x11_gc = XCreateGC(x11_dpy, x11_window, 0, NULL);
        XSetForeground(x11_dpy, x11_gc, x11_colors[15]);
        XSetFont(x11_dpy, x11_gc, x11_font);

        for(;;)
        {
            XEvent event;
            XNextEvent(x11_dpy, &event);
            if (event.type == MapNotify)
                break;
        }

        XSelectInput(x11_dpy, x11_window,
                     KeyPressMask | ButtonPressMask | PointerMotionMask);

        XSync(x11_dpy, False);

        x11_pixmap = XCreatePixmap(x11_dpy, x11_window,
                                   _caca_width * x11_font_width,
                                   _caca_height * x11_font_height,
                                   DefaultDepth(x11_dpy,
                                                DefaultScreen(x11_dpy)));
    }
#endif

    _caca_empty_line = malloc(_caca_width + 1);
    memset(_caca_empty_line, ' ', _caca_width);
    _caca_empty_line[_caca_width] = '\0';

    _caca_scratch_line = malloc(_caca_width + 1);

    _caca_delay = 0;
    _caca_rendertime = 0;

    return 0;
}

int _caca_end_graphics(void)
{
#if defined(USE_SLANG)
    /* Nothing to do */
#endif
#if defined(USE_NCURSES)
    /* Nothing to do */
#endif
#if defined(USE_CONIO)
    if(_caca_driver == CACA_DRIVER_CONIO)
    {
        free(conio_screen);
    }
    else
#endif
#if defined(USE_X11)
    if(_caca_driver == CACA_DRIVER_X11)
    {
        XSync(x11_dpy, False);
        XFreePixmap(x11_dpy, x11_pixmap);
        XFreeFont(x11_dpy, x11_font_struct);
        XFreeGC(x11_dpy, x11_gc);
        XUnmapWindow(x11_dpy, x11_window);
        XDestroyWindow(x11_dpy, x11_window);
        XCloseDisplay(x11_dpy);
        free(x11_screen);
    }
#endif
    free(_caca_empty_line);

    return 0;
}

/** \brief Set the refresh delay.
 *
 *  This function sets the refresh delay in microseconds. The refresh delay
 *  is used by caca_refresh() to achieve constant framerate. See the
 *  caca_refresh() documentation for more details.
 *
 *  If the argument is zero, constant framerate is disabled. This is the
 *  default behaviour.
 *
 *  \param usec The refresh delay in microseconds.
 */
void caca_set_delay(unsigned int usec)
{
    _caca_delay = usec;
}

/** \brief Get the average rendering time.
 *
 *  This function returns the average rendering time, which is the average
 *  measured time between two caca_refresh() calls, in microseconds. If
 *  constant framerate was activated by calling caca_set_delay(), the average
 *  rendering time will not be considerably shorter than the requested delay
 *  even if the real rendering time was shorter.
 *
 *  \return The render time in microseconds.
 */
unsigned int caca_get_rendertime(void)
{
    return _caca_rendertime;
}

static unsigned int _caca_getticks(void)
{
    static unsigned int last_sec = 0, last_usec = 0;

    struct timeval tv;
    unsigned int ticks = 0;

    gettimeofday(&tv, NULL);

    if(last_sec != 0)
    {
        ticks = (tv.tv_sec - last_sec) * 1000000 + (tv.tv_usec - last_usec);
    }

    last_sec = tv.tv_sec;
    last_usec = tv.tv_usec;

    return ticks;
}

/** \brief Flush pending changes and redraw the screen.
 *
 *  This function flushes all graphical operations and prints them to the
 *  screen. Nothing will show on the screen caca_refresh() is not called.
 *
 *  If caca_set_delay() was called with a non-zero value, caca_refresh()
 *  will use that value to achieve constant framerate: if two consecutive
 *  calls to caca_refresh() are within a time range shorter than the value
 *  set with caca_set_delay(), the second call will wait a bit before
 *  performing the screen refresh.
 */
void caca_refresh(void)
{
#define IDLE_USEC 10000
    static int lastticks = 0;
    int ticks = lastticks + _caca_getticks();

#if defined(USE_SLANG)
    if(_caca_driver == CACA_DRIVER_SLANG)
    {
        SLsmg_refresh();
    }
    else
#endif
#if defined(USE_NCURSES)
    if(_caca_driver == CACA_DRIVER_NCURSES)
    {
        refresh();
    }
    else
#endif
#if defined(USE_CONIO)
    if(_caca_driver == CACA_DRIVER_CONIO)
    {
#   if defined(SCREENUPDATE_IN_PC_H)
        ScreenUpdate(conio_screen);
#   else
        /* FIXME */
#   endif
    }
    else
#endif
#if defined(USE_X11)
    if(_caca_driver == CACA_DRIVER_X11)
    {
        unsigned int x, y;

        /* FIXME: This is very, very slow. There are several things that can
         * be done in order to speed up things:
         *  - cache characters once rendered
         *  - pre-render all characters
         *  - use our own rendering routine (screen depth dependent) */
        for(y = 0; y < _caca_height; y++)
            for(x = 0; x < _caca_width; x++)
            {
                int item = x11_screen[x + y * _caca_width];
                char data = item >> 8;
                XSetForeground(x11_dpy, x11_gc, x11_colors[(item >> 4) & 0xf]);
                XFillRectangle(x11_dpy, x11_pixmap, x11_gc,
                               x * x11_font_width, y * x11_font_height,
                               x11_font_width, x11_font_height);
                XSetForeground(x11_dpy, x11_gc, x11_colors[item & 0xf]);
                XDrawString(x11_dpy, x11_pixmap, x11_gc, x * x11_font_width,
                            (y + 1) * x11_font_height - x11_font_offset,
                            &data, 1);
            }
        XCopyArea(x11_dpy, x11_pixmap, x11_window, x11_gc, 0, 0,
                  _caca_width * x11_font_width, _caca_height * x11_font_height,
                  0, 0);
        XFlush(x11_dpy);
    }
#endif

    /* Wait until _caca_delay + time of last call */
    ticks += _caca_getticks();
    for(; ticks + IDLE_USEC < (int)_caca_delay; ticks += _caca_getticks())
        usleep(IDLE_USEC);

    /* Update the sliding mean of the render time */
    _caca_rendertime = (7 * _caca_rendertime + ticks) / 8;

    lastticks = ticks - _caca_delay;

    /* If we drifted too much, it's bad, bad, bad. */
    if(lastticks > (int)_caca_delay)
        lastticks = 0;
}

#if defined(USE_SLANG)

#if defined(OPTIMISE_SLANG_PALETTE)
#endif

static void slang_init_palette(void)
{
    /* See SLang ref., 5.4.4. */
    static char *slang_colors[16] =
    {
        /* Standard colours */
        "black",
        "blue",
        "green",
        "cyan",
        "red",
        "magenta",
        "brown",
        "lightgray",
        /* Bright colours */
        "gray",
        "brightblue",
        "brightgreen",
        "brightcyan",
        "brightred",
        "brightmagenta",
        "yellow",
        "white",
    };

#if !defined(OPTIMISE_SLANG_PALETTE)
    int fg, bg;

    for(bg = 0; bg < 16; bg++)
        for(fg = 0; fg < 16; fg++)
        {
            int i = fg + 16 * bg;
            SLtt_set_color(i, NULL, slang_colors[fg], slang_colors[bg]);
        }
#else
    int i, cur = 0;

    /* 6 colours in hue order */
    static const enum caca_color hue_list[] =
    {
        CACA_COLOR_RED,
        CACA_COLOR_BROWN,
        CACA_COLOR_GREEN,
        CACA_COLOR_CYAN,
        CACA_COLOR_BLUE,
        CACA_COLOR_MAGENTA
    };

#define SETPAIR(_fg, _bg, _n) \
    do \
    { \
        int fg = _fg, bg = _bg, n = _n; \
        SLtt_set_color(n, NULL, slang_colors[fg], slang_colors[bg]); \
        slang_assoc[fg + 16 * bg] = n; \
    } \
    while(0);

    /*
     * XXX: See the NOTES file for what follows
     */

    /* black background colour pairs that are needed for the old renderer */
    for(i = 1; i < 16; i++)
        SETPAIR(i, CACA_COLOR_BLACK, cur++);

    /* gray combinations used for grayscale dithering */
    SETPAIR(CACA_COLOR_BLACK, CACA_COLOR_DARKGRAY, cur++);
    SETPAIR(CACA_COLOR_DARKGRAY, CACA_COLOR_LIGHTGRAY, cur++);
    SETPAIR(CACA_COLOR_LIGHTGRAY, CACA_COLOR_DARKGRAY, cur++);
    SETPAIR(CACA_COLOR_WHITE, CACA_COLOR_LIGHTGRAY, cur++);
    SETPAIR(CACA_COLOR_LIGHTGRAY, CACA_COLOR_WHITE, cur++);

    /* white/light, light/dark, lightgray/light, darkgray/dark, dark/black
     * combinations often used for saturation/value dithering (the two
     * other possible combinations, lightgray/dark and darkgray/light, are
     * not considered here) */
    for(i = 1; i < 7; i++)
    {
        SETPAIR(CACA_COLOR_WHITE, i + 8, cur++);
        SETPAIR(i + 8, CACA_COLOR_WHITE, cur++);
        SETPAIR(i, i + 8, cur++);
        SETPAIR(i + 8, i, cur++);
        SETPAIR(CACA_COLOR_LIGHTGRAY, i + 8, cur++);
        SETPAIR(i + 8, CACA_COLOR_LIGHTGRAY, cur++);
        SETPAIR(CACA_COLOR_DARKGRAY, i, cur++);
        SETPAIR(i, CACA_COLOR_DARKGRAY, cur++);
        SETPAIR(CACA_COLOR_BLACK, i, cur++);
    }

    /* next colour combinations for hue dithering (magenta/blue, blue/green
     * and so on) */
    for(i = 0; i < 6; i++)
    {
        SETPAIR(hue_list[i], hue_list[(i + 1) % 6], cur++);
        SETPAIR(hue_list[(i + 1) % 6], hue_list[i], cur++);
        SETPAIR(hue_list[i] + 8, hue_list[(i + 1) % 6] + 8, cur++);
        SETPAIR(hue_list[(i + 1) % 6] + 8, hue_list[i] + 8, cur++);
    }

    /* next colour combinations for hue/value dithering (blue/lightgreen,
     * green/lightblue and so on) */
    for(i = 0; i < 6; i++)
    {
        SETPAIR(hue_list[i], hue_list[(i + 1) % 6] + 8, cur++);
        SETPAIR(hue_list[(i + 1) % 6], hue_list[i] + 8, cur++);
        SETPAIR(hue_list[i] + 8, hue_list[(i + 1) % 6], cur++);
        SETPAIR(hue_list[(i + 1) % 6] + 8, hue_list[i], cur++);
    }

    /* black on light gray, black on white, white on dark gray, dark gray
     * on white, white on blue, light gray on blue (chosen arbitrarily) */
    SETPAIR(CACA_COLOR_BLACK, CACA_COLOR_LIGHTGRAY, cur++);
    SETPAIR(CACA_COLOR_BLACK, CACA_COLOR_WHITE, cur++);
    SETPAIR(CACA_COLOR_WHITE, CACA_COLOR_DARKGRAY, cur++);
    SETPAIR(CACA_COLOR_DARKGRAY, CACA_COLOR_WHITE, cur++);
    SETPAIR(CACA_COLOR_WHITE, CACA_COLOR_BLUE, cur++);
    SETPAIR(CACA_COLOR_LIGHTGRAY, CACA_COLOR_BLUE, cur++);

    /*
     * Now the possibly emulated pairs
     */

    /* light gray on dark colour: emulate with light colour on dark colour
     * white on dark colour: emulate with light gray on light colour
     * black on light colour: emulate with dark gray on dark colour
     * dark gray on light colour: emulate with dark colour on light colour
     * light colour on dark gray: emulate with dark colour on dark gray
     * dark colour on light gray: emulate with light colour on light gray
     * dark colour on white: emulate with light colour on white */
    for(i = 1; i < 7; i++)
    {
        if(i != CACA_COLOR_BLUE)
        {
            SETPAIR(CACA_COLOR_LIGHTGRAY, i, 128 +
                    slang_assoc[i + 8 + 16 * i]);
            SETPAIR(CACA_COLOR_WHITE, i, 128 +
                    slang_assoc[CACA_COLOR_LIGHTGRAY + 16 * (i + 8)]);
        }
        SETPAIR(CACA_COLOR_BLACK, i + 8,
                128 + slang_assoc[CACA_COLOR_DARKGRAY + 16 * i]);
        SETPAIR(CACA_COLOR_DARKGRAY, i + 8,
                128 + slang_assoc[i + 16 * (i + 8)]);
        SETPAIR(i + 8, CACA_COLOR_DARKGRAY,
                128 + slang_assoc[i + 16 * CACA_COLOR_DARKGRAY]);
        SETPAIR(i, CACA_COLOR_LIGHTGRAY,
                128 + slang_assoc[i + 8 + 16 * CACA_COLOR_LIGHTGRAY]);
        SETPAIR(i, CACA_COLOR_WHITE,
                128 + slang_assoc[i + 8 + 16 * CACA_COLOR_WHITE]);
    }

    /* 120 degree hue pairs can be emulated as well; for instance blue on
     * red can be emulated using magenta on red, and blue on green using
     * cyan on green */
    for(i = 0; i < 6; i++)
    {
        SETPAIR(hue_list[(i + 2) % 6], hue_list[i],
            128 + slang_assoc[hue_list[(i + 1) % 6] + 16 * hue_list[i]]);
        SETPAIR(hue_list[(i + 2) % 6] + 8, hue_list[i] + 8,
            128 + slang_assoc[hue_list[(i + 1) % 6] + 16 * hue_list[i] + 136]);
        SETPAIR(hue_list[(i + 2) % 6] + 8, hue_list[i],
            128 + slang_assoc[hue_list[(i + 1) % 6] + 16 * hue_list[i] + 8]);
        SETPAIR(hue_list[(i + 2) % 6], hue_list[i] + 8,
            128 + slang_assoc[hue_list[(i + 1) % 6] + 16 * hue_list[i] + 128]);

        SETPAIR(hue_list[(i + 4) % 6], hue_list[i],
            128 + slang_assoc[hue_list[(i + 5) % 6] + 16 * hue_list[i]]);
        SETPAIR(hue_list[(i + 4) % 6] + 8, hue_list[i] + 8,
            128 + slang_assoc[hue_list[(i + 5) % 6] + 16 * hue_list[i] + 136]);
        SETPAIR(hue_list[(i + 4) % 6] + 8, hue_list[i],
            128 + slang_assoc[hue_list[(i + 5) % 6] + 16 * hue_list[i] + 8]);
        SETPAIR(hue_list[(i + 4) % 6], hue_list[i] + 8,
            128 + slang_assoc[hue_list[(i + 5) % 6] + 16 * hue_list[i] + 128]);
    }

    /* dark opposite on dark: emulate with dark opposite on black
     * light opposite on dark: emulate with light opposite on black
     * dark opposite on light: emulate with black on dark
     * light opposite on light: emulate with white on light */
    for(i = 0; i < 6; i++)
    {
        SETPAIR(hue_list[i], hue_list[(i + 3) % 6],
                128 + slang_assoc[hue_list[i] + 16 * CACA_COLOR_BLACK]);
        SETPAIR(hue_list[i] + 8, hue_list[(i + 3) % 6],
                128 + slang_assoc[hue_list[i] + 8 + 16 * CACA_COLOR_BLACK]);
        SETPAIR(hue_list[(i + 3) % 6], hue_list[i] + 8,
                128 + slang_assoc[CACA_COLOR_BLACK + 16 * hue_list[i]]);
        SETPAIR(hue_list[(i + 3) % 6] + 8, hue_list[i] + 8,
                128 + slang_assoc[CACA_COLOR_WHITE + 16 * (hue_list[i] + 8)]);
    }
#endif
}
#endif /* USE_SLANG */

