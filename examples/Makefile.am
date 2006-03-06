# $Id$

pkgdata_DATA = caca.txt

EXTRA_DIST = caca.txt

bin_PROGRAMS = cacafire cacaball cacaplas cacaview cacamoir

cacafire_SOURCES = aafire.c
cacafire_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@
cacafire_CPPFLAGS = -I$(top_srcdir)/cucul -I$(top_srcdir)/caca -DLIBCACA=1

cacaball_SOURCES = cacaball.c
cacaball_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@ @MATH_LIBS@
cacaball_CPPFLAGS = -I$(top_srcdir)/cucul -I$(top_srcdir)/caca

cacaplas_SOURCES = cacaplas.c
cacaplas_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@ @MATH_LIBS@
cacaplas_CPPFLAGS = -I$(top_srcdir)/cucul -I$(top_srcdir)/caca

cacamoir_SOURCES = cacamoir.c
cacamoir_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@ @MATH_LIBS@
cacamoir_CPPFLAGS = -I$(top_srcdir)/cucul -I$(top_srcdir)/caca

cacaview_SOURCES = cacaview.c
cacaview_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@
cacaview_CPPFLAGS = -I$(top_srcdir)/cucul -I$(top_srcdir)/caca -DX_DISPLAY_MISSING=1
if USE_IMLIB2
cacaview_CFLAGS = `imlib2-config --cflags`
cacaview_LDFLAGS = `imlib2-config --libs`
else
cacaview_CFLAGS =
cacaview_LDFLAGS =
endif

