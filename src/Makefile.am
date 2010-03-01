lib_LTLIBRARIES = libredwg.la

libredwg_la_SOURCES = \
	dwg.c \
	common.c \
	bits.c \
	decode.c \
	encode.c \
        print.c

libredwg_la_LDFLAGS = \
	-version-info 0:0:0

include_HEADERS = dwg.h

EXTRA_DIST = \
	dwg.spec \
	$(include_HEADERS) \
	common.h \
	bits.h \
	decode.h \
	encode.h \
	logging.h \
        print.h

#check_PROGRAMS = test testsvg

#test_SOURCES = \
#	test.c

#test_LDADD = libredwg.la

#testsvg_SOURCES = \
#	testSVG.c

#testsvg_LDADD = libredwg.la

#testsvg_LDFLAGS = -lm
