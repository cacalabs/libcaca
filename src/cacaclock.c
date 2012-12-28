/*
 *  cacaclock     Text-mode clock display
 *  Copyright (c) 2011-2012 Jean-Yves Lamoureux <jylam@lnxscene.org>
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
#   include <time.h>
#   include <stdio.h>
#   include <string.h>
#   include <stdlib.h>
#endif

#include "caca.h"

#define CACACLOCKVERSION "0.1"

static void usage(int argc, char **argv)
{
    fprintf(stderr, "Usage: %s [OPTIONS]...\n", argv[0]);
    fprintf(stderr, "Display current time in text mode     (q to quit)\n");
    fprintf(stderr, "Example : %s -d '%%R'\n\n", argv[0]);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h, --help\t\t\tThis help\n");
    fprintf(stderr, "  -v, --version\t\t\tVersion of the program\n");
    fprintf(stderr, "  -f, --font=FONT\t\tUse FONT for time display\n");
    fprintf(stderr, "  -d, --dateformat=FORMAT\tUse FORMAT as strftime argument (default %%R:%%S)\n");
}


static void version(void)
{
    printf(
            "cacaclock Copyright 2011-2012 Jean-Yves Lamoureux\n"
            "Internet: <jylam@lnxscene.org> Version: %s (libcaca %s), date: %s\n"
            "\n"
            "cacaclock, along with its documentation, may be freely copied and distributed.\n"
            "\n"
            "The latest version of cacaclock is available from the web site,\n"
            "        http://caca.zoy.org/wiki/libcaca in the libcaca package.\n"
            "\n",
            CACACLOCKVERSION, caca_get_version(), __DATE__);
}


static char* get_date(char *format) {
    time_t  currtime;
    char *charTime = malloc(101);

    time(&currtime);
    strftime(charTime, 100,format,localtime(&currtime));

    return charTime;
}

int main(int argc, char *argv[]) {

    caca_canvas_t *cv;
    caca_canvas_t *figcv;
    caca_display_t *dp;
    uint32_t w, h, fw, fh;

    char *format = "%R:%S";
    char *font   = "/usr/share/figlet/mono12.tlf";


    for(;;)
    {
        int option_index = 0;
        static struct caca_option long_options[] =
        {
            { "font",        1, NULL, 'f' },
            { "dateformat",  1, NULL, 'd' },
            { "help",        0, NULL, 'h' },
            { "version",     0, NULL, 'v' },
        };
        int c = caca_getopt(argc, argv, "f:d:hv",
                long_options, &option_index);
        if(c == -1)
            break;

        switch(c)
        {
            case 'h': /* --help       */
                usage(argc, argv);
                return 0;
                break;
            case 'v': /* --version    */
                version();
                return 0;
                break;
            case 'f': /* --font       */
                font = caca_optarg;
                break;
            case 'd': /* --dateformat */
                format = caca_optarg;
                break;
            default:
                return 1;
                break;
        }
    }



    cv = caca_create_canvas(0, 0);
    figcv = caca_create_canvas(0, 0);
    if(!cv || !figcv)
    {
        fprintf(stderr, "%s: unable to initialise libcaca\n", argv[0]);
        return 1;
    }

    if(caca_canvas_set_figfont(figcv, font))
    {
        fprintf(stderr, "Could not open font\n");
        return -1;
    }


    dp = caca_create_display(cv);
    if(!dp) {
        printf("Can't open window. CACA_DRIVER problem ?\n");
        return -1;
    }

    caca_set_color_ansi(figcv, CACA_DEFAULT, CACA_DEFAULT);
    caca_clear_canvas(cv);
    for(;;) {
        caca_event_t ev;

        while(caca_get_event(dp, CACA_EVENT_KEY_PRESS
                    | CACA_EVENT_QUIT, &ev, 1))
        {
            if(caca_get_event_type(&ev))
                goto end;
        }
        char *d = get_date(format);
        uint32_t o = 0;

        // figfont API is not complete, and does not allow us to put a string
        // at another position than 0,0
        // So, we have to create a canvas which will hold the figfont string,
        // then blit this canvas to the main one at the desired position.
        caca_clear_canvas(cv);
        caca_clear_canvas(figcv);
        while(d[o])
        {
            caca_put_figchar(figcv, d[o++]);
        }
        caca_flush_figlet (figcv);
        free(d);

        w = caca_get_canvas_width (cv);
        h = caca_get_canvas_height(cv);
        fw = caca_get_canvas_width (figcv);
        fh = caca_get_canvas_height(figcv);

        uint32_t x = (w/2) - (fw/2);
        uint32_t y = (h/2) - (fh/2);

        caca_blit(cv, x, y, figcv, NULL);
        caca_refresh_display(dp);
        usleep(250000);
    }
end:

    caca_free_canvas(figcv);
    caca_free_canvas(cv);
    caca_free_display(dp);

    return 0;
}
