/*
 *  libcucul++    C++ bindings for libcucul
 *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file cucul++.h
 *  \version \$Id$
 *  \author Jean-Yves Lamoureux <jylam@lnxscene.org>
 *  \brief The \e libcucul++ public header.
 *
 *  This header contains the public types and functions that applications
 *  using \e libcucul++ may use.
 */

#ifndef _CUCUL_PP_H
#define _CUCUL_PP_H

#include <cucul.h>

class Cucul;

class Charset
{
 public:
    unsigned long int utf8ToUtf32(char const *, unsigned int *);
    unsigned int utf32ToUtf8(char *, unsigned long int);
    unsigned char utf32ToCp437(unsigned long int);
    unsigned long int cp437ToUtf32(unsigned char);
};

/* Ugly, I know */
class Font
{
 public:
    ~Font();
    Font(void const *, unsigned int);
    char const *const * getList(void);
    unsigned int getWidth();
    unsigned int getHeight();
    void renderCanvas(Cucul *, unsigned char *, unsigned int,
                               unsigned int, unsigned int);
    unsigned long int const *getBlocks();

 private:
    cucul_font *font;
};

class Dither
{
 public:
    Dither(unsigned int, unsigned int, unsigned int, unsigned int,
           unsigned int, unsigned int, unsigned int, unsigned int);
    ~Dither();

    void setPalette(unsigned int r[], unsigned int g[],
                    unsigned int b[], unsigned int a[]);
    void setBrightness(float);
    void setGamma(float);
    void setContrast(float);
    void setAntialias(char const *);
    char const *const * getAntialiasList();
    void setColor(char const *);
    char const *const * getColorList();
    void setCharset(char const *);
    char const *const * getCharsetList();
    void setMode(char const *);
    char const *const * getModeList();
    void Bitmap(Cucul *, int, int, int, int, void *);

 private:
    cucul_dither *dither;
};

class Cucul
{
    friend class Caca;
    friend class Dither;
    friend class Font;
 public:
    Cucul();
    Cucul(int width, int height);
    ~Cucul();

    void setSize(unsigned int w, unsigned int h);
    unsigned int getWidth(void);
    unsigned int getHeight(void);
    unsigned long int getAttr(int, int);
    int setAttr(unsigned long int);
    int setColorANSI(unsigned char f, unsigned char b);
    int setColorARGB(unsigned int f, unsigned int b);
    void Printf(int x, int y , char const * format, ...);
    void putChar(int x, int y, unsigned long int ch);
    unsigned long int getChar(int, int);
    void putStr(int x, int y, char *str);
    void Clear(void);
    void Blit(int, int, Cucul* c1, Cucul* c2);
    void Invert();
    void Flip();
    void Flop();
    void Rotate();
    void drawLine(int, int, int, int, unsigned long int);
    void drawPolyline(int const x[], int const y[], int, unsigned long int);
    void drawThinLine(int, int, int, int);
    void drawThinPolyline(int const x[], int const y[], int);
    void drawCircle(int, int, int, unsigned long int);
    void drawEllipse(int, int, int, int, unsigned long int);
    void drawThinEllipse(int, int, int, int);
    void fillEllipse(int, int, int, int, unsigned long int);
    void drawBox(int, int, int, int, unsigned long int);
    void drawThinBox(int, int, int, int);
    void drawCP437Box(int, int, int, int);
    void fillBox(int, int, int, int, unsigned long int);
    void drawTriangle(int, int, int, int, int, int, unsigned long int);
    void drawThinTriangle(int, int, int, int, int, int);
    void fillTriangle(int, int, int, int, int, int, unsigned long int);
    int Rand(int, int);
    int setBoundaries(cucul_canvas_t *, int, int, unsigned int, unsigned int);
    unsigned int getFrameCount();
    int setFrame(unsigned int);
    int createFrame(unsigned int);
    int freeFrame(unsigned int);

    char const * const * getImportList(void);
    long int importMemory(void const *, unsigned long int, char const *);
    long int importFile(char const *, char const *);
    char const * const * getExportList(void);
    void *exportMemory(char const *, unsigned long int *);

 protected:
    cucul_canvas_t *get_cucul_canvas_t();

 private:
    cucul_canvas_t *cv;
};

#endif /* _CUCUL_PP_H */
