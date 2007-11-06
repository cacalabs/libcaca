/*
 *  pic2irc       image to IRC converter
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
 *                2007 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "config.h"
#include "common.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <string.h>
#   include <stdlib.h>
#endif

#if !defined HAVE_GETOPT_LONG
#   include "mygetopt.h"
#elif defined HAVE_GETOPT_H
#   include <getopt.h>
#endif
#if defined HAVE_GETOPT_LONG
#   define mygetopt getopt_long
#   define myoptind optind
#   define myoptarg optarg
#   define myoption option
#endif

#include "cucul.h"
#include "common-image.h"

static void usage(int argc, char **argv)
{
    fprintf(stderr, "Usage: %s [OPTIONS]... <IMAGE>\n", argv[0]);
    fprintf(stderr, "Convert IMAGE to any text based available format.\n");
    fprintf(stderr, "Example : %s -w 80 -f ansi ./caca.png\n\n", argv[0]);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h, --help\tThis help\n");
    fprintf(stderr, "  -W, --width=WIDTH\tWidth of resulting image\n");
    fprintf(stderr, "  -H, --HEIGHT=HEIGHT\tHeight of resulting image\n");
    fprintf(stderr, "  -f, --format=FORMAT\tFormat of the resulting image :\n");
    fprintf(stderr, "\t\t\tansi : coulored ANSI\n");
    fprintf(stderr, "\t\t\tcaca : internal libcaca format\n");
    fprintf(stderr, "\t\t\tutf8 : UTF8 with CR\n");
    fprintf(stderr, "\t\t\tutf8 : UTF8 with CRLF (MS Windows)\n");
    fprintf(stderr, "\t\t\thtml : HTML with CSS and DIV support\n");
    fprintf(stderr, "\t\t\thtml : Pure HTML3 with tables\n");
    fprintf(stderr, "\t\t\tirc  : IRC with ctrl-k codes\n");
    fprintf(stderr, "\t\t\tps   : Postscript\n");
    fprintf(stderr, "\t\t\tsvg  : Scalable Vector Graphics\n");
    fprintf(stderr, "\t\t\ttga  : Targa Image\n\n");
}

int main(int argc, char **argv)
{
    /* libcucul context */
    cucul_canvas_t *cv;
    void *export;
    unsigned long int len;
    struct image *i;
    unsigned int cols = 0, lines = 0;
    char *format = NULL;


    if(argc < 2)
    {
        fprintf(stderr, "%s: wrong argument count\n", argv[0]);
        usage(argc, argv);
        return 1;
    }

    for(;;)
    {
        int option_index = 0;
        static struct myoption long_options[] =
        {
            { "width",  1, NULL, 'W' },
            { "height", 1, NULL, 'H' },
            { "format", 1, NULL, 'f' },
            { "help",   0, NULL, 'h' },
        };
        int c = mygetopt(argc, argv, "W:H:f:h", long_options, &option_index);
        if(c == -1)
            break;

        switch(c)
        {
        case 'W': /* --width */
            cols = atoi(myoptarg);
            break;
        case 'H': /* --height */
            lines = atoi(myoptarg);
            break;
        case 'f': /* --help */
            format = myoptarg;
            break;
        case 'h': /* --help */
            usage(argc, argv);
            return 0;
            break;
        default:
            return 1;
            break;
        }
    }



    cv = cucul_create_canvas(0, 0);
    if(!cv)
    {
        fprintf(stderr, "%s: unable to initialise libcucul\n", argv[0]);
        return 1;
    }

    i = load_image(argv[argc-1]);
    if(!i)
    {
        fprintf(stderr, "%s: unable to load %s\n", argv[0], argv[1]);
        cucul_free_canvas(cv);
        return 1;
    }

    /* Assume a 6×10 font */
    if(!cols && !lines)
    {
        cols = 60;
        lines = cols * i->h * 6 / i->w / 10;
    }
    else if(cols && !lines)
    {
        lines = cols * i->h * 6 / i->w / 10;
    }
    else if(!cols && lines)
    {
        cols = lines * i->w * 10 / i->h / 6;
    }


    cucul_set_canvas_size(cv, cols, lines);
    cucul_set_color_ansi(cv, CUCUL_DEFAULT, CUCUL_TRANSPARENT);
    cucul_clear_canvas(cv);
    cucul_dither_bitmap(cv, 0, 0, cols, lines, i->dither, i->pixels);

    unload_image(i);

    export = cucul_export_memory(cv, format?format:"ansi", &len);
    if(!export)
    {
        fprintf(stderr, "Can't export to format '%s'\n", format);
    }
    else
    {
        fwrite(export, len, 1, stdout);
        free(export);
    }

    cucul_free_canvas(cv);

    return 0;
}

