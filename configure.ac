# $Id$

AC_INIT(src/caca.c)

AC_PREREQ(2.50)
AC_CONFIG_AUX_DIR(autotools)
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE(libcaca, 0.5)
AM_CONFIG_HEADER(config.h)

AM_PROG_CC_C_O
AC_PROG_CPP
AC_PROG_RANLIB

dnl AC_PROG_EGREP only exists in autoconf 2.54+, so we use AC_EGREP_CPP right
dnl now otherwise it might be set in an obscure if statement.
AC_EGREP_CPP(foo,foo)

AC_ARG_ENABLE(slang,
  [  --enable-slang          slang graphics support (autodetected)])
AC_ARG_ENABLE(ncurses,
  [  --enable-ncurses        ncurses graphics support (autodetected)])
AC_ARG_ENABLE(conio,
  [  --enable-conio          DOS conio.h graphics support (default disabled)])
AC_ARG_ENABLE(x11,
  [  --enable-x11            X11 support (autodetected)])

AC_CHECK_HEADERS(inttypes.h endian.h)
AC_CHECK_FUNCS(vsnprintf getenv putenv strcasecmp)

CACA_DRIVERS="" 

if test "${enable_conio}" != "no"; then
  AC_CHECK_HEADERS(conio.h,
   [ac_cv_my_have_conio="yes"
    AC_MSG_CHECKING(for ScreenUpdate in pc.h)
    AC_EGREP_HEADER(ScreenUpdate,pc.h,
     [AC_MSG_RESULT(yes)
      AC_DEFINE(SCREENUPDATE_IN_PC_H, 1,
                Define if <pc.h> defines ScreenUpdate.)],
     [AC_MSG_RESULT(no)])
    AC_DEFINE(USE_CONIO, 1, Define to activate the conio.h backend driver)
    CACA_DRIVERS="${CACA_DRIVERS} conio"],
   [ac_cv_my_have_conio="no"])
  if test "${ac_cv_my_have_conio}" = "no" -a "${enable_conio}" = "yes"; then
    AC_MSG_ERROR([cannot find conio.h])
  fi
fi

if test "${enable_slang}" != "no"; then
  AC_CHECK_HEADERS(slang.h,
   [AC_CHECK_LIB(slang, SLkp_init,
     [ac_cv_my_have_slang="yes"
      AC_DEFINE(USE_SLANG, 1, Define to activate the slang backend driver)
      CPPFLAGS="${CPPFLAGS} -DOPTIMISE_SLANG_PALETTE=1"
      CACA_LIBS="${CACA_LIBS} -lslang"
      CACA_DRIVERS="${CACA_DRIVERS} slang"],
     [ac_cv_my_have_slang="no"])],
   [ac_cv_my_have_slang="no"])
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
  if test "${ac_cv_my_have_x11}" != "yes" -a "${enable_x11}" = "yes"; then
    AC_MSG_ERROR([cannot find X11 development files])
  fi
fi

if test "${enable_ncurses}" != "no"; then
  AC_CHECK_HEADERS(ncurses.h,
   [AC_CHECK_LIB(ncurses, initscr,
     [ac_cv_my_have_ncurses="yes"
      AC_DEFINE(USE_NCURSES, 1, Define to activate the ncurses backend driver)
      CACA_LIBS="${CACA_LIBS} -lncurses"
      CACA_DRIVERS="${CACA_DRIVERS} ncurses"],
     [ac_cv_my_have_ncurses="no"])],
   [ac_cv_my_have_ncurses="no"])
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

# Build cacaview?
save_CPPFLAGS="${CPPFLAGS}"
AC_PATH_PROG(IMLIB2_CONFIG, imlib2-config, no)
if test "${IMLIB2_CONFIG}" != "no"; then
  CPPFLAGS="${CPPFLAGS} `imlib2-config --cflags` -DX_DISPLAY_MISSING=1"
fi
AC_CHECK_HEADERS(Imlib2.h, USE_IMLIB2=:, USE_IMLIB2=false)
CPPFLAGS="${save_CPPFLAGS}"
AM_CONDITIONAL(USE_IMLIB2, ${USE_IMLIB2})

# Build documentation?
AC_PATH_PROG(DOXYGEN, doxygen, no)
AM_CONDITIONAL(DOXYGEN, test "${DOXYGEN}" != "no")

# Build LaTeX documentation?
AC_PATH_PROG(LATEX, latex, no)
AC_MSG_CHECKING(for a4wide.sty)
if test -f /usr/share/texmf/tex/latex/misc/a4wide.sty; then
  AC_MSG_RESULT(yes)
else
  LATEX=no
  AC_MSG_RESULT(no)
fi
AM_CONDITIONAL(LATEX, test "${LATEX}" != "no")

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  examples/Makefile
  doc/Makefile
  doc/doxygen.cfg
  autotools/Makefile
  debian/Makefile
])
AC_CONFIG_FILES([caca-config], [chmod 0755 caca-config])
AC_OUTPUT

