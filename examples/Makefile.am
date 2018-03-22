## Makefile.am
##
## Copyright (C) 2010,2018 Free Software Foundation, Inc.
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

AM_CFLAGS = -I$(top_srcdir)/src -Wall -Wextra
LDADD   = $(top_builddir)/src/libredwg.la -lm

bin_PROGRAMS = dwgread dwgbmp dwg2dxf dwg2SVG
check_PROGRAMS = load_dwg
# dwg2svg2 fails on 32bit

if HAVE_LIBPS
bin_PROGRAMS += dwg_ps
dwg_ps_SOURCES = dwg_ps.c
dwg_ps_LDADD = $(LDADD) -lps
endif

if USE_WRITE
bin_PROGRAMS += dwgrewrite
dwgrewrite_SOURCES = dwgrewrite.c
endif

dwgread_SOURCES  = dwgread.c
dwgbmp_SOURCES   = dwgbmp.c
dwg2dxf_SOURCES  = dwg2dxf.c
dwg2SVG_SOURCES  = dwg2SVG.c

load_dwg_SOURCES = load_dwg.c
#dwg2svg2_SOURCES = dwg2svg2.c

if HAVE_WINE
TESTS_ENVIRONMENT = \
 PROGS='$(bin_PROGRAMS) $(check_PROGRAMS)' \
 srcdir='$(srcdir)' \
 TESTPROG='wine'
else
TESTS_ENVIRONMENT = \
 PROGS='$(bin_PROGRAMS) $(check_PROGRAMS)' \
 srcdir='$(srcdir)' \
 TESTPROG=''
endif

TESTS = alive.test

EXTRA_DIST = example_2000.dwg sample_2000.dwg suffix.c load_dwg.py $(TESTS)
CLEANFILES = {example,sample}_2000.{bmp,ps,svg,dxf} {example,sample}_2000.dwg-rewrite.dwg

.c.i:
	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.o$$||'`;\
	$(COMPILE) -MT $@ -MD -MP -MF $$depbase.Tpo -c -E -o $@ $<

# emacs flymake-mode
check-syntax:
	test -n "$(CHK_SOURCES)" && \
	  $(COMPILE) -o /dev/null -S $(CHK_SOURCES)
.PHONY: check-syntax
