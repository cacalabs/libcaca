#ifndef __CACA_DISPLAY_H__
#define __CACA_DISPLAY_H__

#include <ruby.h>

extern VALUE cEvent;
extern VALUE cEventKeyPress;
extern VALUE cEventKeyRelease;
extern VALUE cEventMouse;
extern VALUE cEventMousePress;
extern VALUE cEventMouseRelease;
extern VALUE cEventMouseMotion;
extern VALUE cEventResize;
extern VALUE cEventQuit;
extern void Init_caca_event(VALUE);

#endif
