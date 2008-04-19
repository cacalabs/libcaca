/*
 *  simple        simple testsuite program
 *  Copyright (c) 2007 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <stdlib.h>
#endif

#include "cucul.h"

#define TEST(x) \
    do \
    { \
        tests++; \
        if((x)) \
            passed++; \
        else \
            fprintf(stderr, "test #%i failed\n", (tests)); \
    } \
    while(0)

int main(int argc, char *argv[])
{
    cucul_canvas_t *cv;
    int tests = 0, passed = 0;

    cv = cucul_create_canvas(0, 0);
    cucul_put_char(cv, 0, 0, 'x');
    TEST(cucul_get_char(cv, 0, 0) != 'x');

    cucul_set_canvas_size(cv, 1, 1);
    TEST(cucul_get_char(cv, 0, 0) != 'x');
    TEST(cucul_get_char(cv, 0, 0) == ' ');

    cucul_put_char(cv, 0, 0, 'y');
    TEST(cucul_get_char(cv, 0, 0) == 'y');

    cucul_set_canvas_size(cv, 1000, 1000);
    TEST(cucul_get_canvas_width(cv) == 1000);

    cucul_put_char(cv, 999, 999, 'z');
    TEST(cucul_get_char(cv, 999, 999) == 'z');

    cucul_free_canvas(cv);

    fprintf(stderr, "%i tests, %i errors\n", tests, tests - passed);

    return 0;
}

