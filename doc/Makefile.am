EXTRA_DIST = doxygen.cfg.in footer.html header.html $(man_MANS)

man_MANS = caca-config.1 cacademo.1 caca-spritedit.1 cacaview.1

if DOXYGEN
htmldoc_DATA = html/doxygen.css html/doxygen.png
htmldocdir = $(datadir)/doc/libcaca-dev/html
if LATEX
psdoc_DATA = latex/refman.ps
psdocdir = $(datadir)/doc/libcaca-dev/ps
endif
endif

html/doxygen.css html/doxygen.png: stamp-doxygen
latex/refman.ps: stamp-latex

stamp-doxygen:
if DOXYGEN
	doxygen doxygen.cfg
	touch stamp-doxygen
endif

stamp-latex: stamp-doxygen
if DOXYGEN
if LATEX
	cd latex && $(MAKE) $(AM_CFLAGS) ps
	touch stamp-latex
endif
endif

clean: clean-local
clean-local:
	-rm -Rf html latex man
	-rm -f stamp-latex stamp-doxygen

install-data-local:
if DOXYGEN
	$(mkinstalldirs) $(DESTDIR)$(datadir)/doc/libcaca-dev/html
	cp html/*.html $(DESTDIR)$(datadir)/doc/libcaca-dev/html
	$(mkinstalldirs) $(DESTDIR)$(mandir)/man3
	cp man/man3/*.3caca $(DESTDIR)$(mandir)/man3
endif

