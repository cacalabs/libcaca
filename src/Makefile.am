# $Id$

include_HEADERS = caca.h

lib_LTLIBRARIES = libcaca.la
libcaca_la_SOURCES = \
	caca.c \
	caca.h \
	caca_internals.h \
	graphics.c \
	event.c \
	math.c \
	line.c \
	box.c \
	conic.c \
	triangle.c \
	sprite.c \
	bitmap.c \
	time.c \
	$(NULL)
libcaca_la_LDFLAGS = -no-undefined
libcaca_la_LIBADD = @CACA_LIBS@

if NEED_PIC
libcaca_pic_a = libcaca_pic.a
endif
lib_LIBRARIES = $(libcaca_pic_a)
libcaca_pic_a_SOURCES = $(libcaca_la_SOURCES)
libcaca_pic_a_CPPFLAGS = -fPIC -DPIC

