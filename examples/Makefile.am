
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/caca -I$(top_builddir)/caca

noinst_PROGRAMS = trifiller blit canvas colors conio $(conio_snake) demo demo0 dithering driver event export figfont font font2tga frames fullwidth gamma hsv input spritedit swallow text transform truecolor unicode import mouse

if USE_CXX
conio_snake = conio-snake
endif

trifiller_SOURCES = trifiller.c ../src/common-image.c
trifiller_CFLAGS = $(IMLIB2_CFLAGS)
trifiller_LDFLAGS = $(IMLIB2_LIBS)
trifiller_LDADD = ../caca/libcaca.la -lm

blit_SOURCES = blit.c
blit_LDADD = ../caca/libcaca.la

canvas_SOURCES = canvas.c
canvas_LDADD = ../caca/libcaca.la

colors_SOURCES = colors.c
colors_LDADD = ../caca/libcaca.la

conio_SOURCES = conio.c
conio_LDADD = ../caca/libcaca.la

conio_snake_SOURCES = conio-snake.cpp
conio_snake_LDADD = ../caca/libcaca.la

demo_SOURCES = demo.c
demo_LDADD = ../caca/libcaca.la
demo_LDFLAGS = @MATH_LIBS@

demo0_SOURCES = demo0.c
demo0_LDADD = ../caca/libcaca.la
demo0_LDFLAGS = @MATH_LIBS@

dithering_SOURCES = dithering.c
dithering_LDADD = ../caca/libcaca.la

driver_SOURCES = driver.c
driver_LDADD = ../caca/libcaca.la

event_SOURCES = event.c
event_LDADD = ../caca/libcaca.la

export_SOURCES = export.c
export_LDADD = ../caca/libcaca.la

figfont_SOURCES = figfont.c
figfont_LDADD = ../caca/libcaca.la

font_SOURCES = font.c
font_LDADD = ../caca/libcaca.la

font2tga_SOURCES = font2tga.c
font2tga_LDADD = ../caca/libcaca.la

frames_SOURCES = frames.c
frames_LDADD = ../caca/libcaca.la

fullwidth_SOURCES = fullwidth.c
fullwidth_LDADD = ../caca/libcaca.la

gamma_SOURCES = gamma.c
gamma_LDADD = ../caca/libcaca.la
gamma_LDFLAGS = @MATH_LIBS@

hsv_SOURCES = hsv.c
hsv_LDADD = ../caca/libcaca.la

input_SOURCES = input.c
input_LDADD = ../caca/libcaca.la

spritedit_SOURCES = spritedit.c
spritedit_LDADD = ../caca/libcaca.la

swallow_SOURCES = swallow.c
swallow_LDADD = ../caca/libcaca.la

text_SOURCES = text.c
text_LDADD = ../caca/libcaca.la

transform_SOURCES = transform.c
transform_LDADD = ../caca/libcaca.la

truecolor_SOURCES = truecolor.c
truecolor_LDADD = ../caca/libcaca.la

unicode_SOURCES = unicode.c
unicode_LDADD = ../caca/libcaca.la

import_SOURCES = import.c
import_LDADD = ../caca/libcaca.la

mouse_SOURCES = mouse.c
mouse_LDADD = ../caca/libcaca.la

echo-sources: ; echo $(SOURCES)

