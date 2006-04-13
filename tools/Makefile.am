# $Id$

AM_CPPFLAGS = -I$(top_srcdir)/cucul -I$(top_srcdir)/caca -DDATADIR=\"$(pkgdatadir)\"

noinst_PROGRAMS = optipal $(pango_programs)

optipal_SOURCES = optipal.c

makefont_SOURCES = makefont.c
makefont_CFLAGS = `pkg-config --cflags pangoft2`
makefont_LDFLAGS = `pkg-config --libs pangoft2`

if USE_PANGO
pango_programs = makefont
else
pango_programs =
endif
