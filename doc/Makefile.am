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

refman doxygen refman/index.html: Doxyfile ../include/dwg_api.h ../include/dwg.h
	$(MAKE) -C .. refman

refman-pdf: refman
	$(MAKE) -C latex

DISTCLEANFILES = LibreDWG.pdf LibreDWG.{aux,cp,cps,log,t2p,toc}
distclean-local:
	-rm -rf manual refman latex
