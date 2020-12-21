/* ANSI-C code produced by gperf version 3.1 */
/* Command-line: gperf --output-file src/dxfclasses.tmp.c src/dxfclasses.in  */
/* Computed positions: -k'1,6,10,12,$' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#line 1 "src/dxfclasses.in"
// -*- mode: c -*-
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2020 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dxfclasses.c: create classes, lookup the initial class values from its DXF name.
 *               generated via gperf from dxfclasses.in
 *
 * written Reini Urban
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "config.h"
#include "dwg.h"
#include "common.h"
// other imports
int dwg_add_class (Dwg_Data *restrict dwg, const char *const restrict dxfname,
                   const char *const restrict cppname, const char *const restrict appname,
                   const bool is_entity);
// export
EXPORT int dwg_require_class (Dwg_Data *restrict dwg,
                              const char *const restrict dxfname, const int len);
// internal
static const struct Dwg_DXFClass * in_word_set (register const char *str, register size_t len);

enum apptypes {
  ODBXCLASS,
  ODBX_OR_A2000CLASS,
  A2000CLASS,
  SCENEOECLASS,
  ISMCLASS,
  EXPRESSCLASS,
  POINTCLOUDCLASS,
  SPECIALCLASS,
};
#define ODBX ODBXCLASS
#define ISM ISMCLASS
#define SCENEOE SCENEOECLASS
#define EXPRESS EXPRESSCLASS
#define POINTCLOUD POINTCLOUDCLASS
#define SPECIAL SPECIALCLASS
//%null-strings

#line 63 "src/dxfclasses.in"
struct Dwg_DXFClass {int name; const char *const cppname; unsigned apptype:4; unsigned isent:1;};

#define TOTAL_KEYWORDS 185
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 39
#define MIN_HASH_VALUE 10
#define MAX_HASH_VALUE 498
/* maximum key range = 489, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (register const char *str, register size_t len)
{
  static const unsigned short asso_values[] =
    {
      499, 499, 499, 499, 499, 499, 499, 499, 499, 499,
      499, 499, 499, 499, 499, 499, 499, 499, 499, 499,
      499, 499, 499, 499, 499, 499, 499, 499, 499, 499,
      499, 499, 499,  15,   0, 499, 499, 499, 499, 499,
      499, 499, 499, 499, 499, 499, 499, 499, 499, 499,
      499,   0, 499, 499, 499, 499, 499, 499, 499, 499,
      499, 499, 499, 499, 499,   0,  20, 145,  80,   0,
        5,  95,  45, 130, 255,  75, 135,  80,  25, 190,
       15,  10,  30,   5, 105,  45,  65, 175,  65,   0,
        0, 499, 499, 499, 499,  35,   5, 499, 499, 499,
      499, 499, 499, 499, 499, 499, 499, 499, 499, 499,
      499, 499, 499, 499, 499, 499, 499, 499, 499, 499,
      499, 499, 499, 499, 499, 499, 499, 499, 499
    };
  register unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[11]+1];
#if defined __cplusplus && (__cplusplus >= 201703L || (__cplusplus >= 201103L && defined __clang_major__ && defined __clang_minor__ && __clang_major__ + (__clang_minor__ >= 9) > 3))
      [[fallthrough]];
#elif defined __GNUC__ && __GNUC__ >= 7
      __attribute__ ((__fallthrough__));
#endif
      /*FALLTHROUGH*/
      case 11:
      case 10:
        hval += asso_values[(unsigned char)str[9]];
#if defined __cplusplus && (__cplusplus >= 201703L || (__cplusplus >= 201103L && defined __clang_major__ && defined __clang_minor__ && __clang_major__ + (__clang_minor__ >= 9) > 3))
      [[fallthrough]];
#elif defined __GNUC__ && __GNUC__ >= 7
      __attribute__ ((__fallthrough__));
#endif
      /*FALLTHROUGH*/
      case 9:
      case 8:
      case 7:
      case 6:
        hval += asso_values[(unsigned char)str[5]];
#if defined __cplusplus && (__cplusplus >= 201703L || (__cplusplus >= 201103L && defined __clang_major__ && defined __clang_minor__ && __clang_major__ + (__clang_minor__ >= 9) > 3))
      [[fallthrough]];
#elif defined __GNUC__ && __GNUC__ >= 7
      __attribute__ ((__fallthrough__));
#endif
      /*FALLTHROUGH*/
      case 5:
      case 4:
      case 3:
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

struct stringpool_t
  {
    char stringpool_str10[sizeof("SCALE")];
    char stringpool_str27[sizeof("SWEPTSURFACE")];
    char stringpool_str29[sizeof("FLIPPARAMETERENTITY")];
    char stringpool_str32[sizeof("ACSH_FILLET_CLASS")];
    char stringpool_str33[sizeof("SUN")];
    char stringpool_str34[sizeof("ACDBASSOCARRAYACTIONBODY")];
    char stringpool_str37[sizeof("PLANESURFACE")];
    char stringpool_str40[sizeof("ACDBASSOCARRAYMODIFYACTIONBODY")];
    char stringpool_str43[sizeof("ACDBASSOCARRAYPATHPARAMETERS")];
    char stringpool_str44[sizeof("ACDBASSOCARRAYPOLARPARAMETERS")];
    char stringpool_str45[sizeof("ACDBASSOCARRAYMODIFYPARAMETERS")];
    char stringpool_str46[sizeof("ACDBASSOCSWEPTSURFACEACTIONBODY")];
    char stringpool_str50[sizeof("ACDBASSOCARRAYRECTANGULARPARAMETERS")];
    char stringpool_str55[sizeof("ACDBASSOC3POINTANGULARDIMACTIONBODY")];
    char stringpool_str58[sizeof("SUNSTUDY")];
    char stringpool_str61[sizeof("ACDBASSOCBLENDSURFACEACTIONBODY")];
    char stringpool_str62[sizeof("ACSH_SPHERE_CLASS")];
    char stringpool_str66[sizeof("ACDB_FCFOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str71[sizeof("ACDBASSOCPLANESURFACEACTIONBODY")];
    char stringpool_str74[sizeof("ACDB_BLKREFOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str75[sizeof("ACAD_PROXY_ENTITY_WRAPPER")];
    char stringpool_str79[sizeof("ACDBASSOCEDGEFILLETACTIONBODY")];
    char stringpool_str80[sizeof("ACDBASSOCEDGECHAMFERACTIONBODY")];
    char stringpool_str81[sizeof("ACDBASSOCPERSSUBENTMANAGER")];
    char stringpool_str82[sizeof("ACDBASSOCEXTENDSURFACEACTIONBODY")];
    char stringpool_str83[sizeof("BLOCKALIGNMENTGRIP")];
    char stringpool_str84[sizeof("ACDBASSOCEXTRUDEDSURFACEACTIONBODY")];
    char stringpool_str85[sizeof("ACAD_PROXY_OBJECT_WRAPPER")];
    char stringpool_str87[sizeof("XYPARAMETERENTITY")];
    char stringpool_str89[sizeof("DATATABLE")];
    char stringpool_str90[sizeof("ACDBASSOCACTION")];
    char stringpool_str93[sizeof("BLOCKFLIPPARAMETER")];
    char stringpool_str95[sizeof("HATCH")];
    char stringpool_str96[sizeof("ACDBASSOCPATCHSURFACEACTIONBODY")];
    char stringpool_str97[sizeof("MLEADERSTYLE")];
    char stringpool_str99[sizeof("BLOCKPOLARGRIP")];
    char stringpool_str100[sizeof("BLOCKPROPERTIESTABLE")];
    char stringpool_str101[sizeof("ACDBPERSSUBENTMANAGER")];
    char stringpool_str103[sizeof("BLOCKALIGNMENTPARAMETER")];
    char stringpool_str106[sizeof("BLOCKALIGNEDCONSTRAINTPARAMETER")];
    char stringpool_str108[sizeof("ACDBASSOCNETWORKSURFACEACTIONBODY")];
    char stringpool_str109[sizeof("ACDBASSOCROTATEDDIMACTIONBODY")];
    char stringpool_str110[sizeof("BLOCKFLIPACTION")];
    char stringpool_str113[sizeof("SPATIAL_INDEX")];
    char stringpool_str114[sizeof("ACDBASSOCDEPENDENCY")];
    char stringpool_str115[sizeof("ACAD_TABLE")];
    char stringpool_str116[sizeof("ACDBASSOCMLEADERACTIONBODY")];
    char stringpool_str118[sizeof("ACSH_PYRAMID_CLASS")];
    char stringpool_str119[sizeof("BLOCKPROPERTIESTABLEGRIP")];
    char stringpool_str120[sizeof("TABLESTYLE")];
    char stringpool_str121[sizeof("ACSH_SWEEP_CLASS")];
    char stringpool_str122[sizeof("ACDBASSOCFILLETSURFACEACTIONBODY")];
    char stringpool_str123[sizeof("SORTENTSTABLE")];
    char stringpool_str124[sizeof("ALIGNMENTPARAMETERENTITY")];
    char stringpool_str129[sizeof("ACDB_BLOCKREPRESENTATION_DATA")];
    char stringpool_str133[sizeof("BLOCKUSERPARAMETER")];
    char stringpool_str135[sizeof("IMAGE")];
    char stringpool_str136[sizeof("ACDBASSOCDIMDEPENDENCYBODY")];
    char stringpool_str137[sizeof("ACDBASSOCASMBODYACTIONPARAM")];
    char stringpool_str138[sizeof("ACDBASSOCGEOMDEPENDENCY")];
    char stringpool_str139[sizeof("ACDBCURVEPATH   \011\011\011\"AcDbCurvePath\"")];
    char stringpool_str140[sizeof("RTEXT")];
    char stringpool_str141[sizeof("BLOCKARRAYACTION")];
    char stringpool_str145[sizeof("LWPOLYLINE")];
    char stringpool_str146[sizeof("BLOCKSCALEACTION")];
    char stringpool_str150[sizeof("ACDBASSOCACTIONPARAM")];
    char stringpool_str154[sizeof("ACDBASSOCEDGEACTIONPARAM")];
    char stringpool_str155[sizeof("ACSH_BREP_CLASS")];
    char stringpool_str156[sizeof("ACAD_EVALUATION_GRAPH")];
    char stringpool_str157[sizeof("SURFACE")];
    char stringpool_str158[sizeof("BLOCKPOLARSTRETCHACTION")];
    char stringpool_str159[sizeof("EXACXREFPANELOBJECT")];
    char stringpool_str162[sizeof("PLOTSETTINGS")];
    char stringpool_str166[sizeof("ACDBASSOCNETWORK")];
    char stringpool_str168[sizeof("BLOCKDIAMETRICCONSTRAINTPARAMETER")];
    char stringpool_str169[sizeof("ACDBASSOCPATHACTIONPARAM")];
    char stringpool_str170[sizeof("POLARGRIPENTITY")];
    char stringpool_str173[sizeof("IDBUFFER")];
    char stringpool_str175[sizeof("POINTPARAMETERENTITY")];
    char stringpool_str177[sizeof("ACDBASSOCRESTOREENTITYSTATEACTIONBODY")];
    char stringpool_str178[sizeof("ACSH_CHAMFER_CLASS")];
    char stringpool_str181[sizeof("ACDBASSOCVERTEXACTIONPARAM")];
    char stringpool_str182[sizeof("XRECORD")];
    char stringpool_str183[sizeof("ACDBBLOCKPARAMDEPENDENCYBODY")];
    char stringpool_str184[sizeof("ACDB_ANGDIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str185[sizeof("BLOCKMOVEACTION")];
    char stringpool_str186[sizeof("BLOCKXYPARAMETER")];
    char stringpool_str188[sizeof("ARC_DIMENSION")];
    char stringpool_str189[sizeof("ACDB_LEADEROBJECTCONTEXTDATA_CLASS")];
    char stringpool_str190[sizeof("BLOCKRADIALCONSTRAINTPARAMETER")];
    char stringpool_str191[sizeof("LINEARPARAMETERENTITY")];
    char stringpool_str193[sizeof("ACSH_REVOLVE_CLASS")];
    char stringpool_str194[sizeof("ACDBASSOCFACEACTIONPARAM")];
    char stringpool_str195[sizeof("ACDB_RADIMLGOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str198[sizeof("MOTIONPATH   \011\011\011\011\"AcDbMotionPath\"")];
    char stringpool_str199[sizeof("ACSH_CYLINDER_CLASS")];
    char stringpool_str202[sizeof("BLOCKROTATIONGRIP")];
    char stringpool_str203[sizeof("ROTATIONPARAMETERENTITY")];
    char stringpool_str204[sizeof("ACSH_BOX_CLASS")];
    char stringpool_str205[sizeof("ACDB_MLEADEROBJECTCONTEXTDATA_CLASS")];
    char stringpool_str209[sizeof("BLOCKPOINTPARAMETER")];
    char stringpool_str211[sizeof("ACSH_TORUS_CLASS")];
    char stringpool_str215[sizeof("BLOCKLINEARPARAMETER")];
    char stringpool_str217[sizeof("BLOCKROTATEACTION")];
    char stringpool_str218[sizeof("TABLEGEOMETRY")];
    char stringpool_str219[sizeof("BLOCKVISIBILITYGRIP")];
    char stringpool_str221[sizeof("BLOCKANGULARCONSTRAINTPARAMETER")];
    char stringpool_str222[sizeof("BLOCKROTATIONPARAMETER")];
    char stringpool_str223[sizeof("IMAGEDEF")];
    char stringpool_str224[sizeof("SPATIAL_FILTER")];
    char stringpool_str225[sizeof("ACDBASSOCORDINATEDIMACTIONBODY")];
    char stringpool_str226[sizeof("NURBSURFACE")];
    char stringpool_str228[sizeof("ACDB_ANNOTSCALEOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str229[sizeof("OLE2FRAME")];
    char stringpool_str230[sizeof("BLOCKLINEARGRIP")];
    char stringpool_str233[sizeof("ACSH_BOOLEAN_CLASS")];
    char stringpool_str235[sizeof("EXTRUDEDSURFACE")];
    char stringpool_str238[sizeof("DIMASSOC")];
    char stringpool_str239[sizeof("BLOCKVISIBILITYPARAMETER")];
    char stringpool_str241[sizeof("BLOCKXYGRIP")];
    char stringpool_str244[sizeof("ACDBASSOCREVOLVEDSURFACEACTIONBODY")];
    char stringpool_str245[sizeof("LIGHT")];
    char stringpool_str246[sizeof("LAYER_INDEX")];
    char stringpool_str250[sizeof("REVOLVEDSURFACE")];
    char stringpool_str254[sizeof("BASEPOINTPARAMETERENTITY")];
    char stringpool_str256[sizeof("MULTILEADER")];
    char stringpool_str258[sizeof("ACSH_HISTORY_CLASS")];
    char stringpool_str260[sizeof("BLOCKLOOKUPPARAMETER")];
    char stringpool_str262[sizeof("BLOCKLOOKUPACTION")];
    char stringpool_str263[sizeof("ACDB_ALDIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str266[sizeof("ACDB_DYNAMICBLOCKPROXYNODE")];
    char stringpool_str267[sizeof("ACDBASSOCLOFTEDSURFACEACTIONBODY")];
    char stringpool_str270[sizeof("ACSH_LOFT_CLASS")];
    char stringpool_str275[sizeof("BLOCKLOOKUPGRIP")];
    char stringpool_str276[sizeof("ACSH_WEDGE_CLASS")];
    char stringpool_str280[sizeof("ACSH_CONE_CLASS")];
    char stringpool_str283[sizeof("ACDBASSOCCOMPOUNDACTIONPARAM")];
    char stringpool_str284[sizeof("ACDBPOINTPATH   \011\011\011\"AcDbPointPath\"")];
    char stringpool_str288[sizeof("ACDBASSOCALIGNEDIMACTIONBODY")];
    char stringpool_str289[sizeof("ACDBASSOCALIGNEDDIMACTIONBODY")];
    char stringpool_str291[sizeof("WIPEOUTVARIABLES")];
    char stringpool_str293[sizeof("ACDB_RADIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str295[sizeof("BLOCKLINEARCONSTRAINTPARAMETER")];
    char stringpool_str297[sizeof("BLOCKVERTICALCONSTRAINTPARAMETER")];
    char stringpool_str303[sizeof("BLOCKSTRETCHACTION")];
    char stringpool_str304[sizeof("ACDB_DYNAMICBLOCKPURGEPREVENTER_VERSION")];
    char stringpool_str305[sizeof("ACDBPLACEHOLDER")];
    char stringpool_str306[sizeof("IMAGEDEF_REACTOR")];
    char stringpool_str313[sizeof("LOFTEDSURFACE")];
    char stringpool_str318[sizeof("NPOCOLLECTION")];
    char stringpool_str319[sizeof("BLOCKHORIZONTALCONSTRAINTPARAMETER")];
    char stringpool_str322[sizeof("ACDBASSOCOFFSETSURFACEACTIONBODY")];
    char stringpool_str324[sizeof("POSITIONMARKER")];
    char stringpool_str325[sizeof("ACSH_EXTRUSION_CLASS")];
    char stringpool_str332[sizeof("WIPEOUT")];
    char stringpool_str333[sizeof("DICTIONARYVAR")];
    char stringpool_str334[sizeof("ARCALIGNEDTEXT")];
    char stringpool_str343[sizeof("ACDB_DMDIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str346[sizeof("VISUALSTYLE")];
    char stringpool_str351[sizeof("LAYOUT")];
    char stringpool_str353[sizeof("MATERIAL")];
    char stringpool_str355[sizeof("RASTERVARIABLES")];
    char stringpool_str356[sizeof("VBA_PROJECT")];
    char stringpool_str363[sizeof("BLOCKBASEPOINTPARAMETER")];
    char stringpool_str367[sizeof("CELLSTYLEMAP")];
    char stringpool_str372[sizeof("ACDBPOINTCLOUDDEF")];
    char stringpool_str374[sizeof("ACDB_ORDDIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str376[sizeof("ACDBASSOCOBJECTACTIONPARAM")];
    char stringpool_str383[sizeof("ACDBASSOCPOINTREFACTIONPARAM")];
    char stringpool_str385[sizeof("ACDBSECTIONVIEWSTYLE")];
    char stringpool_str387[sizeof("ACDBPOINTCLOUDCOLORMAP")];
    char stringpool_str388[sizeof("CONTEXTDATAMANAGER")];
    char stringpool_str395[sizeof("ACDBASSOCTRIMSURFACEACTIONBODY")];
    char stringpool_str403[sizeof("BLOCKFLIPGRIP")];
    char stringpool_str405[sizeof("ACDBPOINTCLOUDDEF_REACTOR")];
    char stringpool_str407[sizeof("ACDB_TEXTOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str409[sizeof("ACDBDETAILVIEWSTYLE")];
    char stringpool_str412[sizeof("TABLECONTENT")];
    char stringpool_str431[sizeof("ACDBPOINTCLOUDEX")];
    char stringpool_str434[sizeof("ACDBPOINTCLOUDDEFEX")];
    char stringpool_str443[sizeof("ACDBPOINTCLOUDDEF_REACTOR_EX")];
    char stringpool_str444[sizeof("ACDBPOINTCLOUD")];
    char stringpool_str461[sizeof("BLOCKGRIPLOCATIONCOMPONENT")];
    char stringpool_str464[sizeof("ACDBDICTIONARYWDFLT")];
    char stringpool_str498[sizeof("ACDBASSOCOSNAPPOINTREFACTIONPARAM")];
  };
static const struct stringpool_t stringpool_contents =
  {
    "SCALE",
    "SWEPTSURFACE",
    "FLIPPARAMETERENTITY",
    "ACSH_FILLET_CLASS",
    "SUN",
    "ACDBASSOCARRAYACTIONBODY",
    "PLANESURFACE",
    "ACDBASSOCARRAYMODIFYACTIONBODY",
    "ACDBASSOCARRAYPATHPARAMETERS",
    "ACDBASSOCARRAYPOLARPARAMETERS",
    "ACDBASSOCARRAYMODIFYPARAMETERS",
    "ACDBASSOCSWEPTSURFACEACTIONBODY",
    "ACDBASSOCARRAYRECTANGULARPARAMETERS",
    "ACDBASSOC3POINTANGULARDIMACTIONBODY",
    "SUNSTUDY",
    "ACDBASSOCBLENDSURFACEACTIONBODY",
    "ACSH_SPHERE_CLASS",
    "ACDB_FCFOBJECTCONTEXTDATA_CLASS",
    "ACDBASSOCPLANESURFACEACTIONBODY",
    "ACDB_BLKREFOBJECTCONTEXTDATA_CLASS",
    "ACAD_PROXY_ENTITY_WRAPPER",
    "ACDBASSOCEDGEFILLETACTIONBODY",
    "ACDBASSOCEDGECHAMFERACTIONBODY",
    "ACDBASSOCPERSSUBENTMANAGER",
    "ACDBASSOCEXTENDSURFACEACTIONBODY",
    "BLOCKALIGNMENTGRIP",
    "ACDBASSOCEXTRUDEDSURFACEACTIONBODY",
    "ACAD_PROXY_OBJECT_WRAPPER",
    "XYPARAMETERENTITY",
    "DATATABLE",
    "ACDBASSOCACTION",
    "BLOCKFLIPPARAMETER",
    "HATCH",
    "ACDBASSOCPATCHSURFACEACTIONBODY",
    "MLEADERSTYLE",
    "BLOCKPOLARGRIP",
    "BLOCKPROPERTIESTABLE",
    "ACDBPERSSUBENTMANAGER",
    "BLOCKALIGNMENTPARAMETER",
    "BLOCKALIGNEDCONSTRAINTPARAMETER",
    "ACDBASSOCNETWORKSURFACEACTIONBODY",
    "ACDBASSOCROTATEDDIMACTIONBODY",
    "BLOCKFLIPACTION",
    "SPATIAL_INDEX",
    "ACDBASSOCDEPENDENCY",
    "ACAD_TABLE",
    "ACDBASSOCMLEADERACTIONBODY",
    "ACSH_PYRAMID_CLASS",
    "BLOCKPROPERTIESTABLEGRIP",
    "TABLESTYLE",
    "ACSH_SWEEP_CLASS",
    "ACDBASSOCFILLETSURFACEACTIONBODY",
    "SORTENTSTABLE",
    "ALIGNMENTPARAMETERENTITY",
    "ACDB_BLOCKREPRESENTATION_DATA",
    "BLOCKUSERPARAMETER",
    "IMAGE",
    "ACDBASSOCDIMDEPENDENCYBODY",
    "ACDBASSOCASMBODYACTIONPARAM",
    "ACDBASSOCGEOMDEPENDENCY",
    "ACDBCURVEPATH   \011\011\011\"AcDbCurvePath\"",
    "RTEXT",
    "BLOCKARRAYACTION",
    "LWPOLYLINE",
    "BLOCKSCALEACTION",
    "ACDBASSOCACTIONPARAM",
    "ACDBASSOCEDGEACTIONPARAM",
    "ACSH_BREP_CLASS",
    "ACAD_EVALUATION_GRAPH",
    "SURFACE",
    "BLOCKPOLARSTRETCHACTION",
    "EXACXREFPANELOBJECT",
    "PLOTSETTINGS",
    "ACDBASSOCNETWORK",
    "BLOCKDIAMETRICCONSTRAINTPARAMETER",
    "ACDBASSOCPATHACTIONPARAM",
    "POLARGRIPENTITY",
    "IDBUFFER",
    "POINTPARAMETERENTITY",
    "ACDBASSOCRESTOREENTITYSTATEACTIONBODY",
    "ACSH_CHAMFER_CLASS",
    "ACDBASSOCVERTEXACTIONPARAM",
    "XRECORD",
    "ACDBBLOCKPARAMDEPENDENCYBODY",
    "ACDB_ANGDIMOBJECTCONTEXTDATA_CLASS",
    "BLOCKMOVEACTION",
    "BLOCKXYPARAMETER",
    "ARC_DIMENSION",
    "ACDB_LEADEROBJECTCONTEXTDATA_CLASS",
    "BLOCKRADIALCONSTRAINTPARAMETER",
    "LINEARPARAMETERENTITY",
    "ACSH_REVOLVE_CLASS",
    "ACDBASSOCFACEACTIONPARAM",
    "ACDB_RADIMLGOBJECTCONTEXTDATA_CLASS",
    "MOTIONPATH   \011\011\011\011\"AcDbMotionPath\"",
    "ACSH_CYLINDER_CLASS",
    "BLOCKROTATIONGRIP",
    "ROTATIONPARAMETERENTITY",
    "ACSH_BOX_CLASS",
    "ACDB_MLEADEROBJECTCONTEXTDATA_CLASS",
    "BLOCKPOINTPARAMETER",
    "ACSH_TORUS_CLASS",
    "BLOCKLINEARPARAMETER",
    "BLOCKROTATEACTION",
    "TABLEGEOMETRY",
    "BLOCKVISIBILITYGRIP",
    "BLOCKANGULARCONSTRAINTPARAMETER",
    "BLOCKROTATIONPARAMETER",
    "IMAGEDEF",
    "SPATIAL_FILTER",
    "ACDBASSOCORDINATEDIMACTIONBODY",
    "NURBSURFACE",
    "ACDB_ANNOTSCALEOBJECTCONTEXTDATA_CLASS",
    "OLE2FRAME",
    "BLOCKLINEARGRIP",
    "ACSH_BOOLEAN_CLASS",
    "EXTRUDEDSURFACE",
    "DIMASSOC",
    "BLOCKVISIBILITYPARAMETER",
    "BLOCKXYGRIP",
    "ACDBASSOCREVOLVEDSURFACEACTIONBODY",
    "LIGHT",
    "LAYER_INDEX",
    "REVOLVEDSURFACE",
    "BASEPOINTPARAMETERENTITY",
    "MULTILEADER",
    "ACSH_HISTORY_CLASS",
    "BLOCKLOOKUPPARAMETER",
    "BLOCKLOOKUPACTION",
    "ACDB_ALDIMOBJECTCONTEXTDATA_CLASS",
    "ACDB_DYNAMICBLOCKPROXYNODE",
    "ACDBASSOCLOFTEDSURFACEACTIONBODY",
    "ACSH_LOFT_CLASS",
    "BLOCKLOOKUPGRIP",
    "ACSH_WEDGE_CLASS",
    "ACSH_CONE_CLASS",
    "ACDBASSOCCOMPOUNDACTIONPARAM",
    "ACDBPOINTPATH   \011\011\011\"AcDbPointPath\"",
    "ACDBASSOCALIGNEDIMACTIONBODY",
    "ACDBASSOCALIGNEDDIMACTIONBODY",
    "WIPEOUTVARIABLES",
    "ACDB_RADIMOBJECTCONTEXTDATA_CLASS",
    "BLOCKLINEARCONSTRAINTPARAMETER",
    "BLOCKVERTICALCONSTRAINTPARAMETER",
    "BLOCKSTRETCHACTION",
    "ACDB_DYNAMICBLOCKPURGEPREVENTER_VERSION",
    "ACDBPLACEHOLDER",
    "IMAGEDEF_REACTOR",
    "LOFTEDSURFACE",
    "NPOCOLLECTION",
    "BLOCKHORIZONTALCONSTRAINTPARAMETER",
    "ACDBASSOCOFFSETSURFACEACTIONBODY",
    "POSITIONMARKER",
    "ACSH_EXTRUSION_CLASS",
    "WIPEOUT",
    "DICTIONARYVAR",
    "ARCALIGNEDTEXT",
    "ACDB_DMDIMOBJECTCONTEXTDATA_CLASS",
    "VISUALSTYLE",
    "LAYOUT",
    "MATERIAL",
    "RASTERVARIABLES",
    "VBA_PROJECT",
    "BLOCKBASEPOINTPARAMETER",
    "CELLSTYLEMAP",
    "ACDBPOINTCLOUDDEF",
    "ACDB_ORDDIMOBJECTCONTEXTDATA_CLASS",
    "ACDBASSOCOBJECTACTIONPARAM",
    "ACDBASSOCPOINTREFACTIONPARAM",
    "ACDBSECTIONVIEWSTYLE",
    "ACDBPOINTCLOUDCOLORMAP",
    "CONTEXTDATAMANAGER",
    "ACDBASSOCTRIMSURFACEACTIONBODY",
    "BLOCKFLIPGRIP",
    "ACDBPOINTCLOUDDEF_REACTOR",
    "ACDB_TEXTOBJECTCONTEXTDATA_CLASS",
    "ACDBDETAILVIEWSTYLE",
    "TABLECONTENT",
    "ACDBPOINTCLOUDEX",
    "ACDBPOINTCLOUDDEFEX",
    "ACDBPOINTCLOUDDEF_REACTOR_EX",
    "ACDBPOINTCLOUD",
    "BLOCKGRIPLOCATIONCOMPONENT",
    "ACDBDICTIONARYWDFLT",
    "ACDBASSOCOSNAPPOINTREFACTIONPARAM"
  };
#define stringpool ((const char *) &stringpool_contents)
const struct Dwg_DXFClass *
in_word_set (register const char *str, register size_t len)
{
  static const struct Dwg_DXFClass wordlist[] =
    {
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1},
#line 95 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str10,		"AcDbScale",		ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 126 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str27,		"AcDbSweptSurface",	ODBX,	1},
      {-1},
#line 199 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str29,			"AcDbBlockFlipParameterEntity",		ODBX,	1},
      {-1}, {-1},
#line 135 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str32,	"AcDbShFillet",		ODBX,	0},
#line 103 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str33, 		"AcDbSun",		SCENEOE, 0},
#line 163 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str34,	"AcDbAssocArrayActionBody",		ODBX,	0},
      {-1}, {-1},
#line 124 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str37,		"AcDbPlaneSurface",	ODBX,	1},
      {-1}, {-1},
#line 162 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str40,	"AcDbAssocArrayModifyActionBody",	ODBX,	0},
      {-1}, {-1},
#line 193 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str43,		"AcDbAssocArrayPathParameters",		ODBX,	0},
#line 194 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str44,		"AcDbAssocArrayPolarParameters",	ODBX,	0},
#line 192 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str45,		"AcDbAssocArrayModifyParameters",	ODBX,	0},
#line 180 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str46,	"AcDbAssocSweptSurfaceActionBody",	ODBX,	0},
      {-1}, {-1}, {-1},
#line 195 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str50,	"AcDbAssocArrayRectangularParameters",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1},
#line 157 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str55,"AcDbAssoc3PointAngularDimActionBody",	ODBX,	0},
      {-1}, {-1},
#line 104 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str58,	"AcDbSunStudy",		SCENEOE, 0},
      {-1}, {-1},
#line 169 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str61,	"AcDbAssocBlendSurfaceActionBody",	ODBX,	0},
#line 140 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str62,	"AcDbShSphere",		ODBX,	0},
      {-1}, {-1}, {-1},
#line 255 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str66,	"AcDbFcfObjectContextData",		ODBX,	0},
      {-1}, {-1}, {-1}, {-1},
#line 176 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str71,	"AcDbAssocPlaneSurfaceActionBody",	ODBX,	0},
      {-1}, {-1},
#line 253 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str74,	"AcDbBlkrefObjectContextData",		ODBX,	0},
#line 76 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str75,	"AcDbProxyEntityWrapper",	ODBX_OR_A2000CLASS,	0},
      {-1}, {-1}, {-1},
#line 165 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str79,	"AcDbAssocEdgeFilletActionBody",	ODBX,	0},
#line 164 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str80,	"AcDbAssocEdgeChamferActionBody",	ODBX,	0},
#line 145 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str81,	"AcDbAssocPersSubentManager",	ODBX,	0},
#line 170 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str82,	"AcDbAssocExtendSurfaceActionBody",	ODBX,	0},
#line 230 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str83,			"AcDbBlockAlignmentGrip",		ODBX,	0},
#line 171 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str84,	"AcDbAssocExtrudedSurfaceActionBody",	ODBX,	0},
#line 77 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str85,	"AcDbProxyObjectWrapper",	ODBX_OR_A2000CLASS,	0},
      {-1},
#line 204 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str87,			"AcDbBlockXYParameterEntity",		ODBX,	1},
      {-1},
#line 94 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str89,	"AcDbDataTable",	ODBX,	0},
#line 148 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str90,		"AcDbAssocAction",		ODBX,	0},
      {-1}, {-1},
#line 234 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str93,			"AcDbBlockFlipParameter",		ODBX,	0},
      {-1},
#line 68 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str95,		"AcDbHatch",		ODBX,	1},
#line 177 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str96,	"AcDbAssocPatchSurfaceActionBody",	ODBX,	0},
#line 111 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str97,	"AcDbMLeaderStyle",	SPECIAL, 0},
      {-1},
#line 225 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str99,				"AcDbBlockPolarGrip",			ODBX,	0},
#line 241 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str100,			"AcDbBlockPropertiesTable",		ODBX,	0},
#line 146 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str101, 		"AcDbPersSubentManager",	ODBX, 0},
      {-1},
#line 231 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str103,		"AcDbBlockAlignmentParameter",		ODBX,	0},
      {-1}, {-1},
#line 211 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str106,	"AcDbBlockAlignedConstraintParameter",  ODBX,	0},
      {-1},
#line 174 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str108,	"AcDbAssocNetworkSurfaceActionBody",	ODBX,	0},
#line 160 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str109,	"AcDbAssocRotatedDimActionBody",	ODBX,	0},
#line 216 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str110,			"AcDbBlockFlipAction",			ODBX,	0},
      {-1}, {-1},
#line 86 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str113,	"AcDbSpatialIndex",	ODBX,	0},
#line 153 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str114,		"AcDbAssocDependency",		ODBX,	0},
#line 72 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str115,	"AcDbTable",		ODBX,	1},
#line 166 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str116,	"AcDbAssocMLeaderActionBody",		ODBX,	0},
      {-1},
#line 138 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str118,	"AcDbShPyramid",	ODBX,	0},
#line 242 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str119,   		"AcDbBlockPropertiesTableGrip",		ODBX,	0},
#line 90 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str120,	"AcDbTableStyle",	ODBX,	0},
#line 141 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str121,	"AcDbShSweep",		ODBX,	0},
#line 172 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str122,	"AcDbAssocFilletSurfaceActionBody",	ODBX,	0},
#line 81 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str123,	"AcDbSortentsTable",	A2000CLASS,	0},
#line 197 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str124,		"AcDbBlockAlignmentParameterEntity",	ODBX,	1},
      {-1}, {-1}, {-1}, {-1},
#line 206 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str129,		"AcDbBlockRepresentationData",     	ODBX,	0},
      {-1}, {-1}, {-1},
#line 238 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str133,   			"AcDbBlockUserParameter",		ODBX,	0},
      {-1},
#line 97 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str135,		"AcDbRasterImage",	ISM,	1},
#line 154 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str136,	"AcDbAssocDimDependencyBody",	ODBX,	0},
#line 184 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str137,		"AcDbAssocAsmbodyActionParam",	ODBX,	0},
#line 152 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str138,	"AcDbAssocGeomDependency",	ODBX,	0},
#line 246 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str139,			ODBX,	0},
#line 109 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str140, 		"AcDbRText", 		EXPRESS, 1},
#line 207 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str141,			"AcDbBlockArrayAction",     		ODBX,	0},
      {-1}, {-1}, {-1},
#line 67 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str145,	"AcDbPolyline",		ODBX,	1},
#line 219 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str146,			"AcDbBlockScaleAction",			ODBX,	0},
      {-1}, {-1}, {-1},
#line 183 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str150,			"AcDbAssocActionParam",		ODBX,	0},
      {-1}, {-1}, {-1},
#line 186 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str154,		"AcDbAssocEdgeActionParam",	ODBX,	0},
#line 130 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str155,	"AcDbShBrep",		ODBX,	0},
#line 118 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str156,	"AcDbEvalGraph",	ODBX,	0},
#line 120 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str157,		"AcDbSurface",		ODBX,	1},
#line 220 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str158,		"AcDbBlockPolarStretchAction",	       	ODBX,	0},
#line 271 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str159,			"ExAcXREFPanelObject",		SPECIAL, 0},
      {-1}, {-1},
#line 114 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str162,		"AcDbPlotSettings",	ODBX,	0},
      {-1}, {-1}, {-1},
#line 147 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str166,		"AcDbAssocNetwork",		ODBX,	0},
      {-1},
#line 209 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str168,	"AcDbBlockDiametricConstraintParameter",ODBX,	0},
#line 188 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str169,		"AcDbAssocPathActionParam",	ODBX,	0},
#line 203 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str170,			"AcDbBlockPolarGripEntity",		ODBX,	1},
      {-1}, {-1},
#line 88 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str173,	"AcDbIdBuffer",		ODBX,	0},
      {-1},
#line 201 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str175,			"AcDbBlockPointParameterEntity",      	ODBX,	1},
      {-1},
#line 178 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str177,	"AcDbAssocRestoreEntityStateActionBody",ODBX,	0},
#line 131 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str178,	"AcDbShChamfer",	ODBX,	0},
      {-1}, {-1},
#line 151 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str181,	"AcDbAssocVertexActionParam",	ODBX,	0},
#line 84 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str182,	"AcDbXrecord",		ODBX,	0},
#line 155 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str183,	"AcDbBlockParameterDependencyBody",	ODBX,	0},
#line 251 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str184,	"AcDbAngularDimensionObjectContextData",ODBX,	0},
#line 215 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str185,			"AcDbBlockMoveAction",			ODBX,	0},
#line 239 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str186,			"AcDbBlockXYParameter",			ODBX,	0},
      {-1},
#line 70 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str188,	"AcDbArcDimension",	ODBX,	1},
#line 256 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str189,	"AcDbLeaderObjectContextData",		ODBX,	0},
#line 210 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str190,		"AcDbBlockRadialConstraintParameter",	ODBX,	0},
#line 200 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str191,			"AcDbBlockLinearParameterEntity",      	ODBX,	1},
      {-1},
#line 139 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str193,	"AcDbShRevolve",	ODBX,	0},
#line 187 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str194,		"AcDbAssocFaceActionParam",	ODBX,	0},
#line 259 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str195,	"AcDbRadialDimensionLargeObjectContextData",	ODBX,	0},
      {-1}, {-1},
#line 247 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str198,			ODBX,	0},
#line 133 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str199,	"AcDbShCylinder",	ODBX,	0},
      {-1}, {-1},
#line 226 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str202,			"AcDbBlockRotationGrip",		ODBX,	0},
#line 202 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str203,		"AcDbBlockRotationParameterEntity",     ODBX,	1},
#line 129 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str204,		"AcDbShBox",		ODBX,	0},
#line 257 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str205,	"AcDbMLeaderObjectContextData",		ODBX,	0},
      {-1}, {-1}, {-1},
#line 223 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str209,			"AcDbBlockPointParameter",		ODBX,	0},
      {-1},
#line 142 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str211,	"AcDbShTorus",		ODBX,	0},
      {-1}, {-1}, {-1},
#line 232 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str215,			"AcDbBlockLinearParameter",		ODBX,	0},
      {-1},
#line 218 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str217,			"AcDbBlockRotateAction",		ODBX,	0},
#line 92 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str218,	"AcDbTableGeometry",	ODBX,	0},
#line 227 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str219,			"AcDbBlockVisibilityGrip",		ODBX,	0},
      {-1},
#line 208 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str221,	"AcDbBlockAngularConstraintParameter",  ODBX,	0},
#line 224 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str222,			"AcDbBlockRotationParameter",		ODBX,	0},
#line 98 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str223,	"AcDbRasterImageDef",	ISM,	0},
#line 87 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str224,	"AcDbSpatialFilter",	ODBX,	0},
#line 159 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str225,	"AcDbAssocOrdinatedDimActionBody",	ODBX,	0},
#line 123 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str226,		"AcDbNurbSurface",	ODBX,	1},
      {-1},
#line 252 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str228,	"AcDbAnnotScaleObjectContextData",	ODBX,	0},
#line 69 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str229,	"AcDbOle2Frame",	ODBX,	1},
#line 236 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str230,			"AcDbBlockLinearGrip",			ODBX,	0},
      {-1}, {-1},
#line 128 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str233,	"AcDbShBoolean",	ODBX,	0},
      {-1},
#line 121 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str235,	"AcDbExtrudedSurface",	ODBX,	1},
      {-1}, {-1},
#line 106 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str238, 	"AcDbDimAssoc",		SPECIAL, 0},
#line 240 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str239,		"AcDbBlockVisibilityParameter",		ODBX,	0},
      {-1},
#line 228 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str241,				"AcDbBlockXYGrip",			ODBX,	0},
      {-1}, {-1},
#line 179 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str244,	"AcDbAssocRevolvedSurfaceActionBody",	ODBX,	0},
#line 102 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str245,		"AcDbLight",		SCENEOE, 1},
#line 85 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str246,	"AcDbLayerIndex",	ODBX,	0},
      {-1}, {-1}, {-1},
#line 125 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str250,	"AcDbRevolvedSurface",	ODBX,	1},
      {-1}, {-1}, {-1},
#line 198 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str254,		"AcDbBlockBasepointParameterEntity",	ODBX,	1},
      {-1},
#line 71 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str256,	"AcDbMLeader",		ODBX,	1},
      {-1},
#line 136 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str258,	"AcDbShHistory",	ODBX,	0},
      {-1},
#line 222 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str260,			"AcDbBlockLookupParameter",		ODBX,	0},
      {-1},
#line 217 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str262,			"AcDbBlockLookupAction",		ODBX,	0},
#line 250 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str263,	"AcDbAlignedDimensionObjectContextData",ODBX,	0},
      {-1}, {-1},
#line 243 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str266,		"AcDbDynamicBlockProxyNode",	       	ODBX,	0},
#line 173 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str267,	"AcDbAssocLoftedSurfaceActionBody",	ODBX,	0},
      {-1}, {-1},
#line 137 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str270,	"AcDbShLoft",		ODBX,	0},
      {-1}, {-1}, {-1}, {-1},
#line 237 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str275,			"AcDbBlockLookupGrip",			ODBX,	0},
#line 143 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str276,	"AcDbShWedge",		ODBX,	0},
      {-1}, {-1}, {-1},
#line 132 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str280,	"AcDbShCone",		ODBX,	0},
      {-1}, {-1},
#line 185 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str283,		"AcDbAssocCompoundActionParam",	ODBX,	0},
#line 248 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str284,			ODBX,	0},
      {-1}, {-1}, {-1},
#line 158 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str288,	"AcDbAssocAlignedDimActionBody",	ODBX,	0},
#line 149 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str289,	"AcDbAssocAlignedDimActionBody",	ODBX,	0},
      {-1},
#line 108 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str291,"AcDbWipeoutVariables",	ODBX,	0},
      {-1},
#line 260 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str293,	"AcDbRadialDimensionObjectContextData",	ODBX,	0},
      {-1},
#line 212 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str295,		"AcDbBlockLinearConstraintParameter",	ODBX,	0},
      {-1},
#line 214 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str297,	"AcDbBlockVerticalConstraintParameter",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1},
#line 221 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str303,			"AcDbBlockStretchAction",	       	ODBX,	0},
#line 244 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str304,"AcDbDynamicBlockPurgePreventer",      	ODBX,	0},
#line 75 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str305,		"AcDbPlaceHolder",		ODBX_OR_A2000CLASS,	0},
#line 99 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str306,"AcDbRasterImageDefReactor",	ISM,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 122 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str313,		"AcDbLoftedSurface",	ODBX,	1},
      {-1}, {-1}, {-1}, {-1},
#line 272 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str318,	"AcDbImpNonPersistentObjectsCollection", ODBX, 0},
#line 213 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str319,	"AcDbBlockHorizontalConstraintParameter",ODBX,	0},
      {-1}, {-1},
#line 175 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str322,	"AcDbAssocOffsetSurfaceActionBody",	ODBX,	0},
      {-1},
#line 117 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str324,		"AcDbGeoPositionMarker",ODBX,	0},
#line 134 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str325,	"AcDbShExtrusion",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 107 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str332,	"AcDbWipeout", 		SPECIAL, 1},
#line 79 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str333,	"AcDbDictionaryVar",	A2000CLASS,	0},
#line 110 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str334, "AcDbArcAlignedText",	EXPRESS, 1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 254 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str343,	"AcDbDiametricDimensionObjectContextData",ODBX,	0},
      {-1}, {-1},
#line 89 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str346,	"AcDbVisualStyle",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1},
#line 80 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str351,		"AcDbLayout",		A2000CLASS,	0},
      {-1},
#line 113 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str353,		"AcDbMaterial",		ODBX,	0},
      {-1},
#line 100 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str355,"AcDbRasterVariables",	ISM,	0},
#line 83 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str356,	"AcDbVbaProject",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 233 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str363,		"AcDbBlockBasepointParameter",		ODBX,	0},
      {-1}, {-1}, {-1},
#line 93 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str367,	"AcDbCellStyleMap",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1},
#line 265 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str372,			"AcDbPointCloudDef",			POINTCLOUD, 0},
      {-1},
#line 258 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str374,	"AcDbOrdinateDimensionObjectContextData",	ODBX,	0},
      {-1},
#line 189 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str376,		"AcDbAssocObjectActionParam",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 190 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str383,		"AcDbAssocPointRefActionParam",	ODBX,	0},
      {-1},
#line 115 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str385,	"AcDbSectionViewStyle",	ODBX,	0},
      {-1},
#line 269 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str387,			"AcDbPointCloudColorMap",		POINTCLOUD, 0},
#line 167 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str388,		"AcDbContextDataManager",		ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 181 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str395,		"AcDbAssocTrimSurfaceActionBody",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 235 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str403,				"AcDbBlockFlipGrip",			ODBX,	0},
      {-1},
#line 267 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str405,		"AcDbPointCloudDefReactor",		POINTCLOUD, 0},
      {-1},
#line 261 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str407,	"AcDbTextObjectContextData",		ODBX,	0},
      {-1},
#line 116 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str409,	"AcDbDetailViewStyle",	ODBX,	0},
      {-1}, {-1},
#line 91 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str412,	"AcDbTableContent",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 264 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str431,			"AcDbPointCloudEx",			POINTCLOUD, 1},
      {-1}, {-1},
#line 266 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str434,			"AcDbPointCloudDefEx",			POINTCLOUD, 0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 268 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str443,		"AcDbPointCloudDefReactorEx",		POINTCLOUD, 0},
#line 263 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str444,				"AcDbPointCloud",			POINTCLOUD, 1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 229 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str461,		"AcDbBlockGripExpr",			ODBX,	0},
      {-1}, {-1},
#line 74 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str464,		"AcDbDictionaryWithDefault",	ODBX_OR_A2000CLASS,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 150 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str498,"AcDbAssocOsnapPointRefActionParam",	ODBX,	0}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          register int o = wordlist[key].name;
          if (o >= 0)
            {
              register const char *s = o + stringpool;

              if (*str == *s && !strcmp (str + 1, s + 1))
                return &wordlist[key];
            }
        }
    }
  return 0;
}
#line 273 "src/dxfclasses.in"


/* Create classes on demand.
   Returns found or new klass->number id (always >=500), <0 on error.
   -1 out of memory (from dwg_add_class())
   -2 unknown class for dxfname.
   -3 invalid apptype for class. (should not happen)
*/
EXPORT int
dwg_require_class (Dwg_Data *restrict dwg, const char *const restrict dxfname, const int len)
{
  const struct Dwg_DXFClass* result;
  for (BITCODE_BL i = 0; i < dwg->num_classes; i++)
    {
      Dwg_Class *klass = &dwg->dwg_class[i];
      if (strEQ (klass->dxfname, dxfname))
        return klass->number;
    }

  result = in_word_set (dxfname, len);
  if (result)
    {
      switch ((enum apptypes)result->apptype)
        {
        case ODBXCLASS:
          return dwg_add_class (dwg, dxfname, result->cppname, "ObjectDBX Classes", result->isent);
        case ODBX_OR_A2000CLASS:
          return dwg_add_class (dwg, dxfname, result->cppname,
                         dwg->header.from_version <= R_2000 ? "AutoCAD 2000" : "ObjectDBX Classes",
                         result->isent);
        case A2000CLASS:
          return dwg_add_class (dwg, dxfname, result->cppname, "AutoCAD 2000", result->isent);
        case SCENEOECLASS:
          return dwg_add_class (dwg, dxfname, result->cppname, "SCENEOE", result->isent);
        case ISMCLASS:
          return dwg_add_class (dwg, dxfname, result->cppname, "ISM", result->isent);
        case POINTCLOUDCLASS:
          return dwg_add_class (dwg, dxfname, result->cppname, "AcDbPointCloudObj", result->isent); //appname?
        case EXPRESSCLASS:
          {
            char appname[128];
            strcpy (appname, dxfname);
            strcat (appname, "|AutoCAD Express Tool");
            return dwg_add_class (dwg, dxfname, result->cppname, appname, result->isent);
          }
        case SPECIALCLASS:
          {
            char appname[128];
            if (strEQc (dxfname, "MLEADERSTYLE"))
              strcpy (appname, "ACDB_MLEADERSTYLE_CLASS");
            else if (strEQc (dxfname, "WIPEOUT"))
              strcpy (appname, "Wipeout|Product Desc:     WipeOut Dbx Application");
            else if (strEQc (dxfname, "DIMASSOC"))
              strcpy (appname, "AcDbDimAssoc|Product Desc:     AcDim ARX App For Dimension");
            else
              {
                strcpy (appname, result->cppname);
                strcat (appname, "|Unknown ARX App");
              }
            return dwg_add_class (dwg, dxfname, result->cppname, appname, result->isent);
          }
          break;
        default:
          fprintf (stderr, "dxfclass_require: Invalid apptype %d", (int)result->apptype);
          return -3;
        }
    }
  return -2;
}

/*
 * Local variables:
 *   c-file-style: "gnu"
 * End:
 * vim: expandtab shiftwidth=4 cinoptions='\:2=2' :
 */
