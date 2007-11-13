all:	cucul/cucul.so caca/caca.so

caca/caca.so:	cucul/cucul.so
	cd caca && \
	ruby extconf.rb && \
	make

cucul/cucul.so:
	cd cucul && \
	ruby extconf.rb && \
	make

clean:
	make -C caca clean
	make -C cucul clean
