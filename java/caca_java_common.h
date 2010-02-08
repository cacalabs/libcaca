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

#ifndef __CACA_JAVA_COMMON_H__
#define __CACA_JAVA_COMMON_H__

#include <jni.h>

#define THROW_EX(msg) (*env)->ThrowNew(env, (*env)->FindClass(env, "org/zoy/caca/CacaException"), msg)

jobjectArray caca_java_to_string_array(JNIEnv *env, const char *const *v);

#endif
