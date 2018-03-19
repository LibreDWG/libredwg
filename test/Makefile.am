AUTOMAKE_OPTIONS = dejagnu

SUBDIRS = unit-testing
if HAVE_DEJAGNU
SUBDIRS += testcases
endif
SUBDIRS += xmlsuite
EXTRA_DIST = test-data

if ENABLE_GCOV
gcov:
	for d in $(SUBDIRS); do $(MAKE) -C $$d gcov; done
endif

CLEANFILES = *.log *.sum site.bak
