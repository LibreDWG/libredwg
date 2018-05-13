## Makefile.am
##
## Copyright (C) 2015 Free Software Foundation, Inc.
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

AM_CFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src @WARN_CFLAGS@
LDADD    = $(top_builddir)/src/libredwg.la
#SOURCES = $(top_builddir)/src/libredwg.la

# NB: After you modify ‘check_PROGRAMS’, do "make update-ignorance"
#     and include this directory's .gitignore in the commit.
check_PROGRAMS = \
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
	xline \
	xrecord

TESTS = $(check_PROGRAMS)
TESTS_ENVIRONMENT = \
  INPUT=$(srcdir)/example_2000.dwg
# todo: more dwg versions, in test/test-data

CLEANFILES = *.o
MAINTAINERCLEANFILES  = *_flymake.[ch] *~ *.i

update-ignorance:
	printf '/%s\n' $(check_PROGRAMS) > .gitignore

EXTRA_DIST = common.c example_2000.dwg

@VALGRIND_CHECK_RULES@

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
