#ifndef __CUCUL_CANVAS_H__
#define __CUCUL_CANVAS_H__

#include <ruby.h>

extern VALUE cCanvas;
extern void Init_cucul_canvas(VALUE);
extern VALUE canvas_create(cucul_canvas_t *canvas);

#endif
