
pkgdata_DATA = caca.txt

EXTRA_DIST = caca.txt cacademo.vcxproj cacafire.vcxproj cacaview.vcxproj
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/caca -I../caca \
              -DLIBCACA=1 -DX_DISPLAY_MISSING=1

bin_PROGRAMS = cacademo cacafire cacaplay cacaview img2txt cacaclock $(fcntl_programs)
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

cacaclock_SOURCES = cacaclock.c
cacaclock_LDADD = ../caca/libcaca.la


img2txt_SOURCES = img2txt.c common-image.c common-image.h
img2txt_LDADD = ../caca/libcaca.la
img2txt_CFLAGS = $(IMLIB2_CFLAGS)
img2txt_LDFLAGS = $(IMLIB2_LIBS)

if USE_NETWORK
fcntl_programs = cacaserver
else
fcntl_programs =
endif

echo-sources: ; echo $(SOURCES)

