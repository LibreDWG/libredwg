AUTOMAKE_OPTIONS = dejagnu

AM_CFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src @WARN_CFLAGS@
LDADD   = $(top_builddir)/src/libredwg.la -lm
#SOURCES = $(top_builddir)/src/libredwg.la

paired = \
	3dsolid \
	arc \
	attdef \
	attrib \
	bits_test \
	block \
	body \
	circle \
	decode_test \
	dim_common \
	dim_aligned \
	dim_ang2ln \
	dim_ang3pt \
	dim_diameter \
	dim_linear \
	dim_ordinate \
	dim_radius \
	ellipse \
	endblk \
	insert \
	line \
	lwpolyline \
	minsert \
	mline \
	mtext \
	ole2frame \
	point \
	polyline_2d \
	polyline_3d \
	polyline_mesh \
	polyline_pface \
	ray \
	region \
	seqend \
	shape \
	solid \
	text \
	tolerance \
	trace \
	vertex_3d \
	vertex_pface \
	xline \
	xrecord

unpaired = \
	vertex_2d \
	vertex_mesh

check_PROGRAMS = $(paired) $(unpaired)

# todo: more dwg versions, in test/test-data
EXTRA_DIST = \
	example_2000.dwg \
	vertex.mesh \
	tests_common.h common.c \
	$(paired:=.exp)

CLEANFILES = *.log *.sum site.bak 
MAINTAINERCLEANFILES = *_flymake.[ch] *~ *.i

bits_test.cbmc: bits_test.c ../../src/bits.c
	cbmc -I. -I../../include -I/opt/local/include bits_test.c ../../src/bits.c \
	     --unwind 6  --cover decision > bits_test.cbmc

@VALGRIND_CHECK_RULES@
#VALGRIND_SUPPRESSIONS_FILES = $(PACKAGE).supp
#EXTRA_DIST = $(PACKAGE).supp

# emacs flymake-mode
check-syntax:
	test -n "$(CHK_SOURCES)" && \
	  $(COMPILE) -o /dev/null -S $(CHK_SOURCES)
.PHONY: check-syntax

if ENABLE_GCOV
AM_CFLAGS += @GCOV_CFLAGS@
LIBS      += @GCOV_LIBS@
gcov: $(check_PROGRAMS) check
	-test -f gcov.log && rm gcov.log
	$(GCOV) *.gcno | tee -a $(builddir)/gcov.log

CLEANFILES  += *.gcov *.gcno *.gcda gcov.log
endif
