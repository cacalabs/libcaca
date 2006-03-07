/*
 *  libcucul      Unicode canvas library
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file math.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Math
 *
 *  This file contains simple mathematical routines.
 */

#include "config.h"

#include <stdlib.h>

#include "cucul.h"
#include "cucul_internals.h"

/**
 * \brief Generate a random integer within a range.
 *
 * \param min The lower bound of the integer range.
 * \param max The upper bound of the integer range.
 * \return A random integer comprised between \p min and \p max, inclusive.
 */
int cucul_rand(int min, int max)
{
    return min + (int)((1.0*(max-min+1)) * rand() / (RAND_MAX+1.0));
}

/**
 * \brief Approximate a square root, using Newton's method to avoid
 *        costly floating point calculations.
 *
 * \param a A positive integer.
 * \return The approximate square root of \p a.
 */
unsigned int cucul_sqrt(unsigned int a)
{
    if(a == 0)
        return 0;

    if(a < 1000000000)
    {
        unsigned int x = a < 10 ? 1
                       : a < 1000 ? 10
                       : a < 100000 ? 100
                       : a < 10000000 ? 1000
                       : 10000;

        /* Newton's method. Three iterations would be more than enough. */
        x = (x * x + a) / x / 2;
        x = (x * x + a) / x / 2;
        x = (x * x + a) / x / 2;
        x = (x * x + a) / x / 2;

        return x;
    }

    return 2 * cucul_sqrt(a / 4);
}


/**
 * \brief powf substitute (x^y)
 * \param x The value to be raised
 * \param y The power to raise x of.
 * \return \p x raised to the power of \p y
 */

float cucul_powf(float x, float y)
{
    int i=((int)y);
    float r=x;

    if(((int)y)==1 || ((int)x)==1)
        return x;

    i--;
    while(i--)
    {
        r*=x;
    }
    return r;
}
