/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2009 Sam Hocevar <sam@hocevar.net>
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
 *  This file contains a full conio.h reimplementation. More information
 *  on conio.h can be found on http://poli.cs.vsb.cz/c/help/conio.htm
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <stdlib.h>
#endif

#include "caca.h"
#include "caca_internals.h"
#include "caca_conio.h"

static caca_canvas_t *cv;
static caca_display_t *dp;

static int unget_ch = -1;
static char pass_buffer[BUFSIZ];
static char cgets_buffer[BUFSIZ];

static void conio_init(void);
static void conio_refresh(void);
static void conio_fini(void);

/** \brief DOS conio.h cgets() equivalent */
char * caca_conio_cgets(char *str)
{
    conio_init();

    /* TODO: implement this function */
    cgets_buffer[0] = '\0';

    return cgets_buffer;
}

/** \brief DOS conio.h clreol() equivalent */
void caca_conio_clreol(void)
{
    conio_init();

    /* TODO: implement this function */
}

/** \brief DOS conio.h clrscr() equivalent */
void caca_conio_clrscr(void)
{
    conio_init();

    caca_clear_canvas(cv);

    conio_refresh();
}

/** \brief DOS conio.h cprintf() equivalent */
int caca_conio_cprintf(const char *format, ...)
{
    va_list args;

    conio_init();

    va_start(args, format);
    caca_vprintf(cv, caca_wherex(cv), caca_wherey(cv), format, args);
    va_end(args);

    conio_refresh();

    /* FIXME: we should fix caca_vprintf so that it returns the number of
     * characters that were printed. */
    return 0;
}

/** \brief DOS conio.h cputs() equivalent */
int caca_conio_cputs(const char *str)
{
    conio_init();

    /* TODO: implement this function */

    return 0;
}

/** \brief DOS conio.h cscanf() equivalent */
int caca_conio_cscanf(char *format, ...)
{
    conio_init();

    /* TODO: implement this function */

    return 0;
}

/** \brief DOS conio.h delline() equivalent */
void caca_conio_delline(void)
{
    conio_init();

    /* TODO: implement this function */
}

/** \brief DOS conio.h getch() equivalent */
int caca_conio_getch(void)
{
    caca_event_t ev;
    int ch;

    conio_init();

    if(unget_ch >= 0)
    {
        int tmp = unget_ch;
        unget_ch = -1;
        return tmp;
    }

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, &ev, -1);
    ch = caca_get_event_key_ch(&ev);

    switch(ch)
    {
        case CACA_KEY_LEFT: ch = 75; break;
        case CACA_KEY_RIGHT: ch = 77; break;
        default: break;
    }

    return ch;
}

/** \brief DOS conio.h getche() equivalent */
int caca_conio_getche(void)
{
    conio_init();

    /* TODO: implement this function */

    return 0;
}

/** \brief DOS conio.h getpass() equivalent */
char * caca_conio_getpass(const char *prompt)
{
    conio_init();

    /* TODO: implement this function */
    pass_buffer[0] = '\0';

    return pass_buffer;
}

/** \brief DOS conio.h gettext() equivalent */
int caca_conio_gettext(int left, int top, int right, int bottom, void *destin)
{
    conio_init();

    /* TODO: implement this function */

    return 0;
}

/** \brief DOS conio.h gettextinfo() equivalent */
void caca_conio_gettextinfo(struct caca_conio_text_info *r)
{
    conio_init();

    /* TODO: implement this function */
}

/** \brief DOS conio.h gotoxy() equivalent */
void caca_conio_gotoxy(int x, int y)
{
    conio_init();

    caca_gotoxy(cv, x - 1, y - 1);

    conio_refresh();
}

/** \brief DOS conio.h highvideo() equivalent */
void caca_conio_highvideo(void)
{
    conio_init();

    /* TODO: implement this function */
}

/** \brief DOS conio.h insline() equivalent */
void caca_conio_insline(void)
{
    conio_init();

    /* TODO: implement this function */
}

/** \brief DOS conio.h kbhit() equivalent */
int caca_conio_kbhit(void)
{
    conio_init();

    /* TODO: implement this function */

    return 0;
}

/** \brief DOS conio.h lowvideo() equivalent */
void caca_conio_lowvideo(void)
{
    conio_init();

    /* TODO: implement this function */
}

/** \brief DOS conio.h movetext() equivalent */
int caca_conio_movetext(int left, int top, int right, int bottom,
                        int destleft, int desttop)
{
    conio_init();

    /* TODO: implement this function */

    return 0;
}

/** \brief DOS conio.h normvideo() equivalent */
void caca_conio_normvideo(void)
{
    conio_init();

    /* TODO: implement this function */
}

/** \brief DOS stdio.h printf() equivalent */
int caca_conio_printf(const char *format, ...)
{
    va_list args;

    conio_init();

    va_start(args, format);
    caca_vprintf(cv, caca_wherex(cv), caca_wherey(cv), format, args);
    va_end(args);

    conio_refresh();

    /* FIXME: we should fix caca_vprintf so that it returns the number of
     * characters that were printed. */
    return 0;
}

/** \brief DOS conio.h putch() equivalent */
int caca_conio_putch(int ch)
{
    conio_init();

    /* TODO: implement this function */

    return 0;
}

/** \brief DOS conio.h puttext() equivalent */
int caca_conio_puttext(int left, int top, int right, int bottom, void *destin)
{
    conio_init();

    /* TODO: implement this function */

    return 0;
}

/** \brief DOS conio.h _setcursortype() equivalent */
void caca_conio__setcursortype(int cur_t)
{
    conio_init();

    /* TODO: implement this function */
}

/** \brief DOS conio.h textattr() equivalent */
void caca_conio_textattr(int newattr)
{
    conio_init();

    /* TODO: implement this function */
}

/** \brief DOS conio.h textbackground() equivalent */
void caca_conio_textbackground(int newcolor)
{
    conio_init();

    caca_set_color_ansi(cv, caca_attr_to_ansi_fg(caca_get_attr(cv, -1, -1)),
                        newcolor);
}

/** \brief DOS conio.h textcolor() equivalent */
void caca_conio_textcolor(int newcolor)
{
    conio_init();

    caca_set_color_ansi(cv, newcolor,
                        caca_attr_to_ansi_bg(caca_get_attr(cv, -1, -1)));
}

/** \brief DOS conio.h textmode() equivalent */
void caca_conio_textmode(int newmode)
{
    conio_init();

    /* TODO: implement this function */
}

/** \brief DOS conio.h ungetch() equivalent */
int caca_conio_ungetch(int ch)
{
    conio_init();

    if(unget_ch >= 0)
        return EOF;

    unget_ch = ch;

    return ch;
}

/** \brief DOS conio.h wherex() equivalent */
int caca_conio_wherex(void)
{
    conio_init();

    return caca_wherex(cv) + 1;
}

/** \brief DOS conio.h wherey() equivalent */
int caca_conio_wherey(void)
{
    conio_init();

    return caca_wherey(cv) + 1;
}

/** \brief DOS conio.h window() equivalent */
void caca_conio_window(int left, int top, int right, int bottom)
{
    conio_init();

    /* TODO: implement this function */
}

/* XXX: the following functions are local. */

static void conio_init(void)
{
    if(!cv)
        cv = caca_create_canvas(80, 25);
    if(!dp)
    {
        dp = caca_create_display(cv);
        caca_set_cursor(dp, 1);
#if defined HAVE_ATEXIT
        atexit(conio_fini);
#endif
    }
}

static void conio_refresh(void)
{
    caca_refresh_display(dp);
}

static void conio_fini(void)
{
    caca_free_display(dp);
    dp = NULL;
    caca_free_canvas(cv);
    cv = NULL;
}

