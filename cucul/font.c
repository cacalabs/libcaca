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
 *  This file contains font handling functions.
 */

#include "config.h"
#include "common.h"

#if !defined(USE_WIN32)
#if !defined(__KERNEL__)
#   if defined(HAVE_ENDIAN_H)
#       include <endian.h>
#   endif
#   if defined(HAVE_ARPA_INET_H)
#       include <arpa/inet.h>
#   elif defined(HAVE_NETINET_IN_H)
#       include <netinet/in.h>
#   endif
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

/* Internal fonts */
#include "font_mono9.h"
#include "font_monobold12.h"

/* Helper structures for font loading */
#if !defined(_DOXYGEN_SKIP_ME)
struct font_header
{
    uint32_t control_size, data_size;
    uint16_t version, blocks;
    uint32_t glyphs;
    uint16_t bpp, width, height, flags;
};

struct block_info
{
    uint32_t start, stop, index;
};

struct glyph_info
{
    uint16_t width, height;
    uint32_t data_offset;
};

struct cucul_font
{
    struct font_header header;

    struct block_info *block_list;
    struct glyph_info *glyph_list;
    uint8_t *font_data;

    uint8_t *private;
};
#endif

#define DECLARE_UNPACKGLYPH(bpp) \
    static inline void \
      unpack_glyph ## bpp(uint8_t *glyph, uint8_t *packed_data, \
                          unsigned int n) \
{ \
    unsigned int i; \
    \
    for(i = 0; i < n; i++) \
    { \
        uint8_t pixel = packed_data[i / (8 / bpp)]; \
        pixel >>= bpp * ((8 / bpp) - 1 - (i % (8 / bpp))); \
        pixel %= (1 << bpp); \
        pixel *= 0xff / ((1 << bpp) - 1); \
        *glyph++ = pixel; \
    } \
}

DECLARE_UNPACKGLYPH(4)
DECLARE_UNPACKGLYPH(2)
DECLARE_UNPACKGLYPH(1)

/** \brief Load a font from memory for future use.
 *
 *  This function loads a font and returns a handle to its internal
 *  structure. The handle can then be used with cucul_render_canvas()
 *  for bitmap output.
 *
 *  Internal fonts can also be loaded: if \c size is set to 0, \c data must
 *  be a string containing the internal font name.
 *
 *  If \c size is non-zero, the \c size bytes of memory at address \c data
 *  are loaded as a font. This memory are must not be freed by the calling
 *  program until the font handle has been freed with cucul_free_font().
 *
 *  \param data The memory area containing the font or its name.
 *  \param size The size of the memory area, or 0 if the font name is given.
 *  \return A font handle or NULL in case of error.
 */
cucul_font_t *cucul_load_font(void const *data, unsigned int size)
{
    cucul_font_t *f;
    unsigned int i;

    if(size == 0)
    {
        if(!strcasecmp(data, "Monospace 9"))
            return cucul_load_font(mono9_data, mono9_size);
        if(!strcasecmp(data, "Monospace Bold 12"))
            return cucul_load_font(monobold12_data, monobold12_size);

        return NULL;
    }

    if(size < sizeof(struct font_header))
        return NULL;

    f = malloc(sizeof(cucul_font_t));
    f->private = (void *)(uintptr_t)data;

    memcpy(&f->header, f->private + 8, sizeof(struct font_header));
    f->header.control_size = htonl(f->header.control_size);
    f->header.data_size = htonl(f->header.data_size);
    f->header.version = htons(f->header.version);
    f->header.blocks = htons(f->header.blocks);
    f->header.glyphs = htonl(f->header.glyphs);
    f->header.bpp = htons(f->header.bpp);
    f->header.width = htons(f->header.width);
    f->header.height = htons(f->header.height);
    f->header.flags = htons(f->header.flags);

    if(size != 8 + f->header.control_size + f->header.data_size
        || (f->header.bpp != 8 && f->header.bpp != 4 &&
            f->header.bpp != 2 && f->header.bpp != 1)
        || (f->header.flags & 1) == 0)
    {
        free(f);
        return NULL;
    }

    f->block_list = malloc(f->header.blocks * sizeof(struct block_info));
    memcpy(f->block_list,
           f->private + 8 + sizeof(struct font_header),
           f->header.blocks * sizeof(struct block_info));
    for(i = 0; i < f->header.blocks; i++)
    {
        f->block_list[i].start = htonl(f->block_list[i].start);
        f->block_list[i].stop = htonl(f->block_list[i].stop);
        f->block_list[i].index = htonl(f->block_list[i].index);

        if(f->block_list[i].start > f->block_list[i].stop
            || (i > 0 && f->block_list[i].start < f->block_list[i - 1].stop)
            || f->block_list[i].index >= f->header.glyphs)
        {
            free(f->block_list);
            free(f);
            return NULL;
        }
    }

    f->glyph_list = malloc(f->header.glyphs * sizeof(struct glyph_info));
    memcpy(f->glyph_list,
           f->private + 8 + sizeof(struct font_header)
                + f->header.blocks * sizeof(struct block_info),
           f->header.glyphs * sizeof(struct glyph_info));
    for(i = 0; i < f->header.glyphs; i++)
    {
        f->glyph_list[i].width = htons(f->glyph_list[i].width);
        f->glyph_list[i].height = htons(f->glyph_list[i].height);
        f->glyph_list[i].data_offset = htonl(f->glyph_list[i].data_offset);

        if(f->glyph_list[i].data_offset >= f->header.data_size
            || f->glyph_list[i].data_offset
                + (f->glyph_list[i].width * f->glyph_list[i].height *
                   f->header.bpp + 7) / 8 > f->header.data_size)
        {
            free(f->glyph_list);
            free(f->block_list);
            free(f);
            return NULL;
        }
    }

    f->font_data = f->private + 8 + f->header.control_size;

    return f;
}

/** \brief Get available builtin fonts
 *
 *  Return a list of available builtin fonts. The list is a NULL-terminated
 *  array of strings.
 *
 *  \return An array of strings.
 */
char const * const * cucul_get_font_list(void)
{
    static char const * const list[] =
    {
        "Monospace 9",
        "Monospace Bold 12",
        NULL
    };

    return list;
}

/** \brief Get a font's maximum glyph width.
 *
 *  This function returns the maximum value for the current font's glyphs
 *
 *  \param f The font, as returned by cucul_load_font()
 *  \return The maximum glyph width.
 */
unsigned int cucul_get_font_width(cucul_font_t *f)
{
    return f->header.width;
}

/** \brief Get a font's maximum glyph height.
 *
 *  This function returns the maximum value for the current font's glyphs
 *
 *  \param f The font, as returned by cucul_load_font()
 *  \return The maximum glyph height.
 */
unsigned int cucul_get_font_height(cucul_font_t *f)
{
    return f->header.height;
}

/** \brief Free a font structure.
 *
 *  This function frees all data allocated by cucul_load_font(). The
 *  font structure is no longer usable by other libcucul functions. Once
 *  this function has returned, the memory area that was given to
 *  cucul_load_font() can be freed.
 *
 *  \param f The font, as returned by cucul_load_font()
 */
void cucul_free_font(cucul_font_t *f)
{
    free(f->glyph_list);
    free(f->block_list);
    free(f);
}

/** \brief Render the canvas onto an image buffer.
 *
 *  This function renders the given canvas on an image buffer using a specific
 *  font. The pixel format is fixed (32-bit ARGB, 8 bits for each component).
 *
 *  The required image width can be computed using
 *  cucul_get_canvas_width() and cucul_get_font_width(). The required
 *  height can be computed using cucul_get_canvas_height() and
 *  cucul_get_font_height().
 *
 *  Glyphs that do not fit in the image buffer are currently not rendered at
 *  all. They may be cropped instead in future versions.
 *
 *  \param cv The canvas to render
 *  \param f The font, as returned by cucul_load_font()
 *  \param buf The image buffer
 *  \param width The width (in pixels) of the image buffer
 *  \param height The height (in pixels) of the image buffer
 *  \param pitch The pitch (in bytes) of an image buffer line.
 */
void cucul_render_canvas(cucul_canvas_t *cv, cucul_font_t *f,
                         void *buf, unsigned int width,
                         unsigned int height, unsigned int pitch)
{
    uint8_t *glyph = NULL;
    unsigned int x, y, xmax, ymax;

    if(f->header.bpp != 8)
        glyph = malloc(f->header.width * f->header.height);

    if(width < cv->width * f->header.width)
        xmax = width / f->header.width;
    else
        xmax = cv->width;

    if(height < cv->height * f->header.height)
        ymax = height / f->header.height;
    else
        ymax = cv->height;

    for(y = 0; y < ymax; y++)
    {
        for(x = 0; x < xmax; x++)
        {
            uint8_t argb[8];
            unsigned int starty = y * f->header.height;
            unsigned int startx = x * f->header.width;
            uint32_t ch = cv->chars[y * cv->width + x];
            uint32_t attr = cv->attr[y * cv->width + x];
            unsigned int b, i, j;
            struct glyph_info *g;

            /* Find the Unicode block where our glyph lies */
            for(b = 0; b < f->header.blocks; b++)
            {
                if(ch < f->block_list[b].start)
                {
                    b = f->header.blocks;
                    break;
                }

                if(ch < f->block_list[b].stop)
                    break;
            }

            /* Glyph not in font? Skip it. */
            if(b == f->header.blocks)
                continue;

            g = &f->glyph_list[f->block_list[b].index
                                + ch - f->block_list[b].start];

            _cucul_argb32_to_argb4(attr, argb);

            /* Step 1: unpack glyph */
            switch(f->header.bpp)
            {
            case 8:
                glyph = f->font_data + g->data_offset;
                break;
            case 4:
                unpack_glyph4(glyph, f->font_data + g->data_offset,
                              g->width * g->height);
                break;
            case 2:
                unpack_glyph2(glyph, f->font_data + g->data_offset,
                              g->width * g->height);
                break;
            case 1:
                unpack_glyph1(glyph, f->font_data + g->data_offset,
                              g->width * g->height);
                break;
            }

            /* Step 2: render glyph using colour attribute */
            for(j = 0; j < g->height; j++)
            {
                uint8_t *line = buf;
                line += (starty + j) * pitch + 4 * startx;

                for(i = 0; i < g->width; i++)
                {
                    uint8_t *pixel = line + 4 * i;
                    uint32_t p, q, t;

                    p = glyph[j * g->width + i];
                    q = 0xff - p;

                    for(t = 0; t < 4; t++)
                       pixel[t] = (((q * argb[t]) + (p * argb[4 + t])) / 0xf);
                }
            }
        }
    }

    if(f->header.bpp != 8)
        free(glyph);
}

/*
 * The libcaca font format, version 1
 * ----------------------------------
 *
 * All types are big endian.
 *
 * struct
 * {
 *    uint8_t caca_header[4];    // "CACA"
 *    uint8_t caca_file_type[4]; // "FONT"
 *
 * font_header:
 *    uint32_t control_size;     // Control size (font_data - font_header)
 *    uint32_t data_size;        // Data size (EOF - font_data)
 *
 *    uint16_t version;          // Font format version
 *                               //  bit 0: set to 1 if font is compatible
 *                               //         with version 1 of the format
 *                               //  bits 1-15: unused yet, must be 0
 *
 *    uint16_t blocks;           // Number of blocks in the font
 *    uint32_t glyphs;           // Total number of glyphs in the font
 *
 *    uint16_t bpp;              // Bits per pixel for glyph data (valid
 *                               // Values are 1, 2, 4 and 8)
 *    uint16_t width;            // Maximum glyph width
 *    uint16_t height;           // Maximum glyph height
 *
 *    uint16_t flags;            // Feature flags
 *                               //  bit 0: set to 1 if font is fixed width
 *                               //  bits 1-15: unused yet, must be 0
 *
 * block_info:
 *    struct
 *    {
 *       uint32_t start;         // Unicode index of the first glyph
 *       uint32_t stop;          // Unicode index of the last glyph + 1
 *       uint32_t index;         // Glyph info index of the first glyph
 *    }
 *    block_list[blocks];
 *
 * glyph_info:
 *    struct
 *    {
 *       uint16_t width;         // Glyph width in pixels
 *       uint16_t height;        // Glyph height in pixels
 *       uint32_t data_offset;   // Offset (starting from data) to the data
 *                               // for the first character
 *    }
 *    glyph_list[glyphs];
 *
 * extension_1:
 * extension_2:
 *    ...
 * extension_N:
 *    ...                        // reserved for future use
 *
 * font_data:
 *    uint8_t data[data_size];   // glyph data
 * };
 */

#endif
