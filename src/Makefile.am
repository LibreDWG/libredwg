## Makefile.am
##
## Copyright (C) 2010 Free Software Foundation, Inc.
##
## This file is part of LibreDWG.
##
## LibreDWG is free software: you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## LibreDWG is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty
## of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
## See the GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

lib_LTLIBRARIES = libredwg.la

libredwg_la_SOURCES = \
	dwg.c \
	common.c \
	bits.c \
	decode.c \
        decode_r2007.c \
	encode.c \
        print.c \
	dwg_api.c
libredwg_la_LDFLAGS = \
	-version-info 0:0:0

include_HEADERS = dwg.h \
		dwg_api.h

EXTRA_DIST = \
	dwg.spec \
        header_variables.spec \
        common_entity_handle_data.spec \
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
