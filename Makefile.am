# $Id$

SUBDIRS = kernel cucul caca src test tools cxx doc
DIST_SUBDIRS = $(SUBDIRS) autotools msvc

EXTRA_DIST = NOTES COPYING.GPL COPYING.LGPL bootstrap build-dos build-kernel build-win32 caca-config.in common.h libcaca.spec
AUTOMAKE_OPTIONS = dist-bzip2

bin_SCRIPTS = caca-config

webhost = poulet.zoy.org
webdir = /srv/libcaca.zoy.org/var/www/

upload: doc/html FORCE
	scp $(distdir).tar.gz $(webhost):$(webdir)/files/
	scp -r doc/html/* $(webhost):$(webdir)/manual/

FORCE:
