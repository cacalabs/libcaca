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

#include "caca-canvas.h"
#include "caca-dither.h"
#include "caca-font.h"
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

    rb_define_const(mCaca, "BLACK", INT2FIX(CACA_BLACK));
    rb_define_const(mCaca, "BLUE", INT2FIX(CACA_BLUE));
    rb_define_const(mCaca, "GREEN", INT2FIX(CACA_GREEN));
    rb_define_const(mCaca, "CYAN", INT2FIX(CACA_CYAN));
    rb_define_const(mCaca, "RED", INT2FIX(CACA_RED));
    rb_define_const(mCaca, "MAGENTA", INT2FIX(CACA_MAGENTA));
    rb_define_const(mCaca, "BROWN", INT2FIX(CACA_BROWN));
    rb_define_const(mCaca, "LIGHTGRAY", INT2FIX(CACA_LIGHTGRAY));
    rb_define_const(mCaca, "DARKGRAY", INT2FIX(CACA_DARKGRAY));
    rb_define_const(mCaca, "LIGHTBLUE", INT2FIX(CACA_LIGHTBLUE));
    rb_define_const(mCaca, "LIGHTGREEN", INT2FIX(CACA_LIGHTGREEN));
    rb_define_const(mCaca, "LIGHTCYAN", INT2FIX(CACA_LIGHTCYAN));
    rb_define_const(mCaca, "LIGHTRED", INT2FIX(CACA_LIGHTRED));
    rb_define_const(mCaca, "LIGHTMAGENTA", INT2FIX(CACA_LIGHTMAGENTA));
    rb_define_const(mCaca, "YELLOW", INT2FIX(CACA_YELLOW));
    rb_define_const(mCaca, "WHITE", INT2FIX(CACA_WHITE));
    rb_define_const(mCaca, "DEFAULT", INT2FIX(CACA_DEFAULT));
    rb_define_const(mCaca, "TRANSPARENT", INT2FIX(CACA_TRANSPARENT));

    rb_define_const(mCaca, "BOLD", INT2FIX(CACA_BOLD));
    rb_define_const(mCaca, "ITALICS", INT2FIX(CACA_ITALICS));
    rb_define_const(mCaca, "UNDERLINE", INT2FIX(CACA_UNDERLINE));
    rb_define_const(mCaca, "BLINK", INT2FIX(CACA_BLINK));

    Init_caca_canvas(mCaca);
    Init_caca_dither(mCaca);
    Init_caca_font(mCaca);
    Init_caca_display(mCaca);
    Init_caca_event(mCaca);
}
