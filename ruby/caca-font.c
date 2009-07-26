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

VALUE cFont;

void font_free(void *font)
{
    caca_free_font((caca_font_t *)font);
}

static VALUE font_alloc(VALUE klass)
{
    VALUE obj;
    obj = Data_Wrap_Struct(klass, 0, font_free, NULL);
    return obj;
}

static VALUE font_initialize(VALUE self, VALUE name)
{
    caca_font_t *font;

    font = caca_load_font(StringValuePtr(name), 0);
    if(font == NULL)
    {
        rb_raise(rb_eRuntimeError, strerror(errno));
    }
    _SELF = font;
    return self;
}

static VALUE font_list(void)
{
    VALUE ary;
    char const* const* list;

    list = caca_get_font_list();

    ary = rb_ary_new();
    while (*list != NULL)
    {
        rb_ary_push(ary, rb_str_new2(*list));
        list++;
    }

    return ary;
}

static VALUE get_font_width(VALUE self)
{
    return UINT2NUM(caca_get_font_width(_SELF));
}

static VALUE get_font_height(VALUE self)
{
    return UINT2NUM(caca_get_font_height(_SELF));
}

static VALUE get_font_blocks(VALUE self)
{
    VALUE ary;
    uint32_t const *list;

    list = caca_get_font_blocks(_SELF);

    ary = rb_ary_new();
    while (*list != 0L)
    {
        rb_ary_push(ary, ULONG2NUM(*list));
        list++;
    }

    return ary;
}

void Init_caca_font(VALUE mCaca)
{
    cFont = rb_define_class_under(mCaca, "Font", rb_cObject);
    rb_define_alloc_func(cFont, font_alloc);

    rb_define_method(cFont, "initialize", font_initialize, 1);
    rb_define_method(cFont, "width", get_font_width, 0);
    rb_define_method(cFont, "height", get_font_height, 0);
    rb_define_method(cFont, "blocks", get_font_blocks, 0);
    rb_define_singleton_method(cFont, "list", font_list, 0);
}

