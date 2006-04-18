/*
 *  libcucul++      C++ bindings for libcucul
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

/** \file cucul++.h
 *  \version \$Id$
 *  \author Jean-Yves Lamoureux <jylam@lnxscene.org>
 *  \brief The \e libcucul++ public header.
 *
 *  This header contains the public types and functions that applications
 *  using \e libcucul++ may use.
 */

#ifndef _CUCUL_PP_H
#define _CUCUL_PP_H
#include <stdio.h> // BUFSIZ
#include <stdarg.h> // va_*
#include "config.h"
#include "cucul.h"


class Cucul {
    friend class Caca;
 public:
    Cucul();
    Cucul(int width, int height);
    ~Cucul();

    /* Ugly, I know */
    class Font {
        friend class Cucul;
    protected:
        cucul_font *font;
    };
    class Sprite {
        friend class Cucul;
    protected:
        cucul_sprite *sprite;
    };
    class Dither {
        friend class Cucul;
    protected:
        cucul_dither *dither;
    };
    class Buffer {
        friend class Cucul;
    protected:
        cucul_buffer *buffer;
    };



    void set_size(unsigned int w, unsigned int h);
    unsigned int get_width(void);
    unsigned int get_height(void);
    void set_color(unsigned int f, unsigned int b);
    char const * get_color_name (unsigned int color);
    void 	printf ( int x , int y , char const * format,...);
    void putchar (int x, int y, char ch);
    void putstr (int x, int y, char *str);
    void 	clear ();
    void 	blit ( int, int, Cucul* c1, Cucul* c2);
    void 	invert ();
    void 	flip ();
    void 	flop ();
    void 	rotate ();
    void 	draw_line ( int, int, int, int, char const *);
    void 	draw_polyline ( int const x[], int const y[], int, char const *);
    void 	draw_thin_line ( int, int, int, int);
    void 	draw_thin_polyline ( int const x[], int const y[], int);
    void 	draw_circle ( int, int, int, char const *);
    void 	draw_ellipse ( int, int, int, int, char const *);
    void 	draw_thin_ellipse ( int, int, int, int);
    void 	fill_ellipse ( int, int, int, int, char const *);
    void 	draw_box ( int, int, int, int, char const *);
    void 	draw_thin_box ( int, int, int, int);
    void 	fill_box ( int, int, int, int, char const *);
    void 	draw_triangle ( int, int, int, int, int, int, char const *);
    void 	draw_thin_triangle ( int, int, int, int, int, int);
    void 	fill_triangle ( int, int, int, int, int, int, char const *);
    int 	rand (int, int);
    Sprite * 	load_sprite (char const *);
    int 	get_sprite_frames (Cucul::Sprite const *);
    int 	get_sprite_width (Cucul::Sprite const *, int);
    int 	get_sprite_height (Cucul::Sprite const *, int);
    int 	get_sprite_dx (Cucul::Sprite const *, int);
    int 	get_sprite_dy (Cucul::Sprite const *, int);
    void 	draw_sprite ( int, int, Cucul::Sprite const *, int);
    void 	free_sprite (Cucul::Sprite*);
    Dither * 	create_dither (unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
    void 	set_dither_palette (Cucul::Dither *, unsigned int r[], unsigned int g[], unsigned int b[], unsigned int a[]);
    void 	set_dither_brightness (Cucul::Dither *, float);
    void 	set_dither_gamma (Cucul::Dither *, float);
    void 	set_dither_contrast (Cucul::Dither *, float);
    void 	set_dither_invert (Cucul::Dither *, int);
    void 	set_dither_antialias (Cucul::Dither *, char const *);
    char const *const * 	get_dither_antialias_list (Cucul::Dither const *);
    void 	set_dither_color (Cucul::Dither *, char const *);
    char const *const * 	get_dither_color_list (Cucul::Dither const *);
    void 	set_dither_charset (Cucul::Dither *, char const *);
    char const *const * 	get_dither_charset_list (Cucul::Dither const *);
    void 	set_dither_mode (Cucul::Dither *, char const *);
    char const *const * 	get_dither_mode_list (Cucul::Dither const *);
    void 	dither_bitmap ( int, int, int, int, Cucul::Dither const *, void *);
    void 	free_dither (Cucul::Dither *);
    Font * 	load_font (void const *, unsigned int);
    char const *const * 	get_font_list (void);
    unsigned int 	get_font_width (Font *);
    unsigned int 	get_font_height (Font *);
    void 	render_canvas ( Font *, unsigned char *, unsigned int, unsigned int, unsigned int);
    void 	free_font (Font *);
    Buffer * 	create_export ( char const *);
    char const *const * 	get_export_list (void);


 protected:
    cucul_canvas_t *get_cucul_canvas_t();

 private:
    cucul_canvas_t *cv;


};


#endif /* _CUCUL_PP_H */
