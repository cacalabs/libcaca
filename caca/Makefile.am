# $Id$

EXTRA_DIST = caca_types.h.in caca.pc.in mono9.data monobold12.data
DISTCLEANFILES = caca_types.h caca.pc

AM_CPPFLAGS = -I$(top_srcdir) -DPLUGINDIR=\"$(plugindir)\"

pkgconfig_DATA = caca.pc
pkgconfigdir = $(libdir)/pkgconfig

include_HEADERS = caca.h caca_types.h caca0.h

lib_LTLIBRARIES = libcaca.la

libcaca_la_SOURCES = \
	caca.c \
	caca.h \
	caca_types.h \
	caca_internals.h \
	caca0.c \
	caca0.h \
	canvas.c \
	string.c \
        legacy.c \
        transform.c \
        charset.c \
        attr.c \
        line.c \
        box.c \
        conic.c \
        triangle.c \
        frame.c \
        dither.c \
        font.c \
        import.c \
        export.c \
        file.c \
        figfont.c \
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
libcaca_la_DEPENDENCIES = \
	mono9.data \
	monobold12.data \
	$(NULL)
libcaca_la_CPPFLAGS = $(AM_CPPFLAGS) @CACA_CFLAGS@ -D__LIBCACA__
libcaca_la_LDFLAGS = -no-undefined -version-number @LT_VERSION@
libcaca_la_LIBADD = @CACA_LIBS@ $(ZLIB_LIBS)

if USE_PLUGINS
plugin_LTLIBRARIES = libx11_plugin.la libgl_plugin.la
plugindir = $(libdir)/caca

libx11_plugin_la_SOURCES = driver_x11.c
libx11_plugin_la_CPPFLAGS = $(AM_CPPFLAGS) $(X11_CFLAGS)
libx11_plugin_la_LDFLAGS = -no-undefined -module -no-version
libx11_plugin_la_LIBADD = libcaca.la $(X11_LIBS)

libgl_plugin_la_SOURCES = driver_gl.c
libgl_plugin_la_CPPFLAGS = $(AM_CPPFLAGS) $(GL_CFLAGS) #$(FTGL_CFLAGS)
libgl_plugin_la_LDFLAGS = -no-undefined -module -no-version
libgl_plugin_la_LIBADD = libcaca.la $(GL_LIBS) #$(FTGL_LIBS)
else
extra_source = driver_x11.c driver_gl.c
endif

if USE_COCOA
cocoa_source = driver_cocoa.m
endif

