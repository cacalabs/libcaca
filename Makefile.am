
include $(top_srcdir)/build/autotools/common.am

SUBDIRS = kernel caca src examples tools \
          cxx caca-sharp python ruby java doc
DIST_SUBDIRS = $(SUBDIRS) build

EXTRA_DIST = NOTES COPYING.GPL COPYING.ISC COPYING.LGPL bootstrap caca-config.in libcaca.spec libcaca.sln
AUTOMAKE_OPTIONS = dist-bzip2

bin_SCRIPTS = caca-config

webhost = poulet.zoy.org
webdir = /srv/caca.zoy.org/var/www/
doxdir = /srv/caca.zoy.org/var/cache/doxygen/libcaca

upload: upload-source upload-doc

upload-source: FORCE
	scp $(distdir).tar.gz $(webhost):$(webdir)/files/

upload-doc: doc/html FORCE
	ssh $(webhost) rm -f '$(doxdir)/*' || true
	scp -r doc/html/* $(webhost):$(doxdir)/

update-fonts: tools/makefont
	tools/makefont mono9 "Monospace 9" 96 4 >| $(srcdir)/caca/mono9.data
	tools/makefont monobold12 "Monospace Bold 12" 96 4 >| $(srcdir)/caca/monobold12.data

# Travis CI uses “make test” instead of “make check”
test: check

echo-dirs: ; echo caca kernel src cxx examples ruby python tools

FORCE:
