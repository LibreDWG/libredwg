## Makefile.am
##
## Copyright (C) 2015-2025 Free Software Foundation, Inc.
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
LDADD = $(top_builddir)/src/libredwg.la \
	$(top_builddir)/src/objects.lo \
	$(top_builddir)/src/classes.lo

add_test_LDADD = $(LDADD) \
	$(top_builddir)/src/codepages.lo \
	$(top_builddir)/src/bits.lo
bits_test_LDADD = $(LDADD) \
	$(top_builddir)/src/codepages.lo \
	$(top_builddir)/src/bits.lo
common_test_LDADD = $(LDADD) \
	$(top_builddir)/src/codepages.lo
decode_test_LDADD = $(LDADD) \
	$(top_builddir)/src/dwg.lo \
	$(top_builddir)/src/common.lo \
	$(top_builddir)/src/codepages.lo \
	$(top_builddir)/src/bits.lo \
	$(top_builddir)/src/hash.lo \
	$(top_builddir)/src/decode_r2007.lo
encode_test_LDADD = $(LDADD) \
	$(top_builddir)/src/dwg.lo \
	$(top_builddir)/src/common.lo \
	$(top_builddir)/src/codepages.lo \
	$(top_builddir)/src/bits.lo \
	$(top_builddir)/src/hash.lo \
	$(top_builddir)/src/decode.lo \
	$(top_builddir)/src/decode_r2007.lo
hash_test_LDADD = $(LDADD) \
	$(top_builddir)/src/codepages.lo \
	$(top_builddir)/src/bits.lo \
	$(top_builddir)/src/hash.lo
dynapi_test_LDADD = $(LDADD) \
	$(top_builddir)/src/codepages.lo \
	$(top_builddir)/src/bits.lo
dxf_test_LDADD = $(LDADD) \
	$(top_builddir)/src/common.lo \
	$(top_builddir)/src/dynapi.lo

testobjects = \
	jump \
	load \
	3dline \
	repeat \
	endrep \
	3dface \
	3dsolid \
	acsh_boolean_class \
	acsh_box_class \
	acsh_brep_class \
	acsh_chamfer_class \
	acsh_cone_class \
	acsh_cylinder_class \
	acsh_fillet_class \
	acsh_history_class \
	acsh_pyramid_class \
	acsh_torus_class \
	acsh_wedge_class \
	acsh_sphere_class \
	appid \
	appid_control \
	arc \
	arc_dimension \
	aldimobjectcontextdata \
	blkrefobjectcontextdata \
	fcfobjectcontextdata \
	leaderobjectcontextdata \
	mtextobjectcontextdata \
	textobjectcontextdata \
	assocaction \
	assocnetwork \
	assocdependency \
	assocgeomdependency \
	assocvaluedependency \
	assocblendsurfaceactionbody \
	assocdimdependencybody \
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
	skylight_background \
	solid_background \
	ibl_background \
	image_background \
	gradient_background \
	ground_plane_background \
	block \
	block_control \
	block_header \
	blockgriplocationcomponent \
	blockalignmentgrip \
	blockalignmentparameter \
	blockbasepointparameter \
	blockflipaction \
	blockflipparameter \
	blockflipgrip \
	blocklinearparameter \
	blocklineargrip \
	blocklookupgrip \
	blockmoveaction \
	blockpolargrip \
	blockpolarparameter \
	blockrotationgrip \
	blockscaleaction \
	blockrotationparameter \
	blockvisibilitygrip \
	blockvisibilityparameter \
	blockxyparameter \
	body \
	camera \
	cellstylemap \
	circle \
	datalink \
	dbcolor \
	dictionary \
	dictionaryvar \
	dictionarywdflt \
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
	blockrepresentation \
	ellipse \
	endblk \
	evaluation_graph \
	field \
	fieldlist \
	geodata \
	group \
	hatch \
	idbuffer \
	image \
	imagedef \
	imagedef_reactor \
	index \
	insert \
	large_radial_dimension \
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
	planesurface \
	plotsettings \
	point \
	polyline_r11 \
	polyline_2d \
	polyline_3d \
	polyline_mesh \
	polyline_pface \
	pointcloud \
	pointcloudex \
	pointclouddef \
	pointclouddefex \
	pointclouddef_reactor \
	pointclouddef_reactor_ex \
	pointcloudcolormap \
	proxy_entity \
	proxy_object \
	rapidrtrendersettings \
	rastervariables \
	ray \
	region \
	renderentry \
	renderenvironment \
	renderglobal \
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
	pdfunderlay \
	pdfdefinition \
	dgnunderlay \
	dgndefinition \
	dwfunderlay \
	dwfdefinition \
	unknown_ent \
	unknown_obj \
	vba_project \
	vertex_r11 \
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
	vx_control \
	vx_table_record \
	wipeout \
	wipeoutvariables \
	xline \
	xrecord \
	assoc2dconstraintgroup \
	assocvariable \
	assocarrayactionbody \
	assocactionparam \
	assocarraymodifyparameters \
	assocarraypathparameters \
	assocarraypolarparameters \
	assocarrayrectangularparameters \
	assocasmbodyactionparam \
	assoccompoundactionparam \
	assocfaceactionparam \
	assocobjectactionparam \
	assocpathactionparam \
	assocpointrefactionparam \
	assocvertexactionparam \
	assocosnappointrefactionparam \
	blockparamdependencybody \
	blockalignedconstraintparameter \
	blockangularconstraintparameter \
	blockarrayaction \
	blockdiametricconstraintparameter \
	blockhorizontalconstraintparameter \
	blocklinearconstraintparameter \
	blockradialconstraintparameter \
	blockverticalconstraintparameter \
	blocklookupaction \
	blocklookupparameter \
	blockpointparameter \
	blockpolarstretchaction \
	blockrotateaction \
	blockstretchaction \
	blockuserparameter \
	blockxygrip

# NB: After you modify this list, do "make update-ignorance"
#     and include this directory's .gitignore in the commit.
if ENABLE_DEBUG
testdebug = \
	acmecommandhistory \
	acmescope \
	acmestatemgr \
	acsh_extrusion_class \
	acsh_loft_class \
	acsh_revolve_class \
	acsh_sweep_class \
	angdimobjectcontextdata \
	annotscaleobjectcontextdata \
	arcalignedtext \
	assocarraymodifyactionbody \
	assocmleaderactionbody \
	assocedgechamferactionbody \
	assocedgefilletactionbody \
	assocrestoreentitystateactionbody \
	assoc3pointangulardimactionbody \
	assocaligneddimactionbody \
	assocordinatedimactionbody \
	assocrotateddimactionbody \
	assocsweptsurfaceactionbody \
	assocperssubentmanager \
	assocedgeactionparam \
	contextdatamanager \
	csacdocumentoptions \
	curvepath \
	datatable \
	dmdimobjectcontextdata \
	dimassoc \
	extrudedsurface \
	geomapimage \
	geopositionmarker \
	helix \
	loftedsurface \
	mleaderobjectcontextdata \
	motionpath \
	mpolygon \
	mtextattributeobjectcontextdata \
	navisworksmodel \
	navisworksmodeldef \
	nurbsurface \
	orddimobjectcontextdata \
	partial_viewing_index \
	persubentmgr \
	pointpath \
	radimobjectcontextdata \
	radimlgobjectcontextdata \
	revolvedsurface \
	rtext \
	sunstudy \
	sweptsurface \
	table \
	tablecontent \
	tvdeviceproperties \
	breakdata \
	breakpointref \
	flipgripentity \
	lineargripentity \
	polargripentity \
	rotationgripentity \
	visibilitygripentity \
	xygripentity \
	visibilityparameterentity \
	alignmentparameterentity \
	basepointparameterentity \
	blockpropertiestable \
	blockpropertiestablegrip \
	dynamicblockproxynode \
	flipparameterentity \
	linearparameterentity \
	pointparameterentity \
	rotationparameterentity \
	xyparameterentity

testobjects += $(testdebug)
endif

check_minimal = \
	bits_test \
	common_test \
	hash_test \
	dynapi_test \
	dxf_test
if USE_WRITE
check_minimal += add_test
endif
if !HAVE_MINGW
check_minimal += decode_test
if USE_WRITE
check_minimal += encode_test
endif
endif

if CHECK_LESS
check_PROGRAMS = $(check_minimal)
else
check_PROGRAMS = $(check_minimal) $(testobjects)
endif

check-prep: $(check_PROGRAMS)
list:
	echo $(testobjects) dynapi_test

#dynapi_test.c: $(srcdir)/dynapi_test.c.in \
#		$(top_srcdir)/include/dwg.h \
#	        $(top_srcdir)/src/gen-dynapi.pl
#	echo make regen-dynapi might be needed

.c.i:
	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.o$$||'`;\
	$(COMPILE) -E -o $@ $<

TESTS = $(check_PROGRAMS)
if HAVE_PERL
TESTS += check-objects.pl
endif
if HAVE_ASAN
TESTS_ENVIRONMENT = \
  @AM_TESTS_ENVIRONMENT@ \
  top_srcdir="$(top_srcdir)" \
  ASAN_OPTIONS="detect_leaks=0"
else
TESTS_ENVIRONMENT = \
  @AM_TESTS_ENVIRONMENT@ \
  top_srcdir="$(top_srcdir)"
endif

check-minimal: $(check_minimal)
	$(MAKE) check check_PROGRAMS="$(check_minimal)"
check-debug: $(testdebug)
	$(MAKE) check check_PROGRAMS="$(testdebug)"

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
	$(GCOV) -lp *.gcno | tee -a $(builddir)/gcov.log

CLEANFILES  += *.gcov *.gcno *.gcda gcov.log
endif
