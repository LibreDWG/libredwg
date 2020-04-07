## Makefile.am
##
## Copyright (C) 2015-2020 Free Software Foundation, Inc.
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
AM_CFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src $(WARN_CFLAGS)
LDADD     = $(top_builddir)/src/libredwg.la \
	$(top_builddir)/src/classes.lo

bits_test_LDADD = $(LDADD) \
	$(top_builddir)/src/bits.lo
decode_test_LDADD = $(LDADD) \
	$(top_builddir)/src/bits.lo \
	$(top_builddir)/src/hash.lo \
	$(top_builddir)/src/decode_r2007.lo \
	$(top_builddir)/src/common.lo \
	$(top_builddir)/src/print.lo
hash_test_LDADD = $(LDADD) \
	$(top_builddir)/src/bits.lo \
	$(top_builddir)/src/hash.lo
dynapi_test_LDADD = $(LDADD) \
	$(top_builddir)/src/bits.lo
dxf_test_LDADD = $(LDADD) \
	$(top_builddir)/src/common.lo \
	$(top_builddir)/src/dynapi.lo

testobjects = \
	3dface \
	3dsolid \
	acsh_box_class \
	acsh_extrusion_class \
	acsh_sweep_class \
	aldimobjectcontextdata \
	appid \
	appid_control \
	arc \
	arc_dimension \
	assoc2dconstraintgroup \
	assocaction \
	assocaligneddimactionbody \
	assocdependency \
	assocextrudedsurfaceactionbody \
	assocloftedsurfaceactionbody \
	assocnetwork \
	assocosnappointrefactionparam \
	assocperssubentmanager \
	assocplanesurfaceactionbody \
	assocrevolvedsurfaceactionbody \
	assocsweptsurfaceactionbody \
	attdef \
	attrib \
	blkrefobjectcontextdata \
	block \
	block_control \
	block_header \
	body \
	camera \
	cellstylemap \
	circle \
	datalink \
	datatable \
	dbcolor \
	detailviewstyle \
	dictionary \
	dictionaryvar \
	dictionarywdflt \
	dimassoc \
	dimension_aligned \
	dimension_ang2ln \
	dimension_ang3pt \
	dimension_diameter \
	dimension_linear \
	dimension_ordinate \
	dimension_radius \
	dimstyle \
	dimstyle_control \
	dummy \
	dynamicblockpurgepreventer \
	ellipse \
	endblk \
	evaluation_graph \
	extrudedsurface \
	field \
	fieldlist \
	geodata \
	geomapimage \
	geopositionmarker \
	group \
	hatch \
	helix \
	idbuffer \
	image \
	imagedef \
	imagedef_reactor \
	insert \
	layer \
	layer_control \
	layer_index \
	layerfilter \
	layout \
	layoutprintconfig \
	leader \
	light \
	lightlist \
	line \
	loftedsurface \
	long_transaction \
	ltype \
	ltype_control \
	lwpolyline \
	material \
	mentalrayrendersettings \
	mesh \
	minsert \
	mleaderobjectcontextdata \
	mleaderstyle \
	mline \
	mlinestyle \
	mtext \
	mtextobjectcontextdata \
	multileader \
	navisworksmodeldef \
	object_ptr \
	objectcontextdata \
	ole2frame \
	oleframe \
	perssubentmanager \
	placeholder \
	planesurface \
	plotsettings \
	point \
	polyline_2d \
	polyline_3d \
	polyline_mesh \
	polyline_pface \
	proxy_entity \
	proxy_object \
	rastervariables \
	ray \
	region \
	renderenvironment \
	renderglobal \
	revolvedsurface \
	scale \
	section \
	sectionviewstyle \
	seqend \
	shape \
	solid \
	sortentstable \
	spatial_filter \
	spatial_index \
	spline \
	style \
	style_control \
        acmecommandhistory \
        acmescope \
        acmestatemgr \
        acsh_history_class \
        assocgeomdependency \
        assocosnappointrefactionparam \
        assocvertexactionparam \
        documentoptions \
        rapidrtrendersettings \
	sun \
	sunstudy \
	sweptsurface \
	table \
	tablecontent \
	tablegeometry \
	tablestyle \
	text \
	textobjectcontextdata \
        leaderobjectcontextdata \
        mtextattributeobjectcontextdata \
	tolerance \
	trace \
	ucs \
	ucs_control \
	underlay \
	underlaydefinition \
	unknown_ent \
	unknown_obj \
	vba_project \
	vertex_2d \
	vertex_3d \
	vertex_mesh \
	vertex_pface \
	vertex_pface_face \
	view \
	view_control \
	viewport \
	visualstyle \
	vport \
	vport_control \
	vport_entity_control \
	vport_entity_header \
	wipeout \
	wipeoutvariables \
	xline \
	xrecord

# NB: After you modify ‘check_PROGRAMS’, do "make update-ignorance"
#     and include this directory's .gitignore in the commit.
check_PROGRAMS = \
	bits_test \
	decode_test \
	hash_test \
	dynapi_test \
	dxf_test \
	$(testobjects)

list:
	echo $(testobjects) dynapi_test

dynapi_test.c: dynapi_test.c.in \
		$(top_srcdir)/include/dwg.h \
	        $(top_srcdir)/src/gen-dynapi.pl
	echo make regen-dynapi might be needed

.c.i:
	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.o$$||'`;\
	$(COMPILE) -c -E -o $@ $<

TESTS = $(check_PROGRAMS)
if HAVE_PERL
TESTS += check-objects.pl
endif
TESTS_ENVIRONMENT = \
  top_srcdir="$(top_srcdir)" \
  ASAN_OPTIONS="detect_leaks=0"

bits_test.cbmc: bits_test.c $(top_srcdir)/src/bits.c
	cbmc -I. -I$(top_srcdir)/include -I/opt/local/include \
	     bits_test.c $(top_srcdir)/src/bits.c \
	     --unwind 6  --cover decision > $@

CLEANFILES = *.o
MAINTAINERCLEANFILES  = *_flymake.[ch] *~ *.i

update-ignorance:
	printf '/%s\n' $(check_PROGRAMS) > $(srcdir)/.gitignore

EXTRA_DIST = \
	tests_common.h \
	common.c \
	dynapi_test.c.in \
	dynapi_test.c \
	check-objects.pl

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
