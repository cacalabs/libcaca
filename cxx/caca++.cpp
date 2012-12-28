/*
 *  libcaca++     C++ bindings for libcaca
 *  Copyright (c) 2006-2007 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                2009-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains the main functions used by \e libcaca++ applications to
 *  initialise the library, get the screen properties, set the framerate and
 *  so on.
 */

#include "config.h"

#include <iostream>

#include <stdio.h> // BUFSIZ
#include <stdarg.h> // va_*

#include "caca++.h"

uint32_t Charset::utf8ToUtf32(char const *s, size_t *read)
{
    return caca_utf8_to_utf32(s, read);
}
size_t Charset::utf32ToUtf8(char *buf, uint32_t ch)
{
    return caca_utf32_to_utf8(buf, ch);
}
uint8_t Charset::utf32ToCp437(uint32_t ch)
{
    return caca_utf32_to_cp437(ch);
}
uint32_t Charset::cp437ToUtf32(uint8_t ch)
{
    return caca_cp437_to_utf32(ch);
}


Canvas::Canvas()
{
    cv = caca_create_canvas(0, 0);
    if(!cv)
        throw -1;
}

Canvas::Canvas(int width, int height)
{
    cv = caca_create_canvas(width, height);
    if(!cv) throw -1;
}

Canvas::~Canvas()
{
    if(cv)
        caca_free_canvas(cv);
}

caca_canvas_t *Canvas::get_caca_canvas_t()
{
    return cv;
}

void Canvas::setSize(unsigned int width, unsigned int height)
{
    caca_set_canvas_size(cv, width, height);
}

unsigned int Canvas::getWidth(void)
{
    return caca_get_canvas_width(cv);
}

unsigned int Canvas::getHeight(void)
{
    return caca_get_canvas_height(cv);
}

int Canvas::setColorANSI(uint8_t f, uint8_t b)
{
    return caca_set_color_ansi(cv, f, b);
}

int  Canvas::setColorARGB(unsigned int f, unsigned int b)
{
    return caca_set_color_argb(cv, f, b);
}

void Canvas::putChar(int x, int y, uint32_t ch)
{
    caca_put_char(cv, x, y, ch);
}

uint32_t Canvas::getChar(int x, int y)
{
    return caca_get_char(cv, x, y);
}

void Canvas::putStr(int x, int y, char *str)
{
    caca_put_str(cv, x, y, str);
}

void Canvas::Printf(int x, int y, char const * format, ...)
{
    char tmp[BUFSIZ];
    char *buf = tmp;
    va_list args;

    va_start(args, format);
#if defined(HAVE_VSNPRINTF)
    vsnprintf(buf, getWidth() - x + 1, format, args);
#else
    vsprintf(buf, format, args);
#endif
    buf[getWidth() - x] = '\0';
    va_end(args);

    putStr(x, y, buf);
}

void Canvas::Clear(void)
{
    caca_clear_canvas(cv);
}

void Canvas::Blit(int x, int y, Canvas* c1, Canvas* c2)
{
    caca_blit(cv, x, y, c1->get_caca_canvas_t(),
                         c2 ? c2->get_caca_canvas_t() : NULL);
}

void Canvas::Invert()
{
    caca_invert(cv);
}

void Canvas::Flip()
{
    caca_flip(cv);
}

void Canvas::Flop()
{
    caca_flop(cv);
}

void Canvas::Rotate180()
{
    caca_rotate_180(cv);
}

void Canvas::RotateLeft()
{
    caca_rotate_left(cv);
}

void Canvas::RotateRight()
{
    caca_rotate_right(cv);
}

void Canvas::drawLine(int x1, int y1, int x2, int y2, uint32_t ch)
{
    caca_draw_line(cv, x1, y1, x2, y2, ch);
}

void Canvas::drawPolyline(int const x[], int const y[], int f, uint32_t ch)
{
    caca_draw_polyline(cv, x, y, f, ch);
}

void Canvas::drawThinLine(int x1, int y1, int x2, int y2)
{
    caca_draw_thin_line(cv, x1, y1, x2, y2);
}

void Canvas::drawThinPolyline(int const x[], int const y[], int f)
{
    caca_draw_thin_polyline(cv, x, y, f);
}

void Canvas::drawCircle(int x, int y, int d, uint32_t ch)
{
    caca_draw_circle(cv, x, y, d, ch);
}

void Canvas::drawEllipse(int x, int y, int d1, int d2, uint32_t ch)
{
    caca_draw_ellipse(cv, x, y, d1, d2, ch);
}

void Canvas::drawThinEllipse(int x, int y, int d1, int d2)
{
    caca_draw_thin_ellipse(cv, x, y, d1, d2);
}

void Canvas::fillEllipse(int x, int y, int d1, int d2, uint32_t ch)
{
    caca_fill_ellipse(cv, x, y, d1, d2, ch);
}

void Canvas::drawBox(int x, int y, int w, int h, uint32_t ch)
{
    caca_draw_box(cv, x, y, w, h, ch);
}

void Canvas::drawThinBox(int x, int y, int w, int h)
{
    caca_draw_thin_box(cv, x, y, w, h);
}

void Canvas::drawCP437Box(int x, int y, int w, int h)
{
    caca_draw_cp437_box(cv, x, y, w, h);
}

void Canvas::fillBox(int x, int y, int w, int h, uint32_t ch)
{
    caca_fill_box(cv, x, y, w, h, ch);
}

void Canvas::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t ch)
{
    caca_draw_triangle(cv, x1, y1, x2, y2, x3, y3, ch);
}

void Canvas::drawThinTriangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
    caca_draw_thin_triangle(cv, x1, y1, x2, y2, x3, y3);
}

void Canvas::fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t ch)
{
    caca_fill_triangle(cv, x1, y1, x2, y2, x3, y3, ch);
}

void Canvas::fillTriangleTextured(int coords[6], Canvas *tex, float uv[6])
{
    caca_fill_triangle_textured(cv, coords, tex->cv, uv);
}

int Canvas::Rand(int min, int max)
{
    return caca_rand(min, max);
}

const char * Canvas::getVersion()
{
    return caca_get_version();
}

int Canvas::setAttr(uint32_t attr)
{
    return caca_set_attr(cv, attr);
}

uint32_t Canvas::getAttr(int x, int y)
{
    return caca_get_attr(cv, x, y);
}

int Canvas::setBoundaries(caca_canvas_t *, int x, int y,
                         unsigned int w, unsigned int h)
{
    return caca_set_canvas_boundaries(cv, x, y, h, w);
}

unsigned int Canvas::getFrameCount()
{
    return caca_get_frame_count(cv);
}
int Canvas::setFrame(unsigned int f)
{
    return caca_set_frame(cv, f);
}
int Canvas::createFrame(unsigned int f)
{
    return caca_create_frame(cv, f);
}
int Canvas::freeFrame(unsigned int f)
{
    return caca_create_frame(cv, f);
}

char const *const * Canvas::getImportList(void)
{
    return caca_get_import_list();
}

long int Canvas::importFromMemory(void const *buf, size_t len, char const *fmt)
{
    return caca_import_canvas_from_memory(cv, buf, len, fmt);
}

long int Canvas::importFromFile(char const *file, char const *fmt)
{
    return caca_import_canvas_from_file(cv, file, fmt);
}

char const *const * Canvas::getExportList(void)
{
    return caca_get_export_list();
}

void *Canvas::exportToMemory(char const *fmt, size_t *len)
{
    return caca_export_canvas_to_memory(cv, fmt, len);
}

Dither::Dither(unsigned int v1, unsigned int v2, unsigned int v3, unsigned int v4, unsigned int v5, unsigned int v6, unsigned int v7, unsigned int v8)
{
    dither = caca_create_dither(v1, v2, v3, v4, v5, v6, v7, v8);
}
Dither::~Dither()
{
    caca_free_dither(dither);
}

void Dither::setPalette(uint32_t r[], uint32_t g[], uint32_t b[], uint32_t a[])
{
    caca_set_dither_palette(dither, r, g, b, a);
}

void Dither::setBrightness(float f)
{
    caca_set_dither_brightness(dither, f);
}

void Dither::setGamma(float f)
{
    caca_set_dither_gamma(dither, f);
}

void Dither::setContrast(float f)
{
    caca_set_dither_contrast(dither, f);
}

void Dither::setAntialias(char const *cv)
{
    caca_set_dither_antialias(dither, cv);
}

char const *const * Dither::getAntialiasList()
{
    return caca_get_dither_antialias_list(dither);
}

void Dither::setColor(char const *cv)
{
    caca_set_dither_color(dither, cv);
}

char const *const * Dither::getColorList()
{
    return caca_get_dither_color_list(dither);
}

void Dither::setCharset(char const *cv)
{
    caca_set_dither_charset(dither, cv);
}

char const *const * Dither::getCharsetList()
{
    return caca_get_dither_charset_list(dither);
}

void Dither::setMode(char const *cv)
{
    caca_set_dither_algorithm(dither, cv);
}

char const *const * Dither::getModeList(void)
{
    return caca_get_dither_algorithm_list(dither);
}

void Dither::Bitmap(Canvas *cv, int x, int y, int w, int h, void *v)
{
    caca_dither_bitmap(cv->get_caca_canvas_t(), x, y, w, h, dither, v);
}

Font::Font(void const *s, unsigned int v)
{
    font = caca_load_font(s, v);
    if(!font) throw -1;
}

char const *const * Font::getList(void)
{
    return caca_get_font_list();
}

unsigned int Font::getWidth()
{
    return caca_get_font_width(font);
}

unsigned int Font::getHeight()
{
    return caca_get_font_height(font);
}

void Font::renderCanvas(Canvas *cv, uint8_t *buf, unsigned int x, unsigned int y, unsigned int w)
{
    caca_render_canvas(cv->get_caca_canvas_t(), font, buf, x, y, w);
}

uint32_t const *Font::getBlocks()
{
    return caca_get_font_blocks(font);
}

Font::~Font()
{
    caca_free_font(font);
}

Caca::Caca(Canvas *cv)
{
    dp = caca_create_display(cv->get_caca_canvas_t());
    if(!dp)
        throw -1;
}

Caca::~Caca()
{
    caca_free_display(dp);
}

void Caca::Attach(Canvas *cv)
{
    dp = caca_create_display(cv->get_caca_canvas_t());
    if(!dp)
        throw -1;
}

void Caca::Detach()
{
    caca_free_display(dp);
}

void Caca::setDisplayTime(unsigned int d)
{
    caca_set_display_time(dp, d);
}

void Caca::Display()
{
    caca_refresh_display(dp);
}

unsigned int Caca::getDisplayTime()
{
    return caca_get_display_time(dp);
}

unsigned int Caca::getWidth()
{
    return caca_get_display_width(dp);
}

unsigned int Caca::getHeight()
{
    return caca_get_display_height(dp);
}

int Caca::setTitle(char const *s)
{
    return caca_set_display_title(dp, s);
}

int Caca::getEvent(unsigned int g, Event *n, int aa)
{
    return caca_get_event(dp, g, n ? &n->e : NULL, aa);
}

unsigned int Caca::getMouseX()
{
    return caca_get_mouse_x(dp);
}

unsigned int Caca::getMouseY()
{
    return caca_get_mouse_x(dp);
}

void Caca::setMouse(int v)
{
    caca_set_mouse(dp, v);
}

const char * Caca::getVersion()
{
    return caca_get_version();
}

