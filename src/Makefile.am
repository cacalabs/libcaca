# $Id$

pkgdata_DATA = caca.txt

EXTRA_DIST = caca.txt
AM_CPPFLAGS = -I$(top_srcdir)/cucul -I$(top_srcdir)/caca -DLIBCACA=1 -DX_DISPLAY_MISSING=1

bin_PROGRAMS = cacafire cacaball cacaplas cacamoir cacaplay cacaview img2irc $(fcntl_programs)

cacafire_SOURCES = aafire.c
cacafire_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@

cacaball_SOURCES = cacaball.c
cacaball_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@ @MATH_LIBS@

cacaplas_SOURCES = cacaplas.c
cacaplas_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@ @MATH_LIBS@

cacamoir_SOURCES = cacamoir.c
cacamoir_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@ @MATH_LIBS@

cacaview_SOURCES = cacaview.c common-image.c common-image.h
cacaview_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@
cacaview_CFLAGS = $(img_cflags)
cacaview_LDFLAGS = $(img_ldflags)

cacaplay_SOURCES = cacaplay.c
cacaplay_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@

cacaserver_SOURCES = cacaserver.c
cacaserver_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@

img2irc_SOURCES = img2irc.c common-image.c common-image.h
img2irc_LDADD = ../cucul/libcucul.la @CACA_LIBS@
img2irc_CFLAGS = $(img_cflags)
img2irc_LDFLAGS = $(img_ldflags)

if USE_IMLIB2
img_cflags = `imlib2-config --cflags`
img_ldflags = `imlib2-config --libs`
else
img_cflags =
img_ldflags =
endif

if USE_FCNTL
fcntl_programs = cacaserver
else
fcntl_programs =
endif

