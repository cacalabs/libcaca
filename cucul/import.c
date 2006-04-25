/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
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
 *  This file contains various import functions.
 */

#include "config.h"
#include "common.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

static cucul_canvas_t *import_caca(void const *, unsigned int);
static cucul_canvas_t *import_text(void const *, unsigned int);
static cucul_canvas_t *import_ansi(void const *, unsigned int);

/** \brief Import a buffer into a canvas
 *
 *  This function imports a memory area into an internal libcucul canvas.
 *
 *  Valid values for \c format are:
 *
 *  \li \c "": attempt to autodetect the file format.
 *
 *  \li \c "ansi": import ANSI files.
 *
 *  \li \c "caca": import native libcaca files.
 *
 *  \param data The memory area to be loaded into a canvas.
 *  \param size The length of the memory area.
 *  \param format A string describing the input format.
 *  \return A libcucul canvas, or NULL in case of error.
 */
cucul_canvas_t * cucul_import_canvas(void const *data, unsigned int size,
                                     char const *format)
{
    char const *buf = (char const*) data;

    if(size==0 || data==NULL)
        return NULL;

    if(!strcasecmp("caca", format))
        return import_caca(data, size);
    if(!strcasecmp("text", format))
        return import_text(data, size);
    if(!strcasecmp("ansi", format))
        return import_ansi(data, size);

    /* Autodetection */
    if(!strcasecmp("", format))
    {
        /* if 4 first letters are CACA */
        if(size >= 4 &&
            buf[0] == 'C' && buf[1] == 'A' && buf[2] == 'C' && buf[3] != 'A')
            return import_caca(data, size);

        /* If 2 first characters are ESC[ (not reliable at all) */
        if(size >= 2 && buf[0] == 0x1b && buf[1] == '[')
            return import_ansi(data, size);

        /* Otherwise, import it as text */
        return import_text(data, size);
    }
    return NULL;
}

/** \brief Get available import formats
 *
 *  Return a list of available import formats. The list is a NULL-terminated
 *  array of strings, interleaving a string containing the internal value for
 *  the import format, to be used with cucul_import_canvas(), and a string
 *  containing the natural language description for that import format.
 *
 *  \return An array of strings.
 */
char const * const * cucul_get_import_list(void)
{
    static char const * const list[] =
    {
        "", "autodetect",
        "text", "plain text",
        "caca", "native libcaca format",
        "ansi", "ANSI coloured text",
        NULL, NULL
    };

    return list;
}

/*
 * XXX: the following functions are local.
 */

static cucul_canvas_t *import_caca(void const *data, unsigned int size)
{
    cucul_canvas_t *cv;
    uint8_t const *buf = (uint8_t const *)data;
    unsigned int width, height, n;

    if(size < 16)
        return NULL;

    if(buf[0] != 'C' || buf[1] != 'A' || buf[2] != 'C' || buf[3] != 'A')
        return NULL;

    if(buf[4] != 'C' || buf[5] != 'A' || buf[6] != 'N' || buf[7] != 'V')
        return NULL;

    width = ((uint32_t)buf[8] << 24) | ((uint32_t)buf[9] << 16)
           | ((uint32_t)buf[10] << 8) | (uint32_t)buf[11];
    height = ((uint32_t)buf[12] << 24) | ((uint32_t)buf[13] << 16)
            | ((uint32_t)buf[14] << 8) | (uint32_t)buf[15];

    if(!width || !height)
        return NULL;

    if(size != 16 + width * height * 8)
        return NULL;

    cv = cucul_create_canvas(width, height);

    if(!cv)
        return NULL;

    for(n = height * width; n--; )
    {
        cv->chars[n] = ((uint32_t)buf[16 + 0 + 8 * n] << 24)
                     | ((uint32_t)buf[16 + 1 + 8 * n] << 16)
                     | ((uint32_t)buf[16 + 2 + 8 * n] << 8)
                     | (uint32_t)buf[16 + 3 + 8 * n];
        cv->attr[n] = ((uint32_t)buf[16 + 4 + 8 * n] << 24)
                    | ((uint32_t)buf[16 + 5 + 8 * n] << 16)
                    | ((uint32_t)buf[16 + 6 + 8 * n] << 8)
                    | (uint32_t)buf[16 + 7 + 8 * n];
    }

    return cv;
}

static cucul_canvas_t *import_text(void const *data, unsigned int size)
{
    cucul_canvas_t *cv;
    char const *text = (char const *)data;
    unsigned int width = 1, height = 1, x = 0, y = 0, i;

    cv = cucul_create_canvas(width, height);
    cucul_set_color(cv, CUCUL_COLOR_DEFAULT, CUCUL_COLOR_TRANSPARENT);

    for(i = 0; i < size; i++)
    {
        unsigned char ch = *text++;

        if(ch == '\r')
            continue;

        if(ch == '\n')
        {
            x = 0;
            y++;
            continue;
        }

        if(x >= width || y >= height)
        {
            if(x >= width)
                width = x + 1;

            if(y >= height)
                height = y + 1;

            cucul_set_canvas_size(cv, width, height);
        }

        cucul_putchar(cv, x, y, ch);
        x++;
    }

    return cv;
}

#define IS_ALPHA(x) (x>='A' && x<='z')
#define END_TUP 0x1337
unsigned char _get_ansi_command(unsigned char const *buffer, int size);
int _parse_tuple(unsigned int *ret, unsigned char const *buffer, int size);
void _manage_modifiers(int i, int *fg, int *bg, int *save_fg, int *save_bg, int *bold);

static cucul_canvas_t *import_ansi(void const *data, unsigned int size)
{
    cucul_canvas_t *cv;
    unsigned char const *buffer = (unsigned char const*)data;
    unsigned int i, sent_size = 0;
    unsigned char c;
    unsigned int count = 0;
    unsigned int tuple[1024]; /* Should be enough. Will it be ? */
    int x = 0, y = 0, width = 80, height = 25;
    int save_x = 0, save_y = 0;
    unsigned int j, add = 0;
    int fg, bg, save_fg, save_bg, bold;

    fg = save_fg = CUCUL_COLOR_LIGHTGRAY;
    bg = save_bg = CUCUL_COLOR_BLACK;
    bold = 0;

    cv = cucul_create_canvas(width, height);

    for(i = 0; i < size; i++)
    {
        if((buffer[i] == 0x1b) && (buffer[i + 1] == '['))  /* ESC code */
        {
            i++; // ESC
            i++; // [
            sent_size = size - i;
            c = _get_ansi_command(&buffer[i], sent_size);
            add = _parse_tuple(tuple, &buffer[i], sent_size);
            count = 0;

            while(tuple[count] != END_TUP)
                count++;  /* Gruik */

            switch(c)
            {
            case 'f':
            case 'H':
                if(tuple[0] != END_TUP)
                {
                    y = tuple[0] - 1;
                    x = tuple[1] == END_TUP ? 0 : tuple[1] - 1;
                }
                else
                {
                    y = 0;
                    x = 0;
                }
                break;
            case 'A':
                y -= tuple[0] == END_TUP ? 1 : tuple[0];
                if(y < 0)
                    y = 0;
                break;
            case 'B':
                y += tuple[0] == END_TUP ? 1 : tuple[0];
                break;
            case 'C':
                x += tuple[0] == END_TUP ? 1 : tuple[0];
                break;
            case 'D':
                x -= tuple[0] == END_TUP ? 1 : tuple[0];
                if(x < 0)
                    x = 0;
                break;
            case 's':
                save_x = x;
                save_y = y;
                break;
            case 'u':
                x = save_x;
                y = save_y;
                break;
            case 'J':
                if(tuple[0] == 2)
                {
                    x = 0;
                    y = 0;
                }
                break;
            case 'K':
                // CLEAR END OF LINE
                break;
            case 'm':
                for(j = 0; j < count; j++)
                    _manage_modifiers(tuple[j], &fg, &bg, &save_fg, &save_bg, &bold);
                if(bold && fg < 8)
                    fg += 8;
                if(bold && bg < 8)
                    bg += 8;
                cucul_set_color(cv, fg, bg);
                break;
            default:
                /*printf("Unknow command %c (%c)\n", c, IS_ALPHA(c)?c:'.');*/
                break;
            }
        }
        else
        {
            if(buffer[i] == '\n')
            {
                x = 0;
                y++;
            }
            else if(buffer[i] == '\r')
            {
                    // DOS sucks.
            }
            else
            {
                _cucul_putchar32(cv, x, y,_cucul_cp437_to_utf32(buffer[i]));
                x++;
            }
        }

        if(x >= width || y >= height)
        {
            if(x >= width)
                width = x + 1;

            if(y >= height)
                height = y + 1;

            cucul_set_canvas_size(cv, width, height);
        }

        i += add; // add is tuple char count, then +[ +command
        add = 0;
    }

    return cv;
}

/* XXX : ANSI loader helpers */

unsigned char _get_ansi_command(unsigned char const *buffer, int size)
{
    int i;

    for(i = 0; i < size; i++)
        if(IS_ALPHA(buffer[i]))
            return buffer[i];

    return 0;
}

int _parse_tuple(unsigned int *ret, unsigned char const *buffer, int size)
{
    int i = 0;
    int j = 0;
    int t = 0;
    unsigned char nbr[1024];

    ret[0] = END_TUP;

    for(i = 0; i < size; i++)
    {
        if(IS_ALPHA(buffer[i]))
        {
            if(j != 0)
            {
                ret[t] = atoi((char*)nbr);
                t++;
            }
            ret[t] = END_TUP;
            j = 0;
            return i;
        }

        if(buffer[i] != ';')
        {
            nbr[j] = buffer[i];
            nbr[j + 1] = 0;
            j++;
        }
        else
        {
            ret[t] = atoi((char*)nbr);
            t++;
            ret[t] = END_TUP;
            j = 0;
        }
    }
    return size;
}



void _manage_modifiers(int i, int *fg, int *bg, int *save_fg, int *save_bg, int *bold)
{
    static uint8_t const ansi2cucul[] =
    {
        CUCUL_COLOR_BLACK,
        CUCUL_COLOR_RED,
        CUCUL_COLOR_GREEN,
        CUCUL_COLOR_BROWN,
        CUCUL_COLOR_BLUE,
        CUCUL_COLOR_MAGENTA,
        CUCUL_COLOR_CYAN,
        CUCUL_COLOR_LIGHTGRAY
    };

    if(i >= 30 && i <= 37)
        *fg = ansi2cucul[i - 30];
    else if(i == 39)
        *fg = CUCUL_COLOR_DEFAULT;
    else if(i >= 40 && i <= 47)
        *bg = ansi2cucul[i - 40];
    else if(i == 49)
        *bg = CUCUL_COLOR_DEFAULT;
    else if(i >= 90 && i <= 97)
        *fg = ansi2cucul[i - 90] + 8;
    else if(i >= 100 && i <= 107)
        *bg = ansi2cucul[i - 100] + 8;
    else switch(i)
    {
    case 0:
        *fg = CUCUL_COLOR_DEFAULT;
        *bg = CUCUL_COLOR_DEFAULT;
        *bold = 0;
        break;
    case 1: /* BOLD */
        *bold = 1;
        break;
    case 4: // Underline
        break;
    case 5: // blink
        break;
    case 7: // reverse
        *fg = 15 - *fg;
        *bg = 15 - *bg;
        break;
    case 8: // invisible
        *save_fg = *fg;
        *save_bg = *bg;
        *fg = CUCUL_COLOR_TRANSPARENT;
        *bg = CUCUL_COLOR_TRANSPARENT;
        break;
    case 28: // not invisible
        *fg = *save_fg;
        *bg = *save_bg;
        break;
    default:
        /*   printf("Unknow option to 'm' %d (%c)\n", c, IS_ALPHA(c)?c:'.'); */
        break;
    }
}

