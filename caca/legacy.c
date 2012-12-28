/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains legacy functions that we keep around until all
 *  applications are ported.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#endif

#include "caca.h"
#include "caca_internals.h"

struct cucul_buffer
{
    size_t size;
    char *data;
    int user_data;
};

/*
 * Functions from canvas.c
 */

int cucul_putchar(cucul_canvas_t *cv, int x, int y, unsigned long int ch)
{
    return caca_put_char(cv, x, y, ch);
}

unsigned long int cucul_getchar(cucul_canvas_t *cv, int x, int y)
{
    return caca_get_char(cv, x, y);
}

int cucul_putstr(cucul_canvas_t *cv, int x, int y, char const *s)
{
    return caca_put_str(cv, x, y, s);
}

/*
 * Functions from color.c
 */

int cucul_set_color(cucul_canvas_t *cv, unsigned char fg, unsigned char bg)
{
    return caca_set_color_ansi(cv, fg, bg);
}

int cucul_set_truecolor(cucul_canvas_t *cv, unsigned int fg, unsigned int bg)
{
    return caca_set_color_argb(cv, fg, bg);
}

/*
 * Functions from dither.c
 */
int cucul_set_dither_invert(cucul_dither_t *d, int value)
{
    float gamma = caca_get_dither_gamma(d);

    if(gamma * (value ? -1 : 1) < 0)
        caca_set_dither_gamma(d, -gamma);

    return 0;
}

int cucul_set_dither_mode(cucul_dither_t *d, char const *s)
{
    return caca_set_dither_algorithm(d, s);
}

char const * const * cucul_get_dither_mode_list(cucul_dither_t const *d)
{
    return caca_get_dither_algorithm_list(d);
}

/*
 * Functions from import.c
 */

cucul_canvas_t * cucul_import_canvas(cucul_buffer_t *buf, char const *format)
{
    caca_canvas_t *cv = caca_create_canvas(0, 0);
    int ret = caca_import_canvas_from_memory(cv,
                                             (unsigned char const *)buf->data,
                                             buf->size, format);
    if(ret < 0)
    {
        caca_free_canvas(cv);
        return NULL;
    }

    return cv;
}

/*
 * Functions from export.c
 */

cucul_buffer_t * cucul_export_canvas(cucul_canvas_t *cv, char const *format)
{
    cucul_buffer_t *ex;

    ex = malloc(sizeof(cucul_buffer_t));
    if(!ex)
    {
        seterrno(ENOMEM);
        return NULL;
    }

    ex->data = caca_export_canvas_to_memory(cv, format, &ex->size);
    if(!ex->data)
    {
        free(ex);
        return NULL;
    }

    ex->user_data = 0;

    return ex;
}

/*
 * Functions from frame.c
 */

unsigned int cucul_get_canvas_frame_count(cucul_canvas_t *cv)
{
    return caca_get_frame_count(cv);
}

int cucul_set_canvas_frame(cucul_canvas_t *cv, unsigned int id)
{
    return caca_set_frame(cv, id);
}

int cucul_create_canvas_frame(cucul_canvas_t *cv, unsigned int id)
{
    return caca_create_frame(cv, id);
}

int cucul_free_canvas_frame(cucul_canvas_t *cv, unsigned int id)
{
    return caca_free_frame(cv, id);
}

/*
 * Functions from buffer.c
 */

cucul_buffer_t *cucul_load_memory(void *data, unsigned long int size)
{
    cucul_buffer_t *buf;

    buf = malloc(sizeof(cucul_buffer_t));
    if(!buf)
        return NULL;

    buf->data = data;
    buf->size = size;
    buf->user_data = 1;

    return buf;
}

cucul_buffer_t *cucul_load_file(char const *file)
{
    cucul_buffer_t *buf;
    caca_file_t *f;
    int ret;

    f = caca_file_open(file, "rb");
    if(!f)
        return NULL;

    buf = malloc(sizeof(cucul_buffer_t));
    if(!buf)
    {
        caca_file_close(f);
        return NULL;
    }

    buf->data = NULL;
    buf->size = 0;

    while(!caca_file_eof(f))
    {
        buf->data = realloc(buf->data, buf->size + 1024);
        if(!buf->data)
        {
            int saved_errno = geterrno();
            free(buf);
            caca_file_close(f);
            seterrno(saved_errno);
            return NULL;
        }

        ret = caca_file_read(f, buf->data + buf->size, 1024);
        if(ret >= 0)
            buf->size += ret;
    }
    caca_file_close(f);

    return buf;
}

unsigned long int cucul_get_buffer_size(cucul_buffer_t *buf)
{
    return buf->size;
}

void * cucul_get_buffer_data(cucul_buffer_t *buf)
{
    return buf->data;
}

int cucul_free_buffer(cucul_buffer_t *buf)
{
    if(!buf->user_data)
        free(buf->data);

    free(buf);

    return 0;
}

/*
 * Functions from transform.c
 */

int cucul_rotate(cucul_canvas_t *cv)
{
    return caca_rotate_180(cv);
}

