/*
 *  libcaca       ASCII-Art library
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file driver_win32.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Win32 driver
 *
 *  This file contains the libcaca Win32 input and output driver
 */

#include "config.h"

#if defined(USE_WIN32)

#include <windows.h>

#include <stdio.h> /* BUFSIZ */
#include <string.h>
#include <stdlib.h>
#if defined(HAVE_UNISTD_H)
#   include <unistd.h>
#endif
#include <stdarg.h>

#include "caca.h"
#include "caca_internals.h"
#include "cucul.h"
#include "cucul_internals.h"

/*
 * Global variables
 */

static int const win32_fg_palette[] =
{
    0,
    FOREGROUND_BLUE,
    FOREGROUND_GREEN,
    FOREGROUND_GREEN | FOREGROUND_BLUE,
    FOREGROUND_RED,
    FOREGROUND_RED | FOREGROUND_BLUE,
    FOREGROUND_RED | FOREGROUND_GREEN,
    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    FOREGROUND_INTENSITY,
    FOREGROUND_INTENSITY | FOREGROUND_BLUE,
    FOREGROUND_INTENSITY | FOREGROUND_GREEN,
    FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
    FOREGROUND_INTENSITY | FOREGROUND_RED,
    FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
    FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
    FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
};

static int const win32_bg_palette[] =
{
    0,
    BACKGROUND_BLUE,
    BACKGROUND_GREEN,
    BACKGROUND_GREEN | BACKGROUND_BLUE,
    BACKGROUND_RED,
    BACKGROUND_RED | BACKGROUND_BLUE,
    BACKGROUND_RED | BACKGROUND_GREEN,
    BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
    BACKGROUND_INTENSITY,
    BACKGROUND_INTENSITY | BACKGROUND_BLUE,
    BACKGROUND_INTENSITY | BACKGROUND_GREEN,
    BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE,
    BACKGROUND_INTENSITY | BACKGROUND_RED,
    BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE,
    BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN,
    BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE
};

static int win32_init_graphics(caca_t *kk)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD size;

    /* This call is allowed to fail in case we already have a console */
    AllocConsole();

    kk->win32.hin = GetStdHandle(STD_INPUT_HANDLE);
    kk->win32.hout = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(kk->win32.hout == INVALID_HANDLE_VALUE)
        return -1;

    GetConsoleCursorInfo(kk->win32.hout, &kk->win32.cci);
    kk->win32.cci.bVisible = FALSE;
    SetConsoleCursorInfo(kk->win32.hout, &kk->win32.cci);

    SetConsoleMode(kk->win32.hout, ENABLE_MOUSE_INPUT);

    kk->win32.front =
        CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
                                  0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    if(!kk->win32.front || kk->win32.front == INVALID_HANDLE_VALUE)
        return -1;

    kk->win32.back =
        CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
                                  0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    if(!kk->win32.back || kk->win32.back == INVALID_HANDLE_VALUE)
        return -1;

    if(!GetConsoleScreenBufferInfo(kk->win32.hout, &csbi))
        return -1;

    /* Sample code to get the biggest possible window */
    //size = GetLargestConsoleWindowSize(kk->win32.hout);
    cucul_set_size(kk->qq, csbi.srWindow.Right - csbi.srWindow.Left + 1,
                           csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
    size.X = kk->qq->width;
    size.Y = kk->qq->height;
    SetConsoleScreenBufferSize(kk->win32.front, size);
    SetConsoleScreenBufferSize(kk->win32.back, size);

    SetConsoleMode(kk->win32.front, 0);
    SetConsoleMode(kk->win32.back, 0);

    GetConsoleCursorInfo(kk->win32.front, &kk->win32.cci);
    kk->win32.cci.dwSize = 0;
    kk->win32.cci.bVisible = FALSE;
    SetConsoleCursorInfo(kk->win32.front, &kk->win32.cci);
    SetConsoleCursorInfo(kk->win32.back, &kk->win32.cci);

    SetConsoleActiveScreenBuffer(kk->win32.front);

    kk->win32.buffer = malloc(kk->qq->width * kk->qq->height
                               * sizeof(CHAR_INFO));
    if(kk->win32.buffer == NULL)
        return -1;

    return 0;
}

static int win32_end_graphics(caca_t *kk)
{
    SetConsoleActiveScreenBuffer(kk->win32.hout);
    CloseHandle(kk->win32.back);
    CloseHandle(kk->win32.front);

    SetConsoleTextAttribute(kk->win32.hout, FOREGROUND_INTENSITY
                                             | FOREGROUND_RED
                                             | FOREGROUND_GREEN
                                             | FOREGROUND_BLUE);
    kk->win32.cci.bVisible = TRUE;
    SetConsoleCursorInfo(kk->win32.hout, &kk->win32.cci);
    CloseHandle(kk->win32.hout);

    return 0;
}

static int win32_set_window_title(caca_t *kk, char const *title)
{
    SetConsoleTitle(title);
    return 0;
}

static unsigned int win32_get_window_width(caca_t *kk)
{
    /* FIXME */

    /* Fallback to a 6x10 font */
    return kk->qq->width * 6;
}

static unsigned int win32_get_window_height(caca_t *kk)
{
    /* FIXME */

    /* Fallback to a 6x10 font */
    return kk->qq->height * 10;
}

static void win32_display(caca_t *kk)
{
    COORD size, pos;
    SMALL_RECT rect;
    unsigned int i;

    /* Render everything to our back buffer */
    for(i = 0; i < kk->qq->width * kk->qq->height; i++)
    {
        kk->win32.buffer[i].Char.AsciiChar = kk->qq->chars[i] & 0x7f;
        kk->win32.buffer[i].Attributes =
                win32_fg_palette[kk->qq->attr[i] & 0xf]
                 | win32_bg_palette[kk->qq->attr[i] >> 4];
    }

    /* Blit the back buffer to the front buffer */
    size.X = kk->qq->width;
    size.Y = kk->qq->height;
    pos.X = pos.Y = 0;
    rect.Left = rect.Top = 0;
    rect.Right = kk->qq->width - 1;
    rect.Bottom = kk->qq->height - 1;
    WriteConsoleOutput(kk->win32.front, kk->win32.buffer, size, pos, &rect);
}

static void win32_handle_resize(caca_t *kk, unsigned int *new_width,
                                            unsigned int *new_height)
{
    /* Nothing to do here. */
    *new_width = kk->qq->width;
    *new_height = kk->qq->height;
}

/*
 * Driver initialisation
 */

void win32_init_driver(caca_t *kk)
{
    kk->driver.driver = CACA_DRIVER_WIN32;

    kk->driver.init_graphics = win32_init_graphics;
    kk->driver.end_graphics = win32_end_graphics;
    kk->driver.set_window_title = win32_set_window_title;
    kk->driver.get_window_width = win32_get_window_width;
    kk->driver.get_window_height = win32_get_window_height;
    kk->driver.display = win32_display;
    kk->driver.handle_resize = win32_handle_resize;
}

#endif /* USE_WIN32 */

