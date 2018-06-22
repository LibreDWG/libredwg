## Makefile.am
##
## Copyright (C) 2010, 2015, 2018 Free Software Foundation, Inc.
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

## TODO: place generated files out of source tree

ACLOCAL_AMFLAGS = -I $(top_srcdir)/m4

## magic to override pkgpythondir = $(pythondir)/$(PACKAGE)
## at least in Ubuntu python wouldn't find the module there
pkgpythondir = $(pythondir)

if HAVE_SWIG_PYTHON

BUILT_SOURCES = swig_wrap_python.c
SWIG_SOURCES = $(srcdir)/../dwg.i $(top_srcdir)/include/dwg.h

dist_python_PYTHON = libredwg.py
pyexec_LTLIBRARIES = _libredwg.la

# the underscore is correct. the python interpreter looks for _libredwg.so
# when libredwg module is loaded
_libredwg_la_SOURCES = $(BUILT_SOURCES)  $(SWIG_SOURCES)

## more magic: SWIG_PYTHON_CPPFLAGS resolves to null and python includes
## are not passed to gcc via -I
##    _libredwg_la_CPPFLAGS = $(SWIG_PYTHON_CPPFLAGS) -I$(top_srcdir)/src
_libredwg_la_CPPFLAGS = -I$(top_srcdir)/include $(AX_SWIG_PYTHON_CPPFLAGS)
_libredwg_la_LDFLAGS = -module
_libredwg_la_LIBADD = ../../src/libredwg.la

## not elegant, but works
## With macports gcc you need -I/opt/local/lib/gcc6/gcc/x86_64-apple-darwin15/6.4.0/include
swig_wrap_python.c : $(SWIG_SOURCES) Makefile.am
	swig -python -I$(top_srcdir)/include -o $@ $(srcdir)/../dwg.i

## should work out of the box, but $(SWIG) resolves to swig -c++
## which we don't want, and SWIG_PYTHON_OPT resolves to null,
## therefore -tcl is called by default
##
##    $(srcdir)/swig_wrap_python.c : $(SWIG_SOURCES)
##	$(SWIG) $(SWIG_PYTHON_OPT) -I$(top_srcdir)/src -o $@ $<

endif
