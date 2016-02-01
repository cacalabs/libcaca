
include $(top_srcdir)/build/autotools/common.am

if USE_CPPUNIT
cppunit_tests = caca-test
endif
if BUILD_DOCUMENTATION
doxygen_tests = check-doxygen
endif

EXTRA_DIST = check-copyright check-doxygen check-source check-win32

noinst_PROGRAMS = simple bench bug-setlocale $(cppunit_tests)

TESTS = simple check-copyright check-source check-win32 \
        $(doxygen_tests) $(cppunit_tests)

simple_SOURCES = simple.c
simple_LDADD = ../libcaca.la

bench_SOURCES = bench.c
bench_LDADD = ../libcaca.la

bug_setlocale_SOURCES = bug-setlocale.c
bug_setlocale_LDADD = ../libcaca.la

caca_test_SOURCES = caca-test.cpp canvas.cpp dirty.cpp driver.cpp export.cpp
caca_test_CXXFLAGS = $(CPPUNIT_CFLAGS)
caca_test_LDADD = ../libcaca.la $(CPPUNIT_LIBS)

