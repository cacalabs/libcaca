/*
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

jobjectArray caca_java_to_string_array(JNIEnv *env, const char *const *v)
{
  jclass java_lang_string = (*env)->FindClass(env, "java/lang/String");
  jsize size;
  jsize i;

  for (size = 0; v[size]; ++size);

  jobjectArray ret = (*env)->NewObjectArray(env, size, java_lang_string, NULL);
  for (i = 0; i < size; ++i)
  {
    (*env)->SetObjectArrayElement(env, ret, i, (*env)->NewStringUTF(env, v[i]));
  }
  return ret;
}

