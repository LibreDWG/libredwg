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
common_test_LDADD =

testobjects = \
	3dface \
	3dsolid \
	acsh_boolean_class \
	acsh_box_class \
	acsh_brep_class \
	acsh_chamfer_class \
	acsh_cone_class \
	acsh_cylinder_class \
	acsh_fillet_class \
	acsh_pyramid_class \
	acsh_torus_class \
	acsh_wedge_class \
	acsh_sphere_class \
	appid \
	appid_control \
	arc \
	arc_dimension \
	assocaction \
	assocnetwork \
	assocdependency \
	assocgeomdependency \
	assocvaluedependency \
	assocblendsurfaceactionbody \
	assocextendsurfaceactionbody \
	assocextrudedsurfaceactionbody \
	assocfilletsurfaceactionbody \
	assocloftedsurfaceactionbody \
	assocnetworksurfaceactionbody \
	assocoffsetsurfaceactionbody \
	assocpatchsurfaceactionbody \
	assocplanesurfaceactionbody \
	assocrevolvedsurfaceactionbody \
	assoctrimsurfaceactionbody \
	attdef \
	attrib \
	background \
	block \
	block_control \
	block_header \
	body \
	camera \
	cellstylemap \
	circle \
	dbcolor \
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
	field \
	fieldlist \
	geodata \
	group \
	hatch \
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
	leader \
	light \
	lightlist \
	line \
	long_transaction \
	ltype \
	ltype_control \
	lwpolyline \
	material \
	mentalrayrendersettings \
	mesh \
	minsert \
	mleaderstyle \
	mline \
	mlinestyle \
	mtext \
	multileader \
	object_ptr \
	ole2frame \
	oleframe \
	placeholder \
	plotsettings \
	point \
	polyline_2d \
	polyline_3d \
	polyline_mesh \
	polyline_pface \
	proxy_entity \
	proxy_object \
	rapidrtrendersettings \
	rastervariables \
	ray \
	region \
	rendersettings \
	scale \
	section_manager \
	section_settings \
	sectionobject \
	detailviewstyle \
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
	sun \
	tablegeometry \
	tablestyle \
	text \
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

if ENABLE_DEBUG
testobjects += \
	acmecommandhistory \
	acmescope \
	acmestatemgr \
	acsh_extrusion_class \
	acsh_history_class \
	acsh_loft_class \
	acsh_revolve_class \
	acsh_sweep_class \
	aldimobjectcontextdata \
	angdimobjectcontextdata \
	assoc2dconstraintgroup \
	assocmleaderactionbody \
	assocarrayactionbody \
	assocarraymodifyactionbody \
	assocrestoreentitystateactionbody \
	assoc3pointangulardimactionbody \
	assocaligneddimactionbody \
	assocordinatedimactionbody \
	assocrotateddimactionbody \
	assocsweptsurfaceactionbody \
	assocperssubentmanager \
	assocvariable \
	assocactionparam \
	assocosnappointrefactionparam \
	assocasmbodyactionparam \
	assocedgeactionparam \
	assoccompoundactionparam \
	assocfaceactionparam \
	assocobjectactionparam \
	assocpathactionparam \
	assocvertexactionparam \
	assocarraymodifyparameters \
	assocarraypathparameters \
	assocarraypolarparameters \
	assocarrayrectangularparameters \
	atext \
	blkrefobjectcontextdata \
	blockgriplocationcomponent \
	blockparamdependencybody \
	blockvisibilitygrip \
	blockvisibilityparameter \
	contextdatamanager \
	csacdocumentoptions \
	curvepath \
	datalink \
	datatable \
	dmdimobjectcontextdata \
	evaluation_graph \
	extrudedsurface \
	fcfobjectcontextdata \
	geomapimage \
	geopositionmarker \
	helix \
	layoutprintconfig \
	leaderobjectcontextdata \
	loftedsurface \
	mleaderobjectcontextdata \
	motionpath \
	mpolygon \
	mtextattributeobjectcontextdata \
	mtextobjectcontextdata \
	navisworksmodel \
	navisworksmodeldef \
	nurbsurface \
	objectcontextdata \
	orddimobjectcontextdata \
	persubentmgr \
	planesurface \
	pointpath \
	radimobjectcontextdata \
	radimlgobjectcontextdata \
	renderenvironment \
	renderentry \
	renderglobal \
	revolvedsurface \
	rtext \
	sunstudy \
	sweptsurface \
	table \
	tablecontent \
	textobjectcontextdata \
	tvdeviceproperties
endif

# NB: After you modify ‘check_PROGRAMS’, do "make update-ignorance"
#     and include this directory's .gitignore in the commit.
check_PROGRAMS = \
	bits_test \
	common_test \
	decode_test \
	hash_test \
	dynapi_test \
	dxf_test \
	$(testobjects)

check-prep: $(check_PROGRAMS)
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
