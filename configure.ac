dnl configure.ac

dnl Copyright (C) 2010, 2014-2015, 2018-2019 Free Software Foundation, Inc.
dnl
dnl This program is free software, licensed under the terms of the GNU
dnl General Public License as published by the Free Software Foundation,
dnl either version 3 of the License, or (at your option) any later version.
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.

AC_PREREQ([2.61])
AC_INIT([LibreDWG],
        m4_esyscmd([build-aux/git-version-gen .tarball-version]),
        [libredwg@gnu.org],,
        [https://savannah.gnu.org/projects/libredwg/])
AC_CONFIG_AUX_DIR([build-aux])
dnl for older systems use this:
dnl AM_INIT_AUTOMAKE([1.11.6 gnu no-define])
AM_INIT_AUTOMAKE([1.12.2 gnu no-define serial-tests dist-xz])

# Keep this in sync with what is passed in AC_INIT
TARBALL_VERSION_FILE=".tarball-version"
dnl Define these substitutions here to keep all version information in
dnl one place. For information on how to properly maintain the library
dnl version information, refer to the libtool manual, section "Updating
dnl library version information":
dnl http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
AC_SUBST([LIBREDWG_SO_VERSION], [0:10:0])
AC_SUBST([TARBALL_VERSION_FILE])

AC_CONFIG_MACRO_DIR([m4])

# Check if automake supports 'pretty' builds, if so enable them by default
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

dnl create a config.h file (Automake will add -DHAVE_CONFIG_H)
AM_CONFIG_HEADER([src/config.h])
AC_MSG_CHECKING([git version])
AC_MSG_RESULT($PACKAGE_VERSION)
AC_MSG_CHECKING([so version-info])
AC_MSG_RESULT($LIBREDWG_SO_VERSION)

ISODATE=`date +%Y-%m-%d`
AC_SUBST(ISODATE)

AC_CANONICAL_HOST

dnl Checks for programs
PKG_PROG_PKG_CONFIG
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_LIBTOOL
LT_INIT

if test x$enable_shared = xyes; then
   AC_DEFINE([ENABLE_SHARED],1,[Define to 1 if a shared library will be built])
   LTEXEC="\${top_builddir}/libtool --mode=execute"
fi
AM_CONDITIONAL([ENABLE_SHARED], [test "x$enable_shared" = xyes])
AC_SUBST(LTEXEC)

dnl Checks for libraries
AC_CHECK_LIB([ps],[PS_open_file],
  libredwg_have_libps=yes,
  libredwg_have_libps=no)

dnl `pcre2-config --libs8`
AC_CHECK_LIB([pcre2-8],[pcre2_config_8],
  [libredwg_have_libpcre2=yes; PCRE2_LIBS=-lpcre2-8],
  [if pkg-config --exists libpcre2; then
    libredwg_have_libpcre2=yes;
    PCRE2_CFLAGS=`pkg-config libpcre2 --cflags`
    PCRE2_LIBS=`pkg-config libpcre2 --libs`
  else
    AC_MSG_WARN([libpcre2 for dwggrep not found])
  fi])
AC_CHECK_LIB([pcre2-16],[pcre2_config_16],
  [libredwg_have_libpcre2_16=yes])
if test yes = "$libredwg_have_libpcre2_16"; then
  AC_DEFINE([HAVE_PCRE2_16],1,[Define to 1 if -lpcre2-16 is used also])
  PCRE2_LIBS="$PCRE2_LIBS -lpcre2-16"
fi
AC_SUBST([PCRE2_CFLAGS])
AC_SUBST([PCRE2_LIBS])
  
dnl PKG_CHECK_MODULES([LIBXML2], [libxml-2.0], [],
dnl    AC_MSG_ERROR([Install libxml-2.0 first]))
if pkg-config --exists libxml-2.0; then
  XML2_CFLAGS=`pkg-config libxml-2.0 --cflags`
  XML2_LIBS=`pkg-config libxml-2.0 --libs`
fi
AC_SUBST([XML2_CFLAGS])
AC_SUBST([XML2_LIBS])

dnl Checks for header files
AC_HEADER_STDC
dnl Avoid rpl_malloc when cross-compiling. we don't need malloc.h
dnl We just want to know if it's needed
ac_cv_func_malloc_0_nonnull=yes
ac_cv_func_realloc_0_nonnull=yes
AC_CHECK_HEADERS([stdlib.h string.h stddef.h unistd.h malloc.h wchar.h ctype.h \
                  libgen.h getopt.h alloca.h valgrind/valgrind.h])
AX_INCLUDE_STRCASECMP

# Don't bother testing for libps headers if libps is not available.
if test yes = "$libredwg_have_libps" ; then
  AC_CHECK_HEADERS([libps/pslib.h])
fi
AM_CONDITIONAL([HAVE_LIBPS],[test "x$libredwg_have_libps" = xyes &&
                             test "x$ac_cv_header_libps_pslib_h" = xyes])

if test yes = "$libredwg_have_libpcre2"; then
  AC_CHECK_HEADERS([pcre2.h],[],[],[#define PCRE2_CODE_UNIT_WIDTH 8])
else
  AC_MSG_WARN([Couldn't find pcre2.h. No regex support in dwggrep])
fi

dnl Checks for typedefs, structures, and compiler characteristics
AC_PROG_CC_C99
case $CFLAGS in
     *-fsanitize=undefined*)   ax_cv_have_aligned_access_required=yes  ;;
     *-fsanitize=*,undefined*) ax_cv_have_aligned_access_required=yes  ;;
esac
AX_CHECK_ALIGNED_ACCESS_REQUIRED

case $CFLAGS in
     *-fsanitize=address*)     HAVE_ASAN=yes ;;
     *-fsanitize=*,address*)   HAVE_ASAN=yes ;;
esac
case $CFLAGS in
     *-fsanitize=leak*)        HAVE_ASAN=yes ;;
     *-fsanitize=*,leak*)      HAVE_ASAN=yes ;;
esac
AC_SUBST(HAVE_ASAN)

AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_INT32_T
AC_TYPE_UINT32_T
AC_TYPE_INT64_T
AC_TYPE_UINT64_T
AC_CHECK_SIZEOF([size_t])
dnl AC_CHECK_TYPE([wchar_t],[],[],[[#include <wchar.h>]])
if test x$ac_cv_header_wchar_h = xyes; then
   AX_COMPILE_CHECK_SIZEOF([wchar_t], [#include <wchar.h>])
fi
dnl AX_GCC_BUILTIN(__builtin_offsetof)
AC_DEFINE([_POSIX_C_SOURCE],[200809L],[Needed for strdup])
AC_DEFINE([__XSI_VISIBLE],[700],[Needed for cygwin strdup])

AX_IS_RELEASE([git-directory])
if test x$ax_is_release = xyes; then
   AC_DEFINE([IS_RELEASE],1,[Define to 1 if this is a release,
   skipping unstable DWG features, unknown DWG versions and objects.])
fi
AM_CONDITIONAL([IS_RELEASE], [test x$ax_is_release = xyes])

dnl Disable unwanted warnings here. These should be fixed before 1.0
enable_Werror=no
ax_enable_compile_warnings=yes
AX_COMPILER_FLAGS([WARN_CFLAGS],[AM_LDFLAGS],[$ax_is_release],[-fvisibility=hidden])
dnl if test x$ax_cv_check_cflags__Wunused_but_set_variable = xyes; then
dnl   WARN_CFLAGS="$WARN_CFLAGS -Wno-unused-but-set-variable"
dnl fi
dnl if test x$ax_cv_check_cflags__Wunused_variable = xyes; then
dnl   WARN_CFLAGS="$WARN_CFLAGS -Wno-unused-variable"
dnl fi

AC_CACHE_CHECK([for __attribute__((visibility("default")))],
  ac_cv_attribute_visibility_default, [
  ac_cv_attribute_visibility_default=no
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
    [[ int __attribute__ ((visibility ("default"))) foo (void) { return 1; } ]], [])],
    [ac_cv_attribute_visibility_default=yes])
  ])
if test x$ac_cv_attribute_visibility_default = xyes;
then
  AC_DEFINE(HAVE_ATTRIBUTE_VISIBILITY_DEFAULT, 1,
    [Define if __attribute__((visibility("default"))) is supported.])
fi
dnl AX_GCC_FUNC_ATTRIBUTE(visibility)
AX_GCC_FUNC_ATTRIBUTE(gnu_format)
AX_GCC_FUNC_ATTRIBUTE(ms_format)
AX_GCC_FUNC_ATTRIBUTE(format)
AX_GCC_FUNC_ATTRIBUTE(malloc)
AX_GCC_FUNC_ATTRIBUTE(returns_nonnull)

dnl until we can make a release, favor no -O2 optims.
dnl it's fast enough and better for debugging.
dnl if test x$ac_compiler_gnu = xyes; then
dnl   CFLAGS="$CFLAGS -fno-omit-frame-pointer"
dnl fi

if test "x$ac_cv_prog_cc_c99" = "xno"; then
   AC_MSG_WARN([This compiler ${CC} has no c99 support])
else
   AC_DEFINE([HAVE_C99], 1,
        [Defined to 1 when the compiler supports c99, mostly (...) macros])
   if test "x$ac_cv_prog_cc_c99" != "x"; then
      AC_MSG_RESULT([added $ac_cv_prog_cc_c99 to CFLAGS])
      CFLAGS="$CFLAGS $ac_cv_prog_cc_c99"
   fi
fi
AC_SUBST(HAVE_C99)

dnl Checks for library functions
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_ALLOCA
dnl broken for cross-compiling
dnl AC_FUNC_STRTOD
AC_CHECK_FUNC([strtod],[],
  AC_MSG_ERROR([C89 strtod not found. Ignore for now]))
AC_CHECK_FUNC([strdup],[],
  AC_MSG_ERROR([strdup not found. Check your _POSIX_C_SOURCE vs ANSI settings.]))
AC_CHECK_FUNCS([getopt_long],[],
  AC_MSG_WARN([getopt_long not found. programs will not accept long options.]))
AC_CHECK_FUNCS([basename],[],
  AC_MSG_WARN([basename not found. The default outfile will be unexpected.]))
AC_CHECK_FUNCS([strcasestr],[],
  AC_MSG_WARN([strcasestr not found. Using a slower workaround.]))

dnl mingw-w64 7.0 bug with _FORTIFY_SOURCE, need -lssp
case $host in
 *-w64-mingw32)
   case $CFLAGS in
     *-fstack-protector*) ;;
     *) CFLAGS="$CFLAGS -fstack-protector" ;;
   esac
   ;;
esac
dnl x86_64-mingw-w64-gcc-9.2 is seriously broken, and not just mingw-w64

dnl --enable-debug sets DEBUG_CLASSES, will crash acad import
AC_MSG_CHECKING([--enable-debug])
AC_ARG_ENABLE([debug],AS_HELP_STRING([--enable-debug],[
    Enable unstable DEBUG_CLASSES (default: no).]),
  [if test x$ac_compiler_gnu = xyes; then
    CFLAGS="`echo $CFLAGS|sed 's,-O2,,'` -fno-omit-frame-pointer"
  fi
  CFLAGS="$CFLAGS -DDEBUG_CLASSES"]
  AC_MSG_RESULT([yes]),
  AC_MSG_RESULT([no (default)]))
AM_CONDITIONAL([ENABLE_DEBUG], [test x$enable_debug = xyes])

AX_ADD_FORTIFY_SOURCE
AX_RESTRICT

dnl Feature: --disable-write
AC_MSG_CHECKING([--disable-write])
AC_ARG_ENABLE([write],AS_HELP_STRING([--disable-write],[
    Disable write support (default: no).]),[],[enable_write=yes])
AM_CONDITIONAL([USE_WRITE], [test x$enable_write != xno])
AS_IF([test x$enable_write != xno],
  AC_DEFINE([USE_WRITE],1,[Undefine to disable write support.])
  AC_MSG_RESULT([no (default)]),
  AC_MSG_RESULT([yes]))

dnl Feature: --enable-trace
AC_MSG_CHECKING([--enable-trace])
AC_ARG_ENABLE([trace],AS_HELP_STRING([--enable-trace],[
    Enable runtime tracing (default: no).  When enabled, the environment
    variable LIBREDWG_TRACE is consulted on the first decode/encode attempt.
    Its value is an integer: 0 (no output) through 9 (full verbosity).]),
  AC_DEFINE([USE_TRACING],1,[Define to 1 to enable runtime tracing support.])
  AC_MSG_RESULT([yes]),
  AC_MSG_RESULT([no (default)]))

AC_CHECK_FUNCS([setenv],[],
  AS_IF([test x$enable_trace = xyes],
    [AC_MSG_WARN([setenv not found. --enable-trace is limited with cmdline apps.
      Only -v args are taken for verbosity not the LIBREDWG_TRACE env var.])],
    [AC_MSG_RESULT([setenv not needed. no --enable-trace.])]
    ))

dnl --disable-dxf only useful for faster debug/test cycles
AC_MSG_CHECKING([--disable-dxf])
AC_ARG_ENABLE([dxf],AS_HELP_STRING([--disable-dxf],[
    Disable DXF and other in/out modules (default: no).]),[],[enable_dxf=yes])
AM_CONDITIONAL([DISABLE_DXF], [test x$enable_dxf = xno])
AS_IF([test x$enable_dxf = xno],
  AC_DEFINE([DISABLE_DXF],1,[Define to disable DXF and other in/out modules.])
  AC_MSG_RESULT([yes]),
  AC_MSG_RESULT([no (default)]))

dnl --disable-bindings
AC_MSG_CHECKING([--disable-bindings])
AC_ARG_ENABLE([bindings],AS_HELP_STRING([--disable-bindings],[
    Disable SWIG bindings (default: no).]),[],[enable_bindings=yes])
AM_CONDITIONAL([DISABLE_BINDINGS], [test x$enable_bindings = xno])
AS_IF([test x$enable_bindings = xno],
  AC_DEFINE([DISABLE_BINDINGS],1,[Define to disable bindings.])
  AC_MSG_RESULT([yes]),
  AC_MSG_RESULT([no (default)]))

AC_MSG_CHECKING([for --enable-gcov])
AC_ARG_ENABLE(gcov,
	AS_HELP_STRING([--enable-gcov],
		       [turn on test coverage @<:@default=no@:>@]),
	[case "${enableval}" in
	yes) enable_gcov=true ; gcov=gcov ;;
	no)  enable_gcov=false ;;
	*)   enable_gcov=true ; gcov="${enableval}" ;;
	esac], [enable_gcov=false ])
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

AC_MSG_CHECKING([for latest clang-format])
CLANG_FORMAT=
for clangfmt in clang-format{-mp,}{-devel,-{8,7,6,5,4,3}{,.9,.8,.7,.6,.5,.4,.3,.2,.1,.0},}
do
    if which "$clangfmt" &>/dev/null; then
        CLANG_FORMAT="$clangfmt"
        break
    fi
done
if test -z $CLANG_FORMAT; then
    if which indent &>/dev/null; then
        CLANG_FORMAT="indent"
    else
        CLANG_FORMAT=echo
    fi
fi
AC_MSG_RESULT($CLANG_FORMAT)
AC_SUBST(CLANG_FORMAT)

dnl Check for mingw, cross and wine
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
      CFLAGS="$CFLAGS -D_WINE_MSVCRT"
   fi
fi
AM_CONDITIONAL([HAVE_WINE], [test -n "$WINE"])

dnl Optional SWIG + python bindings
dnl Feature: --disable-python or --enable-python=python3.6
AC_MSG_CHECKING([for --disable-python])
AC_ARG_ENABLE(python,
	AS_HELP_STRING([--disable-python],
		       [Disable python bindings and tests @<:@default=check@:>@]),
	[case "${enableval}" in
	yes) enable_python=yes; AC_MSG_RESULT([enabled]) ;;
	no)  enable_python=no; AC_MSG_RESULT([disabled]) ;;
	*)   enable_python=yes; PYTHON="${enableval}"; AC_MSG_RESULT([enabled ($PYTHON)]) ;;
	esac], [enable_python=check; AC_MSG_RESULT([check (default)]) ])

if test x$cross_compiling = xno -a x$enable_bindings = xyes; then
    AX_PKG_SWIG([1.3.17], [],
      AC_MSG_WARN([SWIG is required for python bindings. disabled]))
    if test -n "$SWIG" -a x$enable_python != xno; then
      dnl ls -al /usr/bin/python* >&6
      if test x$enable_python = xyes; then
        dnl die on error
        AM_PATH_PYTHON([2.7])
      else
        dnl skip on error
        AM_PATH_PYTHON
        PYTHON_NOVERSIONCHECK=1
      fi
      SWIG_PYTHON
      AC_CHECK_PROGS([PATCH], [patch])
    fi
    dnl swig not required for perl, we ship swig_perl.c
    dnl AX_SWIG_ENABLE_CXX
    dnl SWIG_MULTI_MODULE_SUPPORT
else
    AC_MSG_WARN([disabled swig bindings])
    if test x$enable_python != xno; then
      if test x$enable_python = xyes; then
        dnl die on error
        AM_PATH_PYTHON([2.7])
      else
        dnl skip on error
        AM_PATH_PYTHON
        PYTHON_NOVERSIONCHECK=1
      fi
    fi
fi

if test -n "$PERL"; then
  ac_cv_perl="$PERL"
  AC_CACHE_VAL([ac_cv_perl],$PERL)
else
  ac_cv_perl=perl
fi
AC_PATH_PROG([PERL], [$ac_cv_perl])
if test -n "$PERL"; then
  AC_MSG_CHECKING([PERL version])
  [PERL_VERSION=`"$PERL" -e'print $^V' 2>&1`]
  AC_MSG_RESULT([$PERL_VERSION])
  AM_RUN_LOG([$PERL -V:cc])
fi

AM_CONDITIONAL([HAVE_SWIG_PYTHON], [test -n "$SWIG" && test -n "$PYTHON_VERSION"])
AM_CONDITIONAL([HAVE_PYTHON], [test x$enable_python != xno])
AM_CONDITIONAL([HAVE_SWIG_PERL], [test -n "$SWIG" && test -n "$PERL_VERSION"])
AM_CONDITIONAL([HAVE_PERL], [test -n "$PERL"])
dnl only needed with python bindings
AM_CONDITIONAL([HAVE_PATCH], [test -n "$PATCH"])
dnl AC_MSG_WARN([srcdir: $srcdir ac_builddir: $ac_builddir builddir: $builddir])
AM_CONDITIONAL([HAVE_INSRCDIR], [test x"$srcdir" = x"."])

AC_CHECK_PROGS([PICAT], [picat])
if test -n "$PICAT"; then
  AC_MSG_CHECKING([PICAT version])
  [picat_version=`"$PICAT" --version 2>&1 | grep 'Picat version' | cut -c15-`]
  AC_MSG_RESULT([$picat_version])
fi
AM_CONDITIONAL([HAVE_PICAT], [test -n "$PICAT"])

dnl GNU parallel, skip the old non-perl version from moreutils so far
AC_CHECK_PROGS([PARALLEL], [parallel])
logs_all=logs-all-serial.sh.in
if test -n "$PARALLEL"; then
  AC_MSG_CHECKING([PARALLEL version])
  parallel_version=`"$PARALLEL" --version 2>&1 | head -n1 | cut -c14-`
  case $parallel_version in
    20* )   logs_all=logs-all-parallel.sh.in
            ;;
    *invalid* )
            PARALLEL=
            parallel_version="skip old moreutils version, need GNU parallel"
            #TODO
            #logs_all=logs-all-parallel-old.sh.in
  esac
  AC_MSG_RESULT([$parallel_version])
fi
AM_CONDITIONAL([HAVE_PARALLEL], [test -n "$PARALLEL"])

dnl coreutils timeout
AC_CHECK_PROGS([TIMEOUT], [timeout])
if test -n "$TIMEOUT"; then
  AC_MSG_CHECKING([TIMEOUT version])
  [timeout_version=`"$TIMEOUT" --version 2>&1 | grep '^timeout ' | sed 's/.*\([0-9][0-9]*\.[0-9][0-9]*\).*/\1/g'`]
  AC_MSG_RESULT([$timeout_version])
  TIMEOUT_10="$TIMEOUT 10"
  TIMEOUT_30="$TIMEOUT 30"
fi
AM_CONDITIONAL([HAVE_TIMEOUT], [test -n "$TIMEOUT"])
AC_SUBST([TIMEOUT_10])
AC_SUBST([TIMEOUT_30])

dnl build-aux/missing catches an outdated /usr/bin/makeinfo 4.8 on darwin 10.11
dnl the texi2any version >6 is needed for @indentedblock
case $host_os in
  darwin*)
    if test -f /opt/local/bin/makeinfo ; then
      MAKEINFO=/opt/local/bin/makeinfo
    else
      AC_CHECK_PROGS([MAKEINFO], [makeinfo])
    fi
    ;;
  *) AC_CHECK_PROGS([MAKEINFO], [makeinfo])
    ;;
esac
if test -n "$MAKEINFO"; then
  AC_MSG_CHECKING([MAKEINFO version])
  dnl either old makeinfo or new texi2any (preferred)
  [makeinfo_version=`"$MAKEINFO" --version 2>&1 | $GREP 'texinfo' | cut -d' ' -f4`]
  AC_SUBST([makeinfo_version])
  AC_MSG_RESULT([$makeinfo_version])
fi
AM_CONDITIONAL([HAVE_MAKEINFO], [test -n "$MAKEINFO"])

if test x$enable_debug = xyes; then
   JSON_SED_NAN="\$SED -i -e's/ nan/ \"nan\"/g;'"
else
   JSON_SED_NAN="true"
fi
AC_SUBST([JSON_SED_NAN])

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -n "$DOXYGEN"; then
  AC_MSG_CHECKING([DOXYGEN version])
  [doxygen_version=`"$DOXYGEN" --version 2>&1`]
  AC_MSG_RESULT([$doxygen_version])
fi
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])

AC_CHECK_PROGS([JQ], [jq])
if test -n "$JQ"; then
  AC_MSG_CHECKING([JQ version])
  [jq_version=`"$JQ" --version 2>&1 | grep 'jq-' | cut -c4-`]
  AC_MSG_RESULT([$jq_version])
fi
AM_CONDITIONAL([HAVE_JQ], [test -n "$JQ"])

case $host_os in
  darwin*)  DSYMUTIL=dsymutil ;;
  mingw*)   DSYMUTIL=rem ;;
  windows*) DSYMUTIL=rem ;;
  *) DSYMUTIL=: ;;
esac
AC_SUBST([DSYMUTIL])

dnl help2man only by the maintainer

if test x$cross_compiling = xno; then
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
  if test -n "$VALGRIND"; then
     if test -n "$VALGRIND"; then
       AC_MSG_CHECKING([VALGRIND version])
       [valgrind_version=`"$VALGRIND" --version 2>&1 | sed 's,valgrind-,,'`]
       AC_MSG_RESULT([$valgrind_version])
     fi
     AC_MSG_CHECKING([for Valgrind suppressions])
     case $host_os in
       linux*)  VALGRIND_SUPPRESSIONS_FILE=valgrind-linux.supp ;;
       darwin*) VALGRIND_SUPPRESSIONS_FILE=valgrind-darwin.supp ;;
     esac
     if test -n $VALGRIND_SUPPRESSIONS_FILE; then
       VALGRIND_SUPPRESSIONS="--suppressions=\$(abs_top_srcdir)/\$(VALGRIND_SUPPRESSIONS_FILE)"
       AC_MSG_RESULT([$VALGRIND_SUPPRESSIONS_FILE])
     else
       AC_MSG_RESULT([none])
     fi
   fi
else
  AC_MSG_WARN([disabled valgrind: cross-compiled])
  enable_valgrind=no
  AX_VALGRIND_CHECK()
fi
AC_SUBST(VALGRIND_SUPPRESSIONS)
AC_SUBST(VALGRIND_SUPPRESSIONS_FILE)

dnl Write it out
AC_CONFIG_FILES([
	Makefile
	src/Makefile
	doc/Makefile
	programs/Makefile
	examples/Makefile
	bindings/Makefile
	bindings/python/Makefile
	test/Makefile
	test/xmlsuite/Makefile
	test/unit-testing/Makefile
	libredwg.pc
])

AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([doc/Doxyfile])])
AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([doc/footer])])
AM_COND_IF([HAVE_PERL], [AC_CONFIG_FILES([bindings/perl/Makefile.PL])])
AC_CONFIG_FILES([programs/dwgfilter], [chmod +x programs/dwgfilter])
AC_CONFIG_FILES([programs/alive.test], [chmod +x programs/alive.test])
AC_CONFIG_FILES([programs/json.test], [chmod +x programs/json.test])
AC_CONFIG_FILES([logs-all.sh:$logs_all], [chmod +x logs-all.sh])
AC_CONFIG_FILES([log], [chmod +x log])
AC_CONFIG_FILES([dwg], [chmod +x dwg])
AC_CONFIG_FILES([dxf], [chmod +x dxf])
AC_CONFIG_FILES([json], [chmod +x json])
AC_CONFIG_FILES([dynapi_test_all.sh], [chmod +x dynapi_test_all.sh])

AC_OUTPUT

dnl configure.ac ends here
