# $Id: $

cacadir = $(libdir)/caca-sharp

caca_sources = $(srcdir)/AssemblyInfo.cs $(srcdir)/Caca.cs

if USE_CSHARP
caca_DATA = caca-sharp.dll caca-sharp.dll.config
noinst_DATA = test.exe
endif

EXTRA_DIST = $(caca_sources) caca-sharp.dll.config.in \
             $(srcdir)/test.cs

caca-sharp.dll: $(caca_sources)
	gmcs -unsafe $(caca_sources) -out:$@ -target:library \
	  -r:System.Drawing.dll

test.exe: $(srcdir)/test.cs caca-sharp.dll
	gmcs $(srcdir)/test.cs -out:$@ -lib:./ \
	  -r:System.Drawing.dll -r:./caca-sharp.dll

clean-local:
	rm -f *.exe *.dll

