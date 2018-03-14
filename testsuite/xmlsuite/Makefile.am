
AM_CPPFLAGS = $(XML2_CFLAGS)
# no -O2
CFLAGS = -g
LDADD = $(top_builddir)/src/libredwg.la $(XML2_LIBS)

# Set env var ‘PYTHON’ so that check.py can invoke other .py programs.
TESTS_ENVIRONMENT = PYTHON='$(PYTHON)' $(PYTHON)

check_PROGRAMS = testsuite
TESTS = check.py

@VALGRIND_CHECK_RULES@

CLEANFILES = *.log *.sum site.bak result.htm

EXTRA_DIST = suffix.c common.c
EXTRA_DIST += DWG
EXTRA_DIST += header.htm helper.py $(TESTS)


clean-local:
	rm -rf test_output/
