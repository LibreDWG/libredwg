lib_LTLIBRARIES = libdwg.la

libdwg_la_SOURCES = \
	dwg.c \
	common.c \
	bits.c \
	decode.c \
	encode.c

libdwg_la_LDFLAGS = \
	-version-info 0:0:0

include_HEADERS = dwg.h

EXTRA_DIST = \
	$(include_HEADERS) \
	common.h \
	bits.h \
	decode.h \
	example.dwg \
	encode.h

check_PROGRAMS = test

test_SOURCES = \
	test.c

test_LDADD = libdwg.la
