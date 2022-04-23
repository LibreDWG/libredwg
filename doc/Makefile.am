## Makefile.am
##
## Copyright (C) 2010-2022 Free Software Foundation, Inc.
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

TEXINFO_TEX_DIR = $(abs_builddir)/latex
TEXINPUTS="$(PATH_SEPARATOR)$(TEXINFO_TEX_DIR)"
info_TEXINFOS = LibreDWG.texi
# Note: automake has a distcheck bug in .texi.info: GH #211
LibreDWG_TEXINFOS = fdl.texi dynapi.texi

if HAVE_MAKEINFO
.PHONY: manual
manual:
	cd $(top_srcdir)/doc && ../build-aux/gendocs.sh LibreDWG "GNU LibreDWG - Manual"

endif

dynapi.texi: $(top_srcdir)/src/gen-dynapi.pl
	echo make regen-dynapi might be needed

if HAVE_DOXYGEN
.PHONY: refman doxygen refman-pdf
refman doxygen html/index.html: Doxyfile ../include/dwg_api.h ../include/dwg.h ../src/dynapi.c
	-rm ../{src,include}/*_flymake.{c,h} ../{src,include}/*.ii
	cd .. && $(DOXYGEN) doc/Doxyfile
refman-pdf: html/index.html
	$(MAKE) -C latex

CLEANFILES = Doxyfile footer LibreDWG.pdf LibreDWG.info LibreDWG.info-1 LibreDWG.info-2
all-local: Doxyfile footer
clean-local:
	rm -rf $(builddir)/doc/refman
	rm -rf $(builddir)/doc/manual
	rm -f $(srcdir)/doc/LibreDWG.info
	rm -f $(srcdir)/doc/LibreDWG.info-1
	rm -f $(srcdir)/doc/LibreDWG.info-2
else
clean-local:
	rm -rf $(builddir)/doc/manual
	rm -f $(srcdir)/doc/LibreDWG.info
	rm -f $(srcdir)/doc/LibreDWG.info-1
	rm -f $(srcdir)/doc/LibreDWG.info-2
endif

DISTCLEANFILES = LibreDWG.ps LibreDWG.{aux,cp,cps,log,t2p,toc} LibreDWG.info LibreDWG.info-1 LibreDWG.info-2
distclean-local:
	-rm -rf manual refman latex
