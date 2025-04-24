dnl configure.ac

dnl Copyright (C) 2010, 2014-2015, 2018-2025 Free Software Foundation, Inc.
dnl
dnl This program is free software, licensed under the terms of the GNU
dnl General Public License as published by the Free Software Foundation,
dnl either version 3 of the License, or (at your option) any later version.
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.

AC_PREREQ([2.69])
AC_INIT([LibreDWG],
        [m4_esyscmd(build-aux/git-version-gen .tarball-version)],
        [libredwg@gnu.org],[],
        [https://savannah.gnu.org/projects/libredwg/])
AC_CONFIG_AUX_DIR([build-aux])
dnl for older systems use this:
dnl AM_INIT_AUTOMAKE([1.11.6 gnu no-define])
dnl parallel-tests:
dnl: error: using '$(srcdir)' in TESTS is currently broken: '$(srcdir)/libredwg.spec'
AM_INIT_AUTOMAKE([1.14 gnu no-define serial-tests dist-xz info-in-builddir])
LT_INIT

# Keep this in sync with what is passed in AC_INIT
TARBALL_VERSION_FILE=".tarball-version"
dnl Define these substitutions here to keep all version information in
dnl one place. For information on how to properly maintain the library
dnl version information, refer to the libtool manual, section "Updating
dnl library version information":
dnl http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
dnl Stay at 0 until we leave the beta stage.
dnl- If the library source code has changed at all since the last update,
dnl  then increment revision (‘c:r:a’ becomes ‘c:r+1:a’).
dnl- If any interfaces have been added, removed, or changed since the last update,
dnl  increment current, and set revision to 0.
dnl- If any interfaces have been added since the last public release,
dnl  then increment age.
dnl- If any interfaces have been removed or changed since the last public release,
dnl  then set age to 0.
LIBREDWG_SO_VERSION=0:13:0
AC_DEFINE_UNQUOTED([LIBREDWG_SO_VERSION], ["${LIBREDWG_SO_VERSION}"],
          [Defined in configure.ac to the libtool version-info of the shared lib.])
AC_SUBST([LIBREDWG_SO_VERSION])
AC_SUBST([TARBALL_VERSION_FILE])

AC_CONFIG_MACRO_DIR([m4])

# Check if automake supports 'pretty' builds, if so enable them by default
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

dnl create a config.h file (Automake will add -DHAVE_CONFIG_H)
AC_CONFIG_HEADERS([src/config.h])
AC_MSG_CHECKING([PACKAGE_VERSION])
AC_MSG_RESULT($PACKAGE_VERSION)
AC_MSG_CHECKING([LIBREDWG_SO_VERSION])
AC_MSG_RESULT($LIBREDWG_SO_VERSION)

ISODATE=`date +%Y-%m-%d`
AC_SUBST(ISODATE)

AC_CANONICAL_HOST

dnl Checks for programs
PKG_PROG_PKG_CONFIG
dnl AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_EGREP
AC_CHECK_PROGS([BASH], [bash])

if test x$enable_shared = xyes; then
   AC_DEFINE([ENABLE_SHARED],1,[Define to 1 if a shared library will be built])
   LTEXEC="\${top_builddir}/libtool --mode=execute"
fi
AM_CONDITIONAL([ENABLE_SHARED], [test "x$enable_shared" = xyes])
AC_SUBST(LTEXEC)

AC_C_INLINE
AC_C_RESTRICT

dnl Checks for libraries
AM_ICONV

AC_CHECK_LIB([ps -lm],[PS_open_file],
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
AC_CHECK_LIB([xml2],[newXMLEntity],
  [have_libxml2=yes;
   XML2_LIBS=-lxml2;
   AC_SUBST([XML2_CFLAGS])
   AC_SUBST([XML2_LIBS])
   ],
  [if pkg-config --exists libxml-2.0; then
    have_libxml2=yes;
    XML2_CFLAGS="`pkg-config libxml-2.0 --cflags`"
    XML2_LIBS="`pkg-config libxml-2.0 --libs`"
  else
    AC_MSG_WARN([libxml-2.0 for test/xmlsuite not found])
  fi])
AM_CONDITIONAL([HAVE_LIBXML2], [test x$have_libxml2 = xyes])

dnl Checks for header files
dnl 2.71 only: AC_CHECK_INCLUDES_DEFAULT
AC_HEADER_STDC

dnl Avoid rpl_malloc when cross-compiling. we don't need malloc.h
dnl We just want to know if it's needed
ac_cv_func_malloc_0_nonnull=yes
ac_cv_func_realloc_0_nonnull=yes
AC_CHECK_HEADERS([stdlib.h string.h stddef.h unistd.h malloc.h wchar.h ctype.h])
AC_CHECK_HEADERS([wctype.h float.h limits.h sys/param.h direct.h libgen.h iconv.h sys/time.h])
AC_CHECK_HEADERS([getopt.h alloca.h valgrind/valgrind.h])
AC_CHECK_HEADERS([endian.h sys/endian.h machine/endian.h winsock2.h], [break])
AC_CHECK_HEADERS([byteswap.h byteorder.h sys/byteorder.h], [break])
dnl optionally use a better allocator with --enable-mimalloc
AC_CHECK_HEADERS([mimalloc-override.h])
AC_C_BIGENDIAN
AC_CHECK_HEADER_STDBOOL
AX_INCLUDE_STRCASECMP

# test for correct byteswap.h
if test x$ac_cv_header_endian_h = xyes ; then
   m4_foreach([FUNC], [le16toh,le32toh,le64toh,htole32,htole64,htobe16,htobe32,htobe64,be64toh], [
     AC_MSG_CHECKING([if FUNC is available in endian.h])
     AC_LINK_IFELSE([AC_LANG_SOURCE([
#include <endian.h>
int
main(void)
{
	FUNC[](42);
	return 0;
}
        ])], [AC_DEFINE(HAVE_[]m4_toupper(FUNC), [1],
                [Define to 1 if] FUNC [is available in <endian.h>.])
              AC_MSG_RESULT([yes])
        ], [AC_MSG_RESULT([no])])
        ])dnl
fi

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
case $CFLAGS in
     *-fsanitize=undefined*)   ax_cv_have_aligned_access_required=yes  ;;
     *-fsanitize=*,undefined*) ax_cv_have_aligned_access_required=yes  ;;
esac
case $CC in
     *-fsanitize=undefined*)   ax_cv_have_aligned_access_required=yes  ;;
     *-fsanitize=*,undefined*) ax_cv_have_aligned_access_required=yes  ;;
esac
AX_CHECK_ALIGNED_ACCESS_REQUIRED

case $CC in
     *-fsanitize=address*)     HAVE_ASAN=yes ;;
     *-fsanitize=*,address*)   HAVE_ASAN=yes ;;
esac
case $CC in
     *-fsanitize=leak*)        HAVE_ASAN=yes ;;
     *-fsanitize=*,leak*)      HAVE_ASAN=yes ;;
esac
case $CFLAGS in
     *-fsanitize=address*)     HAVE_ASAN=yes ;;
     *-fsanitize=*,address*)   HAVE_ASAN=yes ;;
esac
case $CFLAGS in
     *-fsanitize=leak*)        HAVE_ASAN=yes ;;
     *-fsanitize=*,leak*)      HAVE_ASAN=yes ;;
esac
AC_SUBST(HAVE_ASAN)
AM_CONDITIONAL([HAVE_ASAN], [test -n "$HAVE_ASAN"])

AC_TYPE_SIZE_T
AC_TYPE_INT16_T
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
case "$host_os" in
darwin*|macos*)
    AC_DEFINE([_POSIX_C_SOURCE],[900000L],[Needed for all APPLE extensions])
    ;;
*)
    AC_DEFINE([_POSIX_C_SOURCE],[200809L],[Needed for strdup])
    ;;
esac
case "$host_os" in
cygwin*) AC_DEFINE([__XSI_VISIBLE],[700],[Needed for cygwin strdup]) ;;
esac

AX_IS_RELEASE([git-directory])
dnl ability to override with a .git dir, to test release features
AC_ARG_ENABLE([release],AS_HELP_STRING([--enable-release],[
    Override IS_RELEASE with .git dir present (default: $ax_is_release).]),
    [ax_is_release=yes
    AC_MSG_RESULT(IS_RELEASE override with --enable-release)])
if test x$ax_is_release = xyes; then
   if test x$have_libxml2 != xyes; then
     AC_MSG_WARN([libxml-2.0 for test/xmlsuite missing])
   fi
   if test x$libredwg_have_libps != xyes; then
     AC_MSG_WARN([pslib for dwg2ps missing with release])
   fi
   AC_DEFINE([IS_RELEASE],1,[Define to 1 if this is a release,
skipping unstable DWG features, unknown DWG versions and objects.])
fi
AM_CONDITIONAL([IS_RELEASE], [test x$ax_is_release = xyes])

dnl Detect emscripten
if test `basename "$CC"` = emcc; then
  ac_cv_prog_cc_emcc=yes
else
  ac_cv_prog_cc_emcc=no
fi
dnl Disable unwanted warnings here
enable_Werror=no
ax_enable_compile_warnings=yes
dnl This is too intrusive for our DWG_TYPE_* switches
ax_cv_check_cflags__Wswitch_enum=no
dnl wasm does not support -fstack-protector yet
if test x$enable_shared = xyes && \
   test x$ac_compiler_gnu = xyes && \
   test x$ac_cv_prog_cc_emcc != xyes
then
  AX_COMPILER_FLAGS([WARN_CFLAGS],[AM_LDFLAGS],[$ax_is_release],
    [-fno-semantic-interposition -fwrapv -fno-common -fvisibility=hidden \
     -fno-strict-aliasing -ftrivial-auto-var-init=zero -fstrict-flex-arrays=3 \
     -fstack-protector-strong -fstack-clash-protection \
     -fcf-protection=full -fno-delete-null-pointer-checks \
     -Wno-alloc-size])
else
  AX_COMPILER_FLAGS([WARN_CFLAGS],[AM_LDFLAGS],[$ax_is_release],
    [-fwrapv -fno-common -fvisibility=hidden -fno-strict-aliasing \
     -fno-delete-null-pointer-checks -Wno-alloc-size])
  if test x$ac_cv_prog_cc_emcc = xyes; then
    enable_werror=no
    dnl AM_LDFLAGS="-s ALLOW_MEMORY_GROWTH=1"
    AM_LDFLAGS=""
    CFLAGS="$CFLAGS -Wno-limited-postlink-optimizations -Wno-cast-align"
  fi
fi
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
AX_GCC_FUNC_ATTRIBUTE(noreturn)
AX_GCC_FUNC_ATTRIBUTE(aligned)
AX_GCC_FUNC_ATTRIBUTE(counted_by)
if test x$ax_cv_check_cflags__Wformat_y2k = xyes || \
   test x$ax_cv_check_cflags__Wformat_2__Wformat_y2k = xyes
then
  AC_DEFINE(HAVE_WFORMAT_Y2K, 1, [Define if -Wformat-y2k is supported.])
fi
AX_PRINTF_SIZE_T

dnl until we can make a release, favor no -O2 optims.
dnl it's fast enough and better for debugging.
dnl if test x$ac_compiler_gnu = xyes; then
dnl   CFLAGS="$CFLAGS -fno-omit-frame-pointer"
dnl fi

if test "x$ac_cv_prog_cc_c11" = "xno"; then
   AC_MSG_WARN([This compiler ${CC} has no C11 support])
   if test "x$ac_cv_prog_cc_c99" != "xno"; then
     AC_DEFINE([HAVE_C99], 1,
        [Defined to 1 when the compiler supports C99, mostly (...) macros])
     AC_MSG_RESULT([added $ac_cv_prog_cc_c99 to CFLAGS])
     CFLAGS="$CFLAGS $ac_cv_prog_cc_c99"
   fi
else
   AC_DEFINE([HAVE_C11], 1,
        [Defined to 1 when the compiler supports C11])
   if test "x$ac_cv_prog_cc_c99" = "xno"; then
     AC_MSG_WARN([This compiler ${CC} has no C99 support])
   else
     AC_DEFINE([HAVE_C99], 1,
        [Defined to 1 when the compiler supports C99, mostly (...) macros])
   fi
   if test "x$ac_cv_prog_cc_c11" != "x"; then
     AC_MSG_RESULT([added $ac_cv_prog_cc_c11 to CFLAGS])
     CFLAGS="$CFLAGS $ac_cv_prog_cc_c11"
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
AC_CHECK_FUNCS([scandir],[],
  AC_MSG_WARN([BSD/POSIX scandir not found. No helper unit_testing_all.sh]))
AC_CHECK_FUNCS([floor sqrt])
AC_CHECK_FUNCS([gettimeofday])
AC_CHECK_LIB([m],[floor])
#AC_CHECK_LIB([m],[sqrt])
AC_CHECK_FUNCS([sincos])
AC_CHECK_FUNCS([memchr memmove strcasecmp strchr strstr strrchr strtol strtoll strtoul strtoull strnlen atoi])
AC_CHECK_FUNCS([memmem],[],
  AC_MSG_WARN([memmem not found. Using a slower workaround.]))
AC_CHECK_FUNCS([gmtime_r])
AC_CHECK_FUNCS([wcslen wcsnlen wcscpy wcscmp wcsstr])
dnl try the secure extensions
AS_ECHO(["#define __STDC_WANT_LIB_EXT1__  1"]) >>confdefs.h
AC_CHECK_FUNCS([sscanf_s],[],
  AC_MSG_WARN([No secure sscanf_s found. dwgfuzz -add will produce false positives.]))
dnl and undo it
$GREP -v "__STDC_WANT_LIB_EXT1__" confdefs.h > confdefs.h.tmp
mv confdefs.h.tmp confdefs.h

dnl mingw-w64 7.0 bug with _FORTIFY_SOURCE, need -lssp
case $LD in
  *-w64-mingw32*)
   AC_MSG_CHECKING([-lssp hack for mingw])
   case $CFLAGS in
     *-fstack-protector*)
        AC_MSG_RESULT([yes])
        ;;
     *) CFLAGS="$CFLAGS -fstack-protector"
        AC_MSG_RESULT([-fstack-protector added])
        ;;
   esac
   ;;
esac
dnl x86_64-mingw-w64-gcc-9.2 is seriously broken, and not just mingw-w64

dnl --enable-debug sets DEBUG_CLASSES, will crash acad import
AC_MSG_CHECKING([for --enable-debug])
AC_ARG_ENABLE([debug],AS_HELP_STRING([--enable-debug],[
    Enable unstable DEBUG_CLASSES (default: no).]),
  [if test x$ac_compiler_gnu = xyes; then
    CFLAGS="`echo $CFLAGS|sed 's,-O2,,'` -fno-omit-frame-pointer"
  fi
  AC_DEFINE([DEBUG_CLASSES],1,[Define to enable unstable classes.])
  AC_SUBST(DEBUG_CLASSES, 1)
  AC_MSG_RESULT([yes])],
  [AC_MSG_RESULT([no (default)])
  AC_SUBST(DEBUG_CLASSES, 0)])
AM_CONDITIONAL([ENABLE_DEBUG], [test x$enable_debug = xyes])

AX_ADD_FORTIFY_SOURCE
AX_C_RESTRICT

dnl Feature: --disable-write
AC_MSG_CHECKING([for --disable-write])
AC_ARG_ENABLE([write],AS_HELP_STRING([--disable-write],[
    Disable write support (default: no).]),[],[enable_write=yes])
AM_CONDITIONAL([USE_WRITE], [test x$enable_write != xno])
AS_IF([test x$enable_write != xno],
  [AC_DEFINE([USE_WRITE],1,[Undefine to disable write support.])
   AC_SUBST(DISABLE_WRITE, 0)
   AC_MSG_RESULT([no (default)])],
  [AC_SUBST(DISABLE_WRITE, 1)
   AC_MSG_RESULT([yes])])

dnl Feature: --enable-trace
AC_MSG_CHECKING([for --enable-trace])
AC_ARG_ENABLE([trace],AS_HELP_STRING([--enable-trace],[
    Enable runtime tracing (default: no).  When enabled, the environment
    variable LIBREDWG_TRACE is consulted on the first decode/encode attempt.
    Its value is an integer: 0 (no output) through 9 (full verbosity).]),
  AC_DEFINE([USE_TRACING],1,[Define to 1 to enable runtime tracing support.])
  AC_MSG_RESULT([yes]),
  AC_MSG_RESULT([no (default)]))

dnl check for --with-dxf-precision=rfc or max or num
AC_MSG_CHECKING([for --with-dxf-precision])
AC_ARG_WITH([dxf-precision],
    AS_HELP_STRING([--with-dxf-precision=],
        [How many digits in the dxf. The recommended low-precision 6 or some number or @<:@default=max@:>@]),
	[case "${with_dxf_precision}" in
	 max|16) with_dxf_precision=16; AC_MSG_RESULT([max]) ;;
	 rfc|6)  with_dxf_precision=6;  AC_MSG_RESULT([rfc]) ;;
	 [6-9]|1[0-6]) AC_MSG_RESULT([${with_dxf_precision}]) ;;
	 *)      AC_MSG_RESULT([invalid ${with_dxf_precision}: rfc,max or some number]) ;;
	 esac],
        [with_dxf_precision=16; AC_MSG_RESULT([max (default)]) ])
AC_DEFINE_UNQUOTED([DXF_PRECISION],[${with_dxf_precision}],[Number of dxf after-comma places (default 16).])

dnl check for --with-geojson-precision=rfc or max or num
AC_MSG_CHECKING([for --with-geojson-precision])
AC_ARG_WITH([geojson-precision],
    AS_HELP_STRING([--with-geojson-precision=],
        [How many digits in the geojson. The RFC7946 recommended 6 or some number or @<:@default=max@:>@]),
	[case "${with_geojson_precision}" in
	 max|16) with_geojson_precision=16; AC_MSG_RESULT([max]) ;;
	 rfc|6)  with_geojson_precision=6;  AC_MSG_RESULT([rfc]) ;;
	 [6-9]|1[0-6]) AC_MSG_RESULT([${with_geojson_precision}]) ;;
	 *)      AC_MSG_RESULT([invalid ${with_geojson_precision}: rfc,max or some number]) ;;
	 esac],
        [with_geojson_precision=16; AC_MSG_RESULT([max (default)]) ])
AC_DEFINE_UNQUOTED([GEOJSON_PRECISION],[${with_geojson_precision}],[Number of geojson after-comma places (recommended 6 by RFC).])

AC_MSG_CHECKING([for --disable-werror])
AC_ARG_ENABLE([werror],AS_HELP_STRING([--disable-werror],[
    Disable -Werror (default: no).]),[],[enable_werror=yes])
AS_IF([test x$enable_werror != xno],
  [AC_DEFINE([HAVE_WERROR],1,[If -Werror is enabled.])
   WARN_CFLAGS="-Werror $WARN_CFLAGS"
   if test x$ax_cv_check_cflags__Wcast_align = xyes; then
      WARN_CFLAGS="$WARN_CFLAGS -Wno-cast-align"
   fi
   AC_MSG_RESULT([no (default)])],
  [AC_MSG_RESULT([yes])])

AC_CHECK_FUNCS([setenv],[],
  AS_IF([test x$enable_trace = xyes],
    [AC_MSG_WARN([setenv not found. --enable-trace is limited with cmdline apps.
      Only -v args are taken for verbosity not the LIBREDWG_TRACE env var.])],
    [AC_MSG_RESULT([setenv not needed. no --enable-trace.])]
    ))

dnl --disable-dxf only useful for faster debug/test cycles
AC_MSG_CHECKING([for --disable-dxf])
AC_ARG_ENABLE([dxf],AS_HELP_STRING([--disable-dxf],[
    Disable DXF, JSON and other in/out modules (default: no).]),[],[enable_dxf=yes])
AM_CONDITIONAL([DISABLE_DXF], [test x$enable_dxf = xno])
AS_IF([test x$enable_dxf = xno],
  AC_DEFINE([DISABLE_DXF],1,[Define to disable DXF, JSON and other in/out modules.])
  AC_MSG_RESULT([yes]),
  AC_MSG_RESULT([no (default)]))

dnl --disable-json only useful for slim libraries (i.e. SolveSpace)
AC_MSG_CHECKING([for --disable-json])
AC_ARG_ENABLE([json],AS_HELP_STRING([--disable-json],[
    Disable JSON and GeoJSON in/out modules (default: no).]),[],[enable_json=yes])
AM_CONDITIONAL([DISABLE_JSON], [test x$enable_json = xno])
AS_IF([test x$enable_json = xno],
  AC_DEFINE([DISABLE_JSON],1,[Define to disable JSON and GeoJSON in/out modules.])
  AC_SUBST(DISABLE_JSON, 1)
  AC_MSG_RESULT([yes]),
  AC_MSG_RESULT([no (default)]))

dnl --disable-bindings
AC_MSG_CHECKING([for --disable-bindings])
AC_ARG_ENABLE([bindings],AS_HELP_STRING([--disable-bindings],[
    Disable SWIG bindings (default: no).]),[],[enable_bindings=yes])
AM_CONDITIONAL([DISABLE_BINDINGS], [test x$enable_bindings = xno])
AS_IF([test x$enable_bindings = xno],
  AC_DEFINE([DISABLE_BINDINGS],1,[Define to disable bindings.])
  AC_MSG_RESULT([yes]),
  AC_MSG_RESULT([no (default)]))

dnl --disable-docs
AC_MSG_CHECKING([for --disable-docs])
AC_ARG_ENABLE([docs],AS_HELP_STRING([--disable-docs],[
    Disable docs generation, texlive is big (default: enabled).]),[],[enable_docs=yes])
AM_CONDITIONAL([DISABLE_DOCS], [test x$enable_docs = xno])
AS_IF([test x$enable_docs = xno],
  AC_MSG_RESULT([yes]),
  AC_MSG_RESULT([no (default)]))

dnl --enable-check-less
AC_MSG_CHECKING([for --enable-check-less])
AC_ARG_ENABLE([check-less],AS_HELP_STRING([--enable-check-less],[
    Enable check less unit-tests (default: no).  When enabled (e.g. in mingw),
    the unit-tests include only the minimal API tests.]),
  [check_less=yes
  AC_MSG_RESULT([yes])],
  AC_MSG_RESULT([no (default)]))
AM_CONDITIONAL([CHECK_LESS], [test x$check_less = xyes])


AC_MSG_CHECKING([for --with-mimalloc])
if test "x$ac_cv_header_mimalloc_override_h" = "xyes"
then
  AC_ARG_WITH([mimalloc],AS_HELP_STRING([--with-mimalloc],[
    With mimalloc.]),
    [with_mimalloc=yes],[with_mimalloc=no])
  if test x$with_mimalloc = xyes; then
    AC_DEFINE([ENABLE_MIMALLOC],1,[Define to 1 if mimalloc-override.h should be used.])
    LIBS="$LIBS -lmimalloc"
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
else
  AC_MSG_RESULT(no mimalloc headers found)
fi
AM_CONDITIONAL([ENABLE_MIMALLOC], [test x$with_mimalloc = xyes])

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
	GCOV_LIBS="-lgcov"
	AC_SUBST(GCOV_LIBS)
else
	AC_MSG_RESULT([no (default)])
fi
AM_CONDITIONAL(ENABLE_GCOV, test "x$enable_gcov" = "xtrue")

AC_CHECK_PROGS([GPERF], [gperf])
if test -n "$GPERF"
then
    AC_MSG_CHECKING([GPERF version])
    [GPERF_VERSION=`"$GPERF" --version | head -n1 2>&1 | sed 's,GNU gperf ,,'`]
    GPERF_MINOR="`echo $GPERF_VERSION | cut -f2 -d.`"
    case $GPERF_VERSION in
      1.*) GPERF_VERSION=$((100+$GPERF_MINOR)) ;;
      2.*) GPERF_VERSION=$((200+$GPERF_MINOR)) ;;
      3.*) GPERF_VERSION=$((300+$GPERF_MINOR)) ;;
      4.*) GPERF_VERSION=$((400+$GPERF_MINOR)) ;;
    esac
    AC_MSG_RESULT([$GPERF_VERSION])
    AC_DEFINE_UNQUOTED([GPERF_VERSION],[$GPERF_VERSION],
       [versions earlier than 301 will have no size_t])
    AC_MSG_CHECKING([GPERF API])
    GPERF_API=$(echo 1 | gperf | grep in_word_set)
    AC_MSG_RESULT([$GPERF_API])
    if echo "$GPERF_API" | grep size_t >/dev/null; then
      HAVE_GPERF_SIZE_T=yes
      AC_DEFINE_UNQUOTED([HAVE_GPERF_SIZE_T],[$HAVE_GPERF_SIZE_T],
       [macOS 12.6.5 lies about its gperf version, using size_t as 3.0])
    fi
fi
AC_SUBST(GPERF)
AC_SUBST(GPERF_VERSION)
AM_CONDITIONAL(HAVE_GPERF, test -n "$GPERF")

AC_MSG_CHECKING([for latest clang-format])
CLANG_FORMAT=
for clangfmt in clang-format{,-mp}{,-devel,-{17,16,15,14,13,12,11,10,9,8,7,6,5,4,3}{,.9,.8,.7,.6,.5,.4,.3,.2,.1,.0}}
do
    if which "$clangfmt" &>/dev/null; then
        CLANG_FORMAT="$clangfmt"
        break
    fi
done
if test -z "$CLANG_FORMAT"; then
    if which indent &>/dev/null; then
        CLANG_FORMAT="indent"
    else
        CLANG_FORMAT=echo
    fi
fi
AC_MSG_RESULT($CLANG_FORMAT)
AC_SUBST(CLANG_FORMAT)

AC_MSG_CHECKING([for latest clang-tidy])
CLANG_TIDY=
for clangtidy in clang-format{,-mp}{,-devel,-{17,16,15,14,13,12,11,10,9,8,7,6,5,4,3}{,.9,.8,.7,.6,.5,.4,.3,.2,.1,.0}}
do
    if which "$clangtidy" &>/dev/null; then
        CLANG_TIDY="$clangtidy"
        break
    fi
done
if test -z "$CLANG_TIDY"; then
    CLANG_TIDY=echo
fi
AC_MSG_RESULT($CLANG_TIDY)
AC_SUBST(CLANG_TIDY)

dnl Check for mingw, cross and wine
AM_CONDITIONAL([HAVE_MINGW], [test "x$host_os" = "xmingw32" -o "x$host_os" = "xmingw64"])
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
AM_CONDITIONAL([HAVE_CYGWIN], [test x$host_os = xcygwin32 -o x$host_os" = "xcygwin64])
# qemu test environment, debian style (redhat sys-root's are mostly broken)
if test "$cross_compiling" = "yes" -a -n "$host_alias" -a -z "$WINE" \
        -a -z "$AM_TESTS_ENVIRONMENT" \
        -a "$enable_shared" = "yes" -a -d "/usr/$host_alias/lib" \
        -a -e "/usr/$host_alias/lib/libc.so"
then
    AC_MSG_CHECKING([AM_TESTS_ENVIRONMENT])
    AM_TESTS_ENVIRONMENT="env $shlibpath_var=/usr/$host_alias/lib"
    AC_SUBST([AM_TESTS_ENVIRONMENT])
    AC_MSG_RESULT([env $shlibpath_var=/usr/$host_alias/lib])
fi
if test -n "$HAVE_ASAN"
then
        HAVE_ASAN_OR_LINUX=1
elif test x$host_os = xlinux-gnu
then
        HAVE_ASAN_OR_LINUX=1
fi
AM_CONDITIONAL([HAVE_ASAN_OR_LINUX],[test -n "$HAVE_ASAN_OR_LINUX"])

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
      AX_SWIG_PYTHON
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

if test x$enable_python != xno && test -n "$XML2_LIBS" && test x$cross_compiling = xno
then
  AC_MSG_CHECKING([for the libxml2 Python package])
  ac_python_libxml2_result=`$PYTHON -c "import libxml2" 2>&1`
  if test $? -eq 0; then
    ac_have_python_libxml2=1
    AC_MSG_RESULT([yes])
  else
    ac_have_python_libxml2=
    AC_MSG_RESULT([no])
    AC_MSG_WARN([disable test/xmlsuite.
The import error was:
$ac_python_libxml2_result])
  fi
fi
AM_CONDITIONAL([HAVE_PYTHON_LIBXML2], [test x$ac_have_python_libxml2 = x1])

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

  dnl --enable-perl-install=vendor. default: site
  AS_IF([test x$enable_bindings = xyes],
    [AC_MSG_CHECKING([for --with-perl-install=])
     AC_ARG_WITH(perl-install,
	AS_HELP_STRING([--with-perl-install=],
          [Where to install the perl bindings, vendor,arch,yes,no or @<:@default=site@:>@]),
	[case "${with_perl_install}" in
	 vendor)   with_perl_install=vendor; AC_MSG_RESULT([vendor]) ;;
	 yes|site) with_perl_install=site;   AC_MSG_RESULT([site]) ;;
	 perl)     with_perl_install=perl;   AC_MSG_RESULT([perl]) ;;
	 no)       with_perl_install=no;     AC_MSG_RESULT([no]) ;;
	 *)        AC_MSG_RESULT([invalid ${with_perl_install}: vendor,site,yes,perl,no]) ;;
	 esac],
        [with_perl_install=site; AC_MSG_RESULT([site (default)]) ])
    ]
  )
fi
AC_SUBST([with_perl_install])

AM_CONDITIONAL([HAVE_SWIG_PYTHON], [test -n "$SWIG" && test -n "$PYTHON_VERSION"])
AM_CONDITIONAL([HAVE_PYTHON], [test x$enable_python != xno])
AM_CONDITIONAL([HAVE_SWIG_PERL], [test -n "$SWIG" && test -n "$PERL_VERSION"])
AM_CONDITIONAL([HAVE_PERL], [test -n "$PERL"])
dnl only needed with python bindings
AM_CONDITIONAL([HAVE_PATCH], [test -n "$PATCH"])
dnl AC_MSG_WARN([srcdir: $srcdir ac_builddir: $ac_builddir builddir: $builddir])
AM_CONDITIONAL([HAVE_INSRCDIR], [test x"$srcdir" = x"."])

AC_CHECK_PROGS([ODAFILECONVERTER], [ODAFileConverter])
if test -z "$ODAFILECONVERTER"; then
  AC_CHECK_PROGS([ODAFILECONVERTER], [TeighaFileConverter])
fi
AM_CONDITIONAL([HAVE_ODA], [test -n "$ODAFILECONVERTER"])

AC_CHECK_PROGS([CPPCHECK], [cppcheck])
if test -n "$CPPCHECK"; then
  AC_MSG_CHECKING([CPPCHECK version])
  [cppcheck_version=`"$CPPCHECK" --version 2>&1 | grep 'Cppcheck ' | cut -c10-`]
  AC_MSG_RESULT([$cppcheck_version])
fi
AM_CONDITIONAL([HAVE_CPPCHECK], [test -n "$CPPCHECK"])

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
AC_SUBST([logs_all])
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

AC_MSG_CHECKING([for $srcdir/../ODA-spec])
if test -e "$srcdir/../ODA-spec/ODA-5.4.2-libredwg.patch"; then
  have_oda_spec=yes
  AC_MSG_RESULT([yes])
else
  have_oda_spec=
  AC_MSG_RESULT([no])
fi
AM_CONDITIONAL([HAVE_ODA_SPEC], [test -n "$have_oda_spec"])

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

DSYMUTIL=
case $host_os in
  darwin*)  DSYMUTIL=dsymutil ;;
esac
AC_SUBST([DSYMUTIL])

AC_CHECK_PROGS([SHELLCHECK], [shellcheck])
AM_CONDITIONAL([HAVE_SHELLCHECK], [test -n "$SHELLCHECK"])
AC_CHECK_PROGS([SHFMT], [shfmt])
dnl if test -n "$SHFMT"
dnl then
dnl     AC_MSG_CHECKING([SHFMT version])
dnl     [SHFMT_VERSION=`"$SHFMT" --version 2>&1`]
dnl     AC_MSG_RESULT([$SHFMT_VERSION])
dnl fi
AC_SUBST(SHFMT)
dnl AC_SUBST(SHFMT_VERSION)
AM_CONDITIONAL(HAVE_SHFMT, test -n "$SHFMT")
AC_CHECK_PROGS([RPMLINT], [rpmlint])
AM_CONDITIONAL([HAVE_RPMLINT], [test -n "$RPMLINT"])
AC_CHECK_PROGS([GEOJSONHINT], [geojsonhint])
if [ test -z "$GEOJSONHINT" ]; then
  dnl fallback to npm i -g geojson-validation
  AC_CHECK_PROGS([GJV], [gjv])
  GEOJSONLINT="$GJV"
else
  GEOJSONLINT="$GEOJSONHINT"
fi
AC_SUBST([GEOJSONLINT])

dnl SVG validation
AC_CHECK_PROGS([JING], [jing])
case $host_os in
  mingw*)   RUN_JING=rem ;;
  windows*) RUN_JING=rem ;;
  *) RUN_JING=":" ;;
esac

dnl LaTeXML has a broken svg11-basic.rng
dnl The zip from http://yupotan.sppd.ne.jp/relax-ng/svg11/ is correct.
dnl Needs to be installed into /usr/local/share/relaxng/svg11/
dnl See https://github.com/brucemiller/LaTeXML/issues/1237
if test -n "$JING"
then
  AC_MSG_CHECKING([for a working relaxng svg11-basic.rng])
  if test -e "/usr/local/share/relaxng/svg11/svg11-basic.rng"
  then
    try_jing="$JING /usr/local/share/relaxng/svg11/svg11-basic.rng"
    AC_MSG_RESULT([$try_jing])
  elif test -e "/usr/share/perl5/vendor_perl/LaTeXML/resources/RelaxNG/svg/svg11-basic.rng"
  then
    try_jing="$JING /usr/share/perl5/vendor_perl/LaTeXML/resources/RelaxNG/svg/svg11-basic.rng"
    AC_MSG_RESULT([$try_jing])
  else
    LATEXPM=`$PERL -MLaTeXML -e'print $INC{"LaTeXML.pm"}'`
    if test -e "$LATEXPM" -a -e "`dirname $LATEXPM`/LaTeXML/resources/RelaxNG/svg/svg11-basic.rng"
    then
      try_jing="$JING `dirname $LATEXPM`/LaTeXML/resources/RelaxNG/svg/svg11-basic.rng"
      AC_MSG_RESULT([$try_jing])
    else
      AC_MSG_RESULT([none])
      JING=
    fi
  fi
  AC_MSG_CHECKING([whether the svg validator works (maybe broken LaTeXML)])
  cat <<_ACEOF >try.svg
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<svg
   xmlns:svg="http://www.w3.org/2000/svg"
   xmlns="http://www.w3.org/2000/svg"
   xmlns:xlink="http://www.w3.org/1999/xlink"
   version="1.1" baseProfile="basic"
   width="100%" height="100%"
   viewBox="-2680430.068109 -1672547.179548 3540706.089899 2726367.694617">
	<path id="dwg-object-45" d="M 2680800.926721,1053089.884765 L 2680880.107866,1053201.295796"
	      style="fill:none;stroke:black;stroke-width:0.1px" />
</svg>
_ACEOF
  if $try_jing try.svg
  then
    RUN_JING="$try_jing"
    AC_MSG_RESULT([yes])
  else
    AC_MSG_RESULT([no])
    AC_MSG_RESULT(m4_normalize([
      Use the zip from http://yupotan.sppd.ne.jp/relax-ng/svg11/
      and unpack it into /usr/local/share/relaxng/svg11/
      ]))
    JING=
  fi
  rm try.svg 2>/dev/null
fi
AC_SUBST([RUN_JING])
AM_CONDITIONAL([HAVE_JING], [test -n "$JING"])

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
  AX_VALGRIND_CHECK
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
  AC_SUBST([VALGRIND_ENABLED],[$enable_valgrind])
  AM_CONDITIONAL([VALGRIND_ENABLED],[test "$enable_valgrind" = "yes"])
fi
AC_SUBST(VALGRIND_SUPPRESSIONS)
AC_SUBST(VALGRIND_SUPPRESSIONS_FILE)

dnl Write it out
AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/codepages/Makefile
	doc/Makefile
	programs/Makefile
	examples/Makefile
	test/Makefile
	test/unit-testing/Makefile
	libredwg.pc
        vcpkg.json
])

AM_COND_IF([DISABLE_BINDINGS], [],
           [AC_CONFIG_FILES([bindings/Makefile bindings/python/Makefile])])
AM_COND_IF([DISABLE_BINDINGS], [],
           [AM_COND_IF([HAVE_PERL], [AC_CONFIG_FILES([bindings/perl/Makefile.PL])])])
AM_COND_IF([HAVE_PYTHON_LIBXML2],  [AC_CONFIG_FILES([test/xmlsuite/Makefile])])
AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([doc/Doxyfile doc/footer])])
AC_CONFIG_FILES([programs/dwgfilter], [chmod +x programs/dwgfilter])
AC_CONFIG_FILES([programs/alive.test], [chmod +x programs/alive.test])
AC_CONFIG_FILES([programs/dxf.test], [chmod +x programs/dxf.test])
AC_CONFIG_FILES([programs/json.test], [chmod +x programs/json.test])
AC_CONFIG_FILES([programs/svg.test], [chmod +x programs/svg.test])
AC_CONFIG_FILES([examples/dwgadd_test.sh], [chmod +x examples/dwgadd_test.sh])
AC_CONFIG_FILES([logs-all.sh:$logs_all], [chmod +x logs-all.sh])
AC_CONFIG_FILES([log], [chmod +x log])
AC_CONFIG_FILES([dwg], [chmod +x dwg])
AC_CONFIG_FILES([dxf], [chmod +x dxf])
AC_CONFIG_FILES([json], [chmod +x json])
AC_CONFIG_FILES([geojson], [chmod +x geojson])
AC_CONFIG_FILES([svg], [chmod +x svg])
AC_CONFIG_FILES([rw], [chmod +x rw])
AC_CONFIG_FILES([dynapi_test_all.sh], [chmod +x dynapi_test_all.sh])
if test "x$ac_cv_func_scandir" = xyes; then
  AC_CONFIG_FILES([unit_testing_all.sh], [chmod +x unit_testing_all.sh])
fi

AC_OUTPUT

dnl configure.ac ends here
