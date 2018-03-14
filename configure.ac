dnl configure.ac

dnl Copyright (C) 2010 Free Software Foundation, Inc.
dnl
dnl This program is free software, licensed under the terms of the GNU
dnl General Public License as published by the Free Software Foundation,
dnl either version 3 of the License, or (at your option) any later version.
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.

AC_PREREQ([2.61])
AC_INIT([LibreDWG],[0.4-dev],[libredwg@gnu.org])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.12.2 gnu no-define serial-tests dist-xz])

dnl libretoolize asked me for it
AC_CONFIG_MACRO_DIR([m4])

dnl create a config.h file (Automake will add -DHAVE_CONFIG_H)
AM_CONFIG_HEADER([src/config.h])

ISODATE=`date +%Y-%m-%d`
AC_SUBST(ISODATE)

AC_CANONICAL_HOST

dnl Checks for programs.
PKG_PROG_PKG_CONFIG
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_LIBTOOL
LT_INIT

dnl Checks for libraries.
AC_CHECK_LIB([ps],[PS_open_file],
  libredwg_have_libps=yes,
  libredwg_have_libps=no)

PKG_CHECK_MODULES([LIBXML2],[libxml-2.0],,
[AC_MSG_ERROR([Install libxml-2.0 first])])

XML2_CFLAGS=`$PKG_CONFIG libxml-2.0 --cflags`
AC_SUBST([XML2_CFLAGS])
XML2_LIBS=`$PKG_CONFIG libxml-2.0 --libs`
AC_SUBST([XML2_LIBS])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h malloc.h])

AC_CHECK_HEADER(dejagnu.h, ,
  [AC_MSG_WARN([Couldn't find dejagnu.h])])
AM_CONDITIONAL([HAVE_DEJAGNU], [test "x$ac_cv_header_dejagnu_h" = xyes])

# Don't bother testing for libps headers if libps is not available.
if test yes = "$libredwg_have_libps" ; then
  AC_CHECK_HEADERS([libps/pslib.h])
fi

dnl Checks for typedefs, structures, and compiler characteristics.
AC_PROG_CC_C99
AC_TYPE_SIZE_T
AC_TYPE_INT64_T
AC_TYPE_UINT64_T
AC_CHECK_SIZEOF(size_t)

AX_IS_RELEASE([git-directory])
dnl disable unwanted warnings here. These should be fixed before 1.0
enable_Werror=no
ax_enable_compile_warnings=yes
AX_COMPILER_FLAGS([WARN_CFLAGS],[WARN_LDFLAGS])
dnl if test x$ax_cv_check_cflags__Wunused_but_set_variable = xyes; then
dnl   WARN_CFLAGS="$WARN_CFLAGS -Wno-unused-but-set-variable"
dnl fi
dnl if test x$ax_cv_check_cflags__Wunused_variable = xyes; then
dnl   WARN_CFLAGS="$WARN_CFLAGS -Wno-unused-variable"
dnl fi

if test "x$ac_cv_prog_cc_c99" = "xno"; then
   AC_MSG_WARN([This compiler ${CC} has no c99 support])
else
   AC_DEFINE([HAVE_C99], 1,
        [Defined to 1 when the compiler supports c99, mostly (...) macros])
   if test "x$ac_cv_prog_cc_c99" != "x"; then
      AC_MSG_RESULT([added $ac_cv_prog_cc_c99 to CFLAGS])
      AM_CFLAGS="$AM_CFLAGS $ac_cv_prog_cc_c99"
   fi
fi
AC_SUBST(HAVE_C99)

dnl Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STAT

dnl Feature: --enable-trace
AC_ARG_ENABLE([trace],[AS_HELP_STRING([--enable-trace],[
    Enable runtime tracing (default: no).  When enabled, the environment
    variable LIBREDWG_TRACE is consulted on the first decode/encode attempt.
    Its value is an integer: 0 (no output) through 9 (full verbosity).])],[
  AC_DEFINE([USE_TRACING],1,[Define to 1 to enable runtime tracing support.])
])

dnl Feature: --enable-write
AC_ARG_ENABLE([write],[AS_HELP_STRING([--enable-write],[
    Enable write support (default: no).])],[
  AC_DEFINE([USE_WRITE],1,[Define to 1 to enable write support.])
])
AM_CONDITIONAL([USE_WRITE], [test -n "$enable_write"])

dnl Test support.
AM_CONDITIONAL([HAVE_LIBPS],[test yes = "$libredwg_have_libps" &&
                             test yes = "$ac_cv_header_libps_pslib_h"])

dnl for SWIG - should be optional
AM_PATH_PYTHON([2.7])
AX_PKG_SWIG([1.3.17], [], [ AC_MSG_WARN([SWIG is required for python bindings]) ])
dnl AX_SWIG_ENABLE_CXX
dnl SWIG_MULTI_MODULE_SUPPORT
SWIG_PYTHON
AM_CONDITIONAL([HAVE_SWIG_PYTHON], [test -n "$SWIG" && test -n "$PYTHON_VERSION"])

dnl check for mingw, cross and wine
AM_CONDITIONAL([HAVE_MINGW], [test "x$host_os" = "xmingw32"])
AM_CONDITIONAL([HAVE_MINGW_CROSS],
        [test "x$host_os" = "xmingw32" -a "x$build_os" != "xmingw32"])
if test "x$host_os" = "xmingw32" -a "x$build_os" != "xmingw32"; then
   AC_CHECK_PROGS([WINE], [wine])
   if test -z "$WINE"; then
      AC_MSG_WARN([wine not found - cannot check cross-compiled executables])
   else
      AC_MSG_RESULT([wine found - test with make check-wine])
      AM_CFLAGS="$AM_CFLAGS -D_WINE_MSVCRT"
   fi
fi

dnl check for valgrind
case $host_os in
  darwin*)
    enable_valgrind_helgrind=no
    enable_valgrind_drd=no
    ;;
esac
dnl passes only on some systems. --enable-valgrind-sgcheck
AX_VALGRIND_DFLT([sgcheck], [off])
AX_VALGRIND_CHECK()

dnl Write it out
AC_CONFIG_FILES([
	Makefile
	src/Makefile
	doc/Makefile
	examples/Makefile
	bindings/Makefile
	bindings/python/Makefile
	testsuite/Makefile
	testsuite/testcases/Makefile
	testsuite/xmlsuite/Makefile
	unit-testing/Makefile
])

AC_OUTPUT

dnl configure.ac ends here
