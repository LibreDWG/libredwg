check_PROGRAMS = load_dwg get_bmp dwg_ps test testSVG

test_SOURCES = test.c
test_LDADD = -lredwg

testSVG_SOURCES = testSVG.c
testSVG_LDADD   = -lredwg -lm

load_dwg_SOURCES = load_dwg.c
load_dwg_LDADD = -lredwg

get_bmp_SOURCES = get_bmp.c
get_bmp_LDADD = -lredwg

dwg_ps_SOURCES = dwg_ps.c
dwg_ps_LDADD = -lredwg -lps

AM_CFLAGS = -Wextra -I../src

EXTRA_DIST = sample.dwg

