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
 *  This file contains font handling functions.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   if defined(HAVE_ENDIAN_H)
#       include <endian.h>
#   endif
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#endif

#include "caca.h"
#include "caca_internals.h"

/* Internal fonts */
#include "mono9.data"
#include "monobold12.data"

/* Helper structures for font loading */
#if !defined(_DOXYGEN_SKIP_ME)
struct font_header
{
    uint32_t control_size, data_size;
    uint16_t version, blocks;
    uint32_t glyphs;
    uint16_t bpp, width, height, maxwidth, maxheight, flags;
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

struct caca_font
{
    struct font_header header;

    struct block_info *block_list;
    uint32_t *user_block_list;
    struct glyph_info *glyph_list;
    uint8_t *font_data;

    uint8_t *private;
};
#endif

#define DECLARE_UNPACKGLYPH(bpp) \
    static inline void \
      unpack_glyph ## bpp(uint8_t *glyph, uint8_t *packed_data, int n) \
{ \
    int i; \
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
 *  structure. The handle can then be used with caca_render_canvas()
 *  for bitmap output.
 *
 *  Internal fonts can also be loaded: if \c size is set to 0, \c data must
 *  be a string containing the internal font name.
 *
 *  If \c size is non-zero, the \c size bytes of memory at address \c data
 *  are loaded as a font. This memory are must not be freed by the calling
 *  program until the font handle has been freed with caca_free_font().
 *
 *  If an error occurs, NULL is returned and \b errno is set accordingly:
 *  - \c ENOENT Requested built-in font does not exist.
 *  - \c EINVAL Invalid font data in memory area.
 *  - \c ENOMEM Not enough memory to allocate font structure.
 *
 *  \param data The memory area containing the font or its name.
 *  \param size The size of the memory area, or 0 if the font name is given.
 *  \return A font handle or NULL in case of error.
 */
caca_font_t *caca_load_font(void const *data, size_t size)
{
    caca_font_t *f;
    int i;

    if(size == 0)
    {
        if(!strcasecmp(data, "Monospace 9"))
            return caca_load_font(mono9_data, mono9_size);
        if(!strcasecmp(data, "Monospace Bold 12"))
            return caca_load_font(monobold12_data, monobold12_size);

        seterrno(ENOENT);
        return NULL;
    }

    if(size < sizeof(struct font_header))
    {
        debug("font error: data size %i < header size %i",
              size, (int)sizeof(struct font_header));
        seterrno(EINVAL);
        return NULL;
    }

    f = malloc(sizeof(caca_font_t));
    if(!f)
    {
        seterrno(ENOMEM);
        return NULL;
    }

    f->private = (void *)(uintptr_t)data;

    memcpy(&f->header, f->private + 4, sizeof(struct font_header));
    f->header.control_size = hton32(f->header.control_size);
    f->header.data_size = hton32(f->header.data_size);
    f->header.version = hton16(f->header.version);
    f->header.blocks = hton16(f->header.blocks);
    f->header.glyphs = hton32(f->header.glyphs);
    f->header.bpp = hton16(f->header.bpp);
    f->header.width = hton16(f->header.width);
    f->header.height = hton16(f->header.height);
    f->header.maxwidth = hton16(f->header.maxwidth);
    f->header.maxheight = hton16(f->header.maxheight);
    f->header.flags = hton16(f->header.flags);

    if(size != 4 + f->header.control_size + f->header.data_size
        || (f->header.bpp != 8 && f->header.bpp != 4 &&
            f->header.bpp != 2 && f->header.bpp != 1)
        || (f->header.flags & 1) == 0)
    {
#if defined DEBUG
        if(size != 4 + f->header.control_size + f->header.data_size)
            debug("font error: data size %i < expected size %i",
                  size, 4 + f->header.control_size + f->header.data_size);
        else if(f->header.bpp != 8 && f->header.bpp != 4 &&
                f->header.bpp != 2 && f->header.bpp != 1)
            debug("font error: invalid bpp %i", f->header.bpp);
        else if((f->header.flags & 1) == 0)
            debug("font error: invalid flags %.04x", f->header.flags);
#endif
        free(f);
        seterrno(EINVAL);
        return NULL;
    }

    f->block_list = malloc(f->header.blocks * sizeof(struct block_info));
    if(!f->block_list)
    {
        free(f);
        seterrno(ENOMEM);
        return NULL;
    }

    f->user_block_list = malloc((f->header.blocks + 1)
                                  * 2 * sizeof(uint32_t));
    if(!f->user_block_list)
    {
        free(f->block_list);
        free(f);
        seterrno(ENOMEM);
        return NULL;
    }

    memcpy(f->block_list,
           f->private + 4 + sizeof(struct font_header),
           f->header.blocks * sizeof(struct block_info));
    for(i = 0; i < f->header.blocks; i++)
    {
        f->block_list[i].start = hton32(f->block_list[i].start);
        f->block_list[i].stop = hton32(f->block_list[i].stop);
        f->block_list[i].index = hton32(f->block_list[i].index);

        if(f->block_list[i].start > f->block_list[i].stop
            || (i > 0 && f->block_list[i].start < f->block_list[i - 1].stop)
            || f->block_list[i].index >= f->header.glyphs)
        {
#if defined DEBUG
            if(f->block_list[i].start > f->block_list[i].stop)
                debug("font error: block %i has start %i > stop %i",
                      i, f->block_list[i].start, f->block_list[i].stop);
            else if(i > 0 && f->block_list[i].start < f->block_list[i - 1].stop)
                debug("font error: block %i has start %i < previous stop %i",
                      f->block_list[i].start, f->block_list[i - 1].stop);
            else if(f->block_list[i].index >= f->header.glyphs)
                debug("font error: block %i has index >= glyph count %i",
                      f->block_list[i].index, f->header.glyphs);
#endif
            free(f->user_block_list);
            free(f->block_list);
            free(f);
            seterrno(EINVAL);
            return NULL;
        }

        f->user_block_list[i * 2] = f->block_list[i].start;
        f->user_block_list[i * 2 + 1] = f->block_list[i].stop;
    }

    f->user_block_list[i * 2] = 0;
    f->user_block_list[i * 2 + 1] = 0;

    f->glyph_list = malloc(f->header.glyphs * sizeof(struct glyph_info));
    if(!f->glyph_list)
    {
        free(f->user_block_list);
        free(f->block_list);
        free(f);
        seterrno(ENOMEM);
        return NULL;
    }

    memcpy(f->glyph_list,
           f->private + 4 + sizeof(struct font_header)
                + f->header.blocks * sizeof(struct block_info),
           f->header.glyphs * sizeof(struct glyph_info));
    for(i = 0; i < (int)f->header.glyphs; i++)
    {
        f->glyph_list[i].width = hton16(f->glyph_list[i].width);
        f->glyph_list[i].height = hton16(f->glyph_list[i].height);
        f->glyph_list[i].data_offset = hton32(f->glyph_list[i].data_offset);

        if(f->glyph_list[i].data_offset >= f->header.data_size
            || f->glyph_list[i].data_offset
                + (f->glyph_list[i].width * f->glyph_list[i].height *
                   f->header.bpp + 7) / 8 > f->header.data_size
            || f->glyph_list[i].width > f->header.maxwidth
            || f->glyph_list[i].height > f->header.maxheight)
        {
#if defined DEBUG
            if(f->glyph_list[i].data_offset >= f->header.data_size)
                debug("font error: glyph %i has data start %i > "
                      "data end %i",
                      f->glyph_list[i].data_offset, f->header.data_size);
            else if(f->glyph_list[i].data_offset
                     + (f->glyph_list[i].width * f->glyph_list[i].height *
                        f->header.bpp + 7) / 8 > f->header.data_size)
                debug("font error: glyph %i has data end %i > "
                      "data end %i", f->glyph_list[i].data_offset
                       + (f->glyph_list[i].width * f->glyph_list[i].height *
                          f->header.bpp + 7) / 8, f->header.data_size);
            else if(f->glyph_list[i].width > f->header.maxwidth)
                debug("font error: glyph %i has width %i > max width %i",
                      f->glyph_list[i].width, f->header.maxwidth);
            else if(f->glyph_list[i].height > f->header.maxheight)
                debug("font error: glyph %i has height %i > max height %i",
                      f->glyph_list[i].height, f->header.maxheight);
#endif
            free(f->glyph_list);
            free(f->user_block_list);
            free(f->block_list);
            free(f);
            seterrno(EINVAL);
            return NULL;
        }
    }

    f->font_data = f->private + 4 + f->header.control_size;

    return f;
}

/** \brief Get available builtin fonts
 *
 *  Return a list of available builtin fonts. The list is a NULL-terminated
 *  array of strings.
 *
 *  This function never fails.
 *
 *  \return An array of strings.
 */
char const * const * caca_get_font_list(void)
{
    static char const * const list[] =
    {
        "Monospace 9",
        "Monospace Bold 12",
        NULL
    };

    return list;
}

/** \brief Get a font's standard glyph width.
 *
 *  Return the standard value for the current font's glyphs. Most glyphs in
 *  the font will have this width, except fullwidth characters.
 *
 *  This function never fails.
 *
 *  \param f The font, as returned by caca_load_font()
 *  \return The standard glyph width.
 */
int caca_get_font_width(caca_font_t const *f)
{
    return f->header.width;
}

/** \brief Get a font's standard glyph height.
 *
 *  Returns the standard value for the current font's glyphs. Most glyphs in
 *  the font will have this height.
 *
 *  This function never fails.
 *
 *  \param f The font, as returned by caca_load_font()
 *  \return The standard glyph height.
 */
int caca_get_font_height(caca_font_t const *f)
{
    return f->header.height;
}

/** \brief Get a font's list of supported glyphs.
 *
 *  This function returns the list of Unicode blocks supported by the
 *  given font. The list is a zero-terminated list of indices. Here is
 *  an example:
 *
 *  \code
 *  {
 *     0x0000, 0x0080,   // Basic latin: A, B, C, a, b, c
 *     0x0080, 0x0100,   // Latin-1 supplement: "A, 'e, ^u
 *     0x0530, 0x0590,   // Armenian
 *     0x0000, 0x0000,   // END
 *  };
 *  \endcode
 *
 *  This function never fails.
 *
 *  \param f The font, as returned by caca_load_font()
 *  \return The list of Unicode blocks supported by the font.
 */
uint32_t const *caca_get_font_blocks(caca_font_t const *f)
{
    return (uint32_t const *)f->user_block_list;
}

/** \brief Free a font structure.
 *
 *  This function frees all data allocated by caca_load_font(). The
 *  font structure is no longer usable by other libcaca functions. Once
 *  this function has returned, the memory area that was given to
 *  caca_load_font() can be freed.
 *
 *  This function never fails.
 *
 *  \param f The font, as returned by caca_load_font()
 *  \return This function always returns 0.
 */
int caca_free_font(caca_font_t *f)
{
    free(f->glyph_list);
    free(f->user_block_list);
    free(f->block_list);
    free(f);

    return 0;
}

/** \brief Render the canvas onto an image buffer.
 *
 *  This function renders the given canvas on an image buffer using a specific
 *  font. The pixel format is fixed (32-bit ARGB, 8 bits for each component).
 *
 *  The required image width can be computed using
 *  caca_get_canvas_width() and caca_get_font_width(). The required
 *  height can be computed using caca_get_canvas_height() and
 *  caca_get_font_height().
 *
 *  Glyphs that do not fit in the image buffer are currently not rendered at
 *  all. They may be cropped instead in future versions.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL Specified width, height or pitch is invalid.
 *
 *  \param cv The canvas to render
 *  \param f The font, as returned by caca_load_font()
 *  \param buf The image buffer
 *  \param width The width (in pixels) of the image buffer
 *  \param height The height (in pixels) of the image buffer
 *  \param pitch The pitch (in bytes) of an image buffer line.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_render_canvas(caca_canvas_t const *cv, caca_font_t const *f,
                        void *buf, int width, int height, int pitch)
{
    uint8_t *glyph = NULL;
    int x, y, xmax, ymax;

    if(width < 0 || height < 0 || pitch < 0)
    {
        seterrno(EINVAL);
        return -1;
    }

    if(f->header.bpp != 8)
        glyph = malloc(f->header.width * 2 * f->header.height);

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
            int starty = y * f->header.height;
            int startx = x * f->header.width;
            uint32_t ch = cv->chars[y * cv->width + x];
            uint32_t attr = cv->attrs[y * cv->width + x];
            int b, i, j;
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

            caca_attr_to_argb64(attr, argb);

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

    return 0;
}

/*
 * XXX: The following functions are aliases.
 */

cucul_font_t *cucul_load_font(void const *, size_t) CACA_ALIAS(caca_load_font);
char const * const * cucul_get_font_list(void) CACA_ALIAS(caca_get_font_list);
int cucul_get_font_width(cucul_font_t const *) CACA_ALIAS(caca_get_font_width);
int cucul_get_font_height(cucul_font_t const *)
         CACA_ALIAS(caca_get_font_height);
uint32_t const *cucul_get_font_blocks(cucul_font_t const *)
         CACA_ALIAS(caca_get_font_blocks);
int cucul_render_canvas(cucul_canvas_t const *, cucul_font_t const *,
                        void *, int, int, int) CACA_ALIAS(caca_render_canvas);
int cucul_free_font(cucul_font_t *) CACA_ALIAS(caca_free_font);

