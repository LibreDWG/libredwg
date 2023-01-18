# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_c_restrict.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_C_RESTRICT
#
# DESCRIPTION
#
#   This macro checks if the C99 restrict keyword is broken with the current
#   compiler and optimizations settings. If broken, it undefines restrict to
#   do no harm. As of 2019 gcc since 5.0 and clang since 6.0 (June 2017) are
#   known to be broken and not yet fixed. See
#   https://github.com/rust-lang/rust/issues/54878 with the links to the gcc
#   and clang tickets.
#
# LICENSE
#
#   Copyright (c) 2019 Reini Urban <rurban@cpan.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 1

AC_DEFUN([AX_C_RESTRICT],
[AC_CACHE_CHECK([for C restrict usability],
  [ax_cv_c_restrict_usable],
  [AC_RUN_IFELSE([AC_LANG_PROGRAM([],
  [[
    void copy (int * restrict to, int * restrict from) {
    	*to = *from;
    }
    void test (int *a, int *b) {
    	for (int i = 0; i < 4; i++) {
    	    copy (&b[i & 1], &a[i & 1]);
    	}
    }
    int main () {
    	int ary[] = {0, 1, 2};
    	test (&ary[1], &ary[0]);
    	/* printf("%d %d %d\n", ary[0], ary[1], ary[2]);
            2 2 2 is correct
            1 2 2 wrong */
    	return ary[0] == 2 ? 0 : 1;
    }
  ]])],
  [ax_cv_c_restrict_usable=restrict],
  [ax_cv_c_restrict_usable=],
  [ax_cv_c_restrict_usable=])])

if test -z $ax_cv_c_restrict_usable; then
  AC_MSG_RESULT([no])
  AC_DEFINE_UNQUOTED(restrict, $ax_cv_c_restrict_usable,
    [If restrict is broken with this C compiler])
else
  AC_MSG_RESULT([yes])
fi
]) dnl AX_C_RESTRICT
