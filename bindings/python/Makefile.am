## Makefile.am
##
## Copyright (C) 2010-2019 Free Software Foundation, Inc.
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

## bindings/python/Makefile.am: call SWIG and build python module
## written by Rodrigo Rodrigues da Silva
## modified by Reini Urban

## TODO: place generated files out of source tree

ACLOCAL_AMFLAGS = -I $(top_srcdir)/m4

## magic to override pkgpythondir = $(pythondir)/$(PACKAGE)
## at least in Ubuntu python wouldn't find the module there
pkgpythondir = $(pythondir)

if HAVE_SWIG_PYTHON
if HAVE_PYTHON

BUILT_SOURCES = swig_python.c
SWIG_SOURCES  = $(top_srcdir)/bindings/dwg.i \
		$(top_srcdir)/include/dwg.h \
		$(top_srcdir)/include/dwg_api.h

dist_python_PYTHON = $(srcdir)/LibreDWG.py
pyexec_LTLIBRARIES = _LibreDWG.la

# The underscore and casing is correct. python looks for _LibreDWG.so
# when the LibreDWG module is loaded
_LibreDWG_la_SOURCES = $(BUILT_SOURCES) $(SWIG_SOURCES)
## more magic: SWIG_PYTHON_CPPFLAGS resolves to null and python includes
## are not passed to gcc via -I
_LibreDWG_la_CPPFLAGS = -I$(top_srcdir)/include $(AX_SWIG_PYTHON_CPPFLAGS)
_LibreDWG_la_LDFLAGS = -module -no-undefined $(PYTHON_EXTRA_LDFLAGS)
_LibreDWG_la_LIBADD = ../../src/libredwg.la $(PYTHON_LIBS) $(PYTHON_EXTRA_LIBS)

## not elegant, but works
## With macports gcc you need -I/opt/local/lib/gcc6/gcc/x86_64-apple-darwin15/6.4.0/include
SWIG_PYTHON_PATCH = $(top_srcdir)/build-aux/swig_python.patch
$(dist_python_PYTHON) : swig_python.c
if HAVE_PATCH
swig_python.c : $(SWIG_SOURCES) Makefile.am
	$(SWIG) -python -I$(top_srcdir)/include -o swig_python.c $(srcdir)/../dwg.i
	-$(PATCH) -t --dry-run -N -p2 -i $(SWIG_PYTHON_PATCH) && \
	  $(PATCH) -t -N -p2 -b -i $(SWIG_PYTHON_PATCH)
CLEANFILES = swig_python.c.orig
else
swig_python.c : $(SWIG_SOURCES) Makefile.am
	$(SWIG) -python -I$(top_srcdir)/include -o swig_python.c $(srcdir)/../dwg.i
endif

## should work out of the box, but $(SWIG) resolves to swig -c++
## which we don't want, and SWIG_PYTHON_OPT resolves to null,
## therefore -tcl is called by default
##
##    $(srcdir)/swig_python.c : $(SWIG_SOURCES)
##	$(SWIG) $(SWIG_PYTHON_OPT) -I$(top_srcdir)/src -o $@ $<

DISTCLEANFILES = LibreDWG.py swig_python.c

endif
endif
