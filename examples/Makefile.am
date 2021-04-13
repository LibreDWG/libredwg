## Makefile.am
##
## Copyright (C) 2010-2021 Free Software Foundation, Inc.
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

WARN_CFLAGS = @WARN_CFLAGS@
AM_CFLAGS   = -I$(top_srcdir)/include -I$(top_srcdir)/src $(WARN_CFLAGS)
LDADD       = $(top_builddir)/src/libredwg.la
CPPCHECK    = @CPPCHECK@
if HAVE_LIBGC
LDADD      += -lgc
endif

if USE_WRITE
bin_PROGRAMS     = dwgadd
endif
check_PROGRAMS   = load_dwg dwg2svg2 dwgfuzz
EXTRA_PROGRAMS   = unknown bd bits llvmfuzz llvmfuzz_standalone odaversion

load_dwg_SOURCES = load_dwg.c
load_dwg_LDADD   = $(top_builddir)/src/libredwg.la
if !HAVE_MINGW
load_dwg_LDADD   += $(top_builddir)/src/bits.lo $(top_builddir)/src/codepages.lo
endif
dwg2svg2_SOURCES = dwg2svg2.c
dwgadd_SOURCES   = dwgadd.c
unknown_SOURCES  = unknown.c
# alldxf_0.inc alldxf_1.inc alldxf_2.inc alldwg.inc bd-unknown.inc
unknown_LDADD    = $(LDADD) \
                   $(top_builddir)/src/bits.lo $(top_builddir)/src/common.lo \
	           $(top_builddir)/src/codepages.lo
bd_SOURCE        = bd.c
bd_LDADD         = $(LDADD) $(top_builddir)/src/bits.lo \
	           $(top_builddir)/src/codepages.lo
bits_SOURCE      = bits.c
bits_LDADD       = $(LDADD) $(top_builddir)/src/bits.lo \
	           $(top_builddir)/src/codepages.lo
dwgfuzz_SOURCES  = dwgfuzz.c
#would need HAVE_ASAN am var set
#dwgfuzz_CFLAGS             = $(CFLAGS) $(AM_CFLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer
llvmfuzz_SOURCES            = llvmfuzz.c
llvmfuzz_LDADD              = $(LDADD) $(top_builddir)/src/bits.lo \
	                      $(top_builddir)/src/codepages.lo
llvmfuzz_standalone_SOURCES = llvmfuzz.c
if HAVE_ASAN_OR_LINUX
# needs CC=clang
llvmfuzz_CFLAGS             = $(CFLAGS) $(AM_CFLAGS) -DHAVE_ASAN \
                              -fsanitize=fuzzer,address,undefined -fno-omit-frame-pointer
# cflags already includes asan
if HAVE_ASAN
llvmfuzz_standalone_CFLAGS  = $(CFLAGS) $(AM_CFLAGS) -DSTANDALONE
else
llvmfuzz_standalone_CFLAGS  = $(CFLAGS) $(AM_CFLAGS) -DSTANDALONE -DHAVE_ASAN \
                              -fsanitize=address,undefined -fno-omit-frame-pointer
endif
else
llvmfuzz_standalone_CFLAGS  = $(CFLAGS) $(AM_CFLAGS) -DSTANDALONE
endif
llvmfuzz_standalone_LDADD   = $(LDADD) $(top_builddir)/src/bits.lo \
	                      $(top_builddir)/src/codepages.lo
odaversion_SOURCES          = odaversion.c

all: $(check_PROGRAMS)

.PHONY: check-syntax regen-unknown full-regen-unknown dsymutil gcov cppcheck \
	check-minimal

if HAVE_PERL
# maintainer rules only
regen-unknown:
	echo "use existing $(srcdir)/alldwg.inc"
	-mv $(srcdir)/alldxf_0.inc alldxf_0.inc~ 2>/dev/null
	-mv $(srcdir)/alldxf_1.inc alldxf_1.inc~ 2>/dev/null
	-mv $(srcdir)/alldxf_2.inc alldxf_2.inc~ 2>/dev/null
	(cd .. && $(PERL) $(abs_srcdir)/log_unknown_dxf.pl $(abs_srcdir)/alldwg.inc)
	-$(MAKE) -C .. unknown
	$(PERL) $(srcdir)/bd-unknown.pl | sort | uniq -w 96 >$(srcdir)/bd-unknown.inc

full-regen-unknown:
	echo Creating $(srcdir)/alldwg.inc will need a few minutes...
	-rm *_r??.log *_20??.log DS_*.log 2>/dev/null
	-(cd .. && ./logs-all.sh; \
	  rm alldwg.tmp 2>/dev/null; \
	  cp $(abs_srcdir)/alldwg.inc examples/alldwg.inc~; \
	  for l in *_r??.log *_20??.log DS_*.log; \
	    do $(PERL) $(abs_srcdir)/log_unknown.pl "$$l" >>alldwg.tmp; done; \
          grep -v '^//' alldwg.tmp | sort -u >$(abs_srcdir)/alldwg.inc; rm alldwg.tmp)
	-mv $(srcdir)/alldxf_0.inc alldxf_0.inc~ 2>/dev/null
	-mv $(srcdir)/alldxf_1.inc alldxf_1.inc~ 2>/dev/null
	-mv $(srcdir)/alldxf_2.inc alldxf_2.inc~ 2>/dev/null
	(cd .. && $(PERL) $(abs_srcdir)/log_unknown_dxf.pl $(abs_srcdir)/alldwg.inc)
	-$(MAKE) -C .. unknown
	$(PERL) $(srcdir)/bd-unknown.pl | sort | uniq -w 96 >$(srcdir)/bd-unknown.inc

#ACAD_EVALUATION_GRAPH.pi: unknown
#	$(MAKE) -C .. unknown

$(srcdir)/bd-unknown.inc: bd-unknown.pl alldxf_0.inc  ACAD_EVALUATION_GRAPH.pi
$(srcdir)/alldxf_2.inc: alldxf_1.inc
$(srcdir)/alldxf_1.inc: alldxf_0.inc
$(srcdir)/alldxf_0.inc: alldwg.inc log_unknown_dxf.pl
$(srcdir)/alldwg.inc: log_unknown.pl
endif

if HAVE_WINE
TESTS_ENVIRONMENT = \
 PROGS='$(check_PROGRAMS)' \
 DATADIR='$(top_srcdir)/test/test-data' \
 TESTPROG='wine'
else
TESTS_ENVIRONMENT = \
 PROGS='$(check_PROGRAMS)' \
 DATADIR='$(top_srcdir)/test/test-data' \
 TESTPROG=''
endif
if CHECK_LESS
TESTS_ENVIRONMENT += CHECK_MINIMAL=1
endif

TESTS = $(top_builddir)/programs/alive.test
if USE_WRITE
TESTS += dwgadd_test.sh
endif

check-minimal:

cppcheck:
if HAVE_CPPCHECK
	$(CPPCHECK) -j4 --inline-suppr -I$(top_srcdir)/include -I../src $(srcdir)/*.c
endif

alldxf_unknowns = \
	alldxf_ACAD_EVALUATION_GRAPH.inc \
	alldxf_ACAD_TABLE.inc \
	alldxf_ACDB_ALDIMOBJECTCONTEXTDATA_CLASS.inc \
	alldxf_ACDBASSOC2DCONSTRAINTGROUP.inc \
	alldxf_ACDBASSOCACTION.inc \
	alldxf_ACDBASSOCALIGNEDDIMACTIONBODY.inc \
	alldxf_ACDBASSOCARRAYACTIONBODY.inc \
	alldxf_ACDBASSOCDEPENDENCY.inc \
	alldxf_ACDBASSOCEDGEACTIONPARAM.inc \
	alldxf_ACDBASSOCEXTRUDEDSURFACEACTIONBODY.inc \
	alldxf_ACDBASSOCGEOMDEPENDENCY.inc \
	alldxf_ACDBASSOCLOFTEDSURFACEACTIONBODY.inc \
	alldxf_ACDBASSOCNETWORK.inc \
	alldxf_ACDBASSOCOSNAPPOINTREFACTIONPARAM.inc \
	alldxf_ACDBASSOCPATHACTIONPARAM.inc \
	alldxf_ACDBASSOCPERSSUBENTMANAGER.inc \
	alldxf_ACDBASSOCPLANESURFACEACTIONBODY.inc \
	alldxf_ACDBASSOCREVOLVEDSURFACEACTIONBODY.inc \
	alldxf_ACDBASSOCSWEPTSURFACEACTIONBODY.inc \
	alldxf_ACDBASSOCVALUEDEPENDENCY.inc \
	alldxf_ACDBASSOCVARIABLE.inc \
	alldxf_ACDBASSOCVERTEXACTIONPARAM.inc \
	alldxf_ACDBDETAILVIEWSTYLE.inc \
	alldxf_ACDBPERSSUBENTMANAGER.inc \
	alldxf_ACDBPOINTCLOUDCOLORMAP.inc \
	alldxf_ACDBSECTIONVIEWSTYLE.inc \
	alldxf_ACDB_BLKREFOBJECTCONTEXTDATA_CLASS.inc \
	alldxf_ACDB_BLOCKREPRESENTATION_DATA.inc \
	alldxf_ACDB_DYNAMICBLOCKPROXYNODE.inc \
	alldxf_ACDB_LEADEROBJECTCONTEXTDATA_CLASS.inc \
	alldxf_ACDB_MLEADEROBJECTCONTEXTDATA_CLASS.inc \
	alldxf_ACDB_MTEXTATTRIBUTEOBJECTCONTEXTDATA_CLASS.inc \
	alldxf_ACDB_MTEXTOBJECTCONTEXTDATA_CLASS.inc \
	alldxf_ACDB_TEXTOBJECTCONTEXTDATA_CLASS.inc \
	alldxf_ACMECOMMANDHISTORY.inc \
	alldxf_ACMESCOPE.inc \
	alldxf_ACMESTATEMGR.inc \
	alldxf_ACSH_BOX_CLASS.inc \
	alldxf_ACSH_CHAMFER_CLASS.inc \
	alldxf_ACSH_EXTRUSION_CLASS.inc \
	alldxf_ACSH_HISTORY_CLASS.inc \
	alldxf_ACSH_LOFT_CLASS.inc \
	alldxf_ACSH_REVOLVE_CLASS.inc \
	alldxf_ACSH_SWEEP_CLASS.inc \
	alldxf_ARC_DIMENSION.inc \
	alldxf_ASSOCDIMDEPENDENCYBODY.inc \
	alldxf_ATTDEF.inc \
	alldxf_BLOCKALIGNMENTGRIP.inc \
	alldxf_BLOCKARRAYACTION.inc \
	alldxf_BLOCKGRIPLOCATIONCOMPONENT.inc \
	alldxf_BLOCKLINEARPARAMETER.inc \
	alldxf_BLOCKLOOKUPACTION.inc \
	alldxf_BLOCKLOOKUPPARAMETER.inc \
	alldxf_BLOCKPARAMDEPENDENCYBODY.inc \
	alldxf_BLOCKPOLARSTRETCHACTION.inc \
	alldxf_BLOCKROTATIONPARAMETER.inc \
	alldxf_BLOCKSTRETCHACTION.inc \
	alldxf_BLOCKVISIBILITYGRIP.inc \
	alldxf_BLOCKVISIBILITYPARAMETER.inc \
	alldxf_BLOCKXYGRIP.inc \
	alldxf_BREAKDATA.inc \
	alldxf_CELLSTYLEMAP.inc \
	alldxf_CSACDOCUMENTOPTIONS.inc \
	alldxf_DATALINK.inc \
	alldxf_DATATABLE.inc \
	alldxf_DBCOLOR.inc \
	alldxf_DIMASSOC.inc \
	alldxf_EXTRUDEDSURFACE.inc \
	alldxf_GEODATA.inc \
	alldxf_HELIX.inc \
	alldxf_LAYOUTPRINTCONFIG.inc \
	alldxf_LIGHT.inc \
	alldxf_MATERIAL.inc \
	alldxf_MENTALRAYRENDERSETTINGS.inc \
	alldxf_MESH.inc \
	alldxf_MLEADERSTYLE.inc \
	alldxf_MULTILEADER.inc \
	alldxf_PLANESURFACE.inc \
	alldxf_PLOTSETTINGS.inc \
	alldxf_RAPIDRTRENDERSETTINGS.inc \
	alldxf_RENDERENTRY.inc \
	alldxf_RENDERGLOBAL.inc \
	alldxf_REVOLVEDSURFACE.inc \
	alldxf_SECTIONOBJECT.inc \
	alldxf_SECTION_MANAGER.inc \
	alldxf_SECTION_SETTINGS.inc \
	alldxf_SKYLIGHT_BACKGROUND.inc \
	alldxf_SPATIAL_INDEX.inc \
	alldxf_SUN.inc \
	alldxf_SWEPTSURFACE.inc \
	alldxf_TABLEGEOMETRY.inc \
	alldxf_TABLESTYLE.inc \
	alldxf_VISUALSTYLE.inc \
	alldxf_WIPEOUT.inc

EXTRA_DIST = unknown.h alldxf_0.inc alldxf_1.inc alldxf_2.inc alldwg.inc $(alldxf_unknowns)
EXTRA_DIST += log_unknown_dxf.pl log_unknown.pl bd-unknown.pl unstable.pm dwgadd.pod dwgadd_test.sh
EXTRA_DIST += unknown.pi ACAD_EVALUATION_GRAPH.pi
EXTRA_man1_MANS = $(srcdir)/dwgfuzz.1
dist_man1_MANS = $(srcdir)/dwgadd.1
dist_man5_MANS = $(srcdir)/dwgadd.5
dist_pkgdata_DATA = dwgadd.example dwgadd.example_r11 dwgadd.example_r10 \
	dwgadd.example_r2_10 dwgadd.example_r1_4 load_dwg.py

CLEANFILES = {example,sample}_2000.svg alldwg.inc~ \
	     alldxf_0.inc~ alldxf_1.inc~ alldxf_2.inc~ dwgadd_test.dwg
DISTCLEANFILES = alldxf_0.inc alldxf_1.inc alldxf_2.inc
MAINTAINERCLEANFILES  = *_flymake.[ch] *~ *.i

.c.i:
	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.o$$||'`;\
	$(COMPILE) -MT $@ -MD -MP -MF $$depbase.Tpo -c -E -o $@ $<

dsymutil:
	for e in $(check_PROGRAMS) $(EXTRA_PROGRAMS); do dsymutil $$e; done
man: $(dist_man1_MANS) $(EXTRA_man1_MANS) $(dist_man5_MANS)

if !HAVE_MINGW_CROSS
# Note: These rules do not work cross-compiled
regen-man: dwgfuzz dwgadd
	-for p in $(dist_man1_MANS) $(EXTRA_man1_MANS); do \
	  base=`basename $$p .1`; \
	  help2man -p $(PACKAGE_NAME) -o $$p ./$$base$(EXEEXT); done
	-for p in $(dist_man5_MANS); do \
	  base=`basename $$p .5`; \
	  pod2man --release="@VERSION@" --section=5 --center="User Commands" $(srcdir)/$$base.pod >$$p; done
endif

.c.ic: # replace the original .c with the expanded version.
	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.o$$||'`;\
	$(COMPILE) -c -E -P -o $@ $<
	$(CLANG_FORMAT) -i $@
	$(am__mv) $< $<.orig
	$(am__mv) $@ $<

# emacs flymake-mode
check-syntax:
	test -n "$(CHK_SOURCES)" && \
	  nice $(COMPILE) -O0 -o /dev/null -S $(CHK_SOURCES)

if ENABLE_GCOV
AM_CFLAGS += @GCOV_CFLAGS@
LIBS      += @GCOV_LIBS@
gcov: $(CHECK)
	-test -f gcov.log && rm gcov.log
	$(GCOV) -lp *.gcno | tee -a $(builddir)/gcov.log

CLEANFILES  += *.gcov *.gcno *.gcda gcov.log
endif

@VALGRIND_CHECK_RULES@
VALGRIND_SUPPRESSIONS_FILES = @VALGRIND_SUPPRESSIONS_FILE@
VALGRIND_SUPPRESSIONS = @VALGRIND_SUPPRESSIONS@
