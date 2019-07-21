AUTOMAKE_OPTIONS = dejagnu

WARN_CFLAGS = @WARN_CFLAGS@
AM_CFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src $(WARN_CFLAGS) @DEJAGNU_CFLAGS@
LDADD   = $(top_builddir)/src/libredwg.la -lm

hash_test_LDADD = $(LDADD) $(top_builddir)/src/bits.lo $(top_builddir)/src/hash.lo

paired = \
	3dsolid \
	arc \
	attdef \
	attrib \
	block \
	body \
	circle \
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
	lwpline \
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

private = decode_test \
	  hash_test

check_PROGRAMS = $(paired) $(unpaired) $(private)

# todo: more dwg versions, from test/test-data
EXTRA_DIST = \
	example_2000.dwg \
	vertex.mesh \
	tests_common.h common.c \
	$(paired:=.exp) \
	$(private:=.exp)

CLEANFILES = *.log *.sum site.bak 
MAINTAINERCLEANFILES = *_flymake.[ch] *~ *.i

.c.i:
	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.o$$||'`;\
	$(COMPILE) -MT $@ -MD -MP -MF $$depbase.Tpo -c -E -o $@ $<

.c.ii: # no linemarkers
	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.o$$||'`;\
	$(COMPILE) -MT $@ -MD -MP -MF $$depbase.Tpo -c -E -P -o $@ $<

@VALGRIND_CHECK_RULES@
VALGRIND_SUPPRESSIONS_FILE = @VALGRIND_SUPPRESSIONS_FILE@
VALGRIND_SUPPRESSIONS = @VALGRIND_SUPPRESSIONS@

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
