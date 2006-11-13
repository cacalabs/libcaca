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
 *  This file contains legacy functions that we keep around until all
 *  applications are ported.
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

/*
 * Functions from canvas.c
 */

int cucul_putchar(cucul_canvas_t *cv, int x, int y, unsigned long int ch)
{
    return cucul_put_char(cv, x, y, ch);
}

unsigned long int cucul_getchar(cucul_canvas_t *cv, int x, int y)
{
    return cucul_get_char(cv, x, y);
}

int cucul_putstr(cucul_canvas_t *cv, int x, int y, char const *s)
{
    return cucul_put_str(cv, x, y, s);
}

/*
 * Functions from color.c
 */

int cucul_set_color(cucul_canvas_t *cv, unsigned char fg, unsigned char bg)
{
    return cucul_set_color_ansi(cv, fg, bg);
}

int cucul_set_truecolor(cucul_canvas_t *cv, unsigned int fg, unsigned int bg)
{
    return cucul_set_color_argb(cv, fg, bg);
}

/*
 * Functions from import.c
 */

cucul_canvas_t * cucul_import_canvas(cucul_buffer_t *buf, char const *format)
{
    cucul_canvas_t *cv = cucul_create_canvas(0, 0);
    int ret = cucul_import_memory(cv, (unsigned char const *)buf->data,
                                  buf->size, format);
    if(ret < 0)
    {
        cucul_free_canvas(cv);
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

    ex->data = cucul_export_memory(cv, format, &ex->size);
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
    return cucul_get_frame_count(cv);
}

int cucul_set_canvas_frame(cucul_canvas_t *cv, unsigned int id)
{
    return cucul_set_frame(cv, id);
}

int cucul_create_canvas_frame(cucul_canvas_t *cv, unsigned int id)
{
    return cucul_create_frame(cv, id);
}

int cucul_free_canvas_frame(cucul_canvas_t *cv, unsigned int id)
{
    return cucul_free_frame(cv, id);
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

#if !defined(__KERNEL__)
cucul_buffer_t *cucul_load_file(char const *file)
{
    cucul_buffer_t *buf;
    FILE *fp;
    long int size;

    fp = fopen(file, "rb");
    if(!fp)
        return NULL;

    buf = malloc(sizeof(cucul_buffer_t));
    if(!buf)
    {
        fclose(fp);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);

    buf->data = malloc(size);
    if(!buf->data)
    {
        free(buf);
        fclose(fp);
        return NULL;
    }
    buf->size = size;
    buf->user_data = 0;

    fseek(fp, 0, SEEK_SET);
    fread(buf->data, buf->size, 1, fp);
    fclose(fp);

    return buf;
}
#endif

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

