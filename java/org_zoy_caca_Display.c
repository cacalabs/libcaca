/**
 *  libcaca       Java bindings for libcaca
 *  Copyright (c) 2009 Adrien Grand <jpountz@dinauz.org>
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#include "caca_java_common.h"
#include "org_zoy_caca_Display.h"

#include <stdlib.h>
#include "caca.h"

#define DISPLAY_CREATION_ERROR "Cannot create display"

JNIEXPORT jlong JNICALL
Java_org_zoy_caca_Display_createDisplayAndCanvas(JNIEnv *env, jclass cls)
{
  caca_display_t *display = caca_create_display(NULL);

  if(display == NULL) {
    THROW_EX(DISPLAY_CREATION_ERROR);
    return 0;
  }

  return (jlong)display;
}

JNIEXPORT jlong JNICALL
Java_org_zoy_caca_Display_createDisplay(JNIEnv *env, jclass cls, jlong canvas_ptr)
{
  caca_display_t *display = caca_create_display((caca_canvas_t *)canvas_ptr);

  if(display == NULL) {
    THROW_EX(DISPLAY_CREATION_ERROR);
    return 0;
  }

  return (jlong)display;
}

JNIEXPORT jlong JNICALL
Java_org_zoy_caca_Display_createDisplayAndCanvasWithDriver(JNIEnv *env, jclass cls,
                                                           jstring driver_name)
{
  const char *driver_name_chars = (*env)->GetStringUTFChars(env, driver_name, 0);
  caca_display_t *ret = caca_create_display_with_driver(NULL, driver_name_chars);
  (*env)->ReleaseStringUTFChars(env, driver_name, driver_name_chars);

  if(ret == NULL) {
    THROW_EX(DISPLAY_CREATION_ERROR);
    return 0;
  }

  return (jlong)ret;
}

JNIEXPORT jlong JNICALL
Java_org_zoy_caca_Display_createDisplayWithDriver(JNIEnv *env, jclass cls,
                                                  jlong canvas_ptr, jstring driver_name)
{
  const char *driver_name_chars = (*env)->GetStringUTFChars(env, driver_name, 0);
  caca_display_t *ret = caca_create_display_with_driver((caca_canvas_t *)canvas_ptr, driver_name_chars);
  (*env)->ReleaseStringUTFChars(env, driver_name, driver_name_chars);

  if(ret == NULL) {
    THROW_EX(DISPLAY_CREATION_ERROR);
    return 0;
  }

  return (jlong)ret;
}

JNIEXPORT jobjectArray JNICALL
Java_org_zoy_caca_Display_getDisplayDriverList(JNIEnv *env, jclass cls)
{
  const char *const *drivers = caca_get_display_driver_list();
  return caca_java_to_string_array(env, drivers);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Display_setDisplayDriver(JNIEnv *env, jclass cls, jlong ptr, jstring driver_name)
{
  const char *driver_name_chars = (*env)->GetStringUTFChars(env, driver_name, 0);
  caca_set_display_driver((caca_display_t *)ptr, driver_name_chars);
  (*env)->ReleaseStringUTFChars(env, driver_name, driver_name_chars);
}

JNIEXPORT jstring JNICALL
Java_org_zoy_caca_Display_getDisplayDriver(JNIEnv *env, jclass cls, jlong ptr)
{
  return (*env)->NewStringUTF(env, caca_get_display_driver((caca_display_t *)ptr));
}

JNIEXPORT jlong JNICALL
Java_org_zoy_caca_Display_getDisplayCanvas(JNIEnv *env, jclass cls, jlong ptr)
{
  return (jlong)caca_get_canvas((caca_display_t *)ptr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Display_displayRefresh(JNIEnv *env, jclass cls, jlong ptr)
{
  caca_refresh_display((caca_display_t *)ptr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Display_setDisplayTime(JNIEnv *env, jclass cls, jlong ptr, jint time)
{
  caca_set_display_time((caca_display_t *)ptr, time);
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Display_getDisplayTime(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_display_time((caca_display_t *)ptr);
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Display_getDisplayWidth(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_display_width((caca_display_t *)ptr);
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Display_getDisplayHeight(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_display_height((caca_display_t *)ptr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Display_setDisplayTitle(JNIEnv *env, jclass cls, jlong ptr, jstring title)
{
  const char *title_chars = (*env)->GetStringUTFChars(env, title, 0);
  caca_set_display_title((caca_display_t *)ptr, title_chars);
  (*env)->ReleaseStringUTFChars(env, title, title_chars);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Display_setDisplayMouse(JNIEnv *env, jclass cls, jlong ptr, jboolean st)
{
  caca_set_mouse((caca_display_t *)ptr, st);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Display_setDisplayCursor(JNIEnv *env, jclass cls, jlong ptr, jboolean st)
{
  caca_set_cursor((caca_display_t *)ptr, st);
}

JNIEXPORT jlong JNICALL
Java_org_zoy_caca_Display_getDisplayEvent(JNIEnv *env, jclass cls, jlong ptr, jint mask, jint timeout)
{
  caca_event_t *ev = malloc(sizeof(caca_event_t));
  if (caca_get_event((caca_display_t *)ptr, mask, ev, timeout))
  {
    return (jlong) ev;
  }
  free(ev);
  (*env)->ThrowNew(env, (*env)->FindClass(env, "org/zoy/caca/TimeoutException"), "No event received");
  return -1;
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Display_getDisplayMouseX(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_mouse_x((caca_display_t *)ptr);
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Display_getDisplayMouseY(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_mouse_y((caca_display_t *)ptr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Display_freeDisplay(JNIEnv *env, jclass cls, jlong ptr)
{
  caca_free_display((caca_display_t *)ptr);
}

