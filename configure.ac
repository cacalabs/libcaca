# $Id$

AC_INIT(src/caca.c)

AC_PREREQ(2.50)
AC_CONFIG_AUX_DIR(autotools)
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE(libcaca, 0.1)
AM_CONFIG_HEADER(config.h)

AM_PROG_CC_C_O
AC_PROG_CPP
AC_PROG_RANLIB

dnl AC_PROG_EGREP only exists in autoconf 2.54+, so we use AC_EGREP_CPP right
dnl now otherwise it might be set in an obscure if statement.
AC_EGREP_CPP(foo,foo)

AC_ARG_ENABLE(slang,
  [  --enable-slang          slang graphics support (default enabled)])
AC_ARG_ENABLE(ncurses,
  [  --enable-ncurses        ncurses graphics support (default disabled)])
AC_ARG_ENABLE(conio,
  [  --enable-conio          DOS conio.h graphics support (default disabled)])

USE_SLANG=false
USE_NCURSES=false
USE_CONIO=false
if test "${enable_conio}" = "yes"; then
  AC_CHECK_HEADER(conio.h,:,AC_MSG_ERROR([cannot find conio.h header]))
  AC_MSG_CHECKING(for ScreenUpdate in pc.h)
  AC_EGREP_HEADER(ScreenUpdate,pc.h,[
    AC_MSG_RESULT(yes)
    AC_DEFINE(SCREENUPDATE_IN_PC_H, 1,
              Define if <pc.h> defines ScreenUpdate.)],[
    AC_MSG_RESULT(no)])
  AC_DEFINE(USE_CONIO, 1, Define if the backend driver is conio.h)
  USE_CONIO=:
elif test "${enable_ncurses}" = "yes"; then
  AC_CHECK_HEADER(ncurses.h,:,AC_MSG_ERROR([cannot find ncurses headers]))
  AC_CHECK_LIB(ncurses,initscr,:,AC_MSG_ERROR([cannot find ncurses library]))
  AC_DEFINE(USE_NCURSES, 1, Define if the backend driver is ncurses)
  USE_NCURSES=:
elif test "${enable_slang}" != "no"; then
  AC_CHECK_HEADER(slang.h,:,AC_MSG_ERROR([cannot find slang headers]))
  AC_CHECK_LIB(slang,SLkp_init,:,AC_MSG_ERROR([cannot find slang library]))
  AC_DEFINE(USE_SLANG, 1, Define if the backend driver is slang)
  USE_SLANG=:
else
  AC_MSG_ERROR([could not find any terminal graphics interface])
fi

AM_CONDITIONAL(USE_SLANG, ${USE_SLANG})
AM_CONDITIONAL(USE_NCURSES, ${USE_NCURSES})

# Optimizations
CFLAGS="${CFLAGS} -g -O2 -fno-strength-reduce -fomit-frame-pointer"
# Code qui fait des warnings == code de porc == deux baffes dans ta gueule
CFLAGS="${CFLAGS} -Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wstrict-prototypes -Wshadow -Waggregate-return -Wmissing-prototypes -Wnested-externs -Wsign-compare"

# Build the PIC library?
case "${target_os}" in
  *mingw32* | *cygwin* | *djgpp*)
    NEED_PIC=false
    ;;
  *)
    NEED_PIC=:
    ;;
esac
AM_CONDITIONAL(NEED_PIC, ${NEED_PIC})

# Build documentation?
AC_PATH_PROG(DOXYGEN, doxygen, no)
AM_CONDITIONAL(DOXYGEN, test "${DOXYGEN}" != "no")
AC_PATH_PROG(LATEX, latex, no)
AC_MSG_CHECKING(for a4wide.sty)
if test -f /usr/share/texmf/tex/latex/misc/a4wide.sty; then
  AC_MSG_RESULT(yes)
else
  LATEX=no
  AC_MSG_RESULT(no)
fi
AM_CONDITIONAL(LATEX, test "${LATEX}" != "no")

AC_OUTPUT([
  Makefile
  src/Makefile
  examples/Makefile
  doc/Makefile
  autotools/Makefile
  debian/Makefile
  caca-config
],[
  chmod 0755 caca-config
])

