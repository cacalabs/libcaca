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
#include "caca-event.h"
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

static VALUE get_mouse_x(VALUE self)
{
    return NUM2UINT(caca_get_mouse_x(_SELF));
}

static VALUE get_mouse_y(VALUE self)
{
    return NUM2UINT(caca_get_mouse_y(_SELF));
}

static VALUE set_mouse(VALUE self, VALUE visible)
{
    caca_set_display_time(_SELF, visible);
    return visible;
}

static VALUE set_mouse2(VALUE self, VALUE visible)
{
    set_mouse(self, visible);
    return self;
}

static VALUE get_event(VALUE self, VALUE event_mask, VALUE timeout)
{
    char utf8[8];
    caca_event_t ev;
    VALUE e;

    event_mask = rb_funcall(event_mask, rb_intern("to_i"), 0);

    if(caca_get_event(_SELF, NUM2UINT(event_mask), &ev, NUM2INT(timeout)) == 0)
    {
        return Qnil;
    }

    switch(caca_get_event_type(&ev))
    {
    case CACA_EVENT_KEY_PRESS:
        caca_get_event_key_utf8(&ev, utf8);
        e = rb_funcall(cEventKeyPress, rb_intern("new"), 3,
                       UINT2NUM(caca_get_event_key_ch(&ev)),
                       ULONG2NUM(caca_get_event_key_utf32(&ev)),
                       rb_str_new(utf8, 8));
        break;
    case CACA_EVENT_KEY_RELEASE:
        caca_get_event_key_utf8(&ev, utf8);
        e = rb_funcall(cEventKeyRelease, rb_intern("new"), 3,
                       UINT2NUM(caca_get_event_key_ch(&ev)),
                       ULONG2NUM(caca_get_event_key_utf32(&ev)),
                       rb_str_new(utf8, 8));
        break;
    case CACA_EVENT_MOUSE_PRESS:
        e = rb_funcall(cEventMousePress, rb_intern("new"), 3,
                       UINT2NUM(caca_get_event_mouse_x(&ev)),
                       UINT2NUM(caca_get_event_mouse_y(&ev)),
                       UINT2NUM(caca_get_event_mouse_button(&ev)));
        break;
    case CACA_EVENT_MOUSE_RELEASE:
        e = rb_funcall(cEventMouseRelease, rb_intern("new"), 3,
                       UINT2NUM(caca_get_event_mouse_x(&ev)),
                       UINT2NUM(caca_get_event_mouse_y(&ev)),
                       UINT2NUM(caca_get_event_mouse_button(&ev)));
        break;
    case CACA_EVENT_MOUSE_MOTION:
        e = rb_funcall(cEventMouseMotion, rb_intern("new"), 3,
                       UINT2NUM(caca_get_event_mouse_x(&ev)),
                       UINT2NUM(caca_get_event_mouse_y(&ev)),
                       Qnil);
        break;
    case CACA_EVENT_RESIZE:
        e = rb_funcall(cEventResize, rb_intern("new"), 2,
                       UINT2NUM(caca_get_event_resize_width(&ev)),
                       UINT2NUM(caca_get_event_resize_height(&ev)));
        break;
    case CACA_EVENT_QUIT:
        e = rb_funcall(cEventQuit, rb_intern("new"), 0);
        break;
    default:
        rb_raise(rb_eRuntimeError, "Invalid event received !");
    }

    return e;
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
    rb_define_method(cDisplay, "mouse_x", get_mouse_x, 0);
    rb_define_method(cDisplay, "mouse_y", get_mouse_y, 0);
    rb_define_method(cDisplay, "mouse=", set_mouse, 1);
    rb_define_method(cDisplay, "set_mouse", set_mouse2, 1);
    rb_define_method(cDisplay, "get_event", get_event, 2);
}
