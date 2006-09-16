# $Id$

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/cucul -I$(top_srcdir)/caca -DDATADIR=\"$(pkgdatadir)\"

noinst_PROGRAMS = colors demo demo0 dithering event export font frames gamma hsv input spritedit font2tga text transform truecolor unicode import

colors_SOURCES = colors.c
colors_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@

demo_SOURCES = demo.c
demo_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@ @MATH_LIBS@

demo0_SOURCES = demo0.c
demo0_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@ @MATH_LIBS@

dithering_SOURCES = dithering.c
dithering_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@

event_SOURCES = event.c
event_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@

export_SOURCES = export.c
export_LDADD = ../cucul/libcucul.la

font_SOURCES = font.c
font_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@

font2tga_SOURCES = font2tga.c
font2tga_LDADD = ../cucul/libcucul.la

frames_SOURCES = frames.c
frames_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@

gamma_SOURCES = gamma.c
gamma_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@ @MATH_LIBS@

hsv_SOURCES = hsv.c
hsv_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@

input_SOURCES = input.c
input_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@

spritedit_SOURCES = spritedit.c
spritedit_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@

text_SOURCES = text.c
text_LDADD = ../cucul/libcucul.la

transform_SOURCES = transform.c
transform_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@

truecolor_SOURCES = truecolor.c
truecolor_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@

unicode_SOURCES = unicode.c
unicode_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@

import_SOURCES = import.c
import_LDADD = ../caca/libcaca.la ../cucul/libcucul.la @CACA_LIBS@
