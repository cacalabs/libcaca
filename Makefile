
# Fallback to ncurses if this is set to 0
USE_SLANG = 1

CC = gcc
CFLAGS = -g -O6 -fno-strength-reduce -fomit-frame-pointer

# Code qui fait des warnings == code de porc == deux baffes dans ta gueule
CFLAGS += -Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wstrict-prototypes -Wshadow -Waggregate-return -Wmissing-prototypes -Wnested-externs

ifeq (1,$(USE_SLANG))
CFLAGS += -DUSE_SLANG
EXECLIBS = -lslang -lm
else
CFLAGS += -DUSE_NCURSES
EXECLIBS = -lncurses -lm
endif

COMPILE_CMD = $(CC) -c $(ALL_CFLAGS)

PROGRAM = ttyvaders
SOURCES = main.c graphics.c tunnel.c starfield.c player.c weapons.c collide.c explosions.c aliens.c
HEADERS = common.h
OBJECTS = $(SOURCES:%.c=%.o)

all: $(PROGRAM)

ttyvaders: $(OBJECTS)
	$(CC) -o $@ $(LDFLAGS) $^ $(EXECLIBS)

$(OBJECTS): Makefile $(HEADERS)

clean:
	-/bin/rm -f $(PROGRAM) $(OBJECTS)

