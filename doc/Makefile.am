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
EXTRA_DIST = $(srcdir)/ODA-5.4.2-libredwg.patch $(srcdir)/ODA-5.4.2-libredwg-diff.pdf
if HAVE_ODA_SPEC
ODA_SPEC_PATCH = $(top_srcdir)/../ODA-spec/ODA-5.4.2-libredwg.patch
ODA_SPEC_PDF = $(top_srcdir)/../ODA-spec/ODA-5.4.2-libredwg-diff.pdf
endif

.PHONY: manual regen-patch refman doxygen refman-pdf
if HAVE_MAKEINFO
manual:
	cd $(top_srcdir)/doc && ../build-aux/gendocs.sh LibreDWG "GNU LibreDWG - Manual"

endif

dynapi.texi: $(top_srcdir)/src/gen-dynapi.pl
	echo make regen-dynapi might be needed

if HAVE_ODA_SPEC
regen-patch: $(srcdir)/ODA-5.4.2-libredwg.patch $(srcdir)/ODA-5.4.2-libredwg-diff.pdf
$(srcdir)/ODA-5.4.2-libredwg.patch: $(ODA_SPEC_PATCH)
	-if [ -e $(ODA_SPEC_PATCH) ]; then \
           if ! cmp -s $(ODA_SPEC_PATCH) $(srcdir)/ODA-5.4.2-libredwg.patch; \
	     then cp -f $(ODA_SPEC_PATCH) $@; fi; fi
$(srcdir)/ODA-5.4.2-libredwg-diff.pdf: $(ODA_SPEC_PDF)
	-if [ -e $(ODA_SPEC_PDF) ]; then \
           if ! cmp -s $(ODA_SPEC_PDF) $(srcdir)/ODA-5.4.2-libredwg-diff.pdf; \
	     then cp -f $(ODA_SPEC_PDF) $@; fi; fi
endif

if HAVE_DOXYGEN
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
