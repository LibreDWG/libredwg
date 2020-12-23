/* ANSI-C code produced by gperf version 3.1 */
/* Command-line: gperf --output-file src/dxfclasses.tmp.c src/dxfclasses.in  */
/* Computed positions: -k'4,6,10,12,21' */

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

#define TOTAL_KEYWORDS 197
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 39
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 649
/* maximum key range = 647, duplicates = 0 */

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
      650, 650, 650, 650, 650, 650, 650, 650, 650, 650,
      650, 650, 650, 650, 650, 650, 650, 650, 650, 650,
      650, 650, 650, 650, 650, 650, 650, 650, 650, 650,
      650, 650, 650, 650, 650, 650, 650, 650, 650, 650,
      650, 650, 650, 650, 650, 650, 650, 650, 650, 650,
        0,  10, 650, 650, 650, 650, 650, 650, 650, 650,
      650, 650, 650, 650, 650,   0,   0,   5,  60,  15,
       40, 245,  95, 175, 120,  20,  80, 195, 170, 175,
       10,  20,  45,   5,  35,   5,  30,  55,  40,  30,
       10, 650, 650, 650, 650, 185,   0, 650, 650, 650,
      650, 650, 650, 650, 650, 650, 650, 650, 650, 650,
      650, 650, 650, 650, 650, 650, 650, 650, 650, 650,
      650, 650, 650, 650, 650, 650, 650, 650, 650
    };
  register unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[20]];
#if defined __cplusplus && (__cplusplus >= 201703L || (__cplusplus >= 201103L && defined __clang_major__ && defined __clang_minor__ && __clang_major__ + (__clang_minor__ >= 9) > 3))
      [[fallthrough]];
#elif defined __GNUC__ && __GNUC__ >= 7
      __attribute__ ((__fallthrough__));
#endif
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
        hval += asso_values[(unsigned char)str[3]];
#if defined __cplusplus && (__cplusplus >= 201703L || (__cplusplus >= 201103L && defined __clang_major__ && defined __clang_minor__ && __clang_major__ + (__clang_minor__ >= 9) > 3))
      [[fallthrough]];
#elif defined __GNUC__ && __GNUC__ >= 7
      __attribute__ ((__fallthrough__));
#endif
      /*FALLTHROUGH*/
      case 3:
        break;
    }
  return hval;
}

struct stringpool_t
  {
    char stringpool_str3[sizeof("SUN")];
    char stringpool_str9[sizeof("DATATABLE")];
    char stringpool_str10[sizeof("HATCH")];
    char stringpool_str13[sizeof("DIMASSOC")];
    char stringpool_str18[sizeof("SUNSTUDY")];
    char stringpool_str21[sizeof("NURBSURFACE")];
    char stringpool_str25[sizeof("ACDBASSOCACTION")];
    char stringpool_str27[sizeof("WIPEOUT")];
    char stringpool_str30[sizeof("ACDBASSOCACTIONPARAM")];
    char stringpool_str33[sizeof("ACDBCURVEPATH")];
    char stringpool_str34[sizeof("ACDBASSOCARRAYACTIONBODY")];
    char stringpool_str39[sizeof("ACDBASSOCARRAYPOLARPARAMETERS")];
    char stringpool_str40[sizeof("ACDBASSOCARRAYMODIFYACTIONBODY")];
    char stringpool_str43[sizeof("BLOCKUSERPARAMETER")];
    char stringpool_str44[sizeof("ACDBASSOCPATHACTIONPARAM")];
    char stringpool_str45[sizeof("RTEXT")];
    char stringpool_str46[sizeof("ACDBASSOCPERSSUBENTMANAGER")];
    char stringpool_str50[sizeof("ACDBASSOCARRAYMODIFYPARAMETERS")];
    char stringpool_str52[sizeof("SURFACE")];
    char stringpool_str53[sizeof("IDBUFFER")];
    char stringpool_str54[sizeof("OLE2FRAME")];
    char stringpool_str55[sizeof("BLOCKPROPERTIESTABLE")];
    char stringpool_str57[sizeof("XRECORD")];
    char stringpool_str60[sizeof("ACDBASSOC3POINTANGULARDIMACTIONBODY")];
    char stringpool_str61[sizeof("ACDBASSOCPLANESURFACEACTIONBODY")];
    char stringpool_str62[sizeof("ACDBASSOCEXTENDSURFACEACTIONBODY")];
    char stringpool_str65[sizeof("BLOCKFLIPACTION")];
    char stringpool_str66[sizeof("ACDBASSOCPATCHSURFACEACTIONBODY")];
    char stringpool_str67[sizeof("SWEPTSURFACE")];
    char stringpool_str69[sizeof("BLOCKPOINTPARAMETER")];
    char stringpool_str72[sizeof("XYPARAMETERENTITY")];
    char stringpool_str78[sizeof("BLOCKFLIPPARAMETER")];
    char stringpool_str79[sizeof("BLOCKPOLARGRIP")];
    char stringpool_str81[sizeof("WIPEOUTVARIABLES")];
    char stringpool_str83[sizeof("ACDBASSOCARRAYPATHPARAMETERS")];
    char stringpool_str84[sizeof("FLIPPARAMETERENTITY")];
    char stringpool_str85[sizeof("SCALE")];
    char stringpool_str89[sizeof("ACDBASSOCROTATEDDIMACTIONBODY")];
    char stringpool_str91[sizeof("ACDBASSOCBLENDSURFACEACTIONBODY")];
    char stringpool_str94[sizeof("ACDB_BLKREFOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str96[sizeof("ACDBASSOCSWEPTSURFACEACTIONBODY")];
    char stringpool_str97[sizeof("MLEADERSTYLE")];
    char stringpool_str99[sizeof("ACDBASSOCEXTRUDEDSURFACEACTIONBODY")];
    char stringpool_str100[sizeof("LIGHT")];
    char stringpool_str101[sizeof("BLOCKSCALEACTION")];
    char stringpool_str102[sizeof("BLOCKROTATEACTION")];
    char stringpool_str104[sizeof("ACDBASSOCDEPENDENCY")];
    char stringpool_str107[sizeof("BLOCKLOOKUPACTION")];
    char stringpool_str109[sizeof("EXACXREFPANELOBJECT")];
    char stringpool_str110[sizeof("TABLESTYLE")];
    char stringpool_str111[sizeof("BLOCKARRAYACTION")];
    char stringpool_str112[sizeof("BLOCKROTATIONGRIP")];
    char stringpool_str114[sizeof("ACSH_BOX_CLASS")];
    char stringpool_str117[sizeof("TABLECONTENT")];
    char stringpool_str118[sizeof("BLOCKDIAMETRICCONSTRAINTPARAMETER")];
    char stringpool_str120[sizeof("ACAD_TABLE")];
    char stringpool_str124[sizeof("ACDB_BLOCKREPRESENTATION_DATA")];
    char stringpool_str125[sizeof("BLOCKLINEARPARAMETER")];
    char stringpool_str126[sizeof("ACDBPERSSUBENTMANAGER")];
    char stringpool_str129[sizeof("ACDBASSOCFACEACTIONPARAM")];
    char stringpool_str130[sizeof("BLOCKLOOKUPPARAMETER")];
    char stringpool_str132[sizeof("BLOCKROTATIONPARAMETER")];
    char stringpool_str133[sizeof("GROUND_PLANE_BACKGROUND")];
    char stringpool_str135[sizeof("ACAD_PROXY_OBJECT_WRAPPER")];
    char stringpool_str138[sizeof("ACDBBLOCKPARAMDEPENDENCYBODY")];
    char stringpool_str139[sizeof("ACDBASSOCEDGEACTIONPARAM")];
    char stringpool_str141[sizeof("MULTILEADER")];
    char stringpool_str143[sizeof("NPOCOLLECTION")];
    char stringpool_str145[sizeof("ACDBASSOCEDGECHAMFERACTIONBODY")];
    char stringpool_str146[sizeof("ACDB_FCFOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str149[sizeof("ACDBASSOCEDGEFILLETACTIONBODY")];
    char stringpool_str151[sizeof("LINEARPARAMETERENTITY")];
    char stringpool_str154[sizeof("GRADIENT_BACKGROUND")];
    char stringpool_str157[sizeof("ACDBASSOCRESTOREENTITYSTATEACTIONBODY")];
    char stringpool_str159[sizeof("ACDBASSOCALIGNEDDIMACTIONBODY")];
    char stringpool_str161[sizeof("RENDERENTRY")];
    char stringpool_str162[sizeof("ACSH_SPHERE_CLASS")];
    char stringpool_str163[sizeof("ACSH_CHAMFER_CLASS")];
    char stringpool_str165[sizeof("ACAD_PROXY_ENTITY_WRAPPER")];
    char stringpool_str167[sizeof("ACSH_FILLET_CLASS")];
    char stringpool_str168[sizeof("ACDB_ANNOTSCALEOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str174[sizeof("ACDB_LEADEROBJECTCONTEXTDATA_CLASS")];
    char stringpool_str175[sizeof("BLOCKRADIALCONSTRAINTPARAMETER")];
    char stringpool_str176[sizeof("VISUALSTYLE")];
    char stringpool_str179[sizeof("ACDBASSOCREVOLVEDSURFACEACTIONBODY")];
    char stringpool_str184[sizeof("LIGHTLIST")];
    char stringpool_str186[sizeof("ACSH_SWEEP_CLASS")];
    char stringpool_str188[sizeof("ACDBASSOCALIGNEDIMACTIONBODY")];
    char stringpool_str190[sizeof("ACDBASSOCTRIMSURFACEACTIONBODY")];
    char stringpool_str193[sizeof("BLOCKBASEPOINTPARAMETER")];
    char stringpool_str195[sizeof("BLOCKLINEARGRIP")];
    char stringpool_str196[sizeof("ACDBASSOCNETWORK")];
    char stringpool_str198[sizeof("MATERIAL")];
    char stringpool_str200[sizeof("BLOCKLOOKUPGRIP")];
    char stringpool_str201[sizeof("ACAD_EVALUATION_GRAPH")];
    char stringpool_str203[sizeof("ACDBPOINTPATH")];
    char stringpool_str204[sizeof("ACDBPOINTCLOUD")];
    char stringpool_str206[sizeof("ACDBPOINTCLOUDEX")];
    char stringpool_str207[sizeof("ACDBPOINTCLOUDDEF")];
    char stringpool_str208[sizeof("LOFTEDSURFACE")];
    char stringpool_str209[sizeof("ACDBPOINTCLOUDDEFEX")];
    char stringpool_str210[sizeof("BLOCKLINEARCONSTRAINTPARAMETER")];
    char stringpool_str211[sizeof("ACSH_TORUS_CLASS")];
    char stringpool_str212[sizeof("ACDBPOINTCLOUDCOLORMAP")];
    char stringpool_str213[sizeof("ACDBASSOCNETWORKSURFACEACTIONBODY")];
    char stringpool_str215[sizeof("ACDBPOINTCLOUDDEF_REACTOR")];
    char stringpool_str216[sizeof("LAYOUT")];
    char stringpool_str218[sizeof("ACDBPOINTCLOUDDEF_REACTOR_EX")];
    char stringpool_str220[sizeof("BLOCKMOVEACTION")];
    char stringpool_str225[sizeof("ACDBASSOCORDINATEDIMACTIONBODY")];
    char stringpool_str226[sizeof("LAYER_INDEX")];
    char stringpool_str227[sizeof("PLANESURFACE")];
    char stringpool_str229[sizeof("SPATIAL_FILTER")];
    char stringpool_str230[sizeof("LWPOLYLINE")];
    char stringpool_str231[sizeof("BLOCKXYGRIP")];
    char stringpool_str233[sizeof("BLOCKALIGNMENTGRIP")];
    char stringpool_str234[sizeof("ACDBDETAILVIEWSTYLE")];
    char stringpool_str236[sizeof("ACDBASSOCVERTEXACTIONPARAM")];
    char stringpool_str238[sizeof("BLOCKSTRETCHACTION")];
    char stringpool_str241[sizeof("ACSH_WEDGE_CLASS")];
    char stringpool_str246[sizeof("VBA_PROJECT")];
    char stringpool_str248[sizeof("ACDB_ALDIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str249[sizeof("BLOCKVISIBILITYGRIP")];
    char stringpool_str250[sizeof("IMAGE")];
    char stringpool_str253[sizeof("DICTIONARYVAR")];
    char stringpool_str254[sizeof("ACDB_ANGDIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str255[sizeof("ACSH_EXTRUSION_CLASS")];
    char stringpool_str258[sizeof("SPATIAL_INDEX")];
    char stringpool_str263[sizeof("BLOCKPOLARSTRETCHACTION")];
    char stringpool_str264[sizeof("BASEPOINTPARAMETERENTITY")];
    char stringpool_str265[sizeof("ACDBSECTIONVIEWSTYLE")];
    char stringpool_str266[sizeof("ACDBASSOCDIMDEPENDENCYBODY")];
    char stringpool_str267[sizeof("PLOTSETTINGS")];
    char stringpool_str269[sizeof("BLOCKVISIBILITYPARAMETER")];
    char stringpool_str273[sizeof("BLOCKALIGNMENTPARAMETER")];
    char stringpool_str275[sizeof("RASTERVARIABLES")];
    char stringpool_str276[sizeof("BLOCKXYPARAMETER")];
    char stringpool_str277[sizeof("ACDBASSOCFILLETSURFACEACTIONBODY")];
    char stringpool_str278[sizeof("ACSH_REVOLVE_CLASS")];
    char stringpool_str280[sizeof("POLARGRIPENTITY")];
    char stringpool_str289[sizeof("ARCALIGNEDTEXT")];
    char stringpool_str290[sizeof("ACDBASSOCARRAYRECTANGULARPARAMETERS")];
    char stringpool_str291[sizeof("BLOCKANGULARCONSTRAINTPARAMETER")];
    char stringpool_str293[sizeof("ACDB_RADIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str298[sizeof("CONTEXTDATAMANAGER")];
    char stringpool_str302[sizeof("ACDB_TEXTOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str303[sizeof("ACSH_BOOLEAN_CLASS")];
    char stringpool_str304[sizeof("BLOCKPROPERTIESTABLEGRIP")];
    char stringpool_str307[sizeof("RENDERENVIRONMENT")];
    char stringpool_str308[sizeof("ACDB_DMDIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str312[sizeof("RENDERGLOBAL")];
    char stringpool_str313[sizeof("IMAGEDEF")];
    char stringpool_str329[sizeof("ACSH_CYLINDER_CLASS")];
    char stringpool_str333[sizeof("ACSH_PYRAMID_CLASS")];
    char stringpool_str338[sizeof("ACDBASSOCPOINTREFACTIONPARAM")];
    char stringpool_str342[sizeof("CELLSTYLEMAP")];
    char stringpool_str358[sizeof("TABLEGEOMETRY")];
    char stringpool_str359[sizeof("ACDBMOTIONPATH")];
    char stringpool_str364[sizeof("BLOCKHORIZONTALCONSTRAINTPARAMETER")];
    char stringpool_str366[sizeof("IMAGEDEF_REACTOR")];
    char stringpool_str367[sizeof("ACDBASSOCLOFTEDSURFACEACTIONBODY")];
    char stringpool_str369[sizeof("ACDBDICTIONARYWDFLT")];
    char stringpool_str370[sizeof("EXTRUDEDSURFACE")];
    char stringpool_str373[sizeof("ROTATIONPARAMETERENTITY")];
    char stringpool_str377[sizeof("ACDBASSOCASMBODYACTIONPARAM")];
    char stringpool_str381[sizeof("ACDB_DYNAMICBLOCKPROXYNODE")];
    char stringpool_str383[sizeof("ACDBASSOCCOMPOUNDACTIONPARAM")];
    char stringpool_str385[sizeof("ACDBPLACEHOLDER")];
    char stringpool_str388[sizeof("ARC_DIMENSION")];
    char stringpool_str391[sizeof("BLOCKALIGNEDCONSTRAINTPARAMETER")];
    char stringpool_str393[sizeof("ACSH_HISTORY_CLASS")];
    char stringpool_str396[sizeof("ACDBASSOCOBJECTACTIONPARAM")];
    char stringpool_str400[sizeof("POINTPARAMETERENTITY")];
    char stringpool_str401[sizeof("SOLID_BACKGROUND")];
    char stringpool_str403[sizeof("ACDBASSOCOSNAPPOINTREFACTIONPARAM")];
    char stringpool_str404[sizeof("SKYLIGHT_BACKGROUND")];
    char stringpool_str413[sizeof("SORTENTSTABLE")];
    char stringpool_str417[sizeof("BLOCKVERTICALCONSTRAINTPARAMETER")];
    char stringpool_str423[sizeof("BLOCKFLIPGRIP")];
    char stringpool_str426[sizeof("BLOCKGRIPLOCATIONCOMPONENT")];
    char stringpool_str429[sizeof("ACDB_ORDDIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str441[sizeof("ACDBASSOCMLEADERACTIONBODY")];
    char stringpool_str444[sizeof("POSITIONMARKER")];
    char stringpool_str453[sizeof("ACDBASSOCGEOMDEPENDENCY")];
    char stringpool_str456[sizeof("RAPIDRTRENDERSETTINGS")];
    char stringpool_str462[sizeof("ACDBASSOCOFFSETSURFACEACTIONBODY")];
    char stringpool_str465[sizeof("ACDB_MLEADEROBJECTCONTEXTDATA_CLASS")];
    char stringpool_str470[sizeof("REVOLVEDSURFACE")];
    char stringpool_str471[sizeof("IMAGE_BACKGROUND")];
    char stringpool_str490[sizeof("ACSH_BREP_CLASS")];
    char stringpool_str495[sizeof("ACSH_CONE_CLASS")];
    char stringpool_str514[sizeof("ALIGNMENTPARAMETERENTITY")];
    char stringpool_str528[sizeof("MENTALRAYRENDERSETTINGS")];
    char stringpool_str540[sizeof("ACDB_RADIMLGOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str570[sizeof("ACSH_LOFT_CLASS")];
    char stringpool_str599[sizeof("ACDB_DYNAMICBLOCKPURGEPREVENTER_VERSION")];
    char stringpool_str649[sizeof("RAPIDRTRENDERENVIRONMENT")];
  };
static const struct stringpool_t stringpool_contents =
  {
    "SUN",
    "DATATABLE",
    "HATCH",
    "DIMASSOC",
    "SUNSTUDY",
    "NURBSURFACE",
    "ACDBASSOCACTION",
    "WIPEOUT",
    "ACDBASSOCACTIONPARAM",
    "ACDBCURVEPATH",
    "ACDBASSOCARRAYACTIONBODY",
    "ACDBASSOCARRAYPOLARPARAMETERS",
    "ACDBASSOCARRAYMODIFYACTIONBODY",
    "BLOCKUSERPARAMETER",
    "ACDBASSOCPATHACTIONPARAM",
    "RTEXT",
    "ACDBASSOCPERSSUBENTMANAGER",
    "ACDBASSOCARRAYMODIFYPARAMETERS",
    "SURFACE",
    "IDBUFFER",
    "OLE2FRAME",
    "BLOCKPROPERTIESTABLE",
    "XRECORD",
    "ACDBASSOC3POINTANGULARDIMACTIONBODY",
    "ACDBASSOCPLANESURFACEACTIONBODY",
    "ACDBASSOCEXTENDSURFACEACTIONBODY",
    "BLOCKFLIPACTION",
    "ACDBASSOCPATCHSURFACEACTIONBODY",
    "SWEPTSURFACE",
    "BLOCKPOINTPARAMETER",
    "XYPARAMETERENTITY",
    "BLOCKFLIPPARAMETER",
    "BLOCKPOLARGRIP",
    "WIPEOUTVARIABLES",
    "ACDBASSOCARRAYPATHPARAMETERS",
    "FLIPPARAMETERENTITY",
    "SCALE",
    "ACDBASSOCROTATEDDIMACTIONBODY",
    "ACDBASSOCBLENDSURFACEACTIONBODY",
    "ACDB_BLKREFOBJECTCONTEXTDATA_CLASS",
    "ACDBASSOCSWEPTSURFACEACTIONBODY",
    "MLEADERSTYLE",
    "ACDBASSOCEXTRUDEDSURFACEACTIONBODY",
    "LIGHT",
    "BLOCKSCALEACTION",
    "BLOCKROTATEACTION",
    "ACDBASSOCDEPENDENCY",
    "BLOCKLOOKUPACTION",
    "EXACXREFPANELOBJECT",
    "TABLESTYLE",
    "BLOCKARRAYACTION",
    "BLOCKROTATIONGRIP",
    "ACSH_BOX_CLASS",
    "TABLECONTENT",
    "BLOCKDIAMETRICCONSTRAINTPARAMETER",
    "ACAD_TABLE",
    "ACDB_BLOCKREPRESENTATION_DATA",
    "BLOCKLINEARPARAMETER",
    "ACDBPERSSUBENTMANAGER",
    "ACDBASSOCFACEACTIONPARAM",
    "BLOCKLOOKUPPARAMETER",
    "BLOCKROTATIONPARAMETER",
    "GROUND_PLANE_BACKGROUND",
    "ACAD_PROXY_OBJECT_WRAPPER",
    "ACDBBLOCKPARAMDEPENDENCYBODY",
    "ACDBASSOCEDGEACTIONPARAM",
    "MULTILEADER",
    "NPOCOLLECTION",
    "ACDBASSOCEDGECHAMFERACTIONBODY",
    "ACDB_FCFOBJECTCONTEXTDATA_CLASS",
    "ACDBASSOCEDGEFILLETACTIONBODY",
    "LINEARPARAMETERENTITY",
    "GRADIENT_BACKGROUND",
    "ACDBASSOCRESTOREENTITYSTATEACTIONBODY",
    "ACDBASSOCALIGNEDDIMACTIONBODY",
    "RENDERENTRY",
    "ACSH_SPHERE_CLASS",
    "ACSH_CHAMFER_CLASS",
    "ACAD_PROXY_ENTITY_WRAPPER",
    "ACSH_FILLET_CLASS",
    "ACDB_ANNOTSCALEOBJECTCONTEXTDATA_CLASS",
    "ACDB_LEADEROBJECTCONTEXTDATA_CLASS",
    "BLOCKRADIALCONSTRAINTPARAMETER",
    "VISUALSTYLE",
    "ACDBASSOCREVOLVEDSURFACEACTIONBODY",
    "LIGHTLIST",
    "ACSH_SWEEP_CLASS",
    "ACDBASSOCALIGNEDIMACTIONBODY",
    "ACDBASSOCTRIMSURFACEACTIONBODY",
    "BLOCKBASEPOINTPARAMETER",
    "BLOCKLINEARGRIP",
    "ACDBASSOCNETWORK",
    "MATERIAL",
    "BLOCKLOOKUPGRIP",
    "ACAD_EVALUATION_GRAPH",
    "ACDBPOINTPATH",
    "ACDBPOINTCLOUD",
    "ACDBPOINTCLOUDEX",
    "ACDBPOINTCLOUDDEF",
    "LOFTEDSURFACE",
    "ACDBPOINTCLOUDDEFEX",
    "BLOCKLINEARCONSTRAINTPARAMETER",
    "ACSH_TORUS_CLASS",
    "ACDBPOINTCLOUDCOLORMAP",
    "ACDBASSOCNETWORKSURFACEACTIONBODY",
    "ACDBPOINTCLOUDDEF_REACTOR",
    "LAYOUT",
    "ACDBPOINTCLOUDDEF_REACTOR_EX",
    "BLOCKMOVEACTION",
    "ACDBASSOCORDINATEDIMACTIONBODY",
    "LAYER_INDEX",
    "PLANESURFACE",
    "SPATIAL_FILTER",
    "LWPOLYLINE",
    "BLOCKXYGRIP",
    "BLOCKALIGNMENTGRIP",
    "ACDBDETAILVIEWSTYLE",
    "ACDBASSOCVERTEXACTIONPARAM",
    "BLOCKSTRETCHACTION",
    "ACSH_WEDGE_CLASS",
    "VBA_PROJECT",
    "ACDB_ALDIMOBJECTCONTEXTDATA_CLASS",
    "BLOCKVISIBILITYGRIP",
    "IMAGE",
    "DICTIONARYVAR",
    "ACDB_ANGDIMOBJECTCONTEXTDATA_CLASS",
    "ACSH_EXTRUSION_CLASS",
    "SPATIAL_INDEX",
    "BLOCKPOLARSTRETCHACTION",
    "BASEPOINTPARAMETERENTITY",
    "ACDBSECTIONVIEWSTYLE",
    "ACDBASSOCDIMDEPENDENCYBODY",
    "PLOTSETTINGS",
    "BLOCKVISIBILITYPARAMETER",
    "BLOCKALIGNMENTPARAMETER",
    "RASTERVARIABLES",
    "BLOCKXYPARAMETER",
    "ACDBASSOCFILLETSURFACEACTIONBODY",
    "ACSH_REVOLVE_CLASS",
    "POLARGRIPENTITY",
    "ARCALIGNEDTEXT",
    "ACDBASSOCARRAYRECTANGULARPARAMETERS",
    "BLOCKANGULARCONSTRAINTPARAMETER",
    "ACDB_RADIMOBJECTCONTEXTDATA_CLASS",
    "CONTEXTDATAMANAGER",
    "ACDB_TEXTOBJECTCONTEXTDATA_CLASS",
    "ACSH_BOOLEAN_CLASS",
    "BLOCKPROPERTIESTABLEGRIP",
    "RENDERENVIRONMENT",
    "ACDB_DMDIMOBJECTCONTEXTDATA_CLASS",
    "RENDERGLOBAL",
    "IMAGEDEF",
    "ACSH_CYLINDER_CLASS",
    "ACSH_PYRAMID_CLASS",
    "ACDBASSOCPOINTREFACTIONPARAM",
    "CELLSTYLEMAP",
    "TABLEGEOMETRY",
    "ACDBMOTIONPATH",
    "BLOCKHORIZONTALCONSTRAINTPARAMETER",
    "IMAGEDEF_REACTOR",
    "ACDBASSOCLOFTEDSURFACEACTIONBODY",
    "ACDBDICTIONARYWDFLT",
    "EXTRUDEDSURFACE",
    "ROTATIONPARAMETERENTITY",
    "ACDBASSOCASMBODYACTIONPARAM",
    "ACDB_DYNAMICBLOCKPROXYNODE",
    "ACDBASSOCCOMPOUNDACTIONPARAM",
    "ACDBPLACEHOLDER",
    "ARC_DIMENSION",
    "BLOCKALIGNEDCONSTRAINTPARAMETER",
    "ACSH_HISTORY_CLASS",
    "ACDBASSOCOBJECTACTIONPARAM",
    "POINTPARAMETERENTITY",
    "SOLID_BACKGROUND",
    "ACDBASSOCOSNAPPOINTREFACTIONPARAM",
    "SKYLIGHT_BACKGROUND",
    "SORTENTSTABLE",
    "BLOCKVERTICALCONSTRAINTPARAMETER",
    "BLOCKFLIPGRIP",
    "BLOCKGRIPLOCATIONCOMPONENT",
    "ACDB_ORDDIMOBJECTCONTEXTDATA_CLASS",
    "ACDBASSOCMLEADERACTIONBODY",
    "POSITIONMARKER",
    "ACDBASSOCGEOMDEPENDENCY",
    "RAPIDRTRENDERSETTINGS",
    "ACDBASSOCOFFSETSURFACEACTIONBODY",
    "ACDB_MLEADEROBJECTCONTEXTDATA_CLASS",
    "REVOLVEDSURFACE",
    "IMAGE_BACKGROUND",
    "ACSH_BREP_CLASS",
    "ACSH_CONE_CLASS",
    "ALIGNMENTPARAMETERENTITY",
    "MENTALRAYRENDERSETTINGS",
    "ACDB_RADIMLGOBJECTCONTEXTDATA_CLASS",
    "ACSH_LOFT_CLASS",
    "ACDB_DYNAMICBLOCKPURGEPREVENTER_VERSION",
    "RAPIDRTRENDERENVIRONMENT"
  };
#define stringpool ((const char *) &stringpool_contents)
const struct Dwg_DXFClass *
in_word_set (register const char *str, register size_t len)
{
  static const struct Dwg_DXFClass wordlist[] =
    {
      {-1}, {-1}, {-1},
#line 104 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str3, 		"AcDbSun",		SCENEOE, 0},
      {-1}, {-1}, {-1}, {-1}, {-1},
#line 94 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str9,	"AcDbDataTable",	ODBX,	0},
#line 68 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str10,		"AcDbHatch",		ODBX,	1},
      {-1}, {-1},
#line 107 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str13, 	"AcDbDimAssoc",		SPECIAL, 0},
      {-1}, {-1}, {-1}, {-1},
#line 105 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str18,	"AcDbSunStudy",		SCENEOE, 0},
      {-1}, {-1},
#line 124 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str21,		"AcDbNurbSurface",	ODBX,	1},
      {-1}, {-1}, {-1},
#line 149 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str25,		"AcDbAssocAction",		ODBX,	0},
      {-1},
#line 108 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str27,	"AcDbWipeout", 		SPECIAL, 1},
      {-1}, {-1},
#line 184 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str30,			"AcDbAssocActionParam",		ODBX,	0},
      {-1}, {-1},
#line 247 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str33,   			"AcDbCurvePath",			SCENEOE,	0},
#line 164 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str34,	"AcDbAssocArrayActionBody",		ODBX,	0},
      {-1}, {-1}, {-1}, {-1},
#line 195 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str39,		"AcDbAssocArrayPolarParameters",	ODBX,	0},
#line 163 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str40,	"AcDbAssocArrayModifyActionBody",	ODBX,	0},
      {-1}, {-1},
#line 239 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str43,   			"AcDbBlockUserParameter",		ODBX,	0},
#line 189 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str44,		"AcDbAssocPathActionParam",	ODBX,	0},
#line 110 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str45, 		"AcDbRText", 		EXPRESS, 1},
#line 146 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str46,	"AcDbAssocPersSubentManager",	ODBX,	0},
      {-1}, {-1}, {-1},
#line 193 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str50,		"AcDbAssocArrayModifyParameters",	ODBX,	0},
      {-1},
#line 121 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str52,		"AcDbSurface",		ODBX,	1},
#line 88 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str53,	"AcDbIdBuffer",		ODBX,	0},
#line 69 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str54,	"AcDbOle2Frame",	ODBX,	1},
#line 242 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str55,			"AcDbBlockPropertiesTable",		ODBX,	0},
      {-1},
#line 84 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str57,	"AcDbXrecord",		ODBX,	0},
      {-1}, {-1},
#line 158 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str60,"AcDbAssoc3PointAngularDimActionBody",	ODBX,	0},
#line 177 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str61,	"AcDbAssocPlaneSurfaceActionBody",	ODBX,	0},
#line 171 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str62,	"AcDbAssocExtendSurfaceActionBody",	ODBX,	0},
      {-1}, {-1},
#line 217 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str65,			"AcDbBlockFlipAction",			ODBX,	0},
#line 178 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str66,	"AcDbAssocPatchSurfaceActionBody",	ODBX,	0},
#line 127 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str67,		"AcDbSweptSurface",	ODBX,	1},
      {-1},
#line 224 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str69,			"AcDbBlockPointParameter",		ODBX,	0},
      {-1}, {-1},
#line 205 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str72,			"AcDbBlockXYParameterEntity",		ODBX,	1},
      {-1}, {-1}, {-1}, {-1}, {-1},
#line 235 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str78,			"AcDbBlockFlipParameter",		ODBX,	0},
#line 226 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str79,				"AcDbBlockPolarGrip",			ODBX,	0},
      {-1},
#line 109 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str81,"AcDbWipeoutVariables",	ODBX,	0},
      {-1},
#line 194 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str83,		"AcDbAssocArrayPathParameters",		ODBX,	0},
#line 200 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str84,			"AcDbBlockFlipParameterEntity",		ODBX,	1},
#line 95 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str85,		"AcDbScale",		ODBX,	0},
      {-1}, {-1}, {-1},
#line 161 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str89,	"AcDbAssocRotatedDimActionBody",	ODBX,	0},
      {-1},
#line 170 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str91,	"AcDbAssocBlendSurfaceActionBody",	ODBX,	0},
      {-1}, {-1},
#line 265 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str94,	"AcDbBlkrefObjectContextData",		ODBX,	0},
      {-1},
#line 181 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str96,	"AcDbAssocSweptSurfaceActionBody",	ODBX,	0},
#line 112 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str97,	"AcDbMLeaderStyle",	SPECIAL, 0},
      {-1},
#line 172 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str99,	"AcDbAssocExtrudedSurfaceActionBody",	ODBX,	0},
#line 102 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str100,		"AcDbLight",		SCENEOE, 1},
#line 220 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str101,			"AcDbBlockScaleAction",			ODBX,	0},
#line 219 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str102,			"AcDbBlockRotateAction",		ODBX,	0},
      {-1},
#line 154 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str104,		"AcDbAssocDependency",		ODBX,	0},
      {-1}, {-1},
#line 218 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str107,			"AcDbBlockLookupAction",		ODBX,	0},
      {-1},
#line 283 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str109,			"ExAcXREFPanelObject",		SPECIAL, 0},
#line 90 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str110,	"AcDbTableStyle",	ODBX,	0},
#line 208 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str111,			"AcDbBlockArrayAction",     		ODBX,	0},
#line 227 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str112,			"AcDbBlockRotationGrip",		ODBX,	0},
      {-1},
#line 130 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str114,		"AcDbShBox",		ODBX,	0},
      {-1}, {-1},
#line 91 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str117,	"AcDbTableContent",	ODBX,	0},
#line 210 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str118,	"AcDbBlockDiametricConstraintParameter",ODBX,	0},
      {-1},
#line 72 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str120,	"AcDbTable",		ODBX,	1},
      {-1}, {-1}, {-1},
#line 207 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str124,		"AcDbBlockRepresentationData",     	ODBX,	0},
#line 233 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str125,			"AcDbBlockLinearParameter",		ODBX,	0},
#line 147 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str126, 		"AcDbPersSubentManager",	ODBX, 0},
      {-1}, {-1},
#line 188 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str129,		"AcDbAssocFaceActionParam",	ODBX,	0},
#line 223 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str130,			"AcDbBlockLookupParameter",		ODBX,	0},
      {-1},
#line 225 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str132,			"AcDbBlockRotationParameter",		ODBX,	0},
#line 259 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str133,	   	"AcDbGroundPlaneBackground",		SCENEOE,	0},
      {-1},
#line 77 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str135,	"AcDbProxyObjectWrapper",	ODBX_OR_A2000CLASS,	0},
      {-1}, {-1},
#line 156 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str138,	"AcDbBlockParameterDependencyBody",	ODBX,	0},
#line 187 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str139,		"AcDbAssocEdgeActionParam",	ODBX,	0},
      {-1},
#line 71 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str141,	"AcDbMLeader",		ODBX,	1},
      {-1},
#line 284 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str143,	"AcDbImpNonPersistentObjectsCollection", ODBX, 0},
      {-1},
#line 165 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str145,	"AcDbAssocEdgeChamferActionBody",	ODBX,	0},
#line 267 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str146,	"AcDbFcfObjectContextData",		ODBX,	0},
      {-1}, {-1},
#line 166 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str149,	"AcDbAssocEdgeFilletActionBody",	ODBX,	0},
      {-1},
#line 201 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str151,			"AcDbBlockLinearParameterEntity",      	ODBX,	1},
      {-1}, {-1},
#line 260 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str154,	   		"AcDbGradientBackground",		SCENEOE,	0},
      {-1}, {-1},
#line 179 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str157,	"AcDbAssocRestoreEntityStateActionBody",ODBX,	0},
      {-1},
#line 150 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str159,	"AcDbAssocAlignedDimActionBody",	ODBX,	0},
      {-1},
#line 254 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str161, 	  			"AcDbRenderEntry",			SCENEOE,	0},
#line 141 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str162,	"AcDbShSphere",		ODBX,	0},
#line 132 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str163,	"AcDbShChamfer",	ODBX,	0},
      {-1},
#line 76 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str165,	"AcDbProxyEntityWrapper",	ODBX_OR_A2000CLASS,	0},
      {-1},
#line 136 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str167,	"AcDbShFillet",		ODBX,	0},
#line 264 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str168,	"AcDbAnnotScaleObjectContextData",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1},
#line 268 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str174,	"AcDbLeaderObjectContextData",		ODBX,	0},
#line 211 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str175,		"AcDbBlockRadialConstraintParameter",	ODBX,	0},
#line 89 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str176,	"AcDbVisualStyle",	ODBX,	0},
      {-1}, {-1},
#line 180 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str179,	"AcDbAssocRevolvedSurfaceActionBody",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1},
#line 103 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str184,	"AcDbLightList",	SCENEOE, 0},
      {-1},
#line 142 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str186,	"AcDbShSweep",		ODBX,	0},
      {-1},
#line 159 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str188,	"AcDbAssocAlignedDimActionBody",	ODBX,	0},
      {-1},
#line 182 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str190,		"AcDbAssocTrimSurfaceActionBody",	ODBX,	0},
      {-1}, {-1},
#line 234 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str193,		"AcDbBlockBasepointParameter",		ODBX,	0},
      {-1},
#line 237 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str195,			"AcDbBlockLinearGrip",			ODBX,	0},
#line 148 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str196,		"AcDbAssocNetwork",		ODBX,	0},
      {-1},
#line 114 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str198,		"AcDbMaterial",		ODBX,	0},
      {-1},
#line 238 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str200,			"AcDbBlockLookupGrip",			ODBX,	0},
#line 119 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str201,	"AcDbEvalGraph",	ODBX,	0},
      {-1},
#line 249 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str203,   			"AcDbPointPath",			SCENEOE,	0},
#line 275 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str204,				"AcDbPointCloud",			POINTCLOUD, 1},
      {-1},
#line 276 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str206,			"AcDbPointCloudEx",			POINTCLOUD, 1},
#line 277 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str207,			"AcDbPointCloudDef",			POINTCLOUD, 0},
#line 123 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str208,		"AcDbLoftedSurface",	ODBX,	1},
#line 278 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str209,			"AcDbPointCloudDefEx",			POINTCLOUD, 0},
#line 213 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str210,		"AcDbBlockLinearConstraintParameter",	ODBX,	0},
#line 143 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str211,	"AcDbShTorus",		ODBX,	0},
#line 281 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str212,			"AcDbPointCloudColorMap",		POINTCLOUD, 0},
#line 175 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str213,	"AcDbAssocNetworkSurfaceActionBody",	ODBX,	0},
      {-1},
#line 279 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str215,		"AcDbPointCloudDefReactor",		POINTCLOUD, 0},
#line 80 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str216,		"AcDbLayout",		A2000CLASS,	0},
      {-1},
#line 280 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str218,		"AcDbPointCloudDefReactorEx",		POINTCLOUD, 0},
      {-1},
#line 216 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str220,			"AcDbBlockMoveAction",			ODBX,	0},
      {-1}, {-1}, {-1}, {-1},
#line 160 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str225,	"AcDbAssocOrdinatedDimActionBody",	ODBX,	0},
#line 85 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str226,	"AcDbLayerIndex",	ODBX,	0},
#line 125 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str227,		"AcDbPlaneSurface",	ODBX,	1},
      {-1},
#line 87 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str229,	"AcDbSpatialFilter",	ODBX,	0},
#line 67 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str230,	"AcDbPolyline",		ODBX,	1},
#line 229 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str231,				"AcDbBlockXYGrip",			ODBX,	0},
      {-1},
#line 231 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str233,			"AcDbBlockAlignmentGrip",		ODBX,	0},
#line 117 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str234,	"AcDbDetailViewStyle",	ODBX,	0},
      {-1},
#line 152 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str236,	"AcDbAssocVertexActionParam",	ODBX,	0},
      {-1},
#line 222 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str238,			"AcDbBlockStretchAction",	       	ODBX,	0},
      {-1}, {-1},
#line 144 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str241,	"AcDbShWedge",		ODBX,	0},
      {-1}, {-1}, {-1}, {-1},
#line 83 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str246,	"AcDbVbaProject",	ODBX,	0},
      {-1},
#line 262 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str248,	"AcDbAlignedDimensionObjectContextData",ODBX,	0},
#line 228 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str249,			"AcDbBlockVisibilityGrip",		ODBX,	0},
#line 97 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str250,		"AcDbRasterImage",	ISM,	1},
      {-1}, {-1},
#line 79 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str253,	"AcDbDictionaryVar",	A2000CLASS,	0},
#line 263 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str254,	"AcDbAngularDimensionObjectContextData",ODBX,	0},
#line 135 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str255,	"AcDbShExtrusion",	ODBX,	0},
      {-1}, {-1},
#line 86 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str258,	"AcDbSpatialIndex",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1},
#line 221 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str263,		"AcDbBlockPolarStretchAction",	       	ODBX,	0},
#line 199 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str264,		"AcDbBlockBasepointParameterEntity",	ODBX,	1},
#line 116 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str265,	"AcDbSectionViewStyle",	ODBX,	0},
#line 155 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str266,	"AcDbAssocDimDependencyBody",	ODBX,	0},
#line 115 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str267,		"AcDbPlotSettings",	ODBX,	0},
      {-1},
#line 241 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str269,		"AcDbBlockVisibilityParameter",		ODBX,	0},
      {-1}, {-1}, {-1},
#line 232 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str273,		"AcDbBlockAlignmentParameter",		ODBX,	0},
      {-1},
#line 100 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str275,"AcDbRasterVariables",	ISM,	0},
#line 240 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str276,			"AcDbBlockXYParameter",			ODBX,	0},
#line 173 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str277,	"AcDbAssocFilletSurfaceActionBody",	ODBX,	0},
#line 140 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str278,	"AcDbShRevolve",	ODBX,	0},
      {-1},
#line 204 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str280,			"AcDbBlockPolarGripEntity",		ODBX,	1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 111 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str289, "AcDbArcAlignedText",	EXPRESS, 1},
#line 196 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str290,	"AcDbAssocArrayRectangularParameters",	ODBX,	0},
#line 209 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str291,	"AcDbBlockAngularConstraintParameter",  ODBX,	0},
      {-1},
#line 272 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str293,	"AcDbRadialDimensionObjectContextData",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1},
#line 168 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str298,		"AcDbContextDataManager",		ODBX,	0},
      {-1}, {-1}, {-1},
#line 273 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str302,	"AcDbTextObjectContextData",		ODBX,	0},
#line 129 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str303,	"AcDbShBoolean",	ODBX,	0},
#line 243 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str304,   		"AcDbBlockPropertiesTableGrip",		ODBX,	0},
      {-1}, {-1},
#line 252 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str307,   			"AcDbRenderEnvironment",		SCENEOE,	0},
#line 266 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str308,	"AcDbDiametricDimensionObjectContextData",ODBX,	0},
      {-1}, {-1}, {-1},
#line 253 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str312,   			"AcDbRenderGlobal",			SCENEOE,	0},
#line 98 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str313,	"AcDbRasterImageDef",	ISM,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 134 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str329,	"AcDbShCylinder",	ODBX,	0},
      {-1}, {-1}, {-1},
#line 139 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str333,	"AcDbShPyramid",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1},
#line 191 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str338,		"AcDbAssocPointRefActionParam",	ODBX,	0},
      {-1}, {-1}, {-1},
#line 93 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str342,	"AcDbCellStyleMap",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 92 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str358,	"AcDbTableGeometry",	ODBX,	0},
#line 248 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str359,   			"AcDbMotionPath",			SCENEOE,	0},
      {-1}, {-1}, {-1}, {-1},
#line 214 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str364,	"AcDbBlockHorizontalConstraintParameter",ODBX,	0},
      {-1},
#line 99 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str366,"AcDbRasterImageDefReactor",	ISM,	0},
#line 174 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str367,	"AcDbAssocLoftedSurfaceActionBody",	ODBX,	0},
      {-1},
#line 74 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str369,		"AcDbDictionaryWithDefault",	ODBX_OR_A2000CLASS,	0},
#line 122 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str370,	"AcDbExtrudedSurface",	ODBX,	1},
      {-1}, {-1},
#line 203 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str373,		"AcDbBlockRotationParameterEntity",     ODBX,	1},
      {-1}, {-1}, {-1},
#line 185 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str377,		"AcDbAssocAsmbodyActionParam",	ODBX,	0},
      {-1}, {-1}, {-1},
#line 244 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str381,		"AcDbDynamicBlockProxyNode",	       	ODBX,	0},
      {-1},
#line 186 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str383,		"AcDbAssocCompoundActionParam",	ODBX,	0},
      {-1},
#line 75 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str385,		"AcDbPlaceHolder",		ODBX_OR_A2000CLASS,	0},
      {-1}, {-1},
#line 70 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str388,	"AcDbArcDimension",	ODBX,	1},
      {-1}, {-1},
#line 212 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str391,	"AcDbBlockAlignedConstraintParameter",  ODBX,	0},
      {-1},
#line 137 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str393,	"AcDbShHistory",	ODBX,	0},
      {-1}, {-1},
#line 190 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str396,		"AcDbAssocObjectActionParam",	ODBX,	0},
      {-1}, {-1}, {-1},
#line 202 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str400,			"AcDbBlockPointParameterEntity",      	ODBX,	1},
#line 258 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str401,	   		"AcDbSolidBackground",			SCENEOE,	0},
      {-1},
#line 151 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str403,"AcDbAssocOsnapPointRefActionParam",	ODBX,	0},
#line 256 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str404,	   		"AcDbSkyBackground",			SCENEOE,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 81 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str413,	"AcDbSortentsTable",	A2000CLASS,	0},
      {-1}, {-1}, {-1},
#line 215 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str417,	"AcDbBlockVerticalConstraintParameter",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1},
#line 236 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str423,				"AcDbBlockFlipGrip",			ODBX,	0},
      {-1}, {-1},
#line 230 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str426,		"AcDbBlockGripExpr",			ODBX,	0},
      {-1}, {-1},
#line 270 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str429,	"AcDbOrdinateDimensionObjectContextData",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1},
#line 167 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str441,	"AcDbAssocMLeaderActionBody",		ODBX,	0},
      {-1}, {-1},
#line 118 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str444,		"AcDbGeoPositionMarker",ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 153 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str453,	"AcDbAssocGeomDependency",	ODBX,	0},
      {-1}, {-1},
#line 251 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str456,   		"AcDbRapidRTRenderSettings",		ODBX,		0},
      {-1}, {-1}, {-1}, {-1}, {-1},
#line 176 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str462,	"AcDbAssocOffsetSurfaceActionBody",	ODBX,	0},
      {-1}, {-1},
#line 269 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str465,	"AcDbMLeaderObjectContextData",		ODBX,	0},
      {-1}, {-1}, {-1}, {-1},
#line 126 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str470,	"AcDbRevolvedSurface",	ODBX,	1},
#line 257 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str471,	   		"AcDbImageBackground",			SCENEOE,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 131 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str490,	"AcDbShBrep",		ODBX,	0},
      {-1}, {-1}, {-1}, {-1},
#line 133 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str495,	"AcDbShCone",		ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 198 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str514,		"AcDbBlockAlignmentParameterEntity",	ODBX,	1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1},
#line 250 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str528,   		"AcDbMentalRayRenderSettings",		SCENEOE,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1},
#line 271 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str540,	"AcDbRadialDimensionLargeObjectContextData",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1},
#line 138 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str570,	"AcDbShLoft",		ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1},
#line 245 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str599,"AcDbDynamicBlockPurgePreventer",      	ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1},
#line 255 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str649,   		"AcDbIBLBackground",			SCENEOE,	0}
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
#line 285 "src/dxfclasses.in"


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
