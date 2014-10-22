
AM_CPPFLAGS = $(XML2_CFLAGS)
AM_CFLAGS = -g
AM_LDFLAGS = -lredwg -lm $(XML2_LIBS)

check_PROGRAMS = testsuite
TESTS = check.py
CLEANFILES = *.log *.sum site.bak result.htm

clean-local:
	rm -rf test_output/
