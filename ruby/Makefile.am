# $Id: $

rubylibdir = $(RUBY_SITEARCHDIR)
rubysitedir = $(RUBY_SITELIBDIR)

if USE_RUBY
rubylib_LTLIBRARIES = caca.la cucul.la
rubysite_DATA = lib/caca.rb
TESTS = test
endif

CFLAGS += -Wno-strict-prototypes -Wno-missing-prototypes

cucul_la_CPPFLAGS = -I$(top_srcdir)/cucul -I../cucul $(RUBY_CFLAGS)
cucul_la_SOURCES = cucul.c \
                   common.h \
                   cucul-canvas.c \
                   cucul-canvas.h \
                   cucul-dither.c \
                   cucul-dither.h \
                   cucul-font.c \
                   cucul-font.h \
                   $(NULL)
cucul_la_LDFLAGS = -module -avoid-version -shared $(RUBY_LIBS)
cucul_la_LIBADD = ../cucul/libcucul.la

caca_la_CPPFLAGS = -I$(top_srcdir)/caca -I$(top_srcdir)/cucul -I../cucul \
                   $(RUBY_CFLAGS)
caca_la_SOURCES = caca.c \
                  common.h \
                  caca-display.c \
                  caca-display.h \
                  caca-event.c \
                  caca-event.h \
                  $(NULL)
caca_la_LDFLAGS = -module -avoid-version -shared $(RUBY_LIBS)
caca_la_LIBADD = ../caca/libcaca.la

EXTRA_DIST = ruby.dox \
             ruby-caca.dox \
             ruby-cucul.dox \
             lib/caca.rb \
             t/tc_canvas.rb \
             t/tc_dither.rb \
             t/tc_font.rb \
             t/tc_frame.rb \
             README \
             $(NULL)

