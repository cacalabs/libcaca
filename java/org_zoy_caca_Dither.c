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

#include "org_zoy_caca_Dither.h"
#include "caca_java_common.h"

#include "caca.h"

JNIEXPORT jlong JNICALL
Java_org_zoy_caca_Dither_createDither(JNIEnv *env, jclass cls, jint bpp, jint w, jint h,
                                      jint pitch, jint rmask, jint gmask, jint bmask, jint amask)
{
  caca_dither_t *dither = caca_create_dither(bpp, w, h, pitch, rmask, gmask, bmask, amask);

  if(dither == NULL) {
    THROW_EX("Cannot create a new Dither");
    return 0;
  }

  return (jlong)dither;
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Dither_setDitherPalette(JNIEnv *env, jclass vls, jlong ptr, jintArray red,
                                          jintArray green, jintArray blue, jintArray alpha)
{
  jint *relems, *gelems, *belems, *aelems;

  relems = (*env)->GetIntArrayElements(env, red, 0);
  gelems = (*env)->GetIntArrayElements(env, green, 0);
  belems = (*env)->GetIntArrayElements(env, blue, 0);
  aelems = (*env)->GetIntArrayElements(env, alpha, 0);
  caca_set_dither_palette((caca_dither_t *)ptr, relems, gelems, belems, aelems);
  (*env)->ReleaseIntArrayElements(env, red, relems, 0);
  (*env)->ReleaseIntArrayElements(env, green, gelems, 0);
  (*env)->ReleaseIntArrayElements(env, blue, belems, 0);
  (*env)->ReleaseIntArrayElements(env, alpha, aelems, 0);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Dither_setDitherBrightness(JNIEnv *env, jclass cls, jlong ptr, jfloat v)
{
  caca_set_dither_brightness((caca_dither_t *)ptr, v);
}

JNIEXPORT jfloat JNICALL
Java_org_zoy_caca_Dither_getDitherBrightness(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_dither_brightness((caca_dither_t *)ptr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Dither_setDitherGamma(JNIEnv *env, jclass cls, jlong ptr, jfloat v)
{
  caca_set_dither_gamma((caca_dither_t *)ptr, v);
}

JNIEXPORT jfloat JNICALL
Java_org_zoy_caca_Dither_getDitherGamma(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_dither_gamma((caca_dither_t *)ptr);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Dither_setDitherContrast(JNIEnv *env, jclass cls, jlong ptr, jfloat v)
{
  caca_set_dither_contrast((caca_dither_t *)ptr, v);
}

JNIEXPORT jfloat JNICALL
Java_org_zoy_caca_Dither_getDitherContrast(JNIEnv *env, jclass cls, jlong ptr)
{
  return caca_get_dither_contrast((caca_dither_t *)ptr);
}

JNIEXPORT jobjectArray JNICALL
Java_org_zoy_caca_Dither_getDitherAntiAliasingList(JNIEnv *env, jclass cls, jlong ptr)
{
  const char *const *antialias_list = caca_get_dither_antialias_list((caca_dither_t *)ptr);
  return caca_java_to_string_array(env, antialias_list);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Dither_setDitherAntiAliasing(JNIEnv *env, jclass cls, jlong ptr, jstring aa)
{
  const char *aa_chars = (*env)->GetStringUTFChars(env, aa, 0);
  caca_set_dither_antialias((caca_dither_t *)ptr, aa_chars);
  (*env)->ReleaseStringUTFChars(env, aa, aa_chars);
}

JNIEXPORT jstring JNICALL
Java_org_zoy_caca_Dither_getDitherAntiAliasing(JNIEnv *env, jclass cls, jlong ptr)
{
  return (*env)->NewStringUTF(env, caca_get_dither_antialias((caca_dither_t *)ptr));
}

JNIEXPORT jobjectArray
JNICALL Java_org_zoy_caca_Dither_getDitherColorList(JNIEnv *env, jclass cls, jlong ptr)
{
  const char *const *color_list = caca_get_dither_color_list((caca_dither_t *)ptr);
  return caca_java_to_string_array(env, color_list);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Dither_setDitherColor(JNIEnv *env, jclass cls, jlong ptr, jstring color)
{
  const char *color_chars = (*env)->GetStringUTFChars(env, color, 0);
  caca_set_dither_color((caca_dither_t *)ptr, color_chars);
  (*env)->ReleaseStringUTFChars(env, color, color_chars);
}

JNIEXPORT jstring JNICALL
Java_org_zoy_caca_Dither_getDitherColor(JNIEnv *env, jclass cls, jlong ptr)
{
  return (*env)->NewStringUTF(env, caca_get_dither_color((caca_dither_t *)ptr));
}

JNIEXPORT jobjectArray JNICALL
Java_org_zoy_caca_Dither_getDitherCharsetList(JNIEnv *env, jclass cls, jlong ptr)
{
  const char *const *color_list = caca_get_dither_color_list((caca_dither_t *)ptr);
  return caca_java_to_string_array(env, color_list);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Dither_setDitherCharset(JNIEnv *env, jclass cls, jlong ptr, jstring charset)
{
  const char *charset_chars = (*env)->GetStringUTFChars(env, charset, 0);
  caca_set_dither_charset((caca_dither_t *)ptr, charset_chars);
  (*env)->ReleaseStringUTFChars(env, charset, charset_chars);
}

JNIEXPORT jstring JNICALL
Java_org_zoy_caca_Dither_getDitherCharset(JNIEnv *env, jclass cls, jlong ptr)
{
  return (*env)->NewStringUTF(env, caca_get_dither_charset((caca_dither_t *)ptr));
}

JNIEXPORT jobjectArray JNICALL
Java_org_zoy_caca_Dither_getDitherAlgorithmList(JNIEnv *env, jclass cls, jlong ptr)
{
  const char *const *algorithm_list = caca_get_dither_algorithm_list((caca_dither_t *)ptr);
  return caca_java_to_string_array(env, algorithm_list);
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Dither_setDitherAlgorithm(JNIEnv *env, jclass cls, jlong ptr, jstring algorithm)
{
  const char *algorithm_chars = (*env)->GetStringUTFChars(env, algorithm, 0);
  caca_set_dither_algorithm((caca_dither_t *)ptr, algorithm_chars);
  (*env)->ReleaseStringUTFChars(env, algorithm, algorithm_chars);
}

JNIEXPORT jstring JNICALL
Java_org_zoy_caca_Dither_getDitherAlgorithm(JNIEnv *env, jclass cls, jlong ptr)
{
  return (*env)->NewStringUTF(env, caca_get_dither_algorithm((caca_dither_t *)ptr));
}

JNIEXPORT void JNICALL
Java_org_zoy_caca_Dither_freeDither(JNIEnv *env, jclass cls, jlong ptr)
{
  caca_free_dither((caca_dither_t *)ptr);
}

