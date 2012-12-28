/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2006-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains debugging functions.
 */

#ifndef __CACA_DEBUG_H__
#define __CACA_DEBUG_H__

/* debug messages */
#if defined DEBUG && !defined __KERNEL__
#   include <stdio.h>
#   include <stdarg.h>
static inline void debug(const char *format, ...)
{
    int saved_errno = geterrno();
    va_list args;
    va_start(args, format);
    fprintf(stderr, "** libcaca debug ** ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
    seterrno(saved_errno);
}
#else
#   define debug(format, ...) do {} while(0)
#endif

#endif /* __CACA_DEBUG_H__ */

