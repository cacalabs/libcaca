/*
 *  simple        simple testsuite program
 *  Copyright (c) 2007 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <stdlib.h>
#endif

#include "caca.h"

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
    caca_canvas_t *cv;
    int tests = 0, passed = 0;

    cv = caca_create_canvas(0, 0);
    caca_put_char(cv, 0, 0, 'x');
    TEST(caca_get_char(cv, 0, 0) != 'x');

    caca_rotate_180(cv);

    caca_set_canvas_size(cv, 1, 1);
    TEST(caca_get_char(cv, 0, 0) != 'x');
    TEST(caca_get_char(cv, 0, 0) == ' ');

    caca_put_char(cv, 0, 0, 'y');
    TEST(caca_get_char(cv, 0, 0) == 'y');

    caca_set_canvas_size(cv, 1000, 1000);
    TEST(caca_get_canvas_width(cv) == 1000);

    caca_put_char(cv, 999, 999, 'z');
    TEST(caca_get_char(cv, 999, 999) == 'z');

    caca_free_canvas(cv);

    fprintf(stderr, "%i tests, %i errors\n", tests, tests - passed);

    return 0;
}

