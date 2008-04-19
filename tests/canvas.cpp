/*
 *  cucul-test    testsuite program for libcucul
 *  Copyright (c) 2008 Sam Hocevar <sam@zoy.org>
 *  Copyright (c) 2008 Sam Hocevar <sam@zoy.org>
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

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#include "cucul.h"

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
        cucul_canvas_t *cv;

        cv = cucul_create_canvas(0, 0);
        CPPUNIT_ASSERT(cv != NULL);

        cucul_free_canvas(cv);
    }

    void test_resize()
    {
        cucul_canvas_t *cv;

        cv = cucul_create_canvas(0, 0);
        CPPUNIT_ASSERT_EQUAL(cucul_get_canvas_width(cv), 0U);
        CPPUNIT_ASSERT_EQUAL(cucul_get_canvas_height(cv), 0U);

        cucul_set_canvas_size(cv, 1, 1);
        CPPUNIT_ASSERT_EQUAL(cucul_get_canvas_width(cv), 1U);
        CPPUNIT_ASSERT_EQUAL(cucul_get_canvas_height(cv), 1U);

        cucul_set_canvas_size(cv, 1234, 1001);
        CPPUNIT_ASSERT_EQUAL(cucul_get_canvas_width(cv), 1234U);
        CPPUNIT_ASSERT_EQUAL(cucul_get_canvas_height(cv), 1001U);

        cucul_set_canvas_size(cv, 0, 0);
        CPPUNIT_ASSERT_EQUAL(cucul_get_canvas_width(cv), 0U);
        CPPUNIT_ASSERT_EQUAL(cucul_get_canvas_height(cv), 0U);

        cucul_free_canvas(cv);
    }

    void test_chars()
    {
        cucul_canvas_t *cv;

        cv = cucul_create_canvas(0, 0);
        CPPUNIT_ASSERT(cucul_get_char(cv, 0, 0) == ' ');

        cucul_put_char(cv, 0, 0, 'x');
        CPPUNIT_ASSERT(cucul_get_char(cv, 0, 0) == ' ');

        cucul_set_canvas_size(cv, 1, 1);
        CPPUNIT_ASSERT(cucul_get_char(cv, 0, 0) == ' ');

        cucul_put_char(cv, 0, 0, 'x');
        CPPUNIT_ASSERT(cucul_get_char(cv, 0, 0) == 'x');

        cucul_put_char(cv, 0, 0, 'y');
        CPPUNIT_ASSERT(cucul_get_char(cv, 0, 0) == 'y');

        cucul_set_canvas_size(cv, 1000, 1000);
        CPPUNIT_ASSERT(cucul_get_char(cv, 999, 999) == ' ');

        cucul_put_char(cv, 999, 999, 'z');
        CPPUNIT_ASSERT(cucul_get_char(cv, 999, 999) == 'z');

        cucul_free_canvas(cv);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(CanvasTest);

