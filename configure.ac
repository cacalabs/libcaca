
AC_INIT(libcaca, 0.99.beta17)
AC_PREREQ(2.50)
AC_CONFIG_AUX_DIR(.auto)
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([no-define tar-ustar])
dnl AM_MAINTAINER_MODE

AM_CONFIG_HEADER(config.h)

LT_MAJOR="0"
LT_MINOR="99"
LT_MICRO="17"
AC_SUBST(LT_MAJOR)
AC_SUBST(LT_MINOR)
AC_SUBST(LT_MICRO)
LT_VERSION="$LT_MAJOR:$LT_MINOR:$LT_MICRO"
AC_SUBST(LT_VERSION)

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

AC_MSG_CHECKING(for suffix of libraries)
case "$shrext_cmds" in
  .dll) eval LT_SUFFIX="-$LT_MAJOR$shrext_cmds" ;;
  "") LT_SUFFIX=".so.$LT_MAJOR" ;;
  *) eval LT_SUFFIX="$shrext_cmds.$LT_MAJOR" ;;
esac
AC_MSG_RESULT($LT_SUFFIX)
AC_SUBST(LT_SUFFIX)

dnl global configuration features
AC_ARG_ENABLE(kernel,
  [  --enable-kernel         kernel mode (default no)])

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
  [  --enable-network        network support (autodetected)])
AC_ARG_ENABLE(vga,
  [  --enable-vga            VGA support for kernel mode (autodetected)])

dnl language bindings
AC_ARG_ENABLE(csharp,
  [  --enable-csharp         C# bindings (autodetected)])
AC_ARG_ENABLE(java,
  [  --enable-java           Java bindings (autodetected)])
AC_ARG_ENABLE(cxx,
  [  --enable-cxx            C++ bindings (autodetected)])
AC_ARG_ENABLE(ruby,
  [  --enable-ruby           Ruby bindings (autodetected)])

dnl example programs features
AC_ARG_ENABLE(imlib2,
  [  --enable-imlib2         Imlib2 graphics support (autodetected)])

dnl conditional builds
AC_ARG_ENABLE(debug,
  [  --enable-debug          build debug versions of the library (default no)])
AC_ARG_ENABLE(profiling,
  [  --enable-profiling      activate built-in profiling (default no)])
AC_ARG_ENABLE(plugins,
  [  --enable-plugins        make X11 and GL drivers plugins (default disabled)])
AC_ARG_ENABLE(doc,
  [  --enable-doc            build documentation (needs doxygen and LaTeX)])
AC_ARG_ENABLE(cppunit,
  [  --enable-cppunit        use cppunit for unit tests (autodetected)])
AC_ARG_ENABLE(zzuf,
  [  --enable-zzuf           use zzuf for fuzzing tests (autodetected)])

ac_cv_my_have_kernel="no"
if test "${enable_kernel}" = "yes"; then
  ac_cv_my_have_kernel="yes"
  CPPFLAGS="${CPPFLAGS} -I. -D__KERNEL__ -nostdinc -include kernel/klibc.h -fno-stack-protector"
  CCASFLAGS="${CCASFLAGS} -I. -fno-stack-protector"
  CFLAGS="${CFLAGS} -fno-builtin -O0 -I. -I.. -I../caca/ -Wall -D__KERNEL__ -fno-stack-protector -m32"
  LDFLAGS="${LDFLAGS} -nostdlib -Wl,-N -Wl,-Ttext -Wl,100000 -fno-stack-protector"
  AC_DEFINE(USE_KERNEL, 1, Define to 1 to activate kernel mode)
fi
AM_CONDITIONAL(USE_KERNEL, test "${ac_cv_my_have_kernel}" = "yes")

AC_CHECK_HEADERS(stdio.h stdarg.h signal.h sys/ioctl.h sys/time.h endian.h unistd.h arpa/inet.h netinet/in.h winsock2.h errno.h locale.h getopt.h dlfcn.h termios.h)
AC_CHECK_FUNCS(signal ioctl snprintf vsnprintf getenv putenv strcasecmp htons)
AC_CHECK_FUNCS(usleep gettimeofday atexit)

ac_cv_have_getopt_long="no"
AC_CHECK_FUNCS(getopt_long,
 [ac_cv_have_getopt_long="yes"],
 [AC_CHECK_LIB(gnugetopt, getopt_long,
   [ac_cv_have_getopt_long="yes"
    GETOPT_LIBS="${GETOPT_LIBS} -lgnugetopt"])])
if test "$ac_cv_have_getopt_long" != "no"; then
  AC_DEFINE(HAVE_GETOPT_LONG, 1, Define to 1 if you have the ‘getopt_long’ function.)
fi
AM_CONDITIONAL(NEED_GETOPT_LONG, test "$ac_cv_have_getopt_long" = "no")
AC_SUBST(GETOPT_LIBS)

AC_MSG_CHECKING(for Sleep)
AC_TRY_COMPILE([#include <windows.h>],[Sleep(42);],
 [AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_SLEEP, 1, [Define to 1 if you have the ‘Sleep’ function.])],
 [AC_MSG_RESULT(no)])

AC_MSG_CHECKING(for fsin/fcos)
AC_TRY_COMPILE([],[double x; asm volatile("fsin; fcos":"=t"(x):);],
 [AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_FSIN_FCOS, 1, [Define to 1 if you have the ‘fsin’ and ‘fcos’ instructions.])],
 [AC_MSG_RESULT(no)])

AC_MSG_CHECKING(for fldln2/fxch/fyl2x)
AC_TRY_COMPILE([],[double x; asm volatile("fldln2; fldln2; fxch; fyl2x":"=t"(x):);],
 [AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_FLDLN2, 1, [Define to 1 if you have the ‘fldln2’ and other floating point instructions.])],
 [AC_MSG_RESULT(no)])

AC_CHECK_HEADERS(zlib.h)
AC_CHECK_LIB(z, gzopen, [ZLIB_LIBS="${ZLIB_LIBS} -lz"])

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
     [AC_DEFINE(HAVE_SLSMG_UTF8_ENABLE, 1, Define to 1 if you have the ‘SLsmg_utf8_enable’ function.)])
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
       [AC_DEFINE(HAVE_GLUTCLOSEFUNC, 1, Define to 1 if you have the ‘glutCloseFunc’ function.)])
      AC_CHECK_LIB(glut, glutMainLoopEvent,
       [ac_cv_my_have_gl="yes"])
      AC_CHECK_LIB(glut, glutCheckLoop,
       [ac_cv_my_have_gl="yes"
        AC_DEFINE(HAVE_GLUTCHECKLOOP, 1, Define to 1 if you have the ‘glutCheckLoop’ function.)])])
      break])
  if test "${ac_cv_my_have_gl}" = "yes"; then
    AC_DEFINE(USE_GL, 1, Define to 1 to activate the OpenGL backend driver)
    GL_LIBS="${GL_LIBS} -lGL -lGLU -lglut"
    CACA_DRIVERS="${CACA_DRIVERS} gl"
  elif test "${enable_gl}" = "yes"; then
    AC_MSG_ERROR([cannot find OpenGL+FreeGLUT development files])
  fi
  PKG_CHECK_MODULES(FTGL, ftgl >= 2.1.3, [FTGL="yes"], [FTGL="no"])
fi
AM_CONDITIONAL(USE_FTGL, test "${FTGL}" = "yes")

if test "${enable_cocoa}" != "no"; then
  ac_cv_my_have_cocoa="no"
  AC_CHECK_HEADERS(Cocoa/Cocoa.h,
   [ac_cv_my_have_cocoa="yes"])
  if test "${ac_cv_my_have_cocoa}" = "yes"; then
    case x${target} in
    xpowerpc*darwin*)
      # 10.3 needed to link with X11
      MACOSX_SDK="${MACOSX_SDK:-/Developer/SDKs/MacOSX10.3.9.sdk}"
      GCC_VERSION="${GCC_VERSION:-3.3}"
      ARCH="${ARCH:--arch ppc}"
      MACOSX_SDK_CFLAGS="${MACOSX_SDK_CFLAGS:--nostdinc -isystem ${MACOSX_SDK}/usr/include/gcc/darwin/${GCC_VERSION} -isystem ${MACOSX_SDK}/usr/include}"
      MACOSX_SDK_CXXFLAGS="${MACOSX_SDK_CXXFLAGS:--nostdinc++ -I${MACOSX_SDK}/usr/include/gcc/darwin/${GCC_VERSION}/c++ -I${MACOSX_SDK}/usr/include/gcc/darwin/${GCC_VERSION}/c++/ppc-darwin -I${MACOSX_SDK}/usr/include/gcc/darwin/${GCC_VERSION}/c++/backward}"
      MACOSX_SDK_LDFLAGS="${MACOSX_SDK_LDFLAGS:--L${MACOSX_SDK}/usr/lib/gcc/darwin -L${MACOSX_SDK}/usr/lib/gcc/darwin/${GCC_VERSION} -L${MACOSX_SDK}/usr/lib}"
      ;;
    x*86_64*darwin*)
      MACOSX_SDK="${MACOSX_SDK:-/Developer/SDKs/MacOSX10.5.sdk}"
      GCC_VERSION="${GCC_VERSION:-4.0}"
      ARCH="${ARCH:--arch i386}"
      MACOSX_SDK_CFLAGS="${MACOSX_SDK_CFLAGS:--isysroot ${MACOSX_SDK}}"
      MACOSX_SDK_CXXFLAGS="${MACOSX_SDK_CXXFLAGS:-${MACOSX_SDK_CFLAGS}}"
      ;;
    x*86*darwin*)
      MACOSX_SDK="${MACOSX_SDK:-/Developer/SDKs/MacOSX10.6.sdk}"
      GCC_VERSION="${GCC_VERSION:-4.0}"
      ARCH="${ARCH:--arch i386}"
      MACOSX_SDK_CFLAGS="${MACOSX_SDK_CFLAGS:--isysroot ${MACOSX_SDK}}"
      MACOSX_SDK_CXXFLAGS="${MACOSX_SDK_CXXFLAGS:-${MACOSX_SDK_CFLAGS}}"
      ;;
    esac
    CC="${CC:-gcc-${GCC_VERSION}}"
    CXX="${CXX:-g++-${GCC_VERSION}}"
    MACOSX_SDK_FRAMEWORKS="${MACOSX_SDK_FRAMEWORKS:--F${MACOSX_SDK}/System/Library/Frameworks}"
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

if test "${enable_vga}" != "no"; then
  if test "${ac_cv_my_have_kernel}" = "yes"; then
    AC_DEFINE(USE_VGA, 1, Define to 1 to activate the VGA backend driver)
    CACA_DRIVERS="${CACA_DRIVERS} vga"
  elif test "${enable_vga}" = "yes"; then
    AC_MSG_ERROR([--enable-vga requires --enable-kernel])
  fi
fi

if test "${enable_debug}" = "yes"; then
  AC_DEFINE(DEBUG, 1, Define to 1 to activate debug)
fi

if test "${enable_profiling}" = "yes"; then
  AC_DEFINE(PROF, 1, Define to 1 to activate profiling)
fi

if test "${enable_plugins}" = "yes"; then
  ac_cv_my_have_plugins="yes"
  AC_DEFINE(USE_PLUGINS, 1, Define to 1 to activate plugins)
  CACA_LIBS="${CACA_LIBS} -ldl"
else
  CACA_CFLAGS="${CACA_CFLAGS} ${X11_CFLAGS} ${GL_CFLAGS} ${FTGL_CFLAGS}"
  CACA_LIBS="${CACA_LIBS} ${X11_LIBS} ${GL_LIBS} ${FTGL_LIBS}"
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
AC_SUBST(ZLIB_LIBS)
AC_SUBST(GETOPT_LIBS)
AC_SUBST(CACA_CFLAGS)
AC_SUBST(CACA_LIBS)
AC_SUBST(X11_CFLAGS)
AC_SUBST(X11_LIBS)
AC_SUBST(GL_CFLAGS)
AC_SUBST(GL_LIBS)

# How to get the C99 types. See caca/caca_types.h.in for details about
# the CACA_TYPES variable
if test "${ac_cv_my_have_kernel}" = "yes"; then
  CACA_TYPES=0
else
  AC_CHECK_HEADERS(stdint.h,
    [CACA_TYPES=1],
    [AC_CHECK_HEADERS(inttypes.h,
      [CACA_TYPES=2],
      [AC_CHECK_HEADERS(windows.h,
        [CACA_TYPES=3],
        [CACA_TYPES=0])])])
fi
AC_SUBST(CACA_TYPES)

# Optimizations
CFLAGS="${CFLAGS} -g -O2 -fno-strength-reduce -fomit-frame-pointer"
# Code qui fait des warnings == code de porc == deux baffes dans ta gueule
CFLAGS="${CFLAGS} -Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wstrict-prototypes -Wshadow -Waggregate-return -Wmissing-prototypes -Wnested-externs -Wsign-compare"
CXXFLAGS="${CXXFLAGS} -Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wshadow -Wsign-compare"

CACA_BINDINGS=""

# Build the C++ bindings?
ac_cv_my_have_cxx="no"
if test "${enable_cxx}" != "no"; then
  AC_MSG_CHECKING([working <iostream>])
  AC_LANG_PUSH(C++)
  AC_TRY_COMPILE(
   [#include <iostream>],
   [],
   [ac_cv_my_have_cxx="yes"
    CACA_BINDINGS="${CACA_BINDINGS} C++"])
  AC_LANG_POP(C++)
  AC_MSG_RESULT([$ac_cv_my_have_cxx])
fi
AM_CONDITIONAL(USE_CXX, test "${ac_cv_my_have_cxx}" = "yes")

# Build the .NET bindings?
ac_cv_my_have_csharp="no"
if test "${enable_csharp}" != "no"; then
  AC_PATH_PROG(CSC, csc, no)
  if test "$CSC" = "no"; then
    AC_PATH_PROG(GMCS, gmcs, no)
    CSC="$GMCS"
  fi
  AC_PATH_PROG(GACUTIL, gacutil, no)
  if test "${CSC}" != "no" -a "${GACUTIL}" != "no"; then
    ac_cv_my_have_csharp="yes"
    CACA_BINDINGS="${CACA_BINDINGS} C#"
  fi
fi
AM_CONDITIONAL(USE_CSHARP, test "${ac_cv_my_have_csharp}" = "yes")

# Build the Java bindings?
ac_cv_my_have_java="no"
if test "${enable_java}" != "no" -a "${ac_cv_my_have_kernel}" != "yes"; then
  AC_PATH_PROG(JAVAC, javac, no)
  AC_PATH_PROG(JAVAH, javah, no)
  AC_PATH_PROG(JAR, jar, no)
  if test "${JAVAC}" != "no" -a "${JAVAH}" != "no" -a "${JAR}" != "no"; then
    if test "$JAVA_HOME" = ""; then
      # Try to resolve JAVA_HOME
      JAVAC_EXE="$JAVAC"
      # Follow symlinks
      while test -h "$JAVAC_EXE"; do
        ls=`ls -ld "$JAVAC_EXE"`
        link=`expr "$ls" : '.*-> \(.*\)$'`
        if expr "$link" : '/.*' > /dev/null; then
          JAVAC_EXE="$link"
        else
          JAVAC_EXE="`dirname "$JAVAC_EXE"`/$link"
        fi
      done
      JAVA_HOME="`dirname $JAVAC_EXE`/.."
    fi
    # Include path to jni.h
    JAVA_CFLAGS="${JAVA_CFLAGS} -I$JAVA_HOME/include"
    # Add the OS specific include path
    for dir in $JAVA_HOME/include/*; do
      if test -f "$dir/jni_md.h"; then
        JAVA_CFLAGS="${JAVA_CFLAGS} -I$dir"
      fi
    done
    AC_SUBST(JAVA_CFLAGS)
    AC_SUBST(JAVA_LIBS)
    AC_LANG_PUSH(C)
    CPPFLAGS="${CPPFLAGS} ${JAVA_CFLAGS}"
    AC_CHECK_HEADERS([jni.h],
     [ac_cv_my_have_java="yes"
      CACA_BINDINGS="${CACA_BINDINGS} Java"])
    AC_LANG_POP(C)
  fi
fi
AM_CONDITIONAL(USE_JAVA, test "${ac_cv_my_have_java}" = "yes")

# Build the Ruby bindings?
ac_cv_my_have_ruby="no"
if test "${enable_ruby}" != "no"; then
  AC_PATH_PROG(RUBY, ruby, no)
  if test "${RUBY}" != "no"; then
    RUBY_CFLAGS="-I$(ruby -r rbconfig -e 'print Config::CONFIG@<:@"archdir"@:>@')"
    RUBY_LIBS="-L$(ruby -r rbconfig -e 'print Config::CONFIG@<:@"libdir"@:>@') -l$(ruby -r rbconfig -e 'print Config::CONFIG@<:@"RUBY_SO_NAME"@:>@')"
    RUBY_SITEARCHDIR=`ruby -r rbconfig -e 'print Config::CONFIG@<:@"sitearchdir"@:>@'`
    RUBY_SITELIBDIR=`ruby -r rbconfig -e 'print Config::CONFIG@<:@"sitelibdir"@:>@'`
    AC_SUBST(RUBY_CFLAGS)
    AC_SUBST(RUBY_LIBS)
    AC_SUBST(RUBY_SITEARCHDIR)
    AC_SUBST(RUBY_SITELIBDIR)
    CPPFLAGS="${CPPFLAGS} ${RUBY_CFLAGS}"
    AC_CHECK_HEADERS([ruby.h],
     [ac_cv_my_have_ruby="yes"
      CACA_BINDINGS="${CACA_BINDINGS} Ruby"])
  fi
fi
AM_CONDITIONAL(USE_RUBY, test "${ac_cv_my_have_ruby}" = "yes")

AC_MSG_CHECKING(available language bindings)
if test -z "${CACA_BINDINGS}"; then
  AC_MSG_RESULT(no)
else
  CACA_BINDINGS="${CACA_BINDINGS# *}"
  AC_MSG_RESULT([${CACA_BINDINGS}])
fi

# Build cacaserver?
ac_cv_my_have_network="no"
AC_CHECK_HEADERS(sys/socket.h,
 [ac_cv_my_have_network="yes"])
AM_CONDITIONAL(USE_NETWORK, test "${ac_cv_my_have_network}" = "yes")

# Use Imlib2?
if test "${enable_imlib2}" != "no"; then
  PKG_CHECK_MODULES(IMLIB2, imlib2, [IMLIB2="yes"], [IMLIB2="no"])
  if test "${IMLIB2}" = "yes"; then
    AC_DEFINE(USE_IMLIB2, 1, Define to 1 to use Imlib2)
  fi
fi

# Build development tools?
PKG_CHECK_MODULES(PANGOFT2, pangoft2, [PANGOFT2="yes"], [PANGOFT2="no"])
AM_CONDITIONAL(USE_PANGO, test "${PANGOFT2}" = "yes")

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

# Use cppunit for unit tests?
PKG_CHECK_MODULES(CPPUNIT, cppunit, [CPPUNIT="yes"], [CPPUNIT="no"])
AM_CONDITIONAL(USE_CPPUNIT, test "$CPPUNIT" = "yes")

# Use zzuf for fuzzing tests?
ZZUF="no"
if test "${enable_zzuf}" != "no"; then
  AC_PATH_PROG(ZZUF, zzuf, no)
fi
AM_CONDITIONAL(USE_ZZUF, test "${ZZUF}" != "no")

AC_CONFIG_FILES([
  Makefile
  kernel/Makefile
  caca/Makefile
  src/Makefile
  examples/Makefile
  test/Makefile
  tools/Makefile
  java/Makefile
  caca-sharp/Makefile
  cxx/Makefile
  python/Makefile
  ruby/Makefile
  doc/Makefile
  win32/Makefile
])
AC_CONFIG_FILES([
  caca/caca_types.h
  caca/caca.pc
  caca-sharp/caca-sharp.dll.config
  cxx/caca++.pc
  doc/doxygen.cfg
])
AC_CONFIG_FILES([caca-config], [chmod 0755 caca-config])
AC_OUTPUT

