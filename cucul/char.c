/*
 *  libcucul      Unicode canvas library
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file char.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Character drawing
 *
 *  This file contains character and string drawing functions.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h> /* BUFSIZ */
#   include <string.h>
#   include <stdlib.h>
#   include <stdarg.h>
#   if defined(HAVE_UNISTD_H)
#       include <unistd.h>
#   endif
#   if defined(HAVE_SIGNAL_H)
#       include <signal.h>
#   endif
#   if defined(HAVE_SYS_IOCTL_H)
#       include <sys/ioctl.h>
#   endif
#endif

#include "cucul.h"
#include "cucul_internals.h"

/** \brief Set the default colour pair.
 *
 *  This function sets the default colour pair. String functions such as
 *  caca_printf() and graphical primitive functions such as caca_draw_line()
 *  will use these colour pairs.
 *
 *  \param fgcolor The requested foreground colour.
 *  \param bgcolor The requested background colour.
 */
void cucul_set_color(cucul_t *qq, enum cucul_color fgcolor,
                                  enum cucul_color bgcolor)
{
    if(fgcolor < 0 || fgcolor > 15 || bgcolor < 0 || bgcolor > 15)
        return;

    qq->fgcolor = fgcolor;
    qq->bgcolor = bgcolor;
}

/** \brief Get the current foreground colour.
 *
 *  This function returns the current foreground colour that was set with
 *  cucul_set_color().
 *
 *  \return The current foreground colour.
 */
enum cucul_color cucul_get_fg_color(cucul_t *qq)
{
    return qq->fgcolor;
}

/** \brief Get the current background colour.
 *
 *  This function returns the current background colour that was set with
 *  cucul_set_color().
 *
 *  \return The current background colour.
 */
enum cucul_color cucul_get_bg_color(cucul_t *qq)
{
    return qq->bgcolor;
}

/** \brief Print an ASCII character.
 *
 *  This function prints an ASCII character at the given coordinates, using
 *  the default foreground and background values. If the coordinates are
 *  outside the screen boundaries, nothing is printed. If the character
 *  value is a non-printable character or is outside the ASCII range, it is
 *  replaced with a space. To print a sequence of bytes forming an UTF-8
 *  character, use cucul_putstr() instead.
 *
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param c The character to print.
 */
void cucul_putchar(cucul_t *qq, int x, int y, char c)
{
    if(x < 0 || x >= (int)qq->width ||
       y < 0 || y >= (int)qq->height)
        return;

    if((unsigned char)c < 0x20 || (unsigned char)c > 0x7f)
        c = 0x20;

    qq->chars[x + y * qq->width] = c;
    qq->attr[x + y * qq->width] = (qq->bgcolor << 4) | qq->fgcolor;
}

/** \brief Print a Unicode character.
 *
 *  FIXME: do we really want this function?
 *
 *  This function prints a Unicode character (native-endian, 32 bits UCS-4,
 *  also known as UTF-32) at the given coordinates, using the default
 *  foreground and background values. If the coordinates are outside the
 *  screen boundaries, nothing is printed. If the character is an invalid
 *  Unicode character, it is replaced with a space.
 *
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param c The character to print.
 */
void cucul_putchar32(cucul_t *qq, int x, int y, unsigned long int c)
{
    if(x < 0 || x >= (int)qq->width ||
       y < 0 || y >= (int)qq->height)
        return;

    if(c < 0x20 || c > 0x7f)
        c = 0x20;

    qq->chars[x + y * qq->width] = c;
    qq->attr[x + y * qq->width] = (qq->bgcolor << 4) | qq->fgcolor;
}

/** \brief Print a string.
 *
 *  This function prints an UTF-8 string at the given coordinates, using the
 *  default foreground and background values. The coordinates may be outside
 *  the screen boundaries (eg. a negative Y coordinate) and the string will
 *  be cropped accordingly if it is too long.
 *
 *  \param x X coordinate.
 *  \param y Y coordinate.
 *  \param s The string to print.
 */
void cucul_putstr(cucul_t *qq, int x, int y, char const *s)
{
    uint32_t *chars;
    uint8_t *attr;
    unsigned int len;

    if(y < 0 || y >= (int)qq->height || x >= (int)qq->width)
        return;

    len = _cucul_strlen_utf8(s);

    if(x < 0)
    {
        if(len < (unsigned int)-x)
            return;
        len -= -x;
        s = _cucul_skip_utf8(s, -x);
        x = 0;
    }

    chars = qq->chars + x + y * qq->width;
    attr = qq->attr + x + y * qq->width;

    if(x + len >= qq->width)
        len = qq->width - x;

    while(len)
    {
        *chars++ = _cucul_utf8_to_utf32(s);
        *attr++ = (qq->bgcolor << 4) | qq->fgcolor;

        s = _cucul_skip_utf8(s, 1);
        len--;
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
void cucul_printf(cucul_t *qq, int x, int y, char const *format, ...)
{
    char tmp[BUFSIZ];
    char *buf = tmp;
    va_list args;

    if(y < 0 || y >= (int)qq->height || x >= (int)qq->width)
        return;

    if(qq->width - x + 1 > BUFSIZ)
        buf = malloc(qq->width - x + 1);

    va_start(args, format);
#if defined(HAVE_VSNPRINTF)
    vsnprintf(buf, qq->width - x + 1, format, args);
#else
    vsprintf(buf, format, args);
#endif
    buf[qq->width - x] = '\0';
    va_end(args);

    cucul_putstr(qq, x, y, buf);

    if(buf != tmp)
        free(buf);
}

/** \brief Get the screen.
 *
 *  This function fills a byte array with the character values.
 */
void cucul_get_screen(cucul_t *qq, char *buffer)
{
    unsigned int x, y;

    for(y = 0; y < qq->height; y++)
    {
        for(x = 0; x < qq->width; x++)
        {
            *buffer++ = qq->attr[x + y * qq->width];
            *buffer++ = qq->chars[x + y * qq->width] & 0x7f; /* FIXME: ASCII */
        }
    }
}

/** \brief Clear the screen.
 *
 *  This function clears the screen using a black background.
 */
void cucul_clear(cucul_t *qq)
{
    enum cucul_color oldfg = cucul_get_fg_color(qq);
    enum cucul_color oldbg = cucul_get_bg_color(qq);
    int y = qq->height;

    cucul_set_color(qq, CUCUL_COLOR_LIGHTGRAY, CUCUL_COLOR_BLACK);

    /* We could use SLsmg_cls() etc., but drawing empty lines is much faster */
    while(y--)
        cucul_putstr(qq, 0, y, qq->empty_line);

    cucul_set_color(qq, oldfg, oldbg);
}

