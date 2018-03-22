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
dnl for older systems use this:
dnl AM_INIT_AUTOMAKE([1.11.6 gnu no-define])
AM_INIT_AUTOMAKE([1.12.2 gnu no-define serial-tests dist-xz])

AC_CONFIG_MACRO_DIR([m4])

# Check if automake supports 'pretty' builds, if so enable them by default
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

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

dnl PKG_CHECK_MODULES([LIBXML2], [libxml-2.0], [],
dnl    AC_MSG_ERROR([Install libxml-2.0 first]))
if pkg-config --exists libxml-2.0; then
  XML2_CFLAGS=`pkg-config libxml-2.0 --cflags`
  XML2_LIBS=`pkg-config libxml-2.0 --libs`
fi
AC_SUBST([XML2_CFLAGS])
AC_SUBST([XML2_LIBS])

dnl Checks for header files.
AC_HEADER_STDC
dnl avoid rpl_malloc when cross-compiling. we don't need malloc.h
dnl we just want to know if it's needed
ac_cv_func_malloc_0_nonnull=yes
ac_cv_func_realloc_0_nonnull=yes
AC_CHECK_HEADERS([stdlib.h string.h unistd.h malloc.h wchar.h])

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
AC_TYPE_UINT16_T
AC_TYPE_INT32_T
AC_TYPE_UINT32_T
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
dnl until we can make a release, favor no optims.
dnl it's fast enough and better for debugging.
if test x$ac_compiler_gnu = xyes; then
  CFLAGS="-g -fno-omit-frame-pointer"
else
  CFLAGS="-g"
fi

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


AC_ARG_ENABLE(gcov,
	AS_HELP_STRING([--enable-gcov],
		       [turn on test coverage @<:@default=no@:>@]),
	[case "${enableval}" in
	yes) enable_gcov=true ; gcov=gcov ;;
	no)  enable_gcov=false ;;
	*)   enable_gcov=true ; gcov="${enableval}" ;;
	esac], [enable_gcov=false ])

AC_MSG_CHECKING([for --enable-gcov])
if test "x$enable_gcov" = "xtrue" ; then
	dnl if test "$GCC" = yes; then
	dnl 	AC_MSG_ERROR([gcov only works if gcc is used])
	dnl fi

        dnl You might need the compiler-specific gcov: e.g. gcov-mp-6
	GCOV="$gcov"
	AC_MSG_RESULT([gcov=$gcov])
	AC_SUBST(GCOV)

	GCOV_CFLAGS="-fprofile-arcs -ftest-coverage"
	AC_SUBST(GCOV_CFLAGS)

	dnl libtool 1.5.22 and lower strip -fprofile-arcs from the flags
	dnl passed to the linker, which is a bug; -fprofile-arcs implicitly
	dnl links in -lgcov, so we do it explicitly here for the same effect
	GCOV_LIBS=-lgcov
	AC_SUBST(GCOV_LIBS)
else
	AC_MSG_RESULT([no (default)])
fi
AM_CONDITIONAL(ENABLE_GCOV, test "x$enable_gcov" = "xtrue")

AM_CONDITIONAL([HAVE_LIBPS],[test yes = "$libredwg_have_libps" &&
                             test yes = "$ac_cv_header_libps_pslib_h"])

dnl check for mingw, cross and wine
AM_CONDITIONAL([HAVE_MINGW], [test "x$host_os" = "xmingw32"])
AM_CONDITIONAL([HAVE_MINGW_CROSS],
        [test "x$host_os" = "xmingw32" -a "x$build_os" != "xmingw32"])
if test "x$host_os" = "xmingw32" -a "x$build_os" != "xmingw32"; then
   cross_compiling=yes
   AC_CHECK_PROGS([WINE], [wine])
   if test -z "$WINE"; then
      AC_MSG_WARN([wine not found - cannot check cross-compiled executables])
   else
      AC_MSG_RESULT([wine found - test with make check-wine])
      AM_CFLAGS="$AM_CFLAGS -D_WINE_MSVCRT"
   fi
fi
AM_CONDITIONAL([HAVE_WINE], [test -n "$WINE"])

dnl optional SWIG + python bindings
if test x$cross_compiling = xno; then
    AX_PKG_SWIG([1.3.17], [],
      AC_MSG_WARN([SWIG is required for python bindings. disabled]))
    if test -n "$SWIG"; then
      AM_PATH_PYTHON([2.7])
      SWIG_PYTHON
    fi
    dnl AX_SWIG_ENABLE_CXX
    dnl SWIG_MULTI_MODULE_SUPPORT
else
    AC_MSG_WARN([disabled swig bindings: cross-compiled])
fi
AM_CONDITIONAL([HAVE_SWIG_PYTHON],
  [test -n "$SWIG" && test -n "$PYTHON_VERSION"])

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
	test/Makefile
	test/testcases/Makefile
	test/xmlsuite/Makefile
	test/unit-testing/Makefile
])

AC_OUTPUT

dnl configure.ac ends here
