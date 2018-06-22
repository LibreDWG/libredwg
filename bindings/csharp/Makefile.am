## Makefile.am
##
## Copyright (C) 2018 Free Software Foundation, Inc.
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

## bindings/csharp/Makefile.am
## written by Reini Urban

ACLOCAL_AMFLAGS = -I $(top_srcdir)/m4

if HAVE_SWIG

BUILT_SOURCES = swig_csharp.c
SWIG_SOURCES = $(srcdir)/../dwg.i $(top_srcdir)/include/dwg.h

dist_sharp_LTLIBRARIES = LibreDWG.la
dist_sharp_CSHARP = libredwg.csh

LibreDWG_la_SOURCES = $(BUILT_SOURCES)  $(SWIG_SOURCES)
LibreDWG_la_CPPFLAGS = -I$(top_srcdir)/include $(CSHARP_CPPFLAGS)
LibreDWG_la_LDFLAGS = -module
LibreDWG_la_LIBADD = ../../src/libredwg.la

swig_csharp.c : $(SWIG_SOURCES) Makefile.am
	$(SWIG) -csharp -I$(top_srcdir)/include -o $@ $(srcdir)/../dwg.i

DISTCLEANFILES = swig_csharp.c

endif
