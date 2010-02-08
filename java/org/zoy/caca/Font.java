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

public class Font extends NativeObject {

  static {
    Caca.load();
  }

  private static native long loadFont(String fontName);
  private static native long loadFont(byte[] fontBytes);

  public Font(String fontName) {
    ptr = loadFont(fontName);
  }

  public Font(byte[] fontBytes) {
    ptr = loadFont(fontBytes);
  }

  public static native String[] getFontNames();

  private static native int getFontWidth(long fontPtr);

  public int getWidth() {
    return getFontWidth(ptr);
  }

  private static native int getFontHeight(long fontPtr);

  public int getHeight() {
    return getFontHeight(ptr);
  }

  private static native int[][] getFontBlocks(long fontPtr);

  public int[][] getBlocks() {
    return getFontBlocks(ptr);
  }

  private static native void freeFont(long fontPtr);

  @Override
  public void finalize() throws Throwable {
    freeFont(ptr);
    super.finalize();
  }

}
