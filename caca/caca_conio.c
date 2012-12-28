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

static caca_timer_t refresh_timer = {0, 0};
static uint64_t refresh_ticks;

static int unget_ch = -1;
static int kbhit_ch = -1;
static char pass_buffer[8 + 1];

static void conio_init(void);
static void conio_refresh(void);
static void conio_fini(void);

int caca_conio_directvideo = 0;
int caca_conio__wscroll = 1;

/** \brief DOS conio.h cgets() equivalent */
char * caca_conio_cgets(char *str)
{
    int len = ((uint8_t *)str)[0];
    int pos = 0;

    conio_init();

    while (pos < len)
    {
        int ch = caca_conio_getch();
        if (ch == '\n' || ch == '\r')
            break;
        str[2 + pos] = (char)(uint8_t)ch;
        /* FIXME: handle scrolling */
        caca_put_char(cv, caca_wherex(cv), caca_wherey(cv), ch);
        caca_gotoxy(cv, caca_wherex(cv) + 1, caca_wherey(cv));
        pos++;
    }

    str[2 + pos] = '\0';
    str[1] = (char)(uint8_t)pos;

    conio_refresh();

    return str + 2;
}

/** \brief DOS conio.h clreol() equivalent */
void caca_conio_clreol(void)
{
    conio_init();

    /* FIXME: must work within the currently active text window */
    caca_fill_box(cv, caca_wherex(cv), caca_wherey(cv),
                  caca_get_canvas_width(cv), caca_wherey(cv), ' ');

    conio_refresh();
}

/** \brief DOS conio.h clrscr() equivalent */
void caca_conio_clrscr(void)
{
    conio_init();

    /* FIXME: must work within the currently active text window */
    caca_clear_canvas(cv);
    caca_gotoxy(cv, 0, 0);

    conio_refresh();
}

/** \brief DOS conio.h cprintf() equivalent */
int caca_conio_cprintf(const char *format, ...)
{
    va_list args;
    int ret;

    conio_init();

    /* FIXME: handle scrolling */
    va_start(args, format);
    ret = caca_vprintf(cv, caca_wherex(cv), caca_wherey(cv), format, args);
    va_end(args);

    caca_gotoxy(cv, caca_wherex(cv) + ret, caca_wherey(cv));

    conio_refresh();

    return ret;
}

/** \brief DOS conio.h cputs() equivalent */
int caca_conio_cputs(const char *str)
{
    int ch;

    conio_init();

    while ((ch = (uint8_t)*str++))
    {
        /* FIXME: handle windows, scrolling, '\n' and '\r' */
        caca_put_char(cv, caca_wherex(cv), caca_wherey(cv), ch);
        caca_gotoxy(cv, caca_wherex(cv) + 1, caca_wherey(cv));
    }

    conio_refresh();

    return ch;
}

/** \brief DOS stdio.h cscanf() equivalent */
int caca_conio_cscanf(char *format, ...)
{
    conio_init();

    /* TODO: implement this function */

    return 0;
}

/** \brief DOS dos.h delay() equivalent */
void caca_conio_delay(unsigned int milliseconds)
{
    int64_t usec = (int64_t)milliseconds * 1000;
    caca_timer_t timer = {0, 0};

    conio_init();

    _caca_getticks(&timer);

    /* Refresh screen as long as we have enough time */
    while(usec > 5000)
    {
        conio_refresh();
        _caca_sleep(5000);
        usec -= _caca_getticks(&timer);
    }

    if(usec > 0)
        _caca_sleep(usec);

    conio_refresh();
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
    int ret;

    conio_init();

    if(unget_ch >= 0)
    {
        int tmp = unget_ch;
        unget_ch = -1;
        return tmp;
    }

    if(kbhit_ch >= 0)
    {
        int tmp = kbhit_ch;
        kbhit_ch = -1;
        return tmp;
    }

    while(caca_get_event(dp, CACA_EVENT_KEY_PRESS, &ev, 1000) == 0)
        conio_refresh();

    ret = caca_get_event_key_ch(&ev);

    conio_refresh();

    return ret;
}

/** \brief DOS conio.h getche() equivalent */
int caca_conio_getche(void)
{
    /* conio_init() is called here. */
    int tmp = caca_conio_getch();
    /* conio_refresh() is called here. */
    caca_conio_printf("%c", tmp);

    return tmp;
}

/** \brief DOS conio.h getpass() equivalent */
char * caca_conio_getpass(const char *prompt)
{
    int pos = 0;

    conio_init();

    while (pos < 8)
    {
        int ch = caca_conio_getch();
        if (ch == '\n' || ch == '\r')
            break;
        pass_buffer[pos] = (char)(uint8_t)ch;
        pos++;
    }

    pass_buffer[pos] = '\0';

    conio_refresh();

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
    static caca_timer_t timer = {0, 0};
    static int last_failed = 0;
    caca_event_t ev;

    conio_init();

    /* If last call failed and this call is made less than 100µs
     * afterwards, we assume the caller is in a busy loop and we
     * delay it slightly to avoid resource leakage. */
    if(last_failed && _caca_getticks(&timer) < 100)
    {
        _caca_sleep(1000);
        conio_refresh();
    }

    last_failed = 0;

    if(kbhit_ch >= 0)
        return 1;

    if(caca_get_event(dp, CACA_EVENT_KEY_PRESS, &ev, 0))
    {
        kbhit_ch = caca_get_event_key_ch(&ev);
        return 1;
    }

    last_failed = 1;

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
    caca_canvas_t *tmp;

    conio_init();

    if (left < 1 || top < 1 || left > right || top > bottom
         || destleft < 1 || desttop < 1 || destleft > right
         || desttop > bottom || right > caca_get_canvas_width(cv)
         || bottom > caca_get_canvas_width(cv))
        return 0;

    tmp = caca_create_canvas(right - left + 1, bottom - top + 1);
    caca_blit(tmp, 1 - left, 1 - top, cv, NULL);
    caca_blit(cv, destleft - 1, desttop - 1, tmp, NULL);

    conio_refresh();

    return 1;
}

/** \brief DOS conio.h normvideo() equivalent */
void caca_conio_normvideo(void)
{
    conio_init();

    /* TODO: implement this function */
}

/** \brief DOS dos.h nosound() equivalent */
void caca_conio_nosound(void)
{
    conio_init();

    /* TODO: implement this function */
}

/** \brief DOS stdio.h printf() equivalent */
int caca_conio_printf(const char *format, ...)
{
    va_list args;
    int ret;

    conio_init();

    va_start(args, format);
    ret = caca_vprintf(cv, caca_wherex(cv), caca_wherey(cv), format, args);
    va_end(args);

    caca_gotoxy(cv, caca_wherex(cv) + ret, caca_wherey(cv));

    conio_refresh();

    return 0;
}

/** \brief DOS conio.h putch() equivalent */
int caca_conio_putch(int ch)
{
    conio_init();

    /* FIXME: handle scrolling, windows */
    caca_put_char(cv, caca_wherex(cv), caca_wherey(cv), ch);
    caca_gotoxy(cv, caca_wherex(cv) + 1, caca_wherey(cv));

    return ch;
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

    switch(cur_t)
    {
        case CACA_CONIO__NOCURSOR:
            caca_set_cursor(dp, 0);
            break;
        case CACA_CONIO__SOLIDCURSOR:
        case CACA_CONIO__NORMALCURSOR:
            caca_set_cursor(dp, 1);
            break;
    }

    conio_refresh();
}

/** \brief DOS dos.h sleep() equivalent */
void caca_conio_sleep(unsigned int seconds)
{
    int64_t usec = (int64_t)seconds * 1000000;
    caca_timer_t timer = {0, 0};

    conio_init();

    _caca_getticks(&timer);

    /* Refresh screen as long as we have enough time */
    while(usec > 5000)
    {
        conio_refresh();
        _caca_sleep(5000);
        usec -= _caca_getticks(&timer);
    }

    if(usec > 0)
        _caca_sleep(usec);

    conio_refresh();
}

/** \brief DOS dos.h sound() equivalent */
void caca_conio_sound(unsigned int frequency)
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
        caca_refresh_display(dp);
        caca_set_cursor(dp, 1);
        _caca_getticks(&refresh_timer);
        refresh_ticks = 0;
#if defined HAVE_ATEXIT
        atexit(conio_fini);
#endif
    }
}

static void conio_refresh(void)
{
    refresh_ticks += _caca_getticks(&refresh_timer);
    if(refresh_ticks > 10000)
    {
        caca_refresh_display(dp);
        _caca_getticks(&refresh_timer);
        refresh_ticks = 0;
    }
}

static void conio_fini(void)
{
    caca_free_display(dp);
    dp = NULL;
    caca_free_canvas(cv);
    cv = NULL;
}

