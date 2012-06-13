
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/caca -I../caca

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
	for x in $$library_names; do \
	  rm -f $(DESTDIR)$(libdir)/$$(echo $$x | sed 's/caca/cucul/g'); \
	done
	rm -f $(DESTDIR)$(libdir)/libcucul++.la
	rmdir $(DESTDIR)$(libdir) 2>/dev/null || true
	rm -f $(DESTDIR)$(pkgconfigdir)/cucul++.pc
	rmdir $(DESTDIR)$(pkgconfigdir) 2>/dev/null || true

echo-sources: ; echo $(SOURCES)

