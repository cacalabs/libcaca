/*
 *  Imaging tools for cacaview and img2irc
 *  Copyright (c) 2003, 2004, 2005, 2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(HAVE_IMLIB2_H)
#   include <Imlib2.h>
#else
#   include <stdio.h>
#endif

#include "cucul.h"
#include "common-image.h"

#if !defined(HAVE_IMLIB2_H)
static int freadint(FILE *);
static int freadshort(FILE *);
static int freadchar(FILE *);
#endif

struct image * load_image(char const * name)
{
    struct image * im = malloc(sizeof(struct image));
    unsigned int depth, bpp, rmask, gmask, bmask, amask;

#if defined(HAVE_IMLIB2_H)
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

    /* Create the libcucul dither */
    im->dither = cucul_create_dither(bpp, im->w, im->h, depth * im->w,
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
    unsigned int red[256], green[256], blue[256], alpha[256];
    unsigned int i, colors, offset, tmp, planes;
    FILE *fp;

    fp = fopen(name, "rb");
    if(!fp)
    {
        free(im);
        return NULL;
    }

    if(freadshort(fp) != 0x4d42)
    {
        fclose(fp);
        free(im);
        return NULL;
    }

    freadint(fp); /* size */
    freadshort(fp); /* reserved 1 */
    freadshort(fp); /* reserved 2 */

    offset = freadint(fp);

    tmp = freadint(fp); /* header size */
    if(tmp == 40)
    {
        im->w = freadint(fp);
        im->h = freadint(fp);
        planes = freadshort(fp);
        bpp = freadshort(fp);

        tmp = freadint(fp); /* compression */
        if(tmp != 0)
        {
            fclose(fp);
            free(im);
            return NULL;
        }

        freadint(fp); /* sizeimage */
        freadint(fp); /* xpelspermeter */
        freadint(fp); /* ypelspermeter */
        freadint(fp); /* biclrused */
        freadint(fp); /* biclrimportantn */

        colors = (offset - 54) / 4;
        for(i = 0; i < colors && i < 256; i++)
        {
            blue[i] = freadchar(fp) * 16;
            green[i] = freadchar(fp) * 16;
            red[i] = freadchar(fp) * 16;
            alpha[i] = 0;
            freadchar(fp);
        }
    }
    else if(tmp == 12)
    {
        im->w = freadint(fp);
        im->h = freadint(fp);
        planes = freadshort(fp);
        bpp = freadshort(fp);

        colors = (offset - 26) / 3;
        for(i = 0; i < colors && i < 256; i++)
        {
            blue[i] = freadchar(fp);
            green[i] = freadchar(fp);
            red[i] = freadchar(fp);
            alpha[i] = 0;
        }
    }
    else
    {
        fclose(fp);
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
        fclose(fp);
        free(im);
        return NULL;
    }

    /* Allocate the pixel buffer */
    im->pixels = malloc(im->w * im->h * depth);
    if(!im->pixels)
    {
        fclose(fp);
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
                        bits = freadint(fp);
                    im->pixels[im->w * i * depth + j] =
                        (bits >> ((k & ~0xf) + 0xf - (k & 0xf))) & 0x1;
                }
                break;
            case 4:
                for(j = 0; j < im->w; j++)
                {
                    k = j % 8;
                    if(k == 0)
                        bits = freadint(fp);
                    im->pixels[im->w * i * depth + j] =
                        (bits >> (4 * ((k & ~0x1) + 0x1 - (k & 0x1)))) & 0xf;
                }
                break;
            default:
                /* Works for 8bpp, but also for 16, 24 etc. */
                fread(im->pixels + im->w * i * depth, im->w * depth, 1, fp);
                /* Pad reads to 4 bytes */
                tmp = (im->w * depth) % 4;
                tmp = (4 - tmp) % 4;
                while(tmp--)
                    freadchar(fp);
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

    fclose(fp);

    /* Create the libcucul dither */
    im->dither = cucul_create_dither(bpp, im->w, im->h, depth * im->w,
                                     rmask, gmask, bmask, amask);
    if(!im->dither)
    {
        free(im->pixels);
        free(im);
        return NULL;
    }

    if(bpp == 8)
        cucul_set_dither_palette(im->dither, red, green, blue, alpha);
#endif

    return im;
}

void unload_image(struct image * im)
{
#if defined(HAVE_IMLIB2_H)
    /* Imlib_Image image = (Imlib_Image)im->priv; */
    imlib_free_image();
#else
    free(im->pixels);
#endif
    cucul_free_dither(im->dither);
}

#if !defined(HAVE_IMLIB2_H)
static int freadint(FILE *fp)
{
    unsigned char buffer[4];
    fread(buffer, 4, 1, fp);
    return ((int)buffer[3] << 24) | ((int)buffer[2] << 16)
             | ((int)buffer[1] << 8) | ((int)buffer[0]);
}

static int freadshort(FILE *fp)
{
    unsigned char buffer[2];
    fread(buffer, 2, 1, fp);
    return ((int)buffer[1] << 8) | ((int)buffer[0]);
}

static int freadchar(FILE *fp)
{
    unsigned char buffer;
    fread(&buffer, 1, 1, fp);
    return (int)buffer;
}
#endif

