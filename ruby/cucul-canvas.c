/*
 *  libcucul Ruby bindings
 *  Copyright (c) 2007 Pascal Terjan <pterjan@linuxfr.org>
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include <ruby.h>
#include <cucul.h>
#include <errno.h>
#include "cucul-font.h"
#include "common.h"

VALUE cCanvas;

#define simple_func(x)                                  \
static VALUE x (VALUE self)                             \
{                                                       \
    if( cucul_##x (_SELF) <0)                           \
        rb_raise(rb_eRuntimeError, strerror(errno));    \
                                                        \
    return self;                                        \
}

#define get_int(x)                                      \
static VALUE get_##x (VALUE self)                       \
{                                                       \
    return INT2NUM(cucul_get_##x (_SELF));              \
}

static void canvas_free(void * p)
{
    cucul_free_canvas((cucul_canvas_t *)p);
}

static VALUE canvas_alloc(VALUE klass)
{
    cucul_canvas_t *canvas;
    VALUE obj;
    
    canvas = cucul_create_canvas(0, 0);
    obj = Data_Wrap_Struct(klass, 0, canvas_free, canvas);
    
    return obj;
}

static VALUE canvas_initialize(VALUE self, VALUE width, VALUE height)
{
    unsigned int w, h;

    w = NUM2INT(width);
    h = NUM2INT(height);
    
    cucul_set_canvas_size(_SELF, w, h);
    
    return self;
}

get_int(canvas_height)
get_int(canvas_width)

static VALUE set_canvas_width(VALUE self, VALUE width)
{
    cucul_set_canvas_size(_SELF, NUM2INT(width), cucul_get_canvas_height(_SELF));
    return width;
}

static VALUE set_canvas_width2(VALUE self, VALUE width)
{
    set_canvas_width(self, width);
    return self;
}

static VALUE set_canvas_height(VALUE self, VALUE height)
{
    cucul_set_canvas_size(_SELF, cucul_get_canvas_width(_SELF), NUM2INT(height));
    return height;
}

static VALUE set_canvas_height2(VALUE self, VALUE height)
{
    set_canvas_height(self, height);
    return self;
}

static VALUE set_canvas_size(VALUE self, VALUE height, VALUE width)
{
    cucul_set_canvas_size(_SELF, NUM2INT(width), NUM2INT(height));
    return self;
}

/****/

static VALUE gotoxy(VALUE self, VALUE x, VALUE y)
{
    if( cucul_gotoxy(_SELF, NUM2INT(x), NUM2INT(y)) <0) {
        rb_raise(rb_eRuntimeError, strerror(errno));
    }
    return self;
}

get_int(cursor_x)
get_int(cursor_y)

simple_func(clear_canvas)

static VALUE put_char(VALUE self, VALUE x, VALUE y, VALUE ch)
{
    cucul_put_char(_SELF, NUM2INT(x), NUM2INT(y), NUM2ULONG(ch));
    return self;
}

static VALUE get_char(VALUE self, VALUE x, VALUE y)
{
    unsigned long int ch;
    ch = cucul_get_char(_SELF, NUM2INT(x), NUM2INT(y));
    return INT2NUM(ch);
}

static VALUE put_str(VALUE self, VALUE x, VALUE y, VALUE str)
{
    cucul_put_str(_SELF, NUM2INT(x), NUM2INT(y), StringValuePtr(str));
    return self;
}

static VALUE get_attr(VALUE self, VALUE x, VALUE y)
{
    unsigned long int ch;
    ch = cucul_get_attr(_SELF, NUM2INT(x), NUM2INT(y));
    return INT2NUM(ch);
}

static VALUE set_attr(VALUE self, VALUE attr)
{
    if(cucul_set_attr(_SELF, NUM2ULONG(attr)) <0)
        rb_raise(rb_eRuntimeError, strerror(errno));

    return self;
}

static VALUE set_attr2(VALUE self, VALUE attr)
{
    set_attr(self, attr);
    return self;
}

static VALUE put_attr(VALUE self, VALUE x, VALUE y, VALUE attr)
{
    if(cucul_put_attr(_SELF, NUM2INT(x), NUM2INT(y), NUM2ULONG(attr)) <0)
        rb_raise(rb_eRuntimeError, strerror(errno));

    return self;
}

static VALUE set_color_ansi(VALUE self, VALUE fg, VALUE bg)
{
    if(cucul_set_color_ansi(_SELF, NUM2INT(fg), NUM2INT(bg)) <0)
        rb_raise(rb_eRuntimeError, strerror(errno));

    return self;
}

static VALUE set_color_argb(VALUE self, VALUE fg, VALUE bg)
{
    if(cucul_set_color_argb(_SELF, NUM2UINT(fg), NUM2UINT(bg)) <0) {
        rb_raise(rb_eRuntimeError, strerror(errno));
    }
    return self;
}

static VALUE cprintf(int argc, VALUE* argv, VALUE self)
{
    int x, y;
    VALUE rx, ry, format, rest, string;
    rb_scan_args(argc, argv, "3*", &rx, &ry, &format, &rest);
    x = NUM2INT(rx);
    y = NUM2INT(ry);
    string = rb_funcall2(rb_mKernel, rb_intern("sprintf"), argc-2, argv+2);
    cucul_put_str(_SELF, x, y, StringValuePtr(string));
    return self;
}


get_int(canvas_handle_x)
get_int(canvas_handle_y)

static VALUE set_canvas_handle(VALUE self, VALUE x, VALUE y)
{
    cucul_set_canvas_handle(_SELF, NUM2INT(x), NUM2INT(y));
    return self;
}

static VALUE blit(int argc, VALUE* argv, VALUE self) {
    VALUE x, y, src, mask;
    cucul_canvas_t *csrc, *cmask;

    rb_scan_args(argc, argv, "31", &x, &y, &src, &mask);

    Check_Type(x, T_FIXNUM);
    Check_Type(y, T_FIXNUM);

    if(CLASS_OF(src) != cCanvas)
    {
        rb_raise(rb_eArgError, "src is not a Cucul::Canvas");
    }
    Data_Get_Struct(src, cucul_canvas_t, csrc);

    if(!NIL_P(mask))
    {
        if(CLASS_OF(mask) != cCanvas)
        {
            rb_raise(rb_eArgError, "mask is not a Cucul::Canvas");
        }
        Data_Get_Struct(mask, cucul_canvas_t, cmask);
    }
    else
        cmask = NULL;
    
    if(cucul_blit(_SELF, NUM2INT(x), NUM2INT(y), csrc, cmask)<0)
        rb_raise(rb_eRuntimeError, strerror(errno));

    return self;
}

static VALUE set_canvas_boundaries(VALUE self, VALUE x, VALUE y, VALUE w, VALUE h)
{
    if(cucul_set_canvas_boundaries(_SELF, NUM2INT(x), NUM2INT(y), NUM2UINT(w), NUM2UINT(h))<0)
    {
        rb_raise(rb_eRuntimeError, strerror(errno));
    }
    return self;
}

/****/

simple_func(invert)
simple_func(flip)
simple_func(flop)
simple_func(rotate_180)
simple_func(rotate_left)
simple_func(rotate_right)
simple_func(stretch_left)
simple_func(stretch_right)

/****/

static VALUE draw_line(VALUE self, VALUE x1, VALUE y1, VALUE x2, VALUE y2, VALUE ch)
{
    cucul_draw_line(_SELF, NUM2INT(x1), NUM2INT(y1), NUM2INT(x2), NUM2INT(y2),NUM2ULONG(ch));
    return self;
}

static VALUE draw_polyline(VALUE self, VALUE points, VALUE ch)
{
    int i, n;
    int *ax, *ay;
    int error = 0;
    VALUE v, x, y;

    n = RARRAY(points)->len;

    ax = (int*)malloc(n*sizeof(int));
    if(!ax)
        rb_raise(rb_eNoMemError,"Out of memory");

    ay = (int*)malloc(n*sizeof(int));
    if(!ay)
    {
        free(ax);
        rb_raise(rb_eNoMemError,"Out of memory");
    }

    for(i=0; i<n; i++)
    {
        v = rb_ary_entry(points, i);
        if((TYPE(v) == T_ARRAY) && (RARRAY(v)->len == 2))
        {
            x = rb_ary_entry(v,0);
            y = rb_ary_entry(v,1);
            if(rb_obj_is_kind_of(x, rb_cInteger) &&
               rb_obj_is_kind_of(y, rb_cInteger))
            {
                ax[i] = NUM2INT(x);
                ay[i] = NUM2INT(y);
            } else
                error = 1;
        }
	else
            error = 1;
    }

    if(error)
    {
        free(ax);
        free(ay);
        rb_raise(rb_eArgError, "Invalid list of points");
    }

    n--;

    cucul_draw_polyline(_SELF, ax, ay, n, NUM2ULONG(ch));

    free(ax);
    free(ay);

    return self;
}

static VALUE draw_thin_line(VALUE self, VALUE x1, VALUE y1, VALUE x2, VALUE y2)
{
    cucul_draw_thin_line(_SELF, NUM2INT(x1), NUM2INT(y1), NUM2INT(x2), NUM2INT(y2));
    return self;
}

static VALUE draw_thin_polyline(VALUE self, VALUE points)
{
    int i, n;
    int *ax, *ay;
    int error = 0;
    VALUE v, x, y;

    n = RARRAY(points)->len;

    ax = (int*)malloc(n*sizeof(int));
    if(!ax)
        rb_raise(rb_eNoMemError,"Out of memory");

    ay = (int*)malloc(n*sizeof(int));
    if(!ay)
    {
        free(ax);
        rb_raise(rb_eNoMemError,"Out of memory");
    }

    for(i=0; i<n; i++)
    {
        v = rb_ary_entry(points, i);
        if((TYPE(v) == T_ARRAY) && (RARRAY(v)->len == 2))
        {
            x = rb_ary_entry(v,0);
            y = rb_ary_entry(v,1);
            if(rb_obj_is_kind_of(x, rb_cInteger) &&
               rb_obj_is_kind_of(y, rb_cInteger))
            {
                ax[i] = NUM2INT(x);
                ay[i] = NUM2INT(y);
            } else
                error = 1;
        }
	else
            error = 1;
    }

    if(error)
    {
        free(ax);
        free(ay);
        rb_raise(rb_eArgError, "Invalid list of points");
    }

    n--;

    cucul_draw_thin_polyline(_SELF, ax, ay, n);

    free(ax);
    free(ay);

    return self;
}

static VALUE draw_circle(VALUE self, VALUE x, VALUE y, VALUE r, VALUE ch)
{
    cucul_draw_circle(_SELF, NUM2INT(x), NUM2INT(y), NUM2INT(r), NUM2ULONG(ch));
    return self;
}

static VALUE draw_ellipse(VALUE self, VALUE x, VALUE y, VALUE a, VALUE b, VALUE ch)
{
    cucul_draw_ellipse(_SELF, NUM2INT(x), NUM2INT(y), NUM2INT(a), NUM2INT(b), NUM2ULONG(ch));
    return self;
}

static VALUE draw_thin_ellipse(VALUE self, VALUE x, VALUE y, VALUE a, VALUE b)
{
    cucul_draw_thin_ellipse(_SELF, NUM2INT(x), NUM2INT(y), NUM2INT(a), NUM2INT(b));
    return self;
}

static VALUE fill_ellipse(VALUE self, VALUE x, VALUE y, VALUE a, VALUE b, VALUE ch)
{
    cucul_fill_ellipse(_SELF, NUM2INT(x), NUM2INT(y), NUM2INT(a), NUM2INT(b), NUM2ULONG(ch));
    return self;
}

static VALUE draw_box(VALUE self, VALUE x1, VALUE y1, VALUE x2, VALUE y2, VALUE ch)
{
    cucul_draw_box(_SELF, NUM2INT(x1), NUM2INT(y1), NUM2INT(x2), NUM2INT(y2), NUM2ULONG(ch));
    return self;
}

static VALUE draw_thin_box(VALUE self, VALUE x1, VALUE y1, VALUE x2, VALUE y2)
{
    cucul_draw_thin_box(_SELF, NUM2INT(x1), NUM2INT(y1), NUM2INT(x2), NUM2INT(y2));
    return self;
}

static VALUE draw_cp437_box(VALUE self, VALUE x1, VALUE y1, VALUE x2, VALUE y2)
{
    cucul_draw_cp437_box(_SELF, NUM2INT(x1), NUM2INT(y1), NUM2INT(x2), NUM2INT(y2));
    return self;
}

static VALUE fill_box(VALUE self, VALUE x1, VALUE y1, VALUE x2, VALUE y2, VALUE ch)
{
    cucul_fill_box(_SELF, NUM2INT(x1), NUM2INT(y1), NUM2INT(x2), NUM2INT(y2), NUM2ULONG(ch));
    return self;
}

static VALUE draw_triangle(VALUE self, VALUE x1, VALUE y1, VALUE x2, VALUE y2, VALUE x3, VALUE y3, VALUE ch)
{
    cucul_draw_triangle(_SELF, NUM2INT(x1), NUM2INT(y1), NUM2INT(x2), NUM2INT(y2),  NUM2INT(x3), NUM2INT(y3), NUM2ULONG(ch));
    return self;
}

static VALUE draw_thin_triangle(VALUE self, VALUE x1, VALUE y1, VALUE x2, VALUE y2, VALUE x3, VALUE y3)
{
    cucul_draw_thin_triangle(_SELF, NUM2INT(x1), NUM2INT(y1), NUM2INT(x2), NUM2INT(y2),  NUM2INT(x3), NUM2INT(y3));
    return self;
}

static VALUE fill_triangle(VALUE self, VALUE x1, VALUE y1, VALUE x2, VALUE y2, VALUE x3, VALUE y3, VALUE ch)
{
    cucul_fill_triangle(_SELF, NUM2INT(x1), NUM2INT(y1), NUM2INT(x2), NUM2INT(y2),  NUM2INT(x3), NUM2INT(y3), NUM2ULONG(ch));
    return self;
}

/****/

get_int(frame_count)

static VALUE set_frame(VALUE self, VALUE id)
{
    if(cucul_set_frame(_SELF, NUM2INT(id))<0)
        rb_raise(rb_eArgError, strerror(errno));

    return self;
}

static VALUE set_frame2(VALUE self, VALUE id)
{
    set_frame(self, id);
    return self;
}

static VALUE get_frame_name(VALUE self)
{
    return rb_str_new2(cucul_get_frame_name(_SELF));
}

static VALUE set_frame_name(VALUE self, VALUE name)
{
    if(cucul_set_frame_name(_SELF, StringValuePtr(name))<0)
        rb_raise(rb_eRuntimeError, strerror(errno));

    return self;
}

static VALUE set_frame_name2(VALUE self, VALUE name)
{
    set_frame_name(self, name);
    return self;
}

static VALUE create_frame(VALUE self, VALUE id)
{
    if(cucul_create_frame(_SELF, NUM2INT(id))<0) {
        rb_raise(rb_eRuntimeError, strerror(errno));
    }
    return self;
}

static VALUE free_frame(VALUE self, VALUE id)
{
    if(cucul_free_frame(_SELF, NUM2INT(id))<0) {
        rb_raise(rb_eArgError, strerror(errno));
    }
    return self;
}

/****/

static VALUE render_canvas(VALUE self, VALUE font, VALUE width, VALUE height, VALUE pitch)
{
    void *buf;
    cucul_font_t *f;
    VALUE b;

    if(CLASS_OF(font) != cFont)
    {
        rb_raise(rb_eArgError, "First argument is not a Cucul::Font");
    }

    buf = malloc(width*height*4);
    if(buf == NULL)
    {
        rb_raise(rb_eNoMemError, "Out of memory");
    }

    f = DATA_PTR(font);
    cucul_render_canvas(_SELF, f, buf, NUM2UINT(width), NUM2UINT(height), NUM2UINT(pitch));

    b = rb_str_new(buf, width*height*4);
    free(buf);
    return b;
}

static VALUE import_memory(VALUE self, VALUE data, VALUE format)
{
    long int bytes;
    bytes = cucul_import_memory (_SELF, StringValuePtr(data), RSTRING(StringValue(data))->len, StringValuePtr(format));
    if(bytes <= 0)
        rb_raise(rb_eRuntimeError, strerror(errno));

    return self;
}

static VALUE import_file(VALUE self, VALUE filename, VALUE format)
{
    long int bytes;
    bytes = cucul_import_file (_SELF, StringValuePtr(filename), StringValuePtr(format));
    if(bytes <= 0)
        rb_raise(rb_eRuntimeError, strerror(errno));

    return self;
}

static VALUE export_memory(VALUE self, VALUE format)
{
    unsigned long int bytes;
    void *result;
    VALUE ret;
    result = cucul_export_memory (_SELF, StringValuePtr(format), &bytes);
    ret = rb_str_new(result, bytes);
    free(result);
    return ret;
}

static VALUE export_list(void)
{
    VALUE ary, ary2;
    
    char const* const* list;
    
    list = cucul_get_export_list();
    
    ary = rb_ary_new();
    
    while (*list != NULL)
    {
        ary2 = rb_ary_new();
        rb_ary_push(ary2, rb_str_new2(*list));
        list++;
        rb_ary_push(ary2, rb_str_new2(*list));
        list++;
        rb_ary_push(ary, ary2);
    }

    return ary;
}

static VALUE import_list(void)
{
    VALUE ary, ary2;
    
    char const* const* list;
    
    list = cucul_get_import_list();
    
    ary = rb_ary_new();
    
    while (*list != NULL)
    {
        ary2 = rb_ary_new();
        rb_ary_push(ary2, rb_str_new2(*list));
        list++;
        rb_ary_push(ary2, rb_str_new2(*list));
        list++;
        rb_ary_push(ary, ary2);
    }

    return ary;
}

/****/

void Init_cucul_canvas(VALUE mCucul)
{
    cCanvas = rb_define_class_under(mCucul, "Canvas", rb_cObject);
    rb_define_alloc_func(cCanvas, canvas_alloc);

    rb_define_method(cCanvas, "initialize", canvas_initialize, 2);
    rb_define_method(cCanvas, "width", get_canvas_width, 0);
    rb_define_method(cCanvas, "width=", set_canvas_width, 1);
    rb_define_method(cCanvas, "set_width", set_canvas_width2, 1);
    rb_define_method(cCanvas, "height", get_canvas_height, 0);
    rb_define_method(cCanvas, "height=", set_canvas_height, 1);
    rb_define_method(cCanvas, "set_height", set_canvas_height2, 1);
    rb_define_method(cCanvas, "set_size", set_canvas_size, 2);
        
    rb_define_method(cCanvas, "gotoxy", gotoxy, 2);
    rb_define_method(cCanvas, "cursor_x", get_cursor_x, 0);
    rb_define_method(cCanvas, "cursor_y", get_cursor_y, 0);
    rb_define_method(cCanvas, "handle_x", get_canvas_handle_x, 0);
    rb_define_method(cCanvas, "handle_y", get_canvas_handle_y, 0);
    rb_define_method(cCanvas, "set_handle", set_canvas_handle, 2);
    rb_define_method(cCanvas, "blit", blit, -1);
    rb_define_method(cCanvas, "set_boundaries", set_canvas_boundaries, 4);

    rb_define_method(cCanvas, "clear", clear_canvas, 0);
    
    rb_define_method(cCanvas, "put_char", put_char, 3);
    rb_define_method(cCanvas, "get_char", get_char, 2);
    rb_define_method(cCanvas, "put_str", put_str, 3);
    rb_define_method(cCanvas, "printf", cprintf, -1);

    rb_define_method(cCanvas, "get_attr", get_attr, 3);
    rb_define_method(cCanvas, "attr=", set_attr, 1);
    rb_define_method(cCanvas, "set_attr", set_attr2, 1);
    rb_define_method(cCanvas, "put_attr", put_attr, 3);
    rb_define_method(cCanvas, "set_color_ansi", set_color_ansi, 2);
    rb_define_method(cCanvas, "set_color_argb", set_color_argb, 2);

    rb_define_method(cCanvas, "invert", invert, 0);
    rb_define_method(cCanvas, "flip", flip, 0);
    rb_define_method(cCanvas, "flop", flop, 0);
    rb_define_method(cCanvas, "rotate_180", rotate_180, 0);
    rb_define_method(cCanvas, "rotate_left", rotate_left, 0);
    rb_define_method(cCanvas, "rotate_right", rotate_right, 0);
    rb_define_method(cCanvas, "stretch_left", stretch_left, 0);
    rb_define_method(cCanvas, "stretch_right", stretch_right, 0);

    rb_define_method(cCanvas, "draw_line", draw_line, 5);
    rb_define_method(cCanvas, "draw_polyline", draw_polyline, 2);
    rb_define_method(cCanvas, "draw_thin_line", draw_thin_line, 4);
    rb_define_method(cCanvas, "draw_thin_polyline", draw_thin_polyline, 1);
    rb_define_method(cCanvas, "draw_circle", draw_circle, 4);
    rb_define_method(cCanvas, "draw_ellipse", draw_ellipse, 5);
    rb_define_method(cCanvas, "draw_thin_ellipse", draw_thin_ellipse, 4);
    rb_define_method(cCanvas, "fill_ellipse", fill_ellipse, 5);
    rb_define_method(cCanvas, "draw_box", draw_box, 5);
    rb_define_method(cCanvas, "draw_thin_box", draw_thin_box, 4);
    rb_define_method(cCanvas, "draw_cp437_box", draw_cp437_box, 4);
    rb_define_method(cCanvas, "fill_box", fill_box, 5);
    rb_define_method(cCanvas, "draw_triangle", draw_triangle, 7);
    rb_define_method(cCanvas, "draw_thin_triangle", draw_thin_triangle, 6);
    rb_define_method(cCanvas, "fill_triangle", fill_triangle, 7);

    rb_define_method(cCanvas, "frame_count", get_frame_count, 0);
    rb_define_method(cCanvas, "frame=", set_frame, 1);
    rb_define_method(cCanvas, "set_frame", set_frame2, 1);
    rb_define_method(cCanvas, "frame_name", get_frame_name, 0);
    rb_define_method(cCanvas, "frame_name=", set_frame_name, 1);
    rb_define_method(cCanvas, "set_frame_name", set_frame_name2, 1);
    rb_define_method(cCanvas, "create_frame", create_frame, 1);
    rb_define_method(cCanvas, "free_frame", free_frame, 1);

    rb_define_method(cCanvas, "render", render_canvas, 4);
    rb_define_method(cCanvas, "import_memory", import_memory, 2);
    rb_define_method(cCanvas, "import_file", import_file, 2);
    rb_define_method(cCanvas, "export_memory", export_memory, 1);
    rb_define_singleton_method(cCanvas, "export_list", export_list, 0);
    rb_define_singleton_method(cCanvas, "import_list", import_list, 0);
   
}

