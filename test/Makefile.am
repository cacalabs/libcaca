# $Id$

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/cucul -I$(top_srcdir)/caca -DDATADIR=\"$(pkgdatadir)\"

noinst_PROGRAMS = blit colors cucul demo demo0 dithering event export font font2tga frames fullwidth gamma hsv input spritedit swallow text transform truecolor unicode import $(fcntl_programs)

blit_SOURCES = blit.c
blit_LDADD = ../caca/libcaca.la ../cucul/libcucul.la

colors_SOURCES = colors.c
colors_LDADD = ../caca/libcaca.la ../cucul/libcucul.la

cucul_SOURCES = cucul.c
cucul_LDADD = ../cucul/libcucul.la

demo_SOURCES = demo.c
demo_LDADD = ../caca/libcaca.la ../cucul/libcucul.la
demo_LDFLAGS = @MATH_LIBS@

demo0_SOURCES = demo0.c
demo0_LDADD = ../caca/libcaca.la ../cucul/libcucul.la
demo0_LDFLAGS = @MATH_LIBS@

dithering_SOURCES = dithering.c
dithering_LDADD = ../caca/libcaca.la ../cucul/libcucul.la

event_SOURCES = event.c
event_LDADD = ../caca/libcaca.la ../cucul/libcucul.la

export_SOURCES = export.c
export_LDADD = ../cucul/libcucul.la

font_SOURCES = font.c
font_LDADD = ../caca/libcaca.la ../cucul/libcucul.la

font2tga_SOURCES = font2tga.c
font2tga_LDADD = ../cucul/libcucul.la

frames_SOURCES = frames.c
frames_LDADD = ../caca/libcaca.la ../cucul/libcucul.la

fullwidth_SOURCES = fullwidth.c
fullwidth_LDADD = ../caca/libcaca.la ../cucul/libcucul.la

gamma_SOURCES = gamma.c
gamma_LDADD = ../caca/libcaca.la ../cucul/libcucul.la
gamma_LDFLAGS = @MATH_LIBS@

hsv_SOURCES = hsv.c
hsv_LDADD = ../caca/libcaca.la ../cucul/libcucul.la

input_SOURCES = input.c
input_LDADD = ../caca/libcaca.la ../cucul/libcucul.la

spritedit_SOURCES = spritedit.c
spritedit_LDADD = ../caca/libcaca.la ../cucul/libcucul.la

swallow_SOURCES = swallow.c
swallow_LDADD = ../caca/libcaca.la ../cucul/libcucul.la

term_SOURCES = term.c
term_LDADD = ../caca/libcaca.la ../cucul/libcucul.la
term_LDFLAGS = @UTIL_LIBS@

text_SOURCES = text.c
text_LDADD = ../cucul/libcucul.la

transform_SOURCES = transform.c
transform_LDADD = ../caca/libcaca.la ../cucul/libcucul.la

truecolor_SOURCES = truecolor.c
truecolor_LDADD = ../caca/libcaca.la ../cucul/libcucul.la

unicode_SOURCES = unicode.c
unicode_LDADD = ../caca/libcaca.la ../cucul/libcucul.la

import_SOURCES = import.c
import_LDADD = ../caca/libcaca.la ../cucul/libcucul.la

if USE_NETWORK
fcntl_programs = term
else
fcntl_programs =
endif
