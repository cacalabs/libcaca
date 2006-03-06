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
	export.c \
	$(NULL)
libcucul_la_LDFLAGS = -no-undefined
libcucul_la_LIBADD = @CUCUL_LIBS@

