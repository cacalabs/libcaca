###############################################################################
# Automake targets and declarations for libee
###############################################################################

AM_CFLAGS = -g -O2 -fno-strength-reduce -fomit-frame-pointer

# Code qui fait des warnings == code de porc == deux baffes dans ta gueule
AM_CFLAGS += -Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wstrict-prototypes -Wshadow -Waggregate-return -Wmissing-prototypes -Wnested-externs

lib_LIBRARIES = libee.a
libee_a_SOURCES = \
	ee.c \
	ee.h \
	io.c \
	math.c \
	line.c \
	circle.c \
	triangle.c \
	$(NULL)

