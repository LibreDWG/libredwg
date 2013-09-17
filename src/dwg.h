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
 */

#ifndef DWG_H
#define DWG_H

#ifdef __cplusplus
//extern "C"
//  {
#endif

#define BITCODE_DOUBLE double

#define BITCODE_RC char
#define FORMAT_RC "%2x"
#define BITCODE_MC long int
#define FORMAT_MC "%l"
#define BITCODE_MS long unsigned int
#define FORMAT_MS "%lu"
#define BITCODE_B unsigned char
#define FORMAT_B "%d"
#define BITCODE_BB unsigned char
#define FORMAT_BB "%d"
#define BITCODE_BS unsigned int
#define FORMAT_BS "%d"
#define BITCODE_RS unsigned int
#define FORMAT_RS "%d" 
#define BITCODE_RL long unsigned int
#define FORMAT_RL "%lu"
#define BITCODE_RD BITCODE_DOUBLE
#define FORMAT_RD "%f"
#define BITCODE_BL long unsigned int
#define FORMAT_BL "%lu"
#define BITCODE_TV unsigned char *
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

#define BITCODE_T BITCODE_TV //TODO: implement version dependant string parsing

#define DWGCHAR wchar_t

typedef struct _dwg_bitecode_2rd
{
  BITCODE_RD x;
  BITCODE_RD y;
} Dwg_Bitcode_2RD;

typedef struct _dwg_bitecode_2bd
{
  BITCODE_BD x;
  BITCODE_BD y;
} Dwg_Bitcode_2BD;

typedef struct _dwg_bitecode_3rd
{
  BITCODE_RD x;
  BITCODE_RD y;
  BITCODE_RD z;
} Dwg_Bitcode_3RD;

typedef struct _dwg_bitecode_3bd
{
  BITCODE_BD x;
  BITCODE_BD y;
  BITCODE_BD z;
} Dwg_Bitcode_3BD;

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
 Object types that exist in dwg-files.
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
  //DWG_TYPE_<UNKNOWN> = 0x09,
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
  //DWG_TYPE_<UNKNOWN> = 0x2b,
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
  //DWG_TYPE_<UNKNOWN> = 0x36,
  //DWG_TYPE_<UNKNOWN> = 0x37,
  DWG_TYPE_LTYPE_CONTROL = 0x38,
  DWG_TYPE_LTYPE = 0x39,
  //DWG_TYPE_<UNKNOWN> = 0x3a,
  //DWG_TYPE_<UNKNOWN> = 0x3b,
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
  //DWG_TYPE_<UNKNOWN> = 0x4a
  //DWG_TYPE_<UNKNOWN> = 0x4b
  //DWG_TYPE_<UNKNOWN> = 0x4c
  DWG_TYPE_LWPLINE = 0x4d,
  DWG_TYPE_HATCH = 0x4e,
  DWG_TYPE_XRECORD = 0x4f,
  DWG_TYPE_PLACEHOLDER = 0x50,
  //DWG_TYPE_<UNKNOWN> = 0x51,
  DWG_TYPE_LAYOUT = 0x52
} Dwg_Object_Type;

/**
 Struct for handles.
 */
typedef struct _dwg_handle
{
  unsigned char code;
  unsigned char size;
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
typedef struct _dwg_color
{
  unsigned int index;
  long unsigned int rgb;
  unsigned char byte;
  char* name;
  char* book_name;
} Dwg_Color;

struct _dwg_binary_chunk
{
  short size;
  char *data;
};

/**
 Struct for result buffers
 */
typedef struct _dwg_resbuf
{
  short type;
  union 
  {
    char   *str;
    double  pt[3];
    char    i8;
    short   i16;
    int     i32;    
    double  dbl;
    unsigned char hdl[8];
    struct _dwg_binary_chunk chunk;
  } value;
  struct _dwg_resbuf *next;
} Dwg_Resbuf;

/**
 struct for dwg header
 */

typedef struct _dwg_header_variables {
  BITCODE_RL bitsize;
  BITCODE_BD unknown_0;
  BITCODE_BD unknown_1;
  BITCODE_BD unknown_2;
  BITCODE_BD unknown_3;
  BITCODE_TV unknown_4;
  BITCODE_TV unknown_5;
  BITCODE_TV unknown_6;
  BITCODE_TV unknown_7;
  BITCODE_BL unknown_8;
  BITCODE_BL unknown_9;
  BITCODE_BS unknown_10;
  BITCODE_H current_viewport_entity_header;
  BITCODE_B DIMASO;
  BITCODE_B DIMSHO;
  BITCODE_B DIMSAV; //undocumented
  BITCODE_B PLINEGEN;
  BITCODE_B ORTHOMODE;
  BITCODE_B REGENMODE;
  BITCODE_B FILLMODE;
  BITCODE_B QTEXTMODE;
  BITCODE_B PSLTSCALE;
  BITCODE_B LIMCHECK;
  BITCODE_B BLIPMODE;
  BITCODE_B unknown_11;
  BITCODE_B user_timer_onoff;
  BITCODE_B SKPOLY;
  BITCODE_B ANGDIR;
  BITCODE_B SPLFRAME;
  BITCODE_B ATTREQ;
  BITCODE_B ATTDIA;
  BITCODE_B MIRRTEXT;
  BITCODE_B WORLDVIEW;
  BITCODE_B WIREFRAME; //Undocumented
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
  BITCODE_BD VTEXTSIZE;
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
  BITCODE_TV MENUNAME;
  BITCODE_BL TDCREATE_JULIAN_DAY;
  BITCODE_BL TDCREATE_MILLISECONDS;
  BITCODE_BL TDUPDATE_JULIAN_DAY;
  BITCODE_BL TDUPDATE_MILLISECONDS;
  BITCODE_BL unknown_15;
  BITCODE_BL unknown_16;
  BITCODE_BL unknown_17;
  BITCODE_BL TDINDWG_DAYS;
  BITCODE_BL TDINDWG_MILLISECONDS;
  BITCODE_BL TDUSRTIMER_DAYS;
  BITCODE_BL TDUSRTIMER_MILLISECONDS;
  BITCODE_CMC CECOLOR;
  BITCODE_H HANDSEED;
  BITCODE_H CLAYER;
  BITCODE_H TEXTSTYLE;
  BITCODE_H CELTYPE;
  BITCODE_H CMATERIAL;
  BITCODE_H DIMSTYLE;
  BITCODE_H CMLSTYLE;
  BITCODE_BD PSVPSCALE;
  BITCODE_3BD INSBASE_PSPACE;
  BITCODE_3BD EXTMIN_PSPACE;
  BITCODE_3BD EXTMAX_PSPACE;
  BITCODE_2RD LIMMIN_PSPACE;
  BITCODE_2RD LIMMAX_PSPACE;
  BITCODE_BD ELEVATION_PSPACE;
  BITCODE_3BD UCSORG_PSPACE;
  BITCODE_3BD UCSXDIR_PSPACE;
  BITCODE_3BD UCSYDIR_PSPACE;
  BITCODE_H UCSNAME_PSPACE;
  BITCODE_H PUCSBASE;
  BITCODE_BS PUCSORTHOVIEW;
  BITCODE_H PUCSORTHOREF;
  BITCODE_3BD PUCSORGTOP;
  BITCODE_3BD PUCSORGBOTTOM;
  BITCODE_3BD PUCSORGLEFT;
  BITCODE_3BD PUCSORGRIGHT;
  BITCODE_3BD PUCSORGFRONT;
  BITCODE_3BD PUCSORGBACK;
  BITCODE_3BD INSBASE_MSPACE;
  BITCODE_3BD EXTMIN_MSPACE;
  BITCODE_3BD EXTMAX_MSPACE;
  BITCODE_2RD LIMMIN_MSPACE;
  BITCODE_2RD LIMMAX_MSPACE;
  BITCODE_BD ELEVATION_MSPACE;
  BITCODE_3BD UCSORG_MSPACE;
  BITCODE_3BD UCSXDIR_MSPACE;
  BITCODE_3BD UCSYDIR_MSPACE;
  BITCODE_H UCSNAME_MSPACE;
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
  BITCODE_RC DIMALTD;
  BITCODE_RC DIMZIN;
  BITCODE_B DIMSD1;
  BITCODE_B DIMSD2;
  BITCODE_RC DIMTOLJ;
  BITCODE_RC DIMJUST;
  BITCODE_RC DIMFIT;
  BITCODE_B DIMUPT;
  BITCODE_RC DIMTZIN;
  BITCODE_RC DIMMALTZ;
  BITCODE_RC DIMMALTTZ;
  BITCODE_RC DIMTAD;
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
  BITCODE_BD DIMMTP;
  BITCODE_BD DIMMTM;
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
  BITCODE_T DIMPOST_T;
  BITCODE_T DIMAPOST_T;
  BITCODE_T DIMBLK_T;
  BITCODE_T DIMBLK1_T;
  BITCODE_T DIMBLK2_T;
  BITCODE_BD DIMALTRND;
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
  BITCODE_B DIMFXLON;
  BITCODE_H DIMTXTSTY;
  BITCODE_H DIMLDRBLK;
  BITCODE_H DIMBLK;
  BITCODE_H DIMBLK1;
  BITCODE_H DIMBLK2;
  BITCODE_H DIMLTYPE;
  BITCODE_H DIMLTEX1;
  BITCODE_H DIMLTEX2;
  BITCODE_BS DIMLWD;
  BITCODE_BS DIMLWE;
  BITCODE_H BLOCK_CONTROL_OBJECT;
  BITCODE_H LAYER_CONTROL_OBJECT;
  BITCODE_H STYLE_CONTROL_OBJECT;
  BITCODE_H LINETYPE_CONTROL_OBJECT;
  BITCODE_H VIEW_CONTROL_OBJECT;
  BITCODE_H UCS_CONTROL_OBJECT;
  BITCODE_H VPORT_CONTROL_OBJECT;
  BITCODE_H APPID_CONTROL_OBJECT;
  BITCODE_H DIMSTYLE_CONTROL_OBJECT;
  BITCODE_H VIEWPORT_ENTITY_HEADER_CONTROL_OBJECT;
  BITCODE_H DICTIONARY_ACAD_GROUP;
  BITCODE_H DICTIONARY_ACAD_MLINESTYLE;
  BITCODE_H DICTIONARY_NAMED_OBJECTS;
  BITCODE_BS unknown_18;
  BITCODE_BS unknown_19;
  BITCODE_TV HYPERLINKBASE;
  BITCODE_TV STYLESHEET;
  BITCODE_H DICTIONARY_LAYOUTS;
  BITCODE_H DICTIONARY_PLOTSETTINGS;
  BITCODE_H DICTIONARY_PLOTSTYLES;
  BITCODE_H DICTIONARY_MATERIALS;
  BITCODE_H DICTIONARY_COLORS;
  BITCODE_H DICTIONARY_VISUALSTYLE;
  BITCODE_BL FLAGS;
  BITCODE_BS INSUNITS;
  BITCODE_BS CEPSNTYPE;
  BITCODE_H CPSNID;
  BITCODE_TV FINGERPRINTGUID;
  BITCODE_TV VERSIONGUID;
  BITCODE_RC SORTENTS;
  BITCODE_RC IDEXCTL;
  BITCODE_RC HIDETEXT;
  BITCODE_RC XCLIPFRAME;
  BITCODE_RC DIMASSOC;
  BITCODE_RC HALOGAP;
  BITCODE_BS OBSCUREDCOLOR;
  BITCODE_BS INTERSECTIONCOLOR;
  BITCODE_RC OBSCUREDLTYPE;
  BITCODE_RC INTERSECTIONDISPLAY;
  BITCODE_TV PROJECTNAME;
  BITCODE_H BLOCK_RECORD_PAPER_SPACE;
  BITCODE_H BLOCK_RECORD_MODEL_SPACE;
  BITCODE_H LTYPE_BYLAYER;
  BITCODE_H LTYPE_BYBLOCK;
  BITCODE_H LTYPE_CONTINUOUS;
  BITCODE_B unknown_20;
  BITCODE_BL unknown_21;
  BITCODE_BL unknown_22;
  BITCODE_BD unknown_23;
  BITCODE_BD unknown_24;
  BITCODE_BD unknown_25;
  BITCODE_BD unknown_26;
  BITCODE_BD unknown_27;
  BITCODE_BD unknown_28;
  BITCODE_RC unknown_29;
  BITCODE_RC unknown_30;
  BITCODE_BD unknown_31;
  BITCODE_BD unknown_32;
  BITCODE_BD unknown_33;
  BITCODE_BD unknown_34;
  BITCODE_BD unknown_35;
  BITCODE_BD unknown_36;
  BITCODE_BS unknown_37;
  BITCODE_RC unknown_38;
  BITCODE_BD unknown_39;
  BITCODE_BD unknown_40;
  BITCODE_BD unknown_41;
  BITCODE_BL unknown_42;
  BITCODE_RC unknown_43;
  BITCODE_RC unknown_44;
  BITCODE_RC unknown_45;
  BITCODE_RC unknown_46;
  BITCODE_B unknown_47;
  BITCODE_CMC unknown_48;
  BITCODE_H unknown_49;
  BITCODE_H unknown_50;
  BITCODE_H unknown_51;
  BITCODE_RC unknown_52;
  BITCODE_BD unknown_53;
  BITCODE_BS unknown_54;
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
  BITCODE_RD rotation_ang;
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
  BITCODE_RD rotation_ang;
  BITCODE_RD height;
  BITCODE_RD width_factor;
  BITCODE_TV text_value;
  BITCODE_BS generation;
  BITCODE_BS horiz_alignment;
  BITCODE_BS vert_alignment;
  BITCODE_RC dataflags;
  BITCODE_TV tag;
  BITCODE_BS field_length;
  BITCODE_RC flags;
  BITCODE_B lock_position_flag;
  BITCODE_H style;
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
  BITCODE_RD rotation_ang;
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
  BITCODE_BD        rotation_ang;
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
  BITCODE_BD        rotation_ang;
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
  BITCODE_RC flags;
  BITCODE_3BD point;
  BITCODE_BD start_width;
  BITCODE_BD end_width;
  BITCODE_BD bulge;
  BITCODE_BD tangent_dir;
} Dwg_Entity_VERTEX_2D;

/**
 Struct for VERTEX_3D (11)
 */
typedef struct _dwg_entity_VERTEX_3D
{
  BITCODE_RC flags;
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
  BITCODE_BS flags;
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
  BITCODE_RC flags_1;
  BITCODE_RC flags_2;
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
    BITCODE_3BD extrusion; \
    BITCODE_2RD text_midpt; \
    struct \
      { \
        BITCODE_BD ecs_11; \
        BITCODE_BD ecs_12; \
      } elevation; \
    BITCODE_RC flags_1; \
    BITCODE_TV user_text; \
    BITCODE_BD text_rot; \
    BITCODE_BD horiz_dir; \
    BITCODE_3BD ins_scale; \
    BITCODE_BD ins_rotation; \
    BITCODE_BS attachment_point; \
    BITCODE_BS lspace_style; \
    BITCODE_BD lspace_factor; \
    BITCODE_BD act_measurement; \
    BITCODE_B unknown; \
    BITCODE_B flip_arrow1; \
    BITCODE_B flip_arrow2;

/**
 Struct for ordinate dimension - DIMENSION_ORDINATE (20)
 */
typedef struct _dwg_entity_DIMENSION_ORDINATE
{
  DIMENSION_COMMON
  BITCODE_2RD _12_pt;
  BITCODE_3BD _10_pt;
  BITCODE_3BD _13_pt;
  BITCODE_3BD _14_pt;
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
  BITCODE_2RD _12_pt;
  BITCODE_3BD _13_pt;
  BITCODE_3BD _14_pt;
  BITCODE_3BD _10_pt;
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
  BITCODE_2RD _12_pt;
  BITCODE_3BD _13_pt;
  BITCODE_3BD _14_pt;
  BITCODE_3BD _10_pt;
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
  BITCODE_2RD _12_pt;
  BITCODE_3BD _10_pt;
  BITCODE_3BD _13_pt;
  BITCODE_3BD _14_pt;
  BITCODE_3BD _15_pt;
  BITCODE_H dimstyle;
  BITCODE_H block;
} Dwg_Entity_DIMENSION_ANG3PT;

/**
 Struct for angular 2 line dimension - DIMENSION_ANG2LN (24)
 */
typedef struct _dwg_entity_DIMENSION_ANG2LN
{
  DIMENSION_COMMON
  BITCODE_2RD _12_pt;
  BITCODE_2RD _16_pt;
  BITCODE_3BD _13_pt;
  BITCODE_3BD _14_pt;
  BITCODE_3BD _15_pt;
  BITCODE_3BD _10_pt;
  BITCODE_H dimstyle;
  BITCODE_H block;
} Dwg_Entity_DIMENSION_ANG2LN;

/**
 Struct for radius dimension - DIMENSION_RADIUS (25)
 */
typedef struct _dwg_entity_DIMENSION_RADIUS
{
  DIMENSION_COMMON
  BITCODE_2RD _12_pt;
  BITCODE_3BD _10_pt;
  BITCODE_3BD _15_pt;
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
  BITCODE_2RD _12_pt;
  BITCODE_3BD _15_pt;
  BITCODE_3BD _10_pt;
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
typedef struct _dwg_entity_3D_FACE
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
  BITCODE_BS flags;
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
  BITCODE_BL status_flags;
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
  BITCODE_B use_def_lights;
  BITCODE_RC def_lighting_type;
  BITCODE_BD brightness;
  BITCODE_BD contrast;
  BITCODE_CMC ambient_light_color;
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
  BITCODE_BS scenario;
  BITCODE_BS degree;
  BITCODE_BD fit_tol;
  BITCODE_3BD beg_tan_vec;
  BITCODE_3BD end_tan_vec;
  BITCODE_B rational;
  BITCODE_B closed_b;
  BITCODE_B periodic;
  BITCODE_B weighted;
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
  struct _dwg_entity_3DSOLID* extra_acis_data;//is it the best approach?
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
 Struct for DICTIONARYWDLFT (varies)
 */
typedef struct _dwg_object_DICTIONARYWDLFT
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
  BITCODE_H defaultid;
} Dwg_Object_DICTIONARYWDLFT;

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
  BITCODE_BD extends_ht; //spec typo?
  BITCODE_BD extends_wid;
  BITCODE_TV text;
  BITCODE_BS linespace_style;
  BITCODE_BD linespace_factor;
  BITCODE_B unknown_bit;
  BITCODE_BL unknown_long;
  BITCODE_H style;
} Dwg_Entity_MTEXT;

/**
 Struct for LEADER (45)
 */
typedef struct _dwg_entity_LEADER
{
  BITCODE_B unknown_bit_1; //always seems to be zero
  BITCODE_BS annot_type;
  BITCODE_BS path_type;
  BITCODE_BL numpts;
  BITCODE_3DPOINT* points;
  BITCODE_3DPOINT end_pt_proj;
  BITCODE_3DPOINT extrusion;
  BITCODE_3DPOINT x_direction;
  BITCODE_3DPOINT offset_to_block_ins_pt;
  BITCODE_3DPOINT unknown_pt;
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
} Dwg_Entity_MLINE_line;

typedef struct _dwg_entity_MLINE_vert
{
  BITCODE_3BD vertex;
  BITCODE_3BD vertex_direction;
  BITCODE_3BD miter_direction;
  Dwg_Entity_MLINE_line* lines;
} Dwg_Entity_MLINE_vert;

typedef struct _dwg_entity_MLINE
{
  BITCODE_BD scale;
  BITCODE_RC just;
  BITCODE_3BD base_point;
  BITCODE_3BD extrusion;
  BITCODE_BS open_closed;
  BITCODE_RC num_lines; //Linesinstyle
  BITCODE_BS num_verts;
  Dwg_Entity_MLINE_vert* verts;
  BITCODE_H mline_style;
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
  int __iterator;
  BITCODE_TV entry_name;
  BITCODE_B _64_flag;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xdep;
  BITCODE_B anonymous;
  BITCODE_B hasattrs;
  BITCODE_B blkisxref;
  BITCODE_B xrefoverlaid;
  BITCODE_B loaded_bit;
  BITCODE_BL owned_object_count;
  BITCODE_3DPOINT base_pt;
  BITCODE_TV xref_pname;
  BITCODE_RL insert_count;
  BITCODE_TV block_description;
  BITCODE_BL size_of_preview_data;
  BITCODE_RC* binary_preview_data;
  BITCODE_BS insert_units;
  BITCODE_B explodable;
  BITCODE_RC block_scaling;
  BITCODE_H block_control_handle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H NULL_handle;
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
  BITCODE_TV entry_name;
  BITCODE_B _64_flag;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;
  BITCODE_B frozen;
  BITCODE_B on;
  BITCODE_B frozen_in_new;
  BITCODE_B locked;
  BITCODE_BS values;
  BITCODE_CMC color;
  BITCODE_H layer_control;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H null_handle;
  BITCODE_H plotstyle;
  BITCODE_H material;
  BITCODE_H linetype;
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
 */
typedef struct _dwg_object_SHAPEFILE
{
  BITCODE_TV entry_name;
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

// 54 and 55 are UNKNOWN OBJECTS

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
  BITCODE_TV entry_name;
  BITCODE_B _64_flag;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;
  BITCODE_TV description;
  BITCODE_BD pattern_len;
  BITCODE_RC alignment;
  BITCODE_RC num_dashes;
  LTYPE_dash* dash;
  BITCODE_RC* strings_area;
  BITCODE_H linetype_control;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H null_handle;
  BITCODE_H* shapefiles;
} Dwg_Object_LTYPE;

// 58 and 59 are UNKNOWN OBJECTS

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
  BITCODE_TV entry_name;
  BITCODE_B _64_flag;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;
  BITCODE_BD height;
  BITCODE_BD width;
  BITCODE_2RD center;
  BITCODE_3BD target;
  BITCODE_3BD direction;
  BITCODE_BD twist_angle;
  BITCODE_BD lens_legth;
  BITCODE_BD front_clip;
  BITCODE_BD back_clip;
  BITCODE_4BITS view_mode;
  BITCODE_RC render_mode;
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
  BITCODE_H base_ucs_handle;
  BITCODE_H named_ucs_handle;
  BITCODE_H live_section;
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
  BITCODE_TV entry_name;
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
  BITCODE_TV entry_name;
  BITCODE_B _64_flag;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;
  BITCODE_BD view_height;
  BITCODE_BD aspect_ratio;
  BITCODE_2RD view_center;
  BITCODE_3BD view_target;
  BITCODE_3BD view_dir;
  BITCODE_BD view_twist;
  BITCODE_BD lens_length;
  BITCODE_BD front_clip;
  BITCODE_BD back_clip;
  BITCODE_4BITS view_mode;
  BITCODE_RC render_mode;
  BITCODE_B use_default_lights;
  BITCODE_RC default_lightining_type;
  BITCODE_BD brightness;
  BITCODE_BD contrast;
  BITCODE_CMC ambient_color;
  BITCODE_2RD lower_left;
  BITCODE_2RD upper_right;
  BITCODE_B UCSFOLLOW;
  BITCODE_BS circle_zoom;
  BITCODE_B fast_zoom;
  BITCODE_B UCSICON_0;
  BITCODE_B UCSICON_1;
  BITCODE_B grid_on_off;
  BITCODE_2RD grid_spacing;
  BITCODE_B snap_on_off;
  BITCODE_B snap_style;
  BITCODE_BS snap_isopair;
  BITCODE_BD snap_rot;
  BITCODE_2RD snap_base;
  BITCODE_2RD snap_spacing;
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
  BITCODE_TV entry_name;
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
  BITCODE_H unknown_handle;  /*This is not stated in the spec*/
  BITCODE_H null_handle;
  BITCODE_H xdicobjhandle;
  BITCODE_H* dimstyles;
} Dwg_Object_DIMSTYLE_CONTROL;

/**
 Struct for DIMSTYLE (69)
 */
typedef struct _dwg_object_DIMSTYLE
{
  BITCODE_TV entry_name;
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
  BITCODE_RC DIMALTD;
  BITCODE_RC DIMZIN;
  BITCODE_B DIMSD1;
  BITCODE_B DIMSD2;
  BITCODE_RC DIMTOLJ;
  BITCODE_RC DIMJUST;
  BITCODE_RC DIMFIT;
  BITCODE_B DIMUPT;
  BITCODE_RC DIMTZIN;
  BITCODE_RC DIMMALTZ;
  BITCODE_RC DIMMALTTZ;
  BITCODE_RC DIMTAD;
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
  BITCODE_T DIMPOST_T;
  BITCODE_T DIMAPOST_T;
  BITCODE_T DIMBLK_T;
  BITCODE_T DIMBLK1_T;
  BITCODE_T DIMBLK2_T;
  BITCODE_TV DIMPOST;
  BITCODE_TV DIMAPOST;
  BITCODE_BD DIMALTRND;  
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
  BITCODE_B DIMFXLON;
  BITCODE_H DIMTXTSTY;
  BITCODE_H DIMLDRBLK;
  BITCODE_H DIMBLK;
  BITCODE_H DIMBLK1;
  BITCODE_H DIMBLK2;
  BITCODE_H DIMLTYPE;
  BITCODE_H DIMLTEX1;
  BITCODE_H DIMLTEX2;
  BITCODE_BS DIMLWD;
  BITCODE_BS DIMLWE;
  BITCODE_B unknown;

  BITCODE_H dimstyle_control;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H null_handle;
  BITCODE_H shapefile;

  BITCODE_H leader_block;
  BITCODE_H dimblk;
  BITCODE_H dimblk1;
  BITCODE_H dimblk2;

  BITCODE_H dimltype;
  BITCODE_H dimltex1;
  BITCODE_H dimltex2;
} Dwg_Object_DIMSTYLE;

/**
 Struct for VP_ENT_HDR_CONTROL (70)
 */
typedef struct _dwg_object_VP_ENT_HDR_CONTROL
{
  BITCODE_BS num_entries;
  BITCODE_H null_handle;
  BITCODE_H xdicobjhandle;
  BITCODE_H* viewport_entity_headers;
} Dwg_Object_VP_ENT_HDR_CONTROL;

/**
 Struct for VP_ENT_HDR (71)
 */
typedef struct _dwg_object_VP_ENT_HDR
{
  BITCODE_TV entry_name;
  BITCODE_B _64_flag;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;
  BITCODE_B one_flag;
  BITCODE_H vp_ent_ctrl;
  BITCODE_H xdicobjhandle;
  BITCODE_H null;

} Dwg_Object_VP_ENT_HDR;

/**
 Struct for GROUP (72)
 */
typedef struct _dwg_object_GROUP
{
  BITCODE_TV str;
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
typedef struct _dwg_object_MLINESTYLE_line
{
  BITCODE_BD offset;
  BITCODE_CMC color;
  BITCODE_BS ltindex;
} Dwg_Object_MLINESTYLE_line;

typedef struct _dwg_object_MLINESTYLE
{
  BITCODE_TV name;
  BITCODE_TV desc;
  BITCODE_BS flags;
  BITCODE_CMC fillcolor;
  BITCODE_BD startang;
  BITCODE_BD endang;
  BITCODE_RC linesinstyle;
  Dwg_Object_MLINESTYLE_line* lines;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;

} Dwg_Object_MLINESTYLE;

/**
 Struct for DICTIONARYVAR (varies)
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
 Structs for HATCH (varies)
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
//BITCODE_2RD center
  BITCODE_2RD endpoint;
  BITCODE_BD minor_major_ratio;
//BITCODE_BD start_angle;
//BITCODE_BD end_angle;
//BITCODE_B is_ccw;
  /* SPLINE */
  BITCODE_BL degree;
  BITCODE_B is_rational;
  BITCODE_B is_periodic;
  BITCODE_BL num_knots;
  BITCODE_BL num_control_points;
  BITCODE_BD* knots;
  Dwg_Entity_HATCH_ControlPoint* control_points;
} Dwg_Entity_HATCH_PathSeg;


typedef struct _dwg_entity_HATCH_polylinepath
{
  BITCODE_2RD point;
  BITCODE_BD bulge;
} Dwg_Entity_HATCH_PolylinePath;

typedef struct _dwg_entity_HATCH_path
{
  BITCODE_BL flag;
  BITCODE_BL num_path_segs;
  Dwg_Entity_HATCH_PathSeg* segs;

  /*POLYLINE PATH:*/
  BITCODE_B bulges_present;
  BITCODE_B closed;
  //BITCODE_BL num_path_segs;
  Dwg_Entity_HATCH_PolylinePath* polyline_paths;
  BITCODE_BL num_boundary_obj_handles;
} Dwg_Entity_HATCH_Path;

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
  BITCODE_BD z_coord; //x,y always 0.0
  BITCODE_3BD extrusion;
  BITCODE_TV name;
  BITCODE_B solid_fill;
  BITCODE_B associative;
  BITCODE_BL num_paths;
  Dwg_Entity_HATCH_Path* paths;
} Dwg_Entity_HATCH;

/**
 Struct for IDBUFFER (varies)
 */
typedef struct _dwg_object_IDBUFFER
{
  BITCODE_RC unknown;
  BITCODE_BL num_ids;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H* obj_ids;
} Dwg_Object_IDBUFFER;

/**
 Struct for IMAGE (varies)
 */
typedef struct _dwg_entity_IMAGE_clip_vert
{
  double x;
  double y;
} Dwg_Entity_IMAGE_clip_vert;

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
  BITCODE_BS clip_boundary_type;
  BITCODE_2RD boundary_pt0;
  BITCODE_2RD boundary_pt1;
  BITCODE_BL num_clip_verts;
  Dwg_Entity_IMAGE_clip_vert* clip_verts;
  BITCODE_H imagedef;
  BITCODE_H imagedefreactor;
} Dwg_Entity_IMAGE;

/**
 Struct for IMAGEDEF (varies)
 */
typedef struct _dwg_object_IMAGEDEF
{
  BITCODE_BL class_version;
  BITCODE_2RD image_size;
  BITCODE_TV file_path;
  BITCODE_B is_loaded;
  BITCODE_RC resunits;
  BITCODE_2RD pixel_size;
  BITCODE_H parent_handle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
} Dwg_Object_IMAGEDEF;

/**
 Struct for IMAGEDEFREACTOR (varies)
 */
typedef struct _dwg_object_IMAGEDEFREACTOR
{
  BITCODE_BL class_version;
  BITCODE_H parent_handle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
} Dwg_Object_IMAGEDEFREACTOR;

/**
 Struct for LAYER_INDEX (varies)
 */
typedef struct _dwg_layer_entry
{
  BITCODE_BL index_long;
  BITCODE_TV index_str;
} Dwg_LAYER_entry;

typedef struct _dwg_object_LAYER_INDEX
{
  BITCODE_BL timestamp1;
  BITCODE_BL timestamp2;
  BITCODE_BL num_entries;
  Dwg_LAYER_entry* entries;
  BITCODE_H parent_handle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H* entry_handles;
} Dwg_Object_LAYER_INDEX;

/**
 Struct for LAYOUT (varies)
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
  BITCODE_BS flags;
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
  BITCODE_RL viewport_count;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H plot_view_handle;
  BITCODE_H visual_style_handle;
  BITCODE_H associated_paperspace_block_record_handle;
  BITCODE_H last_active_viewport_handle;
  BITCODE_H base_ucs_handle;
  BITCODE_H named_ucs_handle;
  BITCODE_H* viewport_handles;

} Dwg_Object_LAYOUT;

/**
 Structures for LWPLINE
 */

typedef struct _dwg_entity_LWPLINE_width
{
  BITCODE_BD start;
  BITCODE_BD end;
} Dwg_Entity_LWPLINE_width;

typedef struct _dwg_entity_LWPLINE
{
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
} Dwg_Entity_LWPLINE;

/**
 Struct for OLE2FRAME
 */
typedef struct _dwg_entity_OLE2FRAME
{
  BITCODE_BS flags;
  BITCODE_BS mode;
  BITCODE_BL data_length;
  BITCODE_RC* data;
  BITCODE_RC unknown;
} Dwg_Entity_OLE2FRAME;

/**
 Struct for PROXY (varies)
 */
typedef struct _dwg_object_PROXY
{
  BITCODE_BL class_id;
  BITCODE_BL object_drawing_format;
  BITCODE_B original_data_format;
  BITCODE_RC* data;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H* objid_object_handles;
} Dwg_Object_PROXY;

/**
 Struct for PLACEHOLDER (varies)
 */
typedef struct _dwg_object_PLACEHOLDER
{
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
} Dwg_Object_PLACEHOLDER;

/**
 Struct for RASTERVARIABLES (varies)
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
 Struct for SORTENTSTABLE (varies)
 */
typedef struct _dwg_object_SORTENTSTABLE
{
  BITCODE_BL num_entries;
  BITCODE_H* sort_handles;
  BITCODE_H parenthandle;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H owner_handle;
  BITCODE_H* object_handles;
} Dwg_Object_SORTENTSTABLE;

/**
 Struct for SPATIAL_FILTER (varies)
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
 Struct for SPATIAL_INDEX (varies)
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
 Structs for TABLE (varies)
 */

typedef struct _dwg_entity_TABLE_cell
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
  BITCODE_BL flags_2007;
  BITCODE_BL data_type;
  BITCODE_BL data_size;
  BITCODE_BL data_long;
  BITCODE_BD data_double;
  BITCODE_TV data_string;
  BITCODE_RC* data_date;
  BITCODE_2RD data_point;
  BITCODE_3RD data_3dpoint;
  BITCODE_BL unknown2;
  BITCODE_TV format_string;
  BITCODE_TV value_string;
  BITCODE_H cell_handle;
  BITCODE_H* attr_def_id;
  BITCODE_H text_style_override;
} Dwg_Entity_TABLE_Cell;

typedef struct _dwg_entity_TABLE
{
  BITCODE_3BD insertion_point;
  BITCODE_3BD scale;
  BITCODE_BB data_flags;
  BITCODE_BD rotation;
  BITCODE_3BD extrusion;
  BITCODE_B has_attribs;
  BITCODE_BL owned_object_count;
  BITCODE_BS flag_for_table_value;
  BITCODE_3BD horiz_direction;
  BITCODE_BL num_cols;
  BITCODE_BL num_rows;
  BITCODE_BD* col_widths;
  BITCODE_BD* row_heights;
  Dwg_Entity_TABLE_Cell* cells; 
  BITCODE_B table_overrides_present;
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
  BITCODE_BD title_row_height;
  BITCODE_BD header_row_height;
  BITCODE_BD data_row_height;

  BITCODE_B border_color_overrides_present;
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

  BITCODE_B border_lineweight_overrides_present;
  BITCODE_BL border_lineweight_overrides_flag;
  BITCODE_BS title_horiz_top_lineweigh;
  BITCODE_BS title_horiz_ins_lineweigh;
  BITCODE_BS title_horiz_bottom_lineweigh;
  BITCODE_BS title_vert_left_lineweigh;
  BITCODE_BS title_vert_ins_lineweigh;
  BITCODE_BS title_vert_right_lineweigh;
  BITCODE_BS header_horiz_top_lineweigh;
  BITCODE_BS header_horiz_ins_lineweigh;
  BITCODE_BS header_horiz_bottom_lineweigh;
  BITCODE_BS header_vert_left_lineweigh;
  BITCODE_BS header_vert_ins_lineweigh;
  BITCODE_BS header_vert_right_lineweigh;
  BITCODE_BS data_horiz_top_lineweigh;
  BITCODE_BS data_horiz_ins_lineweigh;
  BITCODE_BS data_horiz_bottom_lineweigh;
  BITCODE_BS data_vert_left_lineweigh;
  BITCODE_BS data_vert_ins_lineweigh;
  BITCODE_BS data_vert_right_lineweigh;

  BITCODE_B border_visibility_overrides_present;
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
} Dwg_Entity_TABLE;

//NOT SURE ABOUT THIS ONE (IS IT OBJECT OR ENTITY?):
/**
 Struct for VBA_PROJECT (varies)
 */
typedef struct _dwg_object_VBA_PROJECT
{
  char dummy;
//TODO 
} Dwg_Object_VBA_PROJECT;

//NOT SURE ABOUT THIS ONE (IS IT OBJECT OR ENTITY?):
/**
 Struct for WIPEOUTVARIABLE (varies)
 */
typedef struct _dwg_object_WIPEOUTVARIABLE
{
  char dummy;
//TODO 
} Dwg_Object_WIPEOUTVARIABLE;

/**
 Struct for XRECORD (varies)
 */
typedef struct _dwg_object_XRECORD
{
  BITCODE_BL numdatabytes;
  BITCODE_BS cloning_flags;
  Dwg_Resbuf* rbuf;
  BITCODE_H parent;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H* objid_handles;
} Dwg_Object_XRECORD;

/* OBJECTS - END ************************************************************/

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
    Dwg_Entity_MTEXT *MTEXT;
    Dwg_Entity_LEADER *LEADER;
    Dwg_Entity_TOLERANCE *TOLERANCE;
    Dwg_Entity_MLINE *MLINE;
    Dwg_Entity_IMAGE *IMAGE;
    Dwg_Entity_LWPLINE *LWPLINE;
    Dwg_Entity_OLE2FRAME *OLE2FRAME;
    Dwg_Entity_TABLE *TABLE;
    Dwg_Entity_HATCH *HATCH;
  } tio;

  BITCODE_RL bitsize;
  //Dwg_Handle handle;

  BITCODE_BS extended_size;
  Dwg_Handle extended_handle;
  BITCODE_RC *extended;

  BITCODE_B picture_exists;
  BITCODE_BL picture_size;
  BITCODE_RC *picture;

  BITCODE_BB entity_mode;
  BITCODE_BL num_reactors;
  BITCODE_B xdic_missing_flag;
  BITCODE_B isbylayerlt;
  BITCODE_B nolinks;
  BITCODE_CMC color;
  BITCODE_BD linetype_scale;
  BITCODE_BB linetype_flags;
  BITCODE_BB plotstyle_flags;
  BITCODE_BB material_flags;
  BITCODE_RC shadow_flags;
  BITCODE_BS invisible;
  BITCODE_RC lineweight;

  //XXX I think this field is deprecated due to
  //the new handle parsing functions
  unsigned int num_handles;

  //Common Entity Handle Data
  BITCODE_H subentity;
  BITCODE_H* reactors;
  BITCODE_H xdicobjhandle;
  BITCODE_H prev_entity;
  BITCODE_H next_entity;
  BITCODE_H layer;
  BITCODE_H ltype;
  BITCODE_H plotstyle;
  BITCODE_H material;

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
    Dwg_Object_DICTIONARYWDLFT *DICTIONARYWDLFT;
    Dwg_Object_IDBUFFER *IDBUFFER;
    Dwg_Object_IMAGEDEF *IMAGEDEF;
    Dwg_Object_IMAGEDEFREACTOR *IMAGEDEFREACTOR;
    Dwg_Object_LAYER_INDEX *LAYER_INDEX;
    Dwg_Object_LAYOUT *LAYOUT;
    Dwg_Object_PROXY *PROXY;
    Dwg_Object_PLACEHOLDER *PLACEHOLDER;
    Dwg_Object_RASTERVARIABLES *RASTERVARIABLES;
    Dwg_Object_SORTENTSTABLE *SORTENTSTABLE;
    Dwg_Object_SPATIAL_FILTER *SPATIAL_FILTER;
    Dwg_Object_SPATIAL_INDEX *SPATIAL_INDEX;
    Dwg_Object_XRECORD *XRECORD;
    Dwg_Object_VBA_PROJECT *VBA_PROJECT;
    Dwg_Object_WIPEOUTVARIABLE *WIPEOUTVARIABLE;
  } tio;

  long unsigned int bitsize;
  //Dwg_Handle handle;
  unsigned int extended_size;
  Dwg_Handle extended_handle;
  unsigned char *extended;

//TODO: should these be removed?
  long unsigned int num_reactors;
  unsigned char xdic_missing_flag;
  //		unsigned char picture_exists;

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
  unsigned int ckr;
  unsigned int index;

  Dwg_Object_Supertype supertype;
  union
  {
    Dwg_Object_Entity *entity;
    Dwg_Object_Object *object;
    unsigned char *unknown;
  } tio;

  Dwg_Handle handle;

  struct _dwg_struct *parent;

} Dwg_Object;

/**
 Struct for classes
 */
typedef struct _dwg_class
{
  unsigned int number;
  unsigned int version;
  unsigned char *appname;
  unsigned char *cppname;
  unsigned char *dxfname;
  unsigned char wasazombie;
  unsigned int item_class_id;
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

typedef struct _dwg_section
{
  int number;
  unsigned int address;
  unsigned int size;
	int parent;
	int left;
	int right;
	int x00;
} Dwg_Section;

typedef struct
{ 
  long unsigned int size;
  long unsigned int unknown1;	   
  long unsigned int num_sections;  
  long unsigned int max_decomp_size;  
  long unsigned int unknown2;	   
  long unsigned int compressed;
  long unsigned int type;
  long unsigned int encrypted;
  char name[64];
  Dwg_Section **sections;
} Dwg_Section_Info;

typedef enum DWG_SECTION_TYPE
{
  SECTION_HEADER = 0x01,
  SECTION_AUXHEADER = 0x02,
  SECTION_CLASSES = 0x03,
  SECTION_HANDLES = 0x04,
  SECTION_TEMPLATE = 0x05,
  SECTION_OBJFREESPACE = 0x06,
  SECTION_DBOBJECTS = 0x07,
  SECTION_REVHISTORY = 0x08,
  SECTION_SUMMARYINFO = 0x09,
  SECTION_PREVIEW = 0x0a,
  SECTION_APPINFO = 0x0b,
  SECTION_APPINFOHISTORY = 0x0c,
  SECTION_FILEDEPLIST = 0x0d,
  SECTION_SECURITY,      // 
  SECTION_VBAPROJECT,    // not seen
  SECTION_SIGNATURE      //
} Dwg_Section_Type;

/**
 Main DWG struct
 */
typedef struct _dwg_struct
{
  struct
  {
    unsigned int version;
    unsigned int codepage;
    unsigned int num_sections;
    Dwg_Section* section;
    unsigned int num_descriptions;
    Dwg_Section_Info* section_info;
  } header;

#		define DWG_UNKNOWN1_SIZE 123
  Dwg_Chain unknown1;

  Dwg_Chain picture;

  Dwg_Header_Variables header_vars;
  unsigned int num_classes;
  Dwg_Class * dwg_class;

  long unsigned int num_objects;
  Dwg_Object *object;

  long unsigned int num_layers;
  long unsigned int num_entities;
  //Dwg_Object *layer;

  long unsigned int num_object_refs;
  Dwg_Object_Ref ** object_ref;

  Dwg_Object * layer_control;

  struct
  {
    unsigned char unknown[6];
    struct
    {
      int size;
      unsigned char chain[4];
    } handlerik[14];
  } second_header;

  long unsigned int measurement;

  unsigned int dwg_ot_layout;

  struct _bit_chain *bit_chain;

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

void
dwg_free(Dwg_Data * dwg);
unsigned char*
dwg_bmp(Dwg_Data *, long *);

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
dwg_get_entity_count(Dwg_Data *);

Dwg_Object_Entity **
dwg_get_entities(Dwg_Data *);

Dwg_Object_LAYER *
dwg_get_entity_layer(Dwg_Object_Entity *);

Dwg_Object*
dwg_next_object(Dwg_Object* obj);

int
dwg_get_object(Dwg_Object* obj, Dwg_Object_Ref* ref);

Dwg_Object*
get_first_owned_object(Dwg_Object* hdr_obj, Dwg_Object_BLOCK_HEADER* hdr);

Dwg_Object*
get_next_owned_object(Dwg_Object* hdr_obj, Dwg_Object* current, Dwg_Object_BLOCK_HEADER* hdr);

void
dwg_print_object(Dwg_Object *obj);


double dwg_page_x_min(Dwg_Data *dwg);
double dwg_page_x_max(Dwg_Data *dwg);
double dwg_page_y_min(Dwg_Data *dwg);
double dwg_page_y_max(Dwg_Data *dwg);

#ifdef __cplusplus
//}
#endif

#endif
