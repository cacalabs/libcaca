# $Id: Makefile.am 552 2006-04-13 16:10:16Z jylam $

SUFFIXES = .cpp

AM_CXXFLAGS = 

include_HEADERS = cucul++.h caca++.h

noinst_LIBRARIES = libcucul++.a libcaca++.a

libcucul___a_SOURCES = \
	cucul++.cpp \
	cucul++.h \
	$(NULL)
libcucul___la_LDFLAGS = -no-undefined
libcucul___la_LIBADD = 


libcaca___a_SOURCES = \
	caca++.cpp \
	caca++.h \
	$(NULL)
libcaca___la_LDFLAGS = -no-undefined
libcaca___la_LIBADD = 



noinst_PROGRAMS = cpptest


cpptest_SOURCES = cpptest.cpp
cpptest_LDADD = ../caca/libcaca.la ../cucul/libcucul.la libcucul++.a libcaca++.a @CACA_LIBS@