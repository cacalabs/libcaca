###############################################################################
# Automake targets and declarations for ttyvaders
###############################################################################

AM_CFLAGS = -g -O2 -fno-strength-reduce -fomit-frame-pointer

# Code qui fait des warnings == code de porc == deux baffes dans ta gueule
AM_CFLAGS += -Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wstrict-prototypes -Wshadow -Waggregate-return -Wmissing-prototypes -Wnested-externs

if USE_NCURSES
CPPFLAGS_gfx = -DUSE_NCURSES
LDFLAGS_gfx = -lncurses
else
CPPFLAGS_gfx = -DUSE_SLANG
LDFLAGS_gfx = -lslang
endif

bin_PROGRAMS = ttyvaders

ttyvaders_SOURCES = \
	aliens.c \
	bonus.c \
	ceo.c \
	common.h \
	explosions.c \
	main.c \
	starfield.c \
	weapons.c \
	collide.c \
	graphics.c \
	player.c \
	tunnel.c \
	$(NULL)

ttyvaders_CPPFLAGS = $(CPPFLAGS_gfx)
ttyvaders_LDADD = $(LDFLAGS_gfx)

