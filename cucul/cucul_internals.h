/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
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
    uint32_t *attr;
    char *empty_line, *scratch_line;

    uint16_t fgcolor;
    uint16_t bgcolor;

    unsigned int refcount;
};

/* Bitmap functions */
extern int _cucul_init_dither(void);
extern int _cucul_end_dither(void);

/* Canvas functions */
extern void _cucul_set_size(cucul_t *, unsigned int, unsigned int);
extern void _cucul_putchar32(cucul_t *qq, int x, int y, uint32_t c);

/* Charset functions */
extern unsigned int _cucul_strlen_utf8(char const *);
extern char const *_cucul_skip_utf8(char const *, unsigned int);
extern uint32_t _cucul_utf8_to_utf32(char const *);
extern uint8_t _cucul_utf32_to_cp437(uint32_t);
extern uint32_t _cucul_cp437_to_utf32(uint8_t);

/* Colour functions */
uint8_t _cucul_argb32_to_ansi8(uint32_t);
uint8_t _cucul_argb32_to_ansi4fg(uint32_t);
uint8_t _cucul_argb32_to_ansi4bg(uint32_t);
void _cucul_argb32_to_argb4(uint32_t, uint8_t[8]);

/* Export functions */
extern void _cucul_get_ansi(cucul_t *, struct cucul_buffer *);
extern void _cucul_get_html(cucul_t *, struct cucul_buffer *);
extern void _cucul_get_html3(cucul_t *, struct cucul_buffer *);
extern void _cucul_get_irc(cucul_t *, struct cucul_buffer *);
extern void _cucul_get_ps(cucul_t *, struct cucul_buffer *);
extern void _cucul_get_svg(cucul_t *, struct cucul_buffer *);

#endif /* __CUCUL_INTERNALS_H__ */
