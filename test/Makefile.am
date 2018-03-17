AUTOMAKE_OPTIONS = dejagnu

SUBDIRS = unit-testing
if HAVE_DEJAGNU
SUBDIRS += testcases
endif
SUBDIRS += xmlsuite
EXTRA_DIST = test-data

CLEANFILES = *.log *.sum site.bak
