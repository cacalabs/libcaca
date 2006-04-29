all:
	mcs AssemblyInfo.cs Cucul.cs -out:libCucul.dll -target:library  -unsafe
	mcs AssemblyInfo.cs Caca.cs -out:libCaca.dll -target:library  /r:./libCucul.dll /lib:./ -unsafe

	mcs test.cs -out:test.exe /r:./libCucul.dll /r:./libCaca.dll /lib:./ -unsafe