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

public class Dither extends NativeObject {

  static {
    Caca.load();
  }

  private native static long createDither(int bitmapDepth, int weight, int height,
      int pitch, int redMask, int greenMask, int blueMask, int alphaMask);

  public Dither(int bitmapDepth, int weight, int height, int pitch, int redMask,
                int greenMask, int blueMask, int alphaMask) {
    this.ptr = createDither(bitmapDepth, weight, height, pitch, redMask, greenMask,
                            blueMask, alphaMask);
  }

  private static native void setDitherPalette(long ditherPtr, int[] red, int[] green,
                                              int[] blue, int[] alpha);

  public void setPalette(int[] red, int[] green, int[] blue, int[] alpha) {
    if (red.length != 256 ||
        green.length != 256 ||
        blue.length != 256 ||
        alpha.length !=256) {
      throw new IllegalArgumentException("Palette components must have 256 elements");
    }
    setDitherPalette(ptr, red, green, blue, alpha);
  }

  private static native void setDitherBrightness(long ditherPtr, float brightness);

  public void setBrightness(float brightness) {
    setDitherBrightness(ptr, brightness);
  }

  private static native float getDitherBrightness(long ditherPtr);

  public float getBrightness() {
    return getDitherBrightness(ptr);
  }

  private static native void setDitherGamma(long ditherPtr, float gama);

  public void setGamma(float gama) {
    setDitherGamma(ptr, gama);
  }

  private static native float getDitherGamma(long ditherPtr);

  public float getGamma() {
    return getDitherGamma(ptr);
  }

  private static native void setDitherContrast(long ditherPtr, float contrast);

  public void setContrast(float contrast) {
    setDitherContrast(ptr, contrast);
  }

  private static native float getDitherContrast(long ditherPtr);

  public float getContrast() {
    return getDitherContrast(ptr);
  }

  public static class AntiAliasing extends CacaObject {
    public AntiAliasing(String code, String desc) {
      super(code, desc);
    }

    public static AntiAliasing forCode(Dither dither, String code) {
      for (AntiAliasing aa : dither.getAntiAliasingList()) {
        if (aa.code.equals(code)) {
          return aa;
        }
      }
      return null;
    }
  }

  private static native String[] getDitherAntiAliasingList(long ditherPtr);

  public AntiAliasing[] getAntiAliasingList() {
    String[] tmp = getDitherAntiAliasingList(ptr);
    AntiAliasing[] aas;
    aas = new AntiAliasing[tmp.length / 2];
    for (int i = 0; 2*i < tmp.length; i++) {
      aas[i] = new AntiAliasing(tmp[2*i], tmp[2*i+1]);
    }
    return aas;
  }

  private static native void setDitherAntiAliasing(long ditherPtr, String antiAliasing);

  public void setAntiAliasing(AntiAliasing antiAliasing) {
    setDitherAntiAliasing(ptr, antiAliasing.code);
  }

  private static native String getDitherAntiAliasing(long ditherPtr);

  public AntiAliasing getAntiAliasing() {
    return AntiAliasing.forCode(this, getDitherAntiAliasing(ptr));
  }

  public static class Color extends CacaObject {
    public Color(String code, String desc) {
      super(code, desc);
    }
    public static Color forCode(Dither dither, String code) {
      for (Color color : dither.getColorList()) {
        if (color.code.equals(code)) {
          return color;
        }
      }
      return null;
    }
  }

  private static native String[] getDitherColorList(long ditherPtr);

  public Color[] getColorList() {
    String[] tmp = getDitherColorList(ptr);
    Color[] colors = new Color[tmp.length / 2];
    for (int i = 0; 2*i < tmp.length; i++) {
      colors[i] = new Color(tmp[2*i], tmp[2*i+1]);
    }
    return colors;
  }

  private static native void setDitherColor(long ditherPtr, String color);

  public void setColor(Color color) {
    setDitherColor(ptr, color.code);
  }

  private static native String getDitherColor(long ditherPtr);

  public Color getColor() {
    return Color.forCode(this, getDitherColor(ptr));
  }

  public static class Charset extends CacaObject {
    public Charset(String code, String desc) {
      super(code, desc);
    }
    public static Charset forCode(Dither dither, String code) {
      for (Charset charset : dither.getCharsetList()) {
        if (charset.code.equals(code)) {
          return charset;
        }
      }
      return null;
    }
  }

  private static native String[] getDitherCharsetList(long charsetPtr);

  public Charset[] getCharsetList() {
    String[] tmp = getDitherCharsetList(ptr);
    Charset[] charsets = new Charset[tmp.length / 2];
    for (int i = 0; 2*i < tmp.length; i++) {
      charsets[i] = new Charset(tmp[2*i], tmp[2*i+1]);
    }
    return charsets;
  }

  private static native void setDitherCharset(long ditherPtr, String charset);

  public void setCharset(Charset charset) {
    setDitherCharset(ptr, charset.code);
  }

  private static native String getDitherCharset(long ditherPtr);

  public Charset getCharset() {
    return Charset.forCode(this, getDitherCharset(ptr));
  }

  public static class Algorithm extends CacaObject {
    public Algorithm(String code, String desc) {
      super(code, desc);
    }
    public static Algorithm forCode(Dither dither, String code) {
      for (Algorithm algorithm : dither.getAlgorithmList()) {
        if (algorithm.code.equals(code)) {
          return algorithm;
        }
      }
      return null;
    }
  }

  private static native String[] getDitherAlgorithmList(long ditherPtr);

  public Algorithm[] getAlgorithmList() {
    String[] tmp = getDitherAlgorithmList(ptr);
    Algorithm[] result = new Algorithm[tmp.length / 2];
    for (int i = 0; 2*i < tmp.length; i++) {
      result[i] = new Algorithm(tmp[2*i], tmp[2*i+1]);
    }
    return result;
  }

  private static native void setDitherAlgorithm(long ditherPtr, String algorithm);

  public void setAlgorithm(Algorithm algorithm) {
    setDitherAlgorithm(ptr, algorithm.code);
  }

  private static native String getDitherAlgorithm(long ditherPtr);

  public Algorithm getAlgorithm() {
    return Algorithm.forCode(this, getDitherAlgorithm(ptr));
  }

  private static native void freeDither(long ditherPtr);

  @Override
  public void finalize() throws Throwable {
    freeDither(ptr);
    super.finalize();
  }

}
