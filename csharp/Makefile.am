# $Id: $

cacadir = $(libdir)/caca-sharp

caca_sources = \
	$(srcdir)/AssemblyInfo.cs \
	$(srcdir)/Caca.cs \
	$(srcdir)/Attr.cs \
	$(srcdir)/Canvas.cs \
	$(srcdir)/Display.cs \
	$(srcdir)/Dither.cs \
	$(srcdir)/Event.cs \
	$(srcdir)/Font.cs

test_sources = \
	$(srcdir)/test.cs

if USE_CSHARP
caca_DATA = caca-sharp.dll caca-sharp.dll.config
noinst_DATA = test.exe
endif

EXTRA_DIST = $(caca_sources) $(test_sources) caca-sharp.dll.config.in

caca-sharp.dll: $(caca_sources)
	gmcs -unsafe $(caca_sources) -out:$@ -target:library \
	  -r:System.Drawing.dll

test.exe: $(test_sources) caca-sharp.dll
	gmcs $(test_sources) -out:$@ -lib:./ \
	  -r:System.Drawing.dll -r:./caca-sharp.dll

clean-local:
	rm -f *.exe *.dll

