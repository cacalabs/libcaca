# $Id: foo$

pkgdata_DATA = caca.txt

EXTRA_DIST = caca.txt

bin_PROGRAMS = cacademo cacafire cacaball cacaplas cacaview cacamoir

cacademo_SOURCES = demo.c
cacademo_LDADD = ../src/libcaca.a @CACA_LIBS@ @MATH_LIBS@
cacademo_CPPFLAGS = -I$(top_srcdir)/src -DDATADIR=\"$(pkgdatadir)\"

cacafire_SOURCES = aafire.c
cacafire_LDADD = ../src/libcaca.a @CACA_LIBS@
cacafire_CPPFLAGS = -I$(top_srcdir)/src -DLIBCACA=1

cacaball_SOURCES = cacaball.c
cacaball_LDADD = ../src/libcaca.a @CACA_LIBS@ @MATH_LIBS@
cacaball_CPPFLAGS = -I$(top_srcdir)/src

cacaplas_SOURCES = cacaplas.c
cacaplas_LDADD = ../src/libcaca.a @CACA_LIBS@ @MATH_LIBS@
cacaplas_CPPFLAGS = -I$(top_srcdir)/src

cacamoir_SOURCES = cacamoir.c
cacamoir_LDADD = ../src/libcaca.a @CACA_LIBS@ @MATH_LIBS@
cacamoir_CPPFLAGS = -I$(top_srcdir)/src

cacaview_SOURCES = cacaview.c
cacaview_LDADD = ../src/libcaca.a @CACA_LIBS@
cacaview_CPPFLAGS = -I$(top_srcdir)/src -DX_DISPLAY_MISSING=1
if USE_IMLIB2
cacaview_CFLAGS = `imlib2-config --cflags`
cacaview_LDFLAGS = `imlib2-config --libs`
else
cacaview_CFLAGS =
cacaview_LDFLAGS =
endif

