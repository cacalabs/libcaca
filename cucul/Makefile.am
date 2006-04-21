# $Id$

EXTRA_DIST = cucul.pc.in
DISTCLEANFILES = cucul.pc

pkgconfig_DATA = cucul.pc
pkgconfigdir = $(libdir)/pkgconfig

include_HEADERS = cucul.h

lib_LTLIBRARIES = libcucul.la

libcucul_la_SOURCES = \
	cucul.c \
	cucul.h \
	cucul_internals.h \
	buffer.c \
	canvas.c \
	transform.c \
	charset.c \
	colour.c \
	line.c \
	box.c \
	conic.c \
	triangle.c \
	sprite.c \
	dither.c \
	font.c \
	font_mono9.h \
	font_monobold12.h \
	import.c \
	export.c \
	$(NULL)
libcucul_la_LDFLAGS = -no-undefined -version-info @LT_VERSION@
libcucul_la_LIBADD = @CUCUL_LIBS@

