dnl Autoconf settings for ttyvaders

AC_INIT(ttyvaders,0.0cvs-20021218)

AC_PREREQ(2.50)
AC_CONFIG_SRCDIR(src/main.c)
AC_CONFIG_AUX_DIR(autotools)
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE(ttyvaders,0.0cvs-20021218)
AM_CONFIG_HEADER(config.h)

AM_PROG_CC_C_O
AC_PROG_CPP

AC_ARG_ENABLE(ncurses,
  [  --enable-ncurses        ncurses interface support (default is slang)])

if test "${enable_ncurses}" = "yes"
then
  AC_CHECK_HEADER(ncurses.h,:,AC_MSG_ERROR([cannot find ncurses headers]))
  AC_CHECK_LIB(ncurses,initscr,:,AC_MSG_ERROR([cannot find ncurses library]))
else
  AC_CHECK_HEADER(slang.h,:,AC_MSG_ERROR([cannot find slang headers]))
  AC_CHECK_LIB(slang,SLkp_init,:,AC_MSG_ERROR([cannot find slang library]))
fi

AM_CONDITIONAL(USE_NCURSES, test "${enable_ncurses}" = "yes")

AC_OUTPUT([
  Makefile
  src/Makefile
  autotools/Makefile
  debian/Makefile
])

