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
AM_INIT_AUTOMAKE([1.12.2 gnu no-define serial-tests])

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
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

AC_CHECK_HEADER(dejagnu.h, , [AC_MSG_ERROR(
[Couldn't find dejagnu.h. Install Dejagnu first] )])

# Don't bother testing for libps headers if libps is not available.
if test yes = "$libredwg_have_libps" ; then
  AC_CHECK_HEADERS([libps/pslib.h])
fi

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

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

dnl Test support.
AM_CONDITIONAL([HAVE_LIBPS],[test yes = "$libredwg_have_libps" &&
                             test yes = "$ac_cv_header_libps_pslib_h"])

dnl for SWIG - should be optional
AM_PATH_PYTHON([2.3])
AX_PKG_SWIG([1.3.17], [], [ AC_MSG_ERROR([SWIG is required to build..]) ])
dnl AX_SWIG_ENABLE_CXX
dnl SWIG_MULTI_MODULE_SUPPORT
SWIG_PYTHON



dnl Write it out!

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
