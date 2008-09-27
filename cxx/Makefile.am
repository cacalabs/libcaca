# $Id: Makefile.am 552 2006-04-13 16:10:16Z jylam $

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/caca -I../caca

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

install-exec-hook:
	ln -sf libcaca++.so $(DESTDIR)$(libdir)/libcucul++.so
	ln -sf libcaca++.so.0 $(DESTDIR)$(libdir)/libcucul++.so.0

uninstall-hook:
	rm -f $(DESTDIR)$(libdir)/libcucul++.so
	rm -f $(DESTDIR)$(libdir)/libcucul++.so.0

