/*
 *  libcaca       Java bindings for libcaca
 *  Copyright (c) 2009 Adrien Grand <jpountz@dinauz.org>
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#include "org_zoy_caca_Attribute.h"

#include "caca.h"


JNIEXPORT jbyte JNICALL
Java_org_zoy_caca_Attribute_attributeToAnsi(JNIEnv *env, jclass cls, jint attr)
{
  return caca_attr_to_ansi(attr);
}

JNIEXPORT jbyte JNICALL
Java_org_zoy_caca_Attribute_attributeToAnsiForeground(JNIEnv *env, jclass cls, jint attr)
{
  return caca_attr_to_ansi_fg(attr);
}

JNIEXPORT jbyte JNICALL
Java_org_zoy_caca_Attribute_attributeToAnsiBackground(JNIEnv *env, jclass cls, jint attr)
{
  return caca_attr_to_ansi_bg(attr);
}

JNIEXPORT jshort JNICALL
Java_org_zoy_caca_Attribute_attributeToRgb12Foreground(JNIEnv *env, jclass cls, jint attr)
{
  return caca_attr_to_rgb12_fg(attr);
}

JNIEXPORT jshort JNICALL
Java_org_zoy_caca_Attribute_attributeToRgb12Background(JNIEnv *env, jclass cls, jint attr)
{
  return caca_attr_to_rgb12_bg(attr);
}

JNIEXPORT jbyteArray JNICALL
Java_org_zoy_caca_Attribute_attributeToArgb64(JNIEnv *env, jclass cls, jint attr)
{
  jbyteArray ret;
  jbyte *elems;

  ret = (*env)->NewByteArray(env, 8);
  elems = (*env)->GetByteArrayElements(env, ret, 0);
  caca_attr_to_argb64(attr, elems);
  (*env)->ReleaseByteArrayElements(env, ret, elems, 0);

  return ret;
}

