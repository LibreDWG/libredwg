## Makefile.am
##
## Copyright (C) 2010,2018-2023 Free Software Foundation, Inc.
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
SUBDIRS = codepages
lib_LTLIBRARIES = libredwg.la
WARN_CFLAGS = @WARN_CFLAGS@
AM_CFLAGS   = -I$(top_srcdir)/include -I. $(WARN_CFLAGS)
if ENABLE_SHARED
AM_CFLAGS += -DDLL_EXPORT
endif
libredwg_la_LDFLAGS = -version-info $(LIBREDWG_SO_VERSION) -no-undefined -lm @LIBICONV@
if ENABLE_MIMALLOC
libredwg_la_LDFLAGS += -lmimalloc
endif
else
if HAVE_LIBGC
libredwg_la_SOURCES += -lgc
endif
endif
CPPCHECK = @CPPCHECK@

libredwg_la_SOURCES = \
	dwg.c \
	common.c \
	codepages.c \
	bits.c \
	decode.c \
        decode_r11.c \
        decode_r2007.c \
	reedsolomon.c \
        print.c \
        hash.c \
	dynapi.c \
	classes.c \
	dwg_api.c \
	objects.c \
	geom.c \
	$(EXTRA_HEADERS)
if !HAVE_LIBGC
libredwg_la_SOURCES += \
	free.c
endif
if !DISABLE_DXF
libredwg_la_SOURCES += \
        out_dxf.c \
        out_dxfb.c
#       out_xml.c \
#       out_yaml.c
if !DISABLE_JSON
libredwg_la_SOURCES += \
        out_json.c \
        out_geojson.c
endif
endif
if USE_WRITE
libredwg_la_SOURCES += \
	encode.c \
	dxfclasses.c
#       encode_r2007.c
#       in_xml.c
#       in_yaml.c
if !DISABLE_DXF
libredwg_la_SOURCES += \
	in_dxf.c
endif
if !DISABLE_JSON
libredwg_la_SOURCES += \
	in_json.c
endif
endif

EXTRA_HEADERS = \
	dwg.spec \
        header_variables.spec \
        header_variables_r11.spec \
        header_variables_dxf.spec \
        common_entity_data.spec \
        common_entity_handle_data.spec \
        common_object_handle_data.spec \
        header.spec \
        auxheader.spec \
        2ndheader.spec \
	r2004_file_header.spec \
        summaryinfo.spec \
        acds.spec \
        appinfo.spec \
        filedeplist.spec \
        security.spec \
        vbaproject.spec \
        template.spec \
        objfreespace.spec \
        revhistory.spec \
	classes.inc \
	objects.inc \
	spec.h \
	common.h \
	importer.h \
	classes.h \
	codepages.h \
	bits.h \
	myalloca.h \
	decode.h \
	decode_r11.h \
	dec_macros.h \
	free.h \
        print.h \
	logging.h \
	reedsolomon.h \
        hash.h \
	dynapi.h \
	out_json.h \
	geom.h
if !DISABLE_DXF
EXTRA_HEADERS += \
	out_dxf.h
#       out_xml.h
#       out_yaml.h
endif
if USE_WRITE
EXTRA_HEADERS += \
	encode.h
if !DISABLE_DXF
EXTRA_HEADERS += \
	in_dxf.h
#       in_xml.h
#       in_yaml.h
if !DISABLE_JSON
EXTRA_HEADERS += \
	in_json.h
endif
endif
endif

EXTRA_DIST = \
	$(include_HEADERS) \
	$(EXTRA_HEADERS) \
	gen-dynapi.pl \
	dxfclasses.in \
	objects.in

# maintainer only:
$(srcdir)/dynapi.c: $(srcdir)/gen-dynapi.pl $(top_srcdir)/include/dwg.h
	echo make regen-dynapi might be needed
if HAVE_GPERF
$(srcdir)/dxfclasses.c: $(srcdir)/dxfclasses.in
	-cd $(abs_top_srcdir) && \
	  $(GPERF) --output-file src/dxfclasses.tmp.c src/dxfclasses.in && \
	  sed -e's,unsigned int hval = len;,unsigned int hval = len \& UINT_MAX;,' \
	      -e's,unsigned int len,size_t len,;' -e 's,register ,,g' <src/dxfclasses.tmp.c \
	                                                              >src/dxfclasses.c && \
	  rm -f src/dxfclasses.tmp.c
$(srcdir)/objects.c: $(srcdir)/objects.in
	-cd $(abs_top_srcdir) && \
	  $(GPERF) --output-file src/objects.tmp.c src/objects.in && \
	  sed -e's,unsigned int hval = len;,unsigned int hval = len \& UINT_MAX;,' \
              -e's,unsigned int len,size_t len,; s,register ,,g' <src/objects.tmp.c >src/objects.c && \
	  rm -f src/objects.tmp.c
endif

.c.i:
	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.o$$||'`;\
	$(COMPILE) -E -o $@ $<

.c.ii: # no linemarkers
	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.o$$||'`;\
	$(COMPILE) -E -P -o $@ $<

.c.ic: # replace the original .c with the expanded version.
	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.o$$||'`;\
	$(COMPILE) -E -P -o $@ $<
	$(CLANG_FORMAT) -i $@
	$(am__mv) $< $<.orig
	$(am__mv) $@ $<

cppcheck:
if HAVE_CPPCHECK
	$(CPPCHECK) -j4 --inline-suppr -I$(top_srcdir)/include -I. $(srcdir)/*.c
endif

clang-tidy: ../compile_commands.json
	$(CLANG_TIDY) --checks='-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling' $(srcdir)/*.c

# emacs flymake-mode
check-syntax:
	test -n "$(CHK_SOURCES)" && \
	  nice $(COMPILE) -I. -O0 -o /dev/null -S $(CHK_SOURCES)
.PHONY: check-syntax cppcheck clang-tidy

if ENABLE_GCOV
AM_CFLAGS += @GCOV_CFLAGS@
LIBS      += @GCOV_LIBS@
gcov:
	-test -f $(builddir)/gcov.log && rm $(builddir)/gcov.log
	$(GCOV) -lp *.gcno | tee -a $(builddir)/gcov.log
endif
MAINTAINERCLEANFILES = *~ *.i *_flymake.[ch]
if ENABLE_GCOV
CLEANFILES  = *.gcov *.gcno *.gcda gcov.log
endif
