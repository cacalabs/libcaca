/*
 *  libcaca++      C++ bindings for libcaca
 *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *  This file contains the main functions used by \e libcaca++ applications to
 *  initialise the library, get the screen properties, set the framerate and
 *  so on.
 */


#include "caca++.h"

Caca::Caca(void) 
{

}

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

void Caca::attach(Cucul *cv)
{
    dp = caca_create_display(cv->get_cucul_canvas_t());
    if(!dp)
        throw -1;
}

void Caca::detach()
{
    caca_free_display(dp);
}

void Caca::set_delay(unsigned int d)
{
    caca_set_delay(dp, d);
}

void Caca::display()
{
    caca_refresh_display(dp);
}

unsigned int Caca::get_rendertime()
{
    return caca_get_rendertime(dp);
}

unsigned int Caca::get_display_width()
{
    return caca_get_display_width(dp);
}

unsigned int Caca::get_display_height()
{
    return caca_get_display_height(dp);
}

int Caca::set_display_title(char const *s)
{
    return caca_set_display_title(dp, s);
}

int Caca::get_event(unsigned int g, Event *n, int aa)
{
    return caca_get_event(dp, g, n->e, aa);
}

unsigned int Caca::get_mouse_x()
{
    return caca_get_mouse_x(dp);
}

unsigned int Caca::get_mouse_y()
{
    return caca_get_mouse_x(dp);
}

void Caca::set_mouse(int v)
{
     caca_set_mouse(dp, v);
}

