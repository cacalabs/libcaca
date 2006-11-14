# $Id$

EXTRA_DIST = caca.pc.in
DISTCLEANFILES = caca.pc

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/cucul -DPLUGINDIR=\"$(plugindir)\"

pkgconfig_DATA = caca.pc
pkgconfigdir = $(libdir)/pkgconfig

include_HEADERS = caca.h caca0.h

lib_LTLIBRARIES = libcaca.la

libcaca_la_SOURCES = \
	caca.c \
	caca.h \
	caca_internals.h \
	caca0.c \
	caca0.h \
	graphics.c \
	event.c \
	time.c \
	driver_conio.c \
	driver_ncurses.c \
	driver_raw.c \
	driver_slang.c \
	driver_vga.c \
	driver_win32.c \
	$(cocoa_source) \
	$(extra_source) \
	$(NULL)
libcaca_la_LDFLAGS = -no-undefined -version-info @LT_VERSION@
libcaca_la_LIBADD = ../cucul/libcucul.la @CACA_LIBS@

if USE_PLUGINS
plugin_LTLIBRARIES = libx11_plugin.la libgl_plugin.la
plugindir = $(libdir)/caca

libx11_plugin_la_SOURCES = driver_x11.c
libx11_plugin_la_LDFLAGS = -no-undefined -module -no-version
libx11_plugin_la_LIBADD = libcaca.la ../cucul/libcucul.la $(X11_LIBS)

libgl_plugin_la_SOURCES = driver_gl.c
libgl_plugin_la_LDFLAGS = -no-undefined -module -no-version
libgl_plugin_la_LIBADD = libcaca.la ../cucul/libcucul.la $(GL_LIBS)
else
extra_source = driver_x11.c driver_gl.c
endif

if USE_COCOA
cocoa_source = driver_cocoa.m
endif

