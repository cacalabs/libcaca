/*
 *  caca-test     testsuite program for libcaca
 *  Copyright (c) 2009 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
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
    CPPUNIT_TEST(test_simplify);
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
        int dx, dy, dw, dh;

        /* Check that we only have one dirty rectangle upon creation. */
        cv = caca_create_canvas(WIDTH, HEIGHT);
        CPPUNIT_ASSERT_EQUAL(1, caca_get_dirty_rect_count(cv));

        /* Check that our only rectangle contains the whole canvas. */
        caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);
        CPPUNIT_ASSERT_EQUAL(0, dx);
        CPPUNIT_ASSERT_EQUAL(0, dy);
        CPPUNIT_ASSERT_EQUAL(WIDTH, dw);
        CPPUNIT_ASSERT_EQUAL(HEIGHT, dh);

        /* Invalidate the dirty rectangle and check that it stays so. */
        caca_clear_dirty_rect_list(cv);
        CPPUNIT_ASSERT_EQUAL(0, caca_get_dirty_rect_count(cv));

        caca_free_canvas(cv);
    }

    void test_put_char_dirty()
    {
        caca_canvas_t *cv;
        int dx, dy, dw, dh;

        cv = caca_create_canvas(WIDTH, HEIGHT);

        /* Check that one character creates a 1x1 dirty rect. */
        caca_clear_dirty_rect_list(cv);
        caca_put_char(cv, 7, 3, 'x');

        CPPUNIT_ASSERT_EQUAL(1, caca_get_dirty_rect_count(cv));
        caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);
        CPPUNIT_ASSERT_EQUAL(7, dx);
        CPPUNIT_ASSERT_EQUAL(3, dy);
        CPPUNIT_ASSERT_EQUAL(1, dw);
        CPPUNIT_ASSERT_EQUAL(1, dh);

        /* Check that a fullwidth character creates a 2x1 dirty rect. */
        caca_clear_canvas(cv);
        caca_clear_dirty_rect_list(cv);
        caca_put_char(cv, 7, 3, 0x2f06 /* ⼆ */);

        CPPUNIT_ASSERT_EQUAL(1, caca_get_dirty_rect_count(cv));
        caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);
        CPPUNIT_ASSERT_EQUAL(7, dx);
        CPPUNIT_ASSERT_EQUAL(3, dy);
        CPPUNIT_ASSERT_EQUAL(2, dw);
        CPPUNIT_ASSERT_EQUAL(1, dh);

        /* Check that a character over a fullwidth character creates a
         * 2x1 dirty rect because of clobbering on the left side. */
        caca_clear_canvas(cv);
        caca_put_char(cv, 7, 3, 0x2f06 /* ⼆ */);
        caca_clear_dirty_rect_list(cv);
        caca_put_char(cv, 7, 3, 'x');

        CPPUNIT_ASSERT_EQUAL(1, caca_get_dirty_rect_count(cv));
        caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);
        CPPUNIT_ASSERT_EQUAL(7, dx);
        CPPUNIT_ASSERT_EQUAL(3, dy);
        CPPUNIT_ASSERT_EQUAL(2, dw);
        CPPUNIT_ASSERT_EQUAL(1, dh);

        /* Check that a character over a fullwidth character creates a
         * 2x1 dirty rect because of clobbering on the right side. */
        caca_clear_canvas(cv);
        caca_put_char(cv, 7, 3, 0x2f06 /* ⼆ */);
        caca_clear_dirty_rect_list(cv);
        caca_put_char(cv, 8, 3, 'x');

        CPPUNIT_ASSERT_EQUAL(1, caca_get_dirty_rect_count(cv));
        caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);
        CPPUNIT_ASSERT_EQUAL(7, dx);
        CPPUNIT_ASSERT_EQUAL(3, dy);
        CPPUNIT_ASSERT_EQUAL(2, dw);
        CPPUNIT_ASSERT_EQUAL(1, dh);

        /* Check that a fullwidth character over a fullwidth character creates
         * a 3x1 dirty rect because of clobbering on the left side. */
        caca_clear_canvas(cv);
        caca_put_char(cv, 7, 3, 0x2f06 /* ⼆ */);
        caca_clear_dirty_rect_list(cv);
        caca_put_char(cv, 6, 3, 0x2f06 /* ⼆ */);

        CPPUNIT_ASSERT_EQUAL(1, caca_get_dirty_rect_count(cv));
        caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);
        CPPUNIT_ASSERT_EQUAL(6, dx);
        CPPUNIT_ASSERT_EQUAL(3, dy);
        CPPUNIT_ASSERT_EQUAL(3, dw);
        CPPUNIT_ASSERT_EQUAL(1, dh);

        /* Check that a fullwidth character over a fullwidth character creates
         * a 3x1 dirty rect because of clobbering on the right side. */
        caca_clear_canvas(cv);
        caca_put_char(cv, 7, 3, 0x2f06 /* ⼆ */);
        caca_clear_dirty_rect_list(cv);
        caca_put_char(cv, 8, 3, 0x2f06 /* ⼆ */);

        CPPUNIT_ASSERT_EQUAL(1, caca_get_dirty_rect_count(cv));
        caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);
        CPPUNIT_ASSERT_EQUAL(7, dx);
        CPPUNIT_ASSERT_EQUAL(3, dy);
        CPPUNIT_ASSERT_EQUAL(3, dw);
        CPPUNIT_ASSERT_EQUAL(1, dh);
    }

    void test_put_char_not_dirty()
    {
        caca_canvas_t *cv;

        cv = caca_create_canvas(WIDTH, HEIGHT);

        /* Check that pasting the same character does not cause a dirty
         * rectangle to be created. */
        caca_put_char(cv, 7, 3, 'x');
        caca_clear_dirty_rect_list(cv);
        caca_put_char(cv, 7, 3, 'x');

        CPPUNIT_ASSERT_EQUAL(0, caca_get_dirty_rect_count(cv));

        /* Check that pasting the same fullwidth character does not cause
         * a dirty rectangle to be created. */
        caca_clear_canvas(cv);
        caca_put_char(cv, 7, 3, 0x2f06 /* ⼆ */);
        caca_clear_dirty_rect_list(cv);
        caca_put_char(cv, 7, 3, 0x2f06 /* ⼆ */);

        CPPUNIT_ASSERT_EQUAL(0, caca_get_dirty_rect_count(cv));
    }

    void test_simplify()
    {
        caca_canvas_t *cv;
        int dx, dy, dw, dh;

        cv = caca_create_canvas(WIDTH, HEIGHT);

        /* Check that N adjacent blits only create one dirty rectangle */
        caca_clear_dirty_rect_list(cv);
        for(int i = 0; i < 10; i++)
        {
            caca_put_char(cv, 7 + i, 3, '-');

            CPPUNIT_ASSERT_EQUAL(1, caca_get_dirty_rect_count(cv));
            caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);
            CPPUNIT_ASSERT_EQUAL(7, dx);
            CPPUNIT_ASSERT_EQUAL(3, dy);
            CPPUNIT_ASSERT_EQUAL(1 + i, dw);
            CPPUNIT_ASSERT_EQUAL(1, dh);
        }

        /* Check that N adjacent blits only create one dirty rectangle */
        caca_clear_dirty_rect_list(cv);
        for(int j = 0; j < 10; j++)
        {
            caca_put_char(cv, 7, 3 + j, '|');

            CPPUNIT_ASSERT_EQUAL(1, caca_get_dirty_rect_count(cv));
            caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);
            CPPUNIT_ASSERT_EQUAL(7, dx);
            CPPUNIT_ASSERT_EQUAL(3, dy);
            CPPUNIT_ASSERT_EQUAL(1, dw);
            CPPUNIT_ASSERT_EQUAL(1 + j, dh);
        }
    }

    void test_box()
    {
        caca_canvas_t *cv;
        int dx, dy, dw, dh;

        cv = caca_create_canvas(WIDTH, HEIGHT);
        caca_clear_dirty_rect_list(cv);

        /* Check that a filled box creates one dirty rectangle of the same
         * size. */
        caca_fill_box(cv, 7, 3, 14, 9, 'x');

        CPPUNIT_ASSERT_EQUAL(1, caca_get_dirty_rect_count(cv));
        caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);
        CPPUNIT_ASSERT_EQUAL(7, dx);
        CPPUNIT_ASSERT_EQUAL(3, dy);
        CPPUNIT_ASSERT_EQUAL(14, dw);
        CPPUNIT_ASSERT_EQUAL(9, dh);

        /* Check that the same filled box does not create a new dirty
         * rectangle. */
        caca_clear_dirty_rect_list(cv);
        caca_fill_box(cv, 7, 3, 14, 9, 'x');

        CPPUNIT_ASSERT_EQUAL(0, caca_get_dirty_rect_count(cv));
    }

    void test_blit()
    {
        caca_canvas_t *cv, *cv2;
        int i, dx, dy, dw, dh;

        cv = caca_create_canvas(WIDTH, HEIGHT);
        caca_clear_dirty_rect_list(cv);
        cv2 = caca_create_canvas(2, 2);
        caca_fill_box(cv2, 0, 0, 2, 1, 'x');

        /* Check that blitting a canvas make a dirty rectangle
         * only for modified lines */
        /* FIXME: check this test's validity */

        caca_blit(cv, 1, 1, cv2, NULL);
        i = caca_get_dirty_rect_count(cv);
        CPPUNIT_ASSERT_EQUAL(1, i);
        caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);

        CPPUNIT_ASSERT(1 == dx);
        CPPUNIT_ASSERT(1 == dy);
        CPPUNIT_ASSERT(2 <= dw);
        CPPUNIT_ASSERT(1 == dh);

        caca_clear_canvas(cv);
        caca_clear_dirty_rect_list(cv);

        /* Check that blitting a canvas make a dirty rectangle
         * only for modified chars when we have a mask */
        /* FIXME: check this test's validity */

        caca_blit(cv, 1, 1, cv2, cv2);
        i = caca_get_dirty_rect_count(cv);
        CPPUNIT_ASSERT_EQUAL(1, i);
        caca_get_dirty_rect(cv, 0, &dx, &dy, &dw, &dh);

        CPPUNIT_ASSERT(1 == dx);
        CPPUNIT_ASSERT(1 == dy);
        CPPUNIT_ASSERT(2 == dw);
        CPPUNIT_ASSERT(1 == dh);

        CPPUNIT_ASSERT(' ' == caca_get_char(cv, 0, 0));

    }

private:
    static int const WIDTH, HEIGHT;
};

int const DirtyTest::WIDTH = 80;
int const DirtyTest::HEIGHT = 50;

CPPUNIT_TEST_SUITE_REGISTRATION(DirtyTest);

