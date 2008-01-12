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

/*
 *  This file contains the main functions used by \e libcaca++ applications to
 *  initialise the library, get the screen properties, set the framerate and
 *  so on.
 */

#include "config.h"

#include <iostream>

#include "caca++.h"

Caca::Caca(Cucul *cv)
{
    dp = caca_create_display(cv->get_cucul_canvas_t());
    if(!dp)
        throw -1;
}

Caca::~Caca()
{
    caca_free_display(dp);
}

void Caca::Attach(Cucul *cv)
{
    dp = caca_create_display(cv->get_cucul_canvas_t());
    if(!dp)
        throw -1;
}

void Caca::Detach()
{
    caca_free_display(dp);
}

void Caca::setDisplayTime(unsigned int d)
{
    caca_set_display_time(dp, d);
}

void Caca::Display()
{
    caca_refresh_display(dp);
}

unsigned int Caca::getDisplayTime()
{
    return caca_get_display_time(dp);
}

unsigned int Caca::getWidth()
{
    return caca_get_display_width(dp);
}

unsigned int Caca::getHeight()
{
    return caca_get_display_height(dp);
}

int Caca::setTitle(char const *s)
{
    return caca_set_display_title(dp, s);
}

int Caca::getEvent(unsigned int g, Event *n, int aa)
{
    return caca_get_event(dp, g, n ? &n->e : NULL, aa);
}

unsigned int Caca::getMouseX()
{
    return caca_get_mouse_x(dp);
}

unsigned int Caca::getMouseY()
{
    return caca_get_mouse_x(dp);
}

void Caca::setMouse(int v)
{
    caca_set_mouse(dp, v);
}

const char * Caca::getVersion()
{
    return caca_get_version();
}

