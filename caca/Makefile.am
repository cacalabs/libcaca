# $Id$

EXTRA_DIST = caca.pc.in
DISTCLEANFILES = caca.pc

pkgconfig_DATA = caca.pc
pkgconfigdir = $(libdir)/pkgconfig

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
	driver_raw.c \
	driver_slang.c \
	driver_vga.c \
	driver_win32.c \
	driver_x11.c \
	$(NULL)
AM_CPPFLAGS = -I$(top_srcdir)/cucul
libcaca_la_LDFLAGS = -no-undefined -version-info @LT_VERSION@
libcaca_la_LIBADD = ../cucul/libcucul.la @CACA_LIBS@

