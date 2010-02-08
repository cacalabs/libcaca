
EXTRA_DIST = bootsect.S

lib_LTLIBRARIES = $(libkernel_la)

if USE_KERNEL
libkernel_la = libkernel.la
endif

libkernel_la_SOURCES = \
	kernel.c \
	kernel.h \
	multiboot.h \
	$(NULL)
libkernel_la_LDFLAGS = -no-undefined
AM_CPPFLAGS = -I$(top_srcdir)

clean: clean-local
clean-local:
	rm -f bootsect.bin kern.bin kernel.map

echo-sources: ; echo $(SOURCES)

