
AM_CPPFLAGS = $(XML2_CFLAGS)
# no -O2
# CFLAGS = -g
WARN_CFLAGS = @WARN_CFLAGS@
AM_CFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src $(WARN_CFLAGS)
LDADD = $(top_builddir)/src/libredwg.la $(XML2_LIBS)

# Set env var ‘PYTHON’ so that check.py can invoke other .py programs.
TESTS_ENVIRONMENT = PYTHON="$(PYTHON)" $(PYTHON)
check_PROGRAMS = testsuite
TESTS = check.py

CLEANFILES = *.log *.sum site.bak result.htm
MAINTAINERCLEANFILES  = *_flymake.[ch] *~ *.i

@VALGRIND_CHECK_RULES@

if ENABLE_GCOV
AM_CFLAGS += @GCOV_CFLAGS@
LIBS      += @GCOV_LIBS@
gcov: $(CHECK)
	-test -f gcov.log && rm gcov.log
	$(GCOV) *.gcno | tee -a $(builddir)/gcov.log

CLEANFILES  += *.gcov *.gcno *.gcda gcov.log

else
gcov:
endif

EXTRA_DIST = common.c
EXTRA_DIST += newxml.py txttoxml.py
EXTRA_DIST += header.htm helper.py check.py

clean-local:
	rm -rf test_output/
