###############################################################################
# Automake targets and declarations for libee tests
###############################################################################

AM_CFLAGS = -g -O2 -fno-strength-reduce -fomit-frame-pointer

# Code qui fait des warnings == code de porc == deux baffes dans ta gueule
AM_CFLAGS += -Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wstrict-prototypes -Wshadow -Waggregate-return -Wmissing-prototypes -Wnested-externs

if USE_SLANG
CPPFLAGS_slang = -DUSE_SLANG
LDFLAGS_slang = -lslang
endif
if USE_NCURSES
CPPFLAGS_ncurses = -DUSE_NCURSES
LDFLAGS_ncurses = -lncurses
endif

bin_PROGRAMS = demo
demo_SOURCES = demo.c

demo_CPPFLAGS = -I../libee $(CPPFLAGS_slang) $(CPPFLAGS_ncurses)
demo_LDADD = ../libee/libee.a $(LDFLAGS_slang) $(LDFLAGS_ncurses) -lm

