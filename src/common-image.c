/*
 *  Imaging tools for cacaview and img2irc
 *  Copyright (c) 2002-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <string.h>
#   include <stdlib.h>
#endif

#if defined(USE_IMLIB2)
#   include <Imlib2.h>
#endif

#include "caca.h"

#include "common-image.h"

#if !defined(USE_IMLIB2)
static unsigned int u32fread(caca_file_t *);
static unsigned int u16fread(caca_file_t *);
static unsigned int u8fread(caca_file_t *);
#endif

struct image * load_image(char const * name)
{
    struct image * im = malloc(sizeof(struct image));
    unsigned int depth, bpp, rmask, gmask, bmask, amask;

#if defined(USE_IMLIB2)
    Imlib_Image image;

    /* Load the new image */
    image = imlib_load_image(name);

    if(!image)
    {
        free(im);
        return NULL;
    }

    imlib_context_set_image(image);
    im->pixels = (char *)imlib_image_get_data_for_reading_only();
    im->w = imlib_image_get_width();
    im->h = imlib_image_get_height();
    rmask = 0x00ff0000;
    gmask = 0x0000ff00;
    bmask = 0x000000ff;
    amask = 0xff000000;
    bpp = 32;
    depth = 4;

    /* Create the libcaca dither */
    im->dither = caca_create_dither(bpp, im->w, im->h, depth * im->w,
                                     rmask, gmask, bmask, amask);
    if(!im->dither)
    {
        imlib_free_image();
        free(im);
        return NULL;
    }

    im->priv = (void *)image;

#else
    /* Try to load a BMP file */
    uint32_t red[256], green[256], blue[256], alpha[256];
    unsigned int i, colors, offset, tmp, planes;
    caca_file_t *f;

    f = caca_file_open(name, "rb");
    if(!f)
    {
        free(im);
        return NULL;
    }

    if(u16fread(f) != 0x4d42)
    {
        caca_file_close(f);
        free(im);
        return NULL;
    }

    u32fread(f); /* size */
    u16fread(f); /* reserved 1 */
    u16fread(f); /* reserved 2 */

    offset = u32fread(f);

    tmp = u32fread(f); /* header size */
    if(tmp == 40)
    {
        im->w = u32fread(f);
        im->h = u32fread(f);
        planes = u16fread(f);
        bpp = u16fread(f);

        tmp = u32fread(f); /* compression */
        if(tmp != 0)
        {
            caca_file_close(f);
            free(im);
            return NULL;
        }

        u32fread(f); /* sizeimage */
        u32fread(f); /* xpelspermeter */
        u32fread(f); /* ypelspermeter */
        u32fread(f); /* biclrused */
        u32fread(f); /* biclrimportantn */

        colors = (offset - 54) / 4;
        for(i = 0; i < colors && i < 256; i++)
        {
            blue[i] = u8fread(f) * 16;
            green[i] = u8fread(f) * 16;
            red[i] = u8fread(f) * 16;
            alpha[i] = 0;
            u8fread(f);
        }
    }
    else if(tmp == 12)
    {
        im->w = u32fread(f);
        im->h = u32fread(f);
        planes = u16fread(f);
        bpp = u16fread(f);

        colors = (offset - 26) / 3;
        for(i = 0; i < colors && i < 256; i++)
        {
            blue[i] = u8fread(f);
            green[i] = u8fread(f);
            red[i] = u8fread(f);
            alpha[i] = 0;
        }
    }
    else
    {
        caca_file_close(f);
        free(im);
        return NULL;
    }

    /* Fill the rest of the palette */
    for(i = colors; i < 256; i++)
        blue[i] = green[i] = red[i] = alpha[i] = 0;

    depth = (bpp + 7) / 8;

    /* Sanity check */
    if(!im->w || im->w > 0x10000 || !im->h || im->h > 0x10000 || planes != 1)
    {
        caca_file_close(f);
        free(im);
        return NULL;
    }

    /* Allocate the pixel buffer */
    im->pixels = malloc(im->w * im->h * depth);
    if(!im->pixels)
    {
        caca_file_close(f);
        free(im);
        return NULL;
    }

    memset(im->pixels, 0, im->w * im->h * depth);

    /* Read the bitmap data */
    for(i = im->h; i--; )
    {
        unsigned int j, k, bits = 0;

        switch(bpp)
        {
            case 1:
                for(j = 0; j < im->w; j++)
                {
                    k = j % 32;
                    if(k == 0)
                        bits = u32fread(f);
                    im->pixels[im->w * i * depth + j] =
                        (bits >> ((k & ~0xf) + 0xf - (k & 0xf))) & 0x1;
                }
                break;
            case 4:
                for(j = 0; j < im->w; j++)
                {
                    k = j % 8;
                    if(k == 0)
                        bits = u32fread(f);
                    im->pixels[im->w * i * depth + j] =
                        (bits >> (4 * ((k & ~0x1) + 0x1 - (k & 0x1)))) & 0xf;
                }
                break;
            default:
                /* Works for 8bpp, but also for 16, 24 etc. */
                caca_file_read(f, im->pixels + im->w * i * depth,
                                im->w * depth);
                /* Pad reads to 4 bytes */
                tmp = (im->w * depth) % 4;
                tmp = (4 - tmp) % 4;
                while(tmp--)
                    u8fread(f);
                break;
        }
    }

    switch(depth)
    {
    case 3:
        rmask = 0xff0000;
        gmask = 0x00ff00;
        bmask = 0x0000ff;
        amask = 0x000000;
        break;
    case 2: /* XXX: those are the 16 bits values */
        rmask = 0x7c00;
        gmask = 0x03e0;
        bmask = 0x001f;
        amask = 0x0000;
        break;
    case 1:
    default:
        bpp = 8;
        rmask = gmask = bmask = amask = 0;
        break;
    }

    caca_file_close(f);

    /* Create the libcaca dither */
    im->dither = caca_create_dither(bpp, im->w, im->h, depth * im->w,
                                     rmask, gmask, bmask, amask);
    if(!im->dither)
    {
        free(im->pixels);
        free(im);
        return NULL;
    }

    if(bpp == 8)
        caca_set_dither_palette(im->dither, red, green, blue, alpha);
#endif

    return im;
}

void unload_image(struct image * im)
{
#if defined(USE_IMLIB2)
    /* Imlib_Image image = (Imlib_Image)im->priv; */
    imlib_free_image();
#else
    free(im->pixels);
#endif
    caca_free_dither(im->dither);
}

#if !defined(USE_IMLIB2)
static unsigned int u32fread(caca_file_t * f)
{
    uint8_t buffer[4];
    caca_file_read(f, buffer, 4);
    return ((unsigned int)buffer[3] << 24) | ((unsigned int)buffer[2] << 16)
             | ((unsigned int)buffer[1] << 8) | ((unsigned int)buffer[0]);
}

static unsigned int u16fread(caca_file_t * f)
{
    uint8_t buffer[2];
    caca_file_read(f, buffer, 2);
    return ((unsigned int)buffer[1] << 8) | ((unsigned int)buffer[0]);
}

static unsigned int u8fread(caca_file_t * f)
{
    uint8_t buffer;
    caca_file_read(f, &buffer, 1);
    return (unsigned int)buffer;
}
#endif

