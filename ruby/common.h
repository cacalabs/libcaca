/*
 *  libcaca Ruby bindings
 *  Copyright (c) 2007-2010 Pascal Terjan <pterjan@linuxfr.org>
 *                2012 Sam Hocevar <sam@hocevar.net>
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#define _SELF  (DATA_PTR(self))

#define get_singleton_double_list(x)                      \
static VALUE x##_list(void)                     \
{                                               \
    VALUE ary, ary2;                            \
    char const* const* list;                    \
                                                \
    list = caca_get_##x##_list();              \
    ary = rb_ary_new();                         \
    while (*list != NULL)                       \
    {                                           \
        ary2 = rb_ary_new();                    \
        rb_ary_push(ary2, rb_str_new2(*list));  \
        list++;                                 \
        rb_ary_push(ary2, rb_str_new2(*list));  \
        list++;                                 \
        rb_ary_push(ary, ary2);                 \
    }                                           \
                                                \
    return ary;                                 \
}

#define get_double_list(x)                      \
static VALUE x##_list(VALUE self)               \
{                                               \
    VALUE ary, ary2;                            \
    char const* const* list;                    \
                                                \
    list = caca_get_##x##_list(_SELF);         \
    ary = rb_ary_new();                         \
    while (*list != NULL)                       \
    {                                           \
        ary2 = rb_ary_new();                    \
        rb_ary_push(ary2, rb_str_new2(*list));  \
        list++;                                 \
        rb_ary_push(ary2, rb_str_new2(*list));  \
        list++;                                 \
        rb_ary_push(ary, ary2);                 \
    }                                           \
                                                \
    return ary;                                 \
}

#endif
