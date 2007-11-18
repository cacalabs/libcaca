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
#include "cucul-canvas.h"
#include "common.h"

VALUE cDisplay;

void display_free(void *display)
{
    caca_free_display((caca_display_t *)display);
}

static VALUE display_alloc(VALUE klass)
{
    VALUE obj;
    obj = Data_Wrap_Struct(klass, 0, display_free, NULL);    
    return obj;
}

static VALUE display_initialize(VALUE self, VALUE cv)
{
    caca_display_t *display;
    
    if(CLASS_OF(cv) != cCanvas)
    {
        rb_raise(rb_eArgError, "Argument is not a Cucul::Canvas");
    }

    display = caca_create_display(DATA_PTR(cv));
    if(display == NULL)
    {
        rb_raise(rb_eRuntimeError, strerror(errno));
    }

    _SELF = display;

    return self;
}

static VALUE display_refresh(VALUE self)
{
    caca_refresh_display(_SELF);
    return self;
}

static VALUE set_time(VALUE self, VALUE t)
{
    caca_set_display_time(_SELF, UINT2NUM(t));
    return t;
}

static VALUE set_time2(VALUE self, VALUE t)
{
    set_time(self, t);
    return self;
}

static VALUE get_time(VALUE self)
{
    return NUM2UINT(caca_get_display_time(_SELF));
}

static VALUE get_width(VALUE self)
{
    return NUM2UINT(caca_get_display_width(_SELF));
}

static VALUE get_height(VALUE self)
{
    return NUM2UINT(caca_get_display_height(_SELF));
}

static VALUE set_title(VALUE self, VALUE t)
{
    if(caca_set_display_title(_SELF, StringValuePtr(t))<0)
    {
        rb_raise(rb_eRuntimeError, strerror(errno));
    }
    return t;
}

static VALUE set_title2(VALUE self, VALUE t)
{
    set_title(self, t);
    return self;
}

void Init_caca_display(VALUE mCaca)
{
    cDisplay = rb_define_class_under(mCaca, "Display", rb_cObject);
    rb_define_alloc_func(cDisplay, display_alloc);

    rb_define_method(cDisplay, "initialize", display_initialize, 1);
    rb_define_method(cDisplay, "refresh", display_refresh, 0);
    rb_define_method(cDisplay, "time=", set_time, 1);
    rb_define_method(cDisplay, "set_time", set_time2, 1);
    rb_define_method(cDisplay, "time", get_time, 0);
    rb_define_method(cDisplay, "width", get_width, 0);
    rb_define_method(cDisplay, "height", get_height, 0);
    rb_define_method(cDisplay, "title=", set_title, 1);
    rb_define_method(cDisplay, "set_title", set_title2, 1);
}
