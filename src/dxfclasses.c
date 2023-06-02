/* ANSI-C code produced by gperf version 3.1 */
/* Command-line: gperf --output-file src/dxfclasses.tmp.c src/dxfclasses.in  */
/* Computed positions: -k'2,6,10,12,21' */

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
#include <limits.h>
#include "config.h"
#include "dwg.h"
#include "common.h"
// other imports
int dwg_add_class (Dwg_Data *restrict dwg, const char *const restrict dxfname,
                   const char *const restrict cppname, const char *const restrict appname,
                   const bool is_entity);
// export
EXPORT int dwg_require_class (Dwg_Data *restrict dwg,
                              const char *const restrict dxfname, const size_t len);

// v3.1 changed len type from unsigned int to size_t (gperf d519d1a821511eaa22eae6d9019a548aea21e6)
static const struct Dwg_DXFClass *in_word_set (const char *str, size_t len);

enum apptypes
{
  ODBXCLASS,
  ODBX_OR_A2000CLASS,
  A2000CLASS,
  SCENEOECLASS,
  ISMCLASS,
  EXPRESSCLASS,
  POINTCLOUDCLASS,
  DGNLSCLASS,
  AEC60CLASS,
  SPECIALCLASS,
};
#define ODBX ODBXCLASS
#define ISM ISMCLASS
#define SCENEOE SCENEOECLASS
#define EXPRESS EXPRESSCLASS
#define POINTCLOUD POINTCLOUDCLASS
#define DGNLS DGNLSCLASS
#define AEC60 AEC60CLASS
#define SPECIAL SPECIALCLASS
//%null-strings

#line 70 "src/dxfclasses.in"
struct Dwg_DXFClass {int name; const char *const cppname; unsigned apptype:4; unsigned isent:1;};

#define TOTAL_KEYWORDS 213
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 39
#define MIN_HASH_VALUE 5
#define MAX_HASH_VALUE 817
/* maximum key range = 813, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (const char *str, size_t len)
{
  static const unsigned short asso_values[] =
    {
      818, 818, 818, 818, 818, 818, 818, 818, 818, 818,
      818, 818, 818, 818, 818, 818, 818, 818, 818, 818,
      818, 818, 818, 818, 818, 818, 818, 818, 818, 818,
      818, 818, 818, 818, 818, 818, 818, 818, 818, 818,
      818, 818, 818, 818, 818, 818, 818, 818, 818, 818,
      818,  25, 818, 818, 818, 818, 818, 818, 818, 818,
      818, 818, 818, 818, 818,   0, 175,   5, 165,  25,
       30, 215,   5,  70,  35,  55,  50, 228, 209,  20,
       10,  75,  90,   5,  55,   5,   0, 100, 100,  90,
        0, 818, 818, 818, 818,  35,  80, 818, 818, 818,
      818, 818, 818, 818, 818, 818, 818, 818, 818, 818,
      818, 818, 818, 818, 818, 818, 818, 818, 818, 818,
      818, 818, 818, 818, 818, 818, 818, 818, 818
    };
  unsigned int hval = len & UINT_MAX;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[20]];
      /*FALLTHROUGH*/
      case 20:
      case 19:
      case 18:
      case 17:
      case 16:
      case 15:
      case 14:
      case 13:
      case 12:
        hval += asso_values[(unsigned char)str[11]+1];
      /*FALLTHROUGH*/
      case 11:
      case 10:
        hval += asso_values[(unsigned char)str[9]];
      /*FALLTHROUGH*/
      case 9:
      case 8:
      case 7:
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
      case 4:
      case 3:
      case 2:
        hval += asso_values[(unsigned char)str[1]];
        break;
    }
  return hval;
}

struct stringpool_t
  {
    char stringpool_str5[sizeof("HATCH")];
    char stringpool_str8[sizeof("SUN")];
    char stringpool_str9[sizeof("DATATABLE")];
    char stringpool_str10[sizeof("SCALE")];
    char stringpool_str17[sizeof("SURFACE")];
    char stringpool_str18[sizeof("SUNSTUDY")];
    char stringpool_str26[sizeof("NURBSURFACE")];
    char stringpool_str30[sizeof("ACDBASSOCACTION")];
    char stringpool_str35[sizeof("ACDBASSOCACTIONPARAM")];
    char stringpool_str38[sizeof("ACDBCURVEPATH")];
    char stringpool_str40[sizeof("TABLESTYLE")];
    char stringpool_str44[sizeof("ACDBASSOCARRAYPOLARPARAMETERS")];
    char stringpool_str45[sizeof("ACDBASSOCARRAYMODIFYACTIONBODY")];
    char stringpool_str47[sizeof("TABLECONTENT")];
    char stringpool_str48[sizeof("ACSH_HISTORY_CLASS")];
    char stringpool_str49[sizeof("ACDBASSOCPATHACTIONPARAM")];
    char stringpool_str51[sizeof("ACDBASSOCPERSSUBENTMANAGER")];
    char stringpool_str53[sizeof("ACDBPOINTPATH")];
    char stringpool_str54[sizeof("ACDBPOINTCLOUD")];
    char stringpool_str55[sizeof("ACDBASSOCARRAYMODIFYPARAMETERS")];
    char stringpool_str56[sizeof("ACDBPOINTCLOUDEX")];
    char stringpool_str57[sizeof("ACDBPOINTCLOUDDEF")];
    char stringpool_str59[sizeof("ACDBPOINTCLOUDDEFEX")];
    char stringpool_str60[sizeof("RTEXT")];
    char stringpool_str61[sizeof("LAYOUT")];
    char stringpool_str62[sizeof("ACDBPOINTCLOUDCOLORMAP")];
    char stringpool_str63[sizeof("ACSH_CHAMFER_CLASS")];
    char stringpool_str64[sizeof("ACDBASSOCEDGEACTIONPARAM")];
    char stringpool_str65[sizeof("ACDBPOINTCLOUDDEF_REACTOR")];
    char stringpool_str66[sizeof("ACAD_EVALUATION_GRAPH")];
    char stringpool_str68[sizeof("ACDBPOINTCLOUDDEF_REACTOR_EX")];
    char stringpool_str70[sizeof("ACDBASSOCEDGECHAMFERACTIONBODY")];
    char stringpool_str71[sizeof("LAYER_INDEX")];
    char stringpool_str74[sizeof("ACDBASSOCEDGEFILLETACTIONBODY")];
    char stringpool_str75[sizeof("LIGHT")];
    char stringpool_str77[sizeof("ACDBASSOCEXTENDSURFACEACTIONBODY")];
    char stringpool_str78[sizeof("MATERIAL")];
    char stringpool_str79[sizeof("ACDBASSOCALIGNEDDIMACTIONBODY")];
    char stringpool_str80[sizeof("ACDBASSOC3POINTANGULARDIMACTIONBODY")];
    char stringpool_str81[sizeof("ACDBASSOCPATCHSURFACEACTIONBODY")];
    char stringpool_str82[sizeof("WIPEOUT")];
    char stringpool_str83[sizeof("DIMASSOC")];
    char stringpool_str85[sizeof("ACDBASSOCORDINATEDIMACTIONBODY")];
    char stringpool_str88[sizeof("BLOCKUSERPARAMETER")];
    char stringpool_str90[sizeof("ACSH_EXTRUSION_CLASS")];
    char stringpool_str91[sizeof("MULTILEADER")];
    char stringpool_str95[sizeof("ACAD_TABLE")];
    char stringpool_str97[sizeof("PLANESURFACE")];
    char stringpool_str99[sizeof("SPATIAL_FILTER")];
    char stringpool_str100[sizeof("BLOCKFLIPACTION")];
    char stringpool_str101[sizeof("ACDBASSOCSWEPTSURFACEACTIONBODY")];
    char stringpool_str104[sizeof("ACDBASSOCEXTRUDEDSURFACEACTIONBODY")];
    char stringpool_str108[sizeof("ACDBASSOCOSNAPPOINTREFACTIONPARAM")];
    char stringpool_str109[sizeof("LSINTERNALCOMPONENT")];
    char stringpool_str110[sizeof("BLOCKPROPERTIESTABLE")];
    char stringpool_str113[sizeof("BLOCKFLIPPARAMETER")];
    char stringpool_str114[sizeof("BASEPOINTPARAMETERENTITY")];
    char stringpool_str120[sizeof("BLOCKLINEARGRIP")];
    char stringpool_str123[sizeof("ACDBBLOCKPARAMDEPENDENCYBODY")];
    char stringpool_str124[sizeof("FLIPPARAMETERENTITY")];
    char stringpool_str125[sizeof("BLOCKLOOKUPGRIP")];
    char stringpool_str128[sizeof("ACDBASSOCALIGNEDIMACTIONBODY")];
    char stringpool_str129[sizeof("LIGHTLIST")];
    char stringpool_str130[sizeof("ACDBASSOCTRIMSURFACEACTIONBODY")];
    char stringpool_str131[sizeof("SOLID_BACKGROUND")];
    char stringpool_str133[sizeof("ACDBASSOCARRAYPATHPARAMETERS")];
    char stringpool_str134[sizeof("ACDBDETAILVIEWSTYLE")];
    char stringpool_str138[sizeof("NPOCOLLECTION")];
    char stringpool_str139[sizeof("ACDBASSOCROTATEDDIMACTIONBODY")];
    char stringpool_str140[sizeof("ACAD_PROXY_OBJECT_WRAPPER")];
    char stringpool_str147[sizeof("SWEPTSURFACE")];
    char stringpool_str149[sizeof("OLE2FRAME")];
    char stringpool_str153[sizeof("ACDBASSOCPOINTREFACTIONPARAM")];
    char stringpool_str154[sizeof("POSITIONMARKER")];
    char stringpool_str157[sizeof("ACSH_FILLET_CLASS")];
    char stringpool_str160[sizeof("ACAD_PROXY_ENTITY_WRAPPER")];
    char stringpool_str162[sizeof("XYPARAMETERENTITY")];
    char stringpool_str169[sizeof("BLOCKPOLARGRIP")];
    char stringpool_str170[sizeof("ACDBSECTIONVIEWSTYLE")];
    char stringpool_str174[sizeof("ACDB_ANGDIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str176[sizeof("ACDBPERSSUBENTMANAGER")];
    char stringpool_str177[sizeof("LSDEFINITION")];
    char stringpool_str179[sizeof("ACDB_LEADEROBJECTCONTEXTDATA_CLASS")];
    char stringpool_str180[sizeof("RASTERVARIABLES")];
    char stringpool_str181[sizeof("VISUALSTYLE")];
    char stringpool_str187[sizeof("XRECORD")];
    char stringpool_str188[sizeof("ARC_DIMENSION")];
    char stringpool_str193[sizeof("ACSH_REVOLVE_CLASS")];
    char stringpool_str194[sizeof("ACDB_ORDDIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str195[sizeof("BLOCKLINEARPARAMETER")];
    char stringpool_str197[sizeof("ACSH_SPHERE_CLASS")];
    char stringpool_str198[sizeof("BLOCKSTRETCHACTION")];
    char stringpool_str199[sizeof("BLOCKHORIZONTALCONSTRAINTPARAMETER")];
    char stringpool_str200[sizeof("BLOCKLOOKUPPARAMETER")];
    char stringpool_str201[sizeof("ACSH_SWEEP_CLASS")];
    char stringpool_str203[sizeof("IDBUFFER")];
    char stringpool_str207[sizeof("MLEADERSTYLE")];
    char stringpool_str208[sizeof("DWFDEFINITION")];
    char stringpool_str209[sizeof("LSCOMPOUNDCOMPONENT")];
    char stringpool_str212[sizeof("RENDERENVIRONMENT")];
    char stringpool_str214[sizeof("ACDBASSOCARRAYACTIONBODY")];
    char stringpool_str216[sizeof("RENDERENTRY")];
    char stringpool_str217[sizeof("PARTIAL_VIEWING_FILTER")];
    char stringpool_str222[sizeof("BLOCKROTATIONGRIP")];
    char stringpool_str225[sizeof("LWPOLYLINE")];
    char stringpool_str229[sizeof("ACDBASSOCFACEACTIONPARAM")];
    char stringpool_str231[sizeof("BLOCKXYGRIP")];
    char stringpool_str233[sizeof("IMAGE")];
    char stringpool_str239[sizeof("EXACXREFPANELOBJECT")];
    char stringpool_str240[sizeof("ACDBASSOCNETWORK")];
    char stringpool_str243[sizeof("ACSH_BOOLEAN_CLASS")];
    char stringpool_str246[sizeof("ACSH_TORUS_CLASS")];
    char stringpool_str247[sizeof("ACDBASSOCRESTOREENTITYSTATEACTIONBODY")];
    char stringpool_str248[sizeof("BLOCKPOLARSTRETCHACTION")];
    char stringpool_str250[sizeof("ACDBASSOCVERTEXACTIONPARAM")];
    char stringpool_str251[sizeof("ACDBASSOCPLANESURFACEACTIONBODY")];
    char stringpool_str252[sizeof("BLOCKROTATIONPARAMETER")];
    char stringpool_str257[sizeof("ACDBASSOCNETWORKSURFACEACTIONBODY")];
    char stringpool_str258[sizeof("TABLEGEOMETRY")];
    char stringpool_str260[sizeof("REVOLVEDSURFACE")];
    char stringpool_str261[sizeof("BLOCKSCALEACTION")];
    char stringpool_str262[sizeof("SPATIAL_INDEX")];
    char stringpool_str263[sizeof("LSSTROKEPATTERNCOMPONENT")];
    char stringpool_str264[sizeof("ACDBASSOCREVOLVEDSURFACEACTIONBODY")];
    char stringpool_str265[sizeof("ACDBASSOCARRAYRECTANGULARPARAMETERS")];
    char stringpool_str266[sizeof("ACDBASSOCASMBODYACTIONPARAM")];
    char stringpool_str267[sizeof("ACDB_TEXTOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str268[sizeof("ACSH_CYLINDER_CLASS")];
    char stringpool_str269[sizeof("ACDBASSOCDEPENDENCY")];
    char stringpool_str271[sizeof("ACDBASSOCBLENDSURFACEACTIONBODY")];
    char stringpool_str273[sizeof("PDFDEFINITION")];
    char stringpool_str276[sizeof("DWFUNDERLAY")];
    char stringpool_str278[sizeof("BLOCKDIAMETRICCONSTRAINTPARAMETER")];
    char stringpool_str280[sizeof("POLARGRIPENTITY")];
    char stringpool_str281[sizeof("VBA_PROJECT")];
    char stringpool_str282[sizeof("ACDBASSOCOFFSETSURFACEACTIONBODY")];
    char stringpool_str283[sizeof("POINTPARAMETERENTITY")];
    char stringpool_str286[sizeof("ACSH_PYRAMID_CLASS")];
    char stringpool_str288[sizeof("ACSH_CONE_CLASS")];
    char stringpool_str289[sizeof("ACDBDICTIONARYWDFLT")];
    char stringpool_str291[sizeof("PARTIAL_VIEWING_INDEX")];
    char stringpool_str292[sizeof("LSSYMBOLCOMPONENT")];
    char stringpool_str296[sizeof("ACDB_ALDIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str297[sizeof("BLOCKLOOKUPACTION")];
    char stringpool_str298[sizeof("BLOCKMOVEACTION")];
    char stringpool_str301[sizeof("LINEARPARAMETERENTITY")];
    char stringpool_str303[sizeof("ACDBPLACEHOLDER")];
    char stringpool_str304[sizeof("ACDB_BLKREFOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str305[sizeof("ACDBASSOCFILLETSURFACEACTIONBODY")];
    char stringpool_str307[sizeof("BLOCKALIGNMENTGRIP")];
    char stringpool_str309[sizeof("BLOCKPOINTPARAMETER")];
    char stringpool_str311[sizeof("ACSH_WEDGE_CLASS")];
    char stringpool_str312[sizeof("ACDBASSOCLOFTEDSURFACEACTIONBODY")];
    char stringpool_str313[sizeof("GROUND_PLANE_BACKGROUND")];
    char stringpool_str314[sizeof("ACDBASSOCMLEADERACTIONBODY")];
    char stringpool_str316[sizeof("AEC_REFEDIT_STATUS_TRACKER")];
    char stringpool_str318[sizeof("ACDB_ANNOTSCALEOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str320[sizeof("ACDBASSOCOBJECTACTIONPARAM")];
    char stringpool_str321[sizeof("BLOCKARRAYACTION")];
    char stringpool_str322[sizeof("ACDBASSOCCOMPOUNDACTIONPARAM")];
    char stringpool_str323[sizeof("DGNDEFINITION")];
    char stringpool_str326[sizeof("ACDB_FCFOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str329[sizeof("BLOCKPROPERTIESTABLEGRIP")];
    char stringpool_str333[sizeof("ACSH_LOFT_CLASS")];
    char stringpool_str339[sizeof("IMAGE_BACKGROUND")];
    char stringpool_str341[sizeof("PDFUNDERLAY")];
    char stringpool_str343[sizeof("BLOCKFLIPGRIP")];
    char stringpool_str344[sizeof("BLOCKPOLARPARAMETER")];
    char stringpool_str345[sizeof("BLOCKANGULARCONSTRAINTPARAMETER")];
    char stringpool_str348[sizeof("BLOCKBASEPOINTPARAMETER")];
    char stringpool_str349[sizeof("ACDB_BLOCKREPRESENTATION_DATA")];
    char stringpool_str350[sizeof("BLOCKLINEARCONSTRAINTPARAMETER")];
    char stringpool_str351[sizeof("PLOTSETTINGS")];
    char stringpool_str355[sizeof("RAPIDRTRENDERSETTINGS")];
    char stringpool_str356[sizeof("WIPEOUTVARIABLES")];
    char stringpool_str358[sizeof("ROTATIONPARAMETERENTITY")];
    char stringpool_str367[sizeof("BLOCKALIGNMENTPARAMETER")];
    char stringpool_str368[sizeof("DICTIONARYVAR")];
    char stringpool_str369[sizeof("ARCALIGNEDTEXT")];
    char stringpool_str372[sizeof("ALIGNMENTPARAMETERENTITY")];
    char stringpool_str374[sizeof("ACSH_BOX_CLASS")];
    char stringpool_str386[sizeof("ACDB_RADIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str387[sizeof("BLOCKROTATEACTION")];
    char stringpool_str390[sizeof("BLOCKRADIALCONSTRAINTPARAMETER")];
    char stringpool_str391[sizeof("DGNUNDERLAY")];
    char stringpool_str393[sizeof("LOFTEDSURFACE")];
    char stringpool_str395[sizeof("CELLSTYLEMAP")];
    char stringpool_str397[sizeof("BLOCKVERTICALCONSTRAINTPARAMETER")];
    char stringpool_str401[sizeof("IMAGEDEF")];
    char stringpool_str402[sizeof("CONTEXTDATAMANAGER")];
    char stringpool_str415[sizeof("ACDBASSOCDIMDEPENDENCYBODY")];
    char stringpool_str417[sizeof("MENTALRAYRENDERSETTINGS")];
    char stringpool_str423[sizeof("ACDBMOTIONPATH")];
    char stringpool_str458[sizeof("ACSH_BREP_CLASS")];
    char stringpool_str461[sizeof("ACDB_DMDIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str465[sizeof("BLOCKXYPARAMETER")];
    char stringpool_str467[sizeof("ACDBASSOCGEOMDEPENDENCY")];
    char stringpool_str468[sizeof("LSPOINTCOMPONENT")];
    char stringpool_str470[sizeof("SORTENTSTABLE")];
    char stringpool_str472[sizeof("BLOCKVISIBILITYGRIP")];
    char stringpool_str474[sizeof("GRADIENT_BACKGROUND")];
    char stringpool_str500[sizeof("EXTRUDEDSURFACE")];
    char stringpool_str502[sizeof("BLOCKVISIBILITYPARAMETER")];
    char stringpool_str516[sizeof("BLOCKGRIPLOCATIONCOMPONENT")];
    char stringpool_str524[sizeof("BLOCKALIGNEDCONSTRAINTPARAMETER")];
    char stringpool_str530[sizeof("RENDERGLOBAL")];
    char stringpool_str572[sizeof("ACDB_RADIMLGOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str581[sizeof("RAPIDRTRENDERENVIRONMENT")];
    char stringpool_str629[sizeof("SKYLIGHT_BACKGROUND")];
    char stringpool_str647[sizeof("ACDB_MLEADEROBJECTCONTEXTDATA_CLASS")];
    char stringpool_str674[sizeof("IMAGEDEF_REACTOR")];
    char stringpool_str689[sizeof("ACDB_DYNAMICBLOCKPROXYNODE")];
    char stringpool_str817[sizeof("ACDB_DYNAMICBLOCKPURGEPREVENTER_VERSION")];
  };
static const struct stringpool_t stringpool_contents =
  {
    "HATCH",
    "SUN",
    "DATATABLE",
    "SCALE",
    "SURFACE",
    "SUNSTUDY",
    "NURBSURFACE",
    "ACDBASSOCACTION",
    "ACDBASSOCACTIONPARAM",
    "ACDBCURVEPATH",
    "TABLESTYLE",
    "ACDBASSOCARRAYPOLARPARAMETERS",
    "ACDBASSOCARRAYMODIFYACTIONBODY",
    "TABLECONTENT",
    "ACSH_HISTORY_CLASS",
    "ACDBASSOCPATHACTIONPARAM",
    "ACDBASSOCPERSSUBENTMANAGER",
    "ACDBPOINTPATH",
    "ACDBPOINTCLOUD",
    "ACDBASSOCARRAYMODIFYPARAMETERS",
    "ACDBPOINTCLOUDEX",
    "ACDBPOINTCLOUDDEF",
    "ACDBPOINTCLOUDDEFEX",
    "RTEXT",
    "LAYOUT",
    "ACDBPOINTCLOUDCOLORMAP",
    "ACSH_CHAMFER_CLASS",
    "ACDBASSOCEDGEACTIONPARAM",
    "ACDBPOINTCLOUDDEF_REACTOR",
    "ACAD_EVALUATION_GRAPH",
    "ACDBPOINTCLOUDDEF_REACTOR_EX",
    "ACDBASSOCEDGECHAMFERACTIONBODY",
    "LAYER_INDEX",
    "ACDBASSOCEDGEFILLETACTIONBODY",
    "LIGHT",
    "ACDBASSOCEXTENDSURFACEACTIONBODY",
    "MATERIAL",
    "ACDBASSOCALIGNEDDIMACTIONBODY",
    "ACDBASSOC3POINTANGULARDIMACTIONBODY",
    "ACDBASSOCPATCHSURFACEACTIONBODY",
    "WIPEOUT",
    "DIMASSOC",
    "ACDBASSOCORDINATEDIMACTIONBODY",
    "BLOCKUSERPARAMETER",
    "ACSH_EXTRUSION_CLASS",
    "MULTILEADER",
    "ACAD_TABLE",
    "PLANESURFACE",
    "SPATIAL_FILTER",
    "BLOCKFLIPACTION",
    "ACDBASSOCSWEPTSURFACEACTIONBODY",
    "ACDBASSOCEXTRUDEDSURFACEACTIONBODY",
    "ACDBASSOCOSNAPPOINTREFACTIONPARAM",
    "LSINTERNALCOMPONENT",
    "BLOCKPROPERTIESTABLE",
    "BLOCKFLIPPARAMETER",
    "BASEPOINTPARAMETERENTITY",
    "BLOCKLINEARGRIP",
    "ACDBBLOCKPARAMDEPENDENCYBODY",
    "FLIPPARAMETERENTITY",
    "BLOCKLOOKUPGRIP",
    "ACDBASSOCALIGNEDIMACTIONBODY",
    "LIGHTLIST",
    "ACDBASSOCTRIMSURFACEACTIONBODY",
    "SOLID_BACKGROUND",
    "ACDBASSOCARRAYPATHPARAMETERS",
    "ACDBDETAILVIEWSTYLE",
    "NPOCOLLECTION",
    "ACDBASSOCROTATEDDIMACTIONBODY",
    "ACAD_PROXY_OBJECT_WRAPPER",
    "SWEPTSURFACE",
    "OLE2FRAME",
    "ACDBASSOCPOINTREFACTIONPARAM",
    "POSITIONMARKER",
    "ACSH_FILLET_CLASS",
    "ACAD_PROXY_ENTITY_WRAPPER",
    "XYPARAMETERENTITY",
    "BLOCKPOLARGRIP",
    "ACDBSECTIONVIEWSTYLE",
    "ACDB_ANGDIMOBJECTCONTEXTDATA_CLASS",
    "ACDBPERSSUBENTMANAGER",
    "LSDEFINITION",
    "ACDB_LEADEROBJECTCONTEXTDATA_CLASS",
    "RASTERVARIABLES",
    "VISUALSTYLE",
    "XRECORD",
    "ARC_DIMENSION",
    "ACSH_REVOLVE_CLASS",
    "ACDB_ORDDIMOBJECTCONTEXTDATA_CLASS",
    "BLOCKLINEARPARAMETER",
    "ACSH_SPHERE_CLASS",
    "BLOCKSTRETCHACTION",
    "BLOCKHORIZONTALCONSTRAINTPARAMETER",
    "BLOCKLOOKUPPARAMETER",
    "ACSH_SWEEP_CLASS",
    "IDBUFFER",
    "MLEADERSTYLE",
    "DWFDEFINITION",
    "LSCOMPOUNDCOMPONENT",
    "RENDERENVIRONMENT",
    "ACDBASSOCARRAYACTIONBODY",
    "RENDERENTRY",
    "PARTIAL_VIEWING_FILTER",
    "BLOCKROTATIONGRIP",
    "LWPOLYLINE",
    "ACDBASSOCFACEACTIONPARAM",
    "BLOCKXYGRIP",
    "IMAGE",
    "EXACXREFPANELOBJECT",
    "ACDBASSOCNETWORK",
    "ACSH_BOOLEAN_CLASS",
    "ACSH_TORUS_CLASS",
    "ACDBASSOCRESTOREENTITYSTATEACTIONBODY",
    "BLOCKPOLARSTRETCHACTION",
    "ACDBASSOCVERTEXACTIONPARAM",
    "ACDBASSOCPLANESURFACEACTIONBODY",
    "BLOCKROTATIONPARAMETER",
    "ACDBASSOCNETWORKSURFACEACTIONBODY",
    "TABLEGEOMETRY",
    "REVOLVEDSURFACE",
    "BLOCKSCALEACTION",
    "SPATIAL_INDEX",
    "LSSTROKEPATTERNCOMPONENT",
    "ACDBASSOCREVOLVEDSURFACEACTIONBODY",
    "ACDBASSOCARRAYRECTANGULARPARAMETERS",
    "ACDBASSOCASMBODYACTIONPARAM",
    "ACDB_TEXTOBJECTCONTEXTDATA_CLASS",
    "ACSH_CYLINDER_CLASS",
    "ACDBASSOCDEPENDENCY",
    "ACDBASSOCBLENDSURFACEACTIONBODY",
    "PDFDEFINITION",
    "DWFUNDERLAY",
    "BLOCKDIAMETRICCONSTRAINTPARAMETER",
    "POLARGRIPENTITY",
    "VBA_PROJECT",
    "ACDBASSOCOFFSETSURFACEACTIONBODY",
    "POINTPARAMETERENTITY",
    "ACSH_PYRAMID_CLASS",
    "ACSH_CONE_CLASS",
    "ACDBDICTIONARYWDFLT",
    "PARTIAL_VIEWING_INDEX",
    "LSSYMBOLCOMPONENT",
    "ACDB_ALDIMOBJECTCONTEXTDATA_CLASS",
    "BLOCKLOOKUPACTION",
    "BLOCKMOVEACTION",
    "LINEARPARAMETERENTITY",
    "ACDBPLACEHOLDER",
    "ACDB_BLKREFOBJECTCONTEXTDATA_CLASS",
    "ACDBASSOCFILLETSURFACEACTIONBODY",
    "BLOCKALIGNMENTGRIP",
    "BLOCKPOINTPARAMETER",
    "ACSH_WEDGE_CLASS",
    "ACDBASSOCLOFTEDSURFACEACTIONBODY",
    "GROUND_PLANE_BACKGROUND",
    "ACDBASSOCMLEADERACTIONBODY",
    "AEC_REFEDIT_STATUS_TRACKER",
    "ACDB_ANNOTSCALEOBJECTCONTEXTDATA_CLASS",
    "ACDBASSOCOBJECTACTIONPARAM",
    "BLOCKARRAYACTION",
    "ACDBASSOCCOMPOUNDACTIONPARAM",
    "DGNDEFINITION",
    "ACDB_FCFOBJECTCONTEXTDATA_CLASS",
    "BLOCKPROPERTIESTABLEGRIP",
    "ACSH_LOFT_CLASS",
    "IMAGE_BACKGROUND",
    "PDFUNDERLAY",
    "BLOCKFLIPGRIP",
    "BLOCKPOLARPARAMETER",
    "BLOCKANGULARCONSTRAINTPARAMETER",
    "BLOCKBASEPOINTPARAMETER",
    "ACDB_BLOCKREPRESENTATION_DATA",
    "BLOCKLINEARCONSTRAINTPARAMETER",
    "PLOTSETTINGS",
    "RAPIDRTRENDERSETTINGS",
    "WIPEOUTVARIABLES",
    "ROTATIONPARAMETERENTITY",
    "BLOCKALIGNMENTPARAMETER",
    "DICTIONARYVAR",
    "ARCALIGNEDTEXT",
    "ALIGNMENTPARAMETERENTITY",
    "ACSH_BOX_CLASS",
    "ACDB_RADIMOBJECTCONTEXTDATA_CLASS",
    "BLOCKROTATEACTION",
    "BLOCKRADIALCONSTRAINTPARAMETER",
    "DGNUNDERLAY",
    "LOFTEDSURFACE",
    "CELLSTYLEMAP",
    "BLOCKVERTICALCONSTRAINTPARAMETER",
    "IMAGEDEF",
    "CONTEXTDATAMANAGER",
    "ACDBASSOCDIMDEPENDENCYBODY",
    "MENTALRAYRENDERSETTINGS",
    "ACDBMOTIONPATH",
    "ACSH_BREP_CLASS",
    "ACDB_DMDIMOBJECTCONTEXTDATA_CLASS",
    "BLOCKXYPARAMETER",
    "ACDBASSOCGEOMDEPENDENCY",
    "LSPOINTCOMPONENT",
    "SORTENTSTABLE",
    "BLOCKVISIBILITYGRIP",
    "GRADIENT_BACKGROUND",
    "EXTRUDEDSURFACE",
    "BLOCKVISIBILITYPARAMETER",
    "BLOCKGRIPLOCATIONCOMPONENT",
    "BLOCKALIGNEDCONSTRAINTPARAMETER",
    "RENDERGLOBAL",
    "ACDB_RADIMLGOBJECTCONTEXTDATA_CLASS",
    "RAPIDRTRENDERENVIRONMENT",
    "SKYLIGHT_BACKGROUND",
    "ACDB_MLEADEROBJECTCONTEXTDATA_CLASS",
    "IMAGEDEF_REACTOR",
    "ACDB_DYNAMICBLOCKPROXYNODE",
    "ACDB_DYNAMICBLOCKPURGEPREVENTER_VERSION"
  };
#define stringpool ((const char *) &stringpool_contents)
const struct Dwg_DXFClass *
in_word_set (const char *str, size_t len)
{
  static const struct Dwg_DXFClass wordlist[] =
    {
      {-1}, {-1}, {-1}, {-1}, {-1},
#line 75 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str5,		"AcDbHatch",		ODBX,	1},
      {-1}, {-1},
#line 117 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str8, 		"AcDbSun",		SCENEOE, 0},
#line 104 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str9,	"AcDbDataTable",	ODBX,	0},
#line 105 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str10,		"AcDbScale",		ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 134 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str17,		"AcDbSurface",		ODBX,	1},
#line 118 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str18,	"AcDbSunStudy",		SCENEOE, 0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 137 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str26,		"AcDbNurbSurface",	ODBX,	1},
      {-1}, {-1}, {-1},
#line 162 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str30,		"AcDbAssocAction",		ODBX,	0},
      {-1}, {-1}, {-1}, {-1},
#line 197 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str35,			"AcDbAssocActionParam",		ODBX,	0},
      {-1}, {-1},
#line 261 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str38,   			"AcDbCurvePath",			SCENEOE,	0},
      {-1},
#line 100 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str40,	"AcDbTableStyle",	ODBX,	0},
      {-1}, {-1}, {-1},
#line 208 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str44,		"AcDbAssocArrayPolarParameters",	ODBX,	0},
#line 176 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str45,	"AcDbAssocArrayModifyActionBody",	ODBX,	0},
      {-1},
#line 101 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str47,	"AcDbTableContent",	ODBX,	0},
#line 150 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str48,	"AcDbShHistory",	ODBX,	0},
#line 202 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str49,		"AcDbAssocPathActionParam",	ODBX,	0},
      {-1},
#line 159 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str51,	"AcDbAssocPersSubentManager",	ODBX,	0},
      {-1},
#line 263 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str53,   			"AcDbPointPath",			SCENEOE,	0},
#line 289 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str54,				"AcDbPointCloud",			POINTCLOUD, 1},
#line 206 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str55,		"AcDbAssocArrayModifyParameters",	ODBX,	0},
#line 290 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str56,			"AcDbPointCloudEx",			POINTCLOUD, 1},
#line 291 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str57,			"AcDbPointCloudDef",			POINTCLOUD, 0},
      {-1},
#line 292 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str59,			"AcDbPointCloudDefEx",			POINTCLOUD, 0},
#line 123 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str60, 		"AcDbRText", 		EXPRESS, 1},
#line 90 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str61,		"AcDbLayout",		A2000CLASS,	0},
#line 295 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str62,			"AcDbPointCloudColorMap",		POINTCLOUD, 0},
#line 145 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str63,	"AcDbShChamfer",	ODBX,	0},
#line 200 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str64,		"AcDbAssocEdgeActionParam",	ODBX,	0},
#line 293 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str65,		"AcDbPointCloudDefReactor",		POINTCLOUD, 0},
#line 132 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str66,	"AcDbEvalGraph",	ODBX,	0},
      {-1},
#line 294 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str68,		"AcDbPointCloudDefReactorEx",		POINTCLOUD, 0},
      {-1},
#line 178 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str70,	"AcDbAssocEdgeChamferActionBody",	ODBX,	0},
#line 95 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str71,	"AcDbLayerIndex",	ODBX,	0},
      {-1}, {-1},
#line 179 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str74,	"AcDbAssocEdgeFilletActionBody",	ODBX,	0},
#line 115 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str75,		"AcDbLight",		SCENEOE, 1},
      {-1},
#line 184 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str77,	"AcDbAssocExtendSurfaceActionBody",	ODBX,	0},
#line 127 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str78,		"AcDbMaterial",		ODBX,	0},
#line 163 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str79,	"AcDbAssocAlignedDimActionBody",	ODBX,	0},
#line 171 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str80,"AcDbAssoc3PointAngularDimActionBody",	ODBX,	0},
#line 191 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str81,	"AcDbAssocPatchSurfaceActionBody",	ODBX,	0},
#line 121 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str82,	"AcDbWipeout", 		SPECIAL, 1},
#line 120 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str83, 	"AcDbDimAssoc",		SPECIAL, 0},
      {-1},
#line 173 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str85,	"AcDbAssocOrdinatedDimActionBody",	ODBX,	0},
      {-1}, {-1},
#line 253 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str88,   			"AcDbBlockUserParameter",		ODBX,	0},
      {-1},
#line 148 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str90,	"AcDbShExtrusion",	ODBX,	0},
#line 78 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str91,	"AcDbMLeader",		ODBX,	1},
      {-1}, {-1}, {-1},
#line 79 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str95,	"AcDbTable",		ODBX,	1},
      {-1},
#line 138 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str97,		"AcDbPlaneSurface",	ODBX,	1},
      {-1},
#line 97 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str99,	"AcDbSpatialFilter",	ODBX,	0},
#line 230 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str100,			"AcDbBlockFlipAction",			ODBX,	0},
#line 194 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str101,	"AcDbAssocSweptSurfaceActionBody",	ODBX,	0},
      {-1}, {-1},
#line 185 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str104,	"AcDbAssocExtrudedSurfaceActionBody",	ODBX,	0},
      {-1}, {-1}, {-1},
#line 164 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str108,"AcDbAssocOsnapPointRefActionParam",	ODBX,	0},
#line 303 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str109,			"AcDbLSInternalComponent",	DGNLS, 0},
#line 256 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str110,			"AcDbBlockPropertiesTable",		ODBX,	0},
      {-1}, {-1},
#line 249 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str113,			"AcDbBlockFlipParameter",		ODBX,	0},
#line 212 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str114,		"AcDbBlockBasepointParameterEntity",	ODBX,	1},
      {-1}, {-1}, {-1}, {-1}, {-1},
#line 251 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str120,			"AcDbBlockLinearGrip",			ODBX,	0},
      {-1}, {-1},
#line 169 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str123,	"AcDbBlockParameterDependencyBody",	ODBX,	0},
#line 213 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str124,			"AcDbBlockFlipParameterEntity",		ODBX,	1},
#line 252 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str125,			"AcDbBlockLookupGrip",			ODBX,	0},
      {-1}, {-1},
#line 172 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str128,	"AcDbAssocAlignedDimActionBody",	ODBX,	0},
#line 116 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str129,	"AcDbLightList",	SCENEOE, 0},
#line 195 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str130,		"AcDbAssocTrimSurfaceActionBody",	ODBX,	0},
#line 272 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str131,	   		"AcDbSolidBackground",			SCENEOE,	0},
      {-1},
#line 207 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str133,		"AcDbAssocArrayPathParameters",		ODBX,	0},
#line 130 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str134,	"AcDbDetailViewStyle",	ODBX,	0},
      {-1}, {-1}, {-1},
#line 298 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str138,				"AcDbImpNonPersistentObjectsCollection", ODBX, 0},
#line 174 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str139,	"AcDbAssocRotatedDimActionBody",	ODBX,	0},
#line 87 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str140,	"AcDbProxyObjectWrapper",	ODBX_OR_A2000CLASS,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 140 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str147,		"AcDbSweptSurface",	ODBX,	1},
      {-1},
#line 76 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str149,	"AcDbOle2Frame",	ODBX,	1},
      {-1}, {-1}, {-1},
#line 204 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str153,		"AcDbAssocPointRefActionParam",	ODBX,	0},
#line 131 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str154,		"AcDbGeoPositionMarker",ODBX,	0},
      {-1}, {-1},
#line 149 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str157,	"AcDbShFillet",		ODBX,	0},
      {-1}, {-1},
#line 86 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str160,	"AcDbProxyEntityWrapper",	ODBX_OR_A2000CLASS,	0},
      {-1},
#line 218 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str162,			"AcDbBlockXYParameterEntity",		ODBX,	1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 240 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str169,				"AcDbBlockPolarGrip",			ODBX,	0},
#line 129 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str170,	"AcDbSectionViewStyle",	ODBX,	0},
      {-1}, {-1}, {-1},
#line 277 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str174,	"AcDbAngularDimensionObjectContextData",ODBX,	0},
      {-1},
#line 160 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str176, 		"AcDbPersSubentManager",	ODBX, 0},
#line 301 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str177,				"AcDbLSDefinition",		DGNLS, 0},
      {-1},
#line 282 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str179,	"AcDbLeaderObjectContextData",		ODBX,	0},
#line 113 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str180,"AcDbRasterVariables",	ISM,	0},
#line 99 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str181,	"AcDbVisualStyle",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1},
#line 94 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str187,	"AcDbXrecord",		ODBX,	0},
#line 77 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str188,	"AcDbArcDimension",	ODBX,	1},
      {-1}, {-1}, {-1}, {-1},
#line 153 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str193,	"AcDbShRevolve",	ODBX,	0},
#line 284 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str194,	"AcDbOrdinateDimensionObjectContextData",	ODBX,	0},
#line 247 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str195,			"AcDbBlockLinearParameter",		ODBX,	0},
      {-1},
#line 154 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str197,	"AcDbShSphere",		ODBX,	0},
#line 235 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str198,			"AcDbBlockStretchAction",	       	ODBX,	0},
#line 227 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str199,	"AcDbBlockHorizontalConstraintParameter",ODBX,	0},
#line 236 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str200,			"AcDbBlockLookupParameter",		ODBX,	0},
#line 155 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str201,	"AcDbShSweep",		ODBX,	0},
      {-1},
#line 98 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str203,	"AcDbIdBuffer",		ODBX,	0},
      {-1}, {-1}, {-1},
#line 125 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str207,	"AcDbMLeaderStyle",	SPECIAL, 0},
#line 107 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str208, 	"AcDbDwfDefinition",   	ODBX,	0},
#line 304 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str209,			"AcDbLSCompoundComponent",	DGNLS, 0},
      {-1}, {-1},
#line 266 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str212,   			"AcDbRenderEnvironment",		SCENEOE,	0},
      {-1},
#line 177 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str214,	"AcDbAssocArrayActionBody",		ODBX,	0},
      {-1},
#line 268 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str216, 	  			"AcDbRenderEntry",			SCENEOE,	0},
#line 299 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str217,			"OdDbPartialViewingFilter",	SPECIAL, 0},
      {-1}, {-1}, {-1}, {-1},
#line 241 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str222,			"AcDbBlockRotationGrip",		ODBX,	0},
      {-1}, {-1},
#line 74 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str225,	"AcDbPolyline",		ODBX,	1},
      {-1}, {-1}, {-1},
#line 201 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str229,		"AcDbAssocFaceActionParam",	ODBX,	0},
      {-1},
#line 243 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str231,				"AcDbBlockXYGrip",			ODBX,	0},
      {-1},
#line 110 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str233,		"AcDbRasterImage",	ISM,	1},
      {-1}, {-1}, {-1}, {-1}, {-1},
#line 297 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str239,			"ExAcXREFPanelObject",		SPECIAL, 0},
#line 161 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str240,		"AcDbAssocNetwork",		ODBX,	0},
      {-1}, {-1},
#line 142 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str243,	"AcDbShBoolean",	ODBX,	0},
      {-1}, {-1},
#line 156 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str246,	"AcDbShTorus",		ODBX,	0},
#line 192 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str247,	"AcDbAssocRestoreEntityStateActionBody",ODBX,	0},
#line 234 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str248,		"AcDbBlockPolarStretchAction",	       	ODBX,	0},
      {-1},
#line 165 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str250,	"AcDbAssocVertexActionParam",	ODBX,	0},
#line 190 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str251,	"AcDbAssocPlaneSurfaceActionBody",	ODBX,	0},
#line 238 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str252,			"AcDbBlockRotationParameter",		ODBX,	0},
      {-1}, {-1}, {-1}, {-1},
#line 188 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str257,	"AcDbAssocNetworkSurfaceActionBody",	ODBX,	0},
#line 102 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str258,	"AcDbTableGeometry",	ODBX,	0},
      {-1},
#line 139 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str260,	"AcDbRevolvedSurface",	ODBX,	1},
#line 233 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str261,			"AcDbBlockScaleAction",			ODBX,	0},
#line 96 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str262,	"AcDbSpatialIndex",	ODBX,	0},
#line 302 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str263,	       	"AcDbLSStrokePatternComponent",	DGNLS, 0},
#line 193 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str264,	"AcDbAssocRevolvedSurfaceActionBody",	ODBX,	0},
#line 209 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str265,	"AcDbAssocArrayRectangularParameters",	ODBX,	0},
#line 198 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str266,		"AcDbAssocAsmbodyActionParam",	ODBX,	0},
#line 287 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str267,	"AcDbTextObjectContextData",		ODBX,	0},
#line 147 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str268,	"AcDbShCylinder",	ODBX,	0},
#line 167 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str269,		"AcDbAssocDependency",		ODBX,	0},
      {-1},
#line 183 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str271,	"AcDbAssocBlendSurfaceActionBody",	ODBX,	0},
      {-1},
#line 108 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str273, 	"AcDbPdfDefinition",   	ODBX,	0},
      {-1}, {-1},
#line 82 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str276, 	"AcDbDwfReference",   	ODBX,	1},
      {-1},
#line 223 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str278,	"AcDbBlockDiametricConstraintParameter",ODBX,	0},
      {-1},
#line 217 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str280,			"AcDbBlockPolarGripEntity",		ODBX,	1},
#line 93 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str281,	"AcDbVbaProject",	ODBX,	0},
#line 189 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str282,	"AcDbAssocOffsetSurfaceActionBody",	ODBX,	0},
#line 215 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str283,			"AcDbBlockPointParameterEntity",      	ODBX,	1},
      {-1}, {-1},
#line 152 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str286,	"AcDbShPyramid",	ODBX,	0},
      {-1},
#line 146 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str288,	"AcDbShCone",		ODBX,	0},
#line 84 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str289,		"AcDbDictionaryWithDefault",	ODBX_OR_A2000CLASS,	0},
      {-1},
#line 300 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str291,			"OdDbPartialViewingIndex",	SPECIAL, 0},
#line 306 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str292,			"AcDbLSSymbolComponent",	DGNLS, 0},
      {-1}, {-1}, {-1},
#line 276 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str296,	"AcDbAlignedDimensionObjectContextData",ODBX,	0},
#line 231 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str297,			"AcDbBlockLookupAction",		ODBX,	0},
#line 229 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str298,			"AcDbBlockMoveAction",			ODBX,	0},
      {-1}, {-1},
#line 214 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str301,			"AcDbBlockLinearParameterEntity",      	ODBX,	1},
      {-1},
#line 85 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str303,		"AcDbPlaceHolder",		ODBX_OR_A2000CLASS,	0},
#line 279 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str304,	"AcDbBlkrefObjectContextData",		ODBX,	0},
#line 186 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str305,	"AcDbAssocFilletSurfaceActionBody",	ODBX,	0},
      {-1},
#line 245 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str307,			"AcDbBlockAlignmentGrip",		ODBX,	0},
      {-1},
#line 237 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str309,			"AcDbBlockPointParameter",		ODBX,	0},
      {-1},
#line 157 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str311,	"AcDbShWedge",		ODBX,	0},
#line 187 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str312,	"AcDbAssocLoftedSurfaceActionBody",	ODBX,	0},
#line 273 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str313,	   	"AcDbGroundPlaneBackground",		SCENEOE,	0},
#line 180 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str314,	"AcDbAssocMLeaderActionBody",		ODBX,	0},
      {-1},
#line 307 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str316,		"AecDbRefEditStatusTracker",    AEC60, 0},
      {-1},
#line 278 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str318,	"AcDbAnnotScaleObjectContextData",	ODBX,	0},
      {-1},
#line 203 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str320,		"AcDbAssocObjectActionParam",	ODBX,	0},
#line 221 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str321,			"AcDbBlockArrayAction",     		ODBX,	0},
#line 199 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str322,		"AcDbAssocCompoundActionParam",	ODBX,	0},
#line 106 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str323, 	"AcDbDgnDefinition",   	ODBX,	0},
      {-1}, {-1},
#line 281 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str326,	"AcDbFcfObjectContextData",		ODBX,	0},
      {-1}, {-1},
#line 257 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str329,   		"AcDbBlockPropertiesTableGrip",		ODBX,	0},
      {-1}, {-1}, {-1},
#line 151 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str333,	"AcDbShLoft",		ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1},
#line 271 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str339,	   		"AcDbImageBackground",			SCENEOE,	0},
      {-1},
#line 80 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str341, 	"AcDbPdfReference",   	ODBX,	1},
      {-1},
#line 250 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str343,				"AcDbBlockFlipGrip",			ODBX,	0},
#line 239 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str344,			"AcDbBlockPolarParameter",		ODBX,	0},
#line 222 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str345,	"AcDbBlockAngularConstraintParameter",  ODBX,	0},
      {-1}, {-1},
#line 248 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str348,		"AcDbBlockBasepointParameter",		ODBX,	0},
#line 220 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str349,		"AcDbBlockRepresentationData",     	ODBX,	0},
#line 226 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str350,		"AcDbBlockLinearConstraintParameter",	ODBX,	0},
#line 128 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str351,		"AcDbPlotSettings",	ODBX,	0},
      {-1}, {-1}, {-1},
#line 265 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str355,   		"AcDbRapidRTRenderSettings",		ODBX,		0},
#line 122 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str356,"AcDbWipeoutVariables",	ODBX,	0},
      {-1},
#line 216 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str358,		"AcDbBlockRotationParameterEntity",     ODBX,	1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 246 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str367,		"AcDbBlockAlignmentParameter",		ODBX,	0},
#line 89 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str368,	"AcDbDictionaryVar",	A2000CLASS,	0},
#line 124 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str369, "AcDbArcAlignedText",	EXPRESS, 1},
      {-1}, {-1},
#line 211 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str372,		"AcDbBlockAlignmentParameterEntity",	ODBX,	1},
      {-1},
#line 143 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str374,		"AcDbShBox",		ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1},
#line 286 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str386,	"AcDbRadialDimensionObjectContextData",	ODBX,	0},
#line 232 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str387,			"AcDbBlockRotateAction",		ODBX,	0},
      {-1}, {-1},
#line 224 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str390,		"AcDbBlockRadialConstraintParameter",	ODBX,	0},
#line 81 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str391, 	"AcDbDgnReference",   	ODBX,	1},
      {-1},
#line 136 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str393,		"AcDbLoftedSurface",	ODBX,	1},
      {-1},
#line 103 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str395,	"AcDbCellStyleMap",	ODBX,	0},
      {-1},
#line 228 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str397,	"AcDbBlockVerticalConstraintParameter",	ODBX,	0},
      {-1}, {-1}, {-1},
#line 111 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str401,	"AcDbRasterImageDef",	ISM,	0},
#line 181 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str402,		"AcDbContextDataManager",		ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1},
#line 168 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str415,	"AcDbAssocDimDependencyBody",	ODBX,	0},
      {-1},
#line 264 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str417,   		"AcDbMentalRayRenderSettings",		SCENEOE,	0},
      {-1}, {-1}, {-1}, {-1}, {-1},
#line 262 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str423,   			"AcDbMotionPath",			SCENEOE,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 144 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str458,	"AcDbShBrep",		ODBX,	0},
      {-1}, {-1},
#line 280 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str461,	"AcDbDiametricDimensionObjectContextData",ODBX,	0},
      {-1}, {-1}, {-1},
#line 254 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str465,			"AcDbBlockXYParameter",			ODBX,	0},
      {-1},
#line 166 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str467,	"AcDbAssocGeomDependency",	ODBX,	0},
#line 305 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str468,			"AcDbLSPointComponent",		DGNLS, 0},
      {-1},
#line 91 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str470,	"AcDbSortentsTable",	A2000CLASS,	0},
      {-1},
#line 242 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str472,			"AcDbBlockVisibilityGrip",		ODBX,	0},
      {-1},
#line 274 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str474,	   		"AcDbGradientBackground",		SCENEOE,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 135 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str500,	"AcDbExtrudedSurface",	ODBX,	1},
      {-1},
#line 255 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str502,		"AcDbBlockVisibilityParameter",		ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1},
#line 244 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str516,		"AcDbBlockGripExpr",			ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 225 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str524,	"AcDbBlockAlignedConstraintParameter",  ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1},
#line 267 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str530,   			"AcDbRenderGlobal",			SCENEOE,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1},
#line 285 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str572,	"AcDbRadialDimensionLargeObjectContextData",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 269 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str581,   		"AcDbIBLBackground",			SCENEOE,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1},
#line 270 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str629,	   		"AcDbSkyBackground",			SCENEOE,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 283 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str647,	"AcDbMLeaderObjectContextData",		ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 112 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str674,"AcDbRasterImageDefReactor",	ISM,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1},
#line 258 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str689,		"AcDbDynamicBlockProxyNode",	       	ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1},
#line 259 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str817,"AcDbDynamicBlockPurgePreventer",      	ODBX,	0}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          int o = wordlist[key].name;
          if (o >= 0)
            {
              const char *s = o + stringpool;

              if (*str == *s && !strcmp (str + 1, s + 1))
                return &wordlist[key];
            }
        }
    }
  return 0;
}
#line 308 "src/dxfclasses.in"


/* Create classes on demand.
   Returns found or new klass->number id (always >=500), <0 on error.
   -1 out of memory (from dwg_add_class())
   -2 unknown class for dxfname.
   -3 invalid apptype for class. (should not happen)
*/
EXPORT int
dwg_require_class (Dwg_Data *restrict dwg, const char *const restrict dxfname, const size_t len)
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
        case DGNLSCLASS:
          return dwg_add_class (dwg, dxfname, result->cppname, "AcDgnLS", result->isent);
        case AEC60CLASS:
          return dwg_add_class (dwg, dxfname, result->cppname, "AecArchBase60|Product Desc: "
                                "Autodesk Architectural Desktop 2007", result->isent);
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
            else if (memBEGINc (dxfname, "PARTIAL_VIEWING_"))
              strcpy (appname, "OdDbPartialViewing|https://www.opendesign.com Teigha(R) Core Db");
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
