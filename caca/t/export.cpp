/*
 *  caca-test     testsuite program for libcaca
 *  Copyright (c) 2009 Pascal Terjan <pterjan@linuxfr.org>
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

class ExportTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(ExportTest);
    CPPUNIT_TEST(test_export_area_caca);
    CPPUNIT_TEST_SUITE_END();

public:
    ExportTest() : CppUnit::TestCase("Export/Import Test") {}

    void setUp() {}

    void tearDown() {}

    void test_export_area_caca()
    {
        caca_canvas_t *cv;
        size_t bytes, l;
        void *buf;

        cv = caca_create_canvas(WIDTH, HEIGHT);
        caca_put_char(cv, 7, 3, 'a');
        caca_put_char(cv, 10, 3, 'b');
        caca_put_char(cv, 5, 5, 'c');
        buf = caca_export_area_to_memory (cv, 0, 0, 10, 5, "caca", &bytes);
        CPPUNIT_ASSERT(buf != NULL);
        CPPUNIT_ASSERT(bytes > 0);

        caca_clear_canvas(cv);
        l = caca_import_area_from_memory(cv, 0, 0, buf, bytes, "caca");
        CPPUNIT_ASSERT(l == bytes);
        CPPUNIT_ASSERT(caca_get_char(cv, 7, 3) == 'a');
        CPPUNIT_ASSERT(caca_get_char(cv, 10, 3) == ' ');
        CPPUNIT_ASSERT(caca_get_char(cv, 5, 5) == ' ');

        caca_put_char(cv, 10, 3, 'b');
        caca_put_char(cv, 5, 5, 'c');
        l = caca_import_area_from_memory(cv, 0, 0, buf, bytes, "caca");
        CPPUNIT_ASSERT(l == bytes);
        CPPUNIT_ASSERT(caca_get_char(cv, 7, 3) == 'a');
        CPPUNIT_ASSERT(caca_get_char(cv, 10, 3) == 'b');
        CPPUNIT_ASSERT(caca_get_char(cv, 5, 5) == 'c');

        caca_free_canvas(cv);
    }

private:
    static int const WIDTH = 80, HEIGHT = 50;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ExportTest);

