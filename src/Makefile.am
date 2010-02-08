
pkgdata_DATA = caca.txt

EXTRA_DIST = caca.txt cacademo.vcproj cacafire.vcproj cacaview.vcproj
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/caca -I../caca \
              -DLIBCACA=1 -DX_DISPLAY_MISSING=1

bin_PROGRAMS = cacademo cacafire cacaplay cacaview img2txt $(fcntl_programs)
noinst_PROGRAMS = cacadraw

cacademo_SOURCES = cacademo.c texture.h
cacademo_LDADD = ../caca/libcaca.la ../caca/libcaca.la
cacademo_LDFLAGS = @MATH_LIBS@

cacafire_SOURCES = aafire.c
cacafire_LDADD = ../caca/libcaca.la ../caca/libcaca.la

cacaview_SOURCES = cacaview.c common-image.c common-image.h
cacaview_LDADD = ../caca/libcaca.la ../caca/libcaca.la
cacaview_CFLAGS = $(IMLIB2_CFLAGS)
cacaview_LDFLAGS = $(IMLIB2_LIBS)

cacadraw_SOURCES = cacadraw.c
cacadraw_LDADD = ../caca/libcaca.la ../caca/libcaca.la

cacaplay_SOURCES = cacaplay.c
cacaplay_LDADD = ../caca/libcaca.la ../caca/libcaca.la

cacaserver_SOURCES = cacaserver.c
cacaserver_LDADD = ../caca/libcaca.la

img2txt_SOURCES = img2txt.c $(GETOPT) common-image.c common-image.h
img2txt_LDADD = ../caca/libcaca.la
img2txt_CFLAGS = $(IMLIB2_CFLAGS)
img2txt_LDFLAGS = $(IMLIB2_LIBS) $(GETOPT_LIBS)

if USE_NETWORK
fcntl_programs = cacaserver
else
fcntl_programs =
endif

if NEED_GETOPT_LONG
GETOPT = mygetopt.c mygetopt.h
endif

echo-sources: ; echo $(SOURCES)

