/*
 *  libcaca Ruby bindings
 *  Copyright (c) 2007 Pascal Terjan <pterjan@linuxfr.org>
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include <ruby.h>
#include <caca.h>
#include <errno.h>
#include "common.h"

VALUE cDither;

void dither_free(void *dither)
{
    caca_free_dither((caca_dither_t *)dither);
}

static VALUE dither_alloc(VALUE klass)
{
    VALUE obj;
    obj = Data_Wrap_Struct(klass, 0, dither_free, NULL);
    return obj;
}

static VALUE dither_initialize(VALUE self, VALUE bpp, VALUE w, VALUE h, VALUE pitch, VALUE rmask, VALUE gmask, VALUE bmask, VALUE amask)
{
    caca_dither_t *dither;

    dither = caca_create_dither(NUM2UINT(bpp), NUM2UINT(w), NUM2UINT(h), NUM2UINT(pitch), NUM2ULONG(rmask), NUM2ULONG(gmask), NUM2ULONG(bmask), NUM2ULONG(amask));
    if(dither == NULL)
    {
        rb_raise(rb_eRuntimeError, strerror(errno));
    }
    _SELF = dither;
    return self;
}

static VALUE set_dither_palette(VALUE self, VALUE palette)
{
    int i;
    unsigned int *red, *blue, *green, *alpha;
    VALUE v, r, g, b, a;
    int error = 0;

    if(RARRAY(palette)->len != 256)
    {
        rb_raise(rb_eArgError, "Palette must contain 256 elements");
    }

    red = ALLOC_N(unsigned int, 256);
    if(!red)
        rb_raise(rb_eNoMemError,"Out of memory");

    green = ALLOC_N(unsigned int, 256);
    if(!green)
    {
        free(red);
        rb_raise(rb_eNoMemError,"Out of memory");
    }

    blue = ALLOC_N(unsigned int, 256);
    if(!blue)
    {
        free(red);
        free(green);
        rb_raise(rb_eNoMemError,"Out of memory");
    }

    alpha = ALLOC_N(unsigned int, 256);
    if(!alpha)
    {
        free(red);
        free(green);
        free(blue);
        rb_raise(rb_eNoMemError,"Out of memory");
    }

    for(i=0; i<256; i++)
    {
        v = rb_ary_entry(palette, i);
        if((TYPE(v) == T_ARRAY) && (RARRAY(v)->len == 4))
        {
            r = rb_ary_entry(v,0);
            g = rb_ary_entry(v,1);
            b = rb_ary_entry(v,2);
            a = rb_ary_entry(v,3);
            if(rb_obj_is_kind_of(r, rb_cInteger) &&
               rb_obj_is_kind_of(g, rb_cInteger) &&
               rb_obj_is_kind_of(b, rb_cInteger) &&
               rb_obj_is_kind_of(a, rb_cInteger))
            {
                red[i]   = NUM2INT(r);
                green[i] = NUM2INT(g);
                blue[i]  = NUM2INT(b);
                alpha[i] = NUM2INT(a);
            } else
                error = 1;
        }
        else
            error = 1;
    }

    if(error)
    {
        free(red);
        free(green);
        free(blue);
        free(alpha);
        rb_raise(rb_eArgError, "Invalid palette");
    }

    if(caca_set_dither_palette(_SELF, red, green, blue, alpha)<0)
    {
        free(red);
        free(green);
        free(blue);
        free(alpha);
        rb_raise(rb_eRuntimeError, strerror(errno));
    }

    free(red);
    free(green);
    free(blue);
    free(alpha);

    return palette;
}

static VALUE set_dither_palette2(VALUE self, VALUE palette)
{
    set_dither_palette(self, palette);
    return self;
}

#define set_float(x)                                    \
static VALUE set_##x(VALUE self, VALUE x)               \
{                                                       \
    if(caca_set_dither_##x(_SELF, (float)NUM2DBL(x))<0) \
        rb_raise(rb_eRuntimeError, strerror(errno));    \
                                                        \
    return x;                                           \
}                                                       \
                                                        \
static VALUE set_##x##2(VALUE self, VALUE x)            \
{                                                       \
    set_##x(self, x);                                   \
    return self;                                        \
}

set_float(brightness)
set_float(gamma)
set_float(contrast)

#define get_set_str_from_list(x)                         \
get_double_list(dither_##x)                              \
static VALUE set_dither_##x(VALUE self, VALUE x)         \
{                                                        \
    if(caca_set_dither_##x(_SELF, StringValuePtr(x))<0)  \
    {                                                    \
        rb_raise(rb_eRuntimeError, strerror(errno));     \
    }                                                    \
    return x;                                            \
}                                                        \
                                                         \
static VALUE set_dither_##x##2(VALUE self, VALUE x)      \
{                                                        \
     set_dither_##x(self, x);                            \
     return self;                                        \
}

get_set_str_from_list(antialias)
get_set_str_from_list(color)
get_set_str_from_list(charset)
get_set_str_from_list(algorithm)

void Init_caca_dither(VALUE mCaca)
{
    cDither = rb_define_class_under(mCaca, "Dither", rb_cObject);
    rb_define_alloc_func(cDither, dither_alloc);

    rb_define_method(cDither, "initialize", dither_initialize, 8);
    rb_define_method(cDither, "palette=", set_dither_palette, 1);
    rb_define_method(cDither, "set_palette", set_dither_palette2, 1);
    rb_define_method(cDither, "brightness=", set_brightness, 1);
    rb_define_method(cDither, "set_brightness", set_brightness2, 1);
    rb_define_method(cDither, "gamma=", set_gamma, 1);
    rb_define_method(cDither, "set_gamma", set_gamma2, 1);
    rb_define_method(cDither, "contrast=", set_contrast, 1);
    rb_define_method(cDither, "set_contrast", set_contrast2, 1);
    rb_define_method(cDither, "antialias_list", dither_antialias_list, 0);
    rb_define_method(cDither, "antialias=", set_dither_antialias, 1);
    rb_define_method(cDither, "set_antialias", set_dither_antialias2, 1);
    rb_define_method(cDither, "color_list", dither_color_list, 0);
    rb_define_method(cDither, "color=", set_dither_color, 1);
    rb_define_method(cDither, "set_color", set_dither_color2, 1);
    rb_define_method(cDither, "charset_list", dither_charset_list, 0);
    rb_define_method(cDither, "charset=", set_dither_charset, 1);
    rb_define_method(cDither, "set_charset", set_dither_charset2, 1);
    rb_define_method(cDither, "algorithm_list", dither_algorithm_list, 0);
    rb_define_method(cDither, "algorithm=", set_dither_algorithm, 1);
    rb_define_method(cDither, "set_algorithm", set_dither_algorithm2, 1);
}

