###############################################################################
# Automake targets and declarations for libcaca test programs
###############################################################################

noinst_PROGRAMS = dithering optipal

dithering_SOURCES = dithering.c
dithering_LDADD = ../src/libcaca.a @CACA_LIBS@
dithering_CPPFLAGS = -I$(top_srcdir)/src

optipal_SOURCES = optipal.c
optipal_LDADD = ../src/libcaca.a @CACA_LIBS@
optipal_CPPFLAGS = -I$(top_srcdir)/src

