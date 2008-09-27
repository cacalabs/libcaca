#ifndef __CACA_CANVAS_H__
#define __CACA_CANVAS_H__

#include <ruby.h>

extern VALUE cCanvas;
extern void Init_caca_canvas(VALUE);
extern VALUE canvas_create(caca_canvas_t *canvas);

#endif
