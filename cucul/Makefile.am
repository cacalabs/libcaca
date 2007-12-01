# $Id$

EXTRA_DIST = cucul.pc.in mono9.data monobold12.data
DISTCLEANFILES = cucul.pc

pkgconfig_DATA = cucul.pc
pkgconfigdir = $(libdir)/pkgconfig

include_HEADERS = cucul.h

lib_LTLIBRARIES = libcucul.la

libcucul_la_SOURCES = \
	cucul.c \
	cucul.h \
	cucul_internals.h \
	legacy.c \
	canvas.c \
	transform.c \
	charset.c \
	attr.c \
	line.c \
	box.c \
	conic.c \
	triangle.c \
	frame.c \
	dither.c \
	font.c \
	import.c \
	export.c \
	file.c \
	$(NULL)
libcucul_la_DEPENDENCIES = \
	mono9.data \
	monobold12.data \
	$(NULL)
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/cucul
libcucul_la_LDFLAGS = -no-undefined -version-number @LT_VERSION@
libcucul_la_LIBADD = $(ZLIB_LIBS)

