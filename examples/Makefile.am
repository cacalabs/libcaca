###############################################################################
# Automake targets and declarations for libcaca tests
###############################################################################

AM_CPPFLAGS = -I$(top_srcdir)/src

pkgdata_DATA = caca.txt

EXTRA_DIST = caca.txt

if USE_SLANG
LDFLAGS_slang = -lslang
endif
if USE_NCURSES
LDFLAGS_ncurses = -lncurses
endif

bin_PROGRAMS = caca-demo caca-spritedit

caca_demo_SOURCES = demo.c
caca_demo_LDADD = ../src/libcaca.a $(LDFLAGS_slang) $(LDFLAGS_ncurses) -lm
caca_demo_CPPFLAGS = -DDATADIR=\"$(pkgdatadir)\"
#caca_demo_CFLAGS = `pkg-config --cflags gtk+-2.0`
#caca_demo_LDFLAGS = `pkg-config --libs gtk+-2.0`

caca_spritedit_SOURCES = spritedit.c
caca_spritedit_LDADD = ../src/libcaca.a $(LDFLAGS_slang) $(LDFLAGS_ncurses) -lm

