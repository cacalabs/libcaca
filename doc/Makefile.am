# $Id$

EXTRA_DIST = doxygen.cfg.in footer.html header.html $(man_MANS)

man_MANS = caca-config.1 cacademo.1 cacaview.1

if BUILD_DOCUMENTATION
htmldoc_DATA = html/doxygen.css
htmldocdir = $(datadir)/doc/libcaca-dev/html
if USE_LATEX
psdoc_DATA = latex/libcaca.ps
psdocdir = $(datadir)/doc/libcaca-dev/ps
endif
endif

html/doxygen.css html/doxygen.png: stamp-doxygen
latex/libcaca.ps: stamp-latex

stamp-doxygen:
if BUILD_DOCUMENTATION
	doxygen doxygen.cfg
	touch stamp-doxygen
endif

stamp-latex: stamp-doxygen
if BUILD_DOCUMENTATION
if USE_LATEX
	rm -f latex/libcaca.tex latex/libcaca.ps
	mv latex/refman.tex latex/libcaca.tex
	sed 's/setlength{/renewcommand{/' latex/libcaca.tex > latex/refman.tex
	cd latex && $(MAKE) $(AM_CFLAGS) ps
	mv latex/refman.ps latex/libcaca.ps
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
	cp man/man3/*.3caca $(DESTDIR)$(mandir)/man3
	$(mkinstalldirs) $(DESTDIR)$(mandir)/man1
	rm -f $(DESTDIR)$(mandir)/man1/cacaball.1
	ln -s cacademo.1 $(DESTDIR)$(mandir)/man1/cacaball.1
	rm -f $(DESTDIR)$(mandir)/man1/cacafire.1
	ln -s cacademo.1 $(DESTDIR)$(mandir)/man1/cacafire.1
	rm -f $(DESTDIR)$(mandir)/man1/cacamoir.1
	ln -s cacademo.1 $(DESTDIR)$(mandir)/man1/cacamoir.1
	rm -f $(DESTDIR)$(mandir)/man1/cacaplas.1
	ln -s cacademo.1 $(DESTDIR)$(mandir)/man1/cacaplas.1
endif

