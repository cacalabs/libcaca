/*
 *  makefont      create libcaca font data
 *  Copyright (c) 2006-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 *
 * Usage:
 *   makefont <prefix> <font> <dpi> <bpp>
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#if defined HAVE_ARPA_INET_H
#   include <arpa/inet.h>
#elif defined HAVE_NETINET_IN_H
#   include <netinet/in.h>
#endif

#include <pango/pango.h>
#include <pango/pangoft2.h>

#include "caca_stubs.h"
#include "caca.h"

/* Split our big strings into chunks of 480 characters, because it is
 * the multiple of 32 directly below 509, which is the maximum allowed
 * string size in C89. */
#define STRING_CHUNKS 480

/* This list is built so that it includes all of ASCII, Latin-1, CP-437,
 * and the UTF-8 glyphs necessary for canvas rotation and mirroring. */
static unsigned int const blocklist[] =
{
    0x0000, 0x0080, /* Basic latin: A, B, C, a, b, c */
    0x0080, 0x0100, /* Latin-1 Supplement: Ä, Ç, å, ß */
    0x0100, 0x0180, /* Latin Extended-A: Ā č Ō œ */
    0x0180, 0x0250, /* Latin Extended-B: Ǝ Ƹ */
    0x0250, 0x02b0, /* IPA Extensions: ɐ ɔ ɘ ʌ ʍ */
    0x0370, 0x0400, /* Greek and Coptic: Λ α β */
    0x0400, 0x0500, /* Cyrillic: И Я */
    0x0530, 0x0590, /* Armenian: Ո */
    0x1401, 0x1677, /* Unified Canadian Aboriginal Syllabics: ᒐ ᗡ */
    0x1d00, 0x1d80, /* Phonetic Extensions: ᴉ ᵷ */
    0x2000, 0x2070, /* General Punctuation: ‘’ “” */
    0x2100, 0x2150, /* Letterlike Symbols: Ⅎ */
    0x2200, 0x2300, /* Mathematical Operators: ∀ √ ∞ ∙ */
    0x2300, 0x2400, /* Miscellaneous Technical: ⌐ ⌂ ⌠ ⌡ */
    0x2500, 0x2580, /* Box Drawing: ═ ║ ╗ ╔ ╩ */
    0x2580, 0x25a0, /* Block Elements: ▛ ▞ ░ ▒ ▓ */
    0x25a0, 0x2600, /* Geometric Shapes: ◆ ○ ● */
    0x2600, 0x2700, /* Miscellaneous Symbols: ♥ ★ ☭ */
    0x3000, 0x3040, /* CJK Symbols and Punctuation: 。「」 */
    0x3040, 0x30a0, /* Hiragana: で す */
    0x30a0, 0x3100, /* Katakana: ロ ル */
    0xff00, 0xfff0, /* Halfwidth and Fullwidth Forms: Ａ, Ｂ, Ｃ, ａ, ｂ, ｃ */
    0x10400, 0x10450, /* Deseret: 𐐒 𐐋 */
    0, 0
};

struct glyph
{
    uint32_t unicode;
    char buf[10];
    unsigned int same_as;
    unsigned int data_offset;
    unsigned int data_width;
    unsigned int data_size;
};

static void fix_glyph(FT_Bitmap *, uint32_t, unsigned int, unsigned int);
static int printf_unicode(struct glyph *);
static int printf_hex(char const *, uint8_t *, int);
static int printf_u32(char const *, uint32_t);
static int printf_u16(char const *, uint16_t);

/* Counter for written bytes */
static int written = 0;

int main(int argc, char *argv[])
{
    PangoContext *cx;
    PangoFontDescription *fd;
    PangoFontMap *fm;
    PangoLayout *l;
    PangoRectangle r;

    FT_Bitmap img;
    int stdwidth, fullwidth, height, blocks, glyphs, fullglyphs;
    unsigned int n, b, i;
    unsigned int stdsize, fullsize, control_size, data_size, current_offset;
    uint8_t *glyph_data;
    struct glyph *gtab;

    unsigned int bpp, dpi;
    char const *prefix, *font;

    if(argc != 5)
    {
        fprintf(stderr, "%s: wrong argument count\n", argv[0]);
        fprintf(stderr, "usage: %s <prefix> <font> <dpi> <bpp>\n", argv[0]);
        fprintf(stderr, "eg: %s monospace9 \"Monospace 9\" 96 4\n", argv[0]);
        return -1;
    }

    prefix = argv[1];
    font = argv[2];
    dpi = atoi(argv[3]);
    bpp = atoi(argv[4]);

    if(dpi == 0 || (bpp != 1 && bpp != 2 && bpp != 4 && bpp != 8))
    {
        fprintf(stderr, "%s: invalid argument\n", argv[0]);
        return -1;
    }

    fprintf(stderr, "Font \"%s\", %i dpi, %i bpp\n", font, dpi, bpp);

    /* Initialise Pango */
    fm = pango_ft2_font_map_new();
    pango_ft2_font_map_set_resolution(PANGO_FT2_FONT_MAP(fm), dpi, dpi);
    cx = pango_ft2_font_map_create_context(PANGO_FT2_FONT_MAP(fm));

    l = pango_layout_new(cx);
    if(!l)
    {
        fprintf(stderr, "%s: unable to initialise pango\n", argv[0]);
        g_object_unref(cx);
        return -1;
    }

    fd = pango_font_description_from_string(font);
    pango_layout_set_font_description(l, fd);
    pango_font_description_free(fd);

    /* Initialise our FreeType2 bitmap */
    img.width = 256;
    img.pitch = 256;
    img.rows = 256;
    img.buffer = malloc(256 * 256);
    img.num_grays = 256;
    img.pixel_mode = ft_pixel_mode_grays;

    /* Test rendering so that we know the glyph width */
    pango_layout_set_markup(l, "@", -1);
    pango_layout_get_extents(l, NULL, &r);
    stdwidth = PANGO_PIXELS(r.width);
    fullwidth = stdwidth * 2;
    height = PANGO_PIXELS(r.height);
    stdsize = ((stdwidth * height) + (8 / bpp) - 1) / (8 / bpp);
    fullsize = ((fullwidth * height) + (8 / bpp) - 1) / (8 / bpp);

    /* Compute blocks and glyphs count */
    blocks = 0;
    glyphs = 0;
    fullglyphs = 0;
    for(b = 0; blocklist[b + 1]; b += 2)
    {
        blocks++;
        glyphs += blocklist[b + 1] - blocklist[b];
        for(i = blocklist[b]; i < blocklist[b + 1]; i++)
            if(caca_utf32_is_fullwidth(i))
                fullglyphs++;
    }

    control_size = 28 + 12 * blocks + 8 * glyphs;
    data_size = stdsize * (glyphs - fullglyphs) + fullsize * fullglyphs;

    gtab = malloc(glyphs * sizeof(struct glyph));
    glyph_data = malloc(data_size);

    /* Let's go! */
    printf("/* libcaca font file\n");
    printf(" * \"%s\": %i dpi, %i bpp, %ix%i/%ix%i glyphs\n",
           font, dpi, bpp, stdwidth, height, fullwidth, height);
    printf(" * Automatically generated by tools/makefont.c:\n");
    printf(" *   tools/makefont %s \"%s\" %i %i\n", prefix, font, dpi, bpp);
    printf(" */\n");
    printf("\n");

    printf("static size_t const %s_size = %i;\n",
           prefix, 4 + control_size + data_size);
    printf("static uint8_t const %s_data[%i] =\n",
           prefix, 4 + control_size + data_size);
    printf("{\n");

    printf("/* file: */\n");
    printf("0xCA,0xCA, /* caca_header */\n");
    written += 2;
    printf("'F','T', /* caca_file_type */\n");
    written += 2;
    printf("\n");

    printf("/* font_header: */\n");
    printf_u32("%s /* control_size */\n", control_size);
    printf_u32("%s /* data_size */\n", data_size);
    printf_u16("%s /* version */\n", 1);
    printf_u16("%s /* blocks */\n", blocks);
    printf_u32("%s /* glyphs */\n", glyphs);
    printf_u16("%s /* bpp */\n", bpp);
    printf_u16("%s /* std width */\n", stdwidth);
    printf_u16("%s /* std height */\n", height);
    printf_u16("%s /* max width */\n", fullwidth);
    printf_u16("%s /* max height */\n", height);
    printf_u16("%s /* flags */\n", 1);
    printf("\n");

    printf("/* block_info: */\n");
    n = 0;
    for(b = 0; blocklist[b + 1]; b += 2)
    {
        printf_u32("%s", blocklist[b]);
        printf_u32("%s", blocklist[b + 1]);
        printf_u32("%s\n", n);
        n += blocklist[b + 1] - blocklist[b];
    }
    printf("\n");

    /* Render all glyphs, so that we can know their offset */
    current_offset = n = 0;
    for(b = 0; blocklist[b + 1]; b += 2)
    {
        for(i = blocklist[b]; i < blocklist[b + 1]; i++)
        {
            int x, y, bytes, current_width = stdwidth;
            unsigned int k, current_size = stdsize;

            if(caca_utf32_is_fullwidth(i))
            {
                current_width = fullwidth;
                current_size = fullsize;
            }
            gtab[n].unicode = i;
            bytes = caca_utf32_to_utf8(gtab[n].buf, gtab[n].unicode);
            gtab[n].buf[bytes] = '\0';

            /* Render glyph on a bitmap */
            pango_layout_set_text(l, gtab[n].buf, -1);
            memset(img.buffer, 0, img.pitch * height);
            pango_ft2_render_layout(&img, l, 0, 0);

            /* Fix glyphs that we know how to handle better */
            fix_glyph(&img, gtab[n].unicode, current_width, height);

            /* Write bitmap as an escaped C string */
            memset(glyph_data + current_offset, 0, current_size);
            k = 0;
            for(y = 0; y < height; y++)
            {
                for(x = 0; x < current_width; x++)
                {
                    uint8_t pixel = img.buffer[y * img.pitch + x];

                    pixel >>= (8 - bpp);
                    glyph_data[current_offset + k / 8]
                        |= pixel << (8 - bpp - (k % 8));
                    k += bpp;
                }
            }

            /* Check whether this is the same glyph as another one. Please
             * don't bullshit me about sorting, hashing and stuff like that,
             * our data is small enough for this to work. */
            for(k = 0; k < n; k++)
            {
                if(gtab[k].data_size != current_size)
                    continue;
#if 0
                if(!memcmp(glyph_data + gtab[k].data_offset,
                           glyph_data + current_offset, current_size))
                    break;
#endif
            }

            gtab[n].data_offset = current_offset;
            gtab[n].data_width = current_width;
            gtab[n].data_size = current_size;
            gtab[n].same_as = k;

            if(k == n)
                current_offset += current_size;

            n++;
        }
    }

    printf("/* glyph_info: */\n");
    n = 0;
    for(b = 0; blocklist[b + 1]; b += 2)
    {
        for(i = blocklist[b]; i < blocklist[b + 1]; i++)
        {
            printf_u16("%s", gtab[n].data_width);
            printf_u16("%s", height);
            printf_u32("%s\n", gtab[gtab[n].same_as].data_offset);
            n++;
        }
    }
    printf("\n");

    printf("/* font_data: */\n");
    n = 0;
    for(b = 0; blocklist[b + 1]; b += 2)
    {
        for(i = blocklist[b]; i < blocklist[b + 1]; i++)
        {
            /* Print glyph value in comment */
            printf("/* ");
            printf_unicode(&gtab[n]);

            if(gtab[n].same_as == n)
                printf_hex(" */ %s\n",
                           glyph_data + gtab[n].data_offset, gtab[n].data_size);
            else
            {
                printf(" is ");
                printf_unicode(&gtab[gtab[n].same_as]);
                printf(" */\n");
            }

            n++;
        }
    }

    printf("};\n");

    free(img.buffer);
    free(gtab);
    free(glyph_data);
    g_object_unref(l);
    g_object_unref(cx);
    g_object_unref(fm);

    return 0;
}

/*
 * XXX: the following functions are local
 */

static void fix_glyph(FT_Bitmap *i, uint32_t ch,
                      unsigned int width, unsigned int height)
{
    unsigned int x, y;

    switch(ch)
    {
    case 0x00002580: /* ▀ */
        for(y = 0; y < height; y++)
            for(x = 0; x < width; x++)
                i->buffer[x + y * i->pitch] = y < height / 2 ? 0xff : 0x00;
        if(height & 1)
            for(x = 0; x < width; x++)
                i->buffer[x + (height / 2) * i->pitch] = 0x7f;
        break;
    case 0x00002584: /* ▄ */
        for(y = 0; y < height; y++)
            for(x = 0; x < width; x++)
                i->buffer[x + y * i->pitch] = y < height / 2 ? 0x00 : 0xff;
        if(height & 1)
            for(x = 0; x < width; x++)
                i->buffer[x + (height / 2) * i->pitch] = 0x7f;
        break;
    case 0x0000258c: /* ▌ */
        for(y = 0; y < height; y++)
            for(x = 0; x < width; x++)
                i->buffer[x + y * i->pitch] = x < width / 2 ? 0xff : 0x00;
        if(width & 1)
            for(y = 0; y < height; y++)
                i->buffer[(width / 2) + y * i->pitch] = 0x7f;
        break;
    case 0x00002590: /* ▐ */
        for(y = 0; y < height; y++)
            for(x = 0; x < width; x++)
                i->buffer[x + y * i->pitch] = x < width / 2 ? 0x00 : 0xff;
        if(width & 1)
            for(y = 0; y < height; y++)
                i->buffer[(width / 2) + y * i->pitch] = 0x7f;
        break;
    case 0x000025a0: /* ■ */
        for(y = 0; y < height; y++)
            for(x = 0; x < width; x++)
                i->buffer[x + y * i->pitch] =
                    (y >= height / 4) && (y < 3 * height / 4) ? 0xff : 0x00;
        if(height & 3)
            for(x = 0; x < width; x++) /* FIXME: could be more precise */
                i->buffer[x + (height / 4) * i->pitch] =
                    i->buffer[x + (3 * height / 4) * i->pitch] = 0x7f;
        break;
    case 0x00002588: /* █ */
        memset(i->buffer, 0xff, height * i->pitch);
        break;
    case 0x00002593: /* ▓ */
        for(y = 0; y < height; y++)
            for(x = 0; x < width; x++)
                i->buffer[x + y * i->pitch] =
                    ((x + 2 * (y & 1)) & 3) ? 0xff : 0x00;
        break;
    case 0x00002592: /* ▒ */
        for(y = 0; y < height; y++)
            for(x = 0; x < width; x++)
                i->buffer[x + y * i->pitch] = ((x + y) & 1) ? 0xff : 0x00;
        break;
    case 0x00002591: /* ░ */
        for(y = 0; y < height; y++)
            for(x = 0; x < width; x++)
                i->buffer[x + y * i->pitch] =
                    ((x + 2 * (y & 1)) & 3) ? 0x00 : 0xff;
        break;
    }
}

static int printf_unicode(struct glyph *g)
{
    int wr = 0;

    wr += printf("U+%.04X: \"", g->unicode);

    if(g->unicode < 0x20 || (g->unicode >= 0x7f && g->unicode <= 0xa0))
        wr += printf("\\x%.02x\"", g->unicode);
    else
        wr += printf("%s\"", g->buf);

    return wr;
}

static int printf_u32(char const *fmt, uint32_t i)
{
    uint32_t ni = hton32(i);
    return printf_hex(fmt, (uint8_t *)&ni, 4);
}

static int printf_u16(char const *fmt, uint16_t i)
{
    uint16_t ni = hton16(i);
    return printf_hex(fmt, (uint8_t *)&ni, 2);
}

static int printf_hex(char const *fmt, uint8_t *data, int bytes)
{
    char buf[BUFSIZ];
    char *parser = buf;

    while(bytes--)
        parser += sprintf(parser, "%i,", (unsigned int)*data++);
    parser[0] = '\0';

    return printf(fmt, buf);
}

