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

if HAVE_SWIG_PERL

dist_perl_PERL = perl/swig_perl.c perl/LibreDWG.pm perl/LibreDWG.xs \
                 perl/Makefile.PL perl/MANIFEST
EXTRA_DIST = $(dist_perl_PERL)

all-am: $(dist_perl_PERL)
	@echo Making all for perl

# building perl bindings is optional. Most setup's don't have all the deps, like
# crypt.h, sys/wait.h on windows.
perl/LibreDWG.o: perl/LibreDWG.pm perl/LibreDWG.xs perl/Makefile
	@echo Note swig_perl.c is huge. This will need some time...
	-cd perl && \
	  $(MAKE) OPTIMIZE=

perl/Makefile: perl/Makefile.PL
	-cd perl && \
	  $(PERL) Makefile.PL

CLEANFILES = perl/LibreDWG.bs perl/LibreDWG.c perl/pm_to_blib perl/Makefile perl/LibreDWG.o

# Maybe disable if LibreDWG.so was not installed yet.
# rpath/LD_LIBRARY_PATH overrides are too troublesome
check-am: $(dist_perl_PERL) perl/Makefile perl/LibreDWG.o
	@echo Making check for perl
	-$(MAKE) -C perl test OPTIMIZE= LD_RUN_PATH="$(abs_top_builddir)/src/.libs"

install-am: $(dist_perl_PERL) perl/Makefile perl/LibreDWG.o
	@echo Making install for perl
	-$(MAKE) -C perl install

PERL_sources =  $(top_srcdir)/bindings/dwg.i \
		$(top_srcdir)/include/dwg.h \
		$(top_srcdir)/include/dwg_api.h \
		$(top_srcdir)/src/dwg_api.c \
		$(top_srcdir)/src/dwg.c

perl/LibreDWG.pm: perl/swig_perl.c
perl/swig_perl.c: $(PERL_sources) Makefile.am
	$(SWIG) -perl5 -I$(abs_top_srcdir)/include -o perl/swig_perl.c $(srcdir)/dwg.i

endif
