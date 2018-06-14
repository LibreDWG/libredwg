AUTOMAKE_OPTIONS = dejagnu
DEJATOOL = testcases

SUBDIRS = unit-testing
if HAVE_DEJAGNU
SUBDIRS += testcases
endif
SUBDIRS += xmlsuite

if IS_RELEASE
TESTDATA = test-data
else
# GNU make and BSD make understands the next line
TESTDATA := $(shell git ls-tree -r --name-only @ test-data)
endif
EXTRA_DIST = $(TESTDATA)

if ENABLE_GCOV
gcov:
	for d in $(SUBDIRS); do $(MAKE) -C $$d gcov; done
endif

CLEANFILES = *.log *.sum site.bak
