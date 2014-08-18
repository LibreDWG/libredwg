
LIBXML = `pkg-config libxml-2.0 --cflags`
AM_CFLAGS = $(LIBXML) -g -lredwg -lm -lxml2

check_PROGRAMS = testsuite
TESTS = check.py
CLEANFILES = *.log *.sum site.bak result.htm

clean-local:
	rm -rf test_output/
