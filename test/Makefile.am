# $Id$

noinst_PROGRAMS = colors demo dithering event hsv optipal spritedit

colors_SOURCES = colors.c
colors_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@
colors_CPPFLAGS = -I$(top_srcdir)/cucul -I$(top_srcdir)/caca

demo_SOURCES = demo.c
demo_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@ @MATH_LIBS@
demo_CPPFLAGS = -I$(top_srcdir)/cucul -I$(top_srcdir)/caca -DDATADIR=\"$(pkgdatadir)\"

dithering_SOURCES = dithering.c
dithering_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@
dithering_CPPFLAGS = -I$(top_srcdir)/cucul -I$(top_srcdir)/caca

event_SOURCES = event.c
event_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@
event_CPPFLAGS = -I$(top_srcdir)/cucul -I$(top_srcdir)/caca

hsv_SOURCES = hsv.c
hsv_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@
hsv_CPPFLAGS = -I$(top_srcdir)/cucul -I$(top_srcdir)/caca

optipal_SOURCES = optipal.c
optipal_LDADD = ../cucul/libcucul.la @CUCUL_LIBS@
optipal_CPPFLAGS = -I$(top_srcdir)/cucul

spritedit_SOURCES = spritedit.c
spritedit_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@
spritedit_CPPFLAGS = -I$(top_srcdir)/cucul -I$(top_srcdir)/caca

