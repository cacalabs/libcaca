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

package org.zoy.caca;

public abstract class Color {

  public static enum Ansi {
    BLACK((byte)0x00),
    BLUE((byte)0x01),
    GREEN((byte)0x02),
    CYAN((byte)0x03),
    RED((byte)0x04),
    MAGENTA((byte)0x05),
    BROWN((byte)0x06),
    LIGHTGREY((byte)0x07),
    DARKGREY((byte)0x08),
    LIGHTBLUE((byte)0x09),
    LIGHTGREEN((byte)0x0a),
    LIGHTCYAN((byte)0x0b),
    LIGHTRED((byte)0x0c),
    LIGHTMAGENTA((byte)0x0d),
    YELLOW((byte)0x0e),
    WHITE((byte)0x0f),
    DEFAULT((byte)0x10),
    TRANSPARENT((byte)0x20);


    private Ansi(byte code) {
      this.code = code;
    }
    protected byte code;

    public static Ansi forCode(byte code) {
      Ansi[] values = Ansi.values();
      for (Ansi color : values) {
        if (color.code == code) {
          return color;
        }
      }
      return null;
    }
  }

  // Use ints mostly because it is more convenient (no need to cast)
  public static class Argb {
    public Argb(int code) {
      if (code < 0 || code > Short.MAX_VALUE - Short.MIN_VALUE) {
        throw new IllegalArgumentException("Code should be a 16-bit unsigned integer");
      }
      this.code = (short)code;
    }
    public Argb(int alpha, int red, int green, int blue) {
      if (alpha < 0 || alpha >15) {
        throw new IllegalArgumentException("alpha should be a 4-bit unsigned integer");
      }
      if (red < 0 || red >15) {
        throw new IllegalArgumentException("red should be a 4-bit unsigned integer");
      }
      if (green < 0 || green >15) {
        throw new IllegalArgumentException("green should be a 4-bit unsigned integer");
      }
      if (blue < 0 || blue >15) {
        throw new IllegalArgumentException("blue should be a 4-bit unsigned integer");
      }
      this.code = (short)((alpha << 16) + (red << 8) + (green << 4) + blue);
    }
    public short getCode() {
      return code;
    }
    protected short code;
  }

}
