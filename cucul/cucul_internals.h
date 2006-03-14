/*
 *  libcucul      Unicode canvas library
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file cucul_internals.h
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief The \e libcucul private header.
 *
 *  This header contains the private types and functions used by \e libcucul.
 */

#ifndef __CUCUL_INTERNALS_H__
#define __CUCUL_INTERNALS_H__

#if defined(HAVE_INTTYPES_H) && !defined(__KERNEL__)
#   include <inttypes.h>
#elif !defined(CUSTOM_INTTYPES) && !defined(_DOXYGEN_SKIP_ME)
#   define CUSTOM_INTTYPES
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long int uint32_t;
typedef long int intptr_t;
typedef long unsigned int uintptr_t;
#endif

struct cucul_context
{
    /* Context size */
    unsigned int width, height;

    uint32_t *chars;
    uint8_t *attr;
    char *empty_line, *scratch_line;

    enum cucul_color fgcolor;
    enum cucul_color bgcolor;

    /* Internal libcucul features */
    enum cucul_feature background, antialiasing, dithering;

    unsigned int refcount;

    /* Exporters facilities */
    char *ansi_buffer;
    char *irc_buffer;
    char *html3_buffer;
    char *html_buffer;
    char *ps_buffer;
};

/* Initialisation functions */
extern int _cucul_init_bitmap(void);
extern int _cucul_end_bitmap(void);
void _cucul_set_size(cucul_t *qq, unsigned int width, unsigned int height);


#endif /* __CUCUL_INTERNALS_H__ */
