###############################################################################
# Automake targets and declarations for libcaca tests
###############################################################################

pkgdata_DATA = caca.txt

EXTRA_DIST = caca.txt

bin_PROGRAMS = cacademo cacafire caca-spritedit $(cacaview)

cacademo_SOURCES = demo.c
cacademo_LDADD = ../src/libcaca.a @CACA_LIBS@ -lm
cacademo_CPPFLAGS = -I$(top_srcdir)/src -DDATADIR=\"$(pkgdatadir)\"

cacafire_SOURCES = aafire.c
cacafire_LDADD = ../src/libcaca.a @CACA_LIBS@
cacafire_CPPFLAGS = -I$(top_srcdir)/src -DLIBCACA=1

caca_spritedit_SOURCES = spritedit.c
caca_spritedit_LDADD = ../src/libcaca.a @CACA_LIBS@
caca_spritedit_CPPFLAGS = -I$(top_srcdir)/src

if USE_IMLIB2
cacaview = cacaview
cacaview_SOURCES = view.c
cacaview_LDADD = ../src/libcaca.a @CACA_LIBS@
cacaview_CPPFLAGS = -I$(top_srcdir)/src -DX_DISPLAY_MISSING=1
cacaview_CFLAGS = `imlib2-config --cflags`
cacaview_LDFLAGS = `imlib2-config --libs`
endif

