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

WARN_CFLAGS = @WARN_CFLAGS@
AM_CFLAGS  = -I$(top_srcdir)/include -I$(top_srcdir)/src $(WARN_CFLAGS)
LDADD      = $(top_builddir)/src/libredwg.la -lm

check_PROGRAMS = load_dwg dwg2svg2
EXTRA_PROGRAMS = unknown

load_dwg_SOURCES = load_dwg.c
dwg2svg2_SOURCES = dwg2svg2.c
unknown_SOURCES  = unknown.c alldxf_0.inc alldxf_1.inc alldxf_2.inc
all: $(check_PROGRAMS)

.PHONY: check-syntax regen-unknown dsymutil gcov

if HAVE_PERL
# maintainer rules only
alldxf_2.inc: alldxf_1.inc
alldxf_1.inc: alldxf_0.inc
alldxf_0.inc: # log_unknown_dxf.pl
	test -d ../.git || false
	test -f alldwg.inc || (echo make regen-unknown; false)
	-mv alldxf_0.inc alldxf_0.inc~
	-mv alldxf_1.inc alldxf_1.inc~
	-mv alldxf_2.inc alldxf_2.inc~
	cd .. && $(PERL) examples/log_unknown_dxf.pl examples/alldwg.inc
alldwg.inc: # log_unknown.pl
	echo Creating alldwg.inc will need a few minutes...
	-cd $(abs_top_builddir) && ./logs.sh; \
	  rm alldwg.tmp; mv examples/alldwg.inc examples/alldwg.inc~; \
	  for l in *_r*.log *_20*.log DS_*.log; \
	    do $(PERL) examples/log_unknown.pl "$$l" >>alldwg.tmp; done \
          sort < alldwg.tmp > examples/alldwg.inc; rm alldwg.tmp

regen-unknown:
	$(MAKE) alldwg.inc
	$(MAKE) alldxf_0.inc
endif

if HAVE_WINE
TESTS_ENVIRONMENT = \
 PROGS='$(check_PROGRAMS)' \
 datadir='$(top_srcdir)/test/test-data' \
 TESTPROG='wine'
else
TESTS_ENVIRONMENT = \
 PROGS='$(check_PROGRAMS)' \
 datadir='$(top_srcdir)/test/test-data' \
 TESTPROG=''
endif

TESTS = $(top_srcdir)/programs/alive.test

EXTRA_DIST = load_dwg.py
CLEANFILES = {example,sample}_2000.svg alldwg.inc~ \
	     alldxf_0.inc~ alldxf_1.inc~ alldxf_2.inc~
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
#@VALGRIND_CHECK_RULES@
#VALGRIND_SUPPRESSIONS_FILES = $(PACKAGE).supp
#EXTRA_DIST = $(PACKAGE).supp
