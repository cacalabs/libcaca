/*
 *  libcucul++    C++ bindings for libcucul
 *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *  This file contains the main functions used by \e libcucul++ applications
 *  to initialise a drawing context.
 */

#include "config.h"

#include <stdio.h> // BUFSIZ
#include <stdarg.h> // va_*

#include "cucul++.h"

Cucul::Cucul() 
{
    cv = cucul_create_canvas(0, 0);
    if(!cv)
        throw -1;
}

Cucul::Cucul(int width, int height) 
{
    cv = cucul_create_canvas(width, height);
    if(!cv) throw -1;
}

Cucul::~Cucul() 
{
    if(cv)
        cucul_free_canvas(cv);
}

cucul_canvas_t *Cucul::get_cucul_canvas_t()
{
    return cv;
}

void Cucul::set_size(unsigned int width, unsigned int height) 
{
    cucul_set_canvas_size(cv, width, height);
}

unsigned int Cucul::get_width(void) 
{
    return cucul_get_canvas_width(cv);
}

unsigned int Cucul::get_height(void) 
{
    return cucul_get_canvas_height(cv);
}

void Cucul::set_color(unsigned int f, unsigned int b) 
{
    cucul_set_color(cv, f, b);
}

char const * Cucul::get_color_name(unsigned int color) 
{
    return cucul_get_color_name(color);
}

void Cucul::putchar(int x, int y, char ch)
{
    cucul_putchar(cv, x, y, ch);
}

void Cucul::putstr(int x, int y, char *str) 
{
    cucul_putstr(cv, x, y, str);
}

void Cucul::printf(int x, int y, char const * format,...)
{
    char tmp[BUFSIZ];
    char *buf = tmp;
    va_list args;

    va_start(args, format);
#if defined(HAVE_VSNPRINTF)
    vsnprintf(buf, get_width() - x + 1, format, args);
#else
    vsprintf(buf, format, args);
#endif
    buf[get_width() - x] = '\0';
    va_end(args);

    putstr(x, y, buf);
}

void Cucul::clear(void)
{
    cucul_clear_canvas(cv);
}

void Cucul::blit(int x, int y, Cucul* c1, Cucul* c2)
{
    cucul_blit(cv, x, y, c1->get_cucul_canvas_t(), c2->get_cucul_canvas_t());
}

void Cucul::invert()
{
    cucul_invert(cv);
}

void Cucul::flip()
{
    cucul_flip(cv);
}

void Cucul::flop()
{
    cucul_flop(cv);
}

void Cucul::rotate()
{
    cucul_rotate(cv);
}

void Cucul::draw_line(int x1, int y1, int x2, int y2, char const *ch)
{
    cucul_draw_line(cv, x1, y1, x2, y2, ch);
}
 
void Cucul::draw_polyline(int const x[], int const y[], int f, char const *ch)
{
    cucul_draw_polyline(cv, x, y, f, ch);
}
 
void Cucul::draw_thin_line(int x1, int y1, int x2, int y2)
{
    cucul_draw_thin_line(cv, x1, y1, x2, y2);
}

void Cucul::draw_thin_polyline(int const x[], int const y[], int f)
{
    cucul_draw_thin_polyline(cv, x, y, f);
}
 
void Cucul::draw_circle(int x, int y, int d, char const *ch)
{
    cucul_draw_circle(cv, x, y, d, ch);
}

void Cucul::draw_ellipse(int x, int y, int d1, int d2, char const *ch)
{
    cucul_draw_ellipse(cv, x, y, d1, d2, ch);
}

void Cucul::draw_thin_ellipse(int x, int y, int d1, int d2)
{
    cucul_draw_thin_ellipse(cv, x, y, d1, d2);
}

void Cucul::fill_ellipse(int x, int y, int d1, int d2, char const *ch)
{
    cucul_fill_ellipse(cv, x, y, d1, d2, ch);
}

void Cucul::draw_box(int x, int y, int w, int h, char const *ch)
{
    cucul_draw_box(cv, x, y, w, h, ch);
}
 
void Cucul::draw_thin_box(int x, int y, int w, int h)
{
    cucul_draw_thin_box(cv, x, y, w, h);
}

void Cucul::fill_box(int x, int y, int w, int h, char const *ch)
{
    cucul_fill_box(cv, x, y, w, h, ch);
}

void Cucul::draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, char const *ch)
{
    cucul_draw_triangle(cv, x1, y1, x2, y2, x3, y3, ch);
}

void Cucul::draw_thin_triangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
    cucul_draw_thin_triangle(cv, x1, y1, x2, y2, x3, y3);
}

void Cucul::fill_triangle(int x1, int y1, int x2, int y2, int x3, int y3, const char *ch)
{
    cucul_fill_triangle(cv, x1, y1, x2, y2, x3, y3, ch);
}

int Cucul::rand(int min, int max)
{
    return cucul_rand(min, max);
}

Cucul::Dither * Cucul::create_dither(unsigned int v1, unsigned int v2, unsigned int v3, unsigned int v4, unsigned int v5, unsigned int v6, unsigned int v7, unsigned int v8)
{
    Cucul::Dither *d = new Dither();
    d->dither = cucul_create_dither(v1, v2, v3, v4, v5, v6, v7, v8);
    return d;
}

void Cucul::set_dither_palette(Cucul::Dither *d, unsigned int r[], unsigned int g[], unsigned int b[], unsigned int a[])
{
    cucul_set_dither_palette(d->dither, r, g, b, a);
}

void Cucul::set_dither_brightness(Cucul::Dither *d, float f)
{
    cucul_set_dither_brightness(d->dither, f);
}

void Cucul::set_dither_gamma(Cucul::Dither *d, float f)
{
    cucul_set_dither_gamma(d->dither, f);
}

void Cucul::set_dither_contrast(Cucul::Dither *d, float f)
{
    cucul_set_dither_contrast(d->dither, f);
}

void Cucul::set_dither_invert(Cucul::Dither *d, int i)
{
    cucul_set_dither_invert(d->dither, i);
}

void Cucul::set_dither_antialias(Cucul::Dither *d, char const *cv)
{
    cucul_set_dither_antialias(d->dither, cv);
}
 
char const *const * Cucul::get_dither_antialias_list(Cucul::Dither const *d)
{
    return cucul_get_dither_antialias_list(d->dither);
}

void Cucul::set_dither_color(Cucul::Dither *d, char const *cv)
{
    cucul_set_dither_color(d->dither, cv);
}

char const *const * Cucul::get_dither_color_list(Cucul::Dither const *d)
{
    return cucul_get_dither_color_list(d->dither);
}
 
void Cucul::set_dither_charset(Cucul::Dither *d, char const *cv)
{
    cucul_set_dither_charset(d->dither, cv);
}

char const *const * Cucul::get_dither_charset_list(Cucul::Dither const *d)
{
    return cucul_get_dither_charset_list(d->dither);
}
 
void Cucul::set_dither_mode(Cucul::Dither *d, char const *cv)
{
    cucul_set_dither_mode(d->dither, cv);
}

char const *const * Cucul::get_dither_mode_list(Cucul::Dither const *d)
{
    return cucul_get_dither_mode_list(d->dither);
}

void Cucul::dither_bitmap(int x, int y, int w, int h,  Cucul::Dither const *d, void *v)
{
    cucul_dither_bitmap(cv, x, y, w, h, d->dither, v);
}

void Cucul::free_dither(Cucul::Dither *d)
{
    cucul_free_dither(d->dither);
}

Cucul::Font * Cucul::load_font(void const *s, unsigned int v)
{
    Cucul::Font *f = new Cucul::Font();
    f->font = cucul_load_font(s, v);
    return f;
}

char const *const * Cucul::get_font_list(void)
{
    return cucul_get_font_list();
}

unsigned int Cucul::get_font_width(Cucul::Font *f)
{
    return cucul_get_font_width(f->font);
}

unsigned int Cucul::get_font_height(Cucul::Font *f)
{
    return cucul_get_font_height(f->font);
}

void Cucul::render_canvas(Cucul::Font *f, unsigned char *buf, unsigned int x, unsigned int y, unsigned int w)
{
    cucul_render_canvas(cv, f->font, buf, x, y, w);
}

void Cucul::free_font(Cucul::Font *f)
{
    cucul_free_font(f->font);
}

Cucul::Buffer * Cucul::export_canvas(char const *buf)
{
    Cucul::Buffer *b = new Cucul::Buffer();
    b->buffer = cucul_export_canvas(cv, buf);
    return b;
}

char const *const * Cucul::get_export_list(void)
{
    return cucul_get_export_list();
}
