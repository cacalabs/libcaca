# $Id$

pkgdata_DATA = caca.txt

EXTRA_DIST = caca.txt
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/cucul -I$(top_srcdir)/caca -DLIBCACA=1 -DX_DISPLAY_MISSING=1

bin_PROGRAMS = cacademo cacafire cacaplay cacaview img2irc $(fcntl_programs)
noinst_PROGRAMS = cacadraw

cacademo_SOURCES = cacademo.c
cacademo_LDADD = ../caca/libcaca.la ../cucul/libcucul.la
cacademo_LDFLAGS = @MATH_LIBS@

cacafire_SOURCES = aafire.c
cacafire_LDADD = ../caca/libcaca.la ../cucul/libcucul.la

cacaview_SOURCES = cacaview.c common-image.c common-image.h
cacaview_LDADD = ../caca/libcaca.la ../cucul/libcucul.la
cacaview_CFLAGS = $(IMLIB2_CFLAGS)
cacaview_LDFLAGS = $(IMLIB2_LIBS)

cacadraw_SOURCES = cacadraw.c
cacadraw_LDADD = ../caca/libcaca.la ../cucul/libcucul.la

cacaplay_SOURCES = cacaplay.c
cacaplay_LDADD = ../caca/libcaca.la ../cucul/libcucul.la

cacaserver_SOURCES = cacaserver.c
cacaserver_LDADD = ../cucul/libcucul.la

img2irc_SOURCES = img2irc.c common-image.c common-image.h
img2irc_LDADD = ../cucul/libcucul.la
img2irc_CFLAGS = $(IMLIB2_CFLAGS)
img2irc_LDFLAGS = $(IMLIB2_LIBS)

if USE_NETWORK
fcntl_programs = cacaserver
else
fcntl_programs =
endif
