/*
 *  cacaview      image viewer for libcaca
 *  Copyright (c) 2003-2012 Sam Hocevar <sam@hocevar.net>
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

#if defined(HAVE_SLEEP)
#   include <windows.h>
#endif

#include "caca.h"

#include "common-image.h"

/* Local macros */
#define MODE_IMAGE 1
#define MODE_FILES 2

#define STATUS_DITHERING 1
#define STATUS_ANTIALIASING 2
#define STATUS_BACKGROUND 3

#define ZOOM_FACTOR 1.08f
#define ZOOM_MAX 50
#define GAMMA_FACTOR 1.04f
#define GAMMA_MAX 100
#define GAMMA(g) (((g) < 0) ? 1.0 / gammatab[-(g)] : gammatab[(g)])
#define PAD_STEP 0.15

/* libcaca/libcaca contexts */
caca_canvas_t *cv; caca_display_t *dp;

/* Local functions */
static void print_status(void);
static void print_help(int, int);
static void set_zoom(int);
static void set_gamma(int);
static void draw_checkers(int, int, int, int);

/* Local variables */
struct image *im = NULL;

float zoomtab[ZOOM_MAX + 1];
float gammatab[GAMMA_MAX + 1];
float xfactor = 1.0, yfactor = 1.0, dx = 0.5, dy = 0.5;
int zoom = 0, g = 0, fullscreen = 0, mode, ww, wh;

int main(int argc, char **argv)
{
    char const * const * algos = caca_get_dither_algorithm_list(NULL);
    int dither_algorithm = 0;

    int quit = 0, update = 1, help = 0, status = 0;
    int reload = 0;

    char **list = NULL;
    int current = 0, items = 0, opts = 1;
    int i;

    /* Initialise libcaca */
    cv = caca_create_canvas(0, 0);
    if(!cv)
    {
        fprintf(stderr, "%s: unable to initialise libcaca\n", argv[0]);
        return 1;
    }

    dp = caca_create_display(cv);
    if(!dp)
    {
        fprintf(stderr, "%s: unable to initialise libcaca\n", argv[0]);
        return 1;
    }

    /* Set the window title */
    caca_set_display_title(dp, "cacaview");

    ww = caca_get_canvas_width(cv);
    wh = caca_get_canvas_height(cv);

    /* Fill the zoom table */
    zoomtab[0] = 1.0;
    for(i = 0; i < ZOOM_MAX; i++)
        zoomtab[i + 1] = zoomtab[i] * ZOOM_FACTOR;

    /* Fill the gamma table */
    gammatab[0] = 1.0;
    for(i = 0; i < GAMMA_MAX; i++)
        gammatab[i + 1] = gammatab[i] * GAMMA_FACTOR;

    /* Load items into playlist */
    for(i = 1; i < argc; i++)
    {
        /* Skip options except after `--' */
        if(opts && argv[i][0] == '-')
        {
            if(argv[i][1] == '-' && argv[i][2] == '\0')
                opts = 0;
            continue;
        }

        /* Add argv[i] to the list */
        if(items)
            list = realloc(list, (items + 1) * sizeof(char *));
        else
            list = malloc(sizeof(char *));
        list[items] = argv[i];
        items++;

        reload = 1;
    }

    /* Go ! */
    while(!quit)
    {
        caca_event_t ev;
        unsigned int const event_mask = CACA_EVENT_KEY_PRESS
                                      | CACA_EVENT_RESIZE
                                      | CACA_EVENT_MOUSE_PRESS
                                      | CACA_EVENT_QUIT;
        unsigned int new_status = 0, new_help = 0;
        int event;

        if(update)
            event = caca_get_event(dp, event_mask, &ev, 0);
        else
            event = caca_get_event(dp, event_mask, &ev, -1);

        while(event)
        {
            if(caca_get_event_type(&ev) & CACA_EVENT_MOUSE_PRESS)
            {
                if(caca_get_event_mouse_button(&ev) == 1)
                {
                    if(items) current = (current + 1) % items;
                    reload = 1;
                }
                if(caca_get_event_mouse_button(&ev) == 2)
                {
                    if(items) current = (items + current - 1) % items;
                    reload = 1;
                }
            }
            else if(caca_get_event_type(&ev) & CACA_EVENT_KEY_PRESS)
                switch(caca_get_event_key_ch(&ev))
            {
            case 'n':
            case 'N':
                if(items) current = (current + 1) % items;
                reload = 1;
                break;
            case 'p':
            case 'P':
                if(items) current = (items + current - 1) % items;
                reload = 1;
                break;
            case 'f':
            case 'F':
            case CACA_KEY_F11:
                fullscreen = ~fullscreen;
                update = 1;
                set_zoom(zoom);
                break;
#if 0 /* FIXME */
            case 'b':
                i = 1 + caca_get_feature(cv, CACA_BACKGROUND);
                if(i > CACA_BACKGROUND_MAX) i = CACA_BACKGROUND_MIN;
                caca_set_feature(cv, i);
                new_status = STATUS_BACKGROUND;
                update = 1;
                break;
            case 'B':
                i = -1 + caca_get_feature(cv, CACA_BACKGROUND);
                if(i < CACA_BACKGROUND_MIN) i = CACA_BACKGROUND_MAX;
                caca_set_feature(cv, i);
                new_status = STATUS_BACKGROUND;
                update = 1;
                break;
            case 'a':
                i = 1 + caca_get_feature(cv, CACA_ANTIALIASING);
                if(i > CACA_ANTIALIASING_MAX) i = CACA_ANTIALIASING_MIN;
                caca_set_feature(cv, i);
                new_status = STATUS_ANTIALIASING;
                update = 1;
                break;
            case 'A':
                i = -1 + caca_get_feature(cv, CACA_ANTIALIASING);
                if(i < CACA_ANTIALIASING_MIN) i = CACA_ANTIALIASING_MAX;
                caca_set_feature(cv, i);
                new_status = STATUS_ANTIALIASING;
                update = 1;
                break;
#endif
            case 'd':
                dither_algorithm++;
                if(algos[dither_algorithm * 2] == NULL)
                    dither_algorithm = 0;
                caca_set_dither_algorithm(im->dither,
                                           algos[dither_algorithm * 2]);
                new_status = STATUS_DITHERING;
                update = 1;
                break;
            case 'D':
                dither_algorithm--;
                if(dither_algorithm < 0)
                    while(algos[dither_algorithm * 2 + 2] != NULL)
                        dither_algorithm++;
                caca_set_dither_algorithm(im->dither,
                                           algos[dither_algorithm * 2]);
                new_status = STATUS_DITHERING;
                update = 1;
                break;
            case '+':
                update = 1;
                set_zoom(zoom + 1);
                break;
            case '-':
                update = 1;
                set_zoom(zoom - 1);
                break;
            case 'G':
                update = 1;
                set_gamma(g + 1);
                break;
            case 'g':
                update = 1;
                set_gamma(g - 1);
                break;
            case 'x':
            case 'X':
                update = 1;
                set_zoom(0);
                set_gamma(0);
                break;
            case 'k':
            case 'K':
            case CACA_KEY_UP:
                if(yfactor > 1.0) dy -= PAD_STEP / yfactor;
                if(dy < 0.0) dy = 0.0;
                update = 1;
                break;
            case 'j':
            case 'J':
            case CACA_KEY_DOWN:
                if(yfactor > 1.0) dy += PAD_STEP / yfactor;
                if(dy > 1.0) dy = 1.0;
                update = 1;
                break;
            case 'h':
            case 'H':
            case CACA_KEY_LEFT:
                if(xfactor > 1.0) dx -= PAD_STEP / xfactor;
                if(dx < 0.0) dx = 0.0;
                update = 1;
                break;
            case 'l':
            case 'L':
            case CACA_KEY_RIGHT:
                if(xfactor > 1.0) dx += PAD_STEP / xfactor;
                if(dx > 1.0) dx = 1.0;
                update = 1;
                break;
            case '?':
                new_help = !help;
                update = 1;
                break;
            case 'q':
            case 'Q':
            case CACA_KEY_ESCAPE:
                quit = 1;
                break;
            }
            else if(caca_get_event_type(&ev) == CACA_EVENT_RESIZE)
            {
                caca_refresh_display(dp);
                ww = caca_get_event_resize_width(&ev);
                wh = caca_get_event_resize_height(&ev);
                update = 1;
                set_zoom(zoom);
            }
            else if(caca_get_event_type(&ev) & CACA_EVENT_QUIT)
                quit = 1;

            if(status || new_status)
                status = new_status;

            if(help || new_help)
                help = new_help;

            event = caca_get_event(dp, CACA_EVENT_KEY_PRESS, &ev, 0);
        }

        if(items && reload)
        {
            char *buffer;
            int len = strlen(" Loading `%s'... ") + strlen(list[current]);

            if(len < ww + 1)
                len = ww + 1;

            buffer = malloc(len);

            sprintf(buffer, " Loading `%s'... ", list[current]);
            buffer[ww] = '\0';
            caca_set_color_ansi(cv, CACA_WHITE, CACA_BLUE);
            caca_put_str(cv, (ww - strlen(buffer)) / 2, wh / 2, buffer);
            caca_refresh_display(dp);
            ww = caca_get_canvas_width(cv);
            wh = caca_get_canvas_height(cv);

            if(im)
                unload_image(im);
            im = load_image(list[current]);
            reload = 0;

            /* Reset image-specific runtime variables */
            dx = dy = 0.5;
            update = 1;
            set_zoom(0);
            set_gamma(0);

            free(buffer);
        }

        caca_set_color_ansi(cv, CACA_WHITE, CACA_BLACK);
        caca_clear_canvas(cv);

        if(!items)
        {
            caca_set_color_ansi(cv, CACA_WHITE, CACA_BLUE);
            caca_printf(cv, ww / 2 - 5, wh / 2, " No image. ");
        }
        else if(!im)
        {
#if defined(USE_IMLIB2)
#   define ERROR_STRING " Error loading `%s'. "
#else
#   define ERROR_STRING " Error loading `%s'. Only BMP is supported. "
#endif
            char *buffer;
            int len = strlen(ERROR_STRING) + strlen(list[current]);

            if(len < ww + 1)
                len = ww + 1;

            buffer = malloc(len);

            sprintf(buffer, ERROR_STRING, list[current]);
            buffer[ww] = '\0';
            caca_set_color_ansi(cv, CACA_WHITE, CACA_BLUE);
            caca_put_str(cv, (ww - strlen(buffer)) / 2, wh / 2, buffer);
            free(buffer);
        }
        else
        {
            float xdelta, ydelta;
            int y, height;

            y = fullscreen ? 0 : 1;
            height = fullscreen ? wh : wh - 3;

            xdelta = (xfactor > 1.0) ? dx : 0.5;
            ydelta = (yfactor > 1.0) ? dy : 0.5;

            draw_checkers(ww * (1.0 - xfactor) / 2,
                          y + height * (1.0 - yfactor) / 2,
                          ww * xfactor, height * yfactor);

            caca_dither_bitmap(cv, ww * (1.0 - xfactor) * xdelta,
                            y + height * (1.0 - yfactor) * ydelta,
                            ww * xfactor + 1, height * yfactor + 1,
                            im->dither, im->pixels);
        }

        if(!fullscreen)
        {
            print_status();

            caca_set_color_ansi(cv, CACA_LIGHTGRAY, CACA_BLACK);
            switch(status)
            {
                case STATUS_DITHERING:
                    caca_printf(cv, 0, wh - 1, "Dithering: %s",
                                 caca_get_dither_algorithm(im->dither));
                    break;
#if 0 /* FIXME */
                case STATUS_ANTIALIASING:
                    caca_printf(cv, 0, wh - 1, "Antialiasing: %s",
                  caca_get_feature_name(caca_get_feature(cv, CACA_ANTIALIASING)));
                    break;
                case STATUS_BACKGROUND:
                    caca_printf(cv, 0, wh - 1, "Background: %s",
                  caca_get_feature_name(caca_get_feature(cv, CACA_BACKGROUND)));
                    break;
#endif
            }
        }

        if(help)
        {
            print_help(ww - 26, 2);
        }

        caca_refresh_display(dp);
        update = 0;
    }

    /* Clean up */
    if(im)
        unload_image(im);
    caca_free_display(dp);
    caca_free_canvas(cv);

    return 0;
}

static void print_status(void)
{
    caca_set_color_ansi(cv, CACA_WHITE, CACA_BLUE);
    caca_draw_line(cv, 0, 0, ww - 1, 0, ' ');
    caca_draw_line(cv, 0, wh - 2, ww - 1, wh - 2, '-');
    caca_put_str(cv, 0, 0, "q:Quit  np:Next/Prev  +-x:Zoom  gG:Gamma  "
                            "hjkl:Move  d:Dither  a:Antialias");
    caca_put_str(cv, ww - strlen("?:Help"), 0, "?:Help");
    caca_printf(cv, 3, wh - 2, "cacaview %s", PACKAGE_VERSION);
    caca_printf(cv, ww - 30, wh - 2, "(gamma: %#.3g)", GAMMA(g));
    caca_printf(cv, ww - 14, wh - 2, "(zoom: %s%i)", zoom > 0 ? "+" : "", zoom);

    caca_set_color_ansi(cv, CACA_LIGHTGRAY, CACA_BLACK);
    caca_draw_line(cv, 0, wh - 1, ww - 1, wh - 1, ' ');
}

static void print_help(int x, int y)
{
    static char const *help[] =
    {
        " +: zoom in              ",
        " -: zoom out             ",
        " g: decrease gamma       ",
        " G: increase gamma       ",
        " x: reset zoom and gamma ",
        " ----------------------- ",
        " hjkl: move view         ",
        " arrows: move view       ",
        " ----------------------- ",
        " a: antialiasing method  ",
        " d: dithering method     ",
        " b: background mode      ",
        " ----------------------- ",
        " ?: help                 ",
        " q: quit                 ",
        NULL
    };

    int i;

    caca_set_color_ansi(cv, CACA_WHITE, CACA_BLUE);

    for(i = 0; help[i]; i++)
        caca_put_str(cv, x, y + i, help[i]);
}

static void set_zoom(int new_zoom)
{
    int height;

    if(!im)
        return;

    zoom = new_zoom;

    if(zoom > ZOOM_MAX) zoom = ZOOM_MAX;
    if(zoom < -ZOOM_MAX) zoom = -ZOOM_MAX;

    ww = caca_get_canvas_width(cv);
    height = fullscreen ? wh : wh - 3;

    xfactor = (zoom < 0) ? 1.0 / zoomtab[-zoom] : zoomtab[zoom];
    yfactor = xfactor * ww / height * im->h / im->w
               * caca_get_canvas_height(cv) / caca_get_canvas_width(cv)
               * caca_get_display_width(dp) / caca_get_display_height(dp);

    if(yfactor > xfactor)
    {
        float tmp = xfactor;
        xfactor = tmp * tmp / yfactor;
        yfactor = tmp;
    }
}

static void set_gamma(int new_gamma)
{
    if(!im)
        return;

    g = new_gamma;

    if(g > GAMMA_MAX) g = GAMMA_MAX;
    if(g < -GAMMA_MAX) g = -GAMMA_MAX;

    caca_set_dither_gamma(im->dither,
                           (g < 0) ? 1.0 / gammatab[-g] : gammatab[g]);
}

static void draw_checkers(int x, int y, int w, int h)
{
    int xn, yn;

    if(x + w > (int)caca_get_canvas_width(cv))
        w = caca_get_canvas_width(cv) - x;
    if(y + h > (int)caca_get_canvas_height(cv))
        h = caca_get_canvas_height(cv) - y;

    for(yn = y > 0 ? y : 0; yn < y + h; yn++)
        for(xn = x > 0 ? x : 0; xn < x + w; xn++)
    {
        if((((xn - x) / 5) ^ ((yn - y) / 3)) & 1)
            caca_set_color_ansi(cv, CACA_LIGHTGRAY, CACA_DARKGRAY);
        else
            caca_set_color_ansi(cv, CACA_DARKGRAY, CACA_LIGHTGRAY);
        caca_put_char(cv, xn, yn, ' ');
    }
}

