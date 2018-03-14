AUTOMAKE_OPTIONS = dejagnu

AM_CFLAGS = @WARN_CFLAGS@
LDADD = $(top_builddir)/src/libredwg.la

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
  xline

unpaired = \
  vertex_2d \
  vertex_mesh

check_PROGRAMS = $(paired) $(unpaired)

EXTRA_DIST = \
  example.dwg \
  vertex.mesh \
  tests_common.h common.c ole2frame.c \
  $(paired:=.exp)

CLEANFILES = *.log *.sum site.bak

@VALGRIND_CHECK_RULES@
#VALGRIND_SUPPRESSIONS_FILES = $(PACKAGE).supp
#EXTRA_DIST = $(PACKAGE).supp
