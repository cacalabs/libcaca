# $Id$

noinst_PROGRAMS = colors demo dithering event hsv optipal spritedit

colors_SOURCES = colors.c
colors_LDADD = ../src/libcaca.la @CACA_LIBS@
colors_CPPFLAGS = -I$(top_srcdir)/src

demo_SOURCES = demo.c
demo_LDADD = ../src/libcaca.la @CACA_LIBS@ @MATH_LIBS@
demo_CPPFLAGS = -I$(top_srcdir)/src -DDATADIR=\"$(pkgdatadir)\"

dithering_SOURCES = dithering.c
dithering_LDADD = ../src/libcaca.la @CACA_LIBS@
dithering_CPPFLAGS = -I$(top_srcdir)/src

event_SOURCES = event.c
event_LDADD = ../src/libcaca.la @CACA_LIBS@
event_CPPFLAGS = -I$(top_srcdir)/src

hsv_SOURCES = hsv.c
hsv_LDADD = ../src/libcaca.la @CACA_LIBS@
hsv_CPPFLAGS = -I$(top_srcdir)/src

optipal_SOURCES = optipal.c
optipal_LDADD = ../src/libcucul.la @CUCUL_LIBS@
optipal_CPPFLAGS = -I$(top_srcdir)/src

spritedit_SOURCES = spritedit.c
spritedit_LDADD = ../src/libcaca.la @CACA_LIBS@
spritedit_CPPFLAGS = -I$(top_srcdir)/src

