# $Id$

AC_INIT(ttyvaders,0.0cvs-20021223)

AC_PREREQ(2.50)
AC_CONFIG_SRCDIR(src/main.c)
AC_CONFIG_AUX_DIR(autotools)
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE(ttyvaders,0.0cvs-20021223)
AM_CONFIG_HEADER(config.h)

AM_PROG_CC_C_O
AC_PROG_CPP
AC_PROG_RANLIB

AC_ARG_ENABLE(slang,
  [  --enable-slang          slang graphics support (default enabled)])
AC_ARG_ENABLE(ncurses,
  [  --enable-ncurses        ncurses graphics support (default disabled)])

USE_SLANG=false
USE_NCURSES=false
if test "${enable_ncurses}" = "yes"; then
  AC_CHECK_HEADER(ncurses.h,:,AC_MSG_ERROR([cannot find ncurses headers]))
  AC_CHECK_LIB(ncurses,initscr,:,AC_MSG_ERROR([cannot find ncurses library]))
  AC_DEFINE(USE_NCURSES, 1, Define if the backend driver is ncurses)
  USE_NCURSES=:
elif test "${enable_slang}" != "no"; then
  AC_CHECK_HEADER(slang.h,:,AC_MSG_ERROR([cannot find slang headers]))
  AC_CHECK_LIB(slang,SLkp_init,:,AC_MSG_ERROR([cannot find slang library]))
  AC_DEFINE(USE_SLANG, 1, Define if the backend driver is slang)
  USE_SLANG=:
fi

AM_CONDITIONAL(USE_SLANG, ${USE_SLANG})
AM_CONDITIONAL(USE_NCURSES, ${USE_NCURSES})

AC_OUTPUT([
  Makefile
  libee/Makefile
  test/Makefile
  src/Makefile
  autotools/Makefile
  data/Makefile
  debian/Makefile
])

