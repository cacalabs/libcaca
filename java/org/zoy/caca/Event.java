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

public class Event extends NativeObject {

  static {
    Caca.load();
  }

  public static enum Type {
    NONE(0x0000),
    KEY_PRESS(0x0001),
    KEY_RELEASE(0x0002),
    MOUSE_PRESS(0x0004),
    MOUSE_RELEASE(0x0008),
    MOUSE_MOTION(0x0010),
    RESIZE(0x0020),
    QUIT(0x0040),
    ANY(0xffff);

    protected final int code;
    private Type(int code) {
      this.code = code;
    }
    public static Type forCode(int code) {
      Type[] types = Type.values();
      for (Type type : types) {
        if (type.code == code) {
          return type;
        }
      }
      return null;
    }
  }

  public static enum Key {
    CTRL_A(0x01),
    CTRL_B(0x02),
    CTRL_C(0x03),
    CTRL_D(0x04),
    CTRL_E(0x05),
    CTRL_F(0x06),
    CTRL_G(0x07),
    BACKSPACE(0x08),
    TAB(0x09),
    CTRL_J(0x0a),
    CTRL_K(0x0b),
    CTRL_L(0x0c),
    RETURN(0x0d),
    CTRL_N(0x0e),
    CTRL_O(0x0f),
    CTRL_P(0x10),
    CTRL_Q(0x11),
    CTRL_R(0x12),
    PAUSE(0x13),
    CTRL_T(0x14),
    CTRL_U(0x15),
    CTRL_V(0x16),
    CTRL_W(0x17),
    CTRL_X(0x18),
    CTRL_Y(0x19),
    CTRL_Z(0x20),

    UP(0x111),
    DOWN(0x112),
    LEFT(0x113),
    RIGHT(0x114),

    INSERT(0x115),
    HOME(0x116),
    END(0x117),
    PAGE_HOME(0x118),
    PAGE_DOWN(0x119),

    F1(0x11a),
    F2(0x11b),
    F3(0x11c),
    F4(0x11d),
    F5(0x11e),
    F6(0x11f),
    F7(0x120),
    F8(0x121),
    F9(0x122),
    F10(0x123),
    F11(0x124),
    F12(0x125),
    F13(0x126),
    F14(0x127),
    F15(0x128);

    protected final int code;
    private Key(int code) {
      this.code = code;
    }
    public static Key forCode(int code) {
      Key[] keys = Key.values();
      for (Key key : keys) {
        if (key.code == code) {
          return key;
        }
      }
      return null;
    }
  }

  protected Event(long ptr) {
    this.ptr = ptr;
  }

  private static native int getEventType(long eventPtr);

  public Type getType() {
    return Type.forCode(getEventType(ptr));
  }

  private static native int getEventKeyCh(long eventPtr);

  public int getKeyCh() {
    return getEventKeyCh(ptr);
  }

  private static native int getEventKeyUtf32(long eventPtr);

  public int getKeyUtf32() {
    return getEventKeyUtf32(ptr);
  }

  private static native String getEventKeyUtf8(long eventPtr);

  public String getKeyUtf8() {
    return getEventKeyUtf8(ptr);
  }

  private static native int getEventMouseButton(long eventPtr);

  public int getMouseButton() {
    return getEventMouseButton(ptr);
  }

  private static native int getEventMouseX(long eventPtr);

  public int getMouseX() {
    return getEventMouseX(ptr);
  }

  private static native int getEventMouseY(long eventPtr);

  public int getMouseY() {
    return getEventMouseY(ptr);
  }

  private static native int getEventResizeWidth(long eventPtr);

  public int getResizeWidth() {
    return getEventResizeWidth(ptr);
  }

  private static native int getEventResizeHeight(long eventPtr);

  public int getResizeHeight() {
    return getEventResizeHeight(ptr);
  }

  private static native void freeEvent(long eventPtr);

  @Override
  public void finalize() throws Throwable {
    freeEvent(ptr);
    super.finalize();
  }

}
