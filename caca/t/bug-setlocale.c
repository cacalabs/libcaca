/*
 *  bug-setlocale: unit test for wrong setlocale() calls
 *  Copyright © 2016 Sam Hocevar <sam@hocevar.net>
 *              All Rights Reserved
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
#   include <string.h>
#endif

#include "caca.h"

#define TEST(x) \
    do \
    { \
        tests++; \
        if((x)) \
            passed++; \
        else \
            fprintf(stderr, "test #%i failed: %s\n", (tests), #x); \
    } \
    while(0)

int main(int argc, char *argv[])
{
    char buf[10];
    char const * const * list;

    list = caca_get_display_driver_list();

    int i, tests = 0, passed = 0;

    snprintf(buf, 10, "%.1f", 0.0f);
    TEST(buf[1] == '.');

    for (i = 0; list[i]; i += 2)
    {
        if (!strcmp(list[i], "x11") || !strcmp(list[i], "ncurses"))
        {
            caca_display_t *dp = caca_create_display_with_driver(NULL, list[i]);

            snprintf(buf, 10, "%.1f", 0.0f);
            TEST(buf[1] == '.');

            caca_free_display(dp);
        }
    }

    fprintf(stderr, "%i tests, %i errors\n", tests, tests - passed);

    return 0;
}

