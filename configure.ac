# $Id$

AC_INIT(src/caca.c)

AC_PREREQ(2.50)
AC_CONFIG_AUX_DIR(autotools)
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE(libcaca, 0.7)
AM_CONFIG_HEADER(config.h)

AM_PROG_CC_C_O
AC_PROG_CPP
AC_PROG_RANLIB

AC_C_CONST
AC_C_INLINE

dnl AC_PROG_EGREP only exists in autoconf 2.54+, so we use AC_EGREP_CPP right
dnl now otherwise it might be set in an obscure if statement.
AC_EGREP_CPP(foo, foo)

AC_ARG_ENABLE(slang,
  [  --enable-slang          slang graphics support (autodetected)])
AC_ARG_ENABLE(ncurses,
  [  --enable-ncurses        ncurses graphics support (autodetected)])
AC_ARG_ENABLE(win32,
  [  --enable-win32          Windows console support (autodetected)])
AC_ARG_ENABLE(conio,
  [  --enable-conio          DOS conio.h graphics support (autodetected)])
AC_ARG_ENABLE(x11,
  [  --enable-x11            X11 support (autodetected)])

AC_CHECK_HEADERS(sys/time.h inttypes.h endian.h unistd.h)
AC_CHECK_FUNCS(vsnprintf _vsnprintf getenv putenv strcasecmp usleep gettimeofday)
AC_MSG_CHECKING(for Sleep)
AC_TRY_COMPILE([#include <windows.h>],[Sleep(42);],
 [AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_SLEEP, 1, [Define if you have Windows' Sleep])],
 [AC_MSG_RESULT(no)])
AC_CHECK_LIB(m, sin, MATH_LIBS="${MATH_LIBS} -lm")

CACA_DRIVERS="" 

if test "${enable_conio}" != "no"; then
  ac_cv_my_have_conio="no"
  AC_CHECK_HEADERS(conio.h,
   [AC_MSG_CHECKING(for ScreenUpdate in pc.h)
    AC_EGREP_HEADER(ScreenUpdate, pc.h,
     [ac_cv_my_have_conio="yes"
      AC_MSG_RESULT(yes)
      AC_DEFINE(SCREENUPDATE_IN_PC_H, 1,
                Define if <pc.h> defines ScreenUpdate.)
      AC_DEFINE(USE_CONIO, 1, Define to activate the conio.h backend driver)
      CACA_DRIVERS="${CACA_DRIVERS} conio"],
     [AC_MSG_RESULT(no)])])
  if test "${ac_cv_my_have_conio}" = "no" -a "${enable_conio}" = "yes"; then
    AC_MSG_ERROR([cannot find conio.h])
  fi
fi

if test "${enable_win32}" != "no"; then
  ac_cv_my_have_win32="no"
  AC_CHECK_HEADERS(windows.h,
   [AC_MSG_CHECKING(for AllocConsole in windows.h)
    AC_EGREP_HEADER(AllocConsole, windows.h,
     [ac_cv_my_have_win32="yes"
      AC_MSG_RESULT(yes)
      AC_DEFINE(ALLOCCONSOLE_IN_WINDOWS_H, 1,
                Define if <windows.h> defines AllocConsole.)
      AC_DEFINE(USE_WIN32, 1, Define to activate the win32 backend driver)
      CACA_DRIVERS="${CACA_DRIVERS} win32"],
     [AC_MSG_RESULT(no)])])
  if test "${ac_cv_my_have_win32}" = "no" -a "${enable_win32}" = "yes"; then
    AC_MSG_ERROR([cannot find win32 console development files])
  fi
fi

if test "${enable_slang}" != "no"; then
  ac_cv_my_have_slang="no"
  AC_CHECK_HEADERS(slang.h slang/slang.h,
   [AC_CHECK_LIB(slang, SLkp_init,
     [ac_cv_my_have_slang="yes"
      AC_DEFINE(USE_SLANG, 1, Define to activate the slang backend driver)
      CPPFLAGS="${CPPFLAGS} -DOPTIMISE_SLANG_PALETTE=1"
      CACA_LIBS="${CACA_LIBS} -lslang"
      CACA_DRIVERS="${CACA_DRIVERS} slang"])])
  if test "${ac_cv_my_have_slang}" = "no" -a "${enable_slang}" = "yes"; then
    AC_MSG_ERROR([cannot find slang development files])
  fi
fi

if test "${enable_x11}" != "no"; then
  AC_PATH_X
  AC_CHECK_LIB(X11, XOpenDisplay,
   [ac_cv_my_have_x11="yes"
    if test -n "${x_includes}"; then X_CFLAGS="-I${x_includes}"; fi
    if test -n "${x_libraries}"; then X_LIBS="-lX11 -L${x_libraries}"; fi
    AC_DEFINE(USE_X11, 1, Define to activate the X11 backend driver)
    CPPFLAGS="${CPPFLAGS} ${X_CFLAGS}"
    CACA_LIBS="${CACA_LIBS} ${X_LIBS}"
    CACA_DRIVERS="${CACA_DRIVERS} x11"],
   [ac_cv_my_have_x11="no"],
   [[-lXt -L${x_libraries}]])
  AC_CHECK_HEADERS(X11/XKBlib.h)
  if test "${ac_cv_my_have_x11}" != "yes" -a "${enable_x11}" = "yes"; then
    AC_MSG_ERROR([cannot find X11 development files])
  fi
fi

if test "${enable_ncurses}" != "no"; then
  ac_cv_my_have_ncurses="no"
  AC_CHECK_HEADERS(ncurses.h,
   [AC_CHECK_LIB(ncurses, initscr,
     [ac_cv_my_have_ncurses="yes"
      AC_DEFINE(USE_NCURSES, 1, Define to activate the ncurses backend driver)
      CACA_LIBS="${CACA_LIBS} -lncurses"
      CACA_DRIVERS="${CACA_DRIVERS} ncurses"])])
  if test "${ac_cv_my_have_ncurses}" = "no" -a "${enable_ncurses}" = "yes"; then
    AC_MSG_ERROR([cannot find ncurses development files])
  fi
fi

AC_MSG_CHECKING(valid output drivers)
if test -z "${CACA_DRIVERS}"; then
  AC_MSG_RESULT(no)
  AC_MSG_ERROR([no output drivers were selected!])
else
  AC_MSG_RESULT([${CACA_DRIVERS}])
fi

AC_SUBST(MATH_LIBS)
AC_SUBST(CACA_LIBS)

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

# Use Imlib2?
ac_cv_my_have_imlib2="no"
AC_ARG_ENABLE(imlib2,
  [  --enable-imlib2         Imlib2 graphics support (default enabled)])
save_CPPFLAGS="${CPPFLAGS}"
if test "${enable_imlib2}" != "no"; then
  AC_PATH_PROG(IMLIB2_CONFIG, imlib2-config, no)
  if test "${IMLIB2_CONFIG}" != "no"; then
    CPPFLAGS="${CPPFLAGS} `imlib2-config --cflags` -DX_DISPLAY_MISSING=1"
  fi
  AC_CHECK_HEADERS(Imlib2.h,
    ac_cv_my_have_imlib2="yes",
   [ac_cv_my_have_imlib2="no"
    AC_MSG_ERROR([[cannot find Imlib2 development files. Without Imlib2, cacaview will only open BMP files; if this is really what you want, re-run configure with '--disable-imlib2'.]])])
  CPPFLAGS="${save_CPPFLAGS}"
fi
AM_CONDITIONAL(USE_IMLIB2, test "${ac_cv_my_have_imlib2}" = "yes")

# Build documentation?
AC_PATH_PROG(DOXYGEN, doxygen, no)
AM_CONDITIONAL(DOXYGEN, test "${DOXYGEN}" != "no")

# Build LaTeX documentation?
AC_PATH_PROG(LATEX, latex, no)
AC_PATH_PROG(DVIPS, dvips, no)
if test "${DVIPS}" = "no"; then
  LATEX="no"
fi
AC_MSG_CHECKING(for a4wide.sty)
if test -f /usr/share/texmf/tex/latex/misc/a4wide.sty; then
  AC_MSG_RESULT(yes)
else
  LATEX="no"
  AC_MSG_RESULT(no)
fi
AM_CONDITIONAL(LATEX, test "${LATEX}" != "no")

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  examples/Makefile
  test/Makefile
  doc/Makefile
  doc/doxygen.cfg
  autotools/Makefile
  debian/Makefile
  msvc/Makefile
])
AC_CONFIG_FILES([caca-config], [chmod 0755 caca-config])
AC_OUTPUT

