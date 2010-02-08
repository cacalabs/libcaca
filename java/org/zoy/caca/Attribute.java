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

public class Attribute {

  static {
    Caca.load();
  }

  private Attribute() {}

  private static final native byte attributeToAnsi(int attr);

  public static byte toAnsi(int attr) {
    return attributeToAnsi(attr);
  }

  private static native byte attributeToAnsiForeground(int attr);

  public byte toAnsiForeground(int attr) {
    return attributeToAnsiForeground(attr);
  }

  private static native byte attributeToAnsiBackground(int attr);

  public byte toAnsiBackground(int attr) {
    return attributeToAnsiBackground(attr);
  }

  private static native short attributeToRgb12Foreground(int attr);

  public static short toRgb12Foreground(int attr) {
    return attributeToRgb12Foreground(attr);
  }

  private static native short attributeToRgb12Background(int attr);

  public static short toRgb12Background(int attr) {
    return attributeToRgb12Background(attr);
  }

  private static native byte[] attributeToArgb64(int attr);

  public static byte[] toArgb64(int attr) {
    return attributeToArgb64(attr);
  }

}
