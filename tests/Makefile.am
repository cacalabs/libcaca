# $Id$

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/caca -I../caca

if USE_CPPUNIT
cppunit_tests = caca-test
endif

noinst_PROGRAMS = simple $(cppunit_tests)

TESTS = $(noinst_PROGRAMS)

simple_SOURCES = simple.c
simple_LDADD = ../caca/libcaca.la

caca_test_SOURCES = caca-test.cpp canvas.cpp dirty.cpp driver.cpp
caca_test_CXXFLAGS = $(CPPUNIT_CFLAGS)
caca_test_LDADD = ../caca/libcaca.la $(CPPUNIT_LIBS)

