# $Id$

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/cucul -I$(top_srcdir)/caca -DDATADIR=\"$(pkgdatadir)\"

noinst_PROGRAMS = optipal $(pango_programs)

optipal_SOURCES = optipal.c

makefont_SOURCES = makefont.c
makefont_LDADD = ../cucul/libcucul.la
makefont_CFLAGS = $(PANGOFT2_CFLAGS)
makefont_LDFLAGS = $(PANGOFT2_LIBS)

if USE_PANGO
pango_programs = makefont
else
pango_programs =
endif
