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

public class Canvas extends NativeObject {

  static {
    Caca.load();
  }

  private static native int getCursorX(long canvasPtr);
  private static native int getCursorY(long canvasPtr);
  private static native void setCursorXY(long canvasPtr, int x, int y);

  public class Cursor {

    protected Cursor() {}

    public int getX() {
      return getCursorX(ptr);
    }

    public int getY() {
      return getCursorY(ptr);
    }

    public void setXY(int x, int y) {
      setCursorXY(ptr, x, y);
    }

  }

  private static native int getHandleX(long canvasPtr);
  private static native int getHandleY(long canvasPtr);
  private static native void setHandleXY(long canvasPtr, int x, int y);

  public class Handle {

    protected Handle() {}

    public int getX() {
      return getHandleX(ptr);
    }

    public int getY() {
      return getHandleY(ptr);
    }

    public void setXY(int x, int y) {
      setHandleXY(ptr, x, y);
    }

  }

  // Is this canvas managed by a display?
  private final boolean managed;
  private final Cursor cursor;
  private final Handle handle;

  private static native long createCanvas(int width, int height);

  public Canvas(int width, int height) {
    this(createCanvas(width, height), false);
  }

  protected Canvas(long ptr) {
    this(ptr, true);
  }

  private Canvas(long ptr, boolean managed) {
    this.cursor = new Cursor();
    this.handle = new Handle();
    this.ptr = ptr;
    this.managed = managed;
  }

  public Cursor getCursor() {
    return cursor;
  }

  public Handle getHandle() {
    return handle;
  }

  private static native int getCanvasWidth(long canvasPtr);

  public int getWidth() {
    return getCanvasWidth(ptr);
  }

  private static native int getCanvasHeight(long canvasPtr);

  public int getHeight() {
    return getCanvasHeight(ptr);
  }

  private static native void setCanvasSize(long canvasPtr, int width, int height);

  public void setSize(int width, int height) {
    setCanvasSize(ptr, width, height);
  }

  private static native void clearCanvas(long canvasPtr);

  public void clear() {
    clearCanvas(ptr);
  }

  private static native int getCanvasChar(long canvasPtr, int x, int y);

  public int getChar(int x, int y) {
    return getCanvasChar(ptr, x, y);
  }

  private static native void putCanvasChar(long canvasPtr, int x, int y, int ch);

  public void put(int x, int y, int ch) {
    putCanvasChar(ptr, x, y, ch);
  }

  private static native void putCanvasString(long canvasPtr, int x, int y, String s);

  public void put(int x, int y, String s) {
    putCanvasString(ptr, x, y, s);
  }

  private static native void blitCanvas(long canvasPtr, int x, int y, long otherCanvasPtr, long maskCanvasPtr);

  public void blit(int x, int y, Canvas other, Canvas mask) {
    blitCanvas(ptr, x, y, other.ptr, mask.ptr);
  }

  private static native void setCanvasBoundaries(long canvasPtr, int x, int y, int width, int height);

  public void setBoundaries(int x, int y, int width, int height) {
    setCanvasBoundaries(ptr, x, y, width, height);
  }

  private static native void invertCanvas(long canvasPtr);

  public void invert() {
    invertCanvas(ptr);
  }

  private static native void flipCanvas(long canvasPtr);

  public void flip() {
    flipCanvas(ptr);
  }

  private static native void flopCanvas(long canvasPtr);

  public void flop() {
    flopCanvas(ptr);
  }

  private static native void rotateCanvas180(long canvasPtr);

  public void rotate180() {
    rotateCanvas180(ptr);
  }

  private static native void rotateCanvasLeft(long canvasPtr);

  public void rotateLeft() {
    rotateCanvasLeft(ptr);
  }

  private static native void rotateCanvasRight(long canvasPtr);

  public void rotateRight() {
    rotateCanvasRight(ptr);
  }

  private static native void stretchCanvasLeft(long canvasPtr);

  public void stretchLeft() {
    stretchCanvasLeft(ptr);
  }

  private static native void stretchCanvasRight(long canvasPtr);

  public void stretchRight() {
    stretchCanvasRight(ptr);
  }

  private static native int getCanvasAttribute(long canvasPtr, int x, int y);

  public int getAttribute(int x, int y) {
    return getCanvasAttribute(ptr, x, y);
  }

  private static native void setCanvasAttribute(long canvasPtr, int attr);

  public void setDefaultAttribute(int attribute) {
    setCanvasAttribute(ptr, attribute);
  }

  private static native void putCanvasAttribute(long canvasPtr, int x, int y, int attr);

  public void putAttribute(int x, int y, int attribute) {
    putCanvasAttribute(ptr, x, y, attribute);
  }

  private static native void setCanvasColorAnsi(long canvasPtr, byte colorAnsiFg, byte colorAnsiBg);

  public void setColor(Color.Ansi foreground, Color.Ansi background) {
    setCanvasColorAnsi(ptr, foreground.code, background.code);
  }

  private static native void setCanvasColorArgb(long canvasPtr, short colorArgbFg, short colorArbgBg);

  public void setColor(Color.Argb foreground, Color.Argb background) {
    setCanvasColorArgb(ptr, foreground.code, background.code);
  }

  private static native void canvasDrawLine(long canvasPtr, int x1, int y1, int x2, int y2, int ch);

  public void drawLine(int x1, int y1, int x2, int y2, int ch) {
    canvasDrawLine(ptr, x1, y1, x2, y2, ch);
  }

  private static native void canvasDrawPolyline(long canvasPtr, int[] x, int[] y, int ch);

  public void drawPolyline(int[] x, int[] y, int ch) {
    canvasDrawPolyline(ptr, x, y, ch);
  }

  private static native void canvasDrawThinLine(long canvasPtr, int x1, int y1, int x2, int y2);

  public void drawThinLine(int x1, int y1, int x2, int y2) {
    canvasDrawThinLine(ptr, x1, y1, x2, y2);
  }

  private static native void canvasDrawThinPolyline(long canvasPtr, int[] x, int[] y);

  public void drawThinPolyline(int[] x, int[] y, int ch) {
    canvasDrawThinPolyline(ptr, x, y);
  }

  private static native void canvasDrawCircle(long canvasPtr, int x, int y, int r, int ch);

  public void drawCircle(int x, int y, int r, int ch) {
    canvasDrawCircle(ptr, x, y, r, ch);
  }

  private static native void canvasDrawEllipse(long canvasPtr, int x, int y, int a, int b, int ch);

  public void drawEllipse(int x, int y, int a, int b, int ch) {
    canvasDrawEllipse(ptr, x, y, a, b, ch);
  }

  private static native void canvasDrawThinEllipse(long canvasPtr, int x, int y, int a, int b);

  public void drawThinEllipse(int x, int y, int a, int b) {
    canvasDrawThinEllipse(ptr, x, y, a, b);
  }

  private static native void canvasFillEllipse(long canvasPtr, int x, int y, int a, int b, int ch);

  public void fillEllipse(int x, int y, int a, int b, int ch) {
    canvasFillEllipse(ptr, x, y, a, b, ch);
  }

  private static native void canvasDrawBox(long canvasPtr, int x, int y, int width, int height, int ch);

  public void drawBox(int x, int y, int width, int height, int ch) {
    canvasDrawBox(ptr, x, y, width, height, ch);
  }

  private static native void canvasDrawThinBox(long canvasPtr, int x, int y, int width, int height);

  public void drawThinBox(int x, int y, int width, int height) {
    canvasDrawThinBox(ptr, x, y, width, height);
  }

  private static native void canvasDrawCp437Box(long canvasPtr, int x, int y, int width, int height);

  public void drawCp437Box(int x, int y, int width, int height) {
    canvasDrawCp437Box(ptr, x, y, width, height);
  }

  private static native void canvasFillBox(long canvasPtr, int x, int y, int width, int height, int ch);

  public void fillBox(int x, int y, int width, int height, int ch) {
    canvasFillBox(ptr, x, y, width, height, ch);
  }

  private static native void canvasDrawTriangle(long canvasPtr, int x1, int y1, int x2, int y2, int x3, int y3, int ch);

  public void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int ch) {
    canvasDrawTriangle(ptr, x1, y1, x2, y2, x3, y3, ch);
  }

  private static native void canvasDrawThinTriangle(long canvasPtr, int x1, int y1, int x2, int y2, int x3, int y3);

  public void drawThinTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    canvasDrawThinTriangle(ptr, x1, y1, x2, y2, x3, y3);
  }

  private static native void canvasFillTriangle(long canvasPtr, int x1, int y1, int x2, int y2, int x3, int y3, int ch);

  public void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int ch) {
    canvasFillTriangle(ptr, x1, y1, x2, y2, x3, y3, ch);
  }

  private static native int getCanvasFrameCount(long canvasPtr);

  public int getFrameCount() {
    return getCanvasFrameCount(ptr);
  }

  private static native void setCanvasFrame(long canvasPtr, int id);

  public void setFrame(int id) {
    setCanvasFrame(ptr, id);
  }

  private static native String getCanvasFrameName(long canvasPtr);

  public String getFrameName() {
    return getCanvasFrameName(ptr);
  }

  private static native void setCanvasFrameName(long canvasPtr, String name);

  public void setFrameName(String name) {
    setCanvasFrameName(ptr, name);
  }

  private static native void createCanvasFrame(long canvasPtr, int id);

  public void createFrame(int id) {
    createCanvasFrame(ptr, id);
  }

  private static native void freeCanvasFrame(long canvasPtr, int id);

  public void removeFrame(int id) {
    freeCanvasFrame(ptr, id);
  }

  private static native void canvasRender(long canvasPtr, long fontPtr, byte[] buf,
                                          int width, int height, int pitch);

  public void render(Font font, byte[] buf, int width, int height, int pitch) {
    canvasRender(ptr, font.ptr, buf, width, height, pitch);
  }

  private static native void canvasDitherBitmap(long canvasPtr, int x, int y, int width,
                                                int height, long ditherPtr, byte[] pixels);

  public void ditherBitmap(int x, int y, int width, int height, Dither dither, byte[] pixels) {
    canvasDitherBitmap(ptr, x, y, width, height, dither.ptr, pixels);
  }

  private static native void freeCanvas(long canvasPtr);

  @Override
  public void finalize() throws Throwable {
    if (!managed)
      freeCanvas(ptr);
    super.finalize();
  }

}
