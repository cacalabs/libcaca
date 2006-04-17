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


#include "cucul.h"
#include "caca.h"

#include "cucul++.h"


class Caca {
 public:
    Caca();
    Caca(Cucul *qq);
    ~Caca();

    class Event {
        friend class Caca;
    protected:
        caca_event *e;
    };


    void    attach (Cucul *qq);
    void 	detach ();
    void 	set_delay (unsigned int);
    void 	display ();
    unsigned int 	get_rendertime ();
    unsigned int 	get_window_width ();
    unsigned int 	get_window_height ();
    int 	set_window_title (char const *);
    int 	get_event (unsigned int, Caca::Event*, int);
    unsigned int 	get_mouse_x ();
    unsigned int 	get_mouse_y ();
    void 	set_mouse (int);


    
    private:
    caca_t *kk;


};


#endif /* _CACA_PP_H */
