# $Id: $

cuculdir = $(libdir)/cucul-sharp
cacadir = $(libdir)/caca-sharp

cucul_sources = AssemblyInfo.cs Cucul.cs
caca_sources = AssemblyInfo.cs Caca.cs

if USE_CSHARP
cucul_DATA = cucul-sharp.dll cucul-sharp.dll.config
caca_DATA = caca-sharp.dll caca-sharp.dll.config
noinst_DATA = test.exe
endif

EXTRA_DIST = $(cucul_sources) cucul-sharp.dll.config \
             $(caca_sources) caca-sharp.dll.config \
             test.cs

cucul-sharp.dll: $(cucul_sources)
	gmcs $(cucul_sources) -out:$@ -target:library  -unsafe

caca-sharp.dll: $(caca_sources) cucul-sharp.dll
	gmcs $(caca_sources) -out:$@ -target:library \
	     -r:./cucul-sharp.dll -lib:./ -unsafe

test.exe: test.cs caca-sharp.dll cucul-sharp.dll
	gmcs test.cs -out:$@ \
	     -r:./cucul-sharp.dll -r:./caca-sharp.dll -lib:./ -unsafe

clean-local:
	rm -f *.exe *.dll

