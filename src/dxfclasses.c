/* ANSI-C code produced by gperf version 3.1 */
/* Command-line: gperf --output-file src/dxfclasses.tmp src/dxfclasses.in  */
/* Computed positions: -k'4,6,10,12' */

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
 *               generated via gperf from dxfclass.in
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
const struct Dwg_DXFClass * in_word_set (register const char *str, register size_t len);

enum apptypes {
  ODBXCLASS,
  ODBX_OR_A2000CLASS,
  A2000CLASS,
  SCENEOECLASS,
  ISMCLASS,
  EXPRESSCLASS,
  SPECIALCLASS,
};
#define ODBX ODBXCLASS
#define ISM ISMCLASS
#define SCENEOE SCENEOECLASS
#define EXPRESS EXPRESSCLASS
#define SPECIAL SPECIALCLASS
//%null-strings

#line 60 "src/dxfclasses.in"
struct Dwg_DXFClass {int name; const char *const cppname; unsigned apptype:4; unsigned isent:1;};

#define TOTAL_KEYWORDS 111
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 38
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 248
/* maximum key range = 246, duplicates = 0 */

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
  static const unsigned char asso_values[] =
    {
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
       15,  10, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249,   0,   5,  35,  50,   0,
       25,  37,  25,  35, 110,   0,   0,   0,  60, 105,
        5,  40,  65,   0,  15,   0,   0,  55,  10,  90,
        5, 249, 249, 249, 249, 105,   5, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249, 249, 249, 249,
      249, 249, 249, 249, 249, 249, 249
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
    char stringpool_str5[sizeof("SCALE")];
    char stringpool_str7[sizeof("WIPEOUT")];
    char stringpool_str8[sizeof("DIMASSOC")];
    char stringpool_str9[sizeof("DATATABLE")];
    char stringpool_str10[sizeof("TABLESTYLE")];
    char stringpool_str11[sizeof("VISUALSTYLE")];
    char stringpool_str15[sizeof("RTEXT")];
    char stringpool_str20[sizeof("ACDBASSOCACTION")];
    char stringpool_str25[sizeof("ACDBASSOCACTIONPARAM")];
    char stringpool_str26[sizeof("MULTILEADER")];
    char stringpool_str29[sizeof("ACDBASSOCARRAYACTIONBODY")];
    char stringpool_str30[sizeof("LIGHT")];
    char stringpool_str31[sizeof("ACDBASSOCVERTEXACTIONPARAM")];
    char stringpool_str33[sizeof("IDBUFFER")];
    char stringpool_str34[sizeof("ACDBASSOCPATHACTIONPARAM")];
    char stringpool_str35[sizeof("ACDBASSOCARRAYMODIFYACTIONBODY")];
    char stringpool_str36[sizeof("ACDBASSOCPERSSUBENTMANAGER")];
    char stringpool_str37[sizeof("ACDBASSOCEXTENDSURFACEACTIONBODY")];
    char stringpool_str39[sizeof("ACDBASSOCEXTRUDEDSURFACEACTIONBODY")];
    char stringpool_str40[sizeof("HATCH")];
    char stringpool_str41[sizeof("ACDBASSOCPATCHSURFACEACTIONBODY")];
    char stringpool_str42[sizeof("IMAGE")];
    char stringpool_str43[sizeof("MATERIAL")];
    char stringpool_str44[sizeof("ACDB_LEADEROBJECTCONTEXTDATA_CLASS")];
    char stringpool_str45[sizeof("ACDBPLACEHOLDER")];
    char stringpool_str46[sizeof("ACDBASSOCPLANESURFACEACTIONBODY")];
    char stringpool_str47[sizeof("TABLECONTENT")];
    char stringpool_str48[sizeof("ACSH_PYRAMID_CLASS")];
    char stringpool_str49[sizeof("ACDB_BLKREFOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str50[sizeof("TABLEGEOMETRY")];
    char stringpool_str51[sizeof("ACDBPERSSUBENTMANAGER")];
    char stringpool_str54[sizeof("ACDBASSOCEDGEACTIONPARAM")];
    char stringpool_str55[sizeof("ACDBASSOC3POINTANGULARDIMACTIONBODY")];
    char stringpool_str56[sizeof("ACDBASSOCMLEADERACTIONBODY")];
    char stringpool_str59[sizeof("ACDBASSOCEDGEFILLETACTIONBODY")];
    char stringpool_str60[sizeof("ACDBASSOCEDGECHAMFERACTIONBODY")];
    char stringpool_str61[sizeof("ACDBASSOCSWEPTSURFACEACTIONBODY")];
    char stringpool_str62[sizeof("ACDBASSOCFILLETSURFACEACTIONBODY")];
    char stringpool_str64[sizeof("SPATIAL_FILTER")];
    char stringpool_str66[sizeof("ACDBASSOCBLENDSURFACEACTIONBODY")];
    char stringpool_str67[sizeof("CELLSTYLEMAP")];
    char stringpool_str70[sizeof("ACDBASSOCGEOMDEPENDENCY")];
    char stringpool_str71[sizeof("ACAD_EVALUATION_GRAPH")];
    char stringpool_str72[sizeof("ACSH_FILLET_CLASS")];
    char stringpool_str73[sizeof("ACDB_ALDIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str74[sizeof("ACDBASSOCLOFTEDSURFACEACTIONBODY")];
    char stringpool_str75[sizeof("ACAD_TABLE")];
    char stringpool_str79[sizeof("ACDB_ANGDIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str81[sizeof("ACDBASSOCNETWORK")];
    char stringpool_str84[sizeof("ACSH_BOX_CLASS")];
    char stringpool_str86[sizeof("WIPEOUTVARIABLES")];
    char stringpool_str88[sizeof("SORTENTSTABLE")];
    char stringpool_str89[sizeof("OLE2FRAME")];
    char stringpool_str90[sizeof("ACDB_MLEADEROBJECTCONTEXTDATA_CLASS")];
    char stringpool_str91[sizeof("ACDB_FCFOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str92[sizeof("ACDBASSOCASMBODYACTIONPARAM")];
    char stringpool_str95[sizeof("IMAGEDEF")];
    char stringpool_str96[sizeof("ACSH_SWEEP_CLASS")];
    char stringpool_str98[sizeof("ACDBASSOCNETWORKSURFACEACTIONBODY")];
    char stringpool_str99[sizeof("ACDBASSOCROTATEDDIMACTIONBODY")];
    char stringpool_str102[sizeof("PLOTSETTINGS")];
    char stringpool_str103[sizeof("ACSH_CHAMFER_CLASS")];
    char stringpool_str104[sizeof("ACDBASSOCFACEACTIONPARAM")];
    char stringpool_str106[sizeof("ACSH_TORUS_CLASS")];
    char stringpool_str107[sizeof("XRECORD")];
    char stringpool_str108[sizeof("ACDB_ANNOTSCALEOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str112[sizeof("ACSH_SPHERE_CLASS")];
    char stringpool_str113[sizeof("SPATIAL_INDEX")];
    char stringpool_str114[sizeof("ACDBASSOCDEPENDENCY")];
    char stringpool_str116[sizeof("LAYER_INDEX")];
    char stringpool_str122[sizeof("ACDBASSOCRESTOREENTITYSTATEACTIONBODY")];
    char stringpool_str123[sizeof("ACDB_DMDIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str124[sizeof("ARCALIGNEDTEXT")];
    char stringpool_str126[sizeof("LAYOUT")];
    char stringpool_str127[sizeof("MLEADERSTYLE")];
    char stringpool_str128[sizeof("ACDBASSOCCOMPOUNDACTIONPARAM")];
    char stringpool_str130[sizeof("ACDB_RADIMLGOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str133[sizeof("ACSH_REVOLVE_CLASS")];
    char stringpool_str134[sizeof("ACDBDETAILVIEWSTYLE")];
    char stringpool_str136[sizeof("ACDBASSOCOBJECTACTIONPARAM")];
    char stringpool_str138[sizeof("ACDB_RADIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str140[sizeof("ACDBASSOCORDINATEDIMACTIONBODY")];
    char stringpool_str141[sizeof("ACDBASSOCDIMDEPENDENCYBODY")];
    char stringpool_str143[sizeof("ACDBASSOCALIGNEDIMACTIONBODY")];
    char stringpool_str144[sizeof("ACDBASSOCALIGNEDDIMACTIONBODY")];
    char stringpool_str145[sizeof("ACSH_LOFT_CLASS")];
    char stringpool_str146[sizeof("ACSH_WEDGE_CLASS")];
    char stringpool_str148[sizeof("ACDBASSOCPOINTREFACTIONPARAM")];
    char stringpool_str150[sizeof("ACSH_BREP_CLASS")];
    char stringpool_str153[sizeof("ACSH_BOOLEAN_CLASS")];
    char stringpool_str155[sizeof("ACSH_EXTRUSION_CLASS")];
    char stringpool_str157[sizeof("ACDB_TEXTOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str158[sizeof("ARC_DIMENSION")];
    char stringpool_str159[sizeof("ACDBASSOCREVOLVEDSURFACEACTIONBODY")];
    char stringpool_str160[sizeof("ACDBASSOCTRIMSURFACEACTIONBODY")];
    char stringpool_str164[sizeof("ACSH_CYLINDER_CLASS")];
    char stringpool_str165[sizeof("RASTERVARIABLES")];
    char stringpool_str169[sizeof("ACDBDICTIONARYWDFLT")];
    char stringpool_str173[sizeof("IMAGEDEF_REACTOR")];
    char stringpool_str175[sizeof("ACAD_PROXY_OBJECT_WRAPPER")];
    char stringpool_str178[sizeof("ACSH_HISTORY_CLASS")];
    char stringpool_str179[sizeof("ACDBASSOCOFFSETSURFACEACTIONBODY")];
    char stringpool_str180[sizeof("ACSH_CONE_CLASS")];
    char stringpool_str184[sizeof("ACDB_ORDDIMOBJECTCONTEXTDATA_CLASS")];
    char stringpool_str185[sizeof("ACDBSECTIONVIEWSTYLE")];
    char stringpool_str195[sizeof("ACAD_PROXY_ENTITY_WRAPPER")];
    char stringpool_str205[sizeof("LWPOLYLINE")];
    char stringpool_str216[sizeof("VBA_PROJECT")];
    char stringpool_str228[sizeof("DICTIONARYVAR")];
    char stringpool_str248[sizeof("ACDBASSOCOSNAPPOINTREFACTIONPARAM")];
  };
static const struct stringpool_t stringpool_contents =
  {
    "SUN",
    "SCALE",
    "WIPEOUT",
    "DIMASSOC",
    "DATATABLE",
    "TABLESTYLE",
    "VISUALSTYLE",
    "RTEXT",
    "ACDBASSOCACTION",
    "ACDBASSOCACTIONPARAM",
    "MULTILEADER",
    "ACDBASSOCARRAYACTIONBODY",
    "LIGHT",
    "ACDBASSOCVERTEXACTIONPARAM",
    "IDBUFFER",
    "ACDBASSOCPATHACTIONPARAM",
    "ACDBASSOCARRAYMODIFYACTIONBODY",
    "ACDBASSOCPERSSUBENTMANAGER",
    "ACDBASSOCEXTENDSURFACEACTIONBODY",
    "ACDBASSOCEXTRUDEDSURFACEACTIONBODY",
    "HATCH",
    "ACDBASSOCPATCHSURFACEACTIONBODY",
    "IMAGE",
    "MATERIAL",
    "ACDB_LEADEROBJECTCONTEXTDATA_CLASS",
    "ACDBPLACEHOLDER",
    "ACDBASSOCPLANESURFACEACTIONBODY",
    "TABLECONTENT",
    "ACSH_PYRAMID_CLASS",
    "ACDB_BLKREFOBJECTCONTEXTDATA_CLASS",
    "TABLEGEOMETRY",
    "ACDBPERSSUBENTMANAGER",
    "ACDBASSOCEDGEACTIONPARAM",
    "ACDBASSOC3POINTANGULARDIMACTIONBODY",
    "ACDBASSOCMLEADERACTIONBODY",
    "ACDBASSOCEDGEFILLETACTIONBODY",
    "ACDBASSOCEDGECHAMFERACTIONBODY",
    "ACDBASSOCSWEPTSURFACEACTIONBODY",
    "ACDBASSOCFILLETSURFACEACTIONBODY",
    "SPATIAL_FILTER",
    "ACDBASSOCBLENDSURFACEACTIONBODY",
    "CELLSTYLEMAP",
    "ACDBASSOCGEOMDEPENDENCY",
    "ACAD_EVALUATION_GRAPH",
    "ACSH_FILLET_CLASS",
    "ACDB_ALDIMOBJECTCONTEXTDATA_CLASS",
    "ACDBASSOCLOFTEDSURFACEACTIONBODY",
    "ACAD_TABLE",
    "ACDB_ANGDIMOBJECTCONTEXTDATA_CLASS",
    "ACDBASSOCNETWORK",
    "ACSH_BOX_CLASS",
    "WIPEOUTVARIABLES",
    "SORTENTSTABLE",
    "OLE2FRAME",
    "ACDB_MLEADEROBJECTCONTEXTDATA_CLASS",
    "ACDB_FCFOBJECTCONTEXTDATA_CLASS",
    "ACDBASSOCASMBODYACTIONPARAM",
    "IMAGEDEF",
    "ACSH_SWEEP_CLASS",
    "ACDBASSOCNETWORKSURFACEACTIONBODY",
    "ACDBASSOCROTATEDDIMACTIONBODY",
    "PLOTSETTINGS",
    "ACSH_CHAMFER_CLASS",
    "ACDBASSOCFACEACTIONPARAM",
    "ACSH_TORUS_CLASS",
    "XRECORD",
    "ACDB_ANNOTSCALEOBJECTCONTEXTDATA_CLASS",
    "ACSH_SPHERE_CLASS",
    "SPATIAL_INDEX",
    "ACDBASSOCDEPENDENCY",
    "LAYER_INDEX",
    "ACDBASSOCRESTOREENTITYSTATEACTIONBODY",
    "ACDB_DMDIMOBJECTCONTEXTDATA_CLASS",
    "ARCALIGNEDTEXT",
    "LAYOUT",
    "MLEADERSTYLE",
    "ACDBASSOCCOMPOUNDACTIONPARAM",
    "ACDB_RADIMLGOBJECTCONTEXTDATA_CLASS",
    "ACSH_REVOLVE_CLASS",
    "ACDBDETAILVIEWSTYLE",
    "ACDBASSOCOBJECTACTIONPARAM",
    "ACDB_RADIMOBJECTCONTEXTDATA_CLASS",
    "ACDBASSOCORDINATEDIMACTIONBODY",
    "ACDBASSOCDIMDEPENDENCYBODY",
    "ACDBASSOCALIGNEDIMACTIONBODY",
    "ACDBASSOCALIGNEDDIMACTIONBODY",
    "ACSH_LOFT_CLASS",
    "ACSH_WEDGE_CLASS",
    "ACDBASSOCPOINTREFACTIONPARAM",
    "ACSH_BREP_CLASS",
    "ACSH_BOOLEAN_CLASS",
    "ACSH_EXTRUSION_CLASS",
    "ACDB_TEXTOBJECTCONTEXTDATA_CLASS",
    "ARC_DIMENSION",
    "ACDBASSOCREVOLVEDSURFACEACTIONBODY",
    "ACDBASSOCTRIMSURFACEACTIONBODY",
    "ACSH_CYLINDER_CLASS",
    "RASTERVARIABLES",
    "ACDBDICTIONARYWDFLT",
    "IMAGEDEF_REACTOR",
    "ACAD_PROXY_OBJECT_WRAPPER",
    "ACSH_HISTORY_CLASS",
    "ACDBASSOCOFFSETSURFACEACTIONBODY",
    "ACSH_CONE_CLASS",
    "ACDB_ORDDIMOBJECTCONTEXTDATA_CLASS",
    "ACDBSECTIONVIEWSTYLE",
    "ACAD_PROXY_ENTITY_WRAPPER",
    "LWPOLYLINE",
    "VBA_PROJECT",
    "DICTIONARYVAR",
    "ACDBASSOCOSNAPPOINTREFACTIONPARAM"
  };
#define stringpool ((const char *) &stringpool_contents)
const struct Dwg_DXFClass *
in_word_set (register const char *str, register size_t len)
{
  static const struct Dwg_DXFClass wordlist[] =
    {
      {-1}, {-1}, {-1},
#line 100 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str3, 		"AcDbSun",		SCENEOE, 0},
      {-1},
#line 92 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str5,		"AcDbScale",		ODBX,	0},
      {-1},
#line 103 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str7,	"AcDbWipeout", 		SPECIAL, 1},
#line 102 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str8, 	"AcDbDimAssoc",		SPECIAL, 0},
#line 91 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str9,	"AcDbDataTable",	ODBX,	0},
#line 87 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str10,	"AcDbTableStyle",	ODBX,	0},
#line 86 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str11,	"AcDbVisualStyle",	ODBX,	0},
      {-1}, {-1}, {-1},
#line 105 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str15, 		"AcDbRText", 		EXPRESS, 1},
      {-1}, {-1}, {-1}, {-1},
#line 135 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str20,		"AcDbAssocAction",		ODBX,	0},
      {-1}, {-1}, {-1}, {-1},
#line 168 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str25,			"AcDbAssocActionParam",		ODBX,	0},
#line 68 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str26,	"AcDbMLeader",		ODBX,	1},
      {-1}, {-1},
#line 149 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str29,	"AcDbAssocArrayActionBody",		ODBX,	0},
#line 99 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str30,		"AcDbLight",		SCENEOE, 1},
#line 138 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str31,	"AcDbAssocVertexActionParam",	ODBX,	0},
      {-1},
#line 85 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str33,	"AcDbIdBuffer",		ODBX,	0},
#line 173 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str34,		"AcDbAssocPathActionParam",	ODBX,	0},
#line 148 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str35,	"AcDbAssocArrayModifyActionBody",	ODBX,	0},
#line 132 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str36,	"AcDbAssocPersSubentManager",	ODBX,	0},
#line 155 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str37,	"AcDbAssocExtendSurfaceActionBody",	ODBX,	0},
      {-1},
#line 156 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str39,	"AcDbAssocExtrudedSurfaceActionBody",	ODBX,	0},
#line 65 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str40,		"AcDbHatch",		ODBX,	1},
#line 162 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str41,	"AcDbAssocPatchSurfaceActionBody",	ODBX,	0},
#line 94 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str42,		"AcDbRasterImage",	ISM,	1},
#line 109 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str43,		"AcDbMaterial",		ODBX,	0},
#line 182 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str44,	"AcDbLeaderObjectContextData",		ODBX,	0},
#line 72 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str45,		"AcDbPlaceHolder",		ODBX_OR_A2000CLASS,	0},
#line 161 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str46,	"AcDbAssocPlaneSurfaceActionBody",	ODBX,	0},
#line 88 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str47,	"AcDbTableContent",	ODBX,	0},
#line 125 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str48,	"AcDbShPyramid",	ODBX,	0},
#line 179 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str49,	"AcDbBlkrefObjectContextData",		ODBX,	0},
#line 89 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str50,	"AcDbTableGeometry",	ODBX,	0},
#line 133 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str51, 		"AcDbPersSubentManager",	ODBX, 0},
      {-1}, {-1},
#line 171 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str54,		"AcDbAssocEdgeActionParam",	ODBX,	0},
#line 143 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str55,"AcDbAssoc3PointAngularDimActionBody",	ODBX,	0},
#line 152 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str56,	"AcDbAssocMLeaderActionBody",		ODBX,	0},
      {-1}, {-1},
#line 151 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str59,	"AcDbAssocEdgeFilletActionBody",	ODBX,	0},
#line 150 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str60,	"AcDbAssocEdgeChamferActionBody",	ODBX,	0},
#line 165 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str61,	"AcDbAssocSweptSurfaceActionBody",	ODBX,	0},
#line 157 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str62,	"AcDbAssocFilletSurfaceActionBody",	ODBX,	0},
      {-1},
#line 84 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str64,	"AcDbSpatialFilter",	ODBX,	0},
      {-1},
#line 154 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str66,	"AcDbAssocBlendSurfaceActionBody",	ODBX,	0},
#line 90 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str67,	"AcDbCellStyleMap",	ODBX,	0},
      {-1}, {-1},
#line 139 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str70,	"AcDbAssocGeomDependency",	ODBX,	0},
#line 113 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str71,	"AcDbEvalGraph",	ODBX,	0},
#line 122 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str72,	"AcDbShFillet",		ODBX,	0},
#line 176 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str73,	"AcDbAlignedDimensionObjectContextData",ODBX,	0},
#line 158 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str74,	"AcDbAssocLoftedSurfaceActionBody",	ODBX,	0},
#line 69 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str75,	"AcDbTable",		ODBX,	1},
      {-1}, {-1}, {-1},
#line 177 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str79,	"AcDbAngularDimensionObjectContextData",ODBX,	0},
      {-1},
#line 134 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str81,		"AcDbAssocNetwork",		ODBX,	0},
      {-1}, {-1},
#line 116 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str84,		"AcDbShBox",		ODBX,	0},
      {-1},
#line 104 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str86,"AcDbWipeoutVariables",	ODBX,	0},
      {-1},
#line 78 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str88,	"AcDbSortentsTable",	A2000CLASS,	0},
#line 66 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str89,	"AcDbOle2Frame",	ODBX,	1},
#line 183 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str90,	"AcDbMLeaderObjectContextData",		ODBX,	0},
#line 181 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str91,	"AcDbFcfObjectContextData",		ODBX,	0},
#line 169 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str92,		"AcDbAssocAsmbodyActionParam",	ODBX,	0},
      {-1}, {-1},
#line 95 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str95,	"AcDbRasterImageDef",	ISM,	0},
#line 128 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str96,	"AcDbShSweep",		ODBX,	0},
      {-1},
#line 159 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str98,	"AcDbAssocNetworkSurfaceActionBody",	ODBX,	0},
#line 146 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str99,	"AcDbAssocRotatedDimActionBody",	ODBX,	0},
      {-1}, {-1},
#line 110 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str102,		"AcDbPlotSettings",	ODBX,	0},
#line 118 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str103,	"AcDbShChamfer",	ODBX,	0},
#line 172 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str104,		"AcDbAssocFaceActionParam",	ODBX,	0},
      {-1},
#line 129 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str106,	"AcDbShTorus",		ODBX,	0},
#line 81 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str107,	"AcDbXrecord",		ODBX,	0},
#line 178 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str108,	"AcDbAnnotScaleObjectContextData",	ODBX,	0},
      {-1}, {-1}, {-1},
#line 127 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str112,	"AcDbShSphere",		ODBX,	0},
#line 83 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str113,	"AcDbSpatialIndex",	ODBX,	0},
#line 140 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str114,		"AcDbAssocDependency",		ODBX,	0},
      {-1},
#line 82 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str116,	"AcDbLayerIndex",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1},
#line 163 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str122,	"AcDbAssocRestoreEntityStateActionBody",	ODBX,	0},
#line 180 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str123,	"AcDbDiametricDimensionObjectContextData",ODBX,	0},
#line 106 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str124, "AcDbArcAlignedText",	EXPRESS, 1},
      {-1},
#line 77 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str126,		"AcDbLayout",		A2000CLASS,	0},
#line 107 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str127,	"AcDbMLeaderStyle",	SPECIAL, 0},
#line 170 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str128,		"AcDbAssocCompoundActionParam",	ODBX,	0},
      {-1},
#line 185 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str130,	"AcDbRadialDimensionLargeObjectContextData",	ODBX,	0},
      {-1}, {-1},
#line 126 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str133,	"AcDbShRevolve",	ODBX,	0},
#line 112 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str134,	"AcDbDetailViewStyle",	ODBX,	0},
      {-1},
#line 174 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str136,		"AcDbAssocObjectActionParam",	ODBX,	0},
      {-1},
#line 186 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str138,	"AcDbRadialDimensionObjectContextData",	ODBX,	0},
      {-1},
#line 145 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str140,	"AcDbAssocOrdinatedDimActionBody",	ODBX,	0},
#line 141 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str141,	"AcDbAssocDimDependencyBody",	ODBX,	0},
      {-1},
#line 144 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str143,	"AcDbAssocAlignedDimActionBody",	ODBX,	0},
#line 136 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str144,	"AcDbAssocAlignedDimActionBody",	ODBX,	0},
#line 124 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str145,	"AcDbShLoft",		ODBX,	0},
#line 130 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str146,	"AcDbShWedge",		ODBX,	0},
      {-1},
#line 175 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str148,		"AcDbAssocPointRefActionParam",	ODBX,	0},
      {-1},
#line 117 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str150,	"AcDbShBrep",		ODBX,	0},
      {-1}, {-1},
#line 115 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str153,	"AcDbShBoolean",	ODBX,	0},
      {-1},
#line 121 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str155,	"AcDbShExtrusion",	ODBX,	0},
      {-1},
#line 187 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str157,	"AcDbTextObjectContextData",		ODBX,	0},
#line 67 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str158,	"AcDbArcDimension",	ODBX,	1},
#line 164 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str159,	"AcDbAssocRevolvedSurfaceActionBody",	ODBX,	0},
#line 166 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str160,		"AcDbAssocTrimSurfaceActionBody",		ODBX,	0},
      {-1}, {-1}, {-1},
#line 120 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str164,	"AcDbShCylinder",	ODBX,	0},
#line 97 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str165,"AcDbRasterVariables",	ISM,	0},
      {-1}, {-1}, {-1},
#line 71 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str169,		"AcDbDictionaryWithDefault",	ODBX_OR_A2000CLASS,	0},
      {-1}, {-1}, {-1},
#line 96 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str173,"AcDbRasterImageDefReactor",	ISM,	0},
      {-1},
#line 74 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str175,	"AcDbProxyObjectWrapper",	ODBX_OR_A2000CLASS,	0},
      {-1}, {-1},
#line 123 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str178,	"AcDbShHistory",	ODBX,	0},
#line 160 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str179,	"AcDbAssocOffsetSurfaceActionBody",	ODBX,	0},
#line 119 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str180,	"AcDbShCone",		ODBX,	0},
      {-1}, {-1}, {-1},
#line 184 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str184,	"AcDbOrdinateDimensionObjectContextData",	ODBX,	0},
#line 111 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str185,	"AcDbSectionViewStyle",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 73 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str195,	"AcDbProxyEntityWrapper",	ODBX_OR_A2000CLASS,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 64 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str205,	"AcDbPolyline",		ODBX,	1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1},
#line 80 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str216,	"AcDbVbaProject",	ODBX,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1},
#line 76 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str228,	"AcDbDictionaryVar",	A2000CLASS,	0},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1},
#line 137 "src/dxfclasses.in"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str248,"AcDbAssocOsnapPointRefActionParam",	ODBX,	0}
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
#line 188 "src/dxfclasses.in"


/* Create classes on demand.
   Returns 1 if already in CLASSES, 0 if successfully added, -1 on error.
*/
EXPORT int
dwg_require_class (Dwg_Data *restrict dwg, const char *const restrict dxfname, const int len)
{
  const struct Dwg_DXFClass* result;
  for (BITCODE_BL i = 0; i < dwg->num_classes; i++)
    {
      Dwg_Class *klass = &dwg->dwg_class[i];
      if (strEQ (klass->dxfname, dxfname))
        return 1;
    }

  result = in_word_set (dxfname, len);
  if (result)
    {
      switch ((enum apptypes)result->apptype)
        {
        case ODBXCLASS:
          dwg_add_class (dwg, dxfname, result->cppname, "ObjectDBX Classes", result->isent);
          break;
        case ODBX_OR_A2000CLASS:
          dwg_add_class (dwg, dxfname, result->cppname,
                         dwg->header.from_version <= R_2000 ? "AutoCAD 2000" : "ObjectDBX Classes",
                         result->isent);
          break;
        case A2000CLASS:
          dwg_add_class (dwg, dxfname, result->cppname, "AutoCAD 2000", result->isent);
          break;
        case SCENEOECLASS:
          dwg_add_class (dwg, dxfname, result->cppname, "SCENEOE", result->isent);
          break;
        case ISMCLASS:
          dwg_add_class (dwg, dxfname, result->cppname, "ISM", result->isent);
          break;
        case EXPRESSCLASS:
          {
            char appname[128];
            strcpy (appname, dxfname);
            strcat (appname, "|AutoCAD Express Tool");
            dwg_add_class (dwg, dxfname, result->cppname, appname, result->isent);
          }
          break;
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
            dwg_add_class (dwg, dxfname, result->cppname, appname, result->isent);
          }
          break;
        default:
          fprintf (stderr, "dxfclass_require: Invalid apptype %d", (int)result->apptype);
          return -1;
        }
    }
  return -1;
}

/*
 * Local variables:
 *   c-file-style: "gnu"
 * End:
 * vim: expandtab shiftwidth=4 cinoptions='\:2=2' :
 */
