# $Id: $

if USE_RUBY
noinst_DATA = cucul/cucul.so caca/caca.so
endif

EXTRA_DIST = cucul/extconf.rb \
             cucul/cucul.c \
             cucul/cucul-canvas.c \
             cucul/cucul-canvas.h \
             cucul/test.rb \
             cucul/t/tc_frame.rb \
             caca/extconf.rb

DISTCLEANFILES = cucul/Makefile caca/Makefile

cucul/cucul.so:
	ruby -C cucul extconf.rb
	make -C cucul

caca/caca.so: cucul/cucul.so
	ruby -C caca extconf.rb
	make -C caca

clean-local:
	[ ! -f cucul/Makefile ] || make -C cucul clean
	[ ! -f caca/Makefile ] || make -C caca clean

distclean-local:
	rm -f cucul/Makefile caca/Makefile
