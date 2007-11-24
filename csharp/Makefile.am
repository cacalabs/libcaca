# $Id: $

if USE_CSHARP
noinst_DATA = cucul-sharp.dll caca-sharp.dll test.exe
endif

EXTRA_DIST = AssemblyInfo.cs \
             Cucul.cs \
             Caca.cs \
             cucul-sharp.dll.config \
             caca-sharp.dll.config \
             test.cs

cucul-sharp.dll: AssemblyInfo.cs Cucul.cs
	gmcs $^ -out:$@ -target:library  -unsafe

caca-sharp.dll: AssemblyInfo.cs Caca.cs
	gmcs $^ -out:$@ -target:library -r:./cucul-sharp.dll -lib:./ -unsafe

test.exe: test.cs caca-sharp.dll cucul-sharp.dll
	gmcs test.cs -out:$@ -r:./cucul-sharp.dll -r:./caca-sharp.dll -lib:./ -unsafe

clean-local:
	rm -f *.exe *.dll

