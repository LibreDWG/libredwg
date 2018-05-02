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

AM_CFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src @WARN_CFLAGS@
LDADD   = $(top_builddir)/src/libredwg.la -lm

bin_PROGRAMS = dwgread dwgbmp dwg2SVG
# not yet ready
noinst_PROGRAMS = dwg2dxf

if HAVE_LIBPS
bin_PROGRAMS += dwg2ps
dwg2ps_SOURCES = dwg2ps.c
dwg2ps_LDADD = $(LDADD) -lps
endif

if USE_WRITE
bin_PROGRAMS += dwgrewrite
# not yet ready
noinst_PROGRAMS += dxf2dwg
dwgrewrite_SOURCES = dwgrewrite.c
dxf2dwg_SOURCES    = dxf2dwg.c
endif

dwgread_SOURCES  = dwgread.c
dwgbmp_SOURCES   = dwgbmp.c
dwg2dxf_SOURCES  = dwg2dxf.c
dwg2SVG_SOURCES  = dwg2SVG.c

if HAVE_WINE
TESTS_ENVIRONMENT = \
 PROGS='$(bin_PROGRAMS)' \
 datadir='$(top_srcdir)/test/test-data' \
 TESTPROG='wine'
else
TESTS_ENVIRONMENT = \
 PROGS='$(bin_PROGRAMS)' \
 datadir='$(top_srcdir)/test/test-data' \
 TESTPROG=''
endif

TESTS = alive.test

EXTRA_DIST = suffix.inc common.inc $(TESTS)
CLEANFILES = {example,sample,Leader}_*.{bmp,ps,svg,dxf}
CLEANFILES += {example,sample,,Leader}_*rewrite.dwg

.PHONY: man dsymutil check-syntax

# could also use $(subst .c,.1,)
man: $(bin_PROGRAMS) $(noinst_PROGRAMS)
	for p in $(bin_PROGRAMS) $(noinst_PROGRAMS); do \
	  $(MAKE) $$p.1; done

# TODO $*F stem is GNU make only. ./ prefix is POSIX only
.c.1:
	help2man -p LibreDWG ./$(*F) > $@

.c.i:
	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.o$$||'`;\
	$(COMPILE) -MT $@ -MD -MP -MF $$depbase.Tpo -c -E -o $@ $<

dsymutil:
	for e in $(bin_PROGRAMS); do dsymutil $$e; done

# emacs flymake-mode
check-syntax:
	test -n "$(CHK_SOURCES)" && \
	  $(COMPILE) -o /dev/null -S $(CHK_SOURCES)

#TODO: alive.test => runtests with VALGRIND support
#@VALGRIND_CHECK_RULES@
#VALGRIND_SUPPRESSIONS_FILES = $(PACKAGE).supp
#EXTRA_DIST = $(PACKAGE).supp
