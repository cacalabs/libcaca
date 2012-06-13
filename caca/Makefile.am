
EXTRA_DIST = caca.pc.in \
             mono9.data monobold12.data \
             libcaca.vcxproj libcaca.def
DISTCLEANFILES = caca.pc

AM_CPPFLAGS = -I$(top_srcdir) -DPLUGINDIR=\"$(plugindir)\"

pkgconfig_DATA = caca.pc
pkgconfigdir = $(libdir)/pkgconfig

include_HEADERS = caca.h caca_types.h caca_conio.h caca0.h

lib_LTLIBRARIES = libcaca.la

libcaca_la_SOURCES = \
	caca.c \
	caca.h \
	caca_types.h \
	caca_internals.h \
	caca_debug.h \
	caca_prof.h \
	caca_stubs.h \
	caca_conio.c \
	caca_conio.h \
	caca0.c \
	caca0.h \
	canvas.c \
	dirty.c \
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
	file.c \
	figfont.c \
	graphics.c \
	event.c \
	time.c \
	prof.c \
	getopt.c \
	$(codec_source) \
	$(driver_source) \
	$(NULL)
libcaca_la_DEPENDENCIES = \
	mono9.data \
	monobold12.data \
	$(NULL)
libcaca_la_CPPFLAGS = $(AM_CPPFLAGS) @CACA_CFLAGS@ -D__LIBCACA__
libcaca_la_LDFLAGS = -no-undefined -version-number @LT_VERSION@
libcaca_la_LIBADD = @CACA_LIBS@ $(ZLIB_LIBS) $(GETOPT_LIBS)

codec_source = \
	codec/import.c \
	codec/export.c \
	codec/codec.h \
	codec/text.c \
	$(NULL)

driver_source = \
	driver/conio.c \
	driver/ncurses.c \
	driver/null.c \
	driver/raw.c \
	driver/slang.c \
	driver/vga.c \
	driver/win32.c \
	$(NULL)

uninstall-local:
	. ./libcaca.la || exit 1; \
	for x in $$library_names; do \
	  rm -f $(DESTDIR)$(libdir)/$$(echo $$x | sed 's/caca/cucul/g'); \
	done
	rm -f $(DESTDIR)$(libdir)/libcucul.la
	rmdir $(DESTDIR)$(libdir) 2>/dev/null || true
	rm -f $(DESTDIR)$(pkgconfigdir)/cucul.pc
	rmdir $(DESTDIR)$(pkgconfigdir) 2>/dev/null || true
	rm -f $(DESTDIR)$(includedir)/cucul.h

if USE_PLUGINS
plugin_LTLIBRARIES = libx11_plugin.la libgl_plugin.la
plugindir = $(libdir)/caca

libx11_plugin_la_SOURCES = driver/x11.c
libx11_plugin_la_CPPFLAGS = $(AM_CPPFLAGS) $(X11_CFLAGS)
libx11_plugin_la_LDFLAGS = -no-undefined -module -no-version
libx11_plugin_la_LIBADD = libcaca.la $(X11_LIBS)

libgl_plugin_la_SOURCES = driver/gl.c
libgl_plugin_la_CPPFLAGS = $(AM_CPPFLAGS) $(GL_CFLAGS) #$(FTGL_CFLAGS)
libgl_plugin_la_LDFLAGS = -no-undefined -module -no-version
libgl_plugin_la_LIBADD = libcaca.la $(GL_LIBS) #$(FTGL_LIBS)
else
driver_source += driver/x11.c driver/gl.c
endif

if USE_COCOA
driver_source += driver/cocoa.m
endif

echo-sources: ; echo $(SOURCES)

