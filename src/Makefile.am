###############################################################################
# Automake targets and declarations for ttyvaders
###############################################################################

if USE_SLANG
LDFLAGS_slang = -lslang
endif
if USE_NCURSES
LDFLAGS_ncurses = -lncurses
endif

bin_PROGRAMS = ttyvaders

ttyvaders_SOURCES = \
	aliens.c \
	bonus.c \
	box.c \
	ceo.c \
	collide.c \
	common.h \
	explosions.c \
	main.c \
	overlay.c \
	player.c \
	starfield.c \
	tunnel.c \
	weapons.c \
	$(NULL)

ttyvaders_CPPFLAGS = -I../libee
ttyvaders_LDADD = ../libee/libee.a $(LDFLAGS_slang) $(LDFLAGS_ncurses)

