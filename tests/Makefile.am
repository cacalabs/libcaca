# $Id$

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/cucul -I../cucul

if USE_CPPUNIT
cppunit_tests = cucul-test
endif

noinst_PROGRAMS = simple $(cppunit_tests)

TESTS = $(noinst_PROGRAMS)

simple_SOURCES = simple.c
simple_LDADD = ../cucul/libcucul.la

cucul_test_SOURCES = cucul-test.cpp canvas.cpp
cucul_test_CPPFLAGS = $(CPPUNIT_CFLAGS)
cucul_test_LDADD = ../cucul/libcucul.la $(CPPUNIT_LIBS)

