## Makefile.am
##
## Copyright (C) 2010 Free Software Foundation, Inc.
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

check_PROGRAMS = load_dwg get_bmp test testSVG

if HAVE_LIBPS
check_PROGRAMS += dwg_ps
dwg_ps_SOURCES = dwg_ps.c
dwg_ps_LDADD = -lps
endif

AM_LDFLAGS = -L../src -lredwg

test_SOURCES = test.c

testSVG_SOURCES = testSVG.c
testSVG_LDADD   = -lm

load_dwg_SOURCES = load_dwg.c

get_bmp_SOURCES = get_bmp.c

AM_CFLAGS = -Wextra -I$(top_srcdir)/src

TESTS_ENVIRONMENT = \
 PROGS='$(check_PROGRAMS)' \
 srcdir='$(srcdir)'

TESTS = alive.test

EXTRA_DIST = example.dwg sample.dwg suffix.c load_dwg.py $(TESTS)
