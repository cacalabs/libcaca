/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains triangle drawing functions, both filled and outline.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#endif

#include "caca.h"
#include "caca_internals.h"

/** \brief Draw a triangle on the canvas using the given character.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcaca canvas.
 *  \param x1 X coordinate of the first point.
 *  \param y1 Y coordinate of the first point.
 *  \param x2 X coordinate of the second point.
 *  \param y2 Y coordinate of the second point.
 *  \param x3 X coordinate of the third point.
 *  \param y3 Y coordinate of the third point.
 *  \param ch UTF-32 character to be used to draw the triangle outline.
 *  \return This function always returns 0.
 */
int caca_draw_triangle(caca_canvas_t * cv, int x1, int y1, int x2, int y2,
                       int x3, int y3, uint32_t ch)
{
    caca_draw_line(cv, x1, y1, x2, y2, ch);
    caca_draw_line(cv, x2, y2, x3, y3, ch);
    caca_draw_line(cv, x3, y3, x1, y1, ch);

    return 0;
}

/** \brief Draw a thin triangle on the canvas.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcaca canvas.
 *  \param x1 X coordinate of the first point.
 *  \param y1 Y coordinate of the first point.
 *  \param x2 X coordinate of the second point.
 *  \param y2 Y coordinate of the second point.
 *  \param x3 X coordinate of the third point.
 *  \param y3 Y coordinate of the third point.
 *  \return This function always returns 0.
 */
int caca_draw_thin_triangle(caca_canvas_t * cv, int x1, int y1,
                            int x2, int y2, int x3, int y3)
{
    caca_draw_thin_line(cv, x1, y1, x2, y2);
    caca_draw_thin_line(cv, x2, y2, x3, y3);
    caca_draw_thin_line(cv, x3, y3, x1, y1);

    return 0;
}

/** \brief Fill a triangle on the canvas using the given character.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcaca canvas.
 *  \param x1 X coordinate of the first point.
 *  \param y1 Y coordinate of the first point.
 *  \param x2 X coordinate of the second point.
 *  \param y2 Y coordinate of the second point.
 *  \param x3 X coordinate of the third point.
 *  \param y3 Y coordinate of the third point.
 *  \param ch UTF-32 character to be used to fill the triangle.
 *  \return This function always returns 0.
 */
int caca_fill_triangle(caca_canvas_t * cv, int x1, int y1, int x2, int y2,
                       int x3, int y3, uint32_t ch)
{
    int x, y, xmin, xmax, ymin, ymax;
    int xx1, xx2, xa, xb, sl21, sl31, sl32;

    /* Bubble-sort y1 <= y2 <= y3 */
    if (y1 > y2)
        return caca_fill_triangle(cv, x2, y2, x1, y1, x3, y3, ch);

    if (y2 > y3)
        return caca_fill_triangle(cv, x1, y1, x3, y3, x2, y2, ch);

    /* Compute slopes and promote precision */
    sl21 = (y2 == y1) ? 0 : (x2 - x1) * 0x10000 / (y2 - y1);
    sl31 = (y3 == y1) ? 0 : (x3 - x1) * 0x10000 / (y3 - y1);
    sl32 = (y3 == y2) ? 0 : (x3 - x2) * 0x10000 / (y3 - y2);

    x1 *= 0x10000;
    x2 *= 0x10000;
    x3 *= 0x10000;

    ymin = y1 < 0 ? 0 : y1;
    ymax = y3 + 1 < cv->height ? y3 + 1 : cv->height;

    if (ymin < y2)
    {
        xa = x1 + sl21 * (ymin - y1);
        xb = x1 + sl31 * (ymin - y1);
    }
    else if (ymin == y2)
    {
        xa = x2;
        xb = (y1 == y3) ? x3 : x1 + sl31 * (ymin - y1);
    }
    else                        /* (ymin > y2) */
    {
        xa = x3 + sl32 * (ymin - y3);
        xb = x3 + sl31 * (ymin - y3);
    }

    /* Rasterize our triangle */
    for (y = ymin; y < ymax; y++)
    {
        /* Rescale xa and xb, recentering the division */
        if (xa < xb)
        {
            xx1 = (xa + 0x800) / 0x10000;
            xx2 = (xb + 0x801) / 0x10000;
        }
        else
        {
            xx1 = (xb + 0x800) / 0x10000;
            xx2 = (xa + 0x801) / 0x10000;
        }

        xmin = xx1 < 0 ? 0 : xx1;
        xmax = xx2 + 1 < cv->width ? xx2 + 1 : cv->width;

        for (x = xmin; x < xmax; x++)
            caca_put_char(cv, x, y, ch);

        xa += y < y2 ? sl21 : sl32;
        xb += sl31;
    }

    return 0;
}

/* This function actually renders the triangle, but is not exported due to
   sam's pedantic will. */
static int caca_fill_triangle_textured_l(caca_canvas_t * cv,
                                         int x1, int y1,
                                         int x2, int y2,
                                         int x3, int y3,
                                         caca_canvas_t * tex,
                                         float u1, float v1,
                                         float u2, float v2,
                                         float u3, float v3)
{
    float y2y1, y3y1, y3y2;
    float sl12, sl13, sl23;
    float usl12, usl13, usl23, vsl12, vsl13, vsl23;
    float xa, xb, ua, va, ub, vb, u, v;
    uint32_t savedattr;
    int tw, th, x, y, s;

#define SWAP_F(a, b) {float c = a; a = b; b = c; }

    /* (very) Naive and (very) float-based affine and (very) non-clipped and
       (very) non-corrected triangle mapper Accepts arbitrary texture sizes
       Coordinates clamped to [0.0 - 1.0] (no repeat) */
    if (!cv || !tex)
        return -1;

    /* Bubble-sort y1 <= y2 <= y3 */
    if (y1 > y2)
        return caca_fill_triangle_textured_l(cv,
                                             x2, y2, x1, y1, x3, y3,
                                             tex, u2, v2, u1, v1, u3, v3);
    if (y2 > y3)
        return caca_fill_triangle_textured_l(cv,
                                             x1, y1, x3, y3, x2, y2,
                                             tex, u1, v1, u3, v3, u2, v2);

    savedattr = caca_get_attr(cv, -1, -1);

    /* Clip texture coordinates */
    if (u1 < 0.0f) u1 = 0.0f; else if (u1 > 1.0f) u1 = 1.0f;
    if (u2 < 0.0f) u2 = 0.0f; else if (u2 > 1.0f) u2 = 1.0f;
    if (u3 < 0.0f) u3 = 0.0f; else if (u3 > 1.0f) u3 = 1.0f;
    if (v1 < 0.0f) v1 = 0.0f; else if (v1 > 1.0f) v1 = 1.0f;
    if (v2 < 0.0f) v2 = 0.0f; else if (v2 > 1.0f) v2 = 1.0f;
    if (v3 < 0.0f) v3 = 0.0f; else if (v3 > 1.0f) v3 = 1.0f;

    /* Convert relative tex coordinates to absolute */
    tw = caca_get_canvas_width(tex);
    th = caca_get_canvas_height(tex);

    u1 *= (float)tw;
    u2 *= (float)tw;
    u3 *= (float)tw;
    v1 *= (float)th;
    v2 *= (float)th;
    v3 *= (float)th;

    y2y1 = (float)(y2 - y1);
    y3y1 = (float)(y3 - y1);
    y3y2 = (float)(y3 - y2);

    /* Compute slopes, making sure we don't divide by zero */
    /* (in this case, we don't need the value anyway) */
    /* FIXME : only compute needed slopes */
    sl12 = ((float)x2 - x1) / (y2y1 == 0 ? 1 : y2y1);
    sl13 = ((float)x3 - x1) / (y3y1 == 0 ? 1 : y3y1);
    sl23 = ((float)x3 - x2) / (y3y2 == 0 ? 1 : y3y2);

    usl12 = (u2 - u1) / (y2y1 == 0 ? 1 : y2y1);
    usl13 = (u3 - u1) / (y3y1 == 0 ? 1 : y3y1);
    usl23 = (u3 - u2) / (y3y2 == 0 ? 1 : y3y2);
    vsl12 = (v2 - v1) / (y2y1 == 0 ? 1 : y2y1);
    vsl13 = (v3 - v1) / (y3y1 == 0 ? 1 : y3y1);
    vsl23 = (v3 - v2) / (y3y2 == 0 ? 1 : y3y2);

    xa = (float)x1;
    xb = (float)x1;
    ua = u1; ub = u1;
    va = v1; vb = v1;

    s = 0;

    /* Top */
    for (y = y1; y < y2; y++)
    {
        float tus, tvs;

        if (xb < xa)
        {
            SWAP_F(xb, xa);
            SWAP_F(sl13, sl12);
            SWAP_F(ua, ub);
            SWAP_F(va, vb);
            SWAP_F(usl13, usl12);
            SWAP_F(vsl13, vsl12);
            s = 1;
        }

        tus = (ub - ua) / (xb - xa);
        tvs = (vb - va) / (xb - xa);
        v = va;
        u = ua;

        /* scanline */
        for (x = xa; x < xb; x++)
        {
            uint32_t attr, c;
            u += tus;
            v += tvs;
            /* FIXME: use caca_get_canvas_attrs / caca_get_canvas_chars */
            attr = caca_get_attr(tex, u, v);
            c = caca_get_char(tex, u, v);
            caca_set_attr(cv, attr);
            caca_put_char(cv, x, y, c);
        }

        xa += sl13;
        xb += sl12;

        ua += usl13;
        va += vsl13;
        ub += usl12;
        vb += vsl12;
    }

    if (s)
    {
        SWAP_F(xb, xa);
        SWAP_F(sl13, sl12);
        SWAP_F(ua, ub);
        SWAP_F(va, vb);
        SWAP_F(usl13, usl12);
        SWAP_F(vsl13, vsl12);
    }


    /* Bottom */
    xb = (float)x2;

    /* These variables are set by 'top' routine and are in an incorrect state
       if we only draw the bottom part */
    if (y1 == y2)
    {
        ua = u1;
        ub = u2;
        va = v1;
        vb = v2;
    }

    for (y = y2; y < y3; y++)
    {
        float tus, tvs;

        if (xb <= xa)
        {
            SWAP_F(xb, xa);
            SWAP_F(sl13, sl23);
            SWAP_F(ua, ub);
            SWAP_F(va, vb);
            SWAP_F(usl13, usl23);
            SWAP_F(vsl13, vsl23);
        }

        tus = (ub - ua) / ((float)xb - xa);
        tvs = (vb - va) / ((float)xb - xa);
        u = ua;
        v = va;

        /* scanline */
        for (x = xa; x < xb; x++)
        {
            uint32_t attr, c;
            u += tus;
            v += tvs;
            /* FIXME, can be heavily optimised */
            attr = caca_get_attr(tex, u, v);
            c = caca_get_char(tex, u, v);
            caca_set_attr(cv, attr);
            caca_put_char(cv, x, y, c);
        }

        xa += sl13;
        xb += sl23;

        ua += usl13;
        va += vsl13;
        ub += usl23;
        vb += vsl23;
    }

    caca_set_attr(cv, savedattr);

    return 0;
}

/** \brief Fill a triangle on the canvas using an arbitrary-sized texture.
 *
 *  This function fails if one or both the canvas are missing
 *
 *  \param cv     The handle to the libcaca canvas.
 *  \param coords The coordinates of the triangle (3{x,y})
 *  \param tex    The handle of the canvas texture.
 *  \param uv     The coordinates of the texture (3{u,v})
 *  \return This function return 0 if ok, -1 if canvas or texture are missing.
 */
int caca_fill_triangle_textured(caca_canvas_t * cv,
                                int coords[6],
                                caca_canvas_t * tex, float uv[6])
{

    return caca_fill_triangle_textured_l(cv,
                                         coords[0], coords[1],
                                         coords[2], coords[3],
                                         coords[4], coords[5],
                                         tex,
                                         uv[0], uv[1],
                                         uv[2], uv[3], uv[4], uv[5]);
}



/*
 * XXX: The following functions are aliases.
 */

int cucul_draw_triangle(cucul_canvas_t *, int, int, int, int, int,
                        int, uint32_t) CACA_ALIAS(caca_draw_triangle);
     int cucul_draw_thin_triangle(cucul_canvas_t *, int, int, int, int,
                                  int,
                                  int) CACA_ALIAS(caca_draw_thin_triangle);
     int cucul_fill_triangle(cucul_canvas_t *, int, int, int, int, int, int,
                             uint32_t) CACA_ALIAS(caca_fill_triangle);
