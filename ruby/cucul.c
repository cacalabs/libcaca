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

#include "cucul-canvas.h"
#include "cucul-dither.h"
#include "cucul-font.h"

void Init_cucul()
{
    VALUE mCucul = rb_define_module("Cucul");

    rb_define_const(mCucul, "BLACK", INT2FIX(CUCUL_BLACK));
    rb_define_const(mCucul, "BLUE", INT2FIX(CUCUL_BLUE));
    rb_define_const(mCucul, "GREEN", INT2FIX(CUCUL_GREEN));
    rb_define_const(mCucul, "CYAN", INT2FIX(CUCUL_CYAN));
    rb_define_const(mCucul, "RED", INT2FIX(CUCUL_RED));
    rb_define_const(mCucul, "MAGENTA", INT2FIX(CUCUL_MAGENTA));
    rb_define_const(mCucul, "BROWN", INT2FIX(CUCUL_BROWN));
    rb_define_const(mCucul, "LIGHTGRAY", INT2FIX(CUCUL_LIGHTGRAY));
    rb_define_const(mCucul, "DARKGRAY", INT2FIX(CUCUL_DARKGRAY));
    rb_define_const(mCucul, "LIGHTBLUE", INT2FIX(CUCUL_LIGHTBLUE));
    rb_define_const(mCucul, "LIGHTGREEN", INT2FIX(CUCUL_LIGHTGREEN));
    rb_define_const(mCucul, "LIGHTCYAN", INT2FIX(CUCUL_LIGHTCYAN));
    rb_define_const(mCucul, "LIGHTRED", INT2FIX(CUCUL_LIGHTRED));
    rb_define_const(mCucul, "LIGHTMAGENTA", INT2FIX(CUCUL_LIGHTMAGENTA));
    rb_define_const(mCucul, "YELLOW", INT2FIX(CUCUL_YELLOW));
    rb_define_const(mCucul, "WHITE", INT2FIX(CUCUL_WHITE));
    rb_define_const(mCucul, "DEFAULT", INT2FIX(CUCUL_DEFAULT));
    rb_define_const(mCucul, "TRANSPARENT", INT2FIX(CUCUL_TRANSPARENT));

    rb_define_const(mCucul, "BOLD", INT2FIX(CUCUL_BOLD));
    rb_define_const(mCucul, "ITALICS", INT2FIX(CUCUL_ITALICS));
    rb_define_const(mCucul, "UNDERLINE", INT2FIX(CUCUL_UNDERLINE));
    rb_define_const(mCucul, "BLINK", INT2FIX(CUCUL_BLINK));

    Init_cucul_canvas(mCucul);
    Init_cucul_dither(mCucul);
    Init_cucul_font(mCucul);
}
