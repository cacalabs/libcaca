/*
 *   libcaca       ASCII-Art library
 *   Copyright (c) 2002, 2003 Sam Hocevar <sam@zoy.org>
 *                 All Rights Reserved
 *
 *   $Id$
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *   02111-1307  USA
 */

#include "config.h"

#if defined(USE_SLANG)
#   include <slang.h>
#elif defined(USE_NCURSES)
#   include <curses.h>
#elif defined(USE_CONIO)
#   include <conio.h>
#else
#   error "no graphics library detected"
#endif

#include "caca.h"
#include "caca_internals.h"

static unsigned char back[5] = {0, 0, 0, 0, 0};

static void _push_key(unsigned char key)
{
    back[4] = back[3];
    back[3] = back[2];
    back[2] = back[1];
    back[1] = back[0];
    back[0] = key;
}

static unsigned char _pop_key(void)
{
    unsigned char key = back[0];
    back[0] = back[1];
    back[1] = back[2];
    back[2] = back[3];
    back[3] = back[4];
    return key;
}

static unsigned char _read_key(void)
{
#if defined(USE_SLANG)
    return SLang_input_pending(0) ? SLang_getkey() : 0;
#elif defined(USE_NCURSES)
    unsigned char key = getch();
    return (key == ERR) ? 0 : key;
#elif defined(USE_CONIO)
    return _conio_kbhit() ? getch() : 0;
#endif
}

int caca_get_key(void)
{
    unsigned char key[5];

    /* If there were legacy keys, pop them */
    key[0] = _pop_key();
    if(key[0])
        return key[0];

    key[0] = _read_key();
    if(key[0] != 0x1b)
        return key[0];

    key[1] = _read_key();
    if(!key[1])
        return key[0];

    key[2] = _read_key();
    if(!key[2])
    {
        _push_key(key[1]);
        return key[0];
    }

    if(key[1] == 0x4f)
    {
        switch(key[2])
        {
        case 0x50: return CACA_KEY_F1;
        case 0x51: return CACA_KEY_F2;
        case 0x52: return CACA_KEY_F3;
        case 0x53: return CACA_KEY_F4;
        }
    }
    else if(key[1] == 0x5b)
    {
        switch(key[2])
        {
        case 0x41: return CACA_KEY_UP;
        case 0x42: return CACA_KEY_DOWN;
        case 0x43: return CACA_KEY_LEFT;
        case 0x44: return CACA_KEY_RIGHT;
        }

        key[3] = _read_key();
        key[4] = _read_key();

        if(key[2] == 0x31 && key[4] == 0x7e)
            switch(key[3])
            {
            case 0x35: return CACA_KEY_F5;
            case 0x37: return CACA_KEY_F6;
            case 0x38: return CACA_KEY_F7;
            case 0x39: return CACA_KEY_F8;
            }

        if(key[2] == 0x32 && key[4] == 0x7e)
            switch(key[3])
            {
            case 0x30: return CACA_KEY_F9;
            case 0x31: return CACA_KEY_F10;
            case 0x33: return CACA_KEY_F11;
            case 0x34: return CACA_KEY_F12;
            }

        _push_key(key[4]);
        _push_key(key[3]);
    }

    /* Unknown escape sequence: return 0 */
    _push_key(key[2]);
    _push_key(key[1]);
    return key[0];
}

