if USE_SLANG
CPPFLAGS_slang = -DUSE_SLANG
endif
if USE_NCURSES
CPPFLAGS_ncurses = -DUSE_NCURSES
endif

lib_LIBRARIES = libee.a
libee_a_SOURCES = ee.c ee.h
libee_a_CPPFLAGS = $(CPPFLAGS_slang) $(CPPFLAGS_ncurses)

