/*
 *  cacaplas      plasma effect for libcaca
 *  Copyright (c) 2004 Sam Hocevar <sam@zoy.org>
 *                1998 Bini Michele <mibin@tin.it>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA
 */

#include "config.h"

#include <math.h>

#ifndef M_PI
#   define M_PI 3.14159265358979323846
#endif

#include "caca.h"

/* Virtual buffer size */
#define XSIZ 256
#define YSIZ 256

#define TABLEX (XSIZ * 2)
#define TABLEY (YSIZ * 2)

static unsigned char screen[XSIZ * YSIZ];
static unsigned char table[TABLEX * TABLEY];

static void do_plasma(unsigned char *,
                      double, double, double, double, double, double);

int main (int argc, char **argv)
{
    int red[256], green[256], blue[256], alpha[256];
    double r[3], R[6];
    struct caca_bitmap *bitmap;
    int i, x, y, frame;

    if(caca_init() < 0)
        return 1;

    caca_set_delay(10000);

    /* Fill various tables */
    for(i = 0 ; i < 256; i++)
        red[i] = green[i] = blue[i] = alpha[i] = 0;

    for(i = 0; i < 3; i++)
        r[i] = (double)(caca_rand(1, 1000)) / 30000 * M_PI;

    for(i = 0; i < 6; i++)
        R[i] = (double)(caca_rand(1, 1000)) / 5000;

    for(y = 0 ; y < TABLEY ; y++)
        for(x = 0 ; x < TABLEX ; x++)
    {
        double tmp = (((double)((x - (TABLEX / 2)) * (x - (TABLEX / 2))
                              + (y - (TABLEX / 2)) * (y - (TABLEX / 2))))
                      * (M_PI / (TABLEX * TABLEX + TABLEY * TABLEY)));

        table[x + y * TABLEX] = (1.0 + sin(12.0 * sqrt(tmp))) * 256 / 6;
    }

    /* Create a libcaca bitmap */
    bitmap = caca_create_bitmap(8, XSIZ, YSIZ, XSIZ, 0, 0, 0, 0);

    /* Main loop */
    for(frame = 0; !caca_get_event(CACA_EVENT_KEY_PRESS); frame++)
    {
        for(i = 0 ; i < 256; i++)
        {
            double z = ((double)i) / 256 * 6 * M_PI;

            red[i] = (1.0 + cos(z + r[0] * frame)) / 2 * 0xfff;
            green[i] = (1.0 + sin(z + r[1] * frame)) / 2 * 0xfff;
            blue[i] = (1.0 + cos(z + r[2] * frame)) / 2 * 0xfff;
        }

        /* Set the palette */
        caca_set_bitmap_palette(bitmap, red, green, blue, alpha);

        do_plasma(screen,
                  (1.0 + sin(((double)frame) * R[0])) / 2,
                  (1.0 + sin(((double)frame) * R[1])) / 2,
                  (1.0 + sin(((double)frame) * R[2])) / 2,
                  (1.0 + sin(((double)frame) * R[3])) / 2,
                  (1.0 + sin(((double)frame) * R[4])) / 2,
                  (1.0 + sin(((double)frame) * R[5])) / 2);

        caca_draw_bitmap(0, 0, caca_get_width() - 1, caca_get_height() - 1,
                         bitmap, screen);
        caca_refresh();
    }

    caca_free_bitmap(bitmap);
    caca_end();

    return 0;
}

static void do_plasma(unsigned char *pixels, double x_1, double y_1,
                      double x_2, double y_2, double x_3, double y_3)
{
    unsigned int X1 = x_1 * (TABLEX / 2),
                 Y1 = y_1 * (TABLEY / 2),
                 X2 = x_2 * (TABLEX / 2),
                 Y2 = y_2 * (TABLEY / 2),
                 X3 = x_3 * (TABLEX / 2),
                 Y3 = y_3 * (TABLEY / 2);
    unsigned int y;
    unsigned char * t1 = table + X1 + Y1 * TABLEX,
                  * t2 = table + X2 + Y2 * TABLEX,
                  * t3 = table + X3 + Y3 * TABLEX;

    for(y = 0; y < YSIZ; y++)
    {
        unsigned int x;
        unsigned char * tmp = pixels + y * YSIZ;
        unsigned int ty = y * TABLEX, tmax = ty + XSIZ;
        for(x = 0; ty < tmax; ty++, tmp++)
            tmp[0] = t1[ty] + t2[ty] + t3[ty];
    }
}

