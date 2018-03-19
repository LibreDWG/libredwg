
AM_CPPFLAGS = $(XML2_CFLAGS)
# no -O2
# CFLAGS = -g
AM_CFLAGS = -I$(top_srcdir)/src @WARN_CFLAGS@
LDADD = $(top_builddir)/src/libredwg.la $(XML2_LIBS)

# Set env var ‘PYTHON’ so that check.py can invoke other .py programs.
TESTS_ENVIRONMENT = PYTHON='$(PYTHON)' $(PYTHON)

check_PROGRAMS = testsuite
TESTS = check.py

@VALGRIND_CHECK_RULES@

if ENABLE_GCOV
AM_CFLAGS += @GCOV_CFLAGS@
LIBS      += @GCOV_LIBS@
#  -e's,$(EXEEXT),,'
gcov: $(check_PROGRAMS) check
	-test -f gcov.log && rm gcov.log
	for f in $(check_PROGRAMS); do \
	  c="`echo $$f|sed -e's,.exe,,'`.c"; \
	  echo $(GCOV) $$c; test -e $$c && $(GCOV) $$c >>gcov.log; done
endif

CLEANFILES = *.log *.sum site.bak result.htm
if ENABLE_GCOV
CLEANFILES  += *.gcov *.gcno *.gcda gcov.log
endif

EXTRA_DIST = suffix.c common.c
EXTRA_DIST += newxml.py txttoxml.py
EXTRA_DIST += header.htm helper.py $(TESTS)

clean-local:
	rm -rf test_output/
