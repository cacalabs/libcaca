# $Id: $
rubylibdir=$(RUBY_SITEARCHDIR)

if USE_RUBY
rubylib_LTLIBRARIES = caca.la cucul.la
TESTS = test
endif

cucul_la_CPPFLAGS = -I$(top_srcdir)/cucul -I$(RUBY_ARCHDIR)
cucul_la_SOURCES = cucul.c cucul-canvas.c cucul-dither.c cucul-font.c
cucul_la_LDFLAGS = -module -avoid-version -shared -L$(RUBY_LIBDIR) -l$(RUBY_SO_NAME)
cucul_la_LIBADD = ../cucul/libcucul.la

caca_la_CPPFLAGS = -I$(top_srcdir)/caca -I$(RUBY_ARCHDIR)
caca_la_SOURCES = caca.c caca-display.c caca-event.c
caca_la_LDFLAGS = -module -avoid-version -shared -L$(RUBY_LIBDIR) -l$(RUBY_SO_NAME)
caca_la_LIBADD = ../caca/libcaca.la

EXTRA_DIST = cucul-canvas.h \
             cucul-dither.h \
             cucul-font.h \
             caca-display.h \
             caca-event.h \
             common.h \
             test.rb \
             t/tc_canvas.rb \
             t/tc_dither.rb \
             t/tc_font.rb \
             t/tc_frame.rb \
             README

CFLAGS+=-Wno-strict-prototypes -Wno-missing-prototypes
