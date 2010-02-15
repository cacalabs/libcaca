
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/caca -I../caca

if USE_CPPUNIT
cppunit_tests = caca-test
endif
if BUILD_DOCUMENTATION
doxygen_tests = check-doxygen
endif

EXTRA_DIST = check-copyright check-doxygen check-source check-win32

noinst_PROGRAMS = simple bench $(cppunit_tests)

TESTS = simple check-copyright check-source check-win32 \
        $(doxygen_tests) $(cppunit_tests)

simple_SOURCES = simple.c
simple_LDADD = ../caca/libcaca.la

bench_SOURCES = bench.c
bench_LDADD = ../caca/libcaca.la

caca_test_SOURCES = caca-test.cpp canvas.cpp dirty.cpp driver.cpp export.cpp
caca_test_CXXFLAGS = $(CPPUNIT_CFLAGS)
caca_test_LDADD = ../caca/libcaca.la $(CPPUNIT_LIBS)

