/*
 *  caca-test     testsuite program for libcaca
 *  Copyright (c) 2008 Sam Hocevar <samhocevar.net>
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

class DriverTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(DriverTest);
    CPPUNIT_TEST(test_list);
    CPPUNIT_TEST_SUITE_END();

public:
    DriverTest() : CppUnit::TestCase("Driver Test") {}

    void setUp() {}

    void tearDown() {}

    void test_list()
    {
        char const * const * list;

        list = caca_get_display_driver_list();
        CPPUNIT_ASSERT(list != NULL);
        CPPUNIT_ASSERT(list[0] != NULL);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(DriverTest);

