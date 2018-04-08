## bindings/Makefile.am
##
## Copyright (C) 2010, 2018 Free Software Foundation, Inc.
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

## bindings/Makefile.am: includes swig rules for perl
## written by Reini Urban

ACLOCAL_AMFLAGS = -I m4

# The perl subdir is for the perl Makefile.PL tool and is driven from here
SUBDIRS = python

dist_perl_PERL = perl/wrap_perl.c perl/LibreDWG.pm perl/LibreDWG.xs perl/Makefile.PL \
	         perl/LibreDWG.o
all-am: $(dist_perl_PERL)
	echo Making all for perl

perl/LibreDWG.o: perl/LibreDWG.pm perl/LibreDWG.xs perl/Makefile.PL
	echo Note wrap_perl.c is huge. this will need some time...
	cd perl && \
	  $(PERL) Makefile.PL CCFLAGS="-I$(abs_top_srcdir)/include" \
                              LDFLAGS="-L$(abs_top_builddir)/src/.libs" && \
	  $(MAKE) OPTIMIZE=

# regen the wrapper
if HAVE_SWIG_PERL
SWIG_PERL_sources = perl/wrap_perl.c
PERL_sources = $(top_srcdir)/bindings/dwg.i \
	$(top_srcdir)/include/dwg.h \
	$(top_srcdir)/include/dwg_api.h

$(SWIG_PERL_sources): $(PERL_sources) Makefile.am
	$(SWIG) -perl5 -I$(abs_top_srcdir)/include -o perl/wrap_perl.c $(srcdir)/dwg.i

CLEANFILES = perl/LibreDWG.bs perl/LibreDWG.c perl/pm_to_blib perl/Makefile perl/LibreDWG.o

check-am: $(dist_perl_PERL)
	echo Making check for perl
	$(MAKE) -C perl test
endif
