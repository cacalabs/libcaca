# $Id$

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/caca -I../caca

if USE_CPPUNIT
cppunit_tests = caca-test
endif

EXTRA_DIST = check-build

noinst_PROGRAMS = simple bench $(cppunit_tests)

TESTS = simple check-build $(cppunit_tests)

simple_SOURCES = simple.c
simple_LDADD = ../caca/libcaca.la

bench_SOURCES = bench.c
bench_LDADD = ../caca/libcaca.la

caca_test_SOURCES = caca-test.cpp canvas.cpp dirty.cpp driver.cpp export.cpp
caca_test_CXXFLAGS = $(CPPUNIT_CFLAGS)
caca_test_LDADD = ../caca/libcaca.la $(CPPUNIT_LIBS)

