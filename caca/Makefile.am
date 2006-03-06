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
	driver_conio.c \
	driver_gl.c \
	driver_ncurses.c \
	driver_slang.c \
	driver_win32.c \
	driver_x11.c \
	$(NULL)
libcaca_la_CPPFLAGS = -I$(top_srcdir)/cucul
libcaca_la_LDFLAGS = -no-undefined
libcaca_la_LIBADD = ../cucul/libcucul.la @CACA_LIBS@

