/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *  This file contains the libcaca Win32 input and output driver
 */

#include "config.h"
#include "common.h"

#if defined(USE_WIN32)

#include <windows.h>

#include <stdlib.h>
#include <stdio.h>

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

struct driver_private
{
    HANDLE hin, hout, screen;
    CHAR_INFO *buffer;
    CONSOLE_CURSOR_INFO cci;
};

static int win32_init_graphics(caca_display_t *dp)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    SMALL_RECT rect;
    COORD size;

    dp->drv.p = malloc(sizeof(struct driver_private));

    /* This call is allowed to fail in case we already have a console */
    AllocConsole();

    dp->drv.p->hin = GetStdHandle(STD_INPUT_HANDLE);
    dp->drv.p->hout = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(dp->drv.p->hout == INVALID_HANDLE_VALUE)
        return -1;

    GetConsoleCursorInfo(dp->drv.p->hout, &dp->drv.p->cci);
    dp->drv.p->cci.bVisible = FALSE;
    SetConsoleCursorInfo(dp->drv.p->hout, &dp->drv.p->cci);

    SetConsoleMode(dp->drv.p->hout, ENABLE_MOUSE_INPUT);

    dp->drv.p->screen =
        CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
                                  0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    if(!dp->drv.p->screen || dp->drv.p->screen == INVALID_HANDLE_VALUE)
        return -1;

    /* Set the new console size */
    size.X = dp->cv->width;
    size.Y = dp->cv->height;
    SetConsoleScreenBufferSize(dp->drv.p->screen, size);

    rect.Left = rect.Top = 0;
    rect.Right = dp->cv->width - 1;
    rect.Bottom = dp->cv->height - 1;
    SetConsoleWindowInfo(dp->drv.p->screen, TRUE, &rect);

    /* Report our new size to libcucul */
    if(!GetConsoleScreenBufferInfo(dp->drv.p->screen, &csbi))
        return -1;

    _cucul_set_canvas_size(dp->cv, csbi.srWindow.Right - csbi.srWindow.Left + 1,
                           csbi.srWindow.Bottom - csbi.srWindow.Top + 1);

    SetConsoleMode(dp->drv.p->screen, 0);

    GetConsoleCursorInfo(dp->drv.p->screen, &dp->drv.p->cci);
    dp->drv.p->cci.dwSize = 0;
    dp->drv.p->cci.bVisible = FALSE;
    SetConsoleCursorInfo(dp->drv.p->screen, &dp->drv.p->cci);

    SetConsoleActiveScreenBuffer(dp->drv.p->screen);

    dp->drv.p->buffer = malloc(dp->cv->width * dp->cv->height
                               * sizeof(CHAR_INFO));
    if(dp->drv.p->buffer == NULL)
        return -1;

    return 0;
}

static int win32_end_graphics(caca_display_t *dp)
{
    SetConsoleActiveScreenBuffer(dp->drv.p->hout);
    CloseHandle(dp->drv.p->screen);

    SetConsoleTextAttribute(dp->drv.p->hout, FOREGROUND_INTENSITY
                                             | FOREGROUND_RED
                                             | FOREGROUND_GREEN
                                             | FOREGROUND_BLUE);
    dp->drv.p->cci.bVisible = TRUE;
    SetConsoleCursorInfo(dp->drv.p->hout, &dp->drv.p->cci);
    CloseHandle(dp->drv.p->hout);

    free(dp->drv.p);

    return 0;
}

static int win32_set_display_title(caca_display_t *dp, char const *title)
{
    SetConsoleTitle(title);
    return 0;
}

static unsigned int win32_get_display_width(caca_display_t *dp)
{
    /* FIXME */

    /* Fallback to a 6x10 font */
    return dp->cv->width * 6;
}

static unsigned int win32_get_display_height(caca_display_t *dp)
{
    /* FIXME */

    /* Fallback to a 6x10 font */
    return dp->cv->height * 10;
}

static void win32_display(caca_display_t *dp)
{
    COORD size, pos;
    SMALL_RECT rect;
    unsigned int i;

    /* Render everything to our screen buffer */
    for(i = 0; i < dp->cv->width * dp->cv->height; i++)
    {
        uint32_t ch = dp->cv->chars[i];

#if 0
        if(ch > 0x00000020 && ch < 0x00000080)
            dp->drv.p->buffer[i].Char.AsciiChar = (uint8_t)ch;
        else
            dp->drv.p->buffer[i].Char.AsciiChar = ' ';
#else
        if(ch > 0x00000020 && ch < 0x00010000)
            dp->drv.p->buffer[i].Char.UnicodeChar = (uint16_t)ch;
        else
            dp->drv.p->buffer[i].Char.UnicodeChar = (uint16_t)' ';
#endif

        dp->drv.p->buffer[i].Attributes =
                win32_fg_palette[_cucul_argb32_to_ansi4fg(dp->cv->attr[i])]
                 | win32_bg_palette[_cucul_argb32_to_ansi4bg(dp->cv->attr[i])];
    }

    /* Blit the screen buffer */
    size.X = dp->cv->width;
    size.Y = dp->cv->height;
    pos.X = pos.Y = 0;
    rect.Left = rect.Top = 0;
    rect.Right = dp->cv->width - 1;
    rect.Bottom = dp->cv->height - 1;
#if 0
    WriteConsoleOutput(dp->drv.p->screen, dp->drv.p->buffer, size, pos, &rect);
#else
    WriteConsoleOutputW(dp->drv.p->screen, dp->drv.p->buffer, size, pos, &rect);
#endif
}

static void win32_handle_resize(caca_display_t *dp)
{
    /* FIXME: I don't know what to do here. */
    dp->resize.w = dp->cv->width;
    dp->resize.h = dp->cv->height;
}

static int win32_get_event(caca_display_t *dp, caca_event_t *ev)
{
    INPUT_RECORD rec;
    DWORD num;

    for( ; ; )
    {
        GetNumberOfConsoleInputEvents(dp->drv.p->hin, &num);
        if(num == 0)
            break;

        ReadConsoleInput(dp->drv.p->hin, &rec, 1, &num);
        if(rec.EventType == KEY_EVENT)
        {
            if(rec.Event.KeyEvent.bKeyDown)
                ev->type = CACA_EVENT_KEY_PRESS;
            else
                ev->type = CACA_EVENT_KEY_RELEASE;

            if(rec.Event.KeyEvent.uChar.AsciiChar)
            {
                ev->data.key.ch = rec.Event.KeyEvent.uChar.AsciiChar;
                ev->data.key.ucs4 = (uint32_t)ev->data.key.ch;
                ev->data.key.utf8[0] = ev->data.key.ch;
                ev->data.key.utf8[1] = '\0';

                return 1;
            }
        }

        if(rec.EventType == MOUSE_EVENT)
        {
            if(rec.Event.MouseEvent.dwEventFlags == 0)
            {
                if(rec.Event.MouseEvent.dwButtonState & 0x01)
                {
                    ev->type = CACA_EVENT_MOUSE_PRESS;
                    ev->data.mouse.button = 1;
                    return 1;
                }

                if(rec.Event.MouseEvent.dwButtonState & 0x02)
                {
                    ev->type = CACA_EVENT_MOUSE_PRESS;
                    ev->data.mouse.button = 2;
                    return 1;
                }
            }
            else if(rec.Event.MouseEvent.dwEventFlags == MOUSE_MOVED)
            {
                COORD pos = rec.Event.MouseEvent.dwMousePosition;

                if(dp->mouse.x == (unsigned int)pos.X &&
                   dp->mouse.y == (unsigned int)pos.Y)
                    continue;

                dp->mouse.x = pos.X;
                dp->mouse.y = pos.Y;

                ev->type = CACA_EVENT_MOUSE_MOTION;
                ev->data.mouse.x = dp->mouse.x;
                ev->data.mouse.y = dp->mouse.y;
                return 1;
            }
#if 0
            else if(rec.Event.MouseEvent.dwEventFlags == DOUBLE_CLICK)
            {
                cout << rec.Event.MouseEvent.dwMousePosition.X << "," <<
                        rec.Event.MouseEvent.dwMousePosition.Y << "  " << flush;
            }
            else if(rec.Event.MouseEvent.dwEventFlags == MOUSE_WHEELED)
            {
                SetConsoleCursorPosition(hOut,
                                         WheelWhere);
                if(rec.Event.MouseEvent.dwButtonState & 0xFF000000)
                    cout << "Down" << flush;
                else
                    cout << "Up  " << flush;
            }
#endif
        }

        /* Unknown event */
        ev->type = CACA_EVENT_NONE;
        return 0;
    }

    /* No event */
    ev->type = CACA_EVENT_NONE;
    return 0;
}

/*
 * Driver initialisation
 */

int win32_install(caca_display_t *dp)
{
    dp->drv.driver = CACA_DRIVER_WIN32;

    dp->drv.init_graphics = win32_init_graphics;
    dp->drv.end_graphics = win32_end_graphics;
    dp->drv.set_display_title = win32_set_display_title;
    dp->drv.get_display_width = win32_get_display_width;
    dp->drv.get_display_height = win32_get_display_height;
    dp->drv.display = win32_display;
    dp->drv.handle_resize = win32_handle_resize;
    dp->drv.get_event = win32_get_event;
    dp->drv.set_mouse = NULL;

    return 0;
}

#endif /* USE_WIN32 */

