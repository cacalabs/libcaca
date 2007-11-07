# $Id$

EXTRA_DIST = doxygen.cfg.in footer.html header.html $(man_MANS) $(doxygen_DOX)

doxygen_DOX = libcaca.dox user.dox migrating.dox tutorial.dox canvas.dox font.dox style.dox
man_MANS = caca-config.1 cacafire.1 cacaview.1 cacaserver.1 img2txt.1 cacaplay.1

if BUILD_DOCUMENTATION
htmldoc_DATA = html/doxygen.css
htmldocdir = $(datadir)/doc/libcucul-dev/html
if USE_LATEX
pdfdoc_DATA = latex/libcaca.pdf
pdfdocdir = $(datadir)/doc/libcucul-dev/pdf
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
	cd latex && $(MAKE) $(AM_CFLAGS) refman.pdf || (cat refman.log; exit 1)
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
	$(mkinstalldirs) $(DESTDIR)$(datadir)/doc/libcucul-dev/html
	cp `find html -name '*.html' -o -name '*.gif' -o -name '*.png'` \
	  $(DESTDIR)$(datadir)/doc/libcucul-dev/html
	$(mkinstalldirs) $(DESTDIR)$(mandir)/man3
	for man in $$(find man -name '*.3caca'); do \
	  sed -e 's/man3caca/man3/g' -e "s/fC'\([a-zA-Z0-9]*\)'/fC\"\1\"/g" \
	    $$man >$(DESTDIR)$(mandir)/man3/$${man##*/}; \
	done
	$(mkinstalldirs) $(DESTDIR)$(mandir)/man1
	rm -f $(DESTDIR)$(mandir)/man1/cacademo.1
	ln -s cacafire.1 $(DESTDIR)$(mandir)/man1/cacademo.1
endif

