# $Id$

SUBDIRS = kernel cucul caca src test tools cxx DotNet ruby doc
DIST_SUBDIRS = $(SUBDIRS) msvc

EXTRA_DIST = NOTES COPYING.GPL COPYING.LGPL bootstrap build-dos build-kernel build-win32 caca-config.in common.h libcaca.spec
AUTOMAKE_OPTIONS = dist-bzip2

bin_SCRIPTS = caca-config

webhost = poulet.zoy.org
webdir = /srv/libcaca.zoy.org/var/www/

upload: doc/html FORCE
	scp $(distdir).tar.gz $(webhost):$(webdir)/files/
	scp -r doc/html/* $(webhost):$(webdir)/manual/

fonts: tools/makefont
	tools/makefont mono9 "Monospace 9" 96 4 >| $(srcdir)/cucul/mono9.data
	tools/makefont monobold12 "Monospace Bold 12" 96 4 >| $(srcdir)/cucul/monobold12.data

FORCE:
