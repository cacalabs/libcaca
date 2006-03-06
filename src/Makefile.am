# $Id$

include_HEADERS = cucul.h caca.h

lib_LTLIBRARIES = libcucul.la libcaca.la

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

libcaca_la_SOURCES = \
	caca.c \
	caca.h \
	caca_internals.h \
	graphics.c \
	event.c \
	time.c \
	$(NULL)
libcaca_la_LDFLAGS = -no-undefined
libcaca_la_LIBADD = libcucul.la @CACA_LIBS@

