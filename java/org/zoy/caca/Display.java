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

public class Display extends NativeObject {

  static {
    Caca.load();
  }

  private static native long createDisplayAndCanvas();
  private static native long createDisplay(long canvasPtr);
  private static native long createDisplayAndCanvasWithDriver(String driver);
  private static native long createDisplayWithDriver(long canvasPtr, String driver);

  // Keep a reference to the canvas so that it does not get collected before this.
  private Canvas canvas;

  public Display() {
    ptr = createDisplayAndCanvas();
    long canvasPtr = getDisplayCanvas(ptr);
    // Create a managed canvas
    canvas = new Canvas(canvasPtr);
  }

  public Display(Canvas canvas) {
    this.canvas = canvas;
    ptr = createDisplay(canvas.ptr);
  }

  public Display(String driver) {
    ptr = createDisplayAndCanvasWithDriver(driver);
    long canvasPtr = getDisplayCanvas(ptr);
    // Create a managed canvas
    canvas = new Canvas(canvasPtr);
  }

  public Display(Canvas canvas, Driver driver) {
    ptr = createDisplayWithDriver(canvas.ptr, driver.code);
  }

  public static class Driver extends CacaObject {
    public Driver(String code, String desc) {
      super(code, desc);
    }
    public static Driver forCode(String code) {
      for (Driver driver : getDriverList()) {
        if (driver.code.equals(code)) {
          return driver;
        }
      }
      return null;
    }
  }

  private static native String[] getDisplayDriverList();

  public static Driver[] getDriverList() {
    String[] tmp = getDisplayDriverList();
    Driver[] drivers = new Driver[tmp.length / 2];
    for (int i = 0; 2*i < tmp.length; i++) {
      drivers[i] = new Driver(tmp[2*i], tmp[2*i+1]);
    }
    return drivers;
  }

  private static native void setDisplayDriver(long displayPtr, String driver);

  public void setDriver(Driver driver) {
    setDisplayDriver(ptr, driver.code);
  }

  private static native String getDisplayDriver(long displayPtr);

  public Driver getDriver() {
    return Driver.forCode(getDisplayDriver(ptr));
  }

  private static native long getDisplayCanvas(long displayPtr);

  public Canvas getCanvas() {
    return canvas;
  }

  private static native void displayRefresh(long displayPtr);

  public void refresh() {
    displayRefresh(ptr);
  }

  private static native void setDisplayTime(long displayPtr, int time);

  public void setTime(int time) {
    setDisplayTime(ptr, time);
  }

  private static native int getDisplayTime(long displayPtr);

  public int getTime() {
    return getDisplayTime(ptr);
  }

  private static native int getDisplayWidth(long displayPtr);

  public int getWidth() {
    return getDisplayWidth(ptr);
  }

  private static native int getDisplayHeight(long displayPtr);

  public int getHeight() {
    return getDisplayHeight(ptr);
  }

  private static native void setDisplayTitle(long displayPtr, String title);

  public void setTitle(String title) {
    setDisplayTitle(ptr, title);
  }

  private static native void setDisplayMouse(long displayPtr, boolean status);

  public void setMouse(boolean status) {
    setDisplayMouse(ptr, status);
  }

  private static native void setDisplayCursor(long displayPtr, boolean status);

  public void setCursor(boolean status) {
    setDisplayCursor(ptr, status);
  }

  private static native long getDisplayEvent(long displayPtr, int eventMask, int timeout);

  public Event getEvent(Event.Type type, int timeout) {
    return new Event(getDisplayEvent(ptr, type.code, timeout));
  }

  private static native int getDisplayMouseX(long eventPtr);

  public int getMouseX() {
    return getDisplayMouseX(ptr);
  }

  private static native int getDisplayMouseY(long eventPtr);

  public int getMouseY() {
    return getDisplayMouseY(ptr);
  }

  private static native void freeDisplay(long displayPtr);

  @Override
  public void finalize() throws Throwable {
    freeDisplay(ptr);
    super.finalize();
  }

}
