/*
 *  cacaview      image viewer for libcaca
 *  Copyright (c) 2003-2006 Sam Hocevar <sam@zoy.org>
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

#if defined(HAVE_SLEEP)
#   include <windows.h>
#endif

#include "cucul.h"
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

/* libcucul/libcaca contexts */
cucul_canvas_t *cv; caca_display_t *dp;

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
    int quit = 0, update = 1, help = 0, status = 0;
    int reload = 0;

    char **list = NULL;
    int current = 0, items = 0, opts = 1;
    int i;

    /* Initialise libcucul */
    cv = cucul_create_canvas(0, 0);
    if(!cv)
    {
        fprintf(stderr, "%s: unable to initialise libcucul\n", argv[0]);
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

    ww = cucul_get_canvas_width(cv);
    wh = cucul_get_canvas_height(cv);

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
                                      | CACA_EVENT_MOUSE_PRESS;
        unsigned int new_status = 0, new_help = 0;
        int event;

        if(update)
            event = caca_get_event(dp, event_mask, &ev, 0);
        else
            event = caca_get_event(dp, event_mask, &ev, -1);

        while(event)
        {
            if(ev.type & CACA_EVENT_MOUSE_PRESS)
            {
                if(ev.data.mouse.button == 1)
                {
                    if(items) current = (current + 1) % items;
                    reload = 1;
                }
                else if(ev.data.mouse.button == 2)
                {
                    if(items) current = (items + current - 1) % items;
                    reload = 1;
                }
            }
            else if(ev.type & CACA_EVENT_KEY_PRESS) switch(ev.data.key.ch)
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
                fullscreen = ~fullscreen;
                update = 1;
                set_zoom(zoom);
                break;
#if 0 /* FIXME */
            case 'b':
                i = 1 + cucul_get_feature(cv, CUCUL_BACKGROUND);
                if(i > CUCUL_BACKGROUND_MAX) i = CUCUL_BACKGROUND_MIN;
                cucul_set_feature(cv, i);
                new_status = STATUS_BACKGROUND;
                update = 1;
                break;
            case 'B':
                i = -1 + cucul_get_feature(cv, CUCUL_BACKGROUND);
                if(i < CUCUL_BACKGROUND_MIN) i = CUCUL_BACKGROUND_MAX;
                cucul_set_feature(cv, i);
                new_status = STATUS_BACKGROUND;
                update = 1;
                break;
            case 'a':
                i = 1 + cucul_get_feature(cv, CUCUL_ANTIALIASING);
                if(i > CUCUL_ANTIALIASING_MAX) i = CUCUL_ANTIALIASING_MIN;
                cucul_set_feature(cv, i);
                new_status = STATUS_ANTIALIASING;
                update = 1;
                break;
            case 'A':
                i = -1 + cucul_get_feature(cv, CUCUL_ANTIALIASING);
                if(i < CUCUL_ANTIALIASING_MIN) i = CUCUL_ANTIALIASING_MAX;
                cucul_set_feature(cv, i);
                new_status = STATUS_ANTIALIASING;
                update = 1;
                break;
            case 'd':
                i = 1 + cucul_get_feature(cv, CUCUL_DITHERING);
                if(i > CUCUL_DITHERING_MAX) i = CUCUL_DITHERING_MIN;
                cucul_set_feature(cv, i);
                new_status = STATUS_DITHERING;
                update = 1;
                break;
            case 'D':
                i = -1 + cucul_get_feature(cv, CUCUL_DITHERING);
                if(i < CUCUL_DITHERING_MIN) i = CUCUL_DITHERING_MAX;
                cucul_set_feature(cv, i);
                new_status = STATUS_DITHERING;
                update = 1;
                break;
#endif
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
                quit = 1;
                break;
            }
            else if(ev.type == CACA_EVENT_RESIZE)
            {
                caca_refresh_display(dp);
                ww = ev.data.resize.w;
                wh = ev.data.resize.h;
                update = 1;
                set_zoom(zoom);
            }

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
            cucul_set_color(cv, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
            cucul_putstr(cv, (ww - strlen(buffer)) / 2, wh / 2, buffer);
            caca_refresh_display(dp);
            ww = cucul_get_canvas_width(cv);
            wh = cucul_get_canvas_height(cv);

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

        cucul_set_color(cv, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLACK);
        cucul_clear_canvas(cv);

        if(!items)
        {
            cucul_set_color(cv, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
            cucul_printf(cv, ww / 2 - 5, wh / 2, " No image. ");
        }
        else if(!im)
        {
#if defined(HAVE_IMLIB2_H)
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
            cucul_set_color(cv, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
            cucul_putstr(cv, (ww - strlen(buffer)) / 2, wh / 2, buffer);
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

            cucul_dither_bitmap(cv, ww * (1.0 - xfactor) * xdelta,
                            y + height * (1.0 - yfactor) * ydelta,
                            ww * xfactor + 1, height * yfactor + 1,
                            im->dither, im->pixels);
        }

        if(!fullscreen)
        {
            print_status();

#if 0 /* FIXME */
            cucul_set_color(cv, CUCUL_COLOR_LIGHTGRAY, CUCUL_COLOR_BLACK);
            switch(status)
            {
                case STATUS_ANTIALIASING:
                    cucul_printf(cv, 0, wh - 1, "Antialiasing: %s",
                  cucul_get_feature_name(cucul_get_feature(cv, CUCUL_ANTIALIASING)));
                    break;
                case STATUS_DITHERING:
                    cucul_printf(cv, 0, wh - 1, "Dithering: %s",
                  cucul_get_feature_name(cucul_get_feature(cv, CUCUL_DITHERING)));
                    break;
                case STATUS_BACKGROUND:
                    cucul_printf(cv, 0, wh - 1, "Background: %s",
                  cucul_get_feature_name(cucul_get_feature(cv, CUCUL_BACKGROUND)));
                    break;
            }
#endif
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
    cucul_free_canvas(cv);

    return 0;
}

static void print_status(void)
{
    cucul_set_color(cv, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
    cucul_draw_line(cv, 0, 0, ww - 1, 0, " ");
    cucul_draw_line(cv, 0, wh - 2, ww - 1, wh - 2, "-");
    cucul_putstr(cv, 0, 0, "q:Quit  np:Next/Prev  +-x:Zoom  gG:Gamma  "
                           "hjkl:Move  d:Dither  a:Antialias");
    cucul_putstr(cv, ww - strlen("?:Help"), 0, "?:Help");
    cucul_printf(cv, 3, wh - 2, "cacaview %s", VERSION);
    cucul_printf(cv, ww - 30, wh - 2, "(gamma: %#.3g)", GAMMA(g));
    cucul_printf(cv, ww - 14, wh - 2, "(zoom: %s%i)", zoom > 0 ? "+" : "", zoom);

    cucul_set_color(cv, CUCUL_COLOR_LIGHTGRAY, CUCUL_COLOR_BLACK);
    cucul_draw_line(cv, 0, wh - 1, ww - 1, wh - 1, " ");
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

    cucul_set_color(cv, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);

    for(i = 0; help[i]; i++)
        cucul_putstr(cv, x, y + i, help[i]);
}

static void set_zoom(int new_zoom)
{
    int height;

    if(!im)
        return;

    zoom = new_zoom;

    if(zoom > ZOOM_MAX) zoom = ZOOM_MAX;
    if(zoom < -ZOOM_MAX) zoom = -ZOOM_MAX;

    ww = cucul_get_canvas_width(cv);
    height = fullscreen ? wh : wh - 3;

    xfactor = (zoom < 0) ? 1.0 / zoomtab[-zoom] : zoomtab[zoom];
    yfactor = xfactor * ww / height * im->h / im->w
               * cucul_get_canvas_height(cv) / cucul_get_canvas_width(cv)
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

    cucul_set_dither_gamma(im->dither,
                           (g < 0) ? 1.0 / gammatab[-g] : gammatab[g]);
}

static void draw_checkers(int x, int y, int w, int h)
{
    int xn, yn;

    if(x + w > (int)cucul_get_canvas_width(cv))
        w = cucul_get_canvas_width(cv) - x;
    if(y + h > (int)cucul_get_canvas_height(cv))
        h = cucul_get_canvas_height(cv) - y;

    for(yn = y > 0 ? y : 0; yn < y + h; yn++)
        for(xn = x > 0 ? x : 0; xn < x + w; xn++)
    {
        if((((xn - x) / 5) ^ ((yn - y) / 3)) & 1)
            cucul_set_color(cv, CUCUL_COLOR_LIGHTGRAY, CUCUL_COLOR_DARKGRAY);
        else
            cucul_set_color(cv, CUCUL_COLOR_DARKGRAY, CUCUL_COLOR_LIGHTGRAY);
        cucul_putchar(cv, xn, yn, ' ');
    }
}

