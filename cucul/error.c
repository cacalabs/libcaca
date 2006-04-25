/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
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
 *  This file contains error management functions.
 */

#include "config.h"
#include "common.h"

#include "cucul.h"
#include "cucul_internals.h"

int cucul_errno(void)
{
    return _cucul_errno;
}

char const *cucul_strerror(int error)
{
    switch(error)
    {
        case 0:
            return "no error";
        case ECUCUL_NOMEM:
            return "not enough memory";
        case ECUCUL_INVAL:
            return "invalid argument";
        case ECUCUL_RANGE:
            return "argument out of bounds";
        default:
            return "unknown error";
    }
}

