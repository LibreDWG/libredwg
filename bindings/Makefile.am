## bindings/Makefile.am
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

## bindings/Makefile.am: includes swig rules for perl
## written by Reini Urban

ACLOCAL_AMFLAGS = -I m4

# The perl subdir is for the perl Makefile.PL tool and is driven from here
SUBDIRS = csharp
if HAVE_SWIG_PYTHON
SUBDIRS += python
endif
EXTRA_DIST     = Makefile.am Makefile.in

if HAVE_SWIG_PERL

dist_perl_PERL = perl/LibreDWG.c perl/LibreDWG.pm \
                 perl/Makefile.PL perl/MANIFEST perl/t/0_basic.t
EXTRA_DIST    += $(dist_perl_PERL)

all-am: $(dist_perl_PERL)
	@echo Making all for perl

# building perl bindings is optional. Most setup's don't have all the deps, like
# crypt.h, sys/wait.h on windows.
# grep rem: with a command.com compat Makefile we cannot continue from bash
perl/LibreDWG.$(OBJEXT): perl/LibreDWG.pm perl/Makefile
	@echo Note LibreDWG.c is huge. This will need some time...
	-if test -f perl/pm_to_blib; then rm perl/pm_to_blib; fi
	-if test -d perl/blib; then rm -rf perl/blib; fi
	-if grep "NOOP = rem" perl/Makefile; then false; else \
	    cd perl && $(MAKE) PERL="$(PERL)" \
	                       OPTIMIZE="-I$(abs_top_srcdir)/include -I$(abs_top_builddir)/src" \
	                       OTHERLDFLAGS="-L$(abs_top_builddir)/src/.libs"; fi

perl/Makefile: perl/Makefile.PL
if !HAVE_INSRCDIR
	-mkdir -p perl/t
	-cp -f $(srcdir)/perl/t/0_basic.t perl/t/
endif
	-cd perl && \
	  $(PERL) Makefile.PL PERL="$(PERL)" INSTALLDIRS=@with_perl_install@

# Maybe disable if LibreDWG.so was not installed yet.
# rpath/LD_LIBRARY_PATH overrides are too troublesome
check-am: $(dist_perl_PERL) perl/Makefile perl/LibreDWG.$(OBJEXT)
	@echo Making check for perl
	-if grep "NOOP = rem" perl/Makefile; then false; else \
	    LD_LIBRARY_PATH="$(abs_top_builddir)/src/.libs" $(MAKE) -C perl test \
	      PERL="$(PERL)" OPTIMIZE="-I$(abs_top_srcdir)/include -I$(abs_top_builddir)/src" \
              OTHERLDFLAGS="-L$(abs_top_builddir)/src/.libs"; fi

install-am: $(dist_perl_PERL) perl/Makefile perl/LibreDWG.$(OBJEXT)
	@echo Making install for perl
	-if test x@with_perl_install@ != xno; then if grep "NOOP = rem" perl/Makefile; \
            then false; \
	    else $(MAKE) -C perl install_@with_perl_install@ PERL="$(PERL)"; fi; fi

clean-am:
	-$(MAKE) -C perl clean

distclean-am:
	-$(MAKE) -C perl distclean
if !HAVE_INSRCDIR
	rm -rf perl/t
endif

PERL_sources =  $(top_srcdir)/bindings/dwg.i \
		$(top_srcdir)/include/dwg.h \
		$(top_srcdir)/include/dwg_api.h \
		$(top_srcdir)/src/dwg_api.c \
		$(top_srcdir)/src/dwg.c

perl/LibreDWG.pm: perl/LibreDWG.c
perl/LibreDWG.c: $(PERL_sources) Makefile.am
	$(SWIG) -perl5 -I$(abs_top_srcdir)/include -o perl/LibreDWG.c $(srcdir)/dwg.i

distclean-local: distclean-am
	rm -rf perl/blib

MOSTLYCLEANFILES = perl/LibreDWG.bs perl/LibreDWG.$(OBJEXT)
CLEANFILES       = perl/pm_to_blib perl/Makefile
DISTCLEANFILES   = perl/Makefile.PL perl/LibreDWG.c

endif
