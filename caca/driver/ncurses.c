/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2014 Sam Hocevar <sam@hocevar.net>
 *                2007 Ben Wiley Sittler <bsittler@gmail.com>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains the libcaca Ncurses input and output driver
 */

#include "config.h"

#if defined USE_NCURSES

#if defined HAVE_NCURSESW_NCURSES_H
#   include <ncursesw/ncurses.h>
#elif defined HAVE_NCURSES_NCURSES_H
#   include <ncurses/ncurses.h>
#elif defined HAVE_NCURSES_H
#   include <ncurses.h>
#else
#   include <curses.h>
#endif

#include <stdlib.h>
#include <string.h>

#if defined HAVE_UNISTD_H
#   include <unistd.h>
#endif
#if defined HAVE_SIGNAL_H
#   include <signal.h>
#endif
#if defined HAVE_SYS_IOCTL_H
#   include <sys/ioctl.h>
#endif
#if defined HAVE_LOCALE_H
#   include <locale.h>
#endif
#if defined HAVE_TERMIOS_H
#   include <termios.h>
#endif

#include "caca.h"
#include "caca_internals.h"

/*
 * Emulation for missing ACS_* in older curses
 */

#ifndef ACS_BLOCK
#define ACS_BLOCK '#'
#endif

#ifndef ACS_BOARD
#define ACS_BOARD '#'
#endif

#ifndef ACS_BTEE
#define ACS_BTEE '+'
#endif

#ifndef ACS_BULLET
#define ACS_BULLET '.'
#endif

#ifndef ACS_CKBOARD
#define ACS_CKBOARD ':'
#endif

#ifndef ACS_DARROW
#define ACS_DARROW 'v'
#endif

#ifndef ACS_DEGREE
#define ACS_DEGREE '\''
#endif

#ifndef ACS_DIAMOND
#define ACS_DIAMOND '+'
#endif

#ifndef ACS_GEQUAL
#define ACS_GEQUAL '>'
#endif

#ifndef ACS_HLINE
#define ACS_HLINE '-'
#endif

#ifndef ACS_LANTERN
#define ACS_LANTERN '#'
#endif

#ifndef ACS_LARROW
#define ACS_LARROW '<'
#endif

#ifndef ACS_LEQUAL
#define ACS_LEQUAL '<'
#endif

#ifndef ACS_LLCORNER
#define ACS_LLCORNER '+'
#endif

#ifndef ACS_LRCORNER
#define ACS_LRCORNER '+'
#endif

#ifndef ACS_LTEE
#define ACS_LTEE '+'
#endif

#ifndef ACS_NEQUAL
#define ACS_NEQUAL '!'
#endif

#ifndef ACS_PI
#define ACS_PI '*'
#endif

#ifndef ACS_STERLING
#define ACS_STERLING 'f'
#endif

#ifndef ACS_PLMINUS
#define ACS_PLMINUS '#'
#endif

#ifndef ACS_PLUS
#define ACS_PLUS '+'
#endif

#ifndef ACS_RARROW
#define ACS_RARROW '>'
#endif

#ifndef ACS_RTEE
#define ACS_RTEE '+'
#endif

#ifndef ACS_S1
#define ACS_S1 '-'
#endif

#ifndef ACS_S3
#define ACS_S3 '-'
#endif

#ifndef ACS_S7
#define ACS_S7 '-'
#endif

#ifndef ACS_S9
#define ACS_S9 '-'
#endif

#ifndef ACS_TTEE
#define ACS_TTEE '+'
#endif

#ifndef ACS_UARROW
#define ACS_UARROW '^'
#endif

#ifndef ACS_ULCORNER
#define ACS_ULCORNER '+'
#endif

#ifndef ACS_URCORNER
#define ACS_URCORNER '+'
#endif

#ifndef ACS_VLINE
#define ACS_VLINE '|'
#endif

/*
 * Local functions
 */

#if defined HAVE_SIGNAL
static RETSIGTYPE sigwinch_handler(int);
static caca_display_t *sigwinch_d; /* FIXME: we ought to get rid of this */
#endif
#if defined HAVE_GETENV && defined HAVE_PUTENV
static void ncurses_install_terminal(caca_display_t *);
static void ncurses_uninstall_terminal(caca_display_t *);
#endif
static void ncurses_write_utf32(uint32_t);

struct driver_private
{
    int attr[16*16];
    mmask_t oldmask;
    char *term;
};

static int ncurses_init_graphics(caca_display_t *dp)
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

    mmask_t newmask;
    int fg, bg, max;

    dp->drv.p = malloc(sizeof(struct driver_private));

#if defined HAVE_GETENV && defined HAVE_PUTENV
    ncurses_install_terminal(dp);
#endif

#if defined HAVE_SIGNAL
    sigwinch_d = dp;
    signal(SIGWINCH, sigwinch_handler);
#endif

#if defined HAVE_LOCALE_H
    setlocale(LC_ALL, "");
#endif

    _caca_set_term_title("caca for ncurses");

    initscr();
    keypad(stdscr, TRUE);
    nonl();
    raw();
    noecho();
    nodelay(stdscr, TRUE);
    curs_set(0);

    /* Activate mouse */
    newmask = REPORT_MOUSE_POSITION | ALL_MOUSE_EVENTS;
    mousemask(newmask, &dp->drv.p->oldmask);
    mouseinterval(-1); /* No click emulation */

    /* Set the escape delay to a ridiculously low value */
#if defined set_escdelay
    set_escdelay(10);
#else
    ESCDELAY = 10;
#endif

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
             * is light gray on black. Some terminals don't like this
             * colour pair to be redefined. */
            int col = ((max + 7 - fg) % max) + max * bg;
            init_pair(col, curses_colors[fg], curses_colors[bg]);
            dp->drv.p->attr[fg + 16 * bg] = COLOR_PAIR(col);

            if(max == 8)
            {
                /* Bright fg on simple bg */
                dp->drv.p->attr[fg + 8 + 16 * bg] = A_BOLD | COLOR_PAIR(col);
                /* Simple fg on bright bg */
                dp->drv.p->attr[fg + 16 * (bg + 8)] = A_BLINK
                                                    | COLOR_PAIR(col);
                /* Bright fg on bright bg */
                dp->drv.p->attr[fg + 8 + 16 * (bg + 8)] = A_BLINK | A_BOLD
                                                        | COLOR_PAIR(col);
            }
        }

    caca_add_dirty_rect(dp->cv, 0, 0, dp->cv->width, dp->cv->height);
    dp->resize.allow = 1;
    caca_set_canvas_size(dp->cv, COLS, LINES);
    dp->resize.allow = 0;

    return 0;
}

static int ncurses_end_graphics(caca_display_t *dp)
{
    _caca_set_term_title("");
    mousemask(dp->drv.p->oldmask, NULL);
    curs_set(1);
    noraw();
    endwin();

#if defined HAVE_GETENV && defined HAVE_PUTENV
    ncurses_uninstall_terminal(dp);
#endif

    free(dp->drv.p);

    return 0;
}

static int ncurses_set_display_title(caca_display_t *dp, char const *title)
{
    _caca_set_term_title(title);

    return 0;
}

static int ncurses_get_display_width(caca_display_t const *dp)
{
    /* Fallback to a 6x10 font */
    return caca_get_canvas_width(dp->cv) * 6;
}

static int ncurses_get_display_height(caca_display_t const *dp)
{
    /* Fallback to a 6x10 font */
    return caca_get_canvas_height(dp->cv) * 10;
}

static void ncurses_display(caca_display_t *dp)
{
    int x, y, i;

    for(i = 0; i < caca_get_dirty_rect_count(dp->cv); i++)
    {
        uint32_t const *cvchars, *cvattrs;
        int dx, dy, dw, dh;

        caca_get_dirty_rect(dp->cv, i, &dx, &dy, &dw, &dh);

        cvchars = caca_get_canvas_chars(dp->cv) + dx + dy * dp->cv->width;
        cvattrs = caca_get_canvas_attrs(dp->cv) + dx + dy * dp->cv->width;

        for(y = dy; y < dy + dh; y++)
        {
            move(y, dx);
            for(x = dx; x < dx + dw; x++)
            {
                uint32_t attr = *cvattrs++;

                (void)attrset(dp->drv.p->attr[caca_attr_to_ansi(attr)]);
                if(attr & CACA_BOLD)
                    attron(A_BOLD);
                if(attr & CACA_BLINK)
                    attron(A_BLINK);
                if(attr & CACA_UNDERLINE)
                    attron(A_UNDERLINE);

                ncurses_write_utf32(*cvchars++);
            }

            cvchars += dp->cv->width - dw;
            cvattrs += dp->cv->width - dw;
        }
    }

    x = caca_wherex(dp->cv);
    y = caca_wherey(dp->cv);
    move(y, x);

    refresh();
}

static void ncurses_handle_resize(caca_display_t *dp)
{
    struct winsize size;

#if defined HAVE_SYS_IOCTL_H
    if(ioctl(fileno(stdout), TIOCGWINSZ, &size) == 0)
    {
        dp->resize.w = size.ws_col;
        dp->resize.h = size.ws_row;
#if defined HAVE_RESIZE_TERM
        resize_term(dp->resize.h, dp->resize.w);
#else
        resizeterm(dp->resize.h, dp->resize.w);
#endif
        wrefresh(curscr);
        return;
    }
#endif

    /* Fallback */
    dp->resize.w = caca_get_canvas_width(dp->cv);
    dp->resize.h = caca_get_canvas_height(dp->cv);
}

static int ncurses_get_event(caca_display_t *dp, caca_privevent_t *ev)
{
    int intkey;

    intkey = getch();
    if(intkey == ERR)
    {
        ev->type = CACA_EVENT_NONE;
        return 0;
    }

    if(intkey < 0x7f)
    {
        ev->type = CACA_EVENT_KEY_PRESS;
        ev->data.key.ch = intkey;
        ev->data.key.utf32 = intkey;
        ev->data.key.utf8[0] = intkey;
        ev->data.key.utf8[1] = '\0';
        return 1;
    }

    /* If the key was UTF-8, parse the whole sequence */
    if(intkey >= 0x80 && intkey < 0x100)
    {
        int keys[7]; /* Necessary for ungetch(); */
        char utf8[7];
        uint32_t utf32;
        size_t i, bytes = 0;

        keys[0] = intkey;
        utf8[0] = intkey;

        for(i = 1; i < 6; i++)
        {
            keys[i] = getch();
            utf8[i] = (unsigned char)keys[i];
        }

        utf8[i] = '\0';
        utf32 = caca_utf8_to_utf32(utf8, &bytes);

        while(i > bytes)
            ungetch(keys[--i]);

        if(bytes)
        {
            ev->type = CACA_EVENT_KEY_PRESS;
            ev->data.key.ch = 0;
            ev->data.key.utf32 = utf32;
            strcpy(ev->data.key.utf8, utf8);
            return 1;
        }
    }

    if(intkey == KEY_MOUSE)
    {
        MEVENT mevent;
        getmouse(&mevent);

        switch(mevent.bstate)
        {
#define PRESS(x) ev->data.mouse.button = x; \
                 ev->type = CACA_EVENT_MOUSE_PRESS; _push_event(dp, ev)
#define RELEASE(x) ev->data.mouse.button = x; \
                   ev->type = CACA_EVENT_MOUSE_RELEASE; _push_event(dp, ev)
#define CLICK(x) PRESS(x); RELEASE(x)
            case BUTTON1_PRESSED: PRESS(1); break;
            case BUTTON1_RELEASED: RELEASE(1); break;
            case BUTTON1_CLICKED: CLICK(1); break;
            case BUTTON1_DOUBLE_CLICKED: CLICK(1); CLICK(1); break;
            case BUTTON1_TRIPLE_CLICKED: CLICK(1); CLICK(1); CLICK(1); break;
#if defined BUTTON1_RESERVED_EVENT
            case BUTTON1_RESERVED_EVENT: break;
#endif

            case BUTTON2_PRESSED: PRESS(2); break;
            case BUTTON2_RELEASED: RELEASE(2); break;
            case BUTTON2_CLICKED: CLICK(2); break;
            case BUTTON2_DOUBLE_CLICKED: CLICK(2); CLICK(2); break;
            case BUTTON2_TRIPLE_CLICKED: CLICK(2); CLICK(2); CLICK(2); break;
#if defined BUTTON2_RESERVED_EVENT
            case BUTTON2_RESERVED_EVENT: break;
#endif

            case BUTTON3_PRESSED: PRESS(3); break;
            case BUTTON3_RELEASED: RELEASE(3); break;
            case BUTTON3_CLICKED: CLICK(3); break;
            case BUTTON3_DOUBLE_CLICKED: CLICK(3); CLICK(3); break;
            case BUTTON3_TRIPLE_CLICKED: CLICK(3); CLICK(3); CLICK(3); break;
#if defined BUTTON3_RESERVED_EVENT
            case BUTTON3_RESERVED_EVENT: break;
#endif

            case BUTTON4_PRESSED: PRESS(4); break;
            case BUTTON4_RELEASED: RELEASE(4); break;
            case BUTTON4_CLICKED: CLICK(4); break;
            case BUTTON4_DOUBLE_CLICKED: CLICK(4); CLICK(4); break;
            case BUTTON4_TRIPLE_CLICKED: CLICK(4); CLICK(4); CLICK(4); break;
#if defined BUTTON4_RESERVED_EVENT
            case BUTTON4_RESERVED_EVENT: break;
#endif

            default:
                break;
#undef PRESS
#undef RELEASE
#undef CLICK
        }

        if(dp->mouse.x == mevent.x && dp->mouse.y == mevent.y)
            return _pop_event(dp, ev);

        dp->mouse.x = mevent.x;
        dp->mouse.y = mevent.y;

        ev->type = CACA_EVENT_MOUSE_MOTION;
        ev->data.mouse.x = dp->mouse.x;
        ev->data.mouse.y = dp->mouse.y;
        return 1;
    }

    switch(intkey)
    {
        case KEY_UP: ev->data.key.ch = CACA_KEY_UP; break;
        case KEY_DOWN: ev->data.key.ch = CACA_KEY_DOWN; break;
        case KEY_LEFT: ev->data.key.ch = CACA_KEY_LEFT; break;
        case KEY_RIGHT: ev->data.key.ch = CACA_KEY_RIGHT; break;

        case KEY_IC: ev->data.key.ch = CACA_KEY_INSERT; break;
        case KEY_DC: ev->data.key.ch = CACA_KEY_DELETE; break;
        case 0x7f:
        case KEY_BACKSPACE: ev->data.key.ch = CACA_KEY_BACKSPACE; break;
        case KEY_HOME: ev->data.key.ch = CACA_KEY_HOME; break;
        case KEY_END: ev->data.key.ch = CACA_KEY_END; break;
        case KEY_PPAGE: ev->data.key.ch = CACA_KEY_PAGEUP; break;
        case KEY_NPAGE: ev->data.key.ch = CACA_KEY_PAGEDOWN; break;

        case KEY_F(1): ev->data.key.ch = CACA_KEY_F1; break;
        case KEY_F(2): ev->data.key.ch = CACA_KEY_F2; break;
        case KEY_F(3): ev->data.key.ch = CACA_KEY_F3; break;
        case KEY_F(4): ev->data.key.ch = CACA_KEY_F4; break;
        case KEY_F(5): ev->data.key.ch = CACA_KEY_F5; break;
        case KEY_F(6): ev->data.key.ch = CACA_KEY_F6; break;
        case KEY_F(7): ev->data.key.ch = CACA_KEY_F7; break;
        case KEY_F(8): ev->data.key.ch = CACA_KEY_F8; break;
        case KEY_F(9): ev->data.key.ch = CACA_KEY_F9; break;
        case KEY_F(10): ev->data.key.ch = CACA_KEY_F10; break;
        case KEY_F(11): ev->data.key.ch = CACA_KEY_F11; break;
        case KEY_F(12): ev->data.key.ch = CACA_KEY_F12; break;

        default:
            /* Unknown key */
            ev->type = CACA_EVENT_NONE; return 0;
    }

    ev->type = CACA_EVENT_KEY_PRESS;
    ev->data.key.utf32 = 0;
    ev->data.key.utf8[0] = '\0';
    return 1;
}

static void ncurses_set_cursor(caca_display_t *dp, int flags)
{
    if (!flags)
        curs_set(0);
    else if (curs_set(2) == ERR)
        curs_set(1);
}

/*
 * XXX: following functions are local
 */

#if defined HAVE_SIGNAL
static RETSIGTYPE sigwinch_handler(int sig)
{
    sigwinch_d->resize.resized = 1;

    signal(SIGWINCH, sigwinch_handler);
}
#endif

#if defined HAVE_GETENV && defined HAVE_PUTENV
static void ncurses_install_terminal(caca_display_t *dp)
{
    char *term, *colorterm;

    dp->drv.p->term = NULL;

    term = getenv("TERM");
    colorterm = getenv("COLORTERM");

    if(!term || strcmp(term, "xterm"))
        return;

    /* If we are using gnome-terminal, it's really a 16 colour terminal.
     * Ditto if we are using xfce4-terminal, or Konsole. */
    if((colorterm && (!strcmp(colorterm, "gnome-terminal")
                       || !strcmp(colorterm, "Terminal")))
         || getenv("KONSOLE_DCOP_SESSION"))
    {
        SCREEN *screen;
        screen = newterm("xterm-16color", stdout, stdin);
        if(screen == NULL)
            return;
        endwin();
        (void)putenv("TERM=xterm-16color");
        dp->drv.p->term = strdup(term);
        return;
    }
}

static void ncurses_uninstall_terminal(caca_display_t *dp)
{
    /* Needs to be persistent because we use putenv() */
    static char termenv[1024];

    if(!dp->drv.p->term)
        return;

    snprintf(termenv, 1023, "TERM=%s", dp->drv.p->term);
    free(dp->drv.p->term);
    (void)putenv(termenv);
}
#endif

static void ncurses_write_utf32(uint32_t ch)
{
#if defined HAVE_NCURSESW_NCURSES_H
    char buf[10];
    int bytes;
#endif

    if(ch == CACA_MAGIC_FULLWIDTH)
        return;

#if defined HAVE_NCURSESW_NCURSES_H
    bytes = caca_utf32_to_utf8(buf, ch);
    buf[bytes] = '\0';
    addstr(buf);
#else
    if(ch < 0x80)
    {
        addch(ch);
    }
    else
    {
        chtype cch;
        chtype cch2;

        cch = '?';
        cch2 = ' ';
        if ((ch > 0x0000ff00) && (ch < 0x0000ff5f))
        {
            cch = ch - 0x0000ff00 + ' ';
        }
        switch (ch)
        {
        case 0x000000a0: /* <nbsp> */
        case 0x00003000: /* 　 */
            cch = ' ';
            break;
        case 0x000000a3: /* £ */
            cch = ACS_STERLING;
            break;
        case 0x000000b0: /* ° */
            cch = ACS_DEGREE;
            break;
        case 0x000000b1: /* ± */
            cch = ACS_PLMINUS;
            break;
        case 0x000000b7: /* · */
        case 0x00002219: /* ∙ */
        case 0x000030fb: /* ・ */
            cch = ACS_BULLET;
            break;
        case 0x000003c0: /* π */
            cch = ACS_PI;
            break;
        case 0x00002018: /* ‘ */
        case 0x00002019: /* ’ */
            cch = '\'';
            break;
        case 0x0000201c: /* “ */
        case 0x0000201d: /* ” */
            cch = '"';
            break;
        case 0x00002190: /* ← */
            cch = ACS_LARROW;
            break;
        case 0x00002191: /* ↑ */
            cch = ACS_UARROW;
            break;
        case 0x00002192: /* → */
            cch = ACS_RARROW;
            break;
        case 0x00002193: /* ↓ */
            cch = ACS_DARROW;
            break;
        case 0x00002260: /* ≠ */
            cch = ACS_NEQUAL;
            break;
        case 0x00002261: /* ≡ */
            cch = '=';
            break;
        case 0x00002264: /* ≤ */
            cch = ACS_LEQUAL;
            break;
        case 0x00002265: /* ≥ */
            cch = ACS_GEQUAL;
            break;
        case 0x000023ba: /* ⎺ */
            cch = ACS_S1;
            cch2 = cch;
            break;
        case 0x000023bb: /* ⎻ */
            cch = ACS_S3;
            cch2 = cch;
            break;
        case 0x000023bc: /* ⎼ */
            cch = ACS_S7;
            cch2 = cch;
            break;
        case 0x000023bd: /* ⎽ */
            cch = ACS_S9;
            cch2 = cch;
            break;
        case 0x00002500: /* ─ */
        case 0x00002550: /* ═ */
            cch = ACS_HLINE;
            cch2 = cch;
            break;
        case 0x00002502: /* │ */
        case 0x00002551: /* ║ */
            cch = ACS_VLINE;
            break;
        case 0x0000250c: /* ┌ */
        case 0x00002552: /* ╒ */
        case 0x00002553: /* ╓ */
        case 0x00002554: /* ╔ */
            cch = ACS_ULCORNER;
            cch2 = ACS_HLINE;
            break;
        case 0x00002510: /* ┐ */
        case 0x00002555: /* ╕ */
        case 0x00002556: /* ╖ */
        case 0x00002557: /* ╗ */
            cch = ACS_URCORNER;
            break;
        case 0x00002514: /* └ */
        case 0x00002558: /* ╘ */
        case 0x00002559: /* ╙ */
        case 0x0000255a: /* ╚ */
            cch = ACS_LLCORNER;
            cch2 = ACS_HLINE;
            break;
        case 0x00002518: /* ┘ */
        case 0x0000255b: /* ╛ */
        case 0x0000255c: /* ╜ */
        case 0x0000255d: /* ╝ */
            cch = ACS_LRCORNER;
            break;
        case 0x0000251c: /* ├ */
        case 0x0000255e: /* ╞ */
        case 0x0000255f: /* ╟ */
        case 0x00002560: /* ╠ */
            cch = ACS_LTEE;
            cch2 = ACS_HLINE;
            break;
        case 0x00002524: /* ┤ */
        case 0x00002561: /* ╡ */
        case 0x00002562: /* ╢ */
        case 0x00002563: /* ╣ */
            cch = ACS_RTEE;
            break;
        case 0x0000252c: /* ┬ */
        case 0x00002564: /* ╤ */
        case 0x00002565: /* ╥ */
        case 0x00002566: /* ╦ */
            cch = ACS_TTEE;
            cch2 = ACS_HLINE;
            break;
        case 0x00002534: /* ┴ */
        case 0x00002567: /* ╧ */
        case 0x00002568: /* ╨ */
        case 0x00002569: /* ╩ */
            cch = ACS_BTEE;
            cch2 = ACS_HLINE;
            break;
        case 0x0000253c: /* ┼ */
        case 0x0000256a: /* ╪ */
        case 0x0000256b: /* ╫ */
        case 0x0000256c: /* ╬ */
            cch = ACS_PLUS;
            cch2 = ACS_HLINE;
            break;
        case 0x00002591: /* ░ */
            cch = ACS_BOARD;
            cch2 = cch;
            break;
        case 0x00002592: /* ▒ */
        case 0x00002593: /* ▓ */
            cch = ACS_CKBOARD;
            cch2 = cch;
            break;
        case 0x00002580: /* ▀ */
        case 0x00002584: /* ▄ */
        case 0x00002588: /* █ */
        case 0x0000258c: /* ▌ */
        case 0x00002590: /* ▐ */
        case 0x000025a0: /* ■ */
        case 0x000025ac: /* ▬ */
        case 0x000025ae: /* ▮ */
            cch = ACS_BLOCK;
            cch2 = cch;
            break;
        case 0x000025c6: /* ◆ */
        case 0x00002666: /* ♦ */
            cch = ACS_DIAMOND;
            break;
        case 0x00002022: /* • */
        case 0x000025cb: /* ○ */
        case 0x000025cf: /* ● */
        case 0x00002603: /* ☃ */
        case 0x0000263c: /* ☼ */
            cch = ACS_LANTERN;
            break;
        case 0x0000301c: /* 〜 */
            cch = '~';
            break;
        }
        addch(cch);
        if(caca_utf32_is_fullwidth(ch))
        {
            addch(cch2);
        }
    }
#endif
}

/*
 * Driver initialisation
 */

int ncurses_install(caca_display_t *dp)
{
    dp->drv.id = CACA_DRIVER_NCURSES;
    dp->drv.driver = "ncurses";

    dp->drv.init_graphics = ncurses_init_graphics;
    dp->drv.end_graphics = ncurses_end_graphics;
    dp->drv.set_display_title = ncurses_set_display_title;
    dp->drv.get_display_width = ncurses_get_display_width;
    dp->drv.get_display_height = ncurses_get_display_height;
    dp->drv.display = ncurses_display;
    dp->drv.handle_resize = ncurses_handle_resize;
    dp->drv.get_event = ncurses_get_event;
    dp->drv.set_mouse = NULL;
    dp->drv.set_cursor = ncurses_set_cursor;

    return 0;
}

#endif /* USE_NCURSES */

