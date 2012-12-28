/*
 *  libcaca++     C++ bindings for libcaca
 *  Copyright (c) 2006-2007 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                2009-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/** \file caca++.h
 *  \author Jean-Yves Lamoureux <jylam@lnxscene.org>
 *  \brief The \e libcaca++ public header.
 *
 *  This header contains the public types and functions that applications
 *  using \e libcaca++ may use.
 */
#ifndef _CACA_PP_H
#define _CACA_PP_H

#include <caca.h>

#undef __class
#if defined(_WIN32) && defined(__LIBCACA_PP__)
#   define __class class __declspec(dllexport)
#else
#   define __class class
#endif

class Canvas;

__class Charset
{
 public:
    uint32_t utf8ToUtf32(char const *, size_t *);
    size_t utf32ToUtf8(char *, uint32_t);
    uint8_t utf32ToCp437(uint32_t);
    uint32_t cp437ToUtf32(uint8_t);
};

/* Ugly, I know */
__class Font
{
 public:
    ~Font();
    Font(void const *, unsigned int);
    char const *const * getList(void);
    unsigned int getWidth();
    unsigned int getHeight();
    void renderCanvas(Canvas *, uint8_t *, unsigned int,
                               unsigned int, unsigned int);
    uint32_t const *getBlocks();

 private:
    caca_font *font;
};

__class Dither
{
 public:
    Dither(unsigned int, unsigned int, unsigned int, unsigned int,
           unsigned int, unsigned int, unsigned int, unsigned int);
    ~Dither();

    void setPalette(uint32_t r[], uint32_t g[],
                    uint32_t b[], uint32_t a[]);
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
    void Bitmap(Canvas *, int, int, int, int, void *);

 private:
    caca_dither *dither;
};

__class Canvas
{
    friend class Caca;
    friend class Dither;
    friend class Font;
 public:
    Canvas();
    Canvas(int width, int height);
    ~Canvas();

    void setSize(unsigned int w, unsigned int h);
    unsigned int getWidth(void);
    unsigned int getHeight(void);
    uint32_t getAttr(int, int);
    int setAttr(uint32_t);
    int setColorANSI(uint8_t f, uint8_t b);
    int setColorARGB(unsigned int f, unsigned int b);
    void Printf(int x, int y , char const * format, ...);
    void putChar(int x, int y, uint32_t ch);
    uint32_t getChar(int, int);
    void putStr(int x, int y, char *str);
    void Clear(void);
    void Blit(int, int, Canvas* c1, Canvas* c2);
    void Invert();
    void Flip();
    void Flop();
    void Rotate180();
    void RotateLeft();
    void RotateRight();
    void drawLine(int, int, int, int, uint32_t);
    void drawPolyline(int const x[], int const y[], int, uint32_t);
    void drawThinLine(int, int, int, int);
    void drawThinPolyline(int const x[], int const y[], int);
    void drawCircle(int, int, int, uint32_t);
    void drawEllipse(int, int, int, int, uint32_t);
    void drawThinEllipse(int, int, int, int);
    void fillEllipse(int, int, int, int, uint32_t);
    void drawBox(int, int, int, int, uint32_t);
    void drawThinBox(int, int, int, int);
    void drawCP437Box(int, int, int, int);
    void fillBox(int, int, int, int, uint32_t);
    void drawTriangle(int, int, int, int, int, int, uint32_t);
    void drawThinTriangle(int, int, int, int, int, int);
    void fillTriangle(int, int, int, int, int, int, uint32_t);
    void fillTriangleTextured(int coords[6], Canvas *tex, float uv[6]);
    int setBoundaries(caca_canvas_t *, int, int, unsigned int, unsigned int);
    unsigned int getFrameCount();
    int setFrame(unsigned int);
    int createFrame(unsigned int);
    int freeFrame(unsigned int);

    char const * const * getImportList(void);
    long int importFromMemory(void const *, size_t, char const *);
    long int importFromFile(char const *, char const *);
    char const * const * getExportList(void);
    void *exportToMemory(char const *, size_t *);

    static int Rand(int, int);
    static char const * getVersion();

 protected:
    caca_canvas_t *get_caca_canvas_t();

 private:
    caca_canvas_t *cv;
};

__class Event
{
    friend class Caca;
 public:
    enum caca_event_type
        {
            CACA_EVENT_NONE =          0x0000, /**< No event. */

            CACA_EVENT_KEY_PRESS =     0x0001, /**< A key was pressed. */
            CACA_EVENT_KEY_RELEASE =   0x0002, /**< A key was released. */
            CACA_EVENT_MOUSE_PRESS =   0x0004, /**< A mouse button was pressed. */
            CACA_EVENT_MOUSE_RELEASE = 0x0008, /**< A mouse button was released. */
            CACA_EVENT_MOUSE_MOTION =  0x0010, /**< The mouse was moved. */
            CACA_EVENT_RESIZE =        0x0020, /**< The window was resized. */

            CACA_EVENT_ANY =           0xffff  /**< Bitmask for any event. */
        } type;

 protected:
    caca_event_t e;
};

__class Caca
{
 public:
    Caca();
    Caca(Canvas *cv);
    ~Caca();

    void Attach(Canvas *cv);
    void Detach();
    void setDisplayTime(unsigned int);

    void Display();
    unsigned int getDisplayTime();
    unsigned int getWidth();
    unsigned int getHeight();
    int setTitle(char const *);
    int getEvent(unsigned int, Event*, int);
    unsigned int getMouseX();
    unsigned int getMouseY();
    void setMouse(int);

    static char const * getVersion();
 private:
    caca_display_t *dp;
};

#endif /* _CACA_PP_H */
