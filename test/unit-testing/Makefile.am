## Makefile.am
##
## Copyright (C) 2015-2019 Free Software Foundation, Inc.
##
## This file is part of LibreDWG.
##
## LibreDWG is free software: you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## LibreDWG is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty
## of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
## See the GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

WARN_CFLAGS = @WARN_CFLAGS@
AM_CFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src $(WARN_CFLAGS)
LDADD     = $(top_builddir)/src/libredwg.la

bits_test_LDADD = $(LDADD) \
	$(top_builddir)/src/bits.lo
decode_test_LDADD = $(LDADD) \
	$(top_builddir)/src/bits.lo \
	$(top_builddir)/src/hash.lo \
	$(top_builddir)/src/decode_r2007.lo \
	$(top_builddir)/src/common.lo \
	$(top_builddir)/src/print.lo
hash_test_LDADD = $(LDADD) \
	$(top_builddir)/src/bits.lo \
	$(top_builddir)/src/hash.lo
dynapi_test_LDADD = $(LDADD) \
	$(top_builddir)/src/bits.lo


# NB: After you modify ‘check_PROGRAMS’, do "make update-ignorance"
#     and include this directory's .gitignore in the commit.
check_PROGRAMS = \
	bits_test \
	decode_test \
	hash_test \
	dynapi_test \
	3dsolid \
	arc \
	attdef \
	attrib \
	block \
	body \
	circle \
	dim_aligned \
	dim_ang2ln \
	dim_ang3pt \
	dim_diameter \
	dim_linear \
	dim_ordinate \
	dim_radius \
	ellipse \
	endblk \
	insert \
	line \
	lwpline \
	minsert \
	mline \
	mtext \
	ole2frame \
	point \
	polyline_2d \
	polyline_3d \
	polyline_mesh \
	polyline_pface \
	ray \
	region \
	seqend \
	shape \
	solid \
	text \
	tolerance \
	trace \
	vertex_2d \
	vertex_3d \
	vertex_mesh \
	vertex_pface \
	vertex_pface_face \
	xline \
	xrecord

dynapi_test.c: dynapi_test.c.in \
		$(top_srcdir)/include/dwg.h \
	        $(top_srcdir)/src/gen-dynapi.pl
	-cd $(top_srcdir)/src && $(PERL) gen-dynapi.pl

.c.i:
	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.o$$||'`;\
	$(COMPILE) -c -E -o $@ $<

TESTS = $(check_PROGRAMS)
TESTS_ENVIRONMENT = \
  ASAN_OPTIONS="detect_leaks=0"

bits_test.cbmc: bits_test.c $(top_srcdir)/src/bits.c
	cbmc -I. -I$(top_srcdir)/include -I/opt/local/include \
	     bits_test.c $(top_srcdir)/src/bits.c \
	     --unwind 6  --cover decision > $@

CLEANFILES = *.o
MAINTAINERCLEANFILES  = *_flymake.[ch] *~ *.i

update-ignorance:
	printf '/%s\n' $(check_PROGRAMS) > .gitignore

EXTRA_DIST = \
	tests_common.h \
	common.c \
	dynapi_test.c.in \
	dynapi_test.c

@VALGRIND_CHECK_RULES@
VALGRIND_SUPPRESSIONS_FILE = @VALGRIND_SUPPRESSIONS_FILE@
VALGRIND_SUPPRESSIONS = @VALGRIND_SUPPRESSIONS@

# emacs flymake-mode
check-syntax:
	test -n "$(CHK_SOURCES)" && \
	  $(COMPILE) -o /dev/null -S $(CHK_SOURCES)
.PHONY: check-syntax

if ENABLE_GCOV
AM_CFLAGS += @GCOV_CFLAGS@
LIBS      += @GCOV_LIBS@
gcov: $(check_PROGRAMS) check
	-test -f gcov.log && rm gcov.log
	$(GCOV) *.gcno | tee -a $(builddir)/gcov.log

CLEANFILES  += *.gcov *.gcno *.gcda gcov.log
endif
