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

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

static cucul_canvas_t *import_caca(void const *, unsigned int);

/** \brief Import a buffer into a canvas
 *
 *  This function imports a memory area into an internal libcucul canvas.
 *
 *  Valid values for \c format are:
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
    if(!strcasecmp("caca", format))
        return import_caca(data, size);

    /* FIXME: Try to autodetect */
    if(!strcasecmp("", format))
        return import_caca(data, size);

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
        "caca", "native libcaca format",
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
    uint8_t *buf = (uint8_t *)data;
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

