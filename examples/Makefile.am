check_PROGRAMS = load_dwg get_bmp test testSVG

if HAVE_LIBPS
check_PROGRAMS += dwg_ps
dwg_ps_SOURCES = dwg_ps.c
dwg_ps_LDADD = -lps
endif

AM_LDFLAGS = -L../src -lredwg

test_SOURCES = test.c

testSVG_SOURCES = testSVG.c
testSVG_LDADD   = -lm

load_dwg_SOURCES = load_dwg.c

get_bmp_SOURCES = get_bmp.c

AM_CFLAGS = -Wextra -I$(top_srcdir)/src

TESTS_ENVIRONMENT = \
 PROGS='$(check_PROGRAMS)' \
 srcdir='$(srcdir)'

TESTS = alive.test

EXTRA_DIST = example.dwg sample.dwg suffix.c $(TESTS)
