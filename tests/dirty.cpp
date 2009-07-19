/*
 *  caca-test     testsuite program for libcaca
 *  Copyright (c) 2009 Sam Hocevar <sam@hocevar.net>
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
    CPPUNIT_TEST(test_put_char_dirty);
    CPPUNIT_TEST(test_put_char_not_dirty);
    CPPUNIT_TEST(test_box);
    CPPUNIT_TEST(test_blit);
    CPPUNIT_TEST_SUITE_END();

public:
    DirtyTest() : CppUnit::TestCase("Dirty Rectangles Test") {}

    void setUp() {}

    void tearDown() {}

    void test_create()
    {
        caca_canvas_t *cv;
        int i, dx, dy, dw, dh;

        /* Check that the dirty rectangle contains the whole canvas
         * upon creation. */
        cv = caca_create_canvas(WIDTH, HEIGHT);
        i = caca_get_dirty_rect_count(cv);
        CPPUNIT_ASSERT_EQUAL(i, 1);

        caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);
        CPPUNIT_ASSERT_EQUAL(dx, 0);
        CPPUNIT_ASSERT_EQUAL(dy, 0);
        CPPUNIT_ASSERT_EQUAL(dw, caca_get_canvas_width(cv));
        CPPUNIT_ASSERT_EQUAL(dh, caca_get_canvas_height(cv));

        /* Invalidate the dirty rectangle and check that it stays so. */
        caca_clear_dirty_rect_list(cv);
        i = caca_get_dirty_rect_count(cv);
        CPPUNIT_ASSERT_EQUAL(i, 0);

        caca_free_canvas(cv);
    }

    void test_put_char_dirty()
    {
        caca_canvas_t *cv;
        int i, dx, dy, dw, dh;

        cv = caca_create_canvas(WIDTH, HEIGHT);

        caca_clear_dirty_rect_list(cv);
        caca_put_char(cv, 7, 3, 'x');
        i = caca_get_dirty_rect_count(cv);
        CPPUNIT_ASSERT_EQUAL(i, 1);
        caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);
        CPPUNIT_ASSERT_EQUAL(dx, 7);
        CPPUNIT_ASSERT_EQUAL(dy, 3);
        CPPUNIT_ASSERT_EQUAL(dw, 1);
        CPPUNIT_ASSERT_EQUAL(dh, 1);

        caca_clear_canvas(cv);
        caca_clear_dirty_rect_list(cv);
        caca_put_char(cv, 7, 3, 0x2f06 /* ⼆ */);
        i = caca_get_dirty_rect_count(cv);
        CPPUNIT_ASSERT_EQUAL(i, 1);
        caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);
        CPPUNIT_ASSERT_EQUAL(dx, 7);
        CPPUNIT_ASSERT_EQUAL(dy, 3);
        CPPUNIT_ASSERT_EQUAL(dw, 2);
        CPPUNIT_ASSERT_EQUAL(dh, 1);

        caca_clear_canvas(cv);
        caca_put_char(cv, 7, 3, 0x2f06 /* ⼆ */);
        caca_clear_dirty_rect_list(cv);
        caca_put_char(cv, 7, 3, 'x');
        i = caca_get_dirty_rect_count(cv);
        CPPUNIT_ASSERT_EQUAL(i, 1);
        caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);
        CPPUNIT_ASSERT_EQUAL(dx, 7);
        CPPUNIT_ASSERT_EQUAL(dy, 3);
        CPPUNIT_ASSERT_EQUAL(dw, 2);
        CPPUNIT_ASSERT_EQUAL(dh, 1);

        caca_clear_canvas(cv);
        caca_put_char(cv, 7, 3, 0x2f06 /* ⼆ */);
        caca_clear_dirty_rect_list(cv);
        caca_put_char(cv, 8, 3, 'x');
        i = caca_get_dirty_rect_count(cv);
        CPPUNIT_ASSERT_EQUAL(i, 1);
        caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);
        CPPUNIT_ASSERT_EQUAL(dx, 7);
        CPPUNIT_ASSERT_EQUAL(dy, 3);
        CPPUNIT_ASSERT_EQUAL(dw, 2);
        CPPUNIT_ASSERT_EQUAL(dh, 1);

        caca_clear_canvas(cv);
        caca_put_char(cv, 7, 3, 0x2f06 /* ⼆ */);
        caca_clear_dirty_rect_list(cv);
        caca_put_char(cv, 6, 3, 0x2f06 /* ⼆ */);
        i = caca_get_dirty_rect_count(cv);
        CPPUNIT_ASSERT_EQUAL(i, 1);
        caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);
        CPPUNIT_ASSERT_EQUAL(dx, 6);
        CPPUNIT_ASSERT_EQUAL(dy, 3);
        CPPUNIT_ASSERT_EQUAL(dw, 3);
        CPPUNIT_ASSERT_EQUAL(dh, 1);

        caca_clear_canvas(cv);
        caca_put_char(cv, 7, 3, 0x2f06 /* ⼆ */);
        caca_clear_dirty_rect_list(cv);
        caca_put_char(cv, 8, 3, 0x2f06 /* ⼆ */);
        i = caca_get_dirty_rect_count(cv);
        CPPUNIT_ASSERT_EQUAL(i, 1);
        caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);
        CPPUNIT_ASSERT_EQUAL(dx, 7);
        CPPUNIT_ASSERT_EQUAL(dy, 3);
        CPPUNIT_ASSERT_EQUAL(dw, 3);
        CPPUNIT_ASSERT_EQUAL(dh, 1);
    }

    void test_put_char_not_dirty()
    {
        caca_canvas_t *cv;
        int i;

        cv = caca_create_canvas(WIDTH, HEIGHT);

        /* Check that pasting the same character does not cause
         * a dirty rectangle to be created. */
        caca_put_char(cv, 7, 3, 'x');
        caca_clear_dirty_rect_list(cv);
        caca_put_char(cv, 7, 3, 'x');
        i = caca_get_dirty_rect_count(cv);
        CPPUNIT_ASSERT_EQUAL(i, 0);

        caca_clear_canvas(cv);
        caca_put_char(cv, 7, 3, 0x2f06 /* ⼆ */);
        caca_clear_dirty_rect_list(cv);
        caca_put_char(cv, 7, 3, 0x2f06 /* ⼆ */);
        i = caca_get_dirty_rect_count(cv);
        CPPUNIT_ASSERT_EQUAL(i, 0);
    }

    void test_box()
    {
        caca_canvas_t *cv;
        int i, dx, dy, dw, dh;

        cv = caca_create_canvas(WIDTH, HEIGHT);
        caca_clear_dirty_rect_list(cv);

        caca_fill_box(cv, 7, 3, 14, 9, 'x');
        i = caca_get_dirty_rect_count(cv);
        CPPUNIT_ASSERT_EQUAL(i, 1);
        caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);
        CPPUNIT_ASSERT_EQUAL(dx, 7);
        CPPUNIT_ASSERT_EQUAL(dy, 3);
        CPPUNIT_ASSERT_EQUAL(dw, 14);
        CPPUNIT_ASSERT_EQUAL(dh, 9);

        caca_clear_dirty_rect_list(cv);
        caca_fill_box(cv, 7, 3, 14, 9, 'x');
        i = caca_get_dirty_rect_count(cv);
        CPPUNIT_ASSERT_EQUAL(i, 0);
    }

    void test_blit()
    {
        caca_canvas_t *cv, *cv2;
        int i, dx, dy, dw, dh;

        cv = caca_create_canvas(WIDTH, HEIGHT);
        caca_clear_dirty_rect_list(cv);
        cv2 = caca_create_canvas(2, 2);
        caca_fill_box(cv2, 0, 0, 2, 1, 'x');

        caca_blit(cv, 1, 1, cv2, NULL);
        i = caca_get_dirty_rect_count(cv);
        CPPUNIT_ASSERT_EQUAL(i, 1);
        caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);

        /* Check that blitting a canvas make a dirty rectangle
         * only for modified lines */
        CPPUNIT_ASSERT(dx == 1);
        CPPUNIT_ASSERT(dy == 1);
        CPPUNIT_ASSERT(dw >= 2);
        CPPUNIT_ASSERT(dh == 1);
    }

private:
    static int const WIDTH = 80, HEIGHT = 50;
};

CPPUNIT_TEST_SUITE_REGISTRATION(DirtyTest);

