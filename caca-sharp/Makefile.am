
# We need to install the .config file ourselves because of a gacutil
# bug (https://bugzilla.novell.com/show_bug.cgi?id=363341)
monodir = $(libdir)/mono/caca-sharp-0.0
if USE_CSHARP
noinst_DATA = caca-sharp.dll caca-sharp.dll.config test.exe
endif

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

EXTRA_DIST = $(caca_sources) $(test_sources) \
	caca-sharp.snk caca-sharp.pc.in caca-sharp.dll.config.in \
	caca-sharp.csproj test-csharp.csproj

caca-sharp.dll: $(caca_sources)
	$(CSC) -debug -unsafe $(caca_sources) -out:$@ -target:library \
	  -keyfile:$(srcdir)/caca-sharp.snk -r:System.Drawing.dll

test.exe: $(test_sources) caca-sharp.dll
	$(CSC) -debug $(test_sources) -out:$@ -lib:./ \
	  -r:System.Drawing.dll -r:./caca-sharp.dll

if USE_CSHARP
install-data-local:
	MONO_SHARED_DIR=$$HOME $(GACUTIL) -i caca-sharp.dll \
	  -package caca-sharp-0.0 -f -root $(DESTDIR)$(libdir)
	$(mkinstalldirs) $(DESTDIR)$(monodir)
	sed -e 's@target="[^"]*/@target="@' < caca-sharp.dll.config > $(DESTDIR)$(monodir)/caca-sharp.dll.config

uninstall-local:
	MONO_SHARED_DIR=$$HOME $(GACUTIL) -u caca-sharp.dll, Version=0.0 \
	  -package caca-sharp-0.0 || true
	rm -f $(DESTDIR)$(monodir)/caca-sharp.dll.config
	rmdir $(DESTDIR)$(monodir) 2>/dev/null || true
endif

clean-local:
	rm -f *.exe *.dll *.mdb

