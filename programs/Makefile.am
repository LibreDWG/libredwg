## Makefile.am
##
## Copyright (C) 2010,2018-2021 Free Software Foundation, Inc.
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
AM_CFLAGS   = -I. -I$(top_srcdir)/include -I$(top_srcdir)/src $(WARN_CFLAGS)
LDADD       = $(top_builddir)/src/libredwg.la
if HAVE_LIBGC
LDADD      += -lgc
endif
CPPCHECK    = @CPPCHECK@

bin_PROGRAMS    = dwgread
noinst_PROGRAMS =
dist_man1_MANS  = $(srcdir)/dwgread.1
if !DISABLE_DXF
bin_PROGRAMS   += dwgbmp dwg2dxf dwg2SVG dwglayers dwggrep
dist_bin_SCRIPTS = dwgfilter
dist_man1_MANS += $(srcdir)/dwgbmp.1 $(srcdir)/dwg2dxf.1 \
	          $(srcdir)/dwg2SVG.1 $(srcdir)/dwglayers.1 \
		  $(srcdir)/dwggrep.1 $(srcdir)/dwgfilter.1
endif

if !DISABLE_DXF
if HAVE_LIBPS
bin_PROGRAMS   += dwg2ps
dist_man1_MANS += $(srcdir)/dwg2ps.1
dwg2ps_SOURCES = dwg2ps.c
dwg2ps_LDADD   = $(LDADD) -lps
endif
endif

dwgread_SOURCES    = dwgread.c
dwgbmp_SOURCES     = dwgbmp.c
dwg2dxf_SOURCES    = dwg2dxf.c
dwglayers_SOURCES  = dwglayers.c
dwg2SVG_SOURCES    = dwg2SVG.c escape.c
dwggrep_SOURCES    = dwggrep.c
dwggrep_CFLAGS     = $(AM_CFLAGS) $(CFLAGS) $(PCRE2_CFLAGS)
dwggrep_LDADD      = $(LDADD) $(PCRE2_LIBS)

if USE_WRITE
bin_PROGRAMS      += dwgrewrite
dist_man1_MANS    += $(srcdir)/dwgrewrite.1
dwgrewrite_SOURCES = dwgrewrite.c
if !DISABLE_DXF
bin_PROGRAMS      += dxf2dwg dwgwrite dxfwrite
dist_man1_MANS    += $(srcdir)/dxf2dwg.1 $(srcdir)/dwgwrite.1 $(srcdir)/dxfwrite.1
dxf2dwg_SOURCES    = dxf2dwg.c
dwgwrite_SOURCES   = dwgwrite.c
dxfwrite_SOURCES   = dxfwrite.c
endif
endif

if HAVE_WINE
TESTS_ENVIRONMENT = \
 PROGS='$(bin_PROGRAMS)' \
 DATADIR='$(top_srcdir)/test/test-data' \
 TESTPROG='wine'
else
if DISABLE_DXF
TESTS_ENVIRONMENT = \
 PROGS='$(bin_PROGRAMS)' \
 DATADIR='$(top_srcdir)/test/test-data' \
 TESTPROG=''
else
TESTS_ENVIRONMENT = \
 PROGS='$(bin_PROGRAMS)' \
 DATADIR='$(top_srcdir)/test/test-data' \
 TESTPROG=''
endif
endif
if CHECK_LESS
TESTS_ENVIRONMENT += CHECK_MINIMAL=1
endif

TESTS = alive.test
if !DISABLE_DXF
TESTS += dxf.test
if !DISABLE_JSON
if HAVE_JQ
TESTS += json.test
endif
endif
if HAVE_JING
TESTS += svg.test
endif
endif

check-minimal: $(bin_PROGRAMS)
	$(MAKE) check TESTS=alive.test CHECK_MINIMAL=1

cppcheck:
if HAVE_CPPCHECK
	$(CPPCHECK) -j4 --inline-suppr -I$(top_srcdir)/include -I../src $(srcdir)/*.c
endif

EXTRA_DIST  = suffix.inc escape.h my_getopt.h my_stat.h common.inc \
	      $(TESTS) cmp_dxf.pl
CLEANFILES  = {example_,sample_,entities,ACEB10}*.{bmp,ps,svg,dxf,log,json,png}
if USE_WRITE
CLEANFILES += {example_,sample_,entities,ACEB10}*rewrite.{dwg,log}
CLEANFILES += {example_,sample_,entities,ACEB10}*.dwg
CLEANFILES += example_*.{log1,log2}
endif
MAINTAINERCLEANFILES = *_flymake.[ch] *~ *.i *.ii *.log
DISTCLEANFILES       = core alive.test dxf.test json.test svg.test

.PHONY: man dsymutil clean-dsymutil check-syntax check-cppcheck check-minimal

.c.i:
	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.o$$||'`;\
	$(COMPILE) -MT $@ -MD -MP -MF $$depbase.Tpo -c -E -o $@ $<

.c.ii: # no linemarkers
	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.o$$||'`;\
	$(COMPILE) -MT $@ -MD -MP -MF $$depbase.Tpo -c -E -P -o $@ $<

man: $(dist_man1_MANS)

if !HAVE_MINGW_CROSS
# Note: These rules do not work cross-compiled
regen-man: $(bin_PROGRAMS) $(noinst_PROGRAMS)
	-for p in $(dist_man1_MANS); do \
	  base=`basename $$p .1`; \
	  help2man -p $(PACKAGE_NAME) -o $$p ./$$base$(EXEEXT); done
endif

#.c.1: $(bin_PROGRAMS) $(noinst_PROGRAMS)
#	help2man -p LibreDWG -o $@ $(*F) || true

dsymutil:
	for e in $(bin_PROGRAMS) $(noinst_PROGRAMS); do dsymutil $$e; done
clean-dsymutil:
	for e in $(bin_PROGRAMS) $(noinst_PROGRAMS); do rm -rf $$e.dSYM; done

# emacs flymake-mode
check-syntax:
	test -n "$(CHK_SOURCES)" && \
	  $(COMPILE) -o /dev/null -S $(CHK_SOURCES)

if ENABLE_GCOV
AM_CFLAGS += @GCOV_CFLAGS@
LIBS      += @GCOV_LIBS@
gcov: $(CHECK)
	-test -f gcov.log && rm gcov.log
	$(GCOV) -lp *.gcno | tee -a $(builddir)/gcov.log

CLEANFILES  += *.gcov *.gcno *.gcda gcov.log
endif

@VALGRIND_CHECK_RULES@
VALGRIND_SUPPRESSIONS_FILES = @VALGRIND_SUPPRESSIONS_FILE@
VALGRIND_SUPPRESSIONS = @VALGRIND_SUPPRESSIONS@
