
AM_CPPFLAGS = $(XML2_CFLAGS)
AM_CFLAGS = -g
LDADD = $(top_builddir)/src/libredwg.la -lm $(XML2_LIBS)

# Set env var ‘PYTHON’ so that check.py can invoke other .py programs.
TESTS_ENVIRONMENT = PYTHON='$(PYTHON)' $(PYTHON)

check_PROGRAMS = testsuite
TESTS = check.py
CLEANFILES = *.log *.sum site.bak result.htm

EXTRA_DIST = suffix.c common.c
EXTRA_DIST += DWG
EXTRA_DIST += header.htm helper.py $(TESTS)

clean-local:
	rm -rf test_output/
