/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009, 2010 Free Software Foundation, Inc.                  */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwg.c: main include file
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Till Heuschmann
 * modified by Reini Urban
 */

#ifndef DWG_H
#define DWG_H

/* for uint64_t, but not in swig */
#ifndef SWIGIMPORTED
/* with autotools you get better int types, esp. on 64bit */
# ifdef HAVE_STDINT_H
#  include <stdint.h>
# endif
# ifdef HAVE_INTTYPES_H
#  include <inttypes.h>
# endif
/* wchar for R2007+ support.
 * But we need the WIN32 UTF-16 variant, not UTF-32.
 */
# if defined(HAVE_WCHAR_H) && defined(SIZEOF_WCHAR_T) && SIZEOF_WCHAR_T == 2
#  include <wchar.h>
#  define HAVE_NATIVE_WCHAR2
#  define DWGCHAR wchar_t
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define BITCODE_DOUBLE double

#define BITCODE_RC char
#ifdef _WIN32
# define FORMAT_RC "0x%2x"
#else
# define FORMAT_RC "0x%hhx"
# endif
#define BITCODE_MC long int
#define FORMAT_MC "%l"
#define BITCODE_MS long unsigned int
#define FORMAT_MS "%lu"
#define BITCODE_B unsigned char
#define FORMAT_B "%d"
#define BITCODE_BB unsigned char
#define FORMAT_BB "%uc"
/* Since R24 */
#define BITCODE_3B unsigned char
#define FORMAT_3B "%uc"
#ifdef HAVE_STDINT_H
# define BITCODE_BS uint16_t
# define BITCODE_RS uint16_t
# define BITCODE_BL uint32_t
# define BITCODE_RL uint32_t
#else
# define BITCODE_BS unsigned short int
# define BITCODE_RS unsigned short int
# define BITCODE_BL unsigned int
# define BITCODE_RL unsigned int
#endif
#ifdef HAVE_INTTYPES_H
# define FORMAT_BS "%" PRIu16
# define FORMAT_RS "%" PRIu16
# define FORMAT_BL "%" PRIu32
# define FORMAT_RL "%" PRIu32
#else
# define FORMAT_BS "%hu"
# define FORMAT_RS "%hu"
# define FORMAT_BL "%u"
# define FORMAT_RL "%u"
#endif
#define BITCODE_RD BITCODE_DOUBLE
#define FORMAT_RD "%f"
#ifdef HAVE_STDINT_H
# define BITCODE_RLL uint64_t
/* Since R2004 */
# define BITCODE_BLL uint64_t
#else
/* on 64bit just long */
# define BITCODE_RLL unsigned long
# define BITCODE_BLL unsigned long
#endif
#ifdef HAVE_INTTYPES_H
# define FORMAT_RLL "%" PRIu64
# define FORMAT_BLL "%" PRIu64
#else
# define FORMAT_RLL "%lu"
# define FORMAT_BLL "%lu"
#endif
#ifndef HAVE_NATIVE_WCHAR2
  typedef BITCODE_RS dwg_wchar_t;
# define DWGCHAR dwg_wchar_t
#endif
#define BITCODE_TF char *
#define FORMAT_TF "\"%s\""
#define BITCODE_TV char *
#define FORMAT_TV "\"%s\""
#define BITCODE_BT BITCODE_DOUBLE
#define FORMAT_BT "%f"
#define BITCODE_DD BITCODE_DOUBLE
#define FORMAT_DD "%f"
#define BITCODE_BD BITCODE_DOUBLE
#define FORMAT_BD "%f"
#define BITCODE_BE BITCODE_3BD
#define BITCODE_CMC Dwg_Color
#define BITCODE_H Dwg_Object_Ref*
#define BITCODE_4BITS BITCODE_RC
#define FORMAT_4BITS "%1x"

/* TODO: implement version dependant string parsing */
/* encode codepages/utf8 */
#define BITCODE_T  BITCODE_TV
#define BITCODE_TU BITCODE_RS*   /* UCS-2 unicode text */
#ifdef HAVE_NATIVE_WCHAR2
# define FORMAT_TU "\"%ls\""
#else
# define FORMAT_TU "\"%hn\""     /* will print garbage */
#endif

typedef struct _dwg_time_bll {
  BITCODE_BL days;
  BITCODE_BL ms;
} Dwg_Bitcode_TimeBLL;

typedef struct _dwg_bitcode_2rd
{
  BITCODE_RD x;
  BITCODE_RD y;
} Dwg_Bitcode_2RD;

typedef struct _dwg_bitcode_2bd
{
  BITCODE_BD x;
  BITCODE_BD y;
} Dwg_Bitcode_2BD;

typedef struct _dwg_bitcode_3rd
{
  BITCODE_RD x;
  BITCODE_RD y;
  BITCODE_RD z;
} Dwg_Bitcode_3RD;

typedef struct _dwg_bitcode_3bd
{
  BITCODE_BD x;
  BITCODE_BD y;
  BITCODE_BD z;
} Dwg_Bitcode_3BD;

#define BITCODE_TIMEBLL Dwg_Bitcode_TimeBLL
/* #define FORMAT_TIMEBLL FORMAT_BL "." FORMAT_BL */
#define BITCODE_2RD Dwg_Bitcode_2RD
#define BITCODE_2BD Dwg_Bitcode_2BD
#define BITCODE_2DPOINT BITCODE_2RD
#define BITCODE_3RD Dwg_Bitcode_3RD
#define BITCODE_3BD Dwg_Bitcode_3BD
#define BITCODE_3DPOINT BITCODE_3BD

/**
 Object supertypes that exist in dwg-files.
 */
typedef enum DWG_OBJECT_SUPERTYPE
{
  DWG_SUPERTYPE_UNKNOWN, DWG_SUPERTYPE_ENTITY, DWG_SUPERTYPE_OBJECT
} Dwg_Object_Supertype;

/**
 Object and Entity types that exist in dwg-files.
 */
typedef enum DWG_OBJECT_TYPE
{
  DWG_TYPE_UNUSED = 0x00,
  DWG_TYPE_TEXT = 0x01,
  DWG_TYPE_ATTRIB = 0x02,
  DWG_TYPE_ATTDEF = 0x03,
  DWG_TYPE_BLOCK = 0x04,
  DWG_TYPE_ENDBLK = 0x05,
  DWG_TYPE_SEQEND = 0x06,
  DWG_TYPE_INSERT = 0x07,
  DWG_TYPE_MINSERT = 0x08,
  /* DWG_TYPE_<UNKNOWN> = 0x09, */
  DWG_TYPE_VERTEX_2D = 0x0a,
  DWG_TYPE_VERTEX_3D = 0x0b,
  DWG_TYPE_VERTEX_MESH = 0x0c,
  DWG_TYPE_VERTEX_PFACE = 0x0d,
  DWG_TYPE_VERTEX_PFACE_FACE = 0x0e,
  DWG_TYPE_POLYLINE_2D = 0x0f,
  DWG_TYPE_POLYLINE_3D = 0x10,
  DWG_TYPE_ARC = 0x11,
  DWG_TYPE_CIRCLE = 0x12,
  DWG_TYPE_LINE = 0x13,
  DWG_TYPE_DIMENSION_ORDINATE = 0x14,
  DWG_TYPE_DIMENSION_LINEAR = 0x15,
  DWG_TYPE_DIMENSION_ALIGNED = 0x16,
  DWG_TYPE_DIMENSION_ANG3PT = 0x17,
  DWG_TYPE_DIMENSION_ANG2LN = 0x18,
  DWG_TYPE_DIMENSION_RADIUS = 0x19,
  DWG_TYPE_DIMENSION_DIAMETER = 0x1A,
  DWG_TYPE_POINT = 0x1b,
  DWG_TYPE__3DFACE = 0x1c,
  DWG_TYPE_POLYLINE_PFACE = 0x1d,
  DWG_TYPE_POLYLINE_MESH = 0x1e,
  DWG_TYPE_SOLID = 0x1f,
  DWG_TYPE_TRACE = 0x20,
  DWG_TYPE_SHAPE = 0x21,
  DWG_TYPE_VIEWPORT = 0x22,
  DWG_TYPE_ELLIPSE = 0x23,
  DWG_TYPE_SPLINE = 0x24,
  DWG_TYPE_REGION = 0x25,
  DWG_TYPE_3DSOLID = 0x26,
  DWG_TYPE_BODY = 0x27,
  DWG_TYPE_RAY = 0x28,
  DWG_TYPE_XLINE = 0x29,
  DWG_TYPE_DICTIONARY = 0x2a,
  DWG_TYPE_OLEFRAME = 0x2b,
  DWG_TYPE_MTEXT = 0x2c,
  DWG_TYPE_LEADER = 0x2d,
  DWG_TYPE_TOLERANCE = 0x2e,
  DWG_TYPE_MLINE = 0x2f,
  DWG_TYPE_BLOCK_CONTROL = 0x30,
  DWG_TYPE_BLOCK_HEADER = 0x31,
  DWG_TYPE_LAYER_CONTROL = 0x32,
  DWG_TYPE_LAYER = 0x33,
  DWG_TYPE_SHAPEFILE_CONTROL = 0x34,
  DWG_TYPE_SHAPEFILE = 0x35,
  /* DWG_TYPE_<UNKNOWN> = 0x36, */
  /* DWG_TYPE_<UNKNOWN> = 0x37, */
  DWG_TYPE_LTYPE_CONTROL = 0x38,
  DWG_TYPE_LTYPE = 0x39,
  /* DWG_TYPE_<UNKNOWN> = 0x3a, */
  /* DWG_TYPE_<UNKNOWN> = 0x3b, */
  DWG_TYPE_VIEW_CONTROL = 0x3c,
  DWG_TYPE_VIEW = 0x3d,
  DWG_TYPE_UCS_CONTROL = 0x3e,
  DWG_TYPE_UCS = 0x3f,
  DWG_TYPE_VPORT_CONTROL = 0x40,
  DWG_TYPE_VPORT = 0x41,
  DWG_TYPE_APPID_CONTROL = 0x42,
  DWG_TYPE_APPID = 0x43,
  DWG_TYPE_DIMSTYLE_CONTROL = 0x44,
  DWG_TYPE_DIMSTYLE = 0x45,
  DWG_TYPE_VP_ENT_HDR_CONTROL = 0x46,
  DWG_TYPE_VP_ENT_HDR = 0x47,
  DWG_TYPE_GROUP = 0x48,
  DWG_TYPE_MLINESTYLE = 0x49,
  DWG_TYPE_OLE2FRAME = 0x4a,
  DWG_TYPE_DUMMY = 0x4b,
  DWG_TYPE_LONG_TRANSACTION = 0x4c,
  DWG_TYPE_LWPLINE = 0x4d, /* ?? */
  DWG_TYPE_HATCH = 0x4e,
  DWG_TYPE_XRECORD = 0x4f,
  DWG_TYPE_PLACEHOLDER = 0x50,
  DWG_TYPE_VBA_PROJECT = 0x51,
  DWG_TYPE_LAYOUT = 0x52,

  DWG_TYPE_PROXY_ENTITY = 0x1f2, /* 498 */
  DWG_TYPE_PROXY_OBJECT = 0x1f3, /* 499 */

  DWG_TYPE_FREED = 0x1ff,

  /* non-fixed types > 500:
     TABLE, CELLSTYLEMAP, DBCOLOR, DICTIONARYVAR, DICTIONARYWDFLT ,
     FIELD, GROUP, HATCH, IDBUFFER, IMAGE, IMAGEDEF, IMAGEDEF_REACTOR,
     LAYER_INDEX, LAYER_FILTER, LAYOUT, LIGHTLIST, LWPLINE, MATERIAL,
     MULTILEADER, MLEADERSTYLE, OLE2FRAME, PLACEHOLDER, PLOTSETTINGS,
     RASTERVARIABLES, SCALE, SORTENTSTABLE, SPATIAL_FILTER,
     SPATIAL_INDEX, TABLEGEOMETRY, TABLESTYLE, VBA_PROJECT,
     VISUALSTYLE, WIPEOUT, WIPEOUTVARIABLE, XRECORD
   */
} Dwg_Object_Type;

/**
 Struct for handles.
 */
typedef struct _dwg_handle
{
  unsigned int code;
  unsigned int size;
  long unsigned int value;
} Dwg_Handle;

/**
 Struct for object references.
 */
typedef struct _dwg_object_ref
{
  struct _dwg_object* obj;
  Dwg_Handle handleref;
  long unsigned int absolute_ref;
} Dwg_Object_Ref;

/**
 Struct for CMC colors.
 */
typedef struct _dwg_color /* CmColor: R15 and earlier */
{
  unsigned int index;
  long unsigned int rgb;
  unsigned char flag;    /* 1: name follows, 2: book name follows */
  char* name;
  char* book_name;
  unsigned char transparency_type; /* 0 BYLAYER, 1 BYBLOCK, 3 alpha in the last byte */
} Dwg_Color;

struct _dwg_binary_chunk
{
  short size;
  int  codepage;
  union {
    char *data;
    DWGCHAR *wdata;
  } u;
};

/**
 Struct for result buffers
 */
typedef struct _dwg_resbuf
{
  short type;
  union
  {
    double  pt[3];
    char    i8;
    short   i16;
    int     i32;
    double  dbl;
    unsigned char hdl[8];
    struct _dwg_binary_chunk str;
  } value;
  struct _dwg_resbuf *next;
} Dwg_Resbuf;

/**
 struct for dwg header
 */

typedef struct _dwg_header_variables {
  BITCODE_RL bitsize;
  BITCODE_BLL requiredversions;
  BITCODE_BD unknown_0; /* 412148564080.0 */
  BITCODE_BD unknown_1; /* 1.0 */
  BITCODE_BD unknown_2; /* 1.0 */
  BITCODE_BD unknown_3; /* 1.0 */
  BITCODE_TV unknown_text1; /* "" 4x pre 2007... */
  BITCODE_TV unknown_text2; /* "" */
  BITCODE_TV unknown_text3; /* "" */
  BITCODE_TV unknown_text4; /* "" */
  BITCODE_BL unknown_8; /* 24L */
  BITCODE_BL unknown_9; /* 0L */
  BITCODE_BS unknown_10; /* 0 */
  BITCODE_H current_vp_ent_hdr;
  BITCODE_B DIMASO;
  BITCODE_B DIMSHO;
  BITCODE_B DIMSAV; /* undocumented */
  BITCODE_B PLINEGEN;
  BITCODE_B ORTHOMODE;
  BITCODE_B REGENMODE;
  BITCODE_B FILLMODE;
  BITCODE_B QTEXTMODE;
  BITCODE_B PSLTSCALE;
  BITCODE_B LIMCHECK;
  BITCODE_B BLIPMODE;
  BITCODE_B unknown_11;
  BITCODE_B USRTIMER;
  BITCODE_B SKPOLY;
  BITCODE_B ANGDIR;
  BITCODE_B SPLFRAME;
  BITCODE_B ATTREQ;
  BITCODE_B ATTDIA;
  BITCODE_B MIRRTEXT;
  BITCODE_B WORLDVIEW;
  BITCODE_B WIREFRAME; /* Undocumented */
  BITCODE_B TILEMODE;
  BITCODE_B PLIMCHECK;
  BITCODE_B VISRETAIN;
  BITCODE_B DELOBJ;
  BITCODE_B DISPSILH;
  BITCODE_B PELLIPSE;
  BITCODE_BS SAVEIMAGES;
  BITCODE_BS PROXYGRAPHICS;
  BITCODE_BS DRAGMODE;
  BITCODE_BS TREEDEPTH;
  BITCODE_BS LUNITS;
  BITCODE_BS LUPREC;
  BITCODE_BS AUNITS;
  BITCODE_BS AUPREC;
  BITCODE_BS OSMODE;
  BITCODE_BS ATTMODE;
  BITCODE_BS COORDS;
  BITCODE_BS PDMODE;
  BITCODE_BS PICKSTYLE;
  BITCODE_BL unknown_12;
  BITCODE_BL unknown_13;
  BITCODE_BL unknown_14;
  BITCODE_BS USERI1;
  BITCODE_BS USERI2;
  BITCODE_BS USERI3;
  BITCODE_BS USERI4;
  BITCODE_BS USERI5;
  BITCODE_BS SPLINESEGS;
  BITCODE_BS SURFU;
  BITCODE_BS SURFV;
  BITCODE_BS SURFTYPE;
  BITCODE_BS SURFTAB1;
  BITCODE_BS SURFTAB2;
  BITCODE_BS SPLINETYPE;
  BITCODE_BS SHADEDGE;
  BITCODE_BS SHADEDIF;
  BITCODE_BS UNITMODE;
  BITCODE_BS MAXACTVP;
  BITCODE_BS ISOLINES;
  BITCODE_BS CMLJUST;
  BITCODE_BS TEXTQLTY;
  BITCODE_BL unknown_14b;
  BITCODE_BD LTSCALE;
  BITCODE_BD TEXTSIZE;
  BITCODE_BD TRACEWID;
  BITCODE_BD SKETCHINC;
  BITCODE_BD FILLETRAD;
  BITCODE_BD THICKNESS;
  BITCODE_BD ANGBASE;
  BITCODE_BD PDSIZE;
  BITCODE_BD PLINEWID;
  BITCODE_BD USERR1;
  BITCODE_BD USERR2;
  BITCODE_BD USERR3;
  BITCODE_BD USERR4;
  BITCODE_BD USERR5;
  BITCODE_BD CHAMFERA;
  BITCODE_BD CHAMFERB;
  BITCODE_BD CHAMFERC;
  BITCODE_BD CHAMFERD;
  BITCODE_BD FACETRES;
  BITCODE_BD CMLSCALE;
  BITCODE_BD CELTSCALE;
  BITCODE_TV MENU;
  BITCODE_TIMEBLL TDCREATE;
  BITCODE_TIMEBLL TDUPDATE;
  BITCODE_BL unknown_15;
  BITCODE_BL unknown_16;
  BITCODE_BL unknown_17;
  BITCODE_TIMEBLL TDINDWG;
  BITCODE_TIMEBLL TDUSRTIMER;
  BITCODE_CMC CECOLOR;
  BITCODE_RS CECOLOR_idx; /* <r13 */
  BITCODE_BS HANDLING; /* <r14: default 1 */
  BITCODE_RS HANDSEED_R11;
  BITCODE_H HANDSEED;
  BITCODE_H CLAYER;
  BITCODE_H TEXTSTYLE;
  BITCODE_H CELTYPE;
  BITCODE_H CMATERIAL;
  BITCODE_H DIMSTYLE;
  BITCODE_H CMLSTYLE;
  BITCODE_BD PSVPSCALE;
  BITCODE_3BD PINSBASE;
  BITCODE_3BD PEXTMIN;
  BITCODE_3BD PEXTMAX;
  BITCODE_2RD PLIMMIN;
  BITCODE_2RD PLIMMAX;
  BITCODE_BD  PELEVATION;
  BITCODE_3BD PUCSORG;
  BITCODE_3BD PUCSXDIR;
  BITCODE_3BD PUCSYDIR;
  BITCODE_H PUCSNAME;
  BITCODE_H PUCSBASE;
  BITCODE_BS PUCSORTHOVIEW;
  BITCODE_3BD PUCSORGTOP;
  BITCODE_3BD PUCSORGBOTTOM;
  BITCODE_3BD PUCSORGLEFT;
  BITCODE_3BD PUCSORGRIGHT;
  BITCODE_3BD PUCSORGFRONT;
  BITCODE_3BD PUCSORGBACK;
  BITCODE_3BD INSBASE;
  BITCODE_3BD EXTMIN;
  BITCODE_3BD EXTMAX;
  BITCODE_2RD LIMMIN;
  BITCODE_2RD LIMMAX;
  BITCODE_2RD VIEWCTR;  /* -r11 */
  BITCODE_BD ELEVATION;
  BITCODE_RD VIEWSIZE;  /* -r11 */
  BITCODE_RS SNAPMODE;  /* -r11 */
  BITCODE_2RD SNAPUNIT; /* -r11 */
  BITCODE_2RD SNAPBASE; /* -r11 */
  BITCODE_RD SNAPANG;   /* -r11 */
  BITCODE_RS SNAPSTYL;  /* -r11 */
  BITCODE_RS SNAPISOPAIR; /* -r11 */
  BITCODE_RS GRIDMODE;  /* -r11 */
  BITCODE_2RD GRIDUNIT; /* -r11 */
  BITCODE_3BD UCSORG;
  BITCODE_3BD UCSXDIR;
  BITCODE_3BD UCSYDIR;
  BITCODE_H UCSNAME;
  BITCODE_H UCSBASE;
  BITCODE_BS UCSORTHOVIEW;
  BITCODE_H UCSORTHOREF;
  BITCODE_3BD UCSORGTOP;
  BITCODE_3BD UCSORGBOTTOM;
  BITCODE_3BD UCSORGLEFT;
  BITCODE_3BD UCSORGRIGHT;
  BITCODE_3BD UCSORGFRONT;
  BITCODE_3BD UCSORGBACK;
  BITCODE_TV DIMPOST;
  BITCODE_TV DIMAPOST;
  BITCODE_B DIMTOL;
  BITCODE_B DIMLIM;
  BITCODE_B DIMTIH;
  BITCODE_B DIMTOH;
  BITCODE_B DIMSE1;
  BITCODE_B DIMSE2;
  BITCODE_B DIMALT;
  BITCODE_B DIMTOFL;
  BITCODE_B DIMSAH;
  BITCODE_B DIMTIX;
  BITCODE_B DIMSOXD;
  BITCODE_BS DIMALTD;	/* r13-r14 only RC */
  BITCODE_BS DIMZIN;	/* r13-r14 only RC */
  BITCODE_B DIMSD1;
  BITCODE_B DIMSD2;
  BITCODE_BS DIMTOLJ;	/* r13-r14 only RC */
  BITCODE_BS DIMJUST;	/* r13-r14 only RC */
  BITCODE_BS DIMFIT;    /* r13-r14 only RC */
  BITCODE_B DIMUPT;
  BITCODE_BS DIMTZIN;	/* r13-r14 only RC */
  BITCODE_BS DIMMALTZ;	/* r13-r14 only RC */
  BITCODE_BS DIMMALTTZ;	/* r13-r14 only RC */
  BITCODE_BS DIMTAD;	/* r13-r14 only RC */
  BITCODE_BS DIMUNIT;
  BITCODE_BS DIMAUNIT;
  BITCODE_BS DIMDEC;
  BITCODE_BS DIMTDEC;
  BITCODE_BS DIMALTU;
  BITCODE_BS DIMALTTD;
  BITCODE_H DIMTXSTY;
  BITCODE_BD DIMSCALE;
  BITCODE_BD DIMASZ;
  BITCODE_BD DIMEXO;
  BITCODE_BD DIMDLI;
  BITCODE_BD DIMEXE;
  BITCODE_BD DIMRND;
  BITCODE_BD DIMDLE;
  BITCODE_BD DIMTP;
  BITCODE_BD DIMTM;
  BITCODE_BD DIMFXL;
  BITCODE_BD DIMJOGANG;
  BITCODE_BS DIMTFILL;
  BITCODE_CMC DIMTFILLCLR;
  BITCODE_BS DIMAZIN;
  BITCODE_BS DIMARCSYM;
  BITCODE_BD DIMTXT;
  BITCODE_BD DIMCEN;
  BITCODE_BD DIMTSZ;
  BITCODE_BD DIMALTF;
  BITCODE_BD DIMLFAC;
  BITCODE_BD DIMTVP;
  BITCODE_BD DIMTFAC;
  BITCODE_BD DIMGAP;
  BITCODE_T DIMPOST_T; /* preR13 => handle */
  BITCODE_T DIMAPOST_T;
  BITCODE_T DIMBLK_T;
  BITCODE_T DIMBLK1_T;
  BITCODE_T DIMBLK2_T;
  BITCODE_BD DIMALTRND;
  BITCODE_RS DIMCLRD_C; /* preR13 => CMC */
  BITCODE_RS DIMCLRE_C;
  BITCODE_RS DIMCLRT_C;
  BITCODE_CMC DIMCLRD;
  BITCODE_CMC DIMCLRE;
  BITCODE_CMC DIMCLRT;
  BITCODE_BS DIMADEC; 	/* r2000+ ... */
  BITCODE_BS DIMFRAC;
  BITCODE_BS DIMLUNIT;
  BITCODE_BS DIMDSEP;
  BITCODE_BS DIMTMOVE;
  BITCODE_BS DIMALTZ;
  BITCODE_BS DIMALTTZ;
  BITCODE_BS DIMATFIT;
  BITCODE_B  DIMFXLON;	/* r2007+ */
  BITCODE_B  DIMTXTDIRECTION; /* r2010+ */
  BITCODE_BD DIMALTMZF;	/* r2010+ */
  BITCODE_T  DIMALTMZS;	/* r2010+ */
  BITCODE_BD DIMMZF;	/* r2010+ */
  BITCODE_T  DIMMZS;	/* r2010+ */
  /*BITCODE_H DIMTXSTY;*/  /* r2000+ */
  BITCODE_H DIMLDRBLK;  /* r2000+ */
  BITCODE_H DIMBLK;  	/* r2000+ */
  BITCODE_H DIMBLK1;	/* r2000+ */
  BITCODE_H DIMBLK2;	/* r2000+ */
  BITCODE_H DIMLTYPE; /* r2007+ */
  BITCODE_H DIMLTEX1; /* r2007+ */
  BITCODE_H DIMLTEX2; /* r2007+ */
  BITCODE_BS DIMLWD;  /* r2000+ */
  BITCODE_BS DIMLWE;  /* r2000+ */
  BITCODE_H BLOCK_CONTROL_OBJECT;
  BITCODE_H LAYER_CONTROL_OBJECT;
  BITCODE_H STYLE_CONTROL_OBJECT;
  BITCODE_H LINETYPE_CONTROL_OBJECT;
  BITCODE_H VIEW_CONTROL_OBJECT;
  BITCODE_H UCS_CONTROL_OBJECT;
  BITCODE_H VPORT_CONTROL_OBJECT;
  BITCODE_H APPID_CONTROL_OBJECT;
  BITCODE_H DIMSTYLE_CONTROL_OBJECT;
  BITCODE_H VP_ENT_HDR_CONTROL_OBJECT; /* R13-R15 only */
  BITCODE_H DICTIONARY_ACAD_GROUP;
  BITCODE_H DICTIONARY_ACAD_MLINESTYLE;
  BITCODE_H DICTIONARY_NAMED_OBJECTS;
  BITCODE_BS TSTACKALIGN;	    /* r2000+ */
  BITCODE_BS TSTACKSIZE;	    /* r2000+ */
  BITCODE_TV HYPERLINKBASE;	    /* r2000+ */
  BITCODE_TV STYLESHEET;	    /* r2000+ */
  BITCODE_H DICTIONARY_LAYOUTS;	    /* r2000+ */
  BITCODE_H DICTIONARY_PLOTSETTINGS; /* r2000+ */
  BITCODE_H DICTIONARY_PLOTSTYLES;  /* r2000+ */
  BITCODE_H DICTIONARY_MATERIALS;   /* r2004+ */
  BITCODE_H DICTIONARY_COLORS;      /* r2004+ */
  BITCODE_H DICTIONARY_VISUALSTYLE; /* r2007+ */
  BITCODE_H DICTIONARY_LIGHTLIST;   /* r2010+ ?? */
  BITCODE_H unknown_20;             /* r2013+ */
  BITCODE_BL FLAGS;
  BITCODE_B  CELWEIGHT; /* = FLAGS & 0x1f */
  BITCODE_B  ENDCAPS;   /* = FLAGS & 0x60 */
  BITCODE_B  JOINSTYLE; /* = FLAGS & 0x180 */
  BITCODE_B  LWDISPLAY; /* = !(FLAGS & 0x200) */
  BITCODE_B  XEDIT;     /* = !(FLAGS & 0x400) */
  BITCODE_B  EXTNAMES;  /* = FLAGS & 0x800 */
  BITCODE_B  PSTYLEMODE; /* = FLAGS & 0x2000 */
  BITCODE_B  OLESTARTUP; /* = FLAGS & 0x4000 */
  BITCODE_BS INSUNITS;
  BITCODE_BS CEPSNTYPE;
  BITCODE_H CPSNID;
  BITCODE_TV FINGERPRINTGUID;
  BITCODE_TV VERSIONGUID;
  BITCODE_RC SORTENTS;
  BITCODE_RC INDEXCTL;
  BITCODE_RC HIDETEXT;
  BITCODE_RC XCLIPFRAME;
  BITCODE_RC DIMASSOC;
  BITCODE_RC HALOGAP;
  BITCODE_BS OBSCOLOR;
  BITCODE_BS INTERSECTIONCOLOR;
  BITCODE_RC OBSLTYPE;
  BITCODE_RC INTERSECTIONDISPLAY;
  BITCODE_TV PROJECTNAME;
  BITCODE_H BLOCK_RECORD_PSPACE;
  BITCODE_H BLOCK_RECORD_MSPACE;
  BITCODE_H LTYPE_BYLAYER;
  BITCODE_H LTYPE_BYBLOCK;
  BITCODE_H LTYPE_CONTINUOUS;
  BITCODE_B CAMERADISPLAY; /* r2007+ ... */
  BITCODE_BL unknown_21;
  BITCODE_BL unknown_22;
  BITCODE_BD unknown_23;
  BITCODE_BD STEPSPERSEC;
  BITCODE_BD STEPSIZE;
  BITCODE_BD _3DDWFPREC;
  BITCODE_BD LENSLENGTH;
  BITCODE_BD CAMERAHEIGHT;
  BITCODE_RC SOLIDHIST;
  BITCODE_RC SHOWHIST;
  BITCODE_BD PSOLWIDTH;
  BITCODE_BD PSOLHEIGHT;
  BITCODE_BD LOFTANG1;
  BITCODE_BD LOFTANG2;
  BITCODE_BD LOFTMAG1;
  BITCODE_BD LOFTMAG2;
  BITCODE_BS LOFTPARAM;
  BITCODE_RC LOFTNORMALS;
  BITCODE_BD LATITUDE;
  BITCODE_BD LONGITUDE;
  BITCODE_BD NORTHDIRECTION;
  BITCODE_BL TIMEZONE;
  BITCODE_RC LIGHTGLYPHDISPLAY;
  BITCODE_RC TILEMODELIGHTSYNCH;
  BITCODE_RC DWFFRAME;
  BITCODE_RC DGNFRAME;
  BITCODE_B unknown_47;
  BITCODE_CMC INTERFERECOLOR;
  BITCODE_H INTERFEREOBJVS;
  BITCODE_H INTERFEREVPVS;
  BITCODE_H DRAGVS;
  BITCODE_RC CSHADOW;
  BITCODE_BD unknown_53;
  BITCODE_BS unknown_54; /* r14+ ... optional */
  BITCODE_BS unknown_55;
  BITCODE_BS unknown_56;
  BITCODE_BS unknown_57;
  BITCODE_RS CRC;
} Dwg_Header_Variables;

/* OBJECTS *******************************************************************/
/**
 Struct for UNUSED (0)
 */
typedef int Dwg_Entity_UNUSED;

/**
 Struct for TEXT (1)
 */
typedef struct _dwg_entity_TEXT
{
  BITCODE_RC dataflags;
  BITCODE_RD elevation;
  BITCODE_2DPOINT insertion_pt;
  BITCODE_2DPOINT alignment_pt;
  BITCODE_3DPOINT extrusion;
  BITCODE_RD thickness;
  BITCODE_RD oblique_ang;
  BITCODE_RD rotation;
  BITCODE_RD height;
  BITCODE_RD width_factor;
  BITCODE_TV text_value;
  BITCODE_BS generation;
  BITCODE_BS horiz_alignment;
  BITCODE_BS vert_alignment;
  BITCODE_H style;
} Dwg_Entity_TEXT;

/**
 Struct for ATTRIB (2)
 */
typedef struct _dwg_entity_ATTRIB
{
  BITCODE_BD elevation;
  BITCODE_2DPOINT insertion_pt;
  BITCODE_2DPOINT alignment_pt;
  BITCODE_3DPOINT extrusion;
  BITCODE_RD thickness;
  BITCODE_RD oblique_ang;
  BITCODE_RD rotation;
  BITCODE_RD height;
  BITCODE_RD width_factor;
  BITCODE_TV text_value;
  BITCODE_BS generation;
  BITCODE_BS horiz_alignment;
  BITCODE_BS vert_alignment;
  BITCODE_RC dataflags;
  BITCODE_RC class_version; /* R2010+ */
  BITCODE_RC type;    /* R2018+ */
  BITCODE_TV tag;
  BITCODE_BS field_length;
  BITCODE_RC flags;
  BITCODE_B lock_position_flag;
  BITCODE_H style;
  BITCODE_H mtext_handles; /* R2018+ TODO */
  BITCODE_BS annotative_data_size; /* R2018+ */
  BITCODE_RC annotative_data_bytes;
  BITCODE_H  annotative_app;
  BITCODE_BS annotative_short;
} Dwg_Entity_ATTRIB;

/**
 Struct for ATTDEF (3)
 */
typedef struct _dwg_entity_ATTDEF
{
  BITCODE_BD elevation;
  BITCODE_2DPOINT insertion_pt;
  BITCODE_2DPOINT alignment_pt;
  BITCODE_3DPOINT extrusion;
  BITCODE_RD thickness;
  BITCODE_RD oblique_ang;
  BITCODE_RD rotation;
  BITCODE_RD height;
  BITCODE_RD width_factor;
  BITCODE_TV default_value;
  BITCODE_BS generation;
  BITCODE_BS horiz_alignment;
  BITCODE_BS vert_alignment;
  BITCODE_RC dataflags;
  BITCODE_TV tag;
  BITCODE_BS field_length;
  BITCODE_RC flags;
  BITCODE_B lock_position_flag;

  BITCODE_RC class_version; /* R2010+ */
  BITCODE_TV prompt;
  BITCODE_H style;
} Dwg_Entity_ATTDEF;

/**
 Struct for BLOCK (4)
 */
typedef struct _dwg_entity_BLOCK
{
  BITCODE_TV name;
} Dwg_Entity_BLOCK;

/**
 Struct for ENDBLK (5)
 */
typedef struct _dwg_entity_ENDBLK
{
  char dummy;
} Dwg_Entity_ENDBLK;

/**
 Struct for SEQEND (6)
 */
typedef struct _dwg_entity_SEQEND
{
  char dummy;
} Dwg_Entity_SEQEND;

/**
 Struct for INSERT (7)
 */
typedef struct _dwg_entity_INSERT
{
  BITCODE_3DPOINT   ins_pt;
  BITCODE_BB        scale_flag;
  BITCODE_3DPOINT   scale;
  BITCODE_BD        rotation;
  BITCODE_3DPOINT   extrusion;
  BITCODE_B         has_attribs;
  BITCODE_BL        owned_obj_count;

  BITCODE_H   block_header;
  BITCODE_H   first_attrib;
  BITCODE_H   last_attrib;
  BITCODE_H*  attrib_handles;
  BITCODE_H   seqend;
} Dwg_Entity_INSERT;

/**
 Struct for MINSERT (8)
 */
typedef struct _dwg_entity_MINSERT
{
  BITCODE_3DPOINT   ins_pt;
  BITCODE_BB        scale_flag;
  BITCODE_3DPOINT   scale;
  BITCODE_BD        rotation;
  BITCODE_3DPOINT   extrusion;
  BITCODE_B         has_attribs;
  BITCODE_BL        owned_obj_count;

  BITCODE_BS        numcols;
  BITCODE_BS        numrows;
  BITCODE_BD        col_spacing;
  BITCODE_BD        row_spacing;

  BITCODE_H   block_header;
  BITCODE_H   first_attrib;
  BITCODE_H   last_attrib;
  BITCODE_H*  attrib_handles;
  BITCODE_H   seqend;
} Dwg_Entity_MINSERT;

/**
 Struct for VERTEX_2D (10)
 */
typedef struct _dwg_entity_VERTEX_2D
{
  BITCODE_RC flag;
  BITCODE_3BD point;
  BITCODE_BD start_width;
  BITCODE_BD end_width;
  BITCODE_BL id; /* R2010+ */
  BITCODE_BD bulge;
  BITCODE_BD tangent_dir;
} Dwg_Entity_VERTEX_2D;

/**
 Struct for VERTEX_3D (11)
 */
typedef struct _dwg_entity_VERTEX_3D
{
  BITCODE_RC flag;
  BITCODE_3BD point;
} Dwg_Entity_VERTEX_3D;

/**
 Struct for VERTEX_MESH (12) - same as VERTEX_3D
 */
typedef Dwg_Entity_VERTEX_3D Dwg_Entity_VERTEX_MESH;

/**
 Struct for VERTEX_PFACE (13) - same as VERTEX_3D
 */
typedef Dwg_Entity_VERTEX_3D Dwg_Entity_VERTEX_PFACE;

/**
 Struct for VERTEX_PFACE_FACE (14)
 */
typedef struct _dwg_entity_VERTEX_PFACE_FACE
{
  BITCODE_BS vertind[4];
} Dwg_Entity_VERTEX_PFACE_FACE;

/**
 Struct for 2D POLYLINE (15)
 */
typedef struct _dwg_entity_POLYLINE_2D
{
  BITCODE_BS flag;
  BITCODE_BS curve_type;
  BITCODE_BD start_width;
  BITCODE_BD end_width;
  BITCODE_BT thickness;
  BITCODE_BD elevation;
  BITCODE_BE extrusion;
  BITCODE_BL owned_obj_count;
  BITCODE_H first_vertex;
  BITCODE_H last_vertex;
  BITCODE_H* vertex;
  BITCODE_H seqend;
} Dwg_Entity_POLYLINE_2D;

/**
 Struct for 3D POLYLINE (16)
 */
typedef struct _dwg_entity_POLYLINE_3D
{
  BITCODE_RC flag;
  BITCODE_RC flag2;
  BITCODE_BL owned_obj_count;
  BITCODE_H first_vertex;
  BITCODE_H last_vertex;
  BITCODE_H* vertex;
  BITCODE_H seqend;
} Dwg_Entity_POLYLINE_3D;

/**
 Struct for ARC (17)
 */
typedef struct _dwg_entity_ARC
{
  BITCODE_3BD center;
  BITCODE_BD radius;
  BITCODE_BT thickness;
  BITCODE_BE extrusion;
  BITCODE_BD start_angle;
  BITCODE_BD end_angle;
} Dwg_Entity_ARC;

/**
 Struct for CIRCLE (18)
 */
typedef struct _dwg_entity_CIRCLE
{
  BITCODE_3BD center;
  BITCODE_BD radius;
  BITCODE_BT thickness;
  BITCODE_BE extrusion;
} Dwg_Entity_CIRCLE;

/**
 Struct for LINE (19)
 */
typedef struct _dwg_entity_LINE
{
  BITCODE_RC Zs_are_zero;
  BITCODE_3BD start;
  BITCODE_3BD end;
  BITCODE_BT thickness;
  BITCODE_BE extrusion;
} Dwg_Entity_LINE;

/**
 * Macro for common DIMENSION declaration
 */
#define DIMENSION_COMMON \
    BITCODE_RC class_version; /* R2010+ */ \
    BITCODE_3BD extrusion; \
    BITCODE_2RD text_midpt; \
    BITCODE_BD elevation; \
    BITCODE_RC flags_1; \
    BITCODE_TV user_text; \
    BITCODE_BD text_rot; \
    BITCODE_BD horiz_dir; \
    BITCODE_3BD ins_scale; \
    BITCODE_BD ins_rotation; \
    BITCODE_BS attachment; \
    BITCODE_BS lspace_style; \
    BITCODE_BD lspace_factor; \
    BITCODE_BD act_measurement; \
    BITCODE_B unknown; \
    BITCODE_B flip_arrow1; \
    BITCODE_B flip_arrow2; \
    BITCODE_2RD clone_ins_pt;

struct _dwg_entity_DIMENSION_common
{
  DIMENSION_COMMON
};

/**
 Struct for ordinate dimension - DIMENSION_ORDINATE (20)
 */
typedef struct _dwg_entity_DIMENSION_ORDINATE
{
  DIMENSION_COMMON
  BITCODE_3BD ucsorigin_pt;
  BITCODE_3BD feature_location_pt;
  BITCODE_3BD leader_endpt;
  BITCODE_RC flags_2;
  BITCODE_H dimstyle;
  BITCODE_H block;
} Dwg_Entity_DIMENSION_ORDINATE;

/**
 Struct for linear dimension - DIMENSION_LINEAR (21)
 */
typedef struct _dwg_entity_DIMENSION_LINEAR
{
  DIMENSION_COMMON
  BITCODE_3BD _13_pt;
  BITCODE_3BD _14_pt;
  BITCODE_3BD def_pt;
  BITCODE_BD ext_line_rot;
  BITCODE_BD dim_rot;
  BITCODE_H dimstyle;
  BITCODE_H block;
} Dwg_Entity_DIMENSION_LINEAR;

/**
 Struct for aligned dimension - DIMENSION_ALIGNED (22)
 */
typedef struct _dwg_entity_DIMENSION_ALIGNED
{
  DIMENSION_COMMON
  BITCODE_3BD _13_pt;
  BITCODE_3BD _14_pt;
  BITCODE_3BD def_pt;
  BITCODE_BD ext_line_rot;
  BITCODE_H dimstyle;
  BITCODE_H block;
} Dwg_Entity_DIMENSION_ALIGNED;

/**
 Struct for angular 3pt dimension - DIMENSION_ANG3PT (23)
 */
typedef struct _dwg_entity_DIMENSION_ANG3PT
{
  DIMENSION_COMMON
  BITCODE_3BD def_pt;
  BITCODE_3BD _13_pt;
  BITCODE_3BD _14_pt;
  BITCODE_3BD first_arc_pt;
  BITCODE_H dimstyle;
  BITCODE_H block;
} Dwg_Entity_DIMENSION_ANG3PT;

/**
 Struct for angular 2 line dimension - DIMENSION_ANG2LN (24)
 */
typedef struct _dwg_entity_DIMENSION_ANG2LN
{
  DIMENSION_COMMON
  BITCODE_2RD _16_pt;
  BITCODE_3BD _13_pt;
  BITCODE_3BD _14_pt;
  BITCODE_3BD first_arc_pt;
  BITCODE_3BD def_pt;
  BITCODE_H dimstyle;
  BITCODE_H block;
} Dwg_Entity_DIMENSION_ANG2LN;

/**
 Struct for radius dimension - DIMENSION_RADIUS (25)
 */
typedef struct _dwg_entity_DIMENSION_RADIUS
{
  DIMENSION_COMMON
  BITCODE_3BD def_pt;
  BITCODE_3BD first_arc_pt;
  BITCODE_BD leader_len;
  BITCODE_H dimstyle;
  BITCODE_H block;
} Dwg_Entity_DIMENSION_RADIUS;

/**
 Struct for diameter dimension - DIMENSION_DIAMETER (26)
 */
typedef struct _dwg_entity_DIMENSION_DIAMETER
{
  DIMENSION_COMMON
  BITCODE_3BD first_arc_pt;
  BITCODE_3BD def_pt;
  BITCODE_BD leader_len;
  BITCODE_H dimstyle;
  BITCODE_H block;
} Dwg_Entity_DIMENSION_DIAMETER;

/**
 Struct for:  POINT (27)
 */
typedef struct _dwg_entity_POINT
{
  BITCODE_BD x;
  BITCODE_BD y;
  BITCODE_BD z;
  BITCODE_BT thickness;
  BITCODE_BE extrusion;
  BITCODE_BD x_ang;
} Dwg_Entity_POINT;

/**
 Struct for:  3D FACE (28)
 */
typedef struct _dwg_entity_3DFACE
{
  BITCODE_B has_no_flags;
  BITCODE_B z_is_zero;
  BITCODE_3BD corner1;
  BITCODE_3BD corner2;
  BITCODE_3BD corner3;
  BITCODE_3BD corner4;
  BITCODE_BS invis_flags;
} Dwg_Entity__3DFACE;

/**
 Struct for:  POLYLINE (PFACE) (29)
 */
typedef struct _dwg_entity_POLYLINE_PFACE
{
  BITCODE_BS numverts;
  BITCODE_BS numfaces;
  BITCODE_BL owned_obj_count;
  BITCODE_H first_vertex;
  BITCODE_H last_vertex;
  BITCODE_H* vertex;
  BITCODE_H seqend;
} Dwg_Entity_POLYLINE_PFACE;

/**
 Struct for:  POLYLINE (MESH) (30)
 */
typedef struct _dwg_entity_POLYLINE_MESH
{
  BITCODE_BS flag;
  BITCODE_BS curve_type;
  BITCODE_BS m_vert_count;
  BITCODE_BS n_vert_count;
  BITCODE_BS m_density;
  BITCODE_BS n_density;
  BITCODE_BL owned_obj_count;
  BITCODE_H first_vertex;
  BITCODE_H last_vertex;
  BITCODE_H* vertex;
  BITCODE_H seqend;
} Dwg_Entity_POLYLINE_MESH;

/**
 Struct for:  SOLID (31)
 */

typedef struct _dwg_entity_SOLID
{
  BITCODE_BT thickness;
  BITCODE_BD elevation;
  BITCODE_2RD corner1;
  BITCODE_2RD corner2;
  BITCODE_2RD corner3;
  BITCODE_2RD corner4;
  BITCODE_BE extrusion;
} Dwg_Entity_SOLID;

/**
 Struct for:  TRACE (32)
 */

typedef struct _dwg_entity_TRACE
{
  BITCODE_BT thickness;
  BITCODE_BD elevation;
  BITCODE_2RD corner1;
  BITCODE_2RD corner2;
  BITCODE_2RD corner3;
  BITCODE_2RD corner4;
  BITCODE_BE extrusion;
} Dwg_Entity_TRACE;

/**
 Struct for:  SHAPE (33)
 */

typedef struct _dwg_entity_SHAPE
{
  BITCODE_3BD ins_pt;
  BITCODE_BD scale;
  BITCODE_BD rotation;
  BITCODE_BD width_factor;
  BITCODE_BD oblique;
  BITCODE_BD thickness;
  BITCODE_BS shape_no;
  BITCODE_3BD extrusion;
  BITCODE_H shapefile;
} Dwg_Entity_SHAPE;

/**
 Struct for:  VIEWPORT ENTITY (34)
 */

typedef struct _dwg_entity_VIEWPORT
{
  BITCODE_3BD center;
  BITCODE_BD width;
  BITCODE_BD height;
  BITCODE_RS unknown; /* pre R13 only, DXF 68 */
  BITCODE_3BD view_target;  
  BITCODE_3BD view_direction;
  BITCODE_BD view_twist_angle;
  BITCODE_BD view_height;
  BITCODE_BD lens_length;
  BITCODE_BD front_clip_z;
  BITCODE_BD back_clip_z;
  BITCODE_BD snap_angle;
  BITCODE_2RD view_center;
  BITCODE_2RD snap_base;
  BITCODE_2RD snap_spacing;
  BITCODE_2RD grid_spacing;
  BITCODE_BS circle_zoom;
  BITCODE_BS grid_major;
  BITCODE_BL frozen_layer_count;
  BITCODE_BL status_flag;
  BITCODE_TV style_sheet;
  BITCODE_RC render_mode;
  BITCODE_B ucs_at_origin;
  BITCODE_B ucs_per_viewport;
  BITCODE_3BD ucs_origin;
  BITCODE_3BD ucs_x_axis;
  BITCODE_3BD ucs_y_axis;
  BITCODE_BD ucs_elevation;
  BITCODE_BS ucs_ortho_view_type;
  BITCODE_BS shadeplot_mode;
  BITCODE_B use_default_lights;
  BITCODE_RC default_lighting_type;
  BITCODE_BD brightness;
  BITCODE_BD contrast;
  BITCODE_CMC ambient_color;
} Dwg_Entity_VIEWPORT;

/**
 Struct for ELLIPSE (35)
 */
typedef struct _dwg_entity_ELLIPSE
{
  BITCODE_3BD center;
  BITCODE_3BD sm_axis;
  BITCODE_3BD extrusion;
  BITCODE_BD axis_ratio;
  BITCODE_BD start_angle;
  BITCODE_BD end_angle;
} Dwg_Entity_ELLIPSE;

/**
 Struct for spline - SPLINE (36)
 */
typedef struct _dwg_entity_SPLINE_point
{
  double x;
  double y;
  double z;
} Dwg_Entity_SPLINE_point;

typedef struct _dwg_entity_SPLINE_control_point
{
  double x;
  double y;
  double z;
  double w;
} Dwg_Entity_SPLINE_control_point;

typedef struct _dwg_entity_SPLINE
{
  BITCODE_RS flag; /* computed */
  BITCODE_BS scenario; /* 1 spline, 2 bezier */
  BITCODE_BS degree;
  BITCODE_BL splineflags1; /* 2013+: method fit points = 1, CV frame show = 2, closed = 4 */
  BITCODE_BL knotparam;    /* 2013+: Chord = 0, Square root = 1, Uniform = 2, Custom = 15 */
  BITCODE_BD fit_tol;
  BITCODE_3BD beg_tan_vec;
  BITCODE_3BD end_tan_vec;
  BITCODE_B rational;
  BITCODE_B closed_b;
  BITCODE_B periodic;
  BITCODE_B weighted; /* bit 4 of 70 */
  BITCODE_BD knot_tol;
  BITCODE_BD ctrl_tol;
  BITCODE_BS num_fit_pts;
  Dwg_Entity_SPLINE_point* fit_pts;
  BITCODE_BL num_knots;
  BITCODE_BD* knots;
  BITCODE_BL num_ctrl_pts;
  Dwg_Entity_SPLINE_control_point* ctrl_pts;
} Dwg_Entity_SPLINE;

/**
 Struct for 3DSOLID (38)
 */
#define Dwg_Entity_3DSOLID Dwg_Entity__3DSOLID
typedef struct _dwg_entity_3DSOLID_wire
{
  BITCODE_RC type;
  BITCODE_BL selection_marker;
  BITCODE_BS color;
  BITCODE_BL acis_index;
  BITCODE_BL num_points;
  BITCODE_3BD* points;
  BITCODE_B transform_present;
  BITCODE_3BD axis_x;
  BITCODE_3BD axis_y;
  BITCODE_3BD axis_z;
  BITCODE_3BD translation;
  BITCODE_BD scale;
  BITCODE_B has_rotation;
  BITCODE_B has_reflection;
  BITCODE_B has_shear;
} Dwg_Entity_3DSOLID_wire;

typedef struct _dwg_entity_3DSOLID_silhouette
{
  BITCODE_BL vp_id;
  BITCODE_3BD vp_target;
  BITCODE_3BD vp_dir_from_target;
  BITCODE_3BD vp_up_dir;
  BITCODE_B vp_perspective;
  BITCODE_BL num_wires;
  Dwg_Entity_3DSOLID_wire * wires;
} Dwg_Entity_3DSOLID_silhouette;

typedef struct _dwg_entity_3DSOLID
{
  BITCODE_B acis_empty;
  BITCODE_B unknown;
  BITCODE_BS version;
  BITCODE_BL num_blocks;
  BITCODE_BL* block_size;
  BITCODE_RC** sat_data;
  BITCODE_RC* acis_data;
  BITCODE_B wireframe_data_present;
  BITCODE_B point_present;
  BITCODE_3BD point;
  BITCODE_BL num_isolines;
  BITCODE_B isoline_present;
  BITCODE_BL num_wires;
  Dwg_Entity_3DSOLID_wire * wires;
  BITCODE_BL num_silhouettes;
  Dwg_Entity_3DSOLID_silhouette * silhouettes;
  BITCODE_B acis_empty2;
  struct _dwg_entity_3DSOLID* extra_acis_data;/* is it the best approach? */
  BITCODE_BL unknown_2007;
  BITCODE_H history_id;
  BITCODE_B ACIS_empty_bit;
  unsigned char* raw_sat_data;
} Dwg_Entity_3DSOLID;

/**
 Struct for REGION (37)
 */
typedef Dwg_Entity_3DSOLID Dwg_Entity_REGION;

/**
 Struct for BODY (39)
 */
typedef Dwg_Entity_3DSOLID Dwg_Entity_BODY;

/**
 Struct for ray - RAY (40)
 */
typedef struct _dwg_entity_RAY
{
  BITCODE_3BD point;
  BITCODE_3BD vector;
} Dwg_Entity_RAY;

/**
 Struct for XLINE (41)
 */
typedef Dwg_Entity_RAY Dwg_Entity_XLINE;

/**
 Structure for DICTIONARY (42)
 */
typedef struct _dwg_entity_DICTIONARY
{
  BITCODE_BL numitems;
  BITCODE_RC unknown_r14;
  BITCODE_BS cloning;
  BITCODE_RC hard_owner;
  BITCODE_TV* text;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H* itemhandles;

} Dwg_Object_DICTIONARY;

/**
 Struct for OLEFRAME (43)
 (replaced by OLE2FRAME (74) later)
 */
typedef struct _dwg_entity_OLEFRAME
{
  BITCODE_BS flag;
  BITCODE_BS mode;
  BITCODE_BL data_length;
  BITCODE_RC* data;
} Dwg_Entity_OLEFRAME;

/**
 Struct for MTEXT (44)
 */
typedef struct _dwg_entity_MTEXT
{
  BITCODE_3BD insertion_pt;
  BITCODE_3BD extrusion;
  BITCODE_3BD x_axis_dir;
  BITCODE_BD rect_height;
  BITCODE_BD rect_width;
  BITCODE_BD text_height;
  BITCODE_BS attachment;
  BITCODE_BS drawing_dir;
  BITCODE_BD extents_height; /* spec typo? */
  BITCODE_BD extents_width;
  BITCODE_TV text;
  BITCODE_H style;
  BITCODE_BS linespace_style;
  BITCODE_BD linespace_factor;
  BITCODE_B unknown_bit;
  BITCODE_BL bg_flag;
  BITCODE_BL bg_scale_factor;
  BITCODE_CMC bg_color;
  BITCODE_BL bg_transparent;
  BITCODE_B annotative;
  BITCODE_BS class_version;
  BITCODE_B default_flag;
  BITCODE_H reg_app;
  BITCODE_BL column_type;
  BITCODE_BL num_column_heights;
  BITCODE_BD column_width;
  BITCODE_BD gutter;
  BITCODE_B auto_height;
  BITCODE_B flow_reversed;
  BITCODE_BD *column_heights;
} Dwg_Entity_MTEXT;

/**
 Struct for LEADER (45)
 */
typedef struct _dwg_entity_LEADER
{
  BITCODE_B unknown_bit_1; /* always seems to be zero */
  BITCODE_BS annot_type;
  BITCODE_BS path_type;
  BITCODE_BL numpts;
  BITCODE_3DPOINT* points;
  BITCODE_3DPOINT origin;
  BITCODE_3DPOINT extrusion;
  BITCODE_3DPOINT x_direction;
  BITCODE_3DPOINT offset_to_block_ins_pt;
  BITCODE_3DPOINT endptproj;
  BITCODE_BD dimgap;
  BITCODE_BD box_height;
  BITCODE_BD box_width;
  BITCODE_B hooklineonxdir;
  BITCODE_B arrowhead_on;
  BITCODE_BS arrowhead_type;
  BITCODE_BD dimasz;
  BITCODE_B unknown_bit_2;
  BITCODE_B unknown_bit_3;
  BITCODE_BS unknown_short_1;
  BITCODE_BS byblock_color;
  BITCODE_B unknown_bit_4;
  BITCODE_B unknown_bit_5;
  BITCODE_H associated_annotation;
  BITCODE_H dimstyle;
} Dwg_Entity_LEADER;

/**
 Struct for TOLERANCE (46)
 */
typedef struct _dwg_entity_TOLERANCE
{
  BITCODE_BS unknown_short;
  BITCODE_BD height;
  BITCODE_BD dimgap;
  BITCODE_3BD ins_pt;
  BITCODE_3BD x_direction;
  BITCODE_3BD extrusion;
  BITCODE_TV text_string;
  BITCODE_H dimstyle;
} Dwg_Entity_TOLERANCE;

/**
 Structures for MLINE (47)
 */
typedef struct _dwg_entity_MLINE_line
{
  BITCODE_BS num_segparms;
  BITCODE_BD * segparms;
  BITCODE_BS num_areafillparms;
  BITCODE_BD* areafillparms;
} Dwg_MLINE_line;

typedef struct _dwg_entity_MLINE_vertex
{
  BITCODE_3BD vertex;
  BITCODE_3BD vertex_direction;
  BITCODE_3BD miter_direction;
  Dwg_MLINE_line* lines;
} Dwg_MLINE_vertex;

typedef struct _dwg_entity_MLINE
{
  BITCODE_BD scale;
  BITCODE_RC justification;
  BITCODE_3BD base_point;
  BITCODE_3BD extrusion;
  BITCODE_BS flags;
  BITCODE_RC num_lines; /* Linesinstyle */
  BITCODE_BS num_verts;
  Dwg_MLINE_vertex* verts;
  BITCODE_H mlinestyle;
} Dwg_Entity_MLINE;

/**
 Struct for BLOCK_CONTROL (48)
 */
typedef struct _dwg_object_BLOCK_CONTROL
{
  BITCODE_BS num_entries;
  BITCODE_H null_handle;
  BITCODE_H xdicobjhandle;
  BITCODE_H* block_headers;
  BITCODE_H model_space;
  BITCODE_H paper_space;
} Dwg_Object_BLOCK_CONTROL;

/**
 Struct for BLOCK_HEADER (49)
 */
typedef struct _dwg_object_BLOCK_HEADER
{
  BITCODE_BL __iterator;
  BITCODE_RC flag;  /* preR13 */
  BITCODE_TV entry_name;
  BITCODE_RS used;  /* preR13 */
  BITCODE_RC flag2; /* preR13 */
  BITCODE_RS flag3; /* preR13 */
  BITCODE_B _64_flag;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;
  BITCODE_B anonymous;
  BITCODE_B hasattrs;
  BITCODE_B blkisxref;
  BITCODE_B xrefoverlaid;
  BITCODE_B loaded_bit;
  BITCODE_BL owned_object_count;
  BITCODE_3DPOINT base_pt;
  BITCODE_TV xref_pname;
  BITCODE_RL insert_count;
  BITCODE_TV description;
  BITCODE_BL preview_data_size;
  BITCODE_RC* preview_data;
  BITCODE_BS insert_units;
  BITCODE_B explodable;
  BITCODE_RC block_scaling;
  BITCODE_H block_control_handle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H null_handle;
  BITCODE_H block_entity;
  BITCODE_H first_entity;
  BITCODE_H last_entity;
  BITCODE_H* entities;
  BITCODE_H endblk_entity;
  BITCODE_H* insert_handles;
  BITCODE_H layout_handle;
} Dwg_Object_BLOCK_HEADER;

/**
 Struct for LAYER_CONTROL (50)
 */
typedef struct _dwg_object_LAYER_CONTROL
{
  BITCODE_BS num_entries;
  BITCODE_H null_handle;
  BITCODE_H xdicobjhandle;
  BITCODE_H* layers;
} Dwg_Object_LAYER_CONTROL;

/**
 Struct for LAYER (51)
 */
typedef struct _dwg_object_LAYER
{
  BITCODE_RC flag;
  BITCODE_TV entry_name;
  BITCODE_RS used; /* preR13 */
  BITCODE_B _64_flag;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;
  BITCODE_B frozen;
  BITCODE_B on;
  BITCODE_B frozen_in_new;
  BITCODE_B locked;
  BITCODE_BS flag_s;
  BITCODE_CMC color;
  short      color_rs;    /* preR13, needs to be signed */
  BITCODE_RS linetype_rs; /* preR13 */
  BITCODE_H layer_control;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H xref;
  BITCODE_H plotstyle;
  BITCODE_H material;
  BITCODE_H linetype;
  BITCODE_H null_handle; /* unused, doc error */
} Dwg_Object_LAYER;

/**
 Struct for SHAPEFILE_CONTROL (52)
 */
typedef struct _dwg_object_SHAPEFILE_CONTROL
{
  BITCODE_BS num_entries;
  BITCODE_H null_handle;
  BITCODE_H xdicobjhandle;
  BITCODE_H* shapefiles;
} Dwg_Object_SHAPEFILE_CONTROL;

/**
 Struct for SHAPEFILE (53)
 preR13 this was called the STYLE table. In R14 DXF still as STYLE
 */
typedef struct _dwg_object_SHAPEFILE
{
  BITCODE_RC flag; /* preR13 */
  BITCODE_TV entry_name;
  BITCODE_RS used; /* preR13 */
  BITCODE_B _64_flag;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;
  BITCODE_B vertical;
  BITCODE_B shape_file;
  BITCODE_BD fixed_height;
  BITCODE_BD width_factor;
  BITCODE_BD oblique_ang;
  BITCODE_RC generation;
  BITCODE_BD last_height;
  BITCODE_TV font_name;
  BITCODE_TV bigfont_name;
  BITCODE_H shapefile_control;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H null_handle;
} Dwg_Object_SHAPEFILE;

/* 54 and 55 are UNKNOWN OBJECTS */

/**
 Struct for LTYPE_CONTROL (56)
 */
typedef struct _dwg_object_LTYPE_CONTROL
{
  BITCODE_BS num_entries;
  BITCODE_H null_handle;
  BITCODE_H xdicobjhandle;
  BITCODE_H* linetypes;
  BITCODE_H bylayer;
  BITCODE_H byblock;
} Dwg_Object_LTYPE_CONTROL;

/**
 Struct for LTYPE (57)
 */

typedef struct _ltype_dash{
    BITCODE_BD length;
    BITCODE_BS complex_shapecode;
    BITCODE_RD x_offset;
    BITCODE_RD y_offset;
    BITCODE_BD scale;
    BITCODE_BD rotation;
    BITCODE_BS shape_flag;
} LTYPE_dash;

typedef struct _dwg_object_LTYPE
{
  BITCODE_RC flag; /* preR13 */
  BITCODE_TV entry_name;
  BITCODE_RS used; /* preR13 */
  BITCODE_B _64_flag;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;
  BITCODE_TV description;
  BITCODE_BD pattern_len;
  BITCODE_RC alignment;
  BITCODE_RC num_dashes;
  LTYPE_dash* dash;
  BITCODE_RD* dashes_r11;
  BITCODE_RC* strings_area;
  BITCODE_H linetype_control;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H null_handle;
  BITCODE_H* shapefiles;
  BITCODE_B  text_area_is_present; /* if some shape_flag & 2 */
} Dwg_Object_LTYPE;

/* 58 and 59 are UNKNOWN OBJECTS */

/**
 Struct for VIEW_CONTROL (60)
 */
typedef struct _dwg_object_VIEW_CONTROL
{
  BITCODE_BS num_entries;
  BITCODE_H null_handle;
  BITCODE_H xdicobjhandle;
  BITCODE_H* views;
} Dwg_Object_VIEW_CONTROL;

/**
 Struct for VIEW (61)
 */
typedef struct _dwg_object_VIEW
{
  BITCODE_RC flag; /* preR13 */
  BITCODE_TV entry_name;
  BITCODE_RS used; /* preR13 */
  BITCODE_B _64_flag;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;
  BITCODE_BD height;
  BITCODE_BD width;
  BITCODE_2RD center;
  BITCODE_3BD target;
  BITCODE_3BD direction;
  BITCODE_BD twist_angle;
  BITCODE_BD lens_length;
  BITCODE_BD front_clip;
  BITCODE_BD back_clip;
  BITCODE_4BITS view_mode;
  BITCODE_RC render_mode;
  BITCODE_B use_default_lights;
  BITCODE_RC default_lightning_type;
  BITCODE_BD brightness;
  BITCODE_BD contrast;
  BITCODE_CMC ambient_color;
  BITCODE_B pspace_flag;
  BITCODE_B associated_ucs;
  BITCODE_3BD origin;
  BITCODE_3BD x_direction;
  BITCODE_3BD y_direction;
  BITCODE_BD elevation;
  BITCODE_BS orthographic_view_type;
  BITCODE_B camera_plottable;
  BITCODE_H view_control_handle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H null_handle;
  BITCODE_H background_handle;
  BITCODE_H visual_style_handle;
  BITCODE_H sun_handle;
  BITCODE_H base_ucs_handle;
  BITCODE_H named_ucs_handle;
  BITCODE_H live_section_handle;
} Dwg_Object_VIEW;

/**
 Struct for UCS_CONTROL (62)
 */
typedef struct _dwg_object_UCS_CONTROL
{
  BITCODE_BS num_entries;
  BITCODE_H null_handle;
  BITCODE_H xdicobjhandle;
  BITCODE_H* ucs;
} Dwg_Object_UCS_CONTROL;

/**
 Struct for UCS (63)
 */
typedef struct _dwg_object_UCS
{
  BITCODE_RC flag; /* preR13 */
  BITCODE_TV entry_name;
  BITCODE_RS used; /* preR13 */
  BITCODE_B _64_flag;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;
  BITCODE_3BD origin;
  BITCODE_3BD x_direction;
  BITCODE_3BD y_direction;
  BITCODE_BD elevation;
  BITCODE_BS orthographic_view_type;
  BITCODE_BS orthographic_type;
  BITCODE_H ucs_control_handle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H null_handle;
  BITCODE_H base_ucs_handle;
  BITCODE_H unknown;
} Dwg_Object_UCS;

/**
 Struct for VPORT_CONTROL (64)
 */
typedef struct _dwg_object_VPORT_CONTROL
{
  BITCODE_BS num_entries;
  BITCODE_H null_handle;
  BITCODE_H xdicobjhandle;
  BITCODE_H* vports;
} Dwg_Object_VPORT_CONTROL;

/**
 Struct for VPORT (65)
 */
typedef struct _dwg_object_VPORT
{
  BITCODE_RC flag; /* preR13 */
  BITCODE_TV entry_name;
  BITCODE_RS used; /* preR13 */
  BITCODE_B _64_flag;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;
  BITCODE_BD VIEWSIZE;
  BITCODE_BD aspect_ratio;
  BITCODE_2RD VIEWCTR;
  BITCODE_3BD view_target;
  BITCODE_3BD VIEWDIR;
  BITCODE_BD view_twist;
  BITCODE_BD lens_length;
  BITCODE_BD front_clip;
  BITCODE_BD back_clip;
  BITCODE_4BITS view_mode;
  BITCODE_RC render_mode;
  BITCODE_B use_default_lights;
  BITCODE_RC default_lightning_type;
  BITCODE_BD brightness;
  BITCODE_BD contrast;
  BITCODE_CMC ambient_color;
  BITCODE_2RD lower_left;
  BITCODE_2RD upper_right;
  BITCODE_B UCSFOLLOW;
  BITCODE_BS circle_zoom;
  BITCODE_B FASTZOOM;
  BITCODE_RC UCSICON;
  BITCODE_B GRIDMODE;
  BITCODE_2RD GRIDUNIT;
  BITCODE_B SNAPMODE;
  BITCODE_B SNAPSTYLE;
  BITCODE_BS SNAPISOPAIR;
  BITCODE_BD SNAPANG;
  BITCODE_2RD SNAPBASE;
  BITCODE_2RD SNAPUNIT;
  BITCODE_B unknown;
  BITCODE_B ucs_pre_viewport;
  BITCODE_3BD ucs_origin;
  BITCODE_3BD ucs_x_axis;
  BITCODE_3BD ucs_y_axis;
  BITCODE_BD ucs_elevation;
  BITCODE_BS ucs_orthografic_type;
  BITCODE_BS grid_flags;
  BITCODE_BS grid_major;
  BITCODE_H vport_control;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H null_handle;
  BITCODE_H background_handle;
  BITCODE_H visual_style_handle;
  BITCODE_H sun_handle;
  BITCODE_H named_ucs_handle;
  BITCODE_H base_ucs_handle;
} Dwg_Object_VPORT;

/**
 Struct for APPID_CONTROL (66)
 */
typedef struct _dwg_object_APPID_CONTROL
{
  BITCODE_BS num_entries;
  BITCODE_H null_handle;
  BITCODE_H xdicobjhandle;
  BITCODE_H* apps;
} Dwg_Object_APPID_CONTROL;

/**
 Struct for APPID (67)
 */
typedef struct _dwg_object_APPID
{
  BITCODE_RC flag; /* preR13 */
  BITCODE_TV entry_name;
  BITCODE_RS used; /* preR13 */
  BITCODE_B _64_flag;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;
  BITCODE_RC unknown;
  BITCODE_H app_control;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H null_handle;
} Dwg_Object_APPID;

/**
 Struct for DIMSTYLE_CONTROL (68)
 */
typedef struct _dwg_object_DIMSTYLE_CONTROL
{
  BITCODE_BS num_entries;
  BITCODE_RC num_morehandles; /* undocumented */
  BITCODE_H null_handle;
  BITCODE_H xdicobjhandle;
  BITCODE_H* dimstyles;
  BITCODE_H* morehandles;
} Dwg_Object_DIMSTYLE_CONTROL;

/**
 Struct for DIMSTYLE (69)
 */
typedef struct _dwg_object_DIMSTYLE
{
  BITCODE_RC flag; /* preR13 */
  BITCODE_TV entry_name;
  BITCODE_RS used; /* preR13 */
  BITCODE_B _64_flag;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;

  BITCODE_B DIMTOL;
  BITCODE_B DIMLIM;
  BITCODE_B DIMTIH;
  BITCODE_B DIMTOH;
  BITCODE_B DIMSE1;
  BITCODE_B DIMSE2;
  BITCODE_B DIMALT;
  BITCODE_B DIMTOFL;
  BITCODE_B DIMSAH;
  BITCODE_B DIMTIX;
  BITCODE_B DIMSOXD;
  BITCODE_BS DIMALTD;	/* r13-r14 only RC */
  BITCODE_BS DIMZIN;	/* r13-r14 only RC */
  BITCODE_B DIMSD1;
  BITCODE_B DIMSD2;
  BITCODE_BS DIMTOLJ;	/* r13-r14 only RC */
  BITCODE_BS DIMJUST;	/* r13-r14 only RC */
  BITCODE_BS DIMFIT;	/* r13-r14 only RC */
  BITCODE_B DIMUPT;
  BITCODE_BS DIMTZIN;	/* r13-r14 only RC */
  BITCODE_BS DIMMALTZ;	/* r13-r14 only RC */
  BITCODE_BS DIMMALTTZ;	/* r13-r14 only RC */
  BITCODE_BS DIMTAD;	/* r13-r14 only RC */
  BITCODE_BS DIMUNIT;
  BITCODE_BS DIMAUNIT;
  BITCODE_BS DIMDEC;
  BITCODE_BS DIMTDEC;
  BITCODE_BS DIMALTU;
  BITCODE_BS DIMALTTD;
  /* BITCODE_H DIMTXSTY; */
  BITCODE_BD DIMSCALE;
  BITCODE_BD DIMASZ;
  BITCODE_BD DIMEXO;
  BITCODE_BD DIMDLI;
  BITCODE_BD DIMEXE;
  BITCODE_BD DIMRND;
  BITCODE_BD DIMDLE;
  BITCODE_BD DIMTP;
  BITCODE_BD DIMTM;
  BITCODE_BD DIMFXL;
  BITCODE_BD DIMJOGANG;
  BITCODE_BS DIMTFILL;
  BITCODE_CMC DIMTFILLCLR;
  BITCODE_BS DIMAZIN;
  BITCODE_BS DIMARCSYM;
  BITCODE_BD DIMTXT;
  BITCODE_BD DIMCEN;
  BITCODE_BD DIMTSZ;
  BITCODE_BD DIMALTF;
  BITCODE_BD DIMLFAC;
  BITCODE_BD DIMTVP;
  BITCODE_BD DIMTFAC;
  BITCODE_BD DIMGAP;
  BITCODE_T DIMPOST;
  BITCODE_T DIMAPOST;
  BITCODE_T DIMBLK_T;
  BITCODE_T DIMBLK1_T;
  BITCODE_T DIMBLK2_T;
  BITCODE_BD DIMALTRND;
  BITCODE_RS DIMCLRD_N; /* preR13 */
  BITCODE_RS DIMCLRE_N; /* preR13 */
  BITCODE_RS DIMCLRT_N; /* preR13 */
  BITCODE_CMC DIMCLRD;
  BITCODE_CMC DIMCLRE;
  BITCODE_CMC DIMCLRT;
  BITCODE_BS DIMADEC;
  BITCODE_BS DIMFRAC;
  BITCODE_BS DIMLUNIT;
  BITCODE_BS DIMDSEP;
  BITCODE_BS DIMTMOVE;
  BITCODE_BS DIMALTZ;
  BITCODE_BS DIMALTTZ;
  BITCODE_BS DIMATFIT;
  BITCODE_B DIMFXLON;	/* r2007+ */
  BITCODE_B  DIMTXTDIRECTION; /* r2010+ */
  BITCODE_BD DIMALTMZF;	/* r2010+ */
  BITCODE_T  DIMALTMZS;	/* r2010+ */
  BITCODE_BD DIMMZF;	/* r2010+ */
  BITCODE_T  DIMMZS;	/* r2010+ */
  BITCODE_BS DIMLWD;
  BITCODE_BS DIMLWE;

  BITCODE_H dimstyle_control;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H extref_handle;
  BITCODE_H DIMTXSTY;

  BITCODE_H DIMLDRBLK;
  BITCODE_H DIMBLK;
  BITCODE_H DIMBLK1;
  BITCODE_H DIMBLK2;

  BITCODE_H DIMLTYPE;
  BITCODE_H DIMLTEX1;
  BITCODE_H DIMLTEX2;
} Dwg_Object_DIMSTYLE;

/**
 Struct for VP_ENT_HDR_CONTROL (70)
 */
typedef struct _dwg_object_VP_ENT_HDR_CONTROL
{
  BITCODE_BS num_entries;
  BITCODE_H null_handle;
  BITCODE_H xdicobjhandle;
  BITCODE_H* vp_ent_headers;
} Dwg_Object_VP_ENT_HDR_CONTROL;

/**
 Struct for VP_ENT_HDR (71)
 */
typedef struct _dwg_object_VP_ENT_HDR
{
  BITCODE_RC flag; /* preR13 */
  BITCODE_TV entry_name;
  BITCODE_RS used; /* preR13 */
  BITCODE_B _64_flag;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;
  BITCODE_B flag1;
  BITCODE_H vp_ent_ctrl;
  BITCODE_H xdicobjhandle;
  BITCODE_H xref_handle;
  BITCODE_H vp_ent;

} Dwg_Object_VP_ENT_HDR;

/**
 Struct for GROUP (72)
 */
typedef struct _dwg_object_GROUP
{
  BITCODE_TV name;
  BITCODE_BS unnamed;
  BITCODE_BS selectable;
  BITCODE_BL num_handles;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H* group_entries;
} Dwg_Object_GROUP;

/**
 Struct for MLINESTYLE (73)
 */
typedef struct _dwg_MLINESTYLE_line
{
  BITCODE_BD offset;
  BITCODE_CMC color;
  BITCODE_BS ltindex; /* until 2018 */
  BITCODE_H ltype;    /* since 2018 */
} Dwg_MLINESTYLE_line;

typedef struct _dwg_object_MLINESTYLE
{
  BITCODE_TV name;
  BITCODE_TV desc;
  BITCODE_BS flag;
  BITCODE_CMC fill_color;
  BITCODE_BD start_angle;
  BITCODE_BD end_angle;
  BITCODE_RC num_lines;
  Dwg_MLINESTYLE_line* lines;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;

} Dwg_Object_MLINESTYLE;

/**
 Struct for OLE2FRAME (74 + varies)
 */
typedef struct _dwg_entity_OLE2FRAME
{
  BITCODE_BS flag;
  BITCODE_BS mode;
  BITCODE_BL data_length;
  BITCODE_RC* data;
  BITCODE_RC unknown;
} Dwg_Entity_OLE2FRAME;

/**
 Struct for DUMMY (75)
 */
typedef struct _dwg_entity_DUMMY
{
  ; /* ??? not seen */
} Dwg_Entity_DUMMY;

/**
 Struct for LONG_TRANSACTION (76)
 */
typedef struct _dwg_entity_LONG_TRANSACTION
{
  ; /* ??? not seen */
} Dwg_Entity_LONG_TRANSACTION;

/* NOT SURE ABOUT THIS ONE (IS IT OBJECT OR ENTITY?): */
/**
 Struct for subtype PROXY_LWPOLYLINE (33) in a PROXY
 Not a LWPLINE (77? + varies)
 */
typedef struct _dwg_entity_LWPLINE_width
{
  BITCODE_BD start;
  BITCODE_BD end;
} Dwg_Entity_LWPLINE_width;

typedef struct _dwg_entity_PROXY_LWPOLYLINE
{
  BITCODE_RL size; /* from flags to *widths */
  BITCODE_BS flags;
  BITCODE_BD const_width;
  BITCODE_BD elevation;
  BITCODE_BD thickness;
  BITCODE_3BD normal;
  BITCODE_BL num_points;
  BITCODE_2RD* points;
  BITCODE_BL num_bulges;
  BITCODE_BD* bulges;
  BITCODE_BL num_widths;
  Dwg_Entity_LWPLINE_width* widths;
  BITCODE_RC unknown_1;
  BITCODE_RC unknown_2;
  BITCODE_RC unknown_3;
} Dwg_Entity_PROXY_LWPOLYLINE;

/**
 Struct for PROXY_ENTITY (498, 0x1f2)
 */
typedef struct _dwg_entity_PROXY_ENTITY
{
  BITCODE_BL class_id;
  BITCODE_BL version;
  BITCODE_BL maint_version;
  BITCODE_B from_dxf;
  BITCODE_RC* data;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H* objid_object_handles;
  BITCODE_MS size;
} Dwg_Entity_PROXY_ENTITY;

/**
 Struct for PROXY OBJECT (499, 0x1f3)
 */
typedef struct _dwg_object_PROXY
{
  BITCODE_BL class_id;
  BITCODE_BL version;
  BITCODE_BL maint_version;
  BITCODE_B from_dxf;
  BITCODE_RC* data;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H* objid_object_handles;
} Dwg_Object_PROXY;

/**
 * types which are fixed and non-fixed:
 * also OLE2FRAME above
 */

/**
 Structs for HATCH (78 + varies)
 */
typedef struct _dwg_entity_HATCH_color
{
  BITCODE_BD unknown_double;
  BITCODE_BS unknown_short;
  BITCODE_BL rgb_color;
  BITCODE_RC ignored_color_byte;
} Dwg_Entity_HATCH_Color;

typedef struct _dwg_entity_HATCH_control_point
{
  BITCODE_2RD point;
  BITCODE_BD weigth;
} Dwg_Entity_HATCH_ControlPoint;

typedef struct _dwg_entity_HATCH_pathseg
{
  BITCODE_RC type_status;
  /* LINE */
  BITCODE_2RD first_endpoint;
  BITCODE_2RD second_endpoint;
  /* CIRCULAR ARC */
  BITCODE_2RD center;
  BITCODE_BD radius;
  BITCODE_BD start_angle;
  BITCODE_BD end_angle;
  BITCODE_B is_ccw;
  /* ELLIPTICAL ARC */
  /* BITCODE_2RD center */
  BITCODE_2RD endpoint;
  BITCODE_BD minor_major_ratio;
  /* BITCODE_BD start_angle; */
  /* BITCODE_BD end_angle; */
  /* BITCODE_B is_ccw; */
  /* SPLINE */
  BITCODE_BL degree;
  BITCODE_B is_rational;
  BITCODE_B is_periodic;
  BITCODE_BL num_knots;
  BITCODE_BL num_control_points;
  BITCODE_BD* knots;
  Dwg_Entity_HATCH_ControlPoint* control_points;
  BITCODE_BL num_fitpts;
  BITCODE_2RD *fitpts;
  BITCODE_2RD start_tangent;
  BITCODE_2RD end_tangent;
} Dwg_Entity_HATCH_PathSeg;

typedef struct _dwg_entity_HATCH_polylinepath
{
  BITCODE_2RD point;
  BITCODE_BD bulge;
} Dwg_Entity_HATCH_PolylinePath;

typedef struct _dwg_entity_HATCH_path
{
  /* Segment path */
  BITCODE_BL flag;
  BITCODE_BL num_segs_or_paths;
  Dwg_Entity_HATCH_PathSeg* segs;

  /* Polyline path */
  BITCODE_B bulges_present;
  BITCODE_B closed;
  Dwg_Entity_HATCH_PolylinePath* polyline_paths;

  BITCODE_BL num_boundary_handles;
} Dwg_Entity_HATCH_Path;

typedef struct _dwg_entity_HATCH_DefLine
{
  BITCODE_BD  angle;
  BITCODE_2BD pt0;
  BITCODE_2BD offset;
  BITCODE_BS  num_dashes;
  BITCODE_BD * dashes;
} Dwg_Entity_HATCH_DefLine;

typedef struct _dwg_entity_HATCH
{
  BITCODE_BL is_gradient_fill;
  BITCODE_BL reserved;
  BITCODE_BD gradient_angle;
  BITCODE_BD gradient_shift;
  BITCODE_BL single_color_gradient;
  BITCODE_BD gradient_tint;
  BITCODE_BL num_colors;
  Dwg_Entity_HATCH_Color* colors;
  BITCODE_TV gradient_name;
  BITCODE_BD elevation;
  BITCODE_3BD extrusion;
  BITCODE_TV name;
  BITCODE_B solid_fill;
  BITCODE_B associative;
  BITCODE_BL num_paths;
  Dwg_Entity_HATCH_Path* paths;
  BITCODE_BS style;
  BITCODE_BS pattern_type;
  BITCODE_BD angle;
  BITCODE_BD scale_spacing;
  BITCODE_B double_flag;
  BITCODE_BS num_deflines;
  Dwg_Entity_HATCH_DefLine * deflines;
  BITCODE_B has_derived;
  BITCODE_BD pixel_size;
  BITCODE_BL num_seeds;
  BITCODE_2RD * seeds;
  BITCODE_BL num_boundary_handles;
  BITCODE_H* boundary_handles;
} Dwg_Entity_HATCH;

/**
 Struct for XRECORD (79 + varies)
 */
typedef struct _dwg_object_XRECORD
{
  BITCODE_BL num_databytes;
  BITCODE_BS cloning_flags;
  unsigned int num_eed;
  Dwg_Resbuf* xdata;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_BL num_objid_handles;
  BITCODE_H* objid_handles;
} Dwg_Object_XRECORD;

/**
 Struct for PLACEHOLDER (80 + varies)
 ACDBPLACEHOLDER
 */
typedef struct _dwg_object_PLACEHOLDER
{
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
} Dwg_Object_PLACEHOLDER;

/**
 * Entity MULTILEADER (varies)
 * R2000+
 */
typedef struct
{
  BITCODE_3BD start;
  BITCODE_3BD end;
} Leader_Break;

typedef struct
{
  BITCODE_BL num_points;
  BITCODE_3BD * points;
  BITCODE_BL num_breaks;
  Leader_Break * breaks;
  BITCODE_BL segment_index;
  BITCODE_BL index;

  BITCODE_BS type;
  BITCODE_CMC color;
  BITCODE_H type_handle;
  BITCODE_BL weight;
  BITCODE_BD arrow_size;
  BITCODE_H arrow_handle;
  BITCODE_BL flags;
} Leader_Line;

typedef struct
{
  BITCODE_BL is_default;
  BITCODE_H arrowhead;
} Leader_ArrowHead;

typedef struct
{
  BITCODE_H attdef;
  BITCODE_TV label_text;
  BITCODE_BS ui_index;
  BITCODE_BD width;
} Leader_BlockLabel;

typedef struct
{
  BITCODE_B is_valid;
  BITCODE_B unknown;
  BITCODE_3BD connection;
  BITCODE_3BD direction;
  BITCODE_BL num_breaks;
  Leader_Break * breaks;
  BITCODE_BL num_lines;
  Leader_Line * lines;
  BITCODE_BL index;
  BITCODE_BD landing_distance;
  /* ... */
  BITCODE_BS attach_dir;
} Dwg_Leader;

/* The MLEADERAnnotContext object (par 20.4.86), embedded into an MLEADER */
typedef struct
{
  BITCODE_BS class_version;
  BITCODE_B has_xdic_file;
  BITCODE_B is_default;;

  BITCODE_H scale_handle;

  BITCODE_BL num_leaders;
  Dwg_Leader * leaders;

  BITCODE_BS attach_dir;

  BITCODE_BD scale;
  BITCODE_3BD content_base;
  BITCODE_BD text_height;
  BITCODE_BD arrow_size;
  BITCODE_BD landing_gap;
  BITCODE_BS text_left;
  BITCODE_BS text_right;
  BITCODE_BS text_align;
  BITCODE_BS attach_type;

  BITCODE_B has_text_content;
  struct _text_content
    {
      BITCODE_TV label;
      BITCODE_3BD normal;
      BITCODE_H style;
      BITCODE_3BD location;
      BITCODE_3BD direction;
      BITCODE_BD rotation;
      BITCODE_BD width;
      BITCODE_BD height;
      BITCODE_BD line_spacing_factor;
      BITCODE_BS line_spacing_style;
      BITCODE_CMC color;
      BITCODE_BS align;
      BITCODE_BS flow;
      BITCODE_CMC bg_color;
      BITCODE_BD bg_scale;
      BITCODE_BL bg_transparency;
      BITCODE_B is_bg_fill;
      BITCODE_B is_bg_mask_fill;
      BITCODE_BS col_type;
      BITCODE_B is_height_auto;
      BITCODE_BD col_width;
      BITCODE_BD col_gutter;
      BITCODE_B is_col_flow_reversed;
      BITCODE_BL num_col_sizes;
      BITCODE_BD *col_sizes;
      BITCODE_B word_break;
      BITCODE_B unknown;
    } txt;

  BITCODE_B has_content_block;
  struct _content_block
    {
      BITCODE_H block_table;
      BITCODE_3BD normal;
      BITCODE_3BD location;
      BITCODE_3BD scale;
      BITCODE_BD rotation;
      BITCODE_CMC color;
      BITCODE_BD transform[16];
    } blk;
  
  BITCODE_3BD base;
  BITCODE_3BD base_dir;
  BITCODE_3BD base_vert;
  BITCODE_B is_normal_reversed;

  BITCODE_BS text_top;
  BITCODE_BS text_bottom;

} Dwg_MLEADERAnnotContext;

typedef struct _dwg_object_MULTILEADER
{
  BITCODE_BS class_version; /* r2010+ =2 */
  Dwg_MLEADERAnnotContext ctx;
  BITCODE_H leaderstyle;
  BITCODE_BL flags; /* override */
  BITCODE_BS type;
  BITCODE_CMC color;
  BITCODE_H ltype;
  BITCODE_BL linewt;
  BITCODE_B landing;
  BITCODE_B dog_leg;
  BITCODE_BD landing_dist;
  BITCODE_H arrow_head;
  BITCODE_BD arrow_head_size; /* the default */
  BITCODE_BS style_content;
  BITCODE_H text_style;
  BITCODE_BS text_left;
  BITCODE_BS text_right;
  BITCODE_BS text_angletype;
  BITCODE_BS attach_type;
  BITCODE_CMC text_color;
  BITCODE_B text_frame;
  BITCODE_H block_style;
  BITCODE_CMC block_color;
  BITCODE_3BD block_scale;
  BITCODE_BD block_rotation;
  BITCODE_BS style_attachment;
  BITCODE_B is_annotative;

  /* until r2007: */
  BITCODE_BL num_arrowheads;
  Leader_ArrowHead *arrowheads;
  BITCODE_BL num_blocklabels;
  Leader_BlockLabel *blocklabels;
  BITCODE_B neg_textdir;
  BITCODE_BS ipe_align;
  BITCODE_BS justification;
  BITCODE_BD scale_factor;
  
  BITCODE_BS attach_dir;    /* r2010+ (0 = horizontal, 1 = vertical) */
  BITCODE_BS attach_top;    /* r2010+ */
  BITCODE_BS attach_bottom; /* r2010+ */

  BITCODE_B text_extended;  /* r2013+ */
} Dwg_Entity_MULTILEADER;

/**
 * Object MLEADERSTYLE (varies)
 * R2000+
 */
typedef struct _dwg_object_MLEADERSTYLE
{
  BITCODE_BS class_version; /* r2010+ =2 */

  BITCODE_BS content_type;
  BITCODE_BS mleader_order;
  BITCODE_BS leader_order;
  BITCODE_BL max_points;
  BITCODE_BD first_seg_angle;
  BITCODE_BD second_seg_angle;
  BITCODE_BS type;
  BITCODE_CMC line_color;
  BITCODE_H line_type;
  BITCODE_BL linewt;
  BITCODE_B landing;
  BITCODE_BD landing_gap;
  BITCODE_B dog_leg;
  BITCODE_BD landing_dist;
  BITCODE_TV description;
  BITCODE_H arrow_head;
  BITCODE_BD arrow_head_size;
  BITCODE_TV text_default;
  BITCODE_H text_style;
  BITCODE_BS attach_left;
  BITCODE_BS attach_right;
  BITCODE_BS text_angle_type;
  BITCODE_BS text_align_type;
  BITCODE_CMC text_color;
  BITCODE_BD text_height;
  BITCODE_B text_frame;
  BITCODE_B is_new_format; /* computed */
  BITCODE_B text_always_left;
  BITCODE_BD align_space;
  BITCODE_H block;
  BITCODE_CMC block_color;
  BITCODE_3BD block_scale;
  BITCODE_B use_block_scale;
  BITCODE_BD block_rotation;
  BITCODE_B use_block_rotation;
  BITCODE_BS block_connection;
  BITCODE_BD scale;
  BITCODE_B changed;
  BITCODE_B is_annotative;
  BITCODE_BD break_size;

  BITCODE_BS attach_dir;    /* r2010+ (0 = horizontal, 1 = vertical) */
  BITCODE_BS attach_top;    /* r2010+ */
  BITCODE_BS attach_bottom; /* r2010+ */
  
  BITCODE_B text_extended;   /* r2013+ */
} Dwg_Object_MLEADERSTYLE;

/**
 Struct for VBA_PROJECT (81 + varies)
 Has its own optional section (probably section[5])
 */
typedef struct _dwg_object_VBA_PROJECT
{
  BITCODE_RL num_bytes;
  char *bytes;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
} Dwg_Object_VBA_PROJECT;


/**
 Struct for LAYOUT (82 + varies)
 */
typedef struct _dwg_object_LAYOUT
{
  BITCODE_TV page_setup_name;
  BITCODE_TV printer_or_config;
  BITCODE_BS plot_layout_flags;
  BITCODE_BD left_margin;
  BITCODE_BD bottom_margin;
  BITCODE_BD right_margin;
  BITCODE_BD top_margin;
  BITCODE_BD paper_width;
  BITCODE_BD paper_height;
  BITCODE_TV paper_size;
  BITCODE_2DPOINT plot_origin;
  BITCODE_BS paper_units;
  BITCODE_BS plot_rotation;
  BITCODE_BS plot_type;
  BITCODE_2DPOINT window_min;
  BITCODE_2DPOINT window_max;
  BITCODE_TV plot_view_name;
  BITCODE_BD real_world_units;
  BITCODE_BD drawing_units;
  BITCODE_TV current_style_sheet;
  BITCODE_BS scale_type;
  BITCODE_BD scale_factor;
  BITCODE_2DPOINT paper_image_origin;
  BITCODE_BS shade_plot_mode;
  BITCODE_BS shade_plot_res_level;
  BITCODE_BS shade_plot_custom_dpi;
  BITCODE_TV layout_name;
  BITCODE_BS tab_order;
  BITCODE_BS flag;
  BITCODE_3DPOINT ucs_origin;
  BITCODE_2DPOINT minimum_limits;
  BITCODE_2DPOINT maximum_limits;
  BITCODE_3DPOINT ins_point;
  BITCODE_3DPOINT ucs_x_axis;
  BITCODE_3DPOINT ucs_y_axis;
  BITCODE_BD elevation;
  BITCODE_BS orthoview_type;
  BITCODE_3DPOINT extent_min;
  BITCODE_3DPOINT extent_max;
  BITCODE_RL num_viewports;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H plot_view;
  BITCODE_H visual_style;
  BITCODE_H pspace_block_record;
  BITCODE_H last_viewport;
  BITCODE_H base_ucs;
  BITCODE_H named_ucs;
  BITCODE_H* viewports;

} Dwg_Object_LAYOUT;

/**
 * And the non-fixed types, classes, only
 */

/**
 Class TABLE (varies)
 */

// 20.4.99. also for FIELD
typedef struct _dwg_TABLE_value
{
  BITCODE_BL flags;
  BITCODE_BL data_type;
  BITCODE_BL data_size;
  BITCODE_BL data_long;
  BITCODE_BD data_double;
  BITCODE_TV data_string;
  BITCODE_RC* data_date;
  BITCODE_2RD data_point;
  BITCODE_3RD data_3dpoint;
  BITCODE_BL unit_type;
  BITCODE_TV format_string;
  BITCODE_TV value_string;
} Dwg_TABLE_value;

// 20.4.100 Custom data collection for table cells, cols, rows
typedef struct _dwg_TABLE_CustomDataItem
{
  BITCODE_TV name;
  Dwg_TABLE_value value;
} Dwg_TABLE_CustomDataItem;

typedef struct _dwg_TABLE_cell
{
  BITCODE_BS type;
  BITCODE_RC flags;
  BITCODE_B merged_value;
  BITCODE_B autofit_flag;
  BITCODE_BL merged_width_flag;
  BITCODE_BL merged_height_flag;
  BITCODE_BD rotation_value;
  BITCODE_TV text_string;
  BITCODE_BD block_scale;
  BITCODE_B additional_data_flag;
  BITCODE_BS attr_def_count;
  BITCODE_BS attr_def_index;
  BITCODE_TV attr_def_text;
  BITCODE_B additional_data_flag2;
  BITCODE_BL cell_flag_override;
  BITCODE_RC virtual_edge_flag;
  BITCODE_RS cell_alignment;
  BITCODE_B background_fill_none;
  BITCODE_CMC background_color;
  BITCODE_CMC content_color;
  BITCODE_H text_style;
  BITCODE_BD text_height;
  BITCODE_CMC top_grid_color;
  BITCODE_BS top_grid_linewt;
  BITCODE_BS top_visibility;
  BITCODE_CMC right_grid_color;
  BITCODE_BS right_grid_linewt;
  BITCODE_BS right_visibility;
  BITCODE_CMC bottom_grid_color;
  BITCODE_BS bottom_grid_linewt;
  BITCODE_BS bottom_visibility;
  BITCODE_CMC left_grid_color;
  BITCODE_BS left_grid_linewt;
  BITCODE_BS left_visibility;
  BITCODE_BL unknown;
  Dwg_TABLE_value value;
  BITCODE_H cell_handle;
  BITCODE_H* attr_def_id;
  BITCODE_H text_style_override;
} Dwg_TABLE_Cell;

typedef struct _dwg_TABLE_BreakHeight
{
  BITCODE_3BD position;
  BITCODE_BD height;
  BITCODE_BL flag;
} Dwg_TABLE_BreakHeight;

typedef struct _dwg_TABLE_BreakRow
{
  BITCODE_3BD position;
  BITCODE_BL start;
  BITCODE_BL end;
} Dwg_TABLE_BreakRow;

typedef struct _dwg_entity_TABLE
{
  BITCODE_RC unknown_rc;
  BITCODE_H unknown_h;
  BITCODE_BL unknown_bl;
  BITCODE_B unknown_b;
  BITCODE_BL unknown_bl1;
  BITCODE_3BD insertion_point;
  BITCODE_3BD scale;
  BITCODE_BB data_flags;
  BITCODE_BD rotation;
  BITCODE_3BD extrusion;
  BITCODE_B has_attribs;
  BITCODE_BL owned_object_count;
  BITCODE_BS flag_for_table_value; /* Bit flags, 0x06 (0x02 + 0x04): has block,
                                      0x10: table direction, 0 = up, 1 = down,
                                      0x20: title suppressed.
                                      Normally 0x06 is always set. */
  BITCODE_3BD horiz_direction;
  BITCODE_BL num_cols;
  BITCODE_BL num_rows;
  unsigned long num_cells; /* computed */
  BITCODE_BD* col_widths;
  BITCODE_BD* row_heights;
  Dwg_TABLE_Cell* cells;
  BITCODE_B has_table_overrides;
  BITCODE_BL table_flag_override;
  BITCODE_B title_suppressed;
  BITCODE_B header_suppressed;
  BITCODE_BS flow_direction;
  BITCODE_BD horiz_cell_margin;
  BITCODE_BD vert_cell_margin;
  BITCODE_CMC title_row_color;
  BITCODE_CMC header_row_color;
  BITCODE_CMC data_row_color;
  BITCODE_B title_row_fill_none;
  BITCODE_B header_row_fill_none;
  BITCODE_B data_row_fill_none;
  BITCODE_CMC title_row_fill_color;
  BITCODE_CMC header_row_fill_color;
  BITCODE_CMC data_row_fill_color;
  BITCODE_BS title_row_align;
  BITCODE_BS header_row_align;
  BITCODE_BS data_row_align;
  BITCODE_H title_text_style;
  BITCODE_H header_text_style;
  BITCODE_H data_text_style;
  BITCODE_BD title_row_height;
  BITCODE_BD header_row_height;
  BITCODE_BD data_row_height;

  BITCODE_B has_border_color_overrides;
  BITCODE_BL border_color_overrides_flag;
  BITCODE_CMC title_horiz_top_color;
  BITCODE_CMC title_horiz_ins_color;
  BITCODE_CMC title_horiz_bottom_color;
  BITCODE_CMC title_vert_left_color;
  BITCODE_CMC title_vert_ins_color;
  BITCODE_CMC title_vert_right_color;
  BITCODE_CMC header_horiz_top_color;
  BITCODE_CMC header_horiz_ins_color;
  BITCODE_CMC header_horiz_bottom_color;
  BITCODE_CMC header_vert_left_color;
  BITCODE_CMC header_vert_ins_color;
  BITCODE_CMC header_vert_right_color;
  BITCODE_CMC data_horiz_top_color;
  BITCODE_CMC data_horiz_ins_color;
  BITCODE_CMC data_horiz_bottom_color;
  BITCODE_CMC data_vert_left_color;
  BITCODE_CMC data_vert_ins_color;
  BITCODE_CMC data_vert_right_color;

  BITCODE_B has_border_lineweight_overrides;
  BITCODE_BL border_lineweight_overrides_flag;
  BITCODE_BS title_horiz_top_linewt;
  BITCODE_BS title_horiz_ins_linewt;
  BITCODE_BS title_horiz_bottom_linewt;
  BITCODE_BS title_vert_left_linewt;
  BITCODE_BS title_vert_ins_linewt;
  BITCODE_BS title_vert_right_linewt;
  BITCODE_BS header_horiz_top_linewt;
  BITCODE_BS header_horiz_ins_linewt;
  BITCODE_BS header_horiz_bottom_linewt;
  BITCODE_BS header_vert_left_linewt;
  BITCODE_BS header_vert_ins_linewt;
  BITCODE_BS header_vert_right_linewt;
  BITCODE_BS data_horiz_top_linewt;
  BITCODE_BS data_horiz_ins_linewt;
  BITCODE_BS data_horiz_bottom_linewt;
  BITCODE_BS data_vert_left_linewt;
  BITCODE_BS data_vert_ins_linewt;
  BITCODE_BS data_vert_right_linewt;

  BITCODE_B has_border_visibility_overrides;
  BITCODE_BL border_visibility_overrides_flag;
  BITCODE_BS title_horiz_top_visibility;
  BITCODE_BS title_horiz_ins_visibility;
  BITCODE_BS title_horiz_bottom_visibility;
  BITCODE_BS title_vert_left_visibility;
  BITCODE_BS title_vert_ins_visibility;
  BITCODE_BS title_vert_right_visibility;
  BITCODE_BS header_horiz_top_visibility;
  BITCODE_BS header_horiz_ins_visibility;
  BITCODE_BS header_horiz_bottom_visibility;
  BITCODE_BS header_vert_left_visibility;
  BITCODE_BS header_vert_ins_visibility;
  BITCODE_BS header_vert_right_visibility;
  BITCODE_BS data_horiz_top_visibility;
  BITCODE_BS data_horiz_ins_visibility;
  BITCODE_BS data_horiz_bottom_visibility;
  BITCODE_BS data_vert_left_visibility;
  BITCODE_BS data_vert_ins_visibility;
  BITCODE_BS data_vert_right_visibility;

  BITCODE_H block_header;
  BITCODE_H first_attrib;
  BITCODE_H last_attrib;
  BITCODE_H* attribs;
  BITCODE_H seqend;
  BITCODE_H table_style_id;
  BITCODE_H title_row_style_override;
  BITCODE_H header_row_style_override;
  BITCODE_H data_row_style_override;

  BITCODE_BS unknown_bs;
  BITCODE_3BD hor_dir;
  BITCODE_BL has_break_data;
  BITCODE_BL break_flag;
  BITCODE_BL break_flow_direction;
  BITCODE_BD break_spacing;
  BITCODE_BL break_unknown1;
  BITCODE_BL break_unknown2;
  BITCODE_BL num_break_heights;
  Dwg_TABLE_BreakHeight *break_heights;
  BITCODE_BL num_break_rows;
  Dwg_TABLE_BreakRow *break_rows;
} Dwg_Entity_TABLE;

typedef struct _dwg_LinkedData
{
  BITCODE_TV name;
  BITCODE_TV desc;
} Dwg_LinkedData;

typedef struct _dwg_TableCellContentAttr
{
  BITCODE_H attdef;
  BITCODE_TV value;
  BITCODE_BL index;
} Dwg_TableCellContentAttr;

// Content format 20.4.101.3
typedef struct _dwg_ContentFormat
{
  BITCODE_BL property_override_flags;
  BITCODE_BL property_flags;
  BITCODE_BL value_data_type; /* see 20.4.98 */
  BITCODE_BL value_unit_type; /* see 20.4.98 */
  BITCODE_TV value_format_string;
  BITCODE_BD rotation;
  BITCODE_BD block_scale;
  BITCODE_BL cell_align;
  BITCODE_CMC content_color;
  BITCODE_H text_style;
  BITCODE_BD text_height;
} Dwg_ContentFormat;

typedef struct _dwg_TableCellContent
{
  BITCODE_BL type;
  Dwg_TABLE_value value; // 20.4.99 Value
  BITCODE_H handle;
  BITCODE_BL num_attrs;
  Dwg_TableCellContentAttr *attrs;
  BITCODE_BS has_content_format_overrides;
  Dwg_ContentFormat content_format;
} Dwg_TableCellContent;

// 20.4.98
typedef struct _dwg_CellContentGeometry
{
  BITCODE_3BD dist_top_left;
  BITCODE_3BD dist_center;
  BITCODE_BD content_width;
  BITCODE_BD content_height;
  BITCODE_BD width;
  BITCODE_BD height;
  BITCODE_BD unknown;
} Dwg_CellContentGeometry;

typedef struct _dwg_TableCell
{
  BITCODE_BL flag;
  BITCODE_TV tooltip;
  BITCODE_BL customdata;
  BITCODE_BL num_customdata_items;
  Dwg_TABLE_CustomDataItem *customdata_items;
  BITCODE_BL has_linked_data;
  BITCODE_H  data_link;
  BITCODE_BL num_rows;
  BITCODE_BL num_cols;
  BITCODE_BL unknown;
  BITCODE_BL num_cell_contents;
  Dwg_TableCellContent *cell_contents;
  BITCODE_BL style_id;
  BITCODE_BL has_geom_data;
  BITCODE_BL geom_data_flag;
  BITCODE_BD unknown_d40;
  BITCODE_BD unknown_d41;
  BITCODE_BL has_cell_geom;
  BITCODE_H cell_geom_handle;
  Dwg_CellContentGeometry *geom_data;
} Dwg_TableCell;

typedef struct _dwg_BorderStyle
{
  BITCODE_BL edge_flags;
  BITCODE_BL border_property_overrides_flag;
  BITCODE_BL border_type;
  BITCODE_CMC color;
  BITCODE_BL linewt;
  BITCODE_H line_type;
  BITCODE_BL invisible;
  BITCODE_BD double_line_spacing;
} Dwg_BorderStyle;

// Cell style 20.4.101.4
// for TABLE, TABLECONTENT, CELLSTYLEMAP
typedef struct _dwg_CellStyle
{
  BITCODE_BL type; /* 1 cell, 2 row, 3 col, 4 fmt data, 5 table */
  BITCODE_BS data_flags;
  BITCODE_BL property_override_flags;
  BITCODE_BL merge_flags;
  BITCODE_CMC background_color;
  BITCODE_BL content_layout;
  Dwg_ContentFormat content_format;
  BITCODE_BS margin_override_flags;
  BITCODE_BD vert_margin;
  BITCODE_BD horiz_margin;
  BITCODE_BD bottom_margin;
  BITCODE_BD right_margin;
  BITCODE_BD margin_horiz_spacing;
  BITCODE_BD margin_vert_spacing;
  BITCODE_BL num_borders; /* 0-6 */
  Dwg_BorderStyle *border;
} Dwg_CellStyle;

typedef struct _dwg_TableRow
{
  BITCODE_BL num_cells;
  Dwg_TableCell *cells;
  BITCODE_BL custom_data;
  BITCODE_BL num_customdata_items;
  Dwg_TABLE_CustomDataItem *customdata_items;
  Dwg_CellStyle cell_style;
  BITCODE_BL style_id;
  BITCODE_BL height;
} Dwg_TableRow;

typedef struct _dwg_TableDataColumn
{
  BITCODE_TV name;
  BITCODE_BL custom_data;
  //BITCODE_TV data;
  Dwg_CellStyle cell_style;
  BITCODE_BL cell_style_id;
  BITCODE_BL width;
} Dwg_TableDataColumn;

typedef struct _dwg_LinkedTableData
{
  BITCODE_BL num_cols;
  Dwg_TableDataColumn *cols;
  BITCODE_BL num_rows;
  Dwg_TableRow *rows;
  BITCODE_BL num_field_refs;
  BITCODE_H *field_refs;
} Dwg_LinkedTableData;

typedef struct _dwg_FormattedTableMerged
{
  BITCODE_BL top_row;
  BITCODE_BL left_col;
  BITCODE_BL bottom_row;
  BITCODE_BL right_col;
} Dwg_FormattedTableMerged;

typedef struct _dwg_FormattedTableData
{
  Dwg_CellStyle cell_style;
  BITCODE_BL num_merged_cells;
  Dwg_FormattedTableMerged *merged_cells;
} Dwg_FormattedTableData;

typedef struct _dwg_object_TABLECONTENT
{
  Dwg_LinkedData ldata;
  Dwg_LinkedTableData tdata;
  Dwg_FormattedTableData fdata;
  BITCODE_H table_style;
} Dwg_Object_TABLECONTENT;

/**
 Class TABLESTYLE (varies)
 R2002+ TODO (just guessing)
 */

typedef struct _dwg_TABLESTYLE_Cell
{
  Dwg_TABLE_Cell cell;
  BITCODE_BL id;   /* 1=title, 2=header, 3=data, 4=table.
                      ref TABLESTYLE. custom IDs > 100 */
  BITCODE_BL type; /* 1=data, 2=label */
  BITCODE_TV name;
} Dwg_TABLESTYLE_Cell;

typedef struct _dwg_object_TABLESTYLE
{
  BITCODE_BS class_version;
  BITCODE_TV name;
  BITCODE_BL flags;
  BITCODE_B title_suppressed;
  BITCODE_B header_suppressed;
  BITCODE_BS flow_direction;
  BITCODE_BD horiz_cell_margin;
  BITCODE_BD vert_cell_margin;
  BITCODE_BL num_cells;
  Dwg_TABLESTYLE_Cell* cells;
} Dwg_Object_TABLESTYLE;

/**
 Class CELLSTYLEMAP (varies)
 R2008+ TABLESTYLE extension class
 */

typedef struct _dwg_CELLSTYLEMAP_Cell
{
  Dwg_CellStyle style;
  BITCODE_BL id;   /* 1=title, 2=header, 3=data, 4=table.
                      ref TABLESTYLE. custom IDs > 100 */
  BITCODE_BL type; /* 1=data, 2=label */
  BITCODE_TV name;
} Dwg_CELLSTYLEMAP_Cell;

typedef struct _dwg_object_CELLSTYLEMAP
{
  BITCODE_BL num_cells;
  Dwg_CELLSTYLEMAP_Cell* cells;
} Dwg_Object_CELLSTYLEMAP;

/* 20.4.103 TABLEGEOMETRY
 r2008+ optional, == 20.4.98
 */

typedef struct _dwg_TABLEGEOMETRY_Cell
{
  BITCODE_BL flag;
  BITCODE_BD width_w_gap;
  BITCODE_BD height_w_gap;
  BITCODE_H unknown;
  BITCODE_BL num_geom_data;
  Dwg_CellContentGeometry *geom_data;
} Dwg_TABLEGEOMETRY_Cell;

typedef struct _dwg_object_TABLEGEOMETRY
{
  BITCODE_BL num_rows;
  BITCODE_BL num_cols;
  BITCODE_BL num_cells; /* = num_rows * num_cols */
  Dwg_TABLEGEOMETRY_Cell *cells;
} Dwg_Object_TABLEGEOMETRY;


/**
 Class UNDERLAYDEFINITION (varies)
 */
typedef struct _dwg_object_UNDERLAYDEFINITION
{
  /* TODO */
  BITCODE_TV filename;
  BITCODE_TV name;
} Dwg_Object_UNDERLAYDEFINITION;

/**
 Class DBCOLOR (varies)
 */
typedef struct _dwg_object_DBCOLOR
{
  char dummy;
  /* TODO */
} Dwg_Object_DBCOLOR;

/**
 Class DICTIONARYVAR (varies)
 */
typedef struct _dwg_object_DICTIONARYVAR
{
  BITCODE_RC intval;
  BITCODE_TV str;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
} Dwg_Object_DICTIONARYVAR;

/**
 Class DICTIONARYWDLFT (varies)
 */
typedef struct _dwg_object_DICTIONARYWDLFT
{
  BITCODE_BL numitems;
  BITCODE_RL cloning_rl; /* r14 only */
  BITCODE_BS cloning;    /* r2000+ */
  BITCODE_RC hard_owner;
  BITCODE_TV* text;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H* itemhandles;
  BITCODE_H defaultid;
} Dwg_Object_DICTIONARYWDLFT;

/**
 Class FIELDLIST AcDbField (varies)
 R18+
 */
typedef struct _dwg_FIELD_ChildValue
{                   /* dxf group code */
  BITCODE_TV key;   /* 6 */
  Dwg_TABLE_value value;
} Dwg_FIELD_ChildValue;

typedef struct _dwg_object_FIELD
{                   /* dxf group code */
  BITCODE_TV id;    /* 1 */
  BITCODE_TV code;  /* 2,3 */
  BITCODE_BL num_childhdl; /* 90 */
  BITCODE_H *childhdl;     /* 360 */
  BITCODE_BL num_objects;  /* 97 */
  BITCODE_H *objects;      /* 331 */
  BITCODE_TV format;       /* 4 */
  BITCODE_BL evaluation_option; /* 91 */
  BITCODE_BL filing_option;     /* 92 */
  BITCODE_BL field_state;       /* 94 */
  BITCODE_BL evaluation_status; /* 95 */
  BITCODE_BL evaluation_error_code; /* 96 */
  BITCODE_TV evaluation_error_msg;  /* 300 */
  Dwg_TABLE_value value;
  BITCODE_TV value_string;        /* 301,9 */
  BITCODE_TV value_string_length; /* 98 */
  BITCODE_BL num_childval;      /* 93 */
  Dwg_FIELD_ChildValue *childval;
} Dwg_Object_FIELD;

/**
 * Object FIELDLIST (varies)
 */
typedef struct
{
  BITCODE_BL num_fields;
  BITCODE_B unknown;
  BITCODE_H * field_handles;
} Dwg_Object_FIELDLIST;

/**
 Class GEODATA (varies)
 R2009+
 */
typedef struct _dwg_GEODATA_meshpt
{
  BITCODE_2RD source_pt;
  BITCODE_2RD dest_pt;
} Dwg_GEODATA_meshpt;

typedef struct _dwg_GEODATA_meshface
{
  BITCODE_BL face1;
  BITCODE_BL face2;
  BITCODE_BL face3;
  BITCODE_BL face4;
} Dwg_GEODATA_meshface;

typedef struct _dwg_object_GEODATA
{                   /* dxf group code */
  BITCODE_BL class_version;
  BITCODE_H host_block;
  BITCODE_BS coord_type; /* 0 unknown, 1 local grid, 2 projected grid,
                            3 geographic defined by latitude/longitude) */
  BITCODE_3BD design_pt;
  BITCODE_3BD ref_pt;
  BITCODE_3BD obs_pt;
  BITCODE_BD unit_scale_horiz;
  BITCODE_BL units_value_horiz;
  BITCODE_BD unit_scale_vert;
  BITCODE_BL units_value_vert;
  BITCODE_3BD up_dir;
  BITCODE_3BD north_dir;
  BITCODE_BL scale_est; /* None = 1, User specified scale factor = 2,
                           Grid scale at reference point = 3, Prismodial = 4 */
  BITCODE_BD user_scale_factor;
  BITCODE_B sea_level_corr;
  BITCODE_BD sea_level_elev;
  BITCODE_BD coord_proj_radius;
  BITCODE_T coord_system_def;
  BITCODE_T geo_rss_tag;
  BITCODE_T coord_system_datum; /* obsolete */
  BITCODE_T coord_system_wkt; /* obsolete */
  
  BITCODE_T observation_from_tag;
  BITCODE_T observation_to_tag;
  BITCODE_T observation_coverage_tag;
  BITCODE_BL num_geomesh_pts;
  Dwg_GEODATA_meshpt *geomesh_pts;
  BITCODE_BL num_geomesh_faces;
  Dwg_GEODATA_meshface *geomesh_faces;

  BITCODE_B has_civil_data;
  BITCODE_B obsolete_false;
  BITCODE_BD north_dir_angle_deg;
  BITCODE_BD north_dir_angle_rad;

} Dwg_Object_GEODATA;

/**
 Class IDBUFFER (varies)
 */
typedef struct _dwg_object_IDBUFFER
{
  BITCODE_RC unknown;
  BITCODE_BL num_obj_ids;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H* obj_ids;
} Dwg_Object_IDBUFFER;

/**
 Classes for IMAGE (varies)
 */

typedef struct _dwg_entity_IMAGE
{
  BITCODE_BL class_version;
  BITCODE_3BD pt0;
  BITCODE_3BD uvec;
  BITCODE_3BD vvec;
  struct
  {
    BITCODE_RD width;
    BITCODE_RD height;
  } size;
  BITCODE_BS display_props;
  BITCODE_B clipping;
  BITCODE_RC brightness;
  BITCODE_RC contrast;
  BITCODE_RC fade;
  BITCODE_B clip_mode;
  BITCODE_BS clip_boundary_type;
  BITCODE_2RD boundary_pt0;
  BITCODE_2RD boundary_pt1;
  BITCODE_BL num_clip_verts;
  BITCODE_2RD* clip_verts;
  BITCODE_H imagedef;
  BITCODE_H imagedefreactor;
} Dwg_Entity_IMAGE;

/**
 Class IMAGEDEF (varies)
 */
typedef struct _dwg_object_IMAGEDEF
{
  BITCODE_BL class_version;
  BITCODE_2RD image_size;
  BITCODE_TV file_path;
  BITCODE_B is_loaded;
  BITCODE_RC resunits;
  BITCODE_2RD pixel_size;
  BITCODE_H parenthandle;
  BITCODE_H null_handle; /* r2010+ */
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
} Dwg_Object_IMAGEDEF;

/**
 Class IMAGEDEF_REACTOR (varies)
 */
typedef struct _dwg_object_IMAGEDEF_REACTOR
{
  BITCODE_BL class_version;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
} Dwg_Object_IMAGEDEF_REACTOR;

/**
 Classes for LAYER_INDEX (varies)
 */
typedef struct _dwg_layer_entry
{
  BITCODE_BL idxlong;
  BITCODE_TV layer;
} Dwg_LAYER_entry;

typedef struct _dwg_object_LAYER_INDEX
{
  BITCODE_BL timestamp1;
  BITCODE_BL timestamp2;
  BITCODE_BL num_entries;
  Dwg_LAYER_entry* entries;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H* entry_handles;
} Dwg_Object_LAYER_INDEX;

/**
 Classes for LWPLINE (77 + varies)
 */
typedef struct _dwg_entity_LWPLINE
{
  BITCODE_BS flag;
  BITCODE_BD const_width;
  BITCODE_BD elevation;
  BITCODE_BD thickness;
  BITCODE_3BD normal;
  BITCODE_BL num_points;
  BITCODE_2RD* points;
  BITCODE_BL num_bulges;
  BITCODE_BD* bulges;
  BITCODE_BL num_widths;
  Dwg_Entity_LWPLINE_width* widths;
} Dwg_Entity_LWPLINE;

/**
 * 20.4.89 Class AcDbObjectContextData (varies)
 * R2010+
 */
typedef struct
{
  BITCODE_BS class_version; /* r2010+ =3 */
  BITCODE_B has_file;
  BITCODE_B defaultflag;
} Dwg_Object_OBJECTCONTEXTDATA;

/**
 Class RASTERVARIABLES (varies)
 (used in conjunction with IMAGE entities)
 */
typedef struct _dwg_object_RASTERVARIABLES
{
  BITCODE_BL class_version;
  BITCODE_BS display_frame;
  BITCODE_BS display_quality;
  BITCODE_BS units;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
} Dwg_Object_RASTERVARIABLES;

/**
 Object SCALE (varies)
 */
typedef struct _dwg_object_SCALE
{
  BITCODE_BS flag;
  BITCODE_TV name;
  BITCODE_BD paper_units;
  BITCODE_BD drawing_units;
  BITCODE_B has_unit_scale;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
} Dwg_Object_SCALE;

/**
 Class SORTENTSTABLE (varies)
 */
typedef struct _dwg_object_SORTENTSTABLE
{
  BITCODE_BL num_ents;
  BITCODE_H* sort_handles;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H owner_handle;
  BITCODE_H* ents;
} Dwg_Object_SORTENTSTABLE;

/**
 Class SPATIAL_FILTER (varies)
 */
typedef struct _dwg_object_SPATIAL_FILTER
{
  BITCODE_BS num_points;
  BITCODE_2RD* points;
  BITCODE_3BD extrusion;
  BITCODE_3BD clip_bound_origin;
  BITCODE_BS display_boundary;
  BITCODE_BS front_clip_on;
  BITCODE_BD front_clip_dist;
  BITCODE_BS back_clip_on;
  BITCODE_BD back_clip_dist;
  BITCODE_BD* inverse_block_transform;
  BITCODE_BD* clip_bound_transform;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
} Dwg_Object_SPATIAL_FILTER;

/**
 Class SPATIAL_INDEX (varies)
 */
typedef struct _dwg_object_SPATIAL_INDEX
{
  BITCODE_BL timestamp1;
  BITCODE_BL timestamp2;
  BITCODE_RC* unknown;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
} Dwg_Object_SPATIAL_INDEX;

/**
 WIPEOUT (varies, 504)
 R2000+, undocumented = IMAGE layover
 */
typedef struct _dwg_entity_WIPEOUT
{
  BITCODE_BL class_version;
  BITCODE_3BD pt0;
  BITCODE_3BD uvec;
  BITCODE_3BD vvec;
  struct
  {
    BITCODE_RD width;
    BITCODE_RD height;
  } size;
  BITCODE_BS display_props;
  BITCODE_B clipping;
  BITCODE_RC brightness;
  BITCODE_RC contrast;
  BITCODE_RC fade;
  BITCODE_B clip_mode;
  BITCODE_BS clip_boundary_type;
  BITCODE_2RD boundary_pt0;
  BITCODE_2RD boundary_pt1;
  BITCODE_BL num_clip_verts;
  BITCODE_2RD* clip_verts;
  BITCODE_H imagedef;
  BITCODE_H imagedefreactor;
} Dwg_Entity_WIPEOUT;

/**
 Class WIPEOUTVARIABLE (varies, 505)
 R2000+, Object bitsize: 96
 */
typedef struct _dwg_object_WIPEOUTVARIABLE
{
  BITCODE_BS display_frame;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
} Dwg_Object_WIPEOUTVARIABLE;

/**
 Class VISUALSTYLE (varies)
 R2007+
 */
typedef struct _dwg_object_VISUALSTYLE
{
  BITCODE_H dictionary; /* (hard-pointer to DICTIONARY_VISUALSTYLE or reverse?) */
} Dwg_Object_VISUALSTYLE;


/**
 Object LIGHTLIST (varies)
 R2010+
 */
typedef struct _dwg_object_LIGHTLIST
{
  BITCODE_H* dictionary; /* (hard-pointer to ACAD_LIGHT dictionary entry) */
  /* TODO */
  BITCODE_BS class_version;
  BITCODE_BS num_lights;
  BITCODE_H  light_handle; /* one for each light */
  BITCODE_TV light_name;   /* one for each light */
} Dwg_Object_LIGHTLIST;


/**
 Unknown Class entity (unused)
 */
typedef struct _dwg_entity_UNKNOWN_ENT
{
  int num_bytes;
  BITCODE_RC *bytes;
  int num_bits;
  BITCODE_B *bits;
} Dwg_Entity_UNKNOWN_ENT;

/**
 Unknown Class object
 */
typedef struct _dwg_object_UNKNOWN_OBJ
{
  int num_bytes;
  BITCODE_RC *bytes;
  int num_bits;
  BITCODE_B *bits;
} Dwg_Object_UNKNOWN_OBJ;

/* OBJECTS - END ************************************************************/

#pragma pack(1)
typedef struct _dwg_entity_eed_data
{
  BITCODE_RC code;
  union eed_data_t {
    struct { /* 0 (1000) string */
      BITCODE_RC length;
      BITCODE_RS codepage;
      char string[1];      /* inlined */
    } eed_0;
    struct { /* R2007+ 0 (1000) string */
      BITCODE_RS length;
      DWGCHAR string[1]; /* inlined */
    } eed_0_r2007;
    struct { /* 1 (1001) invalid */
      char invalid[1];
    } eed_1;
    struct { /* 2 (1002) { or } */
      BITCODE_RC byte;
    } eed_2;
    struct { /* 3 (1003) layer */
      BITCODE_RL layer;
    } eed_3;
    struct { /* 4 (1004) binary */
      BITCODE_RC length;
      char data[1];
    } eed_4;
    struct { /* 5 (1005) entity */
      BITCODE_RL entity;
    } eed_5;
    struct { /* 10-13 point */
      BITCODE_3RD point;
    } eed_10;
    struct { /* 40-42 real */
      BITCODE_RD real;
    } eed_40;
    struct { /* 70 short int */
      BITCODE_RS rs;
    } eed_70;
    struct { /* 71 long int */
      BITCODE_RL rl;
    } eed_71;
  } u;
} Dwg_Eed_Data;
#pragma pack()

typedef struct _dwg_entity_eed
{
  BITCODE_BS size;
  Dwg_Handle handle;
  Dwg_Eed_Data *data;
  char* raw; /* a copy of data */
} Dwg_Eed;

/**
 Structure for common entity attributes
 */
typedef struct _dwg_object_entity
{
  struct _dwg_object* object;
  union
  {
    Dwg_Entity_UNUSED *UNUSED;
    Dwg_Entity_TEXT *TEXT;
    Dwg_Entity_ATTRIB *ATTRIB;
    Dwg_Entity_ATTDEF *ATTDEF;
    Dwg_Entity_BLOCK *BLOCK;
    Dwg_Entity_ENDBLK *ENDBLK;
    Dwg_Entity_SEQEND *SEQEND;
    Dwg_Entity_INSERT *INSERT;
    Dwg_Entity_MINSERT *MINSERT;
    Dwg_Entity_VERTEX_2D *VERTEX_2D;
    Dwg_Entity_VERTEX_3D *VERTEX_3D;
    Dwg_Entity_VERTEX_MESH *VERTEX_MESH;
    Dwg_Entity_VERTEX_PFACE *VERTEX_PFACE;
    Dwg_Entity_VERTEX_PFACE_FACE *VERTEX_PFACE_FACE;
    Dwg_Entity_POLYLINE_2D *POLYLINE_2D;
    Dwg_Entity_POLYLINE_3D *POLYLINE_3D;
    Dwg_Entity_ARC *ARC;
    Dwg_Entity_CIRCLE *CIRCLE;
    Dwg_Entity_LINE *LINE;
    struct _dwg_entity_DIMENSION_common *DIMENSION_common;
    Dwg_Entity_DIMENSION_ORDINATE *DIMENSION_ORDINATE;
    Dwg_Entity_DIMENSION_LINEAR *DIMENSION_LINEAR;
    Dwg_Entity_DIMENSION_ALIGNED *DIMENSION_ALIGNED;
    Dwg_Entity_DIMENSION_ANG3PT *DIMENSION_ANG3PT;
    Dwg_Entity_DIMENSION_ANG2LN *DIMENSION_ANG2LN;
    Dwg_Entity_DIMENSION_RADIUS *DIMENSION_RADIUS;
    Dwg_Entity_DIMENSION_DIAMETER *DIMENSION_DIAMETER;
    Dwg_Entity_POINT *POINT;
    Dwg_Entity__3DFACE *_3DFACE;
    Dwg_Entity_POLYLINE_PFACE *POLYLINE_PFACE;
    Dwg_Entity_POLYLINE_MESH *POLYLINE_MESH;
    Dwg_Entity_SOLID *SOLID;
    Dwg_Entity_TRACE *TRACE;
    Dwg_Entity_SHAPE *SHAPE;
    Dwg_Entity_VIEWPORT *VIEWPORT;
    Dwg_Entity_ELLIPSE *ELLIPSE;
    Dwg_Entity_SPLINE *SPLINE;
    Dwg_Entity_3DSOLID *_3DSOLID;
    Dwg_Entity_REGION *REGION;
    Dwg_Entity_BODY *BODY;
    Dwg_Entity_RAY *RAY;
    Dwg_Entity_XLINE *XLINE;
    Dwg_Entity_OLEFRAME *OLEFRAME;
    Dwg_Entity_MTEXT *MTEXT;
    Dwg_Entity_LEADER *LEADER;
    Dwg_Entity_TOLERANCE *TOLERANCE;
    Dwg_Entity_MLINE *MLINE;
    Dwg_Entity_OLE2FRAME *OLE2FRAME;
    Dwg_Entity_DUMMY *DUMMY;
    Dwg_Entity_LONG_TRANSACTION *LONG_TRANSACTION;
    Dwg_Entity_LWPLINE *LWPLINE;
    Dwg_Entity_MULTILEADER *MULTILEADER;
    Dwg_Entity_PROXY_LWPOLYLINE *PROXY_LWPOLYLINE;
    Dwg_Entity_PROXY_ENTITY *PROXY_ENTITY;
    Dwg_Entity_HATCH *HATCH;
    Dwg_Entity_TABLE *TABLE;
    Dwg_Entity_IMAGE *IMAGE;
    Dwg_Entity_WIPEOUT *WIPEOUT;
    Dwg_Entity_UNKNOWN_ENT *UNKNOWN_ENT;
  } tio;

  BITCODE_RL bitsize;
  /* Dwg_Handle handle; */

  unsigned int num_eed;
  Dwg_Eed *eed; /* see also Dwg_Resbuf* xdata */

  BITCODE_B picture_exists;
  BITCODE_BLL picture_size; /* before r2007 only RL */
  BITCODE_RC *picture;

  BITCODE_BB entity_mode;
  BITCODE_BL num_reactors;
  BITCODE_B xdic_missing_flag;  /* r2004+ */
  BITCODE_B has_ds_binary_data; /* r2013+ */
  BITCODE_B isbylayerlt;        /* r13-r14 */
  BITCODE_B nolinks;
  BITCODE_CMC color;
  BITCODE_BD linetype_scale;
  BITCODE_BB linetype_flags;    /* r2000+ */
  BITCODE_BB plotstyle_flags;   /* r2000+ */
  BITCODE_BB material_flags;    /* r2007+ */
  BITCODE_RC shadow_flags;      /* r2007+ */
  BITCODE_B has_full_visualstyle; /* r2010+ */
  BITCODE_B has_face_visualstyle; /* r2010+ */
  BITCODE_B has_edge_visualstyle; /* r2010+ */
  BITCODE_BS invisible;
  BITCODE_RC lineweight;        /* r2000+ */

  /* preR13 entity fields: */
  BITCODE_RC flag_r11;
  BITCODE_RS type_r11;
  BITCODE_RS opts_r11;
  BITCODE_RC extra_r11;
  BITCODE_RS layer_r11;
  BITCODE_RC color_r11;
  BITCODE_RS linetype_r11;
  /* preR13 in the obj: eed, elevation/pt.z, thickness, paper */

  /* XXX I think this field is deprecated due to
     the new handle parsing functions */
  unsigned int num_handles;
  
  /* Common Entity Handle Data */
  BITCODE_H subentity;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H prev_entity;  /* r13-r2000 */
  BITCODE_H next_entity;  /* r13-r2000 */
  BITCODE_H color_handle; /* r2004+ */
  BITCODE_H layer;
  BITCODE_H ltype;
  BITCODE_H material;     /* r2007+ */
  BITCODE_H shadow;       /* r2007+ */
  BITCODE_H plotstyle;    /* r2000+ */
  BITCODE_H full_visualstyle; /* r2010+ */
  BITCODE_H face_visualstyle;
  BITCODE_H edge_visualstyle;
} Dwg_Object_Entity;

/**
 Structure for ordinary object attributes
 */
typedef struct _dwg_object_object
{
  struct _dwg_object* object;
  union
  {
    Dwg_Object_DICTIONARY *DICTIONARY;
    Dwg_Object_DICTIONARYWDLFT *DICTIONARYWDLFT;
    Dwg_Object_BLOCK_CONTROL *BLOCK_CONTROL;
    Dwg_Object_BLOCK_HEADER *BLOCK_HEADER;
    Dwg_Object_LAYER_CONTROL *LAYER_CONTROL;
    Dwg_Object_LAYER *LAYER;
    Dwg_Object_SHAPEFILE_CONTROL *SHAPEFILE_CONTROL;
    Dwg_Object_SHAPEFILE *SHAPEFILE;
    Dwg_Object_LTYPE_CONTROL *LTYPE_CONTROL;
    Dwg_Object_LTYPE *LTYPE;
    Dwg_Object_VIEW_CONTROL *VIEW_CONTROL;
    Dwg_Object_VIEW *VIEW;
    Dwg_Object_UCS_CONTROL *UCS_CONTROL;
    Dwg_Object_UCS *UCS;
    Dwg_Object_VPORT_CONTROL *VPORT_CONTROL;
    Dwg_Object_VPORT *VPORT;
    Dwg_Object_APPID_CONTROL *APPID_CONTROL;
    Dwg_Object_APPID *APPID;
    Dwg_Object_DIMSTYLE_CONTROL *DIMSTYLE_CONTROL;
    Dwg_Object_DIMSTYLE *DIMSTYLE;
    Dwg_Object_VP_ENT_HDR_CONTROL *VP_ENT_HDR_CONTROL;
    Dwg_Object_VP_ENT_HDR *VP_ENT_HDR;
    Dwg_Object_GROUP *GROUP;
    Dwg_Object_MLINESTYLE *MLINESTYLE;
    Dwg_Object_DICTIONARYVAR *DICTIONARYVAR;
    Dwg_Object_LAYER_INDEX *LAYER_INDEX;
    Dwg_Object_LAYOUT *LAYOUT;
    Dwg_Object_PROXY *PROXY;
    Dwg_Object_CELLSTYLEMAP *CELLSTYLEMAP;
    Dwg_Object_FIELD *FIELD;
    Dwg_Object_FIELDLIST *FIELDLIST;
    Dwg_Object_GEODATA *GEODATA;
    Dwg_Object_IDBUFFER *IDBUFFER;
    Dwg_Object_IMAGEDEF *IMAGEDEF;
    Dwg_Object_IMAGEDEF_REACTOR *IMAGEDEF_REACTOR;
    Dwg_Object_LIGHTLIST *LIGHTLIST;
    Dwg_Object_MLEADERSTYLE *MLEADERSTYLE;
    Dwg_Object_OBJECTCONTEXTDATA *OBJECTCONTEXTDATA;
    Dwg_Object_RASTERVARIABLES *RASTERVARIABLES;
    Dwg_Object_SCALE *SCALE;
    Dwg_Object_SORTENTSTABLE *SORTENTSTABLE;
    Dwg_Object_SPATIAL_FILTER *SPATIAL_FILTER;
    Dwg_Object_SPATIAL_INDEX *SPATIAL_INDEX;
    Dwg_Object_TABLECONTENT *TABLECONTENT;
    Dwg_Object_TABLEGEOMETRY *TABLEGEOMETRY;
    Dwg_Object_XRECORD *XRECORD;
    Dwg_Object_PLACEHOLDER *PLACEHOLDER;
    Dwg_Object_VBA_PROJECT *VBA_PROJECT;
    Dwg_Object_WIPEOUTVARIABLE *WIPEOUTVARIABLE;
    Dwg_Object_VISUALSTYLE *VISUALSTYLE;
    Dwg_Object_UNKNOWN_OBJ *UNKNOWN_OBJ;
  } tio;

  BITCODE_RL bitsize;
  long unsigned int datpos; /* the data stream offset */
  /* Dwg_Handle handle; */
  unsigned int num_eed;
  Dwg_Eed *eed;

  /* TODO: should these be removed? */
  BITCODE_BL num_reactors;      /* r13-r14 */
  BITCODE_B xdic_missing_flag;  /* r2004+ */
  BITCODE_B has_ds_binary_data; /* r2013+ */

  unsigned int num_handles;
  Dwg_Handle *handleref;
} Dwg_Object_Object;

/**
 General object struct
 */
typedef struct _dwg_object
{
  unsigned int size;
  unsigned int type;
  /* unsigned int ckr; */
  unsigned int index;

  BITCODE_B  has_strings;       /* r2007+ */
  BITCODE_MC handlestream_size; /* r2010+ in bits */

  Dwg_Object_Supertype supertype;
  union
  {
    Dwg_Object_Entity *entity;
    Dwg_Object_Object *object;
    unsigned char *unknown; /* i.e. unhandled class as raw bits */
  } tio;

  char *dxfname;
  BITCODE_RL bitsize;
  unsigned long hdlpos;
  Dwg_Handle handle;
  struct _dwg_struct *parent;
} Dwg_Object;

/**
 Struct for classes
 */
typedef struct _dwg_class
{
  BITCODE_BS number;
  /* see http://images.autodesk.com/adsk/files/autocad_2012_pdf_dxf-reference_enu.pdf */
  BITCODE_BS proxyflag;
  char *appname;
  char *cppname;
  char *dxfname;
  BITCODE_TU dxfname_u;
  BITCODE_B  wasazombie; /* really Was-a-proxy flag */
  BITCODE_BS item_class_id; /* Is-an-entity. 1f2 for entities, 1f3 for objects */
  BITCODE_BL instance_count; /* 91 Instance count for a custom class */
  BITCODE_BL dwg_version;
  BITCODE_BL maint_version;
  BITCODE_BL unknown_1;
  BITCODE_BL unknown_2;
} Dwg_Class;

/**
 Dwg_Chain same as Bit_Chain in "bits.h"
 */
typedef struct _dwg_chain
{
  unsigned char *chain;
  long unsigned int size;
  long unsigned int byte;
  unsigned char bit;
} Dwg_Chain;

typedef enum DWG_SECTION_TYPE /* since r2004+ */
{
  SECTION_HEADER = 1,			/* AcDb:Header */
  SECTION_AUXHEADER = 2,		/* AcDb:AuxHeader */
  SECTION_CLASSES = 3,			/* AcDb:Classes */
  SECTION_HANDLES = 4,			/* AcDb:Handles */
  SECTION_TEMPLATE = 5,			/* AcDb:Template */
  SECTION_OBJFREESPACE = 6,		/* AcDb:ObjFreeSpace */
  SECTION_OBJECTS = 7,			/* AcDb:AcDbObjects */
  SECTION_REVHISTORY = 8,		/* AcDb:RevHistory */
  SECTION_SUMMARYINFO = 9,		/* AcDb:SummaryInfo */
  SECTION_PREVIEW = 10,			/* AcDb:Preview */
  SECTION_APPINFO = 11, 		/* AcDb:AppInfo */
  SECTION_APPINFOHISTORY = 12,		/* AcDb:AppInfoHistory */
  SECTION_FILEDEPLIST = 13,		/* AcDb:FileDepList */
  SECTION_SECURITY,      		/* AcDb:Security, if stored with a password */
  SECTION_VBAPROJECT,    		/* AcDb:VBAProject */
  SECTION_SIGNATURE,     		/* AcDb:Signature */
  SECTION_PROTOTYPE,      		/* AcDb:AcDsPrototype_1b */
  SECTION_UNKNOWN,
} Dwg_Section_Type;

typedef enum DWG_SECTION_TYPE_R13
{
  SECTION_HEADER_R13 = 0,
  SECTION_CLASSES_R13 = 1,
  SECTION_OBJECTS_R13 = 2,
  SECTION_2NDHEADER_R13 = 3,
  SECTION_MEASUREMENT_R13 = 4,
  SECTION_AUXHEADER_R2000 = 5,
} Dwg_Section_Type_R13;

typedef enum DWG_SECTION_TYPE_R11
{
  SECTION_HEADER_R11 = 0,
  SECTION_BLOCK = 1,
  SECTION_LAYER = 2,
  SECTION_STYLE = 3,
  SECTION_LTYPE = 5,
  SECTION_VIEW  = 6,
  SECTION_UCS   = 7,
  SECTION_VPORT = 8,
  SECTION_APPID = 9,
  SECTION_DIMSTYLE = 10,
  SECTION_VP_ENT_HDR = 11,
} Dwg_Section_Type_r11;

typedef struct _dwg_section
{
  long number; /* preR13: count of entries */
  BITCODE_RL address;
  BITCODE_RL size;
  BITCODE_RL parent;
  BITCODE_RL left;
  BITCODE_RL right;
  BITCODE_RL x00;
  Dwg_Section_Type type; /* to be casted to Dwg_Section_Type_r11 preR13 */
  /* => section_info? */
  BITCODE_TV name;
  /* r2004 section fields: */
  BITCODE_RL section_type;
  BITCODE_RL decomp_data_size;
  BITCODE_RL comp_data_size;
  BITCODE_RL compression_type;
  BITCODE_RL checksum;
} Dwg_Section;

/* Dwg_R2007_Section:
  int64_t  data_size;    // max size of page
  int64_t  max_size;
  int64_t  encrypted;
  int64_t  hashcode;
  int64_t  name_length;  // 0x22
  int64_t  unknown;      // 0x00
  int64_t  encoded;
  int64_t  num_pages;
  DWGCHAR *name;
  r2007_section_page **pages;
  struct _r2007_section *next;
 */

typedef struct
{
  BITCODE_RL size;
  BITCODE_RL pagecount;
  BITCODE_RL num_sections;
  BITCODE_RL max_decomp_size;
  BITCODE_RL unknown2;
  BITCODE_RL compressed; /* Compressed (1 = no, 2 = yes, normally 2) */
  BITCODE_RL type;
  BITCODE_RL encrypted; /* (0 = no, 1 = yes, 2 = unknown) */
  char name[64];
  Dwg_Section **sections;
} Dwg_Section_Info;

/**
 Main DWG struct
 */
typedef struct _dwg_struct
{
  struct Dwg_Header
  {
    unsigned int version; /* see Dwg_Version_Type */
    unsigned int from_version;
    BITCODE_RC   zero_5[5];
    BITCODE_RC   is_maint;
    BITCODE_RC   zero_one_or_three;
    BITCODE_RS   unknown_s[3];         /* <R13 */
    BITCODE_RL   preview_addr;
    BITCODE_RC   dwg_version;
    BITCODE_RC   maint_version;
    BITCODE_RS   codepage;
    BITCODE_RC   unknown_0;            /* R2004+ */
    BITCODE_RC   app_dwg_version;      /* R2004+ */
    BITCODE_RC   app_maint_version;    /* R2004+ */
    BITCODE_RL   security_type;        /* R2004+ */
    BITCODE_RL   rl_1c_address;        /* R2004+ */
    BITCODE_RL   summary_info_address; /* R2004+ */
    BITCODE_RL   vba_proj_address;     /* R2004+ */
    BITCODE_RL   rl_28_80;             /* R2004+ */
    BITCODE_RL   num_sections;
    Dwg_Section* section;
    BITCODE_RL   num_infos;
    Dwg_Section_Info* section_info;
  } header;

  struct Dwg_R2004_Header /* encrypted */
    {
      char file_ID_string[12];
      BITCODE_RL header_offset;
      BITCODE_RL header_size;
      BITCODE_RL x04;
      BITCODE_RL root_tree_node_gap;
      BITCODE_RL lowermost_left_tree_node_gap;
      BITCODE_RL lowermost_right_tree_node_gap;
      BITCODE_RL unknown_long;
      BITCODE_RL last_section_id;
      BITCODE_RLL last_section_address;
      BITCODE_RLL second_header_address;
      BITCODE_RL gap_amount;
      BITCODE_RL section_amount;
      BITCODE_RL x20;
      BITCODE_RL x80;
      BITCODE_RL x40;
      BITCODE_RL section_map_id;
      BITCODE_RLL section_map_address;
      BITCODE_RL section_info_id;
      BITCODE_RL section_array_size;
      BITCODE_RL gap_array_size;
      BITCODE_RL CRC;
      char padding[12];
      /* System Section: Section Page Map */
      BITCODE_RL section_type; /* 0x4163043b */
      BITCODE_RL decomp_data_size;
      BITCODE_RL comp_data_size;
      BITCODE_RL compression_type;
      BITCODE_RL checksum;
  } r2004_header;

  /* #define DWG_AUXHEADER_SIZE 123 */
  struct Dwg_AuxHeader
  {
    BITCODE_RC   aux_intro[3]; /* ff 77 01 */
    BITCODE_RS   dwg_version;
    BITCODE_RS   maint_version;
    BITCODE_RL   num_saves;
    BITCODE_RL   minus_1;
    BITCODE_RS   num_saves_1;
    BITCODE_RS   num_saves_2;
    BITCODE_RL   zero;
    BITCODE_RS   dwg_version_1;
    BITCODE_RS   maint_version_1;
    BITCODE_RS   dwg_version_2;
    BITCODE_RS   maint_version_2;
    BITCODE_RS   unknown_rs[6];
    BITCODE_RC   unknown_rc[20]; /* some vars */
    BITCODE_RD   TDCREATE; /* ?? format TD */
    BITCODE_RD   TDUPDATE;
    BITCODE_RL   HANDSEED;
    BITCODE_RL   plot_stamp;
    BITCODE_RS   zero_1;
    BITCODE_RS   num_saves_3;
    BITCODE_RL   zero_2;
    BITCODE_RL   zero_3;
    BITCODE_RL   zero_4;
    BITCODE_RL   num_saves_4;
    BITCODE_RL   zero_5;
    BITCODE_RL   zero_6;
    BITCODE_RL   zero_7;
    BITCODE_RL   zero_8; /* ?? */
    BITCODE_RS   zero_18[3]; /* R2018+ */
  } auxheader;

  Dwg_Chain picture;

  Dwg_Header_Variables header_vars;

  unsigned int num_classes;
  Dwg_Class * dwg_class;

  long unsigned int num_objects;
  Dwg_Object * object;

  long unsigned int num_entities;
  long unsigned int num_object_refs;
  Dwg_Object_Ref ** object_ref;

  long unsigned int num_layers;
  Dwg_Object * layer_control;
  Dwg_Object * mspace_block;
  Dwg_Object * pspace_block;
  Dwg_Object_APPID_CONTROL * appid_control;

  struct _dwg_second_header {
    BITCODE_RL size;
    BITCODE_RL address;
    char version[12];
    BITCODE_B null_b[4];
    BITCODE_RC unknown_10;
    BITCODE_RC unknown_rc4[4];
    BITCODE_RC num_sections;
    struct _sections
    {
      BITCODE_RC nr;
      BITCODE_BL address;
      BITCODE_BL size;
    } section[6];
    BITCODE_BS num_handlers;
    struct _handler
    {
      BITCODE_RC size;
      BITCODE_RC nr;
      BITCODE_RC *data;
    } handlers[16];
    BITCODE_RL junk_r14_1; /* r14 only */
    BITCODE_RL junk_r14_2; /* r14 only */
  } second_header;

  long unsigned int measurement;
  unsigned int layout_number;
} Dwg_Data;

/*--------------------------------------------------
 * Functions
 */

int
dwg_read_file(char *filename, Dwg_Data * dwg);

#ifdef USE_WRITE
int
dwg_write_file(char *filename, Dwg_Data * dwg_data);
#endif

unsigned char*
dwg_bmp(Dwg_Data *, BITCODE_RL *);

double
dwg_model_x_min(Dwg_Data *);
double
dwg_model_x_max(Dwg_Data *);
double
dwg_model_y_min(Dwg_Data *);
double
dwg_model_y_max(Dwg_Data *);
double
dwg_model_z_min(Dwg_Data *);
double
dwg_model_z_max(Dwg_Data *);
double
dwg_page_x_min(Dwg_Data *);
double
dwg_page_x_max(Dwg_Data *);
double
dwg_page_y_min(Dwg_Data *);
double
dwg_page_y_max(Dwg_Data *);

unsigned int
dwg_get_layer_count(Dwg_Data *);

Dwg_Object_LAYER**
dwg_get_layers(Dwg_Data *);

long unsigned int
dwg_get_object_count(Dwg_Data *dwg);

long unsigned int
dwg_get_object_object_count(Dwg_Data *dwg);

int
dwg_class_is_entity(Dwg_Class *klass);

long unsigned int
dwg_get_entity_count(Dwg_Data *);

Dwg_Object_Entity **
dwg_get_entities(Dwg_Data *);

Dwg_Object_LAYER *
dwg_get_entity_layer(Dwg_Object_Entity *);

Dwg_Object*
dwg_next_object(Dwg_Object* obj);

Dwg_Object*
dwg_ref_get_object(Dwg_Object_Ref* ref);

Dwg_Object*
get_first_owned_object(Dwg_Object* hdr_obj, Dwg_Object_BLOCK_HEADER* hdr);

Dwg_Object*
get_next_owned_object(Dwg_Object* hdr_obj, Dwg_Object* current, Dwg_Object_BLOCK_HEADER* hdr);

Dwg_Section_Type
dwg_section_type(DWGCHAR *wname);

void
dwg_free(Dwg_Data * dwg);

#ifdef __cplusplus
}
#endif

#endif
