/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2009 Sam Hocevar <sam@hocevar.net>
 *                2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *  This file contains text import and export functions
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#   include <stdio.h>
#   include <string.h>
#endif

#include "caca.h"
#include "caca_internals.h"
#include "codec.h"

struct import
{
    uint32_t clearattr;

    /* ANSI Graphic Rendition Combination Mode */
    uint8_t fg, bg;   /* ANSI-context fg/bg */
    uint8_t dfg, dbg; /* Default fg/bg */
    uint8_t bold, blink, italics, negative, concealed, underline;
    uint8_t faint, strike, proportional; /* unsupported */
};

static void ansi_parse_grcm(caca_canvas_t *, struct import *,
                            unsigned int, unsigned int const *);

ssize_t _import_text(caca_canvas_t *cv, void const *data, size_t size)
{
    char const *text = (char const *)data;
    unsigned int width = 0, height = 0, x = 0, y = 0, i;

    caca_set_canvas_size(cv, width, height);

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

            caca_set_canvas_size(cv, width, height);
        }

        caca_put_char(cv, x, y, ch);
        x++;
    }

    if(y > height)
        caca_set_canvas_size(cv, width, height = y);

    return (ssize_t)size;
}

ssize_t _import_ansi(caca_canvas_t *cv, void const *data, size_t size, int utf8)
{
    struct import im;
    unsigned char const *buffer = (unsigned char const*)data;
    unsigned int i, j, skip, growx = 0, growy = 0, dummy = 0;
    unsigned int width, height;
    uint32_t savedattr;
    int x = 0, y = 0, save_x = 0, save_y = 0;

    if(utf8)
    {
        width = cv->width;
        height = cv->height;
        growx = !width;
        growy = !height;
        x = cv->frames[cv->frame].x;
        y = cv->frames[cv->frame].y;
    }
    else
    {
        caca_set_canvas_size(cv, width = 80, height = 0);
        growx = 0;
        growy = 1;
    }

    if(utf8)
    {
        im.dfg = CACA_DEFAULT;
        im.dbg = CACA_TRANSPARENT;
    }
    else
    {
        im.dfg = CACA_LIGHTGRAY;
        im.dbg = CACA_BLACK;
    }

    caca_set_color_ansi(cv, im.dfg, im.dbg);
    im.clearattr = caca_get_attr(cv, -1, -1);

    ansi_parse_grcm(cv, &im, 1, &dummy);

    for(i = 0; i < size; i += skip)
    {
        uint32_t ch = 0;
        int wch = 0;

        skip = 1;

        if(!utf8 && buffer[i] == '\x1a' && i + 7 < size
           && !memcmp(buffer + i + 1, "SAUCE00", 7))
            break; /* End before SAUCE data */

        else if(buffer[i] == '\r')
        {
            x = 0;
        }

        else if(buffer[i] == '\n')
        {
            x = 0;
            y++;
        }

        else if(buffer[i] == '\t')
        {
            x = (x + 7) & ~7;
        }

        else if(buffer[i] == '\x08')
        {
            if(x > 0)
                x--;
        }

        /* If there are not enough characters to parse the escape sequence,
         * wait until the next try. We require 3. */
        else if(buffer[i] == '\033' && i + 2 >= size)
            break;

        /* XXX: What the fuck is this shit? */
        else if(buffer[i] == '\033' && buffer[i + 1] == '('
                 && buffer[i + 2] == 'B')
        {
            skip += 2;
        }

        /* Interpret escape commands, as per Standard ECMA-48 "Control
         * Functions for Coded Character Sets", 5.4. Control sequences. */
        else if(buffer[i] == '\033' && buffer[i + 1] == '[')
        {
            unsigned int argc = 0, argv[101];
            unsigned int param, inter, final;

        /* Compute offsets to parameter bytes, intermediate bytes and
         * to the final byte. Only the final byte is mandatory, there
         * can be zero of the others.
         * 0  param=2             inter                 final           final+1
         * +-----+------------------+---------------------+-----------------+
         * | CSI | parameter bytes  | intermediate bytes  |   final byte    |
         * |     |   0x30 - 0x3f    |    0x20 - 0x2f      |   0x40 - 0x7e   |
         * | ^[[ | 0123456789:;<=>? | SPC !"#$%&'()*+,-./ | azAZ@[\]^_`{|}~ |
         * +-----+------------------+---------------------+-----------------+
         */
            param = 2;

            for(inter = param; i + inter < size; inter++)
                if(buffer[i + inter] < 0x30 || buffer[i + inter] > 0x3f)
                    break;

            for(final = inter; i + final < size; final++)
                if(buffer[i + final] < 0x20 || buffer[i + final] > 0x2f)
                    break;

            if(i + final >= size
                || buffer[i + final] < 0x40 || buffer[i + final] > 0x7e)
                break; /* Invalid Final Byte */

            skip += final;

            /* Sanity checks */
            if(param < inter && buffer[i + param] >= 0x3c)
            {
                /* Private sequence, only parse what we know */
                debug("ansi import: private sequence \"^[[%.*s\"",
                      final - param + 1, buffer + i + param);
                continue; /* Private sequence, skip it entirely */
            }

            if(final - param > 100)
                continue; /* Suspiciously long sequence, skip it */

            /* Parse parameter bytes as per ECMA-48 5.4.2: Parameter string
             * format */
            if(param < inter)
            {
                argv[0] = 0;
                for(j = param; j < inter; j++)
                {
                    if(buffer[i + j] == ';')
                        argv[++argc] = 0;
                    else if(buffer[i + j] >= '0' && buffer[i + j] <= '9')
                        argv[argc] = 10 * argv[argc] + (buffer[i + j] - '0');
                }
                argc++;
            }

            /* Interpret final byte. The code representations are given in
             * ECMA-48 5.4: Control sequences, and the code definitions are
             * given in ECMA-48 8.3: Definition of control functions. */
            switch(buffer[i + final])
            {
            case 'H': /* CUP (0x48) - Cursor Position */
                x = (argc > 1 && argv[1] > 0) ? argv[1] - 1 : 0;
                y = (argc > 0 && argv[0] > 0) ? argv[0] - 1 : 0;
                break;
            case 'A': /* CUU (0x41) - Cursor Up */
                y -= argc ? argv[0] : 1;
                if(y < 0)
                    y = 0;
                break;
            case 'B': /* CUD (0x42) - Cursor Down */
                y += argc ? argv[0] : 1;
                break;
            case 'C': /* CUF (0x43) - Cursor Right */
                x += argc ? argv[0] : 1;
                break;
            case 'D': /* CUB (0x44) - Cursor Left */
                x -= argc ? argv[0] : 1;
                if(x < 0)
                    x = 0;
                break;
            case 'G': /* CHA (0x47) - Cursor Character Absolute */
                x = (argc && argv[0] > 0) ? argv[0] - 1 : 0;
                break;
            case 'J': /* ED (0x4a) - Erase In Page */
                savedattr = caca_get_attr(cv, -1, -1);
                caca_set_attr(cv, im.clearattr);
                if(!argc || argv[0] == 0)
                {
                    caca_draw_line(cv, x, y, width, y, ' ');
                    caca_fill_box(cv, 0, y + 1, width - 1, height - 1, ' ');
                }
                else if(argv[0] == 1)
                {
                    caca_fill_box(cv, 0, 0, width - 1, y - 1, ' ');
                    caca_draw_line(cv, 0, y, x, y, ' ');
                }
                else if(argv[0] == 2)
                    //x = y = 0;
                    caca_fill_box(cv, 0, 0, width - 1, height - 1, ' ');
                caca_set_attr(cv, savedattr);
                break;
            case 'K': /* EL (0x4b) - Erase In Line */
                if(!argc || argv[0] == 0)
                    caca_draw_line(cv, x, y, width, y, ' ');
                else if(argv[0] == 1)
                    caca_draw_line(cv, 0, y, x, y, ' ');
                else if(argv[0] == 2)
                    if((unsigned int)x < width)
                        caca_draw_line(cv, x, y, width - 1, y, ' ');
                //x = width;
                break;
            case 'P': /* DCH (0x50) - Delete Character */
                if(!argc || argv[0] == 0)
                    argv[0] = 1; /* echo -ne 'foobar\r\e[0P\n' */
                for(j = 0; (unsigned int)(j + argv[0]) < width; j++)
                {
                    caca_put_char(cv, j, y,
                                   caca_get_char(cv, j + argv[0], y));
                    caca_put_attr(cv, j, y,
                                   caca_get_attr(cv, j + argv[0], y));
                }
#if 0
                savedattr = caca_get_attr(cv, -1, -1);
                caca_set_attr(cv, im.clearattr);
                for( ; (unsigned int)j < width; j++)
                    caca_put_char(cv, j, y, ' ');
                caca_set_attr(cv, savedattr);
#endif
            case 'X': /* ECH (0x58) - Erase Character */
                if(argc && argv[0])
                {
                    savedattr = caca_get_attr(cv, -1, -1);
                    caca_set_attr(cv, im.clearattr);
                    caca_draw_line(cv, x, y, x + argv[0] - 1, y, ' ');
                    caca_set_attr(cv, savedattr);
                }
            case 'd': /* VPA (0x64) - Line Position Absolute */
                y = (argc && argv[0] > 0) ? argv[0] - 1 : 0;
                break;
            case 'f': /* HVP (0x66) - Character And Line Position */
                x = (argc > 1 && argv[1] > 0) ? argv[1] - 1 : 0;
                y = (argc > 0 && argv[0] > 0) ? argv[0] - 1 : 0;
                break;
            case 'h': /* SM (0x68) - FIXME */
                debug("ansi import: set mode %i", argc ? (int)argv[0] : -1);
                break;
            case 'l': /* RM (0x6c) - FIXME */
                debug("ansi import: reset mode %i", argc ? (int)argv[0] : -1);
                break;
            case 'm': /* SGR (0x6d) - Select Graphic Rendition */
                if(argc)
                    ansi_parse_grcm(cv, &im, argc, argv);
                else
                    ansi_parse_grcm(cv, &im, 1, &dummy);
                break;
            case 's': /* Private (save cursor position) */
                save_x = x;
                save_y = y;
                break;
            case 'u': /* Private (reload cursor position) */
                x = save_x;
                y = save_y;
                break;
            default:
                debug("ansi import: unknown command \"^[[%.*s\"",
                      final - param + 1, buffer + i + param);
                break;
            }
        }

        /* Parse OSC stuff. */
        else if(buffer[i] == '\033' && buffer[i + 1] == ']')
        {
            char *string;
            unsigned int command = 0;
            unsigned int mode = 2, semicolon, final;

            for(semicolon = mode; i + semicolon < size; semicolon++)
            {
                if(buffer[i + semicolon] < '0' || buffer[i + semicolon] > '9')
                    break;
                command = 10 * command + (buffer[i + semicolon] - '0');
            }

            if(i + semicolon >= size || buffer[i + semicolon] != ';')
                break; /* Invalid Mode */

            for(final = semicolon + 1; i + final < size; final++)
                if(buffer[i + final] < 0x20)
                    break;

            if(i + final >= size || buffer[i + final] != '\a')
                break; /* Not enough data or no bell found */
                /* FIXME: XTerm also reacts to <ESC><backslash> and <ST> */
                /* FIXME: differenciate between not enough data (try again)
                 *        and invalid data (print shit) */

            skip += final;

            string = malloc(final - (semicolon + 1) + 1);
            memcpy(string, buffer + (semicolon + 1), final - (semicolon + 1));
            string[final - (semicolon + 1)] = '\0';
            debug("ansi import: got OSC command %i string '%s'", command,
                  string);
            free(string);
        }

        /* Form feed means a new frame */
        else if(buffer[i] == '\f' && buffer[i + 1] == '\n')
        {
            int f = caca_get_frame_count(cv);
            caca_create_frame(cv, f);
            caca_set_frame(cv, f);
            x = y = 0;
            skip++;
        }

        /* Get the character we’re going to paste */
        else if(utf8)
        {
            size_t bytes;

            if(i + 6 < size)
                ch = caca_utf8_to_utf32((char const *)(buffer + i), &bytes);
            else
            {
                /* Add a trailing zero to what we're going to read */
                char tmp[7];
                memcpy(tmp, buffer + i, size - i);
                tmp[size - i] = '\0';
                ch = caca_utf8_to_utf32(tmp, &bytes);
            }

            if(!bytes)
            {
                /* If the Unicode is invalid, assume it was latin1. */
                ch = buffer[i];
                bytes = 1;
            }
            wch = caca_utf32_is_fullwidth(ch) ? 2 : 1;
            skip += (int)(bytes - 1);
        }
        else
        {
            ch = caca_cp437_to_utf32(buffer[i]);
            wch = 1;
        }

        /* Wrap long lines or grow horizontally */
        while((unsigned int)x + wch > width)
        {
            if(growx)
            {
                savedattr = caca_get_attr(cv, -1, -1);
                caca_set_attr(cv, im.clearattr);
                caca_set_canvas_size(cv, width = x + wch, height);
                caca_set_attr(cv, savedattr);
            }
            else
            {
                x -= width;
                y++;
            }
        }

        /* Scroll or grow vertically */
        if((unsigned int)y >= height)
        {
            savedattr = caca_get_attr(cv, -1, -1);
            caca_set_attr(cv, im.clearattr);
            if(growy)
            {
                caca_set_canvas_size(cv, width, height = y + 1);
            }
            else
            {
                int lines = (y - height) + 1;

                for(j = 0; j + lines < height; j++)
                {
                    memcpy(cv->attrs + j * cv->width,
                           cv->attrs + (j + lines) * cv->width, cv->width * 4);
                    memcpy(cv->chars + j * cv->width,
                           cv->chars + (j + lines) * cv->width, cv->width * 4);
                }
                caca_fill_box(cv, 0, height - lines,
                                   cv->width - 1, height - 1, ' ');
                y -= lines;
            }
            caca_set_attr(cv, savedattr);
        }

        /* Now paste our character, if any */
        if(wch)
        {
            caca_put_char(cv, x, y, ch);
            x += wch;
        }
    }

    if(growy && (unsigned int)y > height)
    {
        savedattr = caca_get_attr(cv, -1, -1);
        caca_set_attr(cv, im.clearattr);
        caca_set_canvas_size(cv, width, height = y);
        caca_set_attr(cv, savedattr);
    }

    cv->frames[cv->frame].x = x;
    cv->frames[cv->frame].y = y;

//    if(utf8)
//        caca_set_attr(cv, savedattr);

    return i;
}

/* Generate UTF-8 representation of current canvas. */
void *_export_utf8(caca_canvas_t const *cv, size_t *bytes, int cr)
{
    static uint8_t const palette[] =
    {
        0,  4,  2,  6, 1,  5,  3,  7,
        8, 12, 10, 14, 9, 13, 11, 15
    };

    char *data, *cur;
    int x, y;

    /* 23 bytes assumed for max length per pixel ('\e[5;1;3x;4y;9x;10ym' plus
     * 4 max bytes for a UTF-8 character).
     * Add height*9 to that (zeroes color at the end and jump to next line) */
    *bytes = (cv->height * 9) + (cv->width * cv->height * 23);
    cur = data = malloc(*bytes);

    for(y = 0; y < cv->height; y++)
    {
        uint32_t *lineattr = cv->attrs + y * cv->width;
        uint32_t *linechar = cv->chars + y * cv->width;

        uint8_t prevfg = 0x10;
        uint8_t prevbg = 0x10;

        for(x = 0; x < cv->width; x++)
        {
            uint32_t attr = lineattr[x];
            uint32_t ch = linechar[x];
            uint8_t ansifg, ansibg, fg, bg;

            if(ch == CACA_MAGIC_FULLWIDTH)
                continue;

            ansifg = caca_attr_to_ansi_fg(attr);
            ansibg = caca_attr_to_ansi_bg(attr);

            fg = ansifg < 0x10 ? palette[ansifg] : 0x10;
            bg = ansibg < 0x10 ? palette[ansibg] : 0x10;

            /* TODO: the [0 could be omitted in some cases */
            if(fg != prevfg || bg != prevbg)
            {
                cur += sprintf(cur, "\033[0");

                if(fg < 8)
                    cur += sprintf(cur, ";3%d", fg);
                else if(fg < 16)
                    cur += sprintf(cur, ";1;3%d;9%d", fg - 8, fg - 8);

                if(bg < 8)
                    cur += sprintf(cur, ";4%d", bg);
                else if(bg < 16)
                    cur += sprintf(cur, ";5;4%d;10%d", bg - 8, bg - 8);

                cur += sprintf(cur, "m");
            }

            cur += caca_utf32_to_utf8(cur, ch);

            prevfg = fg;
            prevbg = bg;
        }

        if(prevfg != 0x10 || prevbg != 0x10)
            cur += sprintf(cur, "\033[0m");

        cur += sprintf(cur, cr ? "\r\n" : "\n");
    }

    /* Crop to really used size */
    debug("utf8 export: alloc %lu bytes, realloc %lu",
          (unsigned long int)*bytes, (unsigned long int)(cur - data));
    *bytes = (uintptr_t)(cur - data);
    data = realloc(data, *bytes);

    return data;
}

/* Generate ANSI representation of current canvas. */
void *_export_ansi(caca_canvas_t const *cv, size_t *bytes)
{
    static uint8_t const palette[] =
    {
        0,  4,  2,  6, 1,  5,  3,  7,
        8, 12, 10, 14, 9, 13, 11, 15
    };

    char *data, *cur;
    int x, y;

    uint8_t prevfg = -1;
    uint8_t prevbg = -1;

    /* 16 bytes assumed for max length per pixel ('\e[5;1;3x;4ym' plus
     * 1 byte for a CP437 character).
     * Add height*9 to that (zeroes color at the end and jump to next line) */
    *bytes = (cv->height * 9) + (cv->width * cv->height * 16);
    cur = data = malloc(*bytes);

    for(y = 0; y < cv->height; y++)
    {
        uint32_t *lineattr = cv->attrs + y * cv->width;
        uint32_t *linechar = cv->chars + y * cv->width;

        for(x = 0; x < cv->width; x++)
        {
            uint8_t ansifg = caca_attr_to_ansi_fg(lineattr[x]);
            uint8_t ansibg = caca_attr_to_ansi_bg(lineattr[x]);
            uint8_t fg = ansifg < 0x10 ? palette[ansifg] : CACA_LIGHTGRAY;
            uint8_t bg = ansibg < 0x10 ? palette[ansibg] : CACA_BLACK;
            uint32_t ch = linechar[x];

            if(ch == CACA_MAGIC_FULLWIDTH)
                ch = '?';

            if(fg != prevfg || bg != prevbg)
            {
                cur += sprintf(cur, "\033[0;");

                if(fg < 8)
                    if(bg < 8)
                        cur += sprintf(cur, "3%d;4%dm", fg, bg);
                    else
                        cur += sprintf(cur, "5;3%d;4%dm", fg, bg - 8);
                else
                    if(bg < 8)
                        cur += sprintf(cur, "1;3%d;4%dm", fg - 8, bg);
                    else
                        cur += sprintf(cur, "5;1;3%d;4%dm", fg - 8, bg - 8);
            }

            *cur++ = caca_utf32_to_cp437(ch);

            prevfg = fg;
            prevbg = bg;
        }

        if(cv->width == 80)
        {
            cur += sprintf(cur, "\033[s\n\033[u");
        }
        else
        {
            cur += sprintf(cur, "\033[0m\r\n");
            prevfg = -1;
            prevbg = -1;
        }
    }

    /* Crop to really used size */
    debug("ansi export: alloc %lu bytes, realloc %lu",
          (unsigned long int)*bytes, (unsigned long int)(cur - data));
    *bytes = (uintptr_t)(cur - data);
    data = realloc(data, *bytes);

    return data;
}

/* Export a text file with IRC colours */
void *_export_irc(caca_canvas_t const *cv, size_t *bytes)
{
    static uint8_t const palette[] =
    {
        1, 2, 3, 10, 5, 6, 7, 15, /* Dark */
        14, 12, 9, 11, 4, 13, 8, 0, /* Light */
    };

    char *data, *cur;
    int x, y;

    /* 14 bytes assumed for max length per pixel. Worst case scenario:
     * ^Cxx,yy   6 bytes
     * ^B^B      2 bytes
     * ch        6 bytes
     * 3 bytes for max length per line. Worst case scenario:
     * <spc>     1 byte (for empty lines)
     * \r\n      2 bytes
     * In real life, the average bytes per pixel value will be around 5.
     */

    *bytes = 2 + cv->height * (3 + cv->width * 14);
    cur = data = malloc(*bytes);

    for(y = 0; y < cv->height; y++)
    {
        uint32_t *lineattr = cv->attrs + y * cv->width;
        uint32_t *linechar = cv->chars + y * cv->width;

        uint8_t prevfg = 0x10;
        uint8_t prevbg = 0x10;

        for(x = 0; x < cv->width; x++)
        {
            uint32_t attr = lineattr[x];
            uint32_t ch = linechar[x];
            uint8_t ansifg, ansibg, fg, bg;

            if(ch == CACA_MAGIC_FULLWIDTH)
                continue;

            ansifg = caca_attr_to_ansi_fg(attr);
            ansibg = caca_attr_to_ansi_bg(attr);

            fg = ansifg < 0x10 ? palette[ansifg] : 0x10;
            bg = ansibg < 0x10 ? palette[ansibg] : 0x10;

            /* TODO: optimise series of same fg / same bg
             *       don't change fg value if ch == ' '
             *       make sure the \x03,%d trick works everywhere */
            if(bg != prevbg || fg != prevfg)
            {
                int need_escape = 0;

                if(bg == 0x10)
                {
                    if(fg == 0x10)
                        cur += sprintf(cur, "\x0f");
                    else
                    {
                        if(prevbg == 0x10)
                            cur += sprintf(cur, "\x03%d", fg);
                        else
                            cur += sprintf(cur, "\x0f\x03%d", fg);

                        if(ch == (uint32_t)',')
                            need_escape = 1;
                    }
                }
                else
                {
                    if(fg == 0x10)
                        cur += sprintf(cur, "\x0f\x03,%d", bg);
                    else
                        cur += sprintf(cur, "\x03%d,%d", fg, bg);
                }

                if(ch >= (uint32_t)'0' && ch <= (uint32_t)'9')
                    need_escape = 1;

                if(need_escape)
                    cur += sprintf(cur, "\x02\x02");
            }

            cur += caca_utf32_to_utf8(cur, ch);
            prevfg = fg;
            prevbg = bg;
        }

        /* TODO: do the same the day we optimise whole lines above */
        if(!cv->width)
            *cur++ = ' ';

        *cur++ = '\r';
        *cur++ = '\n';
    }

    /* Crop to really used size */
    debug("IRC export: alloc %lu bytes, realloc %lu",
          (unsigned long int)*bytes, (unsigned long int)(cur - data));
    *bytes = (uintptr_t)(cur - data);
    data = realloc(data, *bytes);

    return data;
}

/* XXX : ANSI loader helper */

static void ansi_parse_grcm(caca_canvas_t *cv, struct import *im,
                            unsigned int argc, unsigned int const *argv)
{
    static uint8_t const ansi2caca[] =
    {
        CACA_BLACK, CACA_RED, CACA_GREEN, CACA_BROWN,
        CACA_BLUE, CACA_MAGENTA, CACA_CYAN, CACA_LIGHTGRAY
    };

    unsigned int j;
    uint8_t efg, ebg; /* Effective (libcaca) fg/bg */

    for(j = 0; j < argc; j++)
    {
        /* Defined in ECMA-48 8.3.117: SGR - SELECT GRAPHIC RENDITION */
        if(argv[j] >= 30 && argv[j] <= 37)
            im->fg = ansi2caca[argv[j] - 30];
        else if(argv[j] >= 40 && argv[j] <= 47)
            im->bg = ansi2caca[argv[j] - 40];
        else if(argv[j] >= 90 && argv[j] <= 97)
            im->fg = ansi2caca[argv[j] - 90] + 8;
        else if(argv[j] >= 100 && argv[j] <= 107)
            im->bg = ansi2caca[argv[j] - 100] + 8;
        else switch(argv[j])
        {
        case 0: /* default rendition */
            im->fg = im->dfg;
            im->bg = im->dbg;
            im->bold = im->blink = im->italics = im->negative
             = im->concealed = im->underline = im->faint = im->strike
             = im->proportional = 0;
            break;
        case 1: /* bold or increased intensity */
            im->bold = 1;
            break;
        case 2: /* faint, decreased intensity or second colour */
            im->faint = 1;
            break;
        case 3: /* italicized */
            im->italics = 1;
            break;
        case 4: /* singly underlined */
            im->underline = 1;
            break;
        case 5: /* slowly blinking (less then 150 per minute) */
        case 6: /* rapidly blinking (150 per minute or more) */
            im->blink = 1;
            break;
        case 7: /* negative image */
            im->negative = 1;
            break;
        case 8: /* concealed characters */
            im->concealed = 1;
            break;
        case 9: /* crossed-out (characters still legible but marked as to be
                 * deleted */
            im->strike = 1;
            break;
        case 21: /* doubly underlined */
            im->underline = 1;
            break;
        case 22: /* normal colour or normal intensity (neither bold nor
                  * faint) */
            im->bold = im->faint = 0;
            break;
        case 23: /* not italicized, not fraktur */
            im->italics = 0;
            break;
        case 24: /* not underlined (neither singly nor doubly) */
            im->underline = 0;
            break;
        case 25: /* steady (not blinking) */
            im->blink = 0;
            break;
        case 26: /* (reserved for proportional spacing as specified in CCITT
                  * Recommendation T.61) */
            im->proportional = 1;
            break;
        case 27: /* positive image */
            im->negative = 0;
            break;
        case 28: /* revealed characters */
            im->concealed = 0;
            break;
        case 29: /* not crossed out */
            im->strike = 0;
            break;
        case 38: /* (reserved for future standardization, intended for setting
                  * character foreground colour as specified in ISO 8613-6
                  * [CCITT Recommendation T.416]) */
            break;
        case 39: /* default display colour (implementation-defined) */
            im->fg = im->dfg;
            break;
        case 48: /* (reserved for future standardization, intended for setting
                  * character background colour as specified in ISO 8613-6
                  * [CCITT Recommendation T.416]) */
            break;
        case 49: /* default background colour (implementation-defined) */
            im->bg = im->dbg;
            break;
        case 50: /* (reserved for cancelling the effect of the rendering
                  * aspect established by parameter value 26) */
            im->proportional = 0;
            break;
        default:
            debug("ansi import: unknown sgr %i", argv[j]);
            break;
        }
    }

    if(im->concealed)
    {
        efg = ebg = CACA_TRANSPARENT;
    }
    else
    {
        efg = im->negative ? im->bg : im->fg;
        ebg = im->negative ? im->fg : im->bg;

        if(im->bold)
        {
            if(efg < 8)
                efg += 8;
            else if(efg == CACA_DEFAULT)
                efg = CACA_WHITE;
        }
    }

    caca_set_color_ansi(cv, efg, ebg);
}

