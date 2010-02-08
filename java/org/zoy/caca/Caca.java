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

package org.zoy.caca;

public class Caca {

  static {
    load();
  }

  private static boolean alreadyLoaded = false;

  synchronized public static void load() {
    if (!alreadyLoaded) {
      System.loadLibrary("caca-java");
      alreadyLoaded = true;
    }
  }

  public static native String getVersion();

}
