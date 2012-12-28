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
 *  This header contains a conio.h reimplementation.
 */

#ifndef __CACA_CONIO_H__
#define __CACA_CONIO_H__

/* Since we're going to redefine standard functions, include these
 * headers first to avoid errors upon later inclusion. */
#if !defined(__KERNEL__)
#   include <stdio.h>
#endif

#include <caca.h>

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
#endif

#if !defined _DOXYGEN_SKIP_ME && !defined __LIBCACA__
#   undef _NOCURSOR
#   define _NOCURSOR CACA_CONIO__NOCURSOR
#   undef _SOLIDCURSOR
#   define _SOLIDCURSOR CACA_CONIO__SOLIDCURSOR
#   undef _NORMALCURSOR
#   define _NORMALCURSOR CACA_CONIO__NORMALCURSOR
#endif

#if !defined _DOXYGEN_SKIP_ME && !defined __LIBCACA__
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
#endif

#if !defined _DOXYGEN_SKIP_ME && !defined __LIBCACA__
#   undef directvideo
#   define directvideo caca_conio_directvideo
#   undef text_info
#   define text_info caca_conio_text_info
#   undef _wscroll
#   define _wscroll caca_conio__wscroll
#endif

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

#endif /* __CACA_CONIO_H__ */
