## Makefile.am
##
## Copyright (C) 2010, 2013-2015, 2018 Free Software Foundation, Inc.
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

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src programs examples test doc
if HAVE_SWIG_PYTHON
SUBDIRS += bindings
endif

include_HEADERS = \
	include/dwg.h \
	include/dwg_api.h

BUILT_SOURCES = $(top_srcdir)/.version
EXTRA_DIST = $(top_srcdir)/.version \
             README README-alpha \
	     dxf_example.lsp dxf_example.sh test-dxf.sh logs.sh

.PHONY: check-dwg check-dxf check-dwg-valgrind \
        regen-man man manual refman refman-pdf scan-build gcov

regen-man:
	$(MAKE) -C programs $@
man:
	$(MAKE) -C programs $@
manual:
	$(MAKE) -C doc $@

if HAVE_DOXYGEN
refman doxygen doc/html/index.html: doc/Doxyfile $(include_HEADERS)
	-rm {src,include}/*_flymake.{c,h}
	$(DOXYGEN) doc/Doxyfile
refman-pdf: doc/html/index.html
	$(MAKE) -C doc/latex
endif

check-dwg: all
	-mv check-dwg.log check-dwg.log~ 2>/dev/null
	for d in `find test/test-data -iname \*.dwg -size -40M`; do \
	  echo programs/dwgread -v3 $$d | tee -a check-dwg.log; \
	  programs/dwgread -v3 $$d >> check-dwg.log 2>&1; \
	done
check-dwg-valgrind: all
	-mv check-dwg-valgrind.log check-dwg-valgrind.log~ 2>/dev/null
	for d in `find test/test-data -iname \*.dwg -size -10M`; do \
	  echo valgrind --leak-check=full programs/dwgread $$d | tee -a check-dwg-valgrind.log; \
	  valgrind --leak-check=full programs/dwgread $$d >> check-dwg-valgrind.log 2>&1; \
	done
check-dxf: all
	-mv check-dxf.log check-dxf.log~ 2>/dev/null
	-for f in test/test-data/Drawing_2*.dwg \
                 test/test-data/sample_2*.dwg \
                 test/test-data/example_2000.dwg; \
          do \
            echo programs/dwg2dxf -v3 $$f | tee -a check-dxf.log; \
            programs/dwg2dxf -v3 $$f >>check-dxf.log 2>&1; \
          done
	-for d in r14 2000 2004 2007 2010 2013 2018; do \
          for f in test/test-data/$$d/*.dwg; do \
            b=`basename $$f .dwg`; dxf="$${b}_$${d}.dxf"; \
            echo programs/dwg2dxf -v3 -o $${dxf} $$f | tee -a check-dxf.log; \
            programs/dwg2dxf -v3 -o $${dxf} $$f >>check-dxf.log 2>&1; \
	  done; \
	done

# clang-analyzer.llvm.org
SCAN_BUILD = scan-build
scan-build: clean
	$(SCAN_BUILD) -V -o .analysis $(MAKE) -j4 &

if ENABLE_GCOV
gcov: check
	$(MAKE) -C test gcov
	$(MAKE) -C src gcov
	$(MAKE) -C programs gcov
	$(MAKE) -C examples gcov
	gcov2perl test/*/*.gcov src/*.gcov programs/*.gcov examples/*.gcov
	cover -no-gcov
endif

$(BUILT_SOURCES):
	echo $(VERSION) > $@-t && mv $@-t $@
dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version

CLEANFILES = check-dwg.log check-dwg-valgrind.log check-dxf.log \
             check-dwg.log~ check-dwg-valgrind.log~ check-dxf.log~ \
	     *_20*.dxf *_r1*.dxf
MAINTAINERCLEANFILES  = *~ *.log cover_db .analysis

maintainer-clean-local:
	-rm -rf cover_db
LIBTOOL_DISTCLEAN_FILES = \
	m4/libtool.m4 m4/ltoptions.m4 m4/ltsugar.m4 m4/ltversion.m4 m4/lt~obsolete.m4 \
	build-aux/compile \
	build-aux/config.guess \
	build-aux/config.sub \
	build-aux/depcomp \
	build-aux/install-sh \
	build-aux/ltmain.sh \
	build-aux/missing \
	build-aux/py-compile \
	build-aux/textinfo.tex
