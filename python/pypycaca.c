/*
 *  PypyCaca      libcaca Python bindings
 *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "pypycaca.h"

static PyMethodDef CacaMethods[] = {
    {"init", pycaca_init, METH_VARARGS, "Init libcaca"},
    {"set_display_time", pycaca_set_display_time, METH_VARARGS, "Set display time"},
    {"get_display_time", pycaca_get_display_time, METH_NOARGS, "Get render time"},
    {"get_width", pycaca_get_width, METH_NOARGS, "Get width"},
    {"get_height", pycaca_get_height, METH_NOARGS, "Get Height"},
    {"set_size", pycaca_set_size, METH_VARARGS, "Set size"},
    {"set_width", pycaca_set_width, METH_VARARGS, "Set width"},
    {"set_height", pycaca_set_height, METH_VARARGS, "Set height"},
    {"set_display_title", pycaca_set_display_title, METH_VARARGS, "Set window titl"},
    {"get_display_width", pycaca_get_display_width, METH_NOARGS, "Get window width"},
    {"get_display_height", pycaca_get_display_height, METH_NOARGS, "Get Window height"},
    {"refresh", pycaca_refresh, METH_NOARGS, "Refresh window"},
    {"end", pycaca_end, METH_NOARGS, "End libcaca"},
    {"get_feature", pycaca_get_feature, METH_VARARGS, "Get feature"},
    {"set_feature", pycaca_set_feature, METH_VARARGS, "Set feature"},
    {"get_feature_name", pycaca_get_feature_name, METH_VARARGS, "Get feature name"},

    {"get_event", pycaca_get_event, METH_VARARGS, "Get event"},
    {"wait_event", pycaca_wait_event, METH_VARARGS, "Wait event"},
    {"get_mouse_x", pycaca_get_mouse_x, METH_NOARGS, "Get Mouse X"},
    {"get_mouse_y", pycaca_get_mouse_y, METH_NOARGS, "Get mouse Y"},
    {"draw_line", pycaca_draw_line, METH_VARARGS, "Init libcaca"},
    {"draw_thin_line", pycaca_draw_thin_line, METH_VARARGS, "Init libcaca"},
    {"draw_circle", pycaca_draw_circle, METH_VARARGS, "Init libcaca"},
    {"draw_ellipse", pycaca_draw_ellipse, METH_VARARGS, "Init libcaca"},
    {"draw_thin_ellipse", pycaca_draw_thin_ellipse, METH_VARARGS, "Init libcaca"},
    {"fill_ellipse", pycaca_fill_ellipse, METH_VARARGS, "Init libcaca"},
    {"draw_box", pycaca_draw_box, METH_VARARGS, "Init libcaca"},
    {"fill_box", pycaca_fill_box, METH_VARARGS, "Init libcaca"},
    {"draw_thin_box", pycaca_draw_thin_box, METH_VARARGS, "Init libcaca"},
    {"draw_triangle", pycaca_draw_triangle, METH_VARARGS, "Init libcaca"},
    {"draw_thin_triangle", pycaca_draw_thin_triangle, METH_VARARGS, "Init libcaca"},
    {"fill_triangle", pycaca_fill_triangle, METH_VARARGS, "Init libcaca"},
    {"draw_polyline", pycaca_draw_polyline, METH_VARARGS, ""},
    {"draw_thin_polyline", pycaca_draw_thin_polyline, METH_VARARGS, ""},

    {"set_color", pycaca_set_color, METH_VARARGS, "Init libcaca"},
    {"get_fg_color", pycaca_get_fg_color, METH_VARARGS, ""},
    {"get_bg_color", pycaca_get_bg_color, METH_VARARGS, ""},
    {"get_color_name", pycaca_get_color_name, METH_VARARGS, ""},
    {"putchar", pycaca_putchar, METH_VARARGS, ""},
    {"putstr", pycaca_putstr, METH_VARARGS, ""},
    {"printf", pycaca_printf, METH_VARARGS, ""},
    {"clear", pycaca_clear, METH_VARARGS, ""},

    {"load_sprite", pycaca_load_sprite, METH_VARARGS, ""},
    {"draw_sprite", pycaca_draw_sprite,  METH_VARARGS, ""},
    {"get_sprite_frames", pycaca_get_sprite_frames, METH_VARARGS, ""},
    {"get_sprite_width", pycaca_get_sprite_width, METH_VARARGS, ""},
    {"get_sprite_height", pycaca_get_sprite_height, METH_VARARGS, ""},
    {"get_sprite_dx", pycaca_get_sprite_dx, METH_VARARGS, ""},
    {"get_sprite_dy", pycaca_get_sprite_dy, METH_VARARGS, ""},
    {"free_sprite", pycaca_free_sprite, METH_VARARGS, ""},

    {"get_html", pycaca_get_html, METH_VARARGS, ""},
    {"get_html3", pycaca_get_html3, METH_VARARGS, ""},
    {"get_irc", pycaca_get_irc, METH_VARARGS, ""},
    {"get_ansi", pycaca_get_ansi, METH_VARARGS, ""},

    {"create_bitmap", pycaca_create_bitmap, METH_VARARGS, ""},
    {"set_bitmap_palette", pycaca_set_bitmap_palette, METH_VARARGS, ""},
    {"set_bitmap_gamma", pycaca_set_bitmap_gamma, METH_VARARGS, ""},
    {"draw_bitmap", pycaca_draw_bitmap, METH_VARARGS, ""},
    {"free_bitmap", pycaca_free_bitmap, METH_VARARGS, ""},




    //   {"", , METH_VARARGS, ""},




    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initcaca(void)
{
  PyObject *obj, *dict;

  obj = Py_InitModule("caca", CacaMethods);
  dict = PyModule_GetDict(obj);

  SET_INTCONSTANT(dict,CACA_EVENT_NONE );
  SET_INTCONSTANT(dict,CACA_EVENT_KEY_PRESS );
  SET_INTCONSTANT(dict,CACA_EVENT_KEY_RELEASE );
  SET_INTCONSTANT(dict,CACA_EVENT_MOUSE_PRESS );
  SET_INTCONSTANT(dict,CACA_EVENT_MOUSE_RELEASE );
  SET_INTCONSTANT(dict,CACA_EVENT_MOUSE_MOTION );
  SET_INTCONSTANT(dict,CACA_EVENT_RESIZE );
  SET_INTCONSTANT(dict,CACA_EVENT_ANY );

  SET_INTCONSTANT(dict, CACA_COLOR_BLACK );
  SET_INTCONSTANT(dict, CACA_COLOR_BLUE );
  SET_INTCONSTANT(dict, CACA_COLOR_GREEN );
  SET_INTCONSTANT(dict, CACA_COLOR_CYAN );
  SET_INTCONSTANT(dict, CACA_COLOR_RED );
  SET_INTCONSTANT(dict, CACA_COLOR_MAGENTA );
  SET_INTCONSTANT(dict, CACA_COLOR_BROWN );
  SET_INTCONSTANT(dict, CACA_COLOR_LIGHTGRAY );
  SET_INTCONSTANT(dict, CACA_COLOR_DARKGRAY );
  SET_INTCONSTANT(dict, CACA_COLOR_LIGHTBLUE );
  SET_INTCONSTANT(dict, CACA_COLOR_LIGHTGREEN );
  SET_INTCONSTANT(dict, CACA_COLOR_LIGHTCYAN );
  SET_INTCONSTANT(dict, CACA_COLOR_LIGHTRED );
  SET_INTCONSTANT(dict, CACA_COLOR_LIGHTMAGENTA );
  SET_INTCONSTANT(dict, CACA_COLOR_YELLOW );
  SET_INTCONSTANT(dict, CACA_COLOR_WHITE );

  SET_INTCONSTANT(dict, CACA_BACKGROUND );
  SET_INTCONSTANT(dict, CACA_BACKGROUND_BLACK );
  SET_INTCONSTANT(dict, CACA_BACKGROUND_SOLID );
  SET_INTCONSTANT(dict, CACA_BACKGROUND_MIN );
  SET_INTCONSTANT(dict, CACA_BACKGROUND_MAX );
  SET_INTCONSTANT(dict, CACA_ANTIALIASING );
  SET_INTCONSTANT(dict, CACA_ANTIALIASING_NONE );
  SET_INTCONSTANT(dict, CACA_ANTIALIASING_PREFILTER );
  SET_INTCONSTANT(dict, CACA_ANTIALIASING_MIN );
  SET_INTCONSTANT(dict, CACA_ANTIALIASING_MAX );
  SET_INTCONSTANT(dict, CACA_DITHERING );
  SET_INTCONSTANT(dict, CACA_DITHERING_NONE );
  SET_INTCONSTANT(dict, CACA_DITHERING_ORDERED2 );
  SET_INTCONSTANT(dict, CACA_DITHERING_ORDERED4 );
  SET_INTCONSTANT(dict, CACA_DITHERING_ORDERED8 );
  SET_INTCONSTANT(dict, CACA_DITHERING_RANDOM );
  SET_INTCONSTANT(dict, CACA_DITHERING_FSTEIN );
  SET_INTCONSTANT(dict, CACA_DITHERING_MIN );
  SET_INTCONSTANT(dict, CACA_DITHERING_MAX );
  SET_INTCONSTANT(dict, CACA_FEATURE_UNKNOWN  );

  SET_INTCONSTANT(dict, CACA_KEY_UNKNOWN );
  SET_INTCONSTANT(dict, CACA_KEY_BACKSPACE );
  SET_INTCONSTANT(dict, CACA_KEY_TAB );
  SET_INTCONSTANT(dict, CACA_KEY_RETURN );
  SET_INTCONSTANT(dict, CACA_KEY_PAUSE );
  SET_INTCONSTANT(dict, CACA_KEY_ESCAPE );
  SET_INTCONSTANT(dict, CACA_KEY_DELETE );
  SET_INTCONSTANT(dict, CACA_KEY_UP );
  SET_INTCONSTANT(dict, CACA_KEY_DOWN );
  SET_INTCONSTANT(dict, CACA_KEY_LEFT );
  SET_INTCONSTANT(dict, CACA_KEY_RIGHT );
  SET_INTCONSTANT(dict, CACA_KEY_INSERT );
  SET_INTCONSTANT(dict, CACA_KEY_HOME );
  SET_INTCONSTANT(dict, CACA_KEY_END );
  SET_INTCONSTANT(dict, CACA_KEY_PAGEUP );
  SET_INTCONSTANT(dict, CACA_KEY_PAGEDOWN );
  SET_INTCONSTANT(dict, CACA_KEY_F1 );
  SET_INTCONSTANT(dict, CACA_KEY_F2 );
  SET_INTCONSTANT(dict, CACA_KEY_F3 );
  SET_INTCONSTANT(dict, CACA_KEY_F4 );
  SET_INTCONSTANT(dict, CACA_KEY_F5 );
  SET_INTCONSTANT(dict, CACA_KEY_F6 );
  SET_INTCONSTANT(dict, CACA_KEY_F7 );
  SET_INTCONSTANT(dict, CACA_KEY_F8 );
  SET_INTCONSTANT(dict, CACA_KEY_F9 );
  SET_INTCONSTANT(dict, CACA_KEY_F10 );
  SET_INTCONSTANT(dict, CACA_KEY_F11 );
  SET_INTCONSTANT(dict, CACA_KEY_F12 );
  SET_INTCONSTANT(dict, CACA_KEY_F13 );
  SET_INTCONSTANT(dict, CACA_KEY_F14 );
  SET_INTCONSTANT(dict, CACA_KEY_F15 );




}


/*******************/
/* Basic functions */
/*******************/
static PyObject *
pycaca_init(PyObject *self, PyObject *args)
{
  int ret;

  ret = caca_init();
  return Py_BuildValue("i", ret);
}

static PyObject *
pycaca_set_display_time(PyObject *self, PyObject *args)
{
  int value=0;
  if (!PyArg_ParseTuple(args, "i", &value))
  caca_set_display_time(value);
  return Py_BuildValue("i", 1); /*FIXME*/
}

static PyObject *
pycaca_get_display_time(PyObject *self, PyObject *args)
{
  int ret = caca_get_display_time();
  return Py_BuildValue("i", ret);
}

static PyObject *
pycaca_get_width(PyObject *self, PyObject *args)
{
  int ret = caca_get_width();
  return Py_BuildValue("i", ret);
}

static PyObject *
pycaca_get_height(PyObject *self, PyObject *args)
{
  int ret = caca_get_height();
  return Py_BuildValue("i", ret);
}

static PyObject *
pycaca_set_size(PyObject *self, PyObject *args)
{
  int width, height;
  if (!PyArg_ParseTuple(args, "ii", &width, &height))
  caca_set_size(width, height);
  return Py_BuildValue("i", 1);  /* FIXME */
}

static PyObject *
pycaca_set_width(PyObject *self, PyObject *args)
{
  int width;

  if (!PyArg_ParseTuple(args, "i", &width));
  caca_set_width(width);

  return Py_BuildValue("i", 1);  /* FIXME */
}

static PyObject *
pycaca_set_height(PyObject *self, PyObject *args)
{
  int height;
  if (!PyArg_ParseTuple(args, "i", &height));
  caca_set_height(height);
  return Py_BuildValue("i", 1);  /* FIXME */
}

static PyObject *
pycaca_set_display_title(PyObject *self, PyObject *args)
{
  int ret;
  const char *str;
  if (!PyArg_ParseTuple(args, "s", &str));
  ret = caca_set_display_title(str);
  return Py_BuildValue("i", ret);  /* FIXME */
}

static PyObject *
pycaca_get_display_width(PyObject *self, PyObject *args)
{
  int ret = caca_get_display_width();
  return Py_BuildValue("i", ret);
}

static PyObject *
pycaca_get_display_height(PyObject *self, PyObject *args)
{
  int ret = caca_get_display_height();
  return Py_BuildValue("i", ret);
}

static PyObject *
pycaca_refresh(PyObject *self, PyObject *args)
{
  caca_refresh();
  return Py_BuildValue("i", 1); /*FIXME*/
}

static PyObject *
pycaca_end(PyObject *self, PyObject *args)
{
  caca_end();
  return Py_BuildValue("i", 1); /*FIXME*/
}


static PyObject *
pycaca_get_feature(PyObject *self, PyObject *args)
{
  int value, ret;
  if (!PyArg_ParseTuple(args, "i", &value));
  ret = caca_get_feature(value);

  return Py_BuildValue("i", ret);

}

static PyObject *
pycaca_set_feature(PyObject *self, PyObject *args)
{
  int value;
  if (!PyArg_ParseTuple(args, "i", &value));
  caca_set_feature(value);

  return Py_BuildValue("i", 1); /* FIXME */

}
static PyObject *
pycaca_get_feature_name(PyObject *self, PyObject *args)
{
  int value;
  char* ret;
  if (!PyArg_ParseTuple(args, "i", &value));

  ret = (char* const)caca_get_feature_name(value);

  return Py_BuildValue("s", ret);
}

/******************/
/* Event handling */
/******************/
static PyObject *
pycaca_get_event(PyObject *self, PyObject *args)
{
  int value, ret;
  if (!PyArg_ParseTuple(args, "i", &value));

  ret = caca_get_event(value);

  return Py_BuildValue("i", ret);
}
static PyObject *
pycaca_wait_event(PyObject *self, PyObject *args)
{
  int value, ret;
  if (!PyArg_ParseTuple(args, "i", &value));

  ret = caca_get_event(value);

  return Py_BuildValue("i", ret);
}

static PyObject *
pycaca_get_mouse_x(PyObject *self, PyObject *args)
{
  int ret = caca_get_mouse_x();
  return Py_BuildValue("i", ret);
}

static PyObject *
pycaca_get_mouse_y(PyObject *self, PyObject *args)
{
  int ret = caca_get_mouse_y();
  return Py_BuildValue("i", ret);
}

/**********************/
/* Primitives drawing */
/**********************/
static PyObject *
pycaca_draw_line(PyObject *self, PyObject *args)
{
  int x1, y1, x2, y2;
  char c;

  if (!PyArg_ParseTuple(args, "iiiic", &x1,&y1,&x2,&y2,&c));
  caca_draw_line(x1,y1,x2,y2,c);
  return Py_BuildValue("i", 1); /* FIXME */
}
static PyObject *
pycaca_draw_polyline(PyObject *self, PyObject *args)
{
  PyObject *list_x, *list_y, *item;
  int *x, *y, n, lenx, leny, i;
  char c;

  if (!PyArg_ParseTuple(args, "OOic", &list_x, &list_y, &n, &c));

  lenx = PySequence_Length(list_x);
  leny = PySequence_Length(list_y);

  x = (int*) malloc(lenx*sizeof(int));
  y = (int*) malloc(leny*sizeof(int));

  if((x == NULL) || (y==NULL))
    return NULL;                /* FIXME */

  for(i=0;i<lenx;i++)
    {
      item = PySequence_GetItem(list_x, i);
      x[i] = PyInt_AsLong(item);
    }
  for(i=0;i<leny;i++)
    {
      item = PySequence_GetItem(list_y, i);
      y[i] = PyInt_AsLong(item);
    }
  caca_draw_polyline(x,y,n,c);

  free(x); free(y);

  return Py_BuildValue("i", 1); /* FIXME */
}


static PyObject *
pycaca_draw_thin_line(PyObject *self, PyObject *args)
{
  int x1, y1, x2, y2;

  if (!PyArg_ParseTuple(args, "iiii", &x1,&y1,&x2,&y2));
  caca_draw_thin_line(x1,y1,x2,y2);
  return Py_BuildValue("i", 1); /* FIXME */
}

static PyObject *
pycaca_draw_thin_polyline(PyObject *self, PyObject *args)
{
  PyObject *list_x, *list_y, *item;
  int *x, *y, n, lenx, leny, i;

  if (!PyArg_ParseTuple(args, "OOi", &list_x, &list_y, &n));

  lenx = PySequence_Length(list_x);
  leny = PySequence_Length(list_y);

  x = (int*) malloc(lenx*sizeof(int));
  y = (int*) malloc(leny*sizeof(int));

  if((x == NULL) || (y==NULL))
    return NULL;                /* FIXME */

  for(i=0;i<lenx;i++)
    {
      item = PySequence_GetItem(list_x, i);
      x[i] = PyInt_AsLong(item);
    }
  for(i=0;i<leny;i++)
    {
      item = PySequence_GetItem(list_y, i);
      y[i] = PyInt_AsLong(item);
    }
  caca_draw_thin_polyline(x,y,n);

  free(x); free(y);

  return Py_BuildValue("i", 1); /* FIXME */
}

static PyObject *
pycaca_draw_circle(PyObject *self, PyObject *args)
{
  int x1, y1, x2;
  char c;
  if (!PyArg_ParseTuple(args, "iiic", &x1,&y1,&x2,&c));
  caca_draw_circle(x1,y1,x2,c);
  return Py_BuildValue("i", 1); /* FIXME */
}
static PyObject *
pycaca_draw_ellipse(PyObject *self, PyObject *args)
{
  int x1, y1, x2,y2;
  char c;
  if (!PyArg_ParseTuple(args, "iiiic", &x1,&y1,&x2,&y2,&c));
  caca_draw_ellipse(x1,y1,x2,y2,c);
  return Py_BuildValue("i", 1); /* FIXME */
}
static PyObject *
pycaca_draw_thin_ellipse(PyObject *self, PyObject *args)
{
  int x1, y1, x2,y2;
  if (!PyArg_ParseTuple(args, "iiii", &x1,&y1,&x2,&y2));
  caca_draw_thin_ellipse(x1,y1,x2,y2);
  return Py_BuildValue("i", 1); /* FIXME */
}
static PyObject *
pycaca_fill_ellipse(PyObject *self, PyObject *args)
{
  int x1, y1, x2,y2;
  char c;
  if (!PyArg_ParseTuple(args, "iiiic", &x1,&y1,&x2,&y2,&c));
    caca_fill_ellipse(x1,y1,x2,y2,c);
  return Py_BuildValue("i", 1); /* FIXME */
}
static PyObject *
pycaca_draw_box(PyObject *self, PyObject *args)
{
  int x1, y1, x2,y2;
  char c;
  if (!PyArg_ParseTuple(args, "iiiic", &x1,&y1,&x2,&y2,&c));
  caca_draw_box(x1,y1,x2,y2,c);
  return Py_BuildValue("i", 1); /* FIXME */
}
static PyObject *
pycaca_fill_box(PyObject *self, PyObject *args)
{
  int x1, y1, x2,y2;
  char c;
  if (!PyArg_ParseTuple(args, "iiiic", &x1,&y1,&x2,&y2,&c));
  caca_fill_box(x1,y1,x2,y2,c);
  return Py_BuildValue("i", 1); /* FIXME */
}
static PyObject *
pycaca_draw_thin_box(PyObject *self, PyObject *args)
{
  int x1, y1, x2,y2;
  if (!PyArg_ParseTuple(args, "iiiic", &x1,&y1,&x2,&y2));
  caca_draw_thin_box(x1,y1,x2,y2);
  return Py_BuildValue("i", 1); /* FIXME */
}
static PyObject *
pycaca_draw_triangle(PyObject *self, PyObject *args)
{
  int x1, y1, x2,y2, x3, y3;
  char c;
  if (!PyArg_ParseTuple(args, "iiiiiic", &x1,&y1,&x2,&y2,&x3,&y3,&c));
  caca_draw_triangle(x1,y1,x2,y2,x3,y3,c);
  return Py_BuildValue("i", 1); /* FIXME */
}
static PyObject *
pycaca_draw_thin_triangle(PyObject *self, PyObject *args)
{
  int x1, y1, x2,y2, x3, y3;
  if (!PyArg_ParseTuple(args, "iiiiii", &x1,&y1,&x2,&y2,&x3,&y3));
  caca_draw_thin_triangle(x1,y1,x2,y2,x3,y3);
  return Py_BuildValue("i", 1); /* FIXME */
}
static PyObject *
pycaca_fill_triangle(PyObject *self, PyObject *args)
{
  int x1, y1, x2,y2, x3, y3;
  char c;
  if (!PyArg_ParseTuple(args, "iiiiiic", &x1,&y1,&x2,&y2,&x3,&y3,&c));
  caca_fill_triangle(x1,y1,x2,y2,x3,y3,c);
  return Py_BuildValue("i", 1); /* FIXME */
}


/**********************/
/* Character printing */
/**********************/
static PyObject *
pycaca_set_color(PyObject *self, PyObject *args)
{
  int c1,c2;
  if (!PyArg_ParseTuple(args, "ii", &c1,&c2));

  caca_set_color(c1,c2);
  return Py_BuildValue("i", 1); /* FIXME */
}

static PyObject *
pycaca_get_fg_color(PyObject *self, PyObject *args)
{
  int ret = caca_get_fg_color();
  return Py_BuildValue("i", ret);
}

static PyObject *
pycaca_get_bg_color(PyObject *self, PyObject *args)
{
  int ret = caca_get_bg_color();
  return Py_BuildValue("i", ret);
}

static PyObject *
pycaca_get_color_name(PyObject *self, PyObject *args)
{
  int c;
  char *ret;
  if (!PyArg_ParseTuple(args, "i", &c));
  ret = (char *)caca_get_color_name(c);

  return Py_BuildValue("s", ret); /* FIXME */
}

static PyObject *
pycaca_putchar(PyObject *self, PyObject *args)
{
  int x,y;
  char c;
  if (!PyArg_ParseTuple(args, "iic", &x,&y,&c));
  caca_putchar(x,y,c);
  return Py_BuildValue("i", 1); /* FIXME */
}


static PyObject *
pycaca_putstr(PyObject *self, PyObject *args)
{
  int x,y;
  char *c;
  if (!PyArg_ParseTuple(args, "iis", &x,&y,&c));
  caca_putstr(x,y,c);

  return Py_BuildValue("i", 1); /* FIXME */
}
static PyObject *
pycaca_printf(PyObject *self, PyObject *args)
{
  int x,y;
  char *c;
  if (!PyArg_ParseTuple(args, "iic", &x,&y,&c));
  caca_putstr(x,y,(char const*)c);
  return Py_BuildValue("i", 1); /* FIXME */
}
/*static PyObject *
  caca_get_screen(PyObject *self, PyObject *args);*/
static PyObject *
pycaca_clear(PyObject *self, PyObject *args)
{
  caca_clear();
  return Py_BuildValue("i", 1); /* FIXME */
}


/********************/
/* Sprite functions */
/********************/
static PyObject *
pycaca_load_sprite(PyObject *self, PyObject *args)
{
  char *filename;
  struct caca_sprite *ret;
  /*Quick and dirty hack. Gruik.*/

  if (!PyArg_ParseTuple(args, "s", &filename));
  ret  = (struct caca_sprite*) caca_load_sprite(filename);

  return Py_BuildValue("i", PyCObject_FromVoidPtr(ret, NULL));
}

static PyObject *
pycaca_draw_sprite(PyObject *self, PyObject *args)
{
  int x, y, sprite, frame;

  if (!PyArg_ParseTuple(args, "iiii", &x, &y, &sprite,&frame));
  caca_draw_sprite(x,y, (struct caca_sprite const *)PyCObject_AsVoidPtr((void*)sprite), frame);

  return Py_BuildValue("i", 1); /* FIXME */
}

static PyObject *
pycaca_get_sprite_frames(PyObject *self, PyObject *args)
{
  int sprite, ret;
  if (!PyArg_ParseTuple(args, "i", &sprite));
  ret = caca_get_sprite_frames((struct caca_sprite const *)PyCObject_AsVoidPtr((void*)sprite));

  return Py_BuildValue("i", ret);

}

static PyObject *
pycaca_get_sprite_width(PyObject *self, PyObject *args)
{
  int sprite, ret, i;
  if (!PyArg_ParseTuple(args, "ii", &sprite, &i));
  ret = caca_get_sprite_width((struct caca_sprite const *)PyCObject_AsVoidPtr((void*)sprite),i);

  return Py_BuildValue("i", ret);

}

static PyObject *
pycaca_get_sprite_height(PyObject *self, PyObject *args)
{
  int sprite, ret, i;
  if (!PyArg_ParseTuple(args, "ii", &sprite, i));
  ret = caca_get_sprite_height((struct caca_sprite const *)PyCObject_AsVoidPtr((void*)sprite),i);

  return Py_BuildValue("i", ret);
}

static PyObject *
pycaca_get_sprite_dx(PyObject *self, PyObject *args)
{
  int sprite, ret, i;
  if (!PyArg_ParseTuple(args, "ii", &sprite), &i);
  ret = caca_get_sprite_dx((struct caca_sprite const *)PyCObject_AsVoidPtr((void*)sprite),i);

  return Py_BuildValue("i", ret);
}

static PyObject *
pycaca_get_sprite_dy(PyObject *self, PyObject *args)
{
  int sprite, ret, i;
  if (!PyArg_ParseTuple(args, "ii", &sprite), &i);
  ret = caca_get_sprite_dy((struct caca_sprite const *)PyCObject_AsVoidPtr((void*)sprite),i);

  return Py_BuildValue("i", ret);
}

static PyObject *
pycaca_free_sprite(PyObject *self, PyObject *args)
{
  int sprite;
  if (!PyArg_ParseTuple(args, "i", &sprite));
  caca_free_sprite((struct caca_sprite *)PyCObject_AsVoidPtr((void*)sprite));

  return Py_BuildValue("i", 1);  /* FIXME */
}

/*************/
/* Exporters */
/*************/
static PyObject *
pycaca_get_html(PyObject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, ""));

  return Py_BuildValue("s",caca_get_html());
}

static PyObject *
pycaca_get_html3(PyObject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, ""));

  return Py_BuildValue("s",caca_get_html3());
}

static PyObject *
pycaca_get_irc(PyObject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, ""));

  return Py_BuildValue("s",caca_get_irc());
}

static PyObject *
pycaca_get_ansi(PyObject *self, PyObject *args)
{
  int trailing;
  if (!PyArg_ParseTuple(args, "i", &trailing));

  return Py_BuildValue("s",caca_get_ansi(trailing));
}

/**********/
/* Bitmap */
/**********/
static PyObject *
pycaca_create_bitmap(PyObject *self, PyObject *args)
{
  int a,b,c,d,e,f,g,h;
  struct caca_bitmap *ret;

  if (!PyArg_ParseTuple(args, "iiiiiiii", &a,&b,&c,&d,&e,&f,&g,&h));
  ret  = (struct caca_bitmap*) caca_create_bitmap(a,b,c,d,e,f,g,h);

  return Py_BuildValue("i", PyCObject_FromVoidPtr(ret, NULL));
}

static PyObject *
pycaca_set_bitmap_palette(PyObject *self, PyObject *args)
{
  int bitmap;
  PyObject *list_r, *list_g, *list_b, *list_a, *item;
  unsigned int *r,*g,*b,*a,i;

  if (!PyArg_ParseTuple(args, "iOOOO", &bitmap, &list_r, &list_g, &list_b, &list_a));

  if((PySequence_Length(list_r)!=256) ||
     (PySequence_Length(list_g)!=256) ||
     (PySequence_Length(list_b)!=256) ||
     (PySequence_Length(list_a)!=256))
    {
      PyErr_SetString(PyExc_TypeError, "Lengths of colors lists must be 256");
    }

  r = malloc(256*sizeof(unsigned int));
  g = malloc(256*sizeof(unsigned int));
  b = malloc(256*sizeof(unsigned int));
  a = malloc(256*sizeof(unsigned int));

  for(i=0;i<256;i++)
    {
      item = PySequence_GetItem(list_r, i);
      r[i] = PyInt_AsLong(item);

      item = PySequence_GetItem(list_g, i);
      g[i] = PyInt_AsLong(item);

      item = PySequence_GetItem(list_b, i);
      b[i] = PyInt_AsLong(item);

      item = PySequence_GetItem(list_a, i);
      a[i] = PyInt_AsLong(item);
    }

  caca_set_bitmap_palette((struct caca_bitmap *)PyCObject_AsVoidPtr((void*)bitmap), r,g,b,a);
  return Py_BuildValue("i", 1); /* FIXME */
}

static PyObject *
pycaca_set_bitmap_gamma(PyObject *self, PyObject *args)
{
  int bitmap;
  float value;
  if (!PyArg_ParseTuple(args, "if", &bitmap, &value));
  caca_set_bitmap_gamma((struct caca_bitmap *)PyCObject_AsVoidPtr((void*)bitmap),value);

  return Py_BuildValue("i", 1);  /* FIXME */
}

static PyObject *
pycaca_draw_bitmap(PyObject *self, PyObject *args)
{
  PyObject *pixels, *item;
  int bitmap, x1,x2,y1,y2;
  unsigned char *buffer;
  int i;

  if (!PyArg_ParseTuple(args, "iiiiiO", &x1,&y1,&x2,&y2,&bitmap,&pixels));


  buffer = malloc(PySequence_Length(pixels)*sizeof(unsigned char));


  for(i=0;i<PySequence_Length(pixels);i++)
    {
      item = PySequence_GetItem(pixels, i);
      buffer[i] = (unsigned char)PyInt_AsLong(item);
    }



  caca_draw_bitmap(x1,y1,x2,y2, (struct caca_bitmap *)PyCObject_AsVoidPtr((void*)bitmap), (void*)buffer);

  free(buffer);

  return Py_BuildValue("i", 1);  /* FIXME */
}

static PyObject *
pycaca_free_bitmap(PyObject *self, PyObject *args)
{
  int bitmap;
  if (!PyArg_ParseTuple(args, "i", &bitmap));
  caca_free_bitmap((struct caca_bitmap *)PyCObject_AsVoidPtr((void*)bitmap));

  return Py_BuildValue("i", 1);  /* FIXME */
}

/*
void caca_draw_bitmap(int, int, int, int, struct caca_bitmap const *, void *);

*/
