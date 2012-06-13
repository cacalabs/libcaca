
SUBDIRS = kernel caca src examples tools \
          cxx caca-sharp python ruby java \
          doc test
DIST_SUBDIRS = $(SUBDIRS) win32

EXTRA_DIST = NOTES COPYING.GPL COPYING.ISC COPYING.LGPL bootstrap build-dos build-kernel build-win32 build-win64 caca-config.in libcaca.spec libcaca.sln
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

update-changelog: FORCE
	git --help >/dev/null 2>&1 \
	  && test -d .git \
	  && git log --stat | awk 'function flush() { if (m != "") { print "Commit: " v "\nAuthor: " a m } m=""; v="UNSUBMITTED" } { if ($$1 == "commit") flush(); else if ($$1 == "Author:") { a = $$2 } else if ($$1 == "git-svn-id:") { split($$2,tmp,"@"); v=tmp[2] } else if ($$_ != "    ") { m=m "\n" $$_ } } END { flush() }' > ChangeLog

update-fonts: tools/makefont
	tools/makefont mono9 "Monospace 9" 96 4 >| $(srcdir)/caca/mono9.data
	tools/makefont monobold12 "Monospace Bold 12" 96 4 >| $(srcdir)/caca/monobold12.data

echo-dirs: ; echo caca kernel src cxx examples ruby python tools

FORCE:
