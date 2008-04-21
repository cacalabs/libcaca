# $Id$

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/cucul -I../cucul \
              -I$(top_srcdir)/caca

if USE_CPPUNIT
cppunit_tests = cucul-test caca-test
endif

noinst_PROGRAMS = simple $(cppunit_tests)

TESTS = $(noinst_PROGRAMS)

simple_SOURCES = simple.c
simple_LDADD = ../cucul/libcucul.la

cucul_test_SOURCES = cucul-test.cpp canvas.cpp
cucul_test_CXXFLAGS = $(CPPUNIT_CFLAGS)
cucul_test_LDADD = ../cucul/libcucul.la $(CPPUNIT_LIBS)

caca_test_SOURCES = caca-test.cpp driver.cpp
caca_test_CXXFLAGS = $(CPPUNIT_CFLAGS)
caca_test_LDADD = ../caca/libcaca.la $(CPPUNIT_LIBS)

