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

#include "caca-display.h"
#include "caca-event.h"

static VALUE get_version(VALUE self)
{
    return rb_str_new2(caca_get_version());
}

void Init_caca()
{
    VALUE mCaca = rb_define_module("Caca");

    rb_define_singleton_method(mCaca, "version", get_version, 0);

    Init_caca_display(mCaca);
    Init_caca_event(mCaca);
}
