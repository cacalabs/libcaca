
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/caca -I../caca \
              -I$(top_srcdir)/caca

noinst_PROGRAMS = optipal sortchars $(pango_programs)

optipal_SOURCES = optipal.c

sortchars_SOURCES = sortchars.c
sortchars_LDADD = ../caca/libcaca.la

makefont_SOURCES = makefont.c
makefont_LDADD = ../caca/libcaca.la
makefont_CFLAGS = $(PANGOFT2_CFLAGS)
makefont_LDFLAGS = $(PANGOFT2_LIBS)

if USE_PANGO
pango_programs = makefont
else
pango_programs =
endif

echo-sources: ; echo $(SOURCES)

