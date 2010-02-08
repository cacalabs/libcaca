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

#include "org_zoy_caca_Caca.h"

#include "caca.h"


JNIEXPORT jstring JNICALL
Java_org_zoy_caca_Caca_getVersion(JNIEnv *env, jclass cls)
{
  const char *version = caca_get_version();
  return (*env)->NewStringUTF(env, version);
}
