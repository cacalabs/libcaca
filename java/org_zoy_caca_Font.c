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

#include "caca_java_common.h"
#include "org_zoy_caca_Font.h"

#include "caca.h"

#define FONT_CREATION_ERROR "Cannot load font"

JNIEXPORT jlong JNICALL
Java_org_zoy_caca_Font_loadFont__Ljava_lang_String_2(JNIEnv *env, jclass cls, jstring font_name)
{
  const char *font_name_chars;
  caca_font_t *font;

  font_name_chars = (*env)->GetStringUTFChars(env, font_name, 0);
  font = caca_load_font(font_name_chars, 0);
  (*env)->ReleaseStringUTFChars(env, font_name, font_name_chars);

  if (!font)
  {
    THROW_EX(FONT_CREATION_ERROR);
    return 0;
  }

  return (jlong)font;
}


JNIEXPORT jlong JNICALL
Java_org_zoy_caca_Font_loadFont___3B(JNIEnv *env, jclass cls, jbyteArray font_bytes)
{
  jbyte *elems;
  jsize size;
  caca_font_t *font;

  elems = (*env)->GetByteArrayElements(env, font_bytes, 0);
  size = (*env)->GetArrayLength(env, font_bytes);
  font = caca_load_font(elems, size);
  (*env)->ReleaseByteArrayElements(env, font_bytes, elems, 0);

  if (!font)
  {
    THROW_EX(FONT_CREATION_ERROR);
    return 0;
  }

  return (jlong)font;
}


JNIEXPORT jobjectArray JNICALL
Java_org_zoy_caca_Font_getFontNames(JNIEnv *env, jclass cls)
{
  const char *const *fonts = caca_get_font_list();
  return caca_java_to_string_array(env, fonts);
}


JNIEXPORT jint JNICALL
Java_org_zoy_caca_Font_getFontWidth(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_font_width(ptr);
}


JNIEXPORT jint JNICALL
Java_org_zoy_caca_Font_getFontHeight(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_font_height(ptr);
}


JNIEXPORT jobjectArray JNICALL
Java_org_zoy_caca_Font_getFontBlocks(JNIEnv *env, jclass cls, jlong ptr)
{
  const uint32_t *blocks;
  jclass int_array;
  jobjectArray ret;
  jsize size;
  jsize i;

  blocks = caca_get_font_blocks(ptr);
  for (size = 0; blocks[2*size] || blocks[2*size+1]; ++size);

  int_array = (*env)->FindClass(env, "[I");
  ret = (*env)->NewObjectArray(env, size, int_array, NULL);
  for (i = 0; i < size; ++i)
  {
    jintArray nth = (*env)->NewIntArray(env, 2); /* Size is 2 */
    jint nth_elems[] = { blocks[2*i], blocks[2*i+1] };
    (*env)->SetIntArrayRegion(env, nth, 0, 2, nth_elems);
    (*env)->SetObjectArrayElement(env, ret, i, nth);
    (*env)->DeleteLocalRef(env, nth);
  }

  return ret;
}


JNIEXPORT void JNICALL
Java_org_zoy_caca_Font_freeFont(JNIEnv *env, jclass cls, jlong ptr)
{
  caca_free_font((caca_font_t *)ptr);
}

