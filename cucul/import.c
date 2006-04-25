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
void updateCanvasSize(cucul_canvas_t *cv, int x, int y, int *max_x, int *max_y);
unsigned char get_ansi_command(unsigned char const *buffer, int size);
int parse_tuple(unsigned int *ret, unsigned char const *buffer, int size);
void manage_modifiers(char c, int *fg, int *bg, int *old_fg, int *old_bg);

static cucul_canvas_t *import_ansi(void const *data, unsigned int size)
{
    cucul_canvas_t *cv;
    unsigned char const *buffer = (unsigned char const*)data;
    unsigned int i, sent_size = 0;
    unsigned char c;
    unsigned int count = 0;
    unsigned int tuple[1024]; /* Should be enough. Will it be ? */
    int x = 0, y = 0, max_x = 80, max_y = 25;
    int save_x = 0, save_y = 0;
    unsigned int j, add = 0;
    int fg, bg, old_fg, old_bg;

    fg = old_fg = CUCUL_COLOR_LIGHTGRAY;
    bg = old_bg = CUCUL_COLOR_BLACK;

    cv = cucul_create_canvas(max_x, max_y);

    for(i = 0; i < size; i++)
    {
        if((buffer[i] == 0x1b) && (buffer[i + 1] == '['))  /* ESC code */
        {
            i++; // ESC
            i++; // [
            sent_size = size - i;
            c = get_ansi_command(&buffer[i], sent_size);
            add = parse_tuple(tuple, &buffer[i], sent_size);
            count = 0;

            while(tuple[count] != 0x1337)
                count++;  /* Gruik */

            switch(c)
            {
            case 'f':
            case 'H':
                if(tuple[0] != 0x1337)
                {
                    x = tuple[0];
                    if(tuple[1] != 0x1337)
                        y = tuple[1];
                }
                else
                {
                    x = 0;
                    y = 0;
                }
                updateCanvasSize(cv, x, y, &max_x, &max_y);
                break;
            case 'A':
                if(tuple[0] == 0x1337)
                    y -= 1;
                else
                    y -= tuple[0];
                if(y < 0) y = 0;
                updateCanvasSize(cv, x, y, &max_x, &max_y);
                break;
            case 'B':
                if(tuple[0] == 0x1337)
                    y++;
                else
                    y += tuple[0];
                updateCanvasSize(cv, x, y, &max_x, &max_y);
                break;
            case 'C':
                if(tuple[0] == 0x1337)
                    x++;
                else
                    x += tuple[0];
                updateCanvasSize(cv, x, y, &max_x, &max_y);
                break;
            case 'D':
                if(tuple[0] == 0x1337)
                    x--;
                else
                    x -= tuple[0];
                if(x < 0) x = 0;
                updateCanvasSize(cv, x, y, &max_x, &max_y);
                break;
            case 's':
                save_x = x;
                save_y = y;
                break;
            case 'u':
                x = save_x;
                y = save_y;
                updateCanvasSize(cv, x, y, &max_x, &max_y);
                break;
            case 'J':
                if(tuple[0] == 2)
                {
                    x = 0;
                    y = 0;
                    updateCanvasSize(cv, x, y, &max_x, &max_y);
                }
                break;
            case 'K':
                // CLEAR END OF LINE
                break;
            case 'm':
                for(j = 0; j < count; j++)
                    manage_modifiers(tuple[j], &fg, &bg, &old_fg, &old_bg);
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
                updateCanvasSize(cv, x, y, &max_x, &max_y);
            }
            else if(buffer[i] == '\r')
            {
                    // DOS sucks.
            }
            else
            {
                if((buffer[i] >= 0x20) || (buffer[i] <= 0x7E))
                {
                    _cucul_putchar32(cv, x, y,_cucul_cp437_to_utf32(buffer[i]));

                    //  cucul_putchar(cv, x, y, buffer[i]);
                }
                else
                    cucul_putchar(cv, x, y, '?');
                x++;
                updateCanvasSize(cv, x, y, &max_x, &max_y);
            }
        }

        i += add; // add is tuple char count, then +[ +command
        add = 0;
    }

    return cv;
}

/* XXX : ANSI loader helpers */

unsigned char get_ansi_command(unsigned char const *buffer, int size)
{
    int i;

    for(i = 0; i < size; i++)
        if(IS_ALPHA(buffer[i]))
            return buffer[i];

    return 0;
}

int parse_tuple(unsigned int *ret, unsigned char const *buffer, int size)
{
    int i = 0;
    int j = 0;
    int t = 0;
    unsigned char nbr[1024];

    ret[0] = 0x1337;

    for(i = 0; i < size; i++)
    {
        if(IS_ALPHA(buffer[i]))
        {
            if(j != 0)
            {
                ret[t] = atoi((char*)nbr);
                t++;
            }
            ret[t] = 0x1337;
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
            ret[t] = 0x1337;
            j = 0;
        }
    }
    return size;
}



void manage_modifiers(char c, int *fg, int *bg, int *old_fg, int *old_bg)
{
    switch(c)
    {
    case 0:
        *fg = CUCUL_COLOR_DEFAULT;
        *bg = CUCUL_COLOR_DEFAULT;
        break;
    case 1: // BOLD
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
        *old_fg = *fg;
        *old_bg = *bg;
        *fg = CUCUL_COLOR_TRANSPARENT;
        *bg = CUCUL_COLOR_TRANSPARENT;
        break;
    case 28: // not invisible
        *fg = *old_fg;
        *bg = *old_bg;
        break;
    case 30: *fg = CUCUL_COLOR_BLACK; break;
    case 31: *fg = CUCUL_COLOR_RED; break;
    case 32: *fg = CUCUL_COLOR_GREEN; break;
    case 33: *fg = CUCUL_COLOR_BROWN; break;
    case 34: *fg = CUCUL_COLOR_BLUE; break;
    case 35: *fg = CUCUL_COLOR_MAGENTA; break;
    case 36: *fg = CUCUL_COLOR_CYAN; break;
    case 37: *fg = CUCUL_COLOR_WHITE; break;
    case 39: *fg = CUCUL_COLOR_LIGHTGRAY; break;
    case 40: *bg = CUCUL_COLOR_BLACK; break;
    case 41: *bg = CUCUL_COLOR_RED; break;
    case 42: *bg = CUCUL_COLOR_GREEN; break;
    case 43: *bg = CUCUL_COLOR_BROWN; break;
    case 44: *bg = CUCUL_COLOR_BLUE; break;
    case 45: *bg = CUCUL_COLOR_MAGENTA; break;
    case 46: *bg = CUCUL_COLOR_CYAN; break;
    case 47: *bg = CUCUL_COLOR_WHITE; break;
    case 49: *bg = CUCUL_COLOR_BLACK; break;

    case 90: *fg = CUCUL_COLOR_DARKGRAY; break;
    case 91: *fg = CUCUL_COLOR_LIGHTRED; break;
    case 92: *fg = CUCUL_COLOR_LIGHTGREEN; break;
    case 93: *fg = CUCUL_COLOR_YELLOW; break;
    case 94: *fg = CUCUL_COLOR_LIGHTBLUE; break;
    case 95: *fg = CUCUL_COLOR_LIGHTMAGENTA; break;
    case 96: *fg = CUCUL_COLOR_LIGHTCYAN; break;
    case 97: *fg = CUCUL_COLOR_WHITE; break;
    case 100: *bg = CUCUL_COLOR_DARKGRAY; break;
    case 101: *bg = CUCUL_COLOR_LIGHTRED; break;
    case 102: *bg = CUCUL_COLOR_LIGHTGREEN; break;
    case 103: *bg = CUCUL_COLOR_YELLOW; break;
    case 104: *bg = CUCUL_COLOR_LIGHTBLUE; break;
    case 105: *bg = CUCUL_COLOR_LIGHTMAGENTA; break;
    case 106: *bg = CUCUL_COLOR_LIGHTCYAN; break;
    case 107: *bg = CUCUL_COLOR_WHITE; break;

    default:
        /*   printf("Unknow option to 'm' %d (%c)\n", c, IS_ALPHA(c)?c:'.'); */
        break;
    }
}

void updateCanvasSize(cucul_canvas_t *cv, int x, int y, int *max_x, int *max_y)
{
    if(x > *max_x)
        *max_x = x;

    if(y > *max_y)
        *max_y = y;

    cucul_set_canvas_size(cv, *max_x, *max_y);
}

