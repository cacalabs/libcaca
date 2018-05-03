
include $(top_srcdir)/build/autotools/common.am

EXTRA_DIST = libcaca++.vcxproj libcaca++.def test-cxx.vcxproj
if USE_CXX
pkgconfig_DATA = caca++.pc
pkgconfigdir = $(libdir)/pkgconfig

include_HEADERS = caca++.h
lib_LTLIBRARIES = libcaca++.la
endif

libcaca___la_SOURCES = caca++.cpp caca++.h
libcaca___la_LDFLAGS = -no-undefined -version-number @LT_VERSION@
libcaca___la_LIBADD = ../caca/libcaca.la

if USE_CXX
noinst_PROGRAMS = cxxtest
endif

cxxtest_SOURCES = cxxtest.cpp
cxxtest_LDADD = libcaca++.la ../caca/libcaca.la

uninstall-local:
	. ./libcaca++.la || exit 1; \
	rmdir $(DESTDIR)$(libdir) 2>/dev/null || true
	rmdir $(DESTDIR)$(pkgconfigdir) 2>/dev/null || true

