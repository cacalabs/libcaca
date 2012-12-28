/*
 *  trifiller     texture mapping features
 *  Copyright (c) 2009-2012 Jean-Yves Lamoureux <jylam@lnxscene.org>
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
#endif

/* libcaca header */
#include "caca.h"

/* Image loading functions */
#include "../src/common-image.h"

/* M_PI / cos / sin */
#include <math.h>


#define SQUARE_SIZE 20

int main(int argc, char *argv[])
{

    /* libcaca/libcaca contexts */
    caca_canvas_t *cv;
    caca_display_t *dp;
    caca_canvas_t *tex;

    /* cached canvas size */
    int ww, wh, tw, th;

    /* logic */
    int quit = 0;
    int update = 1;
    int px, py;
    float angle = 0;


    float square[6][2] = {
        {-SQUARE_SIZE, -SQUARE_SIZE},
        {SQUARE_SIZE, -SQUARE_SIZE},
        {SQUARE_SIZE, SQUARE_SIZE},
        {-SQUARE_SIZE, SQUARE_SIZE},
    };
    float uv1[6] = {
        0, 0,
        1, 0,
        1, 1
    };
    float uv2[6] = {
        0, 0,
        1, 1,
        0, 1
    };


    float rotated[4][2];
    int coords1[6], coords2[6];

    /* Create displayed canvas */
    cv = caca_create_canvas(0, 0);
    if (!cv)
    {
        fprintf(stderr, "%s: unable to initialise libcaca\n", argv[0]);
        return 1;
    }

    /* Create texture holding canvas */
    tex = caca_create_canvas(16, 16);
    if (!tex)
    {
        fprintf(stderr, "%s: unable to initialise libcaca\n", argv[0]);
        return 1;
    }

    /* Open window */
    dp = caca_create_display(cv);
    if (!dp)
    {
        fprintf(stderr, "%s: unable to initialise libcaca\n", argv[0]);
        return 1;
    }



    /* Set the window title */
    caca_set_display_title(dp, "trifiller");

    /* Frame duration */
    caca_set_display_time(dp, 10000);

    /* Get displayed canvas size */
    ww = caca_get_canvas_width(cv);
    wh = caca_get_canvas_height(cv);

    /* Texture size */
    tw = caca_get_canvas_width(tex);
    th = caca_get_canvas_height(tex);

    /* Load texture if any */
    if (argc == 2)
    {
        struct image *im = load_image(argv[1]);
        if (!im)
        {
            fprintf(stderr, "%s: unable to load image '%s'\n", argv[0],
                    argv[1]);
            return 1;
        }

        caca_set_dither_algorithm(im->dither,
                                  caca_get_dither_algorithm_list(NULL)[4]);
        caca_dither_bitmap(tex, 0, 0, tw, th, im->dither, im->pixels);
        unload_image(im);
    }
    /* or generate one */
    else
    {

        int i;
        for (i = 0; i < 16; i++)
        {
            caca_set_color_ansi(tex, (i + 1) % 0xF, i % 0xF);
            caca_put_str(tex, 0, i, "0123456789ABCDEF");
        }
    }


    px = 0;
    py = 0;

    while (!quit)
    {
        caca_event_t ev;
        unsigned int const event_mask = CACA_EVENT_KEY_PRESS
            | CACA_EVENT_RESIZE | CACA_EVENT_QUIT;
        int event;

        if (update)
            event = caca_get_event(dp, event_mask, &ev, 0);
        else
            event = caca_get_event(dp, event_mask, &ev, -1);

        while (event)
        {
            if (caca_get_event_type(&ev) & CACA_EVENT_KEY_PRESS)
                switch (caca_get_event_key_ch(&ev))
                {
                case 'q':
                case 'Q':
                case CACA_KEY_ESCAPE:
                    quit = 1;
                    break;
                case CACA_KEY_UP:
                    py--;
                    break;
                case CACA_KEY_DOWN:
                    py++;
                    break;
                case CACA_KEY_LEFT:
                    px--;
                    break;
                case CACA_KEY_RIGHT:
                    px++;
                    break;
                case 'a':
                    angle += 1.0f;
                    break;
                case 's':
                    angle -= 1.0f;
                    break;
                }
            else if (caca_get_event_type(&ev) == CACA_EVENT_RESIZE)
            {
                caca_refresh_display(dp);
                ww = caca_get_event_resize_width(&ev);
                wh = caca_get_event_resize_height(&ev);
                update = 1;
            }
            else if (caca_get_event_type(&ev) & CACA_EVENT_QUIT)
                quit = 1;

            event = caca_get_event(dp, CACA_EVENT_KEY_PRESS, &ev, 0);
        }



        /* 2D Rotation around screen center */
        int p;
        for (p = 0; p < 4; p++)
        {
            rotated[p][0] =
                square[p][0] * cos(angle * M_PI / 180.0f) -
                square[p][1] * sin(angle * M_PI / 180.0f);
            rotated[p][1] =
                square[p][0] * sin(angle * M_PI / 180.0f) +
                square[p][1] * cos(angle * M_PI / 180.0f);

            rotated[p][0] += ww / 2 + px;
            rotated[p][1] += wh / 2 + py;
        }

        angle += 1.0f;


        /* Reaarange coordinates to fit libcaca's format */
        coords1[0] = rotated[0][0];
        coords1[1] = rotated[0][1];
        coords1[2] = rotated[1][0];
        coords1[3] = rotated[1][1];
        coords1[4] = rotated[2][0];
        coords1[5] = rotated[2][1];

        coords2[0] = rotated[0][0];
        coords2[1] = rotated[0][1];
        coords2[2] = rotated[2][0];
        coords2[3] = rotated[2][1];
        coords2[4] = rotated[3][0];
        coords2[5] = rotated[3][1];

        /* Display two triangles */
        caca_fill_triangle_textured(cv, /* canvas */
                                    coords1,    /* triangle coordinates */
                                    tex,        /* texture canvas */
                                    uv1);       /* texture coordinates */
        caca_fill_triangle_textured(cv, coords2, tex, uv2);

        /* Refresh display and clear for next frame */
        caca_refresh_display(dp);
        caca_clear_canvas(cv);

    }

    caca_free_display(dp);
    caca_free_canvas(cv);
    caca_free_canvas(tex);

    return 0;
}
