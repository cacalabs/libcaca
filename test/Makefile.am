###############################################################################
# Automake targets and declarations for libee tests
###############################################################################

if USE_SLANG
LDFLAGS_slang = -lslang
endif
if USE_NCURSES
LDFLAGS_ncurses = -lncurses
endif

bin_PROGRAMS = demo
demo_SOURCES = demo.c

demo_CPPFLAGS = -I../libee
demo_LDADD = ../libee/libee.a $(LDFLAGS_slang) $(LDFLAGS_ncurses) -lm

