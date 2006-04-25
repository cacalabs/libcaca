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

/** \brief Get the buffer size.
 *
 *  This function returns the length (in bytes) of the memory area stored
 *  in the given \e libcucul buffer.
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
 *  This function returns a pointer to the memory area stored in the given
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
 *  This function frees the structures associated with the given
 *  \e libcucul buffer.
 *
 *  This function never fails.
 *
 *  \param buf A \e libcucul buffer
 *  \return This function always returns 0.
 */
int cucul_free_buffer(cucul_buffer_t *buf)
{
    free(buf->data);
    free(buf);

    return 0;
}

