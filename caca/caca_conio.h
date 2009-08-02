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
 *  This header contains a conio.h reimplementation.
 */

#ifndef __CACA_CONIO_H__
#define __CACA_CONIO_H__

#include <caca.h>

#if !defined(__KERNEL__)
#   include <stdio.h>
#endif

#undef __extern
#if defined(_DOXYGEN_SKIP_ME)
#elif defined(_WIN32) && defined(__LIBCACA__)
#   define __extern extern __declspec(dllexport)
#else
#   define __extern extern
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* conio.h enums and global variables */
enum CACA_CONIO_COLORS
{
    CACA_CONIO_BLINK = 128,
    CACA_CONIO_BLACK = 0,
    CACA_CONIO_BLUE = 1,
    CACA_CONIO_GREEN = 2,
    CACA_CONIO_CYAN = 3,
    CACA_CONIO_RED = 4,
    CACA_CONIO_MAGENTA = 5,
    CACA_CONIO_BROWN = 6,
    CACA_CONIO_LIGHTGRAY = 7,
    CACA_CONIO_DARKGRAY = 8,
    CACA_CONIO_LIGHTBLUE = 9,
    CACA_CONIO_LIGHTGREEN = 10,
    CACA_CONIO_LIGHTCYAN = 11,
    CACA_CONIO_LIGHTRED = 12,
    CACA_CONIO_LIGHTMAGENTA = 13,
    CACA_CONIO_YELLOW = 14,
    CACA_CONIO_WHITE = 15,
};
__extern int caca_conio_directvideo;
enum CACA_CONIO_CURSOR
{
    CACA_CONIO__NOCURSOR = 0,
    CACA_CONIO__SOLIDCURSOR = 1,
    CACA_CONIO__NORMALCURSOR = 2,
};
struct caca_conio_text_info
{
    unsigned char winleft;        /* left window coordinate */
    unsigned char wintop;         /* top window coordinate */
    unsigned char winright;       /* right window coordinate */
    unsigned char winbottom;      /* bottom window coordinate */
    unsigned char attribute;      /* text attribute */
    unsigned char normattr;       /* normal attribute */
    unsigned char currmode;       /* current video mode:
                                     BW40, BW80, C40, C80, or C4350 */
    unsigned char screenheight;   /* text screen's height */
    unsigned char screenwidth;    /* text screen's width */
    unsigned char curx;           /* x-coordinate in current window */
    unsigned char cury;           /* y-coordinate in current window */
};
enum CACA_CONIO_MODE
{
    CACA_CONIO_LASTMODE = -1,
    CACA_CONIO_BW40 = 0,
    CACA_CONIO_C40 = 1,
    CACA_CONIO_BW80 = 2,
    CACA_CONIO_C80 = 3,
    CACA_CONIO_MONO = 7,
    CACA_CONIO_C4350 = 64,
};
__extern int caca_conio__wscroll;

#if !defined _DOXYGEN_SKIP_ME && !defined __LIBCACA__
#   undef BLINK
#   define BLINK CACA_CONIO_BLINK
#   undef BLACK
#   define BLACK CACA_CONIO_BLACK
#   undef BLUE
#   define BLUE CACA_CONIO_BLUE
#   undef GREEN
#   define GREEN CACA_CONIO_GREEN
#   undef CYAN
#   define CYAN CACA_CONIO_CYAN
#   undef RED
#   define RED CACA_CONIO_RED
#   undef MAGENTA
#   define MAGENTA CACA_CONIO_MAGENTA
#   undef BROWN
#   define BROWN CACA_CONIO_BROWN
#   undef LIGHTGRAY
#   define LIGHTGRAY CACA_CONIO_LIGHTGRAY
#   undef DARKGRAY
#   define DARKGRAY CACA_CONIO_DARKGRAY
#   undef LIGHTBLUE
#   define LIGHTBLUE CACA_CONIO_LIGHTBLUE
#   undef LIGHTGREEN
#   define LIGHTGREEN CACA_CONIO_LIGHTGREEN
#   undef LIGHTCYAN
#   define LIGHTCYAN CACA_CONIO_LIGHTCYAN
#   undef LIGHTRED
#   define LIGHTRED CACA_CONIO_LIGHTRED
#   undef LIGHTMAGENTA
#   define LIGHTMAGENTA CACA_CONIO_LIGHTMAGENTA
#   undef YELLOW
#   define YELLOW CACA_CONIO_YELLOW
#   undef WHITE
#   define WHITE CACA_CONIO_WHITE
#   undef directvideo
#   define directvideo caca_conio_directvideo
#   undef _NOCURSOR
#   define _NOCURSOR CACA_CONIO__NOCURSOR
#   undef _SOLIDCURSOR
#   define _SOLIDCURSOR CACA_CONIO__SOLIDCURSOR
#   undef _NORMALCURSOR
#   define _NORMALCURSOR CACA_CONIO__NORMALCURSOR
#   undef text_info
#   define text_info caca_conio_text_info
#   undef LASTMODE
#   define LASTMODE CACA_CONIO_LASTMODE
#   undef BW40
#   define BW40 CACA_CONIO_BW40
#   undef C40
#   define C40 CACA_CONIO_C40
#   undef BW80
#   define BW80 CACA_CONIO_BW80
#   undef C80
#   define C80 CACA_CONIO_C80
#   undef MONO
#   define MONO CACA_CONIO_MONO
#   undef C4350
#   define C4350 CACA_CONIO_C4350
#   undef _wscroll
#   define _wscroll caca_conio__wscroll
#endif

/* conio.h functions */
__extern char * caca_conio_cgets(char *str);
__extern void   caca_conio_clreol(void);
__extern void   caca_conio_clrscr(void);
__extern int    caca_conio_cprintf(const char *format, ...);
__extern int    caca_conio_cputs(const char *str);
__extern int    caca_conio_cscanf(char *format, ...);
__extern void   caca_conio_delay(unsigned int);
__extern void   caca_conio_delline(void);
__extern int    caca_conio_getch(void);
__extern int    caca_conio_getche(void);
__extern char * caca_conio_getpass(const char *prompt);
__extern int    caca_conio_gettext(int left, int top, int right, int bottom,
                                   void *destin);
__extern void   caca_conio_gettextinfo(struct caca_conio_text_info *r);
__extern void   caca_conio_gotoxy(int x, int y);
__extern void   caca_conio_highvideo(void);
__extern void   caca_conio_insline(void);
__extern int    caca_conio_kbhit(void);
__extern void   caca_conio_lowvideo(void);
__extern int    caca_conio_movetext(int left, int top, int right, int bottom,
                                    int destleft, int desttop);
__extern void   caca_conio_normvideo(void);
__extern void   caca_conio_nosound(void);
__extern int    caca_conio_printf(const char *format, ...);
__extern int    caca_conio_putch(int ch);
__extern int    caca_conio_puttext(int left, int top, int right, int bottom,
                                   void *destin);
__extern void   caca_conio__setcursortype(int cur_t);
__extern void   caca_conio_sleep(unsigned int);
__extern void   caca_conio_sound(unsigned int);
__extern void   caca_conio_textattr(int newattr);
__extern void   caca_conio_textbackground(int newcolor);
__extern void   caca_conio_textcolor(int newcolor);
__extern void   caca_conio_textmode(int newmode);
__extern int    caca_conio_ungetch(int ch);
__extern int    caca_conio_wherex(void);
__extern int    caca_conio_wherey(void);
__extern void   caca_conio_window(int left, int top, int right, int bottom);

#if !defined _DOXYGEN_SKIP_ME && !defined __LIBCACA__
#   undef cgets
#   define cgets caca_conio_cgets
#   undef clreol
#   define clreol caca_conio_clreol
#   undef clrscr
#   define clrscr caca_conio_clrscr
#   undef cprintf
#   define cprintf caca_conio_cprintf
#   undef cputs
#   define cputs caca_conio_cputs
#   undef cscanf
#   define cscanf caca_conio_cscanf
#   undef delay
#   define delay caca_conio_delay
#   undef delline
#   define delline caca_conio_delline
#   undef getch
#   define getch caca_conio_getch
#   undef getche
#   define getche caca_conio_getche
#   undef getpass
#   define getpass caca_conio_getpass
#   undef gettext
#   define gettext caca_conio_gettext
#   undef gettextinfo
#   define gettextinfo caca_conio_gettextinfo
#   undef gotoxy
#   define gotoxy caca_conio_gotoxy
#   undef highvideo
#   define highvideo caca_conio_highvideo
#   undef insline
#   define insline caca_conio_insline
#   undef kbhit
#   define kbhit caca_conio_kbhit
#   undef lowvideo
#   define lowvideo caca_conio_lowvideo
#   undef movetext
#   define movetext caca_conio_movetext
#   undef normvideo
#   define normvideo caca_conio_normvideo
#   undef nosound
#   define nosound caca_conio_nosound
#   undef printf
#   define printf caca_conio_printf
#   undef putch
#   define putch caca_conio_putch
#   undef puttext
#   define puttext caca_conio_puttext
#   undef _setcursortype
#   define _setcursortype caca_conio__setcursortype
#   undef sleep
#   define sleep caca_conio_sleep
#   undef sound
#   define sound caca_conio_sound
#   undef textattr
#   define textattr caca_conio_textattr
#   undef textbackground
#   define textbackground caca_conio_textbackground
#   undef textcolor
#   define textcolor caca_conio_textcolor
#   undef textmode
#   define textmode caca_conio_textmode
#   undef ungetch
#   define ungetch caca_conio_ungetch
#   undef wherex
#   define wherex caca_conio_wherex
#   undef wherey
#   define wherey caca_conio_wherey
#   undef window
#   define window caca_conio_window
#endif

#ifdef __cplusplus
}
#endif

#endif /* __CACA_CONIO_H__ */
