# $Id$

include_HEADERS = caca.h

lib_LTLIBRARIES = libcaca.la

libcaca_la_SOURCES = \
	caca.c \
	caca.h \
	caca_internals.h \
	graphics.c \
	event.c \
	time.c \
	$(NULL)
libcaca_la_CPPFLAGS = -I$(top_srcdir)/cucul
libcaca_la_LDFLAGS = -no-undefined
libcaca_la_LIBADD = ../cucul/libcucul.la @CACA_LIBS@

