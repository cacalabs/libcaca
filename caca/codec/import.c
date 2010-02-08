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
 *  This file contains various import functions.
 */

#include "config.h"

#if !defined __KERNEL__
#   include <stdlib.h>
#   include <string.h>
#   include <stdio.h>
#endif

#include "caca.h"
#include "caca_internals.h"
#include "codec.h"

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

static ssize_t import_caca(caca_canvas_t *, void const *, size_t);

/** \brief Import a memory buffer into a canvas
 *
 *  Import a memory buffer into the given libcaca canvas's current
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
 *  \param cv A libcaca canvas in which to import the file.
 *  \param data A memory area containing the data to be loaded into the canvas.
 *  \param len The size in bytes of the memory area.
 *  \param format A string describing the input format.
 *  \return The number of bytes read, or 0 if there was not enough data,
 *  or -1 if an error occurred.
 */
ssize_t caca_import_canvas_from_memory(caca_canvas_t *cv, void const *data,
                                       size_t len, char const *format)
{
    if(!strcasecmp("caca", format))
        return import_caca(cv, data, len);
    if(!strcasecmp("utf8", format))
        return _import_ansi(cv, data, len, 1);
    if(!strcasecmp("text", format))
        return _import_text(cv, data, len);
    if(!strcasecmp("ansi", format))
        return _import_ansi(cv, data, len, 0);

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
            if((str[i] == '\033') && (str[i + 1] == '['))
                return _import_ansi(cv, data, len, 0);

        /* Otherwise, import it as text */
        return _import_text(cv, data, len);
    }

    seterrno(EINVAL);
    return -1;
}

/** \brief Import a file into a canvas
 *
 *  Import a file into the given libcaca canvas's current frame. The
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
 *  caca_import_file() may also fail and set \b errno for any of the
 *  errors specified for the routine fopen().
 *
 *  \param cv A libcaca canvas in which to import the file.
 *  \param filename The name of the file to load.
 *  \param format A string describing the input format.
 *  \return The number of bytes read, or 0 if there was not enough data,
 *  or -1 if an error occurred.
 */
ssize_t caca_import_canvas_from_file(caca_canvas_t *cv, char const *filename,
                                     char const *format)
{
#if defined __KERNEL__
    seterrno(ENOSYS);
    return -1;
#else
    caca_file_t *f;
    char *data = NULL;
    ssize_t ret, size = 0;

    f = caca_file_open(filename, "rb");
    if(!f)
        return -1; /* fopen already set errno */

    while(!caca_file_eof(f))
    {
        data = realloc(data, size + 1024);
        if(!data)
        {
            caca_file_close(f);
            seterrno(ENOMEM);
            return -1;
        }

        ret = (ssize_t)caca_file_read(f, data + size, 1024);
        if(ret >= 0)
            size += ret;
    }
    caca_file_close(f);

    ret = caca_import_canvas_from_memory(cv, data, size, format);
    free(data);

    return ret;
#endif
}

/** \brief Import a memory buffer into a canvas area
 *
 *  Import a memory buffer into the given libcaca canvas's current
 *  frame, at the specified position. For more information, see
 *  caca_import_canvas_from_memory().
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL Unsupported format requested or invalid coordinates.
 *  - \c ENOMEM Not enough memory to allocate canvas.
 *
 *  \param cv A libcaca canvas in which to import the file.
 *  \param x The leftmost coordinate of the area to import to.
 *  \param y The topmost coordinate of the area to import to.
 *  \param data A memory area containing the data to be loaded into the canvas.
 *  \param len The size in bytes of the memory area.
 *  \param format A string describing the input format.
 *  \return The number of bytes read, or 0 if there was not enough data,
 *  or -1 if an error occurred.
 */
ssize_t caca_import_area_from_memory(caca_canvas_t *cv, int x, int y,
                                     void const *data, size_t len,
                                     char const *format)
{
    caca_canvas_t *tmp;
    ssize_t ret;

    tmp = caca_create_canvas(0, 0);
    ret = caca_import_canvas_from_memory(tmp, data, len, format);

    if(ret > 0)
        caca_blit(cv, x, y, tmp, NULL);

    caca_free_canvas(tmp);

    return ret;
}

/** \brief Import a file into a canvas area
 *
 *  Import a file into the given libcaca canvas's current frame, at the
 *  specified position. For more information, see
 *  caca_import_canvas_from_file().
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c ENOSYS File access is not implemented on this system.
 *  - \c ENOMEM Not enough memory to allocate canvas.
 *  - \c EINVAL Unsupported format requested or invalid coordinates.
 *  caca_import_file() may also fail and set \b errno for any of the
 *  errors specified for the routine fopen().
 *
 *  \param cv A libcaca canvas in which to import the file.
 *  \param x The leftmost coordinate of the area to import to.
 *  \param y The topmost coordinate of the area to import to.
 *  \param filename The name of the file to load.
 *  \param format A string describing the input format.
 *  \return The number of bytes read, or 0 if there was not enough data,
 *  or -1 if an error occurred.
 */
ssize_t caca_import_area_from_file(caca_canvas_t *cv, int x, int y,
                                   char const *filename, char const *format)
{
    caca_canvas_t *tmp;
    ssize_t ret;

    tmp = caca_create_canvas(0, 0);
    ret = caca_import_canvas_from_file(tmp, filename, format);

    if(ret > 0)
        caca_blit(cv, x, y, tmp, NULL);

    caca_free_canvas(tmp);

    return ret;
}

/** \brief Get available import formats
 *
 *  Return a list of available import formats. The list is a NULL-terminated
 *  array of strings, interleaving a string containing the internal value for
 *  the import format, to be used with caca_import_canvas(), and a string
 *  containing the natural language description for that import format.
 *
 *  This function never fails.
 *
 *  \return An array of strings.
 */
char const * const * caca_get_import_list(void)
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

static ssize_t import_caca(caca_canvas_t *cv, void const *data, size_t size)
{
    uint8_t const *buf = (uint8_t const *)data;
    size_t control_size, data_size, expected_size;
    unsigned int frames, f, n, offset;
    uint16_t version, flags;
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
        debug("caca import error: control size %u < expected %u",
              (unsigned int)control_size, 16 + frames * 32);
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
        debug("caca import error: data size %u < expected %u",
              (unsigned int)data_size, (unsigned int)expected_size);
        goto invalid_caca;
    }

    caca_set_canvas_size(cv, 0, 0);
    caca_set_canvas_size(cv, xmax - xmin, ymax - ymin);

    for (f = caca_get_frame_count(cv); f--; )
    {
        caca_free_frame(cv, f);
    }

    for (offset = 0, f = 0; f < frames; f ++)
    {
        unsigned int width, height;

        width = sscanu32(buf + 4 + 16 + f * 32);
        height = sscanu32(buf + 4 + 16 + f * 32 + 4);
        caca_create_frame(cv, f);
        caca_set_frame(cv, f);

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

            caca_put_char(cv, x, y, sscanu32(buf + 4 + control_size
                                               + offset + 8 * n));
            caca_put_attr(cv, x, y, sscanu32(buf + 4 + control_size
                                               + offset + 8 * n + 4));
        }
        offset += width * height * 8;

        cv->frames[f].x -= cv->frames[f].handlex;
        cv->frames[f].y -= cv->frames[f].handley;
        cv->frames[f].handlex = -xmin;
        cv->frames[f].handley = -ymin;
    }

    caca_set_frame(cv, 0);

    return (ssize_t)(4 + control_size + data_size);

invalid_caca:
    seterrno(EINVAL);
    return -1;
}

/*
 * XXX: The following functions are aliases.
 */

ssize_t cucul_import_memory(cucul_canvas_t *, void const *, size_t,
                     char const *) CACA_ALIAS(caca_import_canvas_from_memory);
ssize_t cucul_import_file(cucul_canvas_t *, char const *,
                     char const *) CACA_ALIAS(caca_import_canvas_from_file);
ssize_t caca_import_memory(caca_canvas_t *, void const *, size_t, char const *)
                                  CACA_ALIAS(caca_import_canvas_from_memory);
ssize_t caca_import_file(caca_canvas_t *, char const *, char const *)
                                  CACA_ALIAS(caca_import_canvas_from_file);
char const * const * cucul_get_import_list(void)
         CACA_ALIAS(caca_get_import_list);

