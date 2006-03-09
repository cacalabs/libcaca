# $Id: Makefile.am 326 2006-03-06 20:57:14Z sam $

lib_LTLIBRARIES = $(libkernel_la)

if USE_KERNEL
libkernel_la = libkernel.la
endif

libkernel_la_SOURCES = \
	kernel.c \
	kernel.h \
	multiboot.S \
	multiboot.h \
	$(NULL)
libkernel_la_LDFLAGS = -no-undefined

