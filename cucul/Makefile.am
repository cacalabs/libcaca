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
	canvas.c \
	transform.c \
	charset.c \
	colour.c \
	math.c \
	line.c \
	box.c \
	conic.c \
	triangle.c \
	sprite.c \
	bitmap.c \
	export_irc.c \
	export_ansi.c \
	export_html.c \
	export_ps.c \
	export_svg.c \
	$(NULL)
libcucul_la_LDFLAGS = -no-undefined
libcucul_la_LIBADD = @CUCUL_LIBS@

