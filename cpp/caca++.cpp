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
Caca::Caca(Cucul *qq) 
{
    kk = caca_attach(qq->get_cucul_t());
    if(!kk) throw -1;
}
Caca::~Caca() 
{
    caca_detach(kk);
}
void Caca::attach(Cucul *qq)
{
    kk = caca_attach(qq->get_cucul_t());
    if(!kk) throw -1;
}
void 	Caca::detach ()
{
    caca_detach(kk);
}
void 	Caca::set_delay (unsigned int d)
{
    caca_set_delay(kk, d);
}
void 	Caca::display ()
{
    caca_display(kk);
}
unsigned int 	Caca::get_rendertime ()
{
    return caca_get_rendertime(kk);
}
unsigned int 	Caca::get_window_width ()
{
    return caca_get_window_width(kk);
}
unsigned int 	Caca::get_window_height ()
{
    return caca_get_window_height(kk);
}
int 	Caca::set_window_title (char const *s)
{
    return caca_set_window_title(kk, s);
}
int 	Caca::get_event (unsigned int g, Caca::Event *n, int aa)
{
    return caca_get_event(kk, g, n->e, aa);
}
unsigned int 	Caca::get_mouse_x ()
{
    return caca_get_mouse_x(kk);
}
unsigned int 	Caca::get_mouse_y ()
{
    return caca_get_mouse_x(kk);
}
void 	Caca::set_mouse (int v)
{
     caca_set_mouse(kk, v);
}
