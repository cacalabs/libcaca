###############################################################################
# Automake targets and declarations for libee tests
###############################################################################

AM_CPPFLAGS = -I$(top_srcdir)/libee

if USE_SLANG
LDFLAGS_slang = -lslang
endif
if USE_NCURSES
LDFLAGS_ncurses = -lncurses
endif

bin_PROGRAMS = demo spritedit

demo_SOURCES = demo.c
demo_LDADD = ../libee/libee.a $(LDFLAGS_slang) $(LDFLAGS_ncurses) -lm

spritedit_SOURCES = spritedit.c
spritedit_LDADD = ../libee/libee.a $(LDFLAGS_slang) $(LDFLAGS_ncurses) -lm

