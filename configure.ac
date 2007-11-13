# $Id$

AC_INIT(caca/caca.c)

AC_PREREQ(2.50)
AC_CONFIG_AUX_DIR(autotools)
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE(libcaca, 0.99.beta12)
LT_VERSION="99:0:99"
AC_SUBST(LT_VERSION)
AM_CONFIG_HEADER(config.h)

AM_PROG_CC_C_O
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP
_AM_DEPENDENCIES([OBJC])
OBJC="${CC}"
AC_SUBST(OBJC)
AC_SUBST(OBJCFLAGS)
AM_PROG_AS

AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL
AC_LIBTOOL_CXX

AC_C_CONST
AC_C_INLINE
AC_TYPE_SIGNAL

dnl AC_PROG_EGREP only exists in autoconf 2.54+, so we use AC_EGREP_CPP right
dnl now otherwise it might be set in an obscure if statement. Same thing for
dnl PKG_PROG_PKG_CONFIG which needs to be called first.
AC_EGREP_CPP(yes, foo)
PKG_PROG_PKG_CONFIG()
m4_pattern_allow([^PKG_CONFIG_LIBDIR$])
if test "${build}" != "${host}" -a "${PKG_CONFIG_LIBDIR}" = ""; then
  export PKG_CONFIG_LIBDIR=/dev/null
fi

dnl output driver features
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
AC_ARG_ENABLE(gl,
  [  --enable-gl             OpenGL support (autodetected)])
AC_ARG_ENABLE(cocoa,
  [  --enable-cocoa          Cocoa support (autodetected)])
AC_ARG_ENABLE(network,
  [  --enable-network        Network support (autodetected)])
AC_ARG_ENABLE(vga,
  [  --enable-vga            VGA support (default disabled)])

dnl language bindings
AC_ARG_ENABLE(cxx,
  [  --enable-cxx            C++ bindings (default enabled)])
AC_ARG_ENABLE(ruby,
  [  --enable-ruby           Ruby bindings (autodetected)])

dnl example programs features
AC_ARG_ENABLE(imlib2,
  [  --enable-imlib2         Imlib2 graphics support (autodetected)])

dnl conditional builds
AC_ARG_ENABLE(debug,
  [  --enable-debug          build debug versions of the library])
AC_ARG_ENABLE(plugins,
  [  --enable-plugins        build X11 and GL drivers as plugins])
AC_ARG_ENABLE(doc,
  [  --enable-doc            build documentation (needs doxygen and LaTeX)])

AC_CHECK_HEADERS(stdio.h stdarg.h signal.h sys/ioctl.h sys/time.h inttypes.h endian.h unistd.h arpa/inet.h netinet/in.h winsock2.h errno.h locale.h getopt.h dlfcn.h termios.h)
AC_CHECK_FUNCS(signal ioctl vsnprintf getenv putenv strcasecmp htons)
AC_CHECK_FUNCS(usleep gettimeofday)

AC_CHECK_FUNCS(getopt_long,
 [AC_DEFINE(HAVE_GETOPT_LONG, 1, Define to 1 if you have the `getopt_long' function.)],
 [AC_CHECK_LIB(gnugetopt, getopt_long,
   [AC_DEFINE(HAVE_GETOPT_LONG, 1, Define to 1 if you have the `getopt_long' function.)
    GETOPT_LIBS="${GETOPT_LIBS} -lgnugetopt"])])
AC_SUBST(GETOPT_LIBS)

AC_MSG_CHECKING(for Sleep)
AC_TRY_COMPILE([#include <windows.h>],[Sleep(42);],
 [AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_SLEEP, 1, [Define to 1 if you have the `Sleep' function.])],
 [AC_MSG_RESULT(no)])

AC_MSG_CHECKING(for fsin/fcos)
AC_TRY_COMPILE([],[asm("fsin\n\tfcos");],
 [AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_FSIN_FCOS, 1, [Define to 1 if you have the `fsin' and `fcos' operands.])],
 [AC_MSG_RESULT(no)])

AC_MSG_CHECKING(for fldln2/fxch/fyl2x)
AC_TRY_COMPILE([],[asm("fldln2; fldln2; fxch; fyl2x");],
 [AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_FLDLN2, 1, [Define to 1 if you have the `fldln2' and other floating points operands.])],
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
                Define to 1 if <pc.h> defines ScreenUpdate.)
      AC_DEFINE(USE_CONIO, 1, Define to 1 to activate the conio.h backend driver)
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
                Define to 1 if <windows.h> defines AllocConsole.)
      AC_DEFINE(USE_WIN32, 1, Define to 1 to activate the win32 backend driver)
      CACA_DRIVERS="${CACA_DRIVERS} win32"],
     [AC_MSG_RESULT(no)])])
  if test "${ac_cv_my_have_win32}" = "no" -a "${enable_win32}" = "yes"; then
    AC_MSG_ERROR([cannot find win32 console development files])
  fi
fi

if test "${enable_slang}" != "no"; then
  ac_cv_my_have_slang="no"
  AC_CHECK_HEADERS(slang.h slang/slang.h,
   [ac_cv_my_have_slang="yes"
    AC_DEFINE(USE_SLANG, 1, Define to 1 to activate the slang backend driver)
    AC_CHECK_LIB(slang, SLsmg_utf8_enable,
     [AC_DEFINE(HAVE_SLSMG_UTF8_ENABLE, 1, Define to 1 if you have the `SLsmg_utf8_enable' function.)])
    CPPFLAGS="${CPPFLAGS} -DOPTIMISE_SLANG_PALETTE=1"
    CACA_LIBS="${CACA_LIBS} -lslang"
    CACA_DRIVERS="${CACA_DRIVERS} slang"
    break])
  if test "${ac_cv_my_have_slang}" = "no" -a "${enable_slang}" = "yes"; then
    AC_MSG_ERROR([cannot find slang development files])
  fi
fi

if test "${enable_x11}" != "no"; then
  AC_PATH_X
  AC_CHECK_LIB(X11, XOpenDisplay,
   [ac_cv_my_have_x11="yes"
    if test -n "${x_includes}"; then X_CFLAGS="-I${x_includes}"; fi
    if test -n "${x_libraries}"; then X_LIBS="-L${x_libraries}"; fi
    AC_DEFINE(USE_X11, 1, Define to 1 to activate the X11 backend driver)
    CPPFLAGS="${CPPFLAGS} ${X_CFLAGS}"
    X11_LIBS="${X11_LIBS} -lX11 ${X_LIBS}"
    CACA_DRIVERS="${CACA_DRIVERS} x11"],
   [ac_cv_my_have_x11="no"],
   [[`if test -n "${x_libraries}"; then echo -L${x_libraries}; fi`]])
  AC_CHECK_HEADERS(X11/XKBlib.h)
  if test "${ac_cv_my_have_x11}" != "yes" -a "${enable_x11}" = "yes"; then
    AC_MSG_ERROR([cannot find X11 development files])
  fi
fi

if test "${enable_gl}" != "no"; then
  ac_cv_my_have_gl="no"
  AC_CHECK_HEADERS(GL/gl.h OpenGL/gl.h,
   [AC_CHECK_HEADERS(GL/glut.h,
     [AC_CHECK_LIB(glut, glutCloseFunc,
       [AC_DEFINE(HAVE_GLUTCLOSEFUNC, 1, Define to 1 if you have the `glutCloseFunc' function.)])
      AC_CHECK_LIB(glut, glutMainLoopEvent,
       [ac_cv_my_have_gl="yes"])
      AC_CHECK_LIB(glut, glutCheckLoop,
       [ac_cv_my_have_gl="yes"
        AC_DEFINE(HAVE_GLUTCHECKLOOP, 1, Define to 1 if you have the `glutCheckLoop' function.)])])
      break])
  if test "${ac_cv_my_have_gl}" = "yes"; then
    AC_DEFINE(USE_GL, 1, Define to 1 to activate the OpenGL backend driver)
    GL_LIBS="${GL_LIBS} -lGL -lglut"
    CACA_DRIVERS="${CACA_DRIVERS} gl"
  elif test "${enable_gl}" = "yes"; then
    AC_MSG_ERROR([cannot find OpenGL+FreeGLUT development files])
  fi
fi

if test "${enable_cocoa}" != "no"; then
  ac_cv_my_have_cocoa="no"
  AC_CHECK_HEADERS(Cocoa/Cocoa.h,
   [ac_cv_my_have_cocoa="yes"])
  if test "${ac_cv_my_have_cocoa}" = "yes"; then
    case x${target} in
    xpowerpc*darwin*)
      # 10.3 needed to link with X11
      MACOSX_SDK=/Developer/SDKs/MacOSX10.3.9.sdk
      GCC_VERSION=3.3
      ARCH="-arch ppc"
      MACOSX_SDK_CFLAGS="-nostdinc -isystem ${MACOSX_SDK}/usr/include/gcc/darwin/${GCC_VERSION} -isystem ${MACOSX_SDK}/usr/include"
      MACOSX_SDK_CXXFLAGS="-nostdinc++ -I${MACOSX_SDK}/usr/include/gcc/darwin/${GCC_VERSION}/c++ -I${MACOSX_SDK}/usr/include/gcc/darwin/${GCC_VERSION}/c++/ppc-darwin -I${MACOSX_SDK}/usr/include/gcc/darwin/${GCC_VERSION}/c++/backward"
      MACOSX_SDK_LDFLAGS="-L${MACOSX_SDK}/usr/lib/gcc/darwin -L${MACOSX_SDK}/usr/lib/gcc/darwin/${GCC_VERSION} -L${MACOSX_SDK}/usr/lib"
      ;;
    x*86*darwin*)
      MACOSX_SDK=/Developer/SDKs/MacOSX10.4u.sdk
      GCC_VERSION=4.0
      ARCH="-arch i386"
      MACOSX_SDK_CFLAGS="-isysroot ${MACOSX_SDK}"
      MACOSX_SDK_CXXFLAGS="${MACOSX_SDK_CFLAGS}"
      ;;
    esac
    CC=gcc-${GCC_VERSION}
    CXX=g++-${GCC_VERSION}
    MACOSX_SDK_FRAMEWORKS="-F${MACOSX_SDK}/System/Library/Frameworks"
    CPPFLAGS="${CPPFLAGS} ${ARCH} ${MACOSX_SDK_FRAMEWORKS}"
    CFLAGS="${CFLAGS} ${MACOSX_SDK_CFLAGS}"
    CXXFLAGS="${CXXFLAGS} ${MACOSX_SDK_CXXFLAGS}"
    OBJCFLAGS="${OBJCFLAGS} ${MACOSX_SDK_CFLAGS}"
    LDFLAGS="${ARCH} ${MACOSX_SDK_LDFLAGS} ${LDFLAGS}"
    AC_DEFINE(USE_COCOA, 1, Define to 1 to activate the Cocoa backend driver)
    CACA_LIBS="${CACA_LIBS} -Wl,-syslibroot,${MACOSX_SDK},-framework,Cocoa"
    CACA_DRIVERS="${CACA_DRIVERS} cocoa"
  elif test "${enable_cocoa}" = "yes"; then
    AC_MSG_ERROR([cannot find Cocoa development files])
  fi
fi
AM_CONDITIONAL(USE_COCOA, test "${ac_cv_my_have_cocoa}" = "yes")

if test "${enable_ncurses}" != "no"; then
  ac_cv_my_have_ncurses="no"
  AC_CHECK_HEADERS(ncursesw/ncurses.h ncurses/ncurses.h ncurses.h curses.h,
   [ncurses="no"
    AC_CHECK_LIB(ncursesw, initscr,
     [ncurses="-lncursesw"],
     [AC_CHECK_LIB(ncurses, initscr,
       [ncurses="-lncurses"])])
    if test "${ncurses}" = "no"; then
      continue
    fi
    ac_cv_my_have_ncurses="yes"
    AC_DEFINE(USE_NCURSES, 1, Define to 1 to activate the ncurses backend driver)
    CACA_LIBS="${CACA_LIBS} ${ncurses}"
    CACA_DRIVERS="${CACA_DRIVERS} ncurses"
    dnl  Check for resizeterm or resize_term
    SAVED_LIBS="${LIBS}"
    LIBS="${LIBS} ${ncurses}"
    AC_CHECK_FUNCS(resizeterm resize_term)
    LIBS="${SAVED_LIBS}"
    break])
  if test "${ac_cv_my_have_ncurses}" = "no" -a "${enable_ncurses}" = "yes"; then
    AC_MSG_ERROR([cannot find ncurses development files])
  fi
fi

if test "${enable_vga}" = "yes"; then
  ac_cv_my_have_vga="yes"
  CPPFLAGS="${CPPFLAGS} -I. -D__KERNEL__ -nostdinc -include kernel/kernel.h"
  CFLAGS="${CFLAGS} -fno-builtin -O2 -Wall"
  CCASFLAGS="${CCASFLAGS} -I."
  LDFLAGS="${LDFLAGS} -nostdlib -Wl,-N -Wl,-Ttext -Wl,100000"
  AC_DEFINE(USE_VGA, 1, Define to 1 to activate the VGA backend driver)
  CACA_DRIVERS="${CACA_DRIVERS} vga"
fi
AM_CONDITIONAL(USE_KERNEL, test "${ac_cv_my_have_vga}" = "yes")

if test "${enable_debug}" = "yes"; then
  AC_DEFINE(DEBUG, 1, Define to 1 to activate debug)
fi

if test "${enable_plugins}" = "yes"; then
  ac_cv_my_have_plugins="yes"
  AC_DEFINE(USE_PLUGINS, 1, Define to 1 to activate plugins)
  CACA_LIBS="${CACA_LIBS} -ldl"
else
  CACA_LIBS="${CACA_LIBS} ${X11_LIBS} ${GL_LIBS}"
fi
AM_CONDITIONAL(USE_PLUGINS, test "${ac_cv_my_have_plugins}" = "yes")

AC_MSG_CHECKING(valid output drivers)
if test -z "${CACA_DRIVERS}"; then
  AC_MSG_RESULT(no)
  AC_MSG_ERROR([no output drivers were selected!])
else
  CACA_DRIVERS="${CACA_DRIVERS# *}"
  AC_MSG_RESULT([${CACA_DRIVERS}])
fi


AC_SUBST(MATH_LIBS)
AC_SUBST(GETOPT_LIBS)
AC_SUBST(CACA_LIBS)
AC_SUBST(X11_LIBS)
AC_SUBST(GL_LIBS)

# Optimizations
CFLAGS="${CFLAGS} -g -O2 -fno-strength-reduce -fomit-frame-pointer"
# Code qui fait des warnings == code de porc == deux baffes dans ta gueule
# [Jylam] Removed -Wshadow in order to avoid ncurses/gl conflict
# (Comme quoi on n'est pas les seuls porcs)
CFLAGS="${CFLAGS} -Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wstrict-prototypes -Wshadow -Waggregate-return -Wmissing-prototypes -Wnested-externs -Wsign-compare"

# Build the C++ bindings?
ac_cv_my_have_cxx="no"
if test "${enable_cxx}" != "no"; then
  ac_cv_my_have_cxx="yes"
fi
AM_CONDITIONAL(USE_CXX, test "${ac_cv_my_have_cxx}" = "yes")

# Build the Ruby bindings?
ac_cv_my_have_ruby="no"
if test "${enable_ruby}" != "no"; then
  AC_PATH_PROG(RUBY, ruby, no)
  if test "${RUBY}" != "no"; then
    AC_MSG_CHECKING(for mkmf.rb)
    if "${RUBY}" -e 'require "mkmf"' >/dev/null 2>&1; then
      AC_MSG_RESULT(yes)
      ac_cv_my_have_ruby="yes"
    else
      AC_MSG_RESULT(no)
    fi
  fi
fi
AM_CONDITIONAL(USE_RUBY, test "${ac_cv_my_have_ruby}" = "yes")

# Build cacaserver?
ac_cv_my_have_network="no"
AC_CHECK_HEADERS(sys/socket.h,
 [ac_cv_my_have_network="yes"])
AM_CONDITIONAL(USE_NETWORK, test "${ac_cv_my_have_network}" = "yes")

# Use Imlib2?
if test "${enable_imlib2}" != "no"; then
  IMLIB2="no"
  PKG_CHECK_MODULES(IMLIB2, imlib2, [IMLIB2="yes"], [AC_MSG_RESULT(no)])
  if test "${IMLIB2}" != "no"; then
    AC_DEFINE(USE_IMLIB2, 1, Define to 1 to use Imlib2)
  fi
fi

# Build development tools?
PANGOFT2="no"
PKG_CHECK_MODULES(PANGOFT2, pangoft2, [PANGOFT2="yes"], [AC_MSG_RESULT(no)])
AM_CONDITIONAL(USE_PANGO, test "${PANGOFT2}" != "no")

# Build documentation?
DOXYGEN="no"
LATEX="no"
if test "${enable_doc}" != "no"; then
  AC_PATH_PROG(DOXYGEN, doxygen, no)
  if test "${DOXYGEN}" != "no"; then
    # Build LaTeX documentation?
    AC_PATH_PROG(LATEX, pdflatex, no)
    AC_PATH_PROG(KPSEWHICH, kpsewhich, no)
    AC_PATH_PROG(DVIPS, dvips, no)
    if test "${DVIPS}" = "no" -o "${KPSEWHICH}" = "no"; then
      LATEX="no"
    fi
    if test "${LATEX}" != "no"; then
      AC_MSG_CHECKING(for a4.sty and a4wide.sty)
      if "${KPSEWHICH}" a4.sty >/dev/null 2>&1; then
        if "${KPSEWHICH}" a4wide.sty >/dev/null 2>&1; then
          AC_MSG_RESULT(yes)
        else
          LATEX="no"
          AC_MSG_RESULT(no)
        fi
      else
        LATEX="no"
        AC_MSG_RESULT(no)
      fi
    fi
  fi
fi
AM_CONDITIONAL(BUILD_DOCUMENTATION, test "${DOXYGEN}" != "no")
AM_CONDITIONAL(USE_LATEX, test "${LATEX}" != "no")

AC_CONFIG_FILES([
  Makefile
  kernel/Makefile
  cucul/Makefile
  caca/Makefile
  src/Makefile
  test/Makefile
  tools/Makefile
  cxx/Makefile
  ruby/Makefile
  doc/Makefile
  msvc/Makefile
])
AC_CONFIG_FILES([
  cucul/cucul.pc
  caca/caca.pc
  doc/doxygen.cfg
])
AC_CONFIG_FILES([caca-config], [chmod 0755 caca-config])
AC_OUTPUT

