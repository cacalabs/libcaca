###############################################################################
# Automake targets and declarations for libcaca tests
###############################################################################

pkgdata_DATA = caca.txt

EXTRA_DIST = caca.txt

if USE_SLANG
LDFLAGS_slang = -lslang
endif
if USE_NCURSES
LDFLAGS_ncurses = -lncurses
endif

bin_PROGRAMS = cacademo caca-spritedit $(cacaview)

cacademo_SOURCES = demo.c
cacademo_LDADD = ../src/libcaca.a $(LDFLAGS_slang) $(LDFLAGS_ncurses) -lm
cacademo_CPPFLAGS = -I$(top_srcdir)/src -DDATADIR=\"$(pkgdatadir)\"

caca_spritedit_SOURCES = spritedit.c
caca_spritedit_LDADD = ../src/libcaca.a $(LDFLAGS_slang) $(LDFLAGS_ncurses) -lm
caca_spritedit_CPPFLAGS = -I$(top_srcdir)/src

if USE_IMLIB2
cacaview = cacaview
cacaview_SOURCES = view.c
cacaview_LDADD = ../src/libcaca.a $(LDFLAGS_slang) $(LDFLAGS_ncurses)
cacaview_CPPFLAGS = -I$(top_srcdir)/src -DX_DISPLAY_MISSING=1
cacaview_CFLAGS = `imlib2-config --cflags`
cacaview_LDFLAGS = `imlib2-config --libs`
endif

