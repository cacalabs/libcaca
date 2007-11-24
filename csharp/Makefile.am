# $Id: $

if USE_CSHARP
noinst_DATA = libCucul.dll libCaca.dll test.exe
endif

EXTRA_DIST = AssemblyInfo.cs \
             Cucul.cs \
             Caca.cs \
             libCucul.dll.config \
             libCaca.dll.config \
             test.cs

libCucul.dll: AssemblyInfo.cs Cucul.cs
	gmcs $^ -out:$@ -target:library  -unsafe

libCaca.dll: AssemblyInfo.cs Caca.cs
	gmcs $^ -out:$@ -target:library -r:./libCucul.dll -lib:./ -unsafe

test.exe: test.cs libCaca.dll libCucul.dll
	gmcs test.cs -out:$@ -r:./libCucul.dll -r:./libCaca.dll -lib:./ -unsafe

clean-local:
	rm -f *.exe *.dll

