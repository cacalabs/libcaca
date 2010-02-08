/**
 *  libcaca       Java bindings for libcaca
 *  Copyright (c) 2009 Adrien Grand <jpountz@dinauz.org>
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "org_zoy_caca_Canvas.h"

#include "caca.h"


JNIEXPORT jint JNICALL
Java_org_zoy_caca_Canvas_getCursorX(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_cursor_x((caca_canvas_t *)ptr);
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Canvas_getCursorY (JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_cursor_y((caca_canvas_t *)ptr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_setCursorXY(JNIEnv *env, jclass cls, jlong ptr, jint x, jint y)
{
  caca_gotoxy((caca_canvas_t *)ptr, x, y);
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Canvas_getHandleX(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_canvas_handle_x((caca_canvas_t *)ptr);
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Canvas_getHandleY(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_canvas_handle_x((caca_canvas_t *)ptr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_setHandleXY(JNIEnv *env, jclass cls, jlong ptr, jint x, jint y)
{
  caca_set_canvas_handle((caca_canvas_t *)ptr, x, y);
}

JNIEXPORT jlong JNICALL
Java_org_zoy_caca_Canvas_createCanvas(JNIEnv *env, jclass cls, jint w, jint h)
{
  return (jlong)caca_create_canvas(w, h);
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Canvas_getCanvasWidth(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_canvas_width((caca_canvas_t *)ptr);
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Canvas_getCanvasHeight(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_canvas_height((caca_canvas_t *)ptr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_setCanvasSize(JNIEnv *env, jclass cls, jlong ptr, jint w, jint h)
{
  caca_set_canvas_size((caca_canvas_t *)ptr, w, h);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_clearCanvas(JNIEnv *env, jclass cls, jlong ptr)
{
  caca_clear_canvas((caca_canvas_t *)ptr);
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Canvas_getCanvasChar(JNIEnv *env, jclass cls, jlong ptr, jint x, jint y)
{
  return caca_get_char((caca_canvas_t *)ptr, x, y);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_putCanvasChar(JNIEnv *env, jclass cls, jlong ptr, jint x, jint y, jint ch)
{
  caca_put_char((caca_canvas_t *)ptr, x, y, ch);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_putCanvasString(JNIEnv *env, jclass cls, jlong ptr, jint x, jint y, jstring s)
{
  const char *chars;

  chars = (*env)->GetStringUTFChars(env, s, 0);
  caca_put_str((caca_canvas_t *)ptr, x, y, chars);
  (*env)->ReleaseStringUTFChars(env, s, chars);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_blitCanvas(JNIEnv *env, jclass cls, jlong ptr, jint x,
                                    jint y, jlong cv, jlong mask)
{
  caca_blit((caca_canvas_t *)ptr, x, y, (caca_canvas_t *)cv, (caca_canvas_t *)mask);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_setCanvasBoundaries(JNIEnv *env, jclass cls, jlong ptr,
                                             jint x, jint y, jint w, jint h)
{
  caca_set_canvas_boundaries((caca_canvas_t *)ptr, x, y, w, h);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_invertCanvas(JNIEnv *env, jclass cls, jlong ptr)
{
  caca_invert((caca_canvas_t *)ptr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_flipCanvas(JNIEnv *env, jclass cls, jlong ptr)
{
  caca_flip((caca_canvas_t *)ptr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_flopCanvas(JNIEnv *env, jclass cls, jlong ptr)
{
  caca_flop((caca_canvas_t *)ptr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_rotateCanvas180(JNIEnv *env, jclass cls, jlong ptr)
{
  caca_rotate_180((caca_canvas_t *)ptr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_rotateCanvasLeft(JNIEnv *env, jclass cls, jlong ptr)
{
  caca_rotate_left((caca_canvas_t *)ptr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_rotateCanvasRight(JNIEnv *env, jclass cls, jlong ptr)
{
  caca_rotate_right((caca_canvas_t *)ptr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_stretchCanvasLeft(JNIEnv *env, jclass cls, jlong ptr)
{
  caca_stretch_left((caca_canvas_t *)ptr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_stretchCanvasRight(JNIEnv *env, jclass cls, jlong ptr)
{
  caca_stretch_right((caca_canvas_t *)ptr);
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Canvas_getCanvasAttribute(JNIEnv *env, jclass cls, jlong ptr, jint x, jint y)
{
  return caca_get_attr((caca_canvas_t *)ptr, x, y);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_setCanvasAttribute(JNIEnv *env, jclass cls, jlong ptr, jint attr)
{
  caca_set_attr((caca_canvas_t *)ptr, attr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_putCanvasAttribute(JNIEnv *env, jclass cls, jlong ptr,
                                            jint x, jint y, jint attr)
{
  caca_put_attr((caca_canvas_t *)ptr, x, y, attr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_setCanvasColorAnsi(JNIEnv *env, jclass cls, jlong ptr, jbyte fg, jbyte bg)
{
  caca_set_color_ansi((caca_canvas_t *)ptr, fg, bg);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_setCanvasColorArgb(JNIEnv *env, jclass cls, jlong ptr, jshort fg, jshort bg)
{
  caca_set_color_argb((caca_canvas_t *)ptr, fg, bg);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_canvasDrawLine(JNIEnv *env, jclass cls, jlong ptr, jint x1, jint y1,
                                        jint x2, jint y2, jint ch)
{
  caca_draw_line((caca_canvas_t *)ptr, x1, y1, x2, y2, ch);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_canvasDrawPolyline(JNIEnv *env, jclass cls, jlong ptr,
                                            jintArray xs, jintArray ys, jint ch)
{
  jsize size;
  jint *x_elems;
  jint *y_elems;

  size = (*env)->GetArrayLength(env, xs);
  x_elems = (*env)->GetIntArrayElements(env, xs, 0);
  y_elems = (*env)->GetIntArrayElements(env, ys, 0);
  caca_draw_polyline((caca_canvas_t *)ptr, x_elems, y_elems, size, ch);
  (*env)->ReleaseIntArrayElements(env, xs, x_elems, 0);
  (*env)->ReleaseIntArrayElements(env, ys, y_elems, 0);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_canvasDrawThinLine(JNIEnv *env, jclass cls, jlong ptr,
                                            jint x1, jint y1, jint x2, jint y2)
{
  caca_draw_thin_line((caca_canvas_t *)ptr, x1, y1, x2, y2);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_canvasDrawThinPolyline(JNIEnv *env, jclass cls, jlong ptr,
                                                jintArray xs, jintArray ys)
{
  jsize size;
  jint *x_elems;
  jint *y_elems;

  size = (*env)->GetArrayLength(env, xs);
  x_elems = (*env)->GetIntArrayElements(env, xs, 0);
  y_elems = (*env)->GetIntArrayElements(env, ys, 0);
  caca_draw_thin_polyline((caca_canvas_t *)ptr, x_elems, y_elems, size);
  (*env)->ReleaseIntArrayElements(env, xs, x_elems, 0);
  (*env)->ReleaseIntArrayElements(env, ys, y_elems, 0);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_canvasDrawCircle(JNIEnv * env, jclass cls, jlong ptr,
                                          jint x, jint y, jint r, jint ch)
{
  caca_draw_circle((caca_canvas_t *)ptr, x, y, r, ch);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_canvasDrawEllipse(JNIEnv *env, jclass cls, jlong ptr,
                                           jint x, jint y, jint a, jint b, jint ch)
{
  caca_draw_ellipse((caca_canvas_t *)ptr, x, y, a, b, ch);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_canvasDrawThinEllipse(JNIEnv *env, jclass cls, jlong ptr,
                                               jint x, jint y, jint a, jint b)
{
  caca_draw_thin_ellipse((caca_canvas_t *)ptr, x, y, a, b);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_canvasFillEllipse(JNIEnv *env, jclass cls, jlong ptr, jint x,
                                           jint y, jint a, jint b, jint ch)
{
  caca_fill_ellipse((caca_canvas_t *)ptr, x, y, a, b, ch);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_canvasDrawBox(JNIEnv *env, jclass cls, jlong ptr, jint x, jint y,
                                       jint w, jint h, jint ch)
{
  caca_draw_box((caca_canvas_t *)ptr, x, y, w, h, ch);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_canvasDrawThinBox(JNIEnv *env, jclass cls, jlong ptr,
                                           jint x, jint y, jint w, jint h)
{
  caca_draw_thin_box((caca_canvas_t *)ptr, x, y, w, h);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_canvasDrawCp437Box(JNIEnv *env, jclass cls, jlong ptr,
                                            jint x, jint y, jint w, jint h)
{
  caca_draw_cp437_box((caca_canvas_t *)ptr, x, y, w, h);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_canvasFillBox(JNIEnv *env, jclass cls, jlong ptr,
                                       jint x, jint y, jint w, jint h, jint ch)
{
  caca_fill_box((caca_canvas_t *)ptr, x, y, w, h, ch);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_canvasDrawTriangle(JNIEnv *env, jclass cls, jlong ptr,
                                            jint x1, jint y1, jint x2, jint y2,
                                            jint x3, jint y3, jint ch)
{
  caca_draw_triangle((caca_canvas_t *)ptr, x1, y1, x2, y2, x3, y3, ch);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_canvasDrawThinTriangle(JNIEnv *env, jclass cls, jlong ptr,
                                                jint x1, jint y1, jint x2, jint y2,
                                                jint x3, jint y3)
{
  caca_draw_thin_triangle((caca_canvas_t *)ptr, x1, y1, x2, y2, x3, y3);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_canvasFillTriangle(JNIEnv *env, jclass cls, jlong ptr,
                                            jint x1, jint y1, jint x2, jint y2,
                                            jint x3, jint y3, jint ch)
{
  caca_fill_triangle((caca_canvas_t *)ptr, x1, y1, x2, y2, x3, y3, ch);
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Canvas_getCanvasFrameCount(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_frame_count((caca_canvas_t *)ptr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_setCanvasFrame(JNIEnv *env, jclass cls, jlong ptr, jint id)
{
  caca_set_frame((caca_canvas_t *)ptr, id);
}

JNIEXPORT jstring JNICALL
Java_org_zoy_caca_Canvas_getCanvasFrameName(JNIEnv *env, jclass cls, jlong ptr)
{
  const char *frame_name = caca_get_frame_name((caca_canvas_t *)ptr);
  return (*env)->NewStringUTF(env, frame_name);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_setCanvasFrameName(JNIEnv *env, jclass cls, jlong ptr, jstring frame_name)
{
  const char *frame_name_chars = (*env)->GetStringUTFChars(env, frame_name, 0);
  caca_set_frame_name((caca_canvas_t *)ptr, frame_name_chars);
  (*env)->ReleaseStringUTFChars(env, frame_name, frame_name_chars);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_createCanvasFrame(JNIEnv *env, jclass cls, jlong ptr, jint id)
{
  caca_create_frame((caca_canvas_t *)ptr, id);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_freeCanvasFrame(JNIEnv *env, jclass cls, jlong ptr, jint id)
{
  caca_free_frame((caca_canvas_t *)ptr, id);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_canvasRender(JNIEnv *env, jclass cls, jlong ptr, jlong font_ptr,
                                      jbyteArray buf, jint width, jint height, jint pitch)
{
  jbyte *elems = (*env)->GetByteArrayElements(env, buf, 0);
  caca_render_canvas((caca_canvas_t *)ptr, (caca_font_t *)font_ptr, elems, width, height, pitch);
  (*env)->ReleaseByteArrayElements(env, buf, elems, 0);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_canvasDitherBitmap(JNIEnv *env, jclass cls, jlong ptr, jint x, jint y,
                                            jint w, jint h, jlong dither_ptr, jbyteArray pixels)
{
  jbyte *elems = (*env)->GetByteArrayElements(env, pixels, 0);
  jsize size = (*env)->GetArrayLength(env, pixels);
  caca_dither_bitmap((caca_canvas_t *)ptr, x, y, w, h, (caca_dither_t *)dither_ptr, elems);
  (*env)->ReleaseByteArrayElements(env, pixels, elems, 0);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Canvas_freeCanvas(JNIEnv *env, jclass cls, jlong ptr)
{
  caca_free_canvas((caca_canvas_t *)ptr);
}

