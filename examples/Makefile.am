check_PROGRAMS = load_dwg get_bmp dwg_ps test testSVG

AM_LDFLAGS = -L../src -lredwg

test_SOURCES = test.c

testSVG_SOURCES = testSVG.c
testSVG_LDADD   = -lm

load_dwg_SOURCES = load_dwg.c

get_bmp_SOURCES = get_bmp.c

dwg_ps_SOURCES = dwg_ps.c
dwg_ps_LDADD = -lps

AM_CFLAGS = -Wextra -I$(top_srcdir)/src

EXTRA_DIST = sample.dwg

