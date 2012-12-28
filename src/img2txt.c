/*
 *  img2txt       image to text converter
 *  Copyright (c) 2006-2012 Sam Hocevar <sam@hocevar.net>
 *                2007 Jean-Yves Lamoureux <jylam@lnxscene.org>
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
#   include <stdio.h>
#   include <string.h>
#   include <stdlib.h>
#endif

#include "caca.h"

#include "common-image.h"

#define IMG2TXTVERSION "0.1"

static void usage(int argc, char **argv)
{
    char const * const * list;

    fprintf(stderr, "Usage: %s [OPTIONS]... <IMAGE>\n", argv[0]);
    fprintf(stderr, "Convert IMAGE to any text based available format.\n");
    fprintf(stderr, "Example : %s -w 80 -f ansi ./caca.png\n\n", argv[0]);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h, --help\t\t\tThis help\n");
    fprintf(stderr, "  -v, --version\t\t\tVersion of the program\n");
    fprintf(stderr, "  -W, --width=WIDTH\t\tWidth of resulting image\n");
    fprintf(stderr, "  -H, --height=HEIGHT\t\tHeight of resulting image\n");
    fprintf(stderr, "  -x, --font-width=WIDTH\t\tWidth of output font\n");
    fprintf(stderr, "  -y, --font-height=HEIGHT\t\tHeight of output font\n");
    fprintf(stderr, "  -b, --brightness=BRIGHTNESS\tBrightness of resulting image\n");
    fprintf(stderr, "  -c, --contrast=CONTRAST\tContrast of resulting image\n");
    fprintf(stderr, "  -g, --gamma=GAMMA\t\tGamma of resulting image\n");
    fprintf(stderr, "  -d, --dither=DITHER\t\tDithering algorithm to use :\n");
    list = caca_get_dither_algorithm_list(NULL);
    while(*list)
    {
        fprintf(stderr, "\t\t\t%s: %s\n", list[0], list[1]);
        list += 2;
    }

    fprintf(stderr, "  -f, --format=FORMAT\t\tFormat of the resulting image :\n");
    list = caca_get_export_list();
    while(*list)
    {
        fprintf(stderr, "\t\t\t%s: %s\n", list[0], list[1]);
        list += 2;
    }

#if !defined(USE_IMLIB2)
    fprintf(stderr, "NOTE: This program has NOT been built with Imlib2 support. Only BMP loading is supported.\n");
#endif
}

static void version(void)
{
    printf(
    "img2txt Copyright 2006-2007 Sam Hocevar and Jean-Yves Lamoureux\n"
    "Internet: <sam@hocevar.net> <jylam@lnxscene.org> Version: %s, date: %s\n"
    "\n"
    "img2txt, along with its documentation, may be freely copied and distributed.\n"
    "\n"
    "The latest version of img2txt is available from the web site,\n"
    "        http://caca.zoy.org/wiki/libcaca in the libcaca package.\n"
    "\n",
    caca_get_version(), __DATE__);
}
int main(int argc, char **argv)
{
    /* libcaca context */
    caca_canvas_t *cv;
    void *export;
    size_t len;
    struct image *i;
    unsigned int cols = 0, lines = 0, font_width = 6, font_height = 10;
    char *format = NULL;
    char *dither = NULL;
    float gamma = -1, brightness = -1, contrast = -1;

    if(argc < 2)
    {
        fprintf(stderr, "%s: wrong argument count\n", argv[0]);
        usage(argc, argv);
        return 1;
    }

    for(;;)
    {
        int option_index = 0;
        static struct caca_option long_options[] =
        {
            { "width",       1, NULL, 'W' },
            { "height",      1, NULL, 'H' },
            { "font-width",  1, NULL, 'x' },
            { "font-height", 1, NULL, 'y' },
            { "format",      1, NULL, 'f' },
            { "dither",      1, NULL, 'd' },
            { "gamma",       1, NULL, 'g' },
            { "brightness",  1, NULL, 'b' },
            { "contrast",    1, NULL, 'c' },
            { "help",        0, NULL, 'h' },
            { "version",     0, NULL, 'v' },
        };
        int c = caca_getopt(argc, argv, "W:H:f:d:g:b:c:hvx:y:",
                            long_options, &option_index);
        if(c == -1)
            break;

        switch(c)
        {
        case 'W': /* --width */
            cols = atoi(caca_optarg);
            break;
        case 'H': /* --height */
            lines = atoi(caca_optarg);
            break;
        case 'x': /* --width */
            font_width = atoi(caca_optarg);
            break;
        case 'y': /* --height */
            font_height = atoi(caca_optarg);
            break;
        case 'f': /* --format */
            format = caca_optarg;
            break;
        case 'd': /* --dither */
            dither = caca_optarg;
            break;
        case 'g': /* --gamma */
            gamma = atof(caca_optarg);
            break;
        case 'b': /* --brightness */
            brightness = atof(caca_optarg);
            break;
        case 'c': /* --contrast */
            contrast = atof(caca_optarg);
            break;
        case 'h': /* --help */
            usage(argc, argv);
            return 0;
            break;
        case 'v': /* --version */
            version();
            return 0;
            break;
        default:
            return 1;
            break;
        }
    }


    cv = caca_create_canvas(0, 0);
    if(!cv)
    {
        fprintf(stderr, "%s: unable to initialise libcaca\n", argv[0]);
        return 1;
    }

    i = load_image(argv[argc-1]);
    if(!i)
    {
        fprintf(stderr, "%s: unable to load %s\n", argv[0], argv[argc-1]);
        caca_free_canvas(cv);
        return 1;
    }

    /* Assume a 6×10 font */
    if(!cols && !lines)
    {
        cols = 60;
        lines = cols * i->h * font_width / i->w / font_height;
    }
    else if(cols && !lines)
    {
        lines = cols * i->h * font_width / i->w / font_height;
    }
    else if(!cols && lines)
    {
        cols = lines * i->w * font_height / i->h / font_width;
    }


    caca_set_canvas_size(cv, cols, lines);
    caca_set_color_ansi(cv, CACA_DEFAULT, CACA_TRANSPARENT);
    caca_clear_canvas(cv);
    if(caca_set_dither_algorithm(i->dither, dither?dither:"fstein"))
    {
        fprintf(stderr, "%s: Can't dither image with algorithm '%s'\n", argv[0], dither);
        unload_image(i);
        caca_free_canvas(cv);
        return -1;
    }

    if(brightness!=-1) caca_set_dither_brightness (i->dither, brightness);
    if(contrast!=-1) caca_set_dither_contrast (i->dither, contrast);
    if(gamma!=-1) caca_set_dither_gamma (i->dither, gamma);

    caca_dither_bitmap(cv, 0, 0, cols, lines, i->dither, i->pixels);

    unload_image(i);

    export = caca_export_canvas_to_memory(cv, format?format:"ansi", &len);
    if(!export)
    {
        fprintf(stderr, "%s: Can't export to format '%s'\n", argv[0], format);
    }
    else
    {
        fwrite(export, len, 1, stdout);
        free(export);
    }

    caca_free_canvas(cv);

    return 0;
}

