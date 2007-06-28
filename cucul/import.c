/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
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
 *  This file contains various import functions.
 */

#include "config.h"
#include "common.h"

#if !defined __KERNEL__
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

static inline uint32_t sscanu32(void const *s)
{
    uint32_t x;
    memcpy(&x, s, 4);
    return hton32(x);
}

static inline uint16_t sscanu16(void const *s)
{
    uint16_t x;
    memcpy(&x, s, 2);
    return hton16(x);
}

struct import
{
    uint32_t clearattr;

    /* ANSI Graphic Rendition Combination Mode */
    uint8_t fg, bg;   /* ANSI-context fg/bg */
    uint8_t dfg, dbg; /* Default fg/bg */
    uint8_t bold, blink, italics, negative, concealed, underline;
    uint8_t faint, strike, proportional; /* unsupported */
};

static long int import_caca(cucul_canvas_t *, void const *, unsigned int);
static long int import_text(cucul_canvas_t *, void const *, unsigned int);
static long int import_ansi(cucul_canvas_t *, void const *, unsigned int, int);

static void ansi_parse_grcm(cucul_canvas_t *, struct import *,
                            unsigned int, unsigned int const *);

/** \brief Import a memory buffer into a canvas
 *
 *  Import a memory buffer into the given libcucul canvas's current
 *  frame. The current frame is resized accordingly and its contents are
 *  replaced with the imported data.
 *
 *  Valid values for \c format are:
 *  - \c "": attempt to autodetect the file format.
 *  - \c "caca": import native libcaca files.
 *  - \c "text": import ASCII text files.
 *  - \c "ansi": import ANSI files.
 *  - \c "utf8": import UTF-8 files with ANSI colour codes.
 *
 *  The number of bytes read is returned. If the file format is valid, but
 *  not enough data was available, 0 is returned.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c ENOMEM Not enough memory to allocate canvas.
 *  - \c EINVAL Invalid format requested.
 *
 *  \param cv A libcucul canvas in which to import the file.
 *  \param data A memory area containing the data to be loaded into the canvas.
 *  \param len The size in bytes of the memory area.
 *  \param format A string describing the input format.
 *  \return The number of bytes read, or 0 if there was not enough data,
 *  or -1 if an error occurred.
 */
long int cucul_import_memory(cucul_canvas_t *cv, void const *data,
                             unsigned long int len, char const *format)
{
    if(!strcasecmp("caca", format))
        return import_caca(cv, data, len);
    if(!strcasecmp("utf8", format))
        return import_ansi(cv, data, len, 1);
    if(!strcasecmp("text", format))
        return import_text(cv, data, len);
    if(!strcasecmp("ansi", format))
        return import_ansi(cv, data, len, 0);

    /* Autodetection */
    if(!strcasecmp("", format))
    {
        unsigned char const *str = data;
        unsigned int i;

        /* If 4 first bytes are 0xcaca + 'CV' */
        if(len >= 4 && str[0] == 0xca &&
           str[1] == 0xca && str[2] == 'C' && str[3] == 'V')
            return import_caca(cv, data, len);

        /* If we find ESC[ argv, we guess it's an ANSI file */
        for(i = 0; i + 1 < len; i++)
            if((str[i] == 0x1b) && (str[i + 1] == '['))
                return import_ansi(cv, data, len, 0);

        /* Otherwise, import it as text */
        return import_text(cv, data, len);
    }

    seterrno(EINVAL);
    return -1;
}

/** \brief Import a file into a canvas
 *
 *  Import a file into the given libcucul canvas's current frame. The
 *  current frame is resized accordingly and its contents are replaced
 *  with the imported data.
 *
 *  Valid values for \c format are:
 *  - \c "": attempt to autodetect the file format.
 *  - \c "caca": import native libcaca files.
 *  - \c "text": import ASCII text files.
 *  - \c "ansi": import ANSI files.
 *  - \c "utf8": import UTF-8 files with ANSI colour codes.
 *
 *  The number of bytes read is returned. If the file format is valid, but
 *  not enough data was available, 0 is returned.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c ENOSYS File access is not implemented on this system.
 *  - \c ENOMEM Not enough memory to allocate canvas.
 *  - \c EINVAL Invalid format requested.
 *  cucul_import_file() may also fail and set \b errno for any of the
 *  errors specified for the routine fopen().
 *
 *  \param cv A libcucul canvas in which to import the file.
 *  \param filename The name of the file to load.
 *  \param format A string describing the input format.
 *  \return The number of bytes read, or 0 if there was not enough data,
 *  or -1 if an error occurred.
 */
long int cucul_import_file(cucul_canvas_t *cv, char const *filename,
                           char const *format)
{
#if defined __KERNEL__
    seterrno(ENOSYS);
    return -1;
#else
    FILE *fp;
    void *data;
    long int size;
    int ret;

    fp = fopen(filename, "rb");
    if(!fp)
        return -1; /* fopen already set errno */

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);

    data = malloc(size);
    if(!data)
    {
        fclose(fp);
        seterrno(ENOMEM);
        return -1;
    }

    fseek(fp, 0, SEEK_SET);
    fread(data, size, 1, fp);
    fclose(fp);

    ret = cucul_import_memory(cv, data, size, format);
    free(data);

    return ret;
#endif
}
    
/** \brief Get available import formats
 *
 *  Return a list of available import formats. The list is a NULL-terminated
 *  array of strings, interleaving a string containing the internal value for
 *  the import format, to be used with cucul_import_canvas(), and a string
 *  containing the natural language description for that import format.
 *
 *  This function never fails.
 *
 *  \return An array of strings.
 */
char const * const * cucul_get_import_list(void)
{
    static char const * const list[] =
    {
        "", "autodetect",
        "caca", "native libcaca format",
        "text", "plain text",
        "ansi", "ANSI coloured text",
        "utf8", "UTF-8 files with ANSI colour codes",
        NULL, NULL
    };

    return list;
}

/*
 * XXX: the following functions are local.
 */

static long int import_caca(cucul_canvas_t *cv,
                            void const *data, unsigned int size)
{
    uint8_t const *buf = (uint8_t const *)data;
    unsigned int control_size, data_size, expected_size, frames, f, n;
    uint16_t version, flags;
    unsigned int offset;
    int32_t xmin = 0, ymin = 0, xmax = 0, ymax = 0;

    if(size < 20)
        return 0;

    if(buf[0] != 0xca || buf[1] != 0xca || buf[2] != 'C' || buf[3] != 'V')
    {
        debug("caca import error: expected \\xca\\xcaCV header");
        goto invalid_caca;
    }

    control_size = sscanu32(buf + 4);
    data_size = sscanu32(buf + 8);
    version = sscanu16(buf + 12);
    frames = sscanu32(buf + 14);
    flags = sscanu16(buf + 18);

    if(size < 4 + control_size + data_size)
        return 0;

    if(control_size < 16 + frames * 32)
    {
        debug("caca import error: control size %lu < expected %lu",
              (unsigned long int)control_size, 16 + frames * 32);
        goto invalid_caca;
    }

    for(expected_size = 0, f = 0; f < frames; f++)
    {
        unsigned int width, height, duration;
        uint32_t attr;
        int x, y, handlex, handley;

        width = sscanu32(buf + 4 + 16 + f * 32);
        height = sscanu32(buf + 4 + 16 + f * 32 + 4);
        duration = sscanu32(buf + 4 + 16 + f * 32 + 8);
        attr = sscanu32(buf + 4 + 16 + f * 32 + 12);
        x = (int32_t)sscanu32(buf + 4 + 16 + f * 32 + 16);
        y = (int32_t)sscanu32(buf + 4 + 16 + f * 32 + 20);
        handlex = (int32_t)sscanu32(buf + 4 + 16 + f * 32 + 24);
        handley = (int32_t)sscanu32(buf + 4 + 16 + f * 32 + 28);
        expected_size += width * height * 8;
        if(-handlex < xmin)
            xmin = -handlex;
        if(-handley < ymin)
            ymin = -handley;
        if((((int32_t) width) - handlex) > xmax)
            xmax = ((int32_t) width) - handlex;
        if((((int32_t) height) - handley) > ymax)
            ymax = ((int32_t) height) - handley;
    }

    if(expected_size != data_size)
    {
        debug("caca import error: data size %lu < expected %lu",
              (unsigned long int)data_size, (unsigned long int)expected_size);
        goto invalid_caca;
    }

    cucul_set_canvas_size(cv, 0, 0);
    cucul_set_canvas_size(cv, xmax - xmin, ymax - ymin);

    for (f = cucul_get_frame_count(cv); f--; )
    {
        cucul_free_frame(cv, f);
    }

    for (offset = 0, f = 0; f < frames; f ++)
    {
        unsigned int width, height;

        width = sscanu32(buf + 4 + 16 + f * 32);
        height = sscanu32(buf + 4 + 16 + f * 32 + 4);
        cucul_create_frame(cv, f);
        cucul_set_frame(cv, f);

        cv->curattr = sscanu32(buf + 4 + 16 + f * 32 + 12);
        cv->frames[f].x = (int32_t)sscanu32(buf + 4 + 16 + f * 32 + 16);
        cv->frames[f].y = (int32_t)sscanu32(buf + 4 + 16 + f * 32 + 20);
        cv->frames[f].handlex = (int32_t)sscanu32(buf + 4 + 16 + f * 32 + 24);
        cv->frames[f].handley = (int32_t)sscanu32(buf + 4 + 16 + f * 32 + 28);

        /* FIXME: check for return value */

        for(n = width * height; n--; )
        {
            int x = (n % width) - cv->frames[f].handlex - xmin;
            int y = (n / width) - cv->frames[f].handley - ymin;

            cucul_put_char(cv, x, y, sscanu32(buf + 4 + control_size
                                               + offset + 8 * n));
            cucul_put_attr(cv, x, y, sscanu32(buf + 4 + control_size
                                               + offset + 8 * n + 4));
        }
        offset += width * height * 8;

        cv->frames[f].x -= cv->frames[f].handlex;
        cv->frames[f].y -= cv->frames[f].handley;
        cv->frames[f].handlex = -xmin;
        cv->frames[f].handley = -ymin;
    }

    cucul_set_frame(cv, 0);

    return 4 + control_size + data_size;

invalid_caca:
    seterrno(EINVAL);
    return -1;
}

static long int import_text(cucul_canvas_t *cv,
                            void const *data, unsigned int size)
{
    char const *text = (char const *)data;
    unsigned int width = 0, height = 0, x = 0, y = 0, i;

    cucul_set_canvas_size(cv, width, height);

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

        cucul_put_char(cv, x, y, ch);
        x++;
    }

    if(y > height)
        cucul_set_canvas_size(cv, width, height = y);

    return size;
}

static long int import_ansi(cucul_canvas_t *cv,
                            void const *data, unsigned int size, int utf8)
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
        cucul_set_canvas_size(cv, width = 80, height = 0);
        growx = 0;
        growy = 1;
    }

    if(utf8)
    {
        im.dfg = CUCUL_DEFAULT;
        im.dbg = CUCUL_TRANSPARENT;
    }
    else
    {
        im.dfg = CUCUL_LIGHTGRAY;
        im.dbg = CUCUL_BLACK;
    }

    cucul_set_color_ansi(cv, im.dfg, im.dbg);
    im.clearattr = cucul_get_attr(cv, -1, -1);

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
        else if(buffer[i] == '\x1b' && i + 2 >= size)
            break;

        /* XXX: What the fuck is this shit? */
        else if(buffer[i] == '\x1b' && buffer[i + 1] == '('
                 && buffer[i + 2] == 'B')
        {
            skip += 2;
        }

        /* Interpret escape commands, as per Standard ECMA-48 "Control
         * Functions for Coded Character Sets", 5.4. Control sequences. */
        else if(buffer[i] == '\x1b' && buffer[i + 1] == '[')
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
                savedattr = cucul_get_attr(cv, -1, -1);
                cucul_set_attr(cv, im.clearattr);
                if(!argc || argv[0] == 0)
                {
                    cucul_draw_line(cv, x, y, width, y, ' ');
                    cucul_fill_box(cv, 0, y + 1, width - 1, height - 1, ' ');
                }
                else if(argv[0] == 1)
                {
                    cucul_fill_box(cv, 0, 0, width - 1, y - 1, ' ');
                    cucul_draw_line(cv, 0, y, x, y, ' ');
                }
                else if(argv[0] == 2)
                    //x = y = 0;
                    cucul_fill_box(cv, 0, 0, width - 1, height - 1, ' ');
                cucul_set_attr(cv, savedattr);
                break;
            case 'K': /* EL (0x4b) - Erase In Line */
                if(!argc || argv[0] == 0)
                    cucul_draw_line(cv, x, y, width, y, ' ');
                else if(argv[0] == 1)
                    cucul_draw_line(cv, 0, y, x, y, ' ');
                else if(argv[0] == 2)
                    if((unsigned int)x < width)
                        cucul_draw_line(cv, x, y, width - 1, y, ' ');
                //x = width;
                break;
            case 'P': /* DCH (0x50) - Delete Character */
                if(!argc || argv[0] == 0)
                    argv[0] = 1; /* echo -ne 'foobar\r\e[0P\n' */
                for(j = 0; (unsigned int)(j + argv[0]) < width; j++)
                {
                    cucul_put_char(cv, j, y,
                                   cucul_get_char(cv, j + argv[0], y));
                    cucul_put_attr(cv, j, y,
                                   cucul_get_attr(cv, j + argv[0], y));
                }
#if 0
                savedattr = cucul_get_attr(cv, -1, -1);
                cucul_set_attr(cv, im.clearattr);
                for( ; (unsigned int)j < width; j++)
                    cucul_put_char(cv, j, y, ' ');
                cucul_set_attr(cv, savedattr);
#endif
            case 'X': /* ECH (0x58) - Erase Character */
                if(argc && argv[0])
                {
                    savedattr = cucul_get_attr(cv, -1, -1);
                    cucul_set_attr(cv, im.clearattr);
                    cucul_draw_line(cv, x, y, x + argv[0] - 1, y, ' ');
                    cucul_set_attr(cv, savedattr);
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
        else if(buffer[i] == '\x1b' && buffer[i + 1] == ']')
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

        /* Get the character we’re going to paste */
        else if(utf8)
        {
            unsigned int bytes;

            if(i + 6 < size)
                ch = cucul_utf8_to_utf32((char const *)(buffer + i), &bytes);
            else
            {
                /* Add a trailing zero to what we're going to read */
                char tmp[7];
                memcpy(tmp, buffer + i, size - i);
                tmp[size - i] = '\0';
                ch = cucul_utf8_to_utf32(tmp, &bytes);
            }

            if(!bytes)
            {
                /* If the Unicode is invalid, assume it was latin1. */
                ch = buffer[i];
                bytes = 1;
            }
            wch = cucul_utf32_is_fullwidth(ch) ? 2 : 1;
            skip += bytes - 1;
        }
        else
        {
            ch = cucul_cp437_to_utf32(buffer[i]);
            wch = 1;
        }

        /* Wrap long lines or grow horizontally */
        while((unsigned int)x + wch > width)
        {
            if(growx)
            {
                savedattr = cucul_get_attr(cv, -1, -1);
                cucul_set_attr(cv, im.clearattr);
                cucul_set_canvas_size(cv, width = x + wch, height);
                cucul_set_attr(cv, savedattr);
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
            savedattr = cucul_get_attr(cv, -1, -1);
            cucul_set_attr(cv, im.clearattr);
            if(growy)
            {
                cucul_set_canvas_size(cv, width, height = y + 1);
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
                cucul_fill_box(cv, 0, height - lines,
                                   cv->width - 1, height - 1, ' ');
                y -= lines;
            }
            cucul_set_attr(cv, savedattr);
        }

        /* Now paste our character, if any */
        if(wch)
        {
            cucul_put_char(cv, x, y, ch);
            x += wch;
        }
    }

    if(growy && (unsigned int)y > height)
    {
        savedattr = cucul_get_attr(cv, -1, -1);
        cucul_set_attr(cv, im.clearattr);
        cucul_set_canvas_size(cv, width, height = y);
        cucul_set_attr(cv, savedattr);
    }

    cv->frames[cv->frame].x = x;
    cv->frames[cv->frame].y = y;

//    if(utf8)
//        cucul_set_attr(cv, savedattr);

    return i;
}

/* XXX : ANSI loader helper */

static void ansi_parse_grcm(cucul_canvas_t *cv, struct import *im,
                            unsigned int argc, unsigned int const *argv)
{
    static uint8_t const ansi2cucul[] =
    {
        CUCUL_BLACK, CUCUL_RED, CUCUL_GREEN, CUCUL_BROWN,
        CUCUL_BLUE, CUCUL_MAGENTA, CUCUL_CYAN, CUCUL_LIGHTGRAY
    };

    unsigned int j;
    uint8_t efg, ebg; /* Effective (libcucul) fg/bg */

    for(j = 0; j < argc; j++)
    {
        /* Defined in ECMA-48 8.3.117: SGR - SELECT GRAPHIC RENDITION */
        if(argv[j] >= 30 && argv[j] <= 37)
            im->fg = ansi2cucul[argv[j] - 30];
        else if(argv[j] >= 40 && argv[j] <= 47)
            im->bg = ansi2cucul[argv[j] - 40];
        else if(argv[j] >= 90 && argv[j] <= 97)
            im->fg = ansi2cucul[argv[j] - 90] + 8;
        else if(argv[j] >= 100 && argv[j] <= 107)
            im->bg = ansi2cucul[argv[j] - 100] + 8;
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
        efg = ebg = CUCUL_TRANSPARENT;
    }
    else
    {
        efg = im->negative ? im->bg : im->fg;
        ebg = im->negative ? im->fg : im->bg;

        if(im->bold)
        {
            if(efg < 8)
                efg += 8;
            else if(efg == CUCUL_DEFAULT)
                efg = CUCUL_WHITE;
        }
    }

    cucul_set_color_ansi(cv, efg, ebg);
}

