# $Id: Makefile.am 552 2006-04-13 16:10:16Z jylam $

include_HEADERS = cucul++.h caca++.h
lib_LTLIBRARIES = libcucul++.la libcaca++.la

libcucul___la_SOURCES = \
	cucul++.cpp \
	cucul++.h \
	$(NULL)
libcucul___la_LDFLAGS = -no-undefined
libcucul___la_LIBADD = ../cucul/libcucul.la

libcaca___la_SOURCES = \
	caca++.cpp \
	caca++.h \
	$(NULL)
libcaca___la_LDFLAGS = -no-undefined
libcaca___la_LIBADD = ../caca/libcaca.la

noinst_PROGRAMS = cpptest

cpptest_SOURCES = cpptest.cpp
cpptest_LDADD = libcaca++.la libcucul++.la @CACA_LIBS@

