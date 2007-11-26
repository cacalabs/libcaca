/*
 *  libcaca++      C++ bindings for libcaca
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

/** \file caca++.h
 *  \version \$Id$
 *  \author Jean-Yves Lamoureux <jylam@lnxscene.org>
 *  \brief The \e libcaca++ public header.
 *
 *  This header contains the public types and functions that applications
 *  using \e libcaca++ may use.
 */
#ifndef _CACA_PP_H
#define _CACA_PP_H

#include <cucul.h>
#include <caca.h>

#include <cucul++.h>

#undef __class
#if defined(_WIN32) && defined(__LIBCACA_PP__)
#   define __class class __declspec(dllexport)
#else
#   define __class class
#endif

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
    Caca(Cucul *cv);
    ~Caca();

    void Attach(Cucul *cv);
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
