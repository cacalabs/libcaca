/*
 *  caca-test     testsuite program for libcaca
 *  Copyright (c) 2008 Sam Hocevar <samhocevar.net>
 *                All Rights Reserved
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "config.h"

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#include "caca.h"

class CanvasTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(CanvasTest);
    CPPUNIT_TEST(test_creation);
    CPPUNIT_TEST(test_resize);
    CPPUNIT_TEST(test_chars);
    CPPUNIT_TEST_SUITE_END();

public:
    CanvasTest() : CppUnit::TestCase("Canvas Test") {}

    void setUp() {}

    void tearDown() {}

    void test_creation()
    {
        caca_canvas_t *cv;

        cv = caca_create_canvas(0, 0);
        CPPUNIT_ASSERT(cv != NULL);

        caca_free_canvas(cv);
    }

    void test_resize()
    {
        caca_canvas_t *cv;

        cv = caca_create_canvas(0, 0);
        CPPUNIT_ASSERT_EQUAL(caca_get_canvas_width(cv), 0);
        CPPUNIT_ASSERT_EQUAL(caca_get_canvas_height(cv), 0);

        caca_set_canvas_size(cv, 1, 1);
        CPPUNIT_ASSERT_EQUAL(caca_get_canvas_width(cv), 1);
        CPPUNIT_ASSERT_EQUAL(caca_get_canvas_height(cv), 1);

        caca_set_canvas_size(cv, 1234, 1001);
        CPPUNIT_ASSERT_EQUAL(caca_get_canvas_width(cv), 1234);
        CPPUNIT_ASSERT_EQUAL(caca_get_canvas_height(cv), 1001);

        caca_set_canvas_size(cv, 0, 0);
        CPPUNIT_ASSERT_EQUAL(caca_get_canvas_width(cv), 0);
        CPPUNIT_ASSERT_EQUAL(caca_get_canvas_height(cv), 0);

        caca_free_canvas(cv);
    }

    void test_chars()
    {
        caca_canvas_t *cv;

        cv = caca_create_canvas(0, 0);
        CPPUNIT_ASSERT(caca_get_char(cv, 0, 0) == ' ');

        caca_put_char(cv, 0, 0, 'x');
        CPPUNIT_ASSERT(caca_get_char(cv, 0, 0) == ' ');

        caca_set_canvas_size(cv, 1, 1);
        CPPUNIT_ASSERT(caca_get_char(cv, 0, 0) == ' ');

        caca_put_char(cv, 0, 0, 'x');
        CPPUNIT_ASSERT(caca_get_char(cv, 0, 0) == 'x');

        caca_put_char(cv, 0, 0, 'y');
        CPPUNIT_ASSERT(caca_get_char(cv, 0, 0) == 'y');

        caca_set_canvas_size(cv, 1000, 1000);
        CPPUNIT_ASSERT(caca_get_char(cv, 999, 999) == ' ');

        caca_put_char(cv, 999, 999, 'z');
        CPPUNIT_ASSERT(caca_get_char(cv, 999, 999) == 'z');

        caca_free_canvas(cv);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(CanvasTest);

