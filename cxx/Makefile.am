# $Id: Makefile.am 552 2006-04-13 16:10:16Z jylam $

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/cucul -I../cucul \
              -I$(top_srcdir)/caca

if USE_CXX
pkgconfig_DATA = cucul++.pc caca++.pc
pkgconfigdir = $(libdir)/pkgconfig

include_HEADERS = cucul++.h caca++.h
lib_LTLIBRARIES = libcucul++.la libcaca++.la
endif

libcucul___la_SOURCES = cucul++.cpp cucul++.h
libcucul___la_LDFLAGS = -no-undefined -version-number @LT_VERSION@
libcucul___la_LIBADD = ../cucul/libcucul.la

libcaca___la_SOURCES = caca++.cpp caca++.h
libcaca___la_LDFLAGS = -no-undefined -version-number @LT_VERSION@
libcaca___la_LIBADD = libcucul++.la ../caca/libcaca.la

if USE_CXX
noinst_PROGRAMS = cxxtest
endif

cxxtest_SOURCES = cxxtest.cpp
cxxtest_LDADD = libcaca++.la libcucul++.la ../caca/libcaca.la ../cucul/libcucul.la

