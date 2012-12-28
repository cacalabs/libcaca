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

#include "org_zoy_caca_Event.h"

#include <stdlib.h>
#include "caca.h"

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Event_getEventType(JNIEnv *env, jclass cls, jlong ptr)
{
  return (jint)caca_get_event_type((caca_event_t *)ptr);
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Event_getEventKeyCh(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_event_key_ch((caca_event_t *)ptr);
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Event_getEventKeyUtf32(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_event_key_utf32((caca_event_t *)ptr);
}

JNIEXPORT jstring JNICALL
Java_org_zoy_caca_Event_getEventKeyUtf8(JNIEnv *env, jclass cls, jlong ptr)
{
  char *str = malloc(8 * sizeof(char));
  caca_get_event_key_utf8((caca_event_t *)ptr, str);
  return (*env)->NewStringUTF(env, str);
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Event_getEventMouseButton(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_event_mouse_button((caca_event_t *)ptr);
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Event_getEventMouseX(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_event_mouse_x((caca_event_t *)ptr);
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Event_getEventMouseY(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_event_mouse_y((caca_event_t *)ptr);
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Event_getEventResizeWidth(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_event_resize_width((caca_event_t *)ptr);
}

JNIEXPORT jint JNICALL
Java_org_zoy_caca_Event_getEventResizeHeight(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_event_resize_height((caca_event_t *)ptr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Event_freeEvent(JNIEnv *env, jclass cls, jlong ptr)
{
  caca_event_t *ev = (caca_event_t *)ptr;
  free(ev);
}
