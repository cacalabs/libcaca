
rubylibdir = $(RUBY_SITEARCHDIR)
rubysitedir = $(RUBY_SITELIBDIR)

if USE_RUBY
rubylib_LTLIBRARIES = caca.la
rubysite_DATA = lib/caca.rb
TESTS = test
endif

CFLAGS += -Wno-strict-prototypes -Wno-missing-prototypes

caca_la_CPPFLAGS = -I$(top_srcdir)/caca -I../caca $(RUBY_CFLAGS)
caca_la_SOURCES = caca.c \
                  common.h \
                  caca-canvas.c \
                  caca-canvas.h \
                  caca-dither.c \
                  caca-dither.h \
                  caca-font.c \
                  caca-font.h \
                  caca-display.c \
                  caca-display.h \
                  caca-event.c \
                  caca-event.h \
                  $(NULL)
caca_la_LDFLAGS = -module -avoid-version -shared $(RUBY_LIBS)
caca_la_LIBADD = ../caca/libcaca.la

EXTRA_DIST = ruby.dox \
             ruby-caca.dox \
             lib/caca.rb \
             test \
             t/tc_canvas.rb \
             t/tc_dither.rb \
             t/tc_font.rb \
             t/tc_frame.rb \
             README \
             $(NULL)

echo-sources: ; echo $(SOURCES)

