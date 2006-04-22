# $Id$

EXTRA_DIST = doxygen.cfg.in footer.html header.html $(man_MANS) $(doxygen_DOX)

doxygen_DOX = libcaca.dox user.dox migrating.dox tutorial.dox style.dox
man_MANS = caca-config.1 cacafire.1 cacaview.1

if BUILD_DOCUMENTATION
htmldoc_DATA = html/doxygen.css
htmldocdir = $(datadir)/doc/libcaca-dev/html
if USE_LATEX
pdfdoc_DATA = latex/libcaca.pdf
pdfdocdir = $(datadir)/doc/libcaca-dev/pdf
endif
endif

html/doxygen.css html/doxygen.png: stamp-doxygen
latex/libcaca.pdf: stamp-latex

stamp-doxygen: $(doxygen_DOX)
if BUILD_DOCUMENTATION
	doxygen doxygen.cfg
	touch stamp-doxygen
endif

stamp-latex: stamp-doxygen
if BUILD_DOCUMENTATION
if USE_LATEX
	rm -f latex/libcaca.tex latex/libcaca.pdf
	mv latex/refman.tex latex/libcaca.tex
	sed 's/setlength{/renewcommand{/' latex/libcaca.tex > latex/refman.tex
	cd latex && $(MAKE) $(AM_CFLAGS) refman.pdf
	mv latex/refman.pdf latex/libcaca.pdf
	touch stamp-latex
endif
endif

clean: clean-local
clean-local:
	-rm -Rf html latex man
	-rm -f stamp-latex stamp-doxygen

install-data-local:
if BUILD_DOCUMENTATION
	$(mkinstalldirs) $(DESTDIR)$(datadir)/doc/libcaca-dev/html
	cp `find html -name '*.html' -o -name '*.gif' -o -name '*.png'` \
	  $(DESTDIR)$(datadir)/doc/libcaca-dev/html
	$(mkinstalldirs) $(DESTDIR)$(mandir)/man3
	cp `find man -name '*.3caca'` $(DESTDIR)$(mandir)/man3
	$(mkinstalldirs) $(DESTDIR)$(mandir)/man1
	rm -f $(DESTDIR)$(mandir)/man1/cacaball.1
	ln -s cacafire.1 $(DESTDIR)$(mandir)/man1/cacaball.1
	rm -f $(DESTDIR)$(mandir)/man1/cacamoir.1
	ln -s cacafire.1 $(DESTDIR)$(mandir)/man1/cacamoir.1
	rm -f $(DESTDIR)$(mandir)/man1/cacaplas.1
	ln -s cacafire.1 $(DESTDIR)$(mandir)/man1/cacaplas.1
endif

