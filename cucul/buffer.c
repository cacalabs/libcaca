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
 *  This file contains buffer handling functions.
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

/** \brief Load a memory area into a buffer.
 *
 *  Create a \e libcucul buffer that points to the given memory area. The
 *  data is not duplicated and any changes made to the original memory area
 *  will appear in the buffer.
 *
 *  Keep in mind that buffers are not strings. When loading a C string, the
 *  terminating '\\0' must not be part of the buffer, hence \e size should
 *  be computed with strlen(). Conversely, the '\\0' is not appended to
 *  exported buffers even when they could be parsed as strings.
 *
 *  \param data The memory area to load.
 *  \param size The size of the memory area.
 *  \return A \e libcucul buffer pointing to the memory area, or NULL
 *          if an error occurred.
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

/** \brief Load a file into a buffer.
 *
 *  Load a file into memory and returns a \e libcucul buffer for use with
 *  other functions.
 *
 *  \param file The filename
 *  \return A \e libcucul buffer containing the file's contents, or NULL
 *          if an error occurred.
 */
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
/** \brief Get the buffer size.
 *
 *  Return the length (in bytes) of the memory area stored in the given
 *  \e libcucul buffer.
 *
 *  This function never fails.
 *
 *  \param buf A \e libcucul buffer
 *  \return The buffer data length.
 */
unsigned long int cucul_get_buffer_size(cucul_buffer_t *buf)
{
    return buf->size;
}

/** \brief Get the buffer data.
 *
 *  Get a pointer to the memory area stored in the given
 *  \e libcucul buffer.
 *
 *  This function never fails.
 *
 *  \param buf A \e libcucul buffer
 *  \return A pointer to the buffer memory area.
 */
void * cucul_get_buffer_data(cucul_buffer_t *buf)
{
    return buf->data;
}

/** \brief Free a buffer.
 *
 *  Free the structures associated with the given \e libcucul buffer.
 *
 *  This function never fails.
 *
 *  \param buf A \e libcucul buffer
 *  \return This function always returns 0.
 */
int cucul_free_buffer(cucul_buffer_t *buf)
{
    if(!buf->user_data)
        free(buf->data);

    free(buf);

    return 0;
}

