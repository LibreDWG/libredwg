## Makefile.am
##
## Copyright (C) 2010-2019 Free Software Foundation, Inc.
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
AM_CFLAGS  = -I$(top_srcdir)/include -I$(top_srcdir)/src $(WARN_CFLAGS)
LDADD      = $(top_builddir)/src/libredwg.la -lm

check_PROGRAMS = load_dwg dwg2svg2
EXTRA_PROGRAMS = unknown bd bits

load_dwg_SOURCES = load_dwg.c
dwg2svg2_SOURCES = dwg2svg2.c
unknown_SOURCES  = unknown.c
# alldxf_0.inc alldxf_1.inc alldxf_2.inc alldwg.inc bd-unknown.inc
unknown_LDADD  = $(top_builddir)/src/bits.lo $(top_builddir)/src/common.lo
bd_SOURCE  = bd.c
bd_LDADD  = $(top_builddir)/src/bits.lo
bits_SOURCE  = bits.c
bits_LDADD  = $(top_builddir)/src/bits.lo
all: $(check_PROGRAMS)

.PHONY: check-syntax regen-unknown dsymutil gcov

if HAVE_PERL
# maintainer rules only
regen-unknown: $(srcdir)/alldwg.inc $(srcdir)/alldxf_0.inc $(srcdir)/bd-unknown.inc

#ACAD_EVALUATION_GRAPH.pi: unknown
#	$(MAKE) -C .. unknown

$(srcdir)/bd-unknown.inc: $(srcdir)/bd-unknown.pl $(srcdir)/alldxf_0.inc \
	                  ACAD_EVALUATION_GRAPH.pi
	$(PERL) $(srcdir)/bd-unknown.pl | sort | uniq >$(srcdir)/bd-unknown.inc

$(srcdir)/alldxf_2.inc: $(srcdir)/alldxf_1.inc
$(srcdir)/alldxf_1.inc: $(srcdir)/alldxf_0.inc
$(srcdir)/alldxf_0.inc: $(srcdir)/alldwg.inc $(srcdir)/log_unknown_dxf.pl
	test -f $(srcdir)/alldwg.inc || (echo make regen-unknown; false)
	-mv $(srcdir)/alldxf_0.inc alldxf_0.inc~ 2>/dev/null
	-mv $(srcdir)/alldxf_1.inc alldxf_1.inc~ 2>/dev/null
	-mv $(srcdir)/alldxf_2.inc alldxf_2.inc~ 2>/dev/null
	cd .. && $(PERL) $(abs_srcdir)/log_unknown_dxf.pl $(abs_srcdir)/alldwg.inc
$(srcdir)/alldwg.inc: $(srcdir)/log_unknown.pl
	echo Creating $(srcdir)/alldwg.inc will need a few minutes...
	-cd .. && ./logs-all.sh; \
	  rm alldwg.tmp 2>/dev/null; \
	  cp $(abs_srcdir)/alldwg.inc examples/alldwg.inc~; \
	  for l in *_r??.log *_20??.log DS_*.log; \
	    do $(PERL) $(abs_srcdir)/log_unknown.pl "$$l" >>alldwg.tmp; done; \
          grep -v '^//' alldwg.tmp | sort >$(abs_srcdir)/alldwg.inc; rm alldwg.tmp
endif

if HAVE_WINE
TESTS_ENVIRONMENT = \
 PROGS='$(check_PROGRAMS)' \
 DATADIR='$(top_srcdir)/test/test-data' \
 TESTPROG='wine'
else
TESTS_ENVIRONMENT = \
 PROGS='$(check_PROGRAMS)' \
 DATADIR='$(top_srcdir)/test/test-data' \
 TESTPROG=''
endif

TESTS = $(top_builddir)/programs/alive.test

EXTRA_DIST = alldxf_0.inc alldxf_1.inc alldxf_2.inc alldwg.inc log_unknown_dxf.pl log_unknown.pl
EXTRA_DIST += load_dwg.py unknown.pi ACAD_EVALUATION_GRAPH.pi bd-unknown.pl
CLEANFILES = {example,sample}_2000.svg alldwg.inc~ \
	     alldxf_0.inc~ alldxf_1.inc~ alldxf_2.inc~
DISTCLEANFILES = alldxf_0.inc alldxf_1.inc alldxf_2.inc
MAINTAINERCLEANFILES  = *_flymake.[ch] *~ *.i

.c.i:
	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.o$$||'`;\
	$(COMPILE) -MT $@ -MD -MP -MF $$depbase.Tpo -c -E -o $@ $<

dsymutil:
	for e in $(check_PROGRAMS) $(EXTRA_PROGRAMS); do dsymutil $$e; done

# emacs flymake-mode
check-syntax:
	test -n "$(CHK_SOURCES)" && \
	  nice $(COMPILE) -O0 -o /dev/null -S $(CHK_SOURCES)

if ENABLE_GCOV
AM_CFLAGS += @GCOV_CFLAGS@
LIBS      += @GCOV_LIBS@
gcov: $(CHECK)
	-test -f gcov.log && rm gcov.log
	$(GCOV) *.gcno | tee -a $(builddir)/gcov.log

CLEANFILES  += *.gcov *.gcno *.gcda gcov.log
endif

#TODO: alive.test => runtests with VALGRIND support
@VALGRIND_CHECK_RULES@
VALGRIND_SUPPRESSIONS_FILE = @VALGRIND_SUPPRESSIONS_FILE@
VALGRIND_SUPPRESSIONS = @VALGRIND_SUPPRESSIONS@
