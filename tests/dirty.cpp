/*
 *  caca-test     testsuite program for libcaca
 *  Copyright (c) 2009 Sam Hocevar <sam@zoy.org>
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

#include "caca.h"

class DirtyTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(DirtyTest);
    CPPUNIT_TEST(test_create);
    CPPUNIT_TEST(test_put_char);
    CPPUNIT_TEST_SUITE_END();

public:
    DirtyTest() : CppUnit::TestCase("Dirty Rectangles Test") {}

    void setUp() {}

    void tearDown() {}

    void test_create()
    {
        caca_canvas_t *cv;
        int xmin, xmax, ymin, ymax;

        /* Check that the dirty rectangle contains the whole canvas
         * upon creation. */
        cv = caca_create_canvas(WIDTH, HEIGHT);
        caca_get_dirty_rectangle(cv, &xmin, &ymin, &xmax, &ymax);
        CPPUNIT_ASSERT(xmin <= 0);
        CPPUNIT_ASSERT(ymin <= 0);
        CPPUNIT_ASSERT(xmax >= caca_get_canvas_width(cv) - 1);
        CPPUNIT_ASSERT(ymax >= caca_get_canvas_height(cv) - 1);

        /* Invalidate the dirty rectangle and check that it stays so. */
        caca_set_dirty_rectangle(cv, -1, -1, -1, -1);
        caca_get_dirty_rectangle(cv, &xmin, &ymin, &xmax, &ymax);
        CPPUNIT_ASSERT(xmin > xmax || ymin > ymax);

        caca_free_canvas(cv);
    }

    void test_put_char()
    {
        caca_canvas_t *cv;
        int xmin, xmax, ymin, ymax;

        cv = caca_create_canvas(WIDTH, HEIGHT);

        caca_set_dirty_rectangle(cv, -1, -1, -1, -1);
        caca_put_char(cv, 7, 3, 'x');
        caca_get_dirty_rectangle(cv, &xmin, &ymin, &xmax, &ymax);
        CPPUNIT_ASSERT(xmin <= 7);
        CPPUNIT_ASSERT(ymin <= 3);
        CPPUNIT_ASSERT(xmax >= 7);
        CPPUNIT_ASSERT(ymax >= 3);

        caca_clear_canvas(cv);
        caca_set_dirty_rectangle(cv, -1, -1, -1, -1);
        caca_put_char(cv, 7, 3, 0x2f06 /* ⼆ */);
        caca_get_dirty_rectangle(cv, &xmin, &ymin, &xmax, &ymax);
        CPPUNIT_ASSERT(xmin <= 7);
        CPPUNIT_ASSERT(ymin <= 3);
        CPPUNIT_ASSERT(xmax >= 8);
        CPPUNIT_ASSERT(ymax >= 3);

        caca_clear_canvas(cv);
        caca_put_char(cv, 7, 3, 0x2f06 /* ⼆ */);
        caca_set_dirty_rectangle(cv, -1, -1, -1, -1);
        caca_put_char(cv, 7, 3, 'x');
        caca_get_dirty_rectangle(cv, &xmin, &ymin, &xmax, &ymax);
        CPPUNIT_ASSERT(xmin <= 7);
        CPPUNIT_ASSERT(ymin <= 3);
        CPPUNIT_ASSERT(xmax >= 8);
        CPPUNIT_ASSERT(ymax >= 3);

        caca_clear_canvas(cv);
        caca_put_char(cv, 7, 3, 0x2f06 /* ⼆ */);
        caca_set_dirty_rectangle(cv, -1, -1, -1, -1);
        caca_put_char(cv, 8, 3, 'x');
        caca_get_dirty_rectangle(cv, &xmin, &ymin, &xmax, &ymax);
        CPPUNIT_ASSERT(xmin <= 7);
        CPPUNIT_ASSERT(ymin <= 3);
        CPPUNIT_ASSERT(xmax >= 8);
        CPPUNIT_ASSERT(ymax >= 3);

        caca_clear_canvas(cv);
        caca_put_char(cv, 7, 3, 0x2f06 /* ⼆ */);
        caca_set_dirty_rectangle(cv, -1, -1, -1, -1);
        caca_put_char(cv, 6, 3, 0x2f06 /* ⼆ */);
        caca_get_dirty_rectangle(cv, &xmin, &ymin, &xmax, &ymax);
        CPPUNIT_ASSERT(xmin <= 6);
        CPPUNIT_ASSERT(ymin <= 3);
        CPPUNIT_ASSERT(xmax >= 8);
        CPPUNIT_ASSERT(ymax >= 3);

        caca_clear_canvas(cv);
        caca_put_char(cv, 7, 3, 0x2f06 /* ⼆ */);
        caca_set_dirty_rectangle(cv, -1, -1, -1, -1);
        caca_put_char(cv, 8, 3, 0x2f06 /* ⼆ */);
        caca_get_dirty_rectangle(cv, &xmin, &ymin, &xmax, &ymax);
        CPPUNIT_ASSERT(xmin <= 7);
        CPPUNIT_ASSERT(ymin <= 3);
        CPPUNIT_ASSERT(xmax >= 9);
        CPPUNIT_ASSERT(ymax >= 3);
    }

private:
    static int const WIDTH = 80, HEIGHT = 50;
};

CPPUNIT_TEST_SUITE_REGISTRATION(DirtyTest);

