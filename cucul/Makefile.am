# $Id$

include_HEADERS = cucul.h

lib_LTLIBRARIES = libcucul.la

libcucul_la_SOURCES = \
	cucul.c \
	cucul.h \
	cucul_internals.h \
	char.c \
	math.c \
	line.c \
	box.c \
	conic.c \
	triangle.c \
	sprite.c \
	bitmap.c \
	exporter_irc.c \
	exporter_ansi.c \
	exporter_html.c \
	$(NULL)
libcucul_la_LDFLAGS = -no-undefined
libcucul_la_LIBADD = @CUCUL_LIBS@

