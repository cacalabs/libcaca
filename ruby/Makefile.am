# $Id: $
rubylibdir=$(RUBY_SITEARCHDIR)

if USE_RUBY
rubylib_LTLIBRARIES = cucul.la
TESTS = test
endif

cucul_la_CPPFLAGS = -I$(top_srcdir)/cucul -I$(RUBY_ARCHDIR)
cucul_la_SOURCES = cucul.c cucul-canvas.c
cucul_la_LDFLAGS = -module -avoid-version -shared -L$(RUBY_LIBDIR) -l$(RUBY_SO_NAME)
cucul_la_LIBADD = ../cucul/libcucul.la

EXTRA_DIST = cucul-canvas.h \
	     test.rb \
             t/tc_frame.rb \
             README

CFLAGS+=-Wno-strict-prototypes -Wno-missing-prototypes
