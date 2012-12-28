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

#include <ruby.h>
#include <caca.h>
#include <errno.h>
#include "common.h"

VALUE cEvent;
static VALUE cEventKey;
VALUE cEventKeyPress;
VALUE cEventKeyRelease;
static VALUE cEventMouse;
VALUE cEventMousePress;
VALUE cEventMouseRelease;
VALUE cEventMouseMotion;
VALUE cEventResize;
VALUE cEventQuit;

void Init_caca_event(VALUE mCaca)
{
    cEvent = rb_define_class_under(mCaca, "Event", rb_cObject);
    rb_define_const(cEvent, "TYPE", INT2FIX(CACA_EVENT_ANY));

    cEventKey = rb_define_class_under(cEvent, "Key", cEvent);
    rb_define_const(cEventKey, "TYPE",
                    INT2FIX(CACA_EVENT_KEY_PRESS|
                            CACA_EVENT_KEY_RELEASE));

    cEventKeyPress = rb_define_class_under(cEventKey, "Press", cEventKey);
    rb_define_const(cEventKeyPress, "TYPE",
                    INT2FIX(CACA_EVENT_KEY_PRESS));

    cEventKeyRelease = rb_define_class_under(cEventKey, "Release", cEventKey);
    rb_define_const(cEventKeyRelease, "TYPE",
                    INT2FIX(CACA_EVENT_KEY_RELEASE));

    cEventMouse = rb_define_class_under(cEvent, "Mouse", cEvent);
    rb_define_const(cEventMouse, "TYPE",
                    INT2FIX(CACA_EVENT_MOUSE_PRESS|
                            CACA_EVENT_MOUSE_RELEASE|
                            CACA_EVENT_MOUSE_MOTION));

    cEventMousePress = rb_define_class_under(cEventMouse, "Press", cEventMouse);
    rb_define_const(cEventMousePress, "TYPE",
                    INT2FIX(CACA_EVENT_MOUSE_PRESS));

    cEventMouseRelease = rb_define_class_under(cEventMouse, "Release", cEventMouse);
    rb_define_const(cEventMouseRelease, "TYPE",
                    INT2FIX(CACA_EVENT_MOUSE_RELEASE));

    cEventMouseMotion = rb_define_class_under(cEventMouse, "Motion", cEventMouse);
    rb_define_const(cEventMouseMotion, "TYPE",
                    INT2FIX(CACA_EVENT_MOUSE_MOTION));

    cEventResize = rb_define_class_under(cEvent, "Resize", cEvent);
    rb_define_const(cEventResize, "TYPE",
                    INT2FIX(CACA_EVENT_RESIZE));

    cEventQuit = rb_define_class_under(cEvent, "Quit", cEvent);
    rb_define_const(cEventQuit, "TYPE",
                    INT2FIX(CACA_EVENT_QUIT));

}
