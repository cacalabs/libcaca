EXTRA_DIST = doxygen.cfg.in footer.html header.html $(man_MANS)

man_MANS = caca-config.1 cacademo.1 caca-spritedit.1 cacaview.1

all: stamp-doxygen stamp-latex

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

