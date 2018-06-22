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

AM_CFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src $(WARN_CFLAGS)
LDADD   = $(top_builddir)/src/libredwg.la -lm

check_PROGRAMS = load_dwg dwg2svg2
EXTRA_PROGRAMS = unknown

load_dwg_SOURCES = load_dwg.c
dwg2svg2_SOURCES = dwg2svg2.c
unknown_SOURCES  = unknown.c #alldwg.inc

alldwg.inc: log_unknown.pl
	echo Creating alldwg.inc will need a few minutes...
	-cd .. && ./logs.sh; \
	  mv examples/alldwg.inc examples/alldwg.inc~; \
	  for l in *.log; do b=`basename $l .log`; \
	    examples/log_unknown.pl $l >>examples/alldwg.inc; done

.PHONY: check-syntax regen-unknown dsymutil gcov
regen-unknown: alldwg.inc

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

EXTRA_DIST = load_dwg.py log_unknown.pl alldwg.inc
CLEANFILES = {example,sample}_2000.svg alldwg.inc~
MAINTAINERCLEANFILES  = *_flymake.[ch] *~ *.i

.c.i:
	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.o$$||'`;\
	$(COMPILE) -MT $@ -MD -MP -MF $$depbase.Tpo -c -E -o $@ $<

dsymutil:
	for e in $(check_PROGRAMS); do dsymutil $$e; done

# emacs flymake-mode
check-syntax:
	test -n "$(CHK_SOURCES)" && \
	  $(COMPILE) -o /dev/null -S $(CHK_SOURCES)

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
