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

EXTRA_DIST = $(cucul_sources) cucul-sharp.dll.config.in \
             $(caca_sources) caca-sharp.dll.config.in \
             test.cs

cucul-sharp.dll: $(cucul_sources)
	gmcs -unsafe $(cucul_sources) -out:$@ -target:library \
	  -r:System.Drawing.dll

caca-sharp.dll: $(caca_sources) cucul-sharp.dll
	gmcs -unsafe $(caca_sources) -out:$@ -target:library -lib:./ \
	  -r:System.Drawing.dll -r:./cucul-sharp.dll

test.exe: test.cs caca-sharp.dll cucul-sharp.dll
	gmcs test.cs -out:$@ -lib:./ \
	  -r:System.Drawing.dll -r:./cucul-sharp.dll -r:./caca-sharp.dll

clean-local:
	rm -f *.exe *.dll

