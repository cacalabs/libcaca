/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2009 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *  This file contains the libcaca Win32 input and output driver
 */

#include "config.h"

#if defined(USE_WIN32)

#include <windows.h>

#include <stdlib.h>
#include <stdio.h>

#include "caca.h"
#include "caca_internals.h"

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
    int width = caca_get_canvas_width(dp->cv);
    int height = caca_get_canvas_height(dp->cv);
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
    size.X = width ? width : 80;
    size.Y = height ? height : 25;
    SetConsoleScreenBufferSize(dp->drv.p->screen, size);

    rect.Left = rect.Top = 0;
    rect.Right = size.X - 1;
    rect.Bottom = size.Y - 1;
    SetConsoleWindowInfo(dp->drv.p->screen, TRUE, &rect);

    /* Report our new size to libcaca */
    if(!GetConsoleScreenBufferInfo(dp->drv.p->screen, &csbi))
        return -1;

    dp->resize.allow = 1;
    caca_set_canvas_size(dp->cv,
                          csbi.srWindow.Right - csbi.srWindow.Left + 1,
                          csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
    width = caca_get_canvas_width(dp->cv);
    height = caca_get_canvas_height(dp->cv);
    dp->resize.allow = 0;

    SetConsoleMode(dp->drv.p->screen, 0);

    GetConsoleCursorInfo(dp->drv.p->screen, &dp->drv.p->cci);
    dp->drv.p->cci.dwSize = 0;
    dp->drv.p->cci.bVisible = FALSE;
    SetConsoleCursorInfo(dp->drv.p->screen, &dp->drv.p->cci);

    SetConsoleActiveScreenBuffer(dp->drv.p->screen);

    dp->drv.p->buffer = malloc(width * height
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

static int win32_get_display_width(caca_display_t const *dp)
{
    /* FIXME */

    /* Fallback to a 6x10 font */
    return caca_get_canvas_width(dp->cv) * 6;
}

static int win32_get_display_height(caca_display_t const *dp)
{
    /* FIXME */

    /* Fallback to a 6x10 font */
    return caca_get_canvas_height(dp->cv) * 10;
}

static void win32_display(caca_display_t *dp)
{
    COORD size, pos;
    SMALL_RECT rect;
    CHAR_INFO *buffer = dp->drv.p->buffer;
    uint32_t const *cvchars = caca_get_canvas_chars(dp->cv);
    uint32_t const *cvattrs = caca_get_canvas_attrs(dp->cv);
    int width = caca_get_canvas_width(dp->cv);
    int height = caca_get_canvas_height(dp->cv);
    int n;

    /* Render everything to our screen buffer */
    for(n = height * width; n--; )
    {
        uint32_t ch = *cvchars++;
        uint16_t bgfg = caca_attr_to_ansi(*cvattrs);
        uint8_t fg = bgfg & 0xf;
        uint8_t bg = bgfg >> 4;

#if 0
        if(ch > 0x00000020 && ch < 0x00000080)
            dp->drv.p->buffer[i].Char.AsciiChar = (uint8_t)ch;
        else
            dp->drv.p->buffer[i].Char.AsciiChar = ' ';
#else
        if(n && *cvchars == CACA_MAGIC_FULLWIDTH)
            ;
        else if(ch > 0x00000020 && ch < 0x00010000)
            buffer->Char.UnicodeChar = (uint16_t)ch;
        else
            buffer->Char.UnicodeChar = (uint16_t)' ';
#endif

        buffer->Attributes = win32_fg_palette[fg < 0x10 ? fg : CACA_LIGHTGRAY]
                              | win32_bg_palette[bg < 0x10 ? bg : CACA_BLACK];
        cvattrs++;
        buffer++;
    }

    /* Blit the screen buffer */
    size.X = width;
    size.Y = height;
    pos.X = pos.Y = 0;
    rect.Left = rect.Top = 0;
    rect.Right = width - 1;
    rect.Bottom = height - 1;
#if 0
    WriteConsoleOutput(dp->drv.p->screen, dp->drv.p->buffer, size, pos, &rect);
#else
    /* FIXME: would this benefit from dirty rectangles? */
    WriteConsoleOutputW(dp->drv.p->screen, dp->drv.p->buffer, size, pos, &rect);
#endif
}

static void win32_handle_resize(caca_display_t *dp)
{
    /* FIXME: I don't know what to do here. */
    dp->resize.w = caca_get_canvas_width(dp->cv);
    dp->resize.h = caca_get_canvas_height(dp->cv);
}

static int win32_get_event(caca_display_t *dp, caca_privevent_t *ev)
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
                ev->data.key.utf32 = (uint32_t)ev->data.key.ch;
                ev->data.key.utf8[0] = ev->data.key.ch;
                ev->data.key.utf8[1] = '\0';

                return 1;
            }
            else
            {
                switch (rec.Event.KeyEvent.wVirtualKeyCode)
                {
                case VK_TAB:        ev->data.key.ch = '\t';              break;
                case VK_RETURN:     ev->data.key.ch = '\r';              break;
                case VK_ESCAPE:     ev->data.key.ch = '\033';            break;
                case VK_SPACE:      ev->data.key.ch = ' ';               break;
                case VK_DELETE:     ev->data.key.ch = '\x7f';            break;

                case VK_LEFT:       ev->data.key.ch = CACA_KEY_LEFT;     break;
                case VK_RIGHT:      ev->data.key.ch = CACA_KEY_RIGHT;    break;
                case VK_UP:         ev->data.key.ch = CACA_KEY_UP;       break;
                case VK_DOWN:       ev->data.key.ch = CACA_KEY_DOWN;     break;

                case VK_INSERT:     ev->data.key.ch = CACA_KEY_INSERT;   break;
                case VK_HOME:       ev->data.key.ch = CACA_KEY_HOME;     break;
                case VK_END:        ev->data.key.ch = CACA_KEY_END;      break;
                case VK_PRIOR:      ev->data.key.ch = CACA_KEY_PAGEUP;   break;
                case VK_NEXT:       ev->data.key.ch = CACA_KEY_PAGEDOWN; break;

                case VK_F1:         ev->data.key.ch = CACA_KEY_F1;       break;
                case VK_F2:         ev->data.key.ch = CACA_KEY_F2;       break;
                case VK_F3:         ev->data.key.ch = CACA_KEY_F3;       break;
                case VK_F4:         ev->data.key.ch = CACA_KEY_F4;       break;
                case VK_F5:         ev->data.key.ch = CACA_KEY_F5;       break;
                case VK_F6:         ev->data.key.ch = CACA_KEY_F6;       break;
                case VK_F7:         ev->data.key.ch = CACA_KEY_F7;       break;
                case VK_F8:         ev->data.key.ch = CACA_KEY_F8;       break;
                case VK_F9:         ev->data.key.ch = CACA_KEY_F9;       break;
                case VK_F10:        ev->data.key.ch = CACA_KEY_F10;      break;
                case VK_F11:        ev->data.key.ch = CACA_KEY_F11;      break;
                case VK_F12:        ev->data.key.ch = CACA_KEY_F12;      break;
                case VK_F13:        ev->data.key.ch = CACA_KEY_F13;      break;
                case VK_F14:        ev->data.key.ch = CACA_KEY_F14;      break;
                case VK_F15:        ev->data.key.ch = CACA_KEY_F15;      break;

                case VK_NUMPAD0:    ev->data.key.ch = '0';               break;
                case VK_NUMPAD1:    ev->data.key.ch = '1';               break;
                case VK_NUMPAD2:    ev->data.key.ch = '2';               break;
                case VK_NUMPAD3:    ev->data.key.ch = '3';               break;
                case VK_NUMPAD4:    ev->data.key.ch = '4';               break;
                case VK_NUMPAD5:    ev->data.key.ch = '5';               break;
                case VK_NUMPAD6:    ev->data.key.ch = '6';               break;
                case VK_NUMPAD7:    ev->data.key.ch = '7';               break;
                case VK_NUMPAD8:    ev->data.key.ch = '8';               break;
                case VK_NUMPAD9:    ev->data.key.ch = '9';               break;
                case VK_MULTIPLY:   ev->data.key.ch = '*';               break;
                case VK_ADD:        ev->data.key.ch = '+';               break;
                case VK_SEPARATOR:  ev->data.key.ch = ',';               break;
                case VK_SUBTRACT:   ev->data.key.ch = '-';               break;
                case VK_DECIMAL:    ev->data.key.ch = '.';               break;
                case VK_DIVIDE:     ev->data.key.ch = '/';               break;

                default: ev->type = CACA_EVENT_NONE; return 0;
                }

                if ((ev->data.key.ch > 0)
                    &&
                    (ev->data.key.ch <= 0x7f))
                {
                    ev->data.key.utf32 = (uint32_t)ev->data.key.ch;
                    ev->data.key.utf8[0] = ev->data.key.ch;
                    ev->data.key.utf8[1] = '\0';
                }
                else
                {
                    ev->data.key.utf32 = 0;
                    ev->data.key.utf8[0] = '\0';
                }

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

                if(dp->mouse.x == pos.X && dp->mouse.y == pos.Y)
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
    dp->drv.id = CACA_DRIVER_WIN32;
    dp->drv.driver = "win32";

    dp->drv.init_graphics = win32_init_graphics;
    dp->drv.end_graphics = win32_end_graphics;
    dp->drv.set_display_title = win32_set_display_title;
    dp->drv.get_display_width = win32_get_display_width;
    dp->drv.get_display_height = win32_get_display_height;
    dp->drv.display = win32_display;
    dp->drv.handle_resize = win32_handle_resize;
    dp->drv.get_event = win32_get_event;
    dp->drv.set_mouse = NULL;
    dp->drv.set_cursor = NULL;

    return 0;
}

#endif /* USE_WIN32 */

