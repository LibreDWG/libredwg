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

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src programs examples test
if HAVE_SWIG_PYTHON
if !DISABLE_BINDINGS
SUBDIRS += bindings
endif
endif
if HAVE_MAKEINFO
if !DISABLE_DOCS
SUBDIRS += doc
endif
endif
AM_DISTCHECK_CONFIGURE_FLAGS = --enable-release
if DISABLE_BINDINGS
AM_DISTCHECK_CONFIGURE_FLAGS += --disable-bindings
endif
DIST_SUBDIRS = $(SUBDIRS)

include_HEADERS = \
	include/dwg.h \
	include/dwg_api.h

pcdatadir = $(libdir)/pkgconfig
pcdata_DATA = libredwg.pc
BUILT_SOURCES = $(top_srcdir)/.version
LTEXEC = @LTEXEC@
SHFMT = @SHFMT@

VALGRIND_SUPPRESSIONS = @VALGRIND_SUPPRESSIONS@
VALGRIND_SUPPRESSIONS_FILES = @VALGRIND_SUPPRESSIONS_FILE@
VALGRIND_OPTS = --leak-check=full --show-reachable=no --num-callers=30
JSON_DIST = jsmn/jsmn.h jsmn/LICENSE jsmn/README.md
doc_DIST = README HACKING NEWS TODO
EXTRA_DIST = $(top_srcdir)/.version libredwg.spec vcpkg.json libredwg-schema.json \
	     build-aux/Dockerfile \
	     dwg.in dxf.in dxf-allcvt.lsp dxf-allcvt.sh test-dxf.sh \
	     dxf-check dxfb-check json-check oda \
	     json.in log.in svg.in rw.in geojson.in \
	     logs-all-parallel.sh.in logs-all-serial.sh.in unknown.sh \
	     llvmfuzz_standalone_tz.sh llvmfuzz_standalone_data.sh \
             dxf-allcvt.lsp dxf-allcvt.sh test-dxf.sh \
	     cmp-objs.pl .dockerignore \
	     build-aux/swig_python.patch \
             $(JSON_DIST) \
	     $(VALGRIND_SUPPRESSIONS_FILES)
shell_sources = dwg.in dxf.in dxf-allcvt.sh test-dxf.sh dxf-check dxfb-check \
	json-check oda json.in log.in svg.in rw.in geojson.in \
	logs-all-parallel.sh.in logs-all-serial.sh.in unknown.sh \
	dynapi_test_all.sh.in unit_testing_all.sh.in \
	llvmfuzz_standalone_tz.sh llvmfuzz_standalone_data.sh \
	autogen.sh dxf-roundtrip.sh unit_testing_all.sh.in test-dxf.sh
	build-aux/gendocs.sh build-aux/codecov_io.sh build-aux/smoke.sh \
	build-aux/clang-format-all.sh \
	programs/dwgfilter.in programs/alive.test.in programs/dxf.test.in \
	programs/json.test.in programs/svg.test.in \
	examples/dwgadd_test.sh.in

.PHONY: check-dwg check-dxf check-dxfb check-json check-dwg-valgrind check-svg \
	check-docker cppcheck log-dwg shfmt regen-shfmt check-shellcheck \
	check-minimal regen-swig-patch release-web \
        regen-man man manual refman scan-build gcov cover codecov coveralls \
	unknown regen-unknown full-regen-unknown regen-gperf regen-dynapi \
	maintainer-clean-local distuninstallcheck dist-hook

dist-hook: regen-dynapi
	echo dist-hook distdir="$(distdir)"
	echo $(VERSION) > $(distdir)/.tarball-version
	test -f $(distdir)/test/xmlsuite/Makefile.in
	test -f $(distdir)/bindings/python/Makefile.in
	test -f $(distdir)/bindings/perl/Makefile.PL
	test -f $(distdir)/programs/dwg2ps.1
	test -f $(distdir)/examples/dwgadd.1
if !HAVE_LIBPS
	echo "Missing libps" && false
endif
if !HAVE_MAKEINFO
	echo "Missing makeinfo" && false
endif
if DISABLE_DOCS
	echo "config --disable-docs" && false
endif

# automake bug, GH #211 (howto enable skip_mode_fix? tried now info-in-builddir)
#distcheck-hook:
#	echo distcheck-hook distdir="$(distdir)"
#	chmod u+w "$(distdir)/doc"
#	chmod u+w "$(distdir)/doc/LibreDWG.info"

UNKNOWN_LOG = unknown-`git describe --long --tags --dirty --always`.log
UNKNOWN_SKIP = alldwg.skip
TIMEOUT_30 = @TIMEOUT_30@

unknown:
	-mv examples/alldwg.skip examples/alldwg.skip~
	$(MAKE) -C src bits.lo && \
	  $(MAKE) -C examples unknown$(EXEEXT) bd$(EXEEXT) bits$(EXEEXT) && \
	  $(LTEXEC) examples/unknown$(EXEEXT) >examples/$(UNKNOWN_LOG) 2>examples/$(UNKNOWN_SKIP)
	if test -s examples/alldwg.skip; then true; \
	else mv examples/alldwg.skip~ examples/alldwg.skip; fi
if HAVE_PICAT
if !HAVE_INSRCDIR
	test -e examples/unknown.pi || ln -s $(abs_srcdir)/examples/unknown.pi unknown.pi
endif
if HAVE_PARALLEL
	-rm examples/*.pi.tmplog 2>/dev/null
	echo $(PARALLEL) $(TIMEOUT_30) $(PICAT) "{} \\>{}.tmplog ::: examples/[A-Z]*.pi"
	-$(PARALLEL) $(TIMEOUT_30) $(PICAT) {} \>{}.tmplog ::: examples/[A-Z]*.pi
	-for f in examples/*.pi.tmplog; do \
	   if test -n "`grep 'Most probable result:' $$f`"; then \
	     mv $$f examples/`basename $$f .tmplog`.log; fi; done
	-rm examples/*.pi.tmplog 2>/dev/null
else
	-for f in examples/[A-Z]*.pi; do \
	  echo $(TIMEOUT_30) picat $$f; \
	  $(TIMEOUT_30) $(PICAT) $$f >$$f.tmplog; \
	  if test -n "`grep 'Most probable result:' $$f.tmplog`"; then \
	    mv $$f.tmplog $$f.log; fi; done
	-rm examples/[A-Z]*.pi.tmplog 2>/dev/null
endif
endif

regen-dynapi: $(srcdir)/src/gen-dynapi.pl $(srcdir)/include/dwg.h
	-$(PERL) $(srcdir)/src/gen-dynapi.pl
# $(MAKE) -C src $(abs_top_srcdir)/src/dxfclasses.c
regen-gperf:
	-cd $(abs_top_srcdir) && \
	  $(GPERF) --output-file src/dxfclasses.tmp.c src/dxfclasses.in && \
	  sed -e's,unsigned int hval = len;,unsigned int hval = len \& UINT_MAX;,' \
	      -e's,unsigned int len,size_t len,;' -e 's,register ,,g' <src/dxfclasses.tmp.c \
	                                                              >src/dxfclasses.c && \
	  rm -f src/dxfclasses.tmp.c && \
	  $(GPERF) --output-file src/objects.tmp.c src/objects.in && \
	  sed -e's,unsigned int hval = len;,unsigned int hval = len \& UINT_MAX;,' \
	      -e's,unsigned int len,size_t len,' -e's,register ,,g' <src/objects.tmp.c \
	                                                            >src/objects.c && \
	  rm -f src/objects.tmp.c
regen-unknown:
	$(MAKE) -C examples $@
full-regen-unknown:
	$(MAKE) -C examples $@
regen-man:
	$(MAKE) -C programs $@
	$(MAKE) -C examples $@
man:
	$(MAKE) -C programs $@
	$(MAKE) -C examples $@
manual:
	$(MAKE) -C doc $@
refman:
	$(MAKE) -C doc $@
regen-swig-patch:
if !DISABLE_BINDINGS
	$(MAKE) -C bindings/python
	-difforig bindings/python/swig_python.c >xxxx
	if test -s xxxx; then mv xxxx build-aux/swig_python.patch; fi
endif

WEB_DIR = $(srcdir)/../libredwg-web
release-web: manual refman
	if test -d $(WEB_DIR)/manual; then \
	  rsync -avzC $(srcdir)/doc/manual/* $(WEB_DIR)/manual/; \
	  rsync -avzC doc/refman/* $(WEB_DIR)/refman/; \
	fi
	-cd $(WEB_DIR) && \
	  cvs add -m"new with $(VERSION)" manual/index.html manual/LibreDWG.* manual/html_node/*.html \
	    refman/*.{html,js,png} refman/search/*.{html,js}; cd -
	-cd $(WEB_DIR); cvs commit -m"update to $(VERSION)"; cd -

TESTS =
if HAVE_SHELLCHECK
# fragile, serial-tests only. SPEC_LOG_COMPILER does not work there,
# and parallel cannot do srcdir TESTS
TESTS += log dwg dxf json svg rw logs-all.sh dynapi_test_all.sh unit_testing_all.sh \
	 $(srcdir)/json-check $(srcdir)/dxf-check $(srcdir)/dxfb-check \
	 $(srcdir)/dxf-allcvt.sh $(srcdir)/test-dxf.sh $(srcdir)/unknown.sh \
         programs/dwgfilter \
	 programs/alive.test programs/dxf.test programs/json.test programs/svg.test \
	 $(srcdir)/llvmfuzz_standalone_tz.sh $(srcdir)/llvmfuzz_standalone_data.sh \
	 examples/dwgadd_test.sh
TESTS_ENVIRONMENT = $(SHELLCHECK)
check-shellcheck:
	$(SHELLCHECK) $(TESTS)
# TEST_EXTENSIONS = .sh
# SPEC_LOG_COMPILER = $(SHELLCHECK)
else
if HAVE_RPMLINT
# serial-tests only, see above. only with parallel-tests we can do both
TESTS += $(srcdir)/libredwg.spec
TESTS_ENVIRONMENT = $(RPMLINT)
# TEST_EXTENSIONS = .spec
# SPEC_LOG_COMPILER = $(RPMLINT)
endif
endif

check-minimal:
	$(MAKE) -C src
	$(MAKE) -C programs $@
	$(MAKE) -C test/unit-testing $@
check-dwg: all
	-mv check-dwg.log check-dwg.log~ 2>/dev/null
	for d in `find $(srcdir)/test/test-data -iname \*.dwg -size -40M`; do \
	  echo programs/dwgread -v3 $$d | tee -a check-dwg.log; \
	  $(LTEXEC) programs/dwgread -v3 $$d >> check-dwg.log 2>&1; \
	done
check-dwg-valgrind: all
	-mv check-dwg-valgrind.log check-dwg-valgrind.log~ 2>/dev/null
	for d in `find $(srcdir)/test/test-data -iname \*.dwg -size -10M`; do \
	  echo $(VALGRIND) $(VALGRIND_OPTS) $(VALGRIND_SUPPRESSIONS) programs/dwgread $$d | tee -a check-dwg-valgrind.log; \
	  $(VALGRIND) $(VALGRIND_OPTS) $(VALGRIND_SUPPRESSIONS) $(LTEXEC) programs/dwgread $$d >> check-dwg-valgrind.log 2>&1; \
	done
check-dxf: all
	-for f in $(srcdir)/test/test-data/Drawing_*.dwg \
                  $(srcdir)/test/test-data/sample_*.dwg \
                  $(srcdir)/test/test-data/example_*.dwg \
                  $(srcdir)/test/test-data/r*/entities*.dwg; \
          do \
            env MAKEFLAGS= $(srcdir)/dxf-check $$f; \
          done
	-for d in r14 2000 2004 2007 2010 2013 2018; do \
          for f in $(srcdir)/test/test-data/$$d/*.dwg; do \
            env MAKEFLAGS= $(srcdir)/dxf-check $$f; \
	  done; \
	done
check-dxfb: all
	-for f in $(srcdir)/test/test-data/Drawing_*.dwg \
                  $(srcdir)/test/test-data/sample_*.dwg \
                  $(srcdir)/test/test-data/example_*.dwg \
                  $(srcdir)/test/test-data/r*/entities*.dwg; \
          do \
            env MAKEFLAGS= $(srcdir)/dxfb-check $$f; \
          done
	-for d in r14 2000 2004 2007 2010 2013 2018; do \
          for f in $(srcdir)/test/test-data/$$d/*.dwg; do \
            env MAKEFLAGS= $(srcdir)/dxfb-check $$f; \
	  done; \
	done
check-json: all
	-for f in $(srcdir)/test/test-data/Drawing_*.dwg \
                  $(srcdir)/test/test-data/sample_*.dwg \
                  $(srcdir)/test/test-data/example_*.dwg \
                  $(srcdir)/test/test-data/r*/entities*.dwg; \
          do \
            env MAKEFLAGS= $(srcdir)/json-check $$f; \
          done
	-for d in r14 2000 2004 2007 2010 2013 2018; do \
          for f in $(srcdir)/test/test-data/$$d/*.dwg; do \
            env MAKEFLAGS= $(srcdir)/json-check $$f; \
	  done; \
	done
check-svg: all
	-for f in $(srcdir)/test/test-data/Drawing_*.dwg \
            $(srcdir)/test/test-data/sample_*.dwg \
            $(srcdir)/test/test-data/example_*.dwg \
	    $(srcdir)/test/test-data/r*/entities*.dwg; \
          do env MAKEFLAGS= ./svg $$f; done
	-for d in r14 2000 2004 2007 2010 2013 2018; do \
	   for f in $(srcdir)/test/test-data/$$d/*.dwg; \
       do env MAKEFLAGS= ./svg $$f; done; \
	done
check-docker: dist
	docker build --build-arg distdir="$(distdir)" --rm -t $(distdir) \
	             -f $(srcdir)/Dockerfile-dist .
cppcheck:
if HAVE_CPPCHECK
	$(MAKE) -C src $@
	$(MAKE) -C programs $@
	$(MAKE) -C examples $@
	$(CPPCHECK) -DDEBUG_CLASSES -DUSE_WRITE -DHAVE_SCANDIR -j4 --inline-suppr \
		-I$(top_srcdir)/include -Isrc test/unit-testing/*.c
	$(CPPCHECK) -DDEBUG -j4 --inline-suppr -I$(top_srcdir)/include -Isrc \
		test/xmlsuite/*.c
endif

SHFMT_FLAGS=-i 4 -p
shfmt:
if HAVE_SHFMT
	for s in $(shell_sources); do \
	  $(SHFMT) $(SHFMT_FLAGS) -d $(top_srcdir)/$$s; done
endif
regen-shfmt:
if HAVE_SHFMT
	for s in $(shell_sources); do \
	  $(SHFMT) $(SHFMT_FLAGS) -w $(top_srcdir)/$$s; done
endif

log-dwg: all
	find $(srcdir)/test/test-data $(srcdir)/test/test-old $(srcdir)/test/issues \
	     $(srcdir)/test/ossfuzz -type f -iname \*.dwg -size -40M | \
          xargs -n1 -P8 ./log
	find $(srcdir)/test/ossfuzz/indwg* -type f -size -40M | \
	  xargs -n1 -P8 ./log

check-ossfuzz: all
	+find $(srcdir)/test/ossfuzz -type f -print0 -iname \*.dwg -size -40M | \
          xargs -0 -n1 -P8 ./log "$d"
	+find $(srcdir)/test/ossfuzz/indwg* -type f -print0 -size -40M | \
          xargs -0 -n1 -P8 ./log "$d"
	+find $(srcdir)/test/ossfuzz/outdxf* -type f -print0 -size -40M | \
          xargs -0 -n1 -P8 ./dxf "$d"
	+find $(srcdir)/test/ossfuzz/outjson -type f -print0 -size -40M | \
	  xargs -0 -n1 -P8 ./json "$d"
	+find $(srcdir)/test/ossfuzz/outgeojson -type f -print0 -size -40M | \
	  xargs -0 -n1 -P8 $(LTEXEC) programs/dwgread -Ogeojson -o/dev/null "$$d"
	+find $(srcdir)/test/ossfuzz/in{dxf,dxfb,json} -type f -size -40M | \
	  xargs -0 -n1 -P8 ./dwg "$d"
	+find $(srcdir)/test/ossfuzz/outsvg -type f -size -40M | \
	  xargs -0 -n1 -P8 ./svg "$d"

# clang-analyzer.llvm.org, debian: clang-tools-{6.0,7}, redhat: clang-analyzer
SCAN_BUILD = scan-build
scan-build: clean
	$(SCAN_BUILD) -V -o $(srcdir)/.analysis $(MAKE) -j4 &

if ENABLE_GCOV
gcov: check
	$(MAKE) -C test gcov
	$(MAKE) -C src gcov
	$(MAKE) -C programs gcov
	$(MAKE) -C examples gcov
cover: gcov
	gcov2perl test/*/*.gcov src/*.gcov programs/*.gcov examples/*.gcov
	cover -no-gcov
codecov: check
	-(cd src && mkdir gcno && mv *.gcno gcno/ && \
	  for g in .libs/*.gcda .libs/*.gcno; do ln -s $g; done)
	-$(srcdir)/build-aux/codecov_io.sh -X test -X bindings
coveralls: gcov
	coveralls -e test -e bindings
endif

compile_commands.json: Makefile
	+$(MAKE) clean
	bear -- make

$(BUILT_SOURCES):
	echo $(VERSION) > $@-t && mv $@-t $@

CLEANFILES = check-dwg.log check-dwg-valgrind.log check-dxf.log \
             check-dwg.log~ check-dwg-valgrind.log~ check-dxf.log~ \
	     *_20*.dxf *_r1*.dxf example_*.dxf.log
if ENABLE_GCOV
CLEANFILES += codecov_io.sh *.gcov
endif
DISTCLEANFILES  = logs-all.sh dynapi_test_all.sh unit_testing_all.sh log json dwg dxf svg rw libredwg.pc vcpkg.json
MAINTAINERCLEANFILES  = *~ *.log *.dxf *.json *.dwg cover_db .analysis

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

# cannot uninstall the generated perllocal.pod and share/info/dir
distuninstallcheck:
	@:
