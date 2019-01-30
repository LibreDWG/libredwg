## Makefile.am
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

TEXINFO_TEX_DIR = $(abs_builddir)/latex
TEXINPUTS="$(TEXINPUTS)$(PATH_SEPARATOR)$(TEXINFO_TEX_DIR)"
info_TEXINFOS = LibreDWG.texi
LibreDWG_TEXINFOS = fdl.texi

manual:
	../build-aux/gendocs.sh LibreDWG "GNU LibreDWG - Manual"

if HAVE_DOXYGEN
refman doxygen html/index.html: Doxyfile ../include/dwg_api.h ../include/dwg.h ../src/dynapi.c
	-rm ../{src,include}/*_flymake.{c,h} ../{src,include}/*.ii
	cd .. && $(DOXYGEN) doc/Doxyfile
refman-pdf: html/index.html
	$(MAKE) -C latex

CLEANFILES = Doxyfile footer
all-local: Doxyfile footer
clean-local:
	rm -rf $(builddir)/doc/refman
	rm -rf $(builddir)/doc/manual
else
clean-local:
	rm -rf $(builddir)/doc/manual
endif

DISTCLEANFILES = LibreDWG.ps LibreDWG.{aux,cp,cps,log,t2p,toc}
distclean-local:
	-rm -rf manual refman latex
