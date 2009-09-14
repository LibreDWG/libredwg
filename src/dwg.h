/*****************************************************************************/
/*  LibreDWG - Free DWG library                                              */
/*  http://code.google.com/p/libredwg/                                       */
/*                                                                           */
/*    based on LibDWG - Free DWG read-only library                           */
/*    http://sourceforge.net/projects/libdwg                                 */
/*    originally written by Felipe Castro <felipo at users.sourceforge.net>  */
/*                                                                           */
/*  Copyright (C) 2008, 2009 Free Software Foundation, Inc.                  */
/*  Copyright (C) 2009 Rodrigo Rodrigues da Silva <rodrigopitanga@gmail.com> */
/*  Copyright (C) 2009 Felipe Sanches <jucablues@users.sourceforge.net>      */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either versionn 3 of the License, or (at your option) any later versionn.*/
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/// Main include-file for the library.

#ifndef DWG_H
#define DWG_H

#ifdef __cplusplus
extern "C"
{
#endif


/**
 Object supertypes that exist in dwg-files.
 */
	typedef enum DWG_OBJECT_SUPERTYPE
	{
		DWG_SUPERTYPE_UNKNOWN,
		DWG_SUPERTYPE_ENTITY,
		DWG_SUPERTYPE_OBJECT
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
		DWG_TYPE_STYLE_CONTROL = 0x34,
		DWG_TYPE_STYLE = 0x35,
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
		DWG_TYPE_FREMDA = 0x50
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
		unsigned char dataflags;
		double elevation;
		double x0;
		double y0;
		struct
		{
			double x;
			double y;
			unsigned int h;
			unsigned int v;
		} alignment;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
		double thickness;
		double oblique_ang;
		double rotation_ang;
		double height;
		double width_factor;
		unsigned char *text;
		unsigned int generation;
    unsigned int horiz_align;
    unsigned int vert_align;
    Dwg_Object_Ref* style;
	} Dwg_Entity_TEXT;

/**
 Struct for ATTRIB (2)
 */
	typedef struct _dwg_entity_ATTRIB
	{
		unsigned char dataflags;
		double elevation;
		double x0;
		double y0;
		struct
		{
			double x;
			double y;
			unsigned int h;
			unsigned int v;
		} alignment;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
		double thickness;
		double oblique_ang;
		double rotation_ang;
		double height;
		double width_factor;
		unsigned char *text;
		unsigned int generation;
                unsigned int horiz_align;
                unsigned int vert_align;
		unsigned char *tag;
		unsigned int field_length;
		unsigned char flags;
		unsigned char lock_position_flag;
	    Dwg_Object_Ref* style;
	} Dwg_Entity_ATTRIB;

/**
 Struct for ATTDEF (3)
 */
	typedef struct _dwg_entity_ATTDEF
	{
		unsigned char dataflags;
		double elevation;
		double x0;
		double y0;
		struct
		{
			double x;
			double y;
			unsigned int h;
			unsigned int v;
		} alignment;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
		double thickness;
		double oblique_ang;
		double rotation_ang;
		double height;
		double width_factor;
		unsigned char *text;
		unsigned int generation;
                unsigned int horiz_align;
                unsigned int vert_align;
		unsigned char *tag;
		unsigned int field_length;
		unsigned char flags;
		unsigned char lock_position_flag;
		unsigned char *prompt;
	    Dwg_Object_Ref* style;
	} Dwg_Entity_ATTDEF;

/**
 Struct for BLOCK (4)
 */
	typedef struct _dwg_entity_BLOCK
	{
		unsigned char *name;
	} Dwg_Entity_BLOCK;

/**
 Struct for ENDBLK (5)
 */
	typedef struct _dwg_entity_ENDBLK
	{
	} Dwg_Entity_ENDBLK;

/**
 Struct for SEQEND (6)
 */
	typedef struct _dwg_entity_SEQEND
	{
	} Dwg_Entity_SEQEND;

/**
 Struct for INSERT (7)
 */
	typedef struct _dwg_entity_INSERT
	{
		double x0;
		double y0;
		double z0;
		unsigned char scale_flag;
		struct
		{
			double x;
			double y;
			double z;
		} scale;
		double rotation_ang;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
		unsigned char has_attribs;
		long unsigned int owned_obj_count;
		Dwg_Object_Ref* block_header;
		Dwg_Object_Ref* first_attrib;
		Dwg_Object_Ref* last_attrib;
		Dwg_Object_Ref** attrib_handles;
		Dwg_Object_Ref* seqend;
	} Dwg_Entity_INSERT;

/**
 Struct for MINSERT (8)
 */
	typedef struct _dwg_entity_MINSERT
	{
		double x0;
		double y0;
		double z0;
		unsigned char scale_flag;
		struct
		{
			double x;
			double y;
			double z;
		} scale;
		double rotation_ang;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
		unsigned char has_attribs;
		long unsigned int owned_obj_count;
		struct
		{
			unsigned int size;
			double dx;
		} column;
		struct
		{
			unsigned int size;
			double dy;
		} line;
		Dwg_Object_Ref* block_header;
                Dwg_Object_Ref* first_attrib;
                Dwg_Object_Ref* last_attrib;
                Dwg_Object_Ref** attrib_handles;
                Dwg_Object_Ref* seqend;
	} Dwg_Entity_MINSERT;

/**
 Struct for VERTEX_2D (10)
 */
	typedef struct _dwg_entity_VERTEX_2D
	{
		unsigned char flags;
		double x0;
		double y0;
		double z0;
		double start_width;
		double end_width;
		double bulge;
		double tangent_dir;
	} Dwg_Entity_VERTEX_2D;

/**
 Struct for VERTEX_3D (11)
 */
	typedef struct _dwg_entity_VERTEX_3D
	{
		unsigned char flags;
		double x0;
		double y0;
		double z0;
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
		unsigned int vertind[4];
	} Dwg_Entity_VERTEX_PFACE_FACE;

/**
 Struct for 2D POLYLINE (15)
 */
	typedef struct _dwg_entity_POLYLINE_2D
	{
		unsigned int flags;
		unsigned int curve_type;
		double start_width;
		double end_width;
		double thickness;
		double elevation;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
		long unsigned int owned_obj_count;
                Dwg_Object_Ref* first_vertex;
                Dwg_Object_Ref* last_vertex;
                Dwg_Object_Ref** vertex;
                Dwg_Object_Ref* seqend;
	} Dwg_Entity_POLYLINE_2D;

/**
 Struct for 3D POLYLINE (16)
 */
	typedef struct _dwg_entity_POLYLINE_3D
	{
		unsigned char flags_1;
		unsigned char flags_2;
		long unsigned int owned_obj_count;
		Dwg_Object_Ref* first_vertex;
                Dwg_Object_Ref* last_vertex;
                Dwg_Object_Ref** vertex;
                Dwg_Object_Ref* seqend;
	} Dwg_Entity_POLYLINE_3D;

/**
 Struct for ARC (17)
 */
	typedef struct _dwg_entity_ARC
	{
		double x0;
		double y0;
		double z0;
		double radius;
		double thickness;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
		double start_angle;
		double end_angle;
	} Dwg_Entity_ARC;

/**
 Struct for CIRCLE (18)
 */
	typedef struct _dwg_entity_CIRCLE
	{
		double x0;
		double y0;
		double z0;
		double radius;
		double thickness;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
	} Dwg_Entity_CIRCLE;

/**
 Struct for LINE (19)
 */
	typedef struct _dwg_entity_LINE
	{
		unsigned char Zs_are_zero;
		double x0;
		double x1;
		double y0;
		double y1;
		double z0;
		double z1;
		double thickness;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
	} Dwg_Entity_LINE;

 /**
 Struct for ordinate dimension - DIMENSION_ORDINATE (20)
 */
	typedef struct _dwg_entity_DIMENSION_ORDINATE
	{
                struct
		{
			double x;
			double y;
			double z;
		} extrusion;
                double x0;
                double y0;
                struct
		{
			double ecs_11;
			double ecs_12;
		} elevation;
                unsigned char flags_1;
                unsigned char* user_text;
                double text_rot;
                double horiz_dir;
                struct
		{
			double x;
			double y;
			double z;
		} ins_scale;
                double ins_rotation;
                unsigned int attachment_point;
                unsigned int lspace_style;
                double lspace_factor;
                double act_measurement;
                unsigned char unknown;
                unsigned char flip_arrow1;
                unsigned char flip_arrow2;
                struct
		{
			double x;
			double y;
		} _12_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _10_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _13_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _14_pt;
                unsigned char flags_2;
                Dwg_Object_Ref* dimstyle;
                Dwg_Object_Ref* block;
	} Dwg_Entity_DIMENSION_ORDINATE;

/**
 Struct for linear dimension - DIMENSION_LINEAR (21)
 */
	typedef struct _dwg_entity_DIMENSION_LINEAR
	{
                struct
		{
			double x;
			double y;
			double z;
		} extrusion;
                double x0;
                double y0;
                struct
		{
			double ecs_11;
			double ecs_12;
		} elevation;
                unsigned char flags;
                unsigned char* user_text;
                double text_rot;
                double horiz_dir;
                struct
		{
			double x;
			double y;
			double z;
		} ins_scale;
                double ins_rotation;
                unsigned int attachment_point;
                unsigned int lspace_style;
                double lspace_factor;
                double act_measurement;
                unsigned char unknown;
                unsigned char flip_arrow1;
                unsigned char flip_arrow2;
                struct
		{
			double x;
			double y;
		} _12_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _13_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _14_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _10_pt;
                double ext_line_rot;
                double dim_rot;
                Dwg_Object_Ref* dimstyle;
                Dwg_Object_Ref* block;
	} Dwg_Entity_DIMENSION_LINEAR;

/**
 Struct for aligned dimension - DIMENSION_ALIGNED (22)
 */
	typedef struct _dwg_entity_DIMENSION_ALIGNED
	{
                struct
		{
			double x;
			double y;
			double z;
		} extrusion;
                double x0;
                double y0;
                struct
		{
			double ecs_11;
			double ecs_12;
		} elevation;
                unsigned char flags;
                unsigned char* user_text;
                double text_rot;
                double horiz_dir;
                struct
		{
			double x;
			double y;
			double z;
		} ins_scale;
                double ins_rotation;
                unsigned int attachment_point;
                unsigned int lspace_style;
                double lspace_factor;
                double act_measurement;
                unsigned char unknown;
                unsigned char flip_arrow1;
                unsigned char flip_arrow2;
                struct
		{
			double x;
			double y;
		} _12_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _13_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _14_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _10_pt;
                double ext_line_rot;
                Dwg_Object_Ref* dimstyle;
                Dwg_Object_Ref* block;
	} Dwg_Entity_DIMENSION_ALIGNED;

/**
 Struct for angular 3pt dimension - DIMENSION_ANG3PT (23)
 */
	typedef struct _dwg_entity_DIMENSION_ANG3PT
	{
                struct
		{
			double x;
			double y;
			double z;
		} extrusion;
                double x0;
                double y0;
                struct
		{
			double ecs_11;
			double ecs_12;
		} elevation;
                unsigned char flags;
                unsigned char* user_text;
                double text_rot;
                double horiz_dir;
                struct
		{
			double x;
			double y;
			double z;
		} ins_scale;
                double ins_rotation;
                unsigned int attachment_point;
                unsigned int lspace_style;
                double lspace_factor;
                double act_measurement;
                unsigned char unknown;
                unsigned char flip_arrow1;
                unsigned char flip_arrow2;
                struct
		{
			double x;
			double y;
		} _12_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _10_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _13_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _14_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _15_pt;
                Dwg_Object_Ref* dimstyle;
                Dwg_Object_Ref* block;
	} Dwg_Entity_DIMENSION_ANG3PT;

/**
 Struct for angular 2 line dimension - DIMENSION_ANG2LN (24)
 */
	typedef struct _dwg_entity_DIMENSION_ANG2LN
	{
                struct
		{
			double x;
			double y;
			double z;
		} extrusion;
                double x0;
                double y0;
                struct
		{
			double ecs_11;
			double ecs_12;
			double ecs_16;
		} elevation;
                unsigned char flags;
                unsigned char* user_text;
                double text_rot;
                double horiz_dir;
                struct
		{
			double x;
			double y;
			double z;
		} ins_scale;
                double ins_rotation;
                unsigned int attachment_point;
                unsigned int lspace_style;
                double lspace_factor;
                double act_measurement;
                unsigned char unknown;
                unsigned char flip_arrow1;
                unsigned char flip_arrow2;
                struct
		{
			double x;
			double y;
		} _12_pt;
                struct
		{
			double x;
			double y;
		} _16_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _13_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _14_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _15_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _10_pt;
                Dwg_Object_Ref* dimstyle;
                Dwg_Object_Ref* block;
	} Dwg_Entity_DIMENSION_ANG2LN;

 /**
 Struct for radius dimension - DIMENSION_RADIUS (25)
 */
	typedef struct _dwg_entity_DIMENSION_RADIUS
	{
                struct
		{
			double x;
			double y;
			double z;
		} extrusion;
                double x0;
                double y0;
                struct
		{
			double ecs_11;
			double ecs_12;
		} elevation;
                unsigned char flags;
                unsigned char* user_text;
                double text_rot;
                double horiz_dir;
                struct
		{
			double x;
			double y;
			double z;
		} ins_scale;
                double ins_rotation;
                unsigned int attachment_point;
                unsigned int lspace_style;
                double lspace_factor;
                double act_measurement;
                unsigned char unknown;
                unsigned char flip_arrow1;
                unsigned char flip_arrow2;
                struct
		{
			double x;
			double y;
		} _12_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _10_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _15_pt;
                double leader_len;
                Dwg_Object_Ref* dimstyle;
                Dwg_Object_Ref* block;
	} Dwg_Entity_DIMENSION_RADIUS;

 /**
 Struct for diameter dimension - DIMENSION_DIAMETER (26)
 */
	typedef struct _dwg_entity_DIMENSION_DIAMETER
	{
                struct
		{
			double x;
			double y;
			double z;
		} extrusion;
                double x0;
                double y0;
                struct
		{
			double ecs_11;
			double ecs_12;
		} elevation;
                unsigned char flags;
                unsigned char* user_text;
                double text_rot;
                double horiz_dir;
                struct
		{
			double x;
			double y;
			double z;
		} ins_scale;
                double ins_rotation;
                unsigned int attachment_point;
                unsigned int lspace_style;
                double lspace_factor;
                double act_measurement;
                unsigned char unknown;
                unsigned char flip_arrow1;
                unsigned char flip_arrow2;
                struct
		{
			double x;
			double y;
		} _12_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _10_pt;
                struct
		{
			double x;
			double y;
			double z;
		} _15_pt;
                double leader_len;
                Dwg_Object_Ref* dimstyle;
                Dwg_Object_Ref* block;
	} Dwg_Entity_DIMENSION_DIAMETER;

/**
 Struct for:  POINT (27)
 */
	typedef struct _dwg_entity_POINT
        {
                double x;
                double y;
                double z;
                double thickness;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
                double x_ang;
	} Dwg_Entity_POINT;

/**
 Struct for:  3D FACE (28)
 */
	typedef struct _dwg_entity_3D_FACE
	{
                unsigned int has_no_flags;
                unsigned int z_is_zero;
		struct
		{
			double x;
			double y;
			double z;
		} corner1;
		struct
		{
			double x;
			double y;
			double z;
		} corner2;
		struct
		{
			double x;
			double y;
			double z;
		} corner3;		
		struct
		{
			double x;
			double y;
			double z;
		} corner4;
                unsigned char invis_flags;
	} Dwg_Entity__3DFACE;

/**
 Struct for:  POLYLINE (PFACE) (29)
 */
	typedef struct _dwg_entity_POLYLINE_PFACE
	{
		unsigned int numverts;
		unsigned int numfaces;
                unsigned long owned_obj_count;
                Dwg_Object_Ref* first_vertex;
                Dwg_Object_Ref* last_vertex;
                Dwg_Object_Ref** vertex;
                Dwg_Object_Ref* seqend;
		
	} Dwg_Entity_POLYLINE_PFACE;

/**
 Struct for:  POLYLINE (MESH) (30)
 */
	typedef struct _dwg_entity_POLYLINE_MESH
	{
		unsigned char flags;
		unsigned char curve_type;
		unsigned int m_vert_count;
		unsigned int n_vert_count;
		unsigned int m_density;
		unsigned int n_density;
                unsigned long owned_obj_count;
                Dwg_Object_Ref* first_vertex;
                Dwg_Object_Ref* last_vertex;
                Dwg_Object_Ref** vertex;
                Dwg_Object_Ref* seqend;
		
	} Dwg_Entity_POLYLINE_MESH;

/**
 Struct for:  SOLID (31)
 */

	typedef struct _dwg_entity_SOLID
	{
		double thickness;
		struct
		{
			double x;
			double y;
			double z;
		} corner1;
		struct
		{
			double x;
			double y;
			double z;
		} corner2;
		struct
		{
			double x;
			double y;
			double z;
		} corner3;		
		struct
		{
			double x;
			double y;
			double z;
		} corner4;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
		
	} Dwg_Entity_SOLID;

/**
 Struct for:  TRACE (32)
 */

	typedef struct _dwg_entity_TRACE
	{
		double thickness;
		struct
		{
			double x;
			double y;
			double z;
		} corner1;
		struct
		{
			double x;
			double y;
			double z;
		} corner2;
		struct
		{
			double x;
			double y;
			double z;
		} corner3;		
		struct
		{
			double x;
			double y;
			double z;
		} corner4;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
		
	} Dwg_Entity_TRACE;

/**
 Struct for:  SHAPE (33)
 */

	typedef struct _dwg_entity_SHAPE
	{
		struct
		{
			double x;
			double y;
			double z;
		} ins_pt;
		double scale;
		double rotation;
		double width_factor;
		double oblique;
		double thickness;
		double shape_no;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
		Dwg_Object_Ref* shapefile;
		
	} Dwg_Entity_SHAPE;
/**
 Struct for:  VIEWPORT ENTITY (34)
 */

	typedef struct _dwg_entity_VIEWPORT
	{
		struct
		{
			double x;
			double y;
			double z;
		} center;
		double width;
		double height;
                struct
		{
			double x;
			double y;
			double z;
		} view_target;
                struct
		{
			double x;
			double y;
			double z;
		} view_direction;
		double view_twist_angle;
		double view_height;
		double lens_length;
        double front_clip_z;
        double back_clip_z;
        double snap_angle;
        struct
		{
			double x;
			double y;
		} view_center;
                struct
		{
			double x;
			double y;
		} snap_base;
                struct
		{
			double x;
			double y;
		} snap_spacing;
                struct
		{
			double x;
			double y;
		} grid_spacing;
        unsigned int circle_zoom;
        unsigned int grid_major;
        unsigned long frozen_layer_count;
        unsigned long status_flags;
        unsigned char* style_sheet;
        char render_mode;
        unsigned int ucs_at_origin;
        unsigned int ucs_per_viewport;
        struct
		{
			double x;
			double y;
			double z;
		} ucs_origin;
        struct
		{
			double x;
			double y;
			double z;
		} ucs_x_axis;
        struct
		{
			double x;
			double y;
			double z;
		} ucs_y_axis;
        double ucs_elevation;
        unsigned int ucs_ortho_view_type;
        unsigned int shadeplot_mode;
        unsigned int use_def_lights;
        char def_lighting_type;
        double brightness;
        double contrast;
        Dwg_Color ambient_light_color;
	} Dwg_Entity_VIEWPORT;

/**
 Struct for ELLIPSE (35)
 */
	typedef struct _dwg_entity_ELLIPSE
	{
		double x0;
		double y0;
		double z0;
		double x1;
		double y1;
		double z1;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
		double axis_ratio;
		double start_angle;
		double end_angle;
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
        unsigned char scenario;
        unsigned int degree;
        double fit_tol;
        struct
        {
			double x;
			double y;
			double z;
		} beg_tan_vec;
        struct
        {
			double x;
			double y;
			double z;
		} end_tan_vec;
        unsigned char rational;
        unsigned char closed_b;
        unsigned char periodic;
        unsigned char weighted;
        double knot_tol;
        double ctrl_tol;
        unsigned int num_fit_pts;
	    Dwg_Entity_SPLINE_point* fit_pts;
        unsigned int num_knots;
        double* knots;
        unsigned int num_ctrl_pts;
	    Dwg_Entity_SPLINE_control_point* ctrl_pts;
} Dwg_Entity_SPLINE;

/**
 Struct for 3DSOLID (37)
 */
	typedef struct _dwg_entity_3DSOLID_wire
	{
		unsigned char type;
		long unsigned int selection_marker;
		unsigned int color;
		long unsigned int acis_index;
		long unsigned int num_points;
		struct
		{
			double x;
			double y;
			double z;
		} point;
		unsigned int transform_present;
		struct
		{
			double x;
			double y;
			double z;
		} axis_x;
		struct
		{
			double x;
			double y;
			double z;
		} axis_y;
		struct
		{
			double x;
			double y;
			double z;
		} axis_z;
		struct
		{
			double x;
			double y;
			double z;
		} translation;
		double scale;
		unsigned int has_rotation;
		unsigned int has_reflection;
		unsigned int has_shear;
	} Dwg_Entity_3DSOLID_wire;

	typedef struct _dwg_entity_3DSOLID_silhouette
	{
		unsigned long vp_id;
		struct
		{
			double x;
			double y;
			double z;
		} vp_target;
		struct
		{
			double x;
			double y;
			double z;
		} dir_target;
		struct
		{
			double x;
			double y;
			double z;
		} up_dir;
		unsigned long num_wires;
		Dwg_Entity_3DSOLID_wire * wires;
	} Dwg_Entity_3DSOLID_silhouette;
		

	typedef struct _dwg_entity_3DSOLID
	{
		unsigned char acis_empty;
		unsigned char unknown;
		unsigned int version;
		long unsigned int block_size;
		unsigned char ** sat_data;
		unsigned char * acis_data;
		unsigned int acis_data_size;
		unsigned int wireframe_data_present;
		unsigned int point_present;
		struct
		{
			double x;
			double y;
			double z;
		} point;
		unsigned long num_isolines;
		unsigned int isoline_present;
		unsigned int num_wires;
		Dwg_Entity_3DSOLID_wire * wires;
		unsigned long num_silhouettes;
		Dwg_Entity_3DSOLID_silhouette * silhouettes;
		//TODO review spec - extremely fuzzy
		unsigned int acis_empty2;
		long unsigned int unknown2007;
	} Dwg_Entity_3DSOLID;

/**
 Struct for REGION (38)
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
		double x0;
		double y0;
		double z0;
		double x1;
		double y1;
		double z1;
	} Dwg_Entity_RAY;

/**
 Struct for ray - RAY (40)
 */
        typedef Dwg_Entity_RAY Dwg_Entity_XLINE;
        
/**
 Structure for DICTIONARY (42)
 */
	typedef struct _dwg_entity_DICTIONARY
	{
		unsigned int size;
		unsigned int cloning;
		unsigned char hard_owner;
		char **name;
	} Dwg_Object_DICTIONARY;

/**
 Struct for MTEXT (44)
 */
	typedef struct _dwg_entity_MTEXT
	{
		double x0;
		double y0;
		double z0;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
		double x1;
		double y1;
		double z1;
		double rect_width;
		double rect_height;
		double text_height;
		unsigned int attachment;
		unsigned int drawing_dir;
		double extends;
		double extends_wid;
		char *text;
		unsigned int linespace_style;
		unsigned int linespace_factor;
		unsigned char unknown_bit;
		long unsigned int unknown_long;
	} Dwg_Entity_MTEXT;

/**
 Struct for LEADER (45)
 */
	typedef struct _dwg_entity_LEADER
	{
                unsigned int unknown_bit_1; //always seems to be zero
                unsigned int annot_type;
                unsigned int path_type;
                unsigned int numpts;
                struct
                {
                        double x;
                        double y;
                        double z;
                } * point;
                struct
                {
                        double x;
                        double y;
                        double z;
                } end_pt_proj;
                struct
                {
                        double x;
                        double y;
                        double z;
                } extrusion;
                struct
                {
                        double x;
                        double y;
                        double z;
                } x_direction;
                struct
                {
                        double x;
                        double y;
                        double z;
                } offset_block_inspt;
                struct
                {
                        double x;
                        double y;
                        double z;
                } unknown_pt;
                double dimgap;
                double box_height;
                double box_width;
                unsigned int hooklineonxdir;
                unsigned int arrowhead_on;
                unsigned int arrowhead_type;
                double dimasz;
                unsigned int unknown_bit_2;
                unsigned int unknown_bit_3;
                unsigned int unknown_short_1;
                unsigned int byblock_color;
                unsigned int unknown_bit_4;
                unsigned int unknown_bit_5;
	} Dwg_Entity_LEADER;

/**
 Struct for TOLERANCE (46)
 */
	typedef struct _dwg_entity_TOLERANCE
	{
                unsigned int unknown_short;
                double height;
                double dimgap;

                struct
                {
                        double x;
                        double y;
                        double z;
                } ins_pt;
                struct
                {
                        double x;
                        double y;
                        double z;
                } x_direction;
                struct
                {
                        double x;
                        double y;
                        double z;
                } extrusion;
                unsigned int text_string;
	} Dwg_Entity_TOLERANCE;

/**
 Structures for MLINE (47)
 */
	typedef struct _dwg_entity_MLINE_line
	{
        unsigned int num_segparms;
        double* segparms;
        unsigned int num_areafillparms;
        double* areafillparms;
    } Dwg_Entity_MLINE_line;

	typedef struct _dwg_entity_MLINE_vert
	{
        struct
        {
            double x;
            double y;
            double z;
        } vertex;
        struct
        {
            double x;
            double y;
            double z;
        } vertex_direction;
        struct
        {
            double x;
            double y;
            double z;
        } miter_direction;
        Dwg_Entity_MLINE_line* lines;
	} Dwg_Entity_MLINE_vert;

	typedef struct _dwg_entity_MLINE
	{
        double scale;
        unsigned int just;
        struct
        {
            double x;
            double y;
            double z;
        } base_point;
        struct
        {
            double x;
            double y;
            double z;
        } extrusion;
        unsigned int open_closed;
        unsigned int num_lines; //Linesinstyle
        unsigned int num_verts;
        Dwg_Entity_MLINE_vert* verts;
	} Dwg_Entity_MLINE;

/**
 Struct for BLOCK_CONTROL (48)
 */
	typedef struct _dwg_object_BLOCK_CONTROL
	{
	    unsigned int type;
		unsigned int size;
	} Dwg_Object_BLOCK_CONTROL;

/**
 Struct for BLOCK_HEADER (49)
 */	
	typedef struct _dwg_object_BLOCK_HEADER
	{
//TODO 

    unsigned char *entry_name;
    unsigned char _64_flag;
    unsigned int xrefindex_plus1;
    unsigned char xdep;
    unsigned char anonymous;
    unsigned char hasattrs;
    unsigned char blxisxref;
    unsigned char xrefoverlaid;
    unsigned char loaded_bit;
	  long unsigned int owned_object_count;
    struct
		{
		    double x;
		    double y;
		    double z;
		} base_pt;
    unsigned char* xref_pname;
  	unsigned char insert_count;
    unsigned char* block_description;
  	long unsigned int size_of_preview_data;
    Dwg_Object_Ref* block_control_handle;
    Dwg_Object_Ref** reactor;
    Dwg_Object_Ref* xdicobjhandle;
    Dwg_Object_Ref* NULL_handle;
    Dwg_Object_Ref* block_entity;
	} Dwg_Object_BLOCK_HEADER;

/**
 Struct for LAYER_CONTROL (50)
 */	
	typedef struct _dwg_object_LAYER_CONTROL
	{
//TODO 
	} Dwg_Object_LAYER_CONTROL;

/**
 Struct for LAYER (51)
 */
	typedef struct _dwg_object_LAYER
	{
		char *name;
		unsigned char bit64;
		unsigned int xrefi;
		unsigned char xrefdep;
		unsigned int values;
		unsigned int colour;
	} Dwg_Object_LAYER;

/**
 Struct for STYLE_CONTROL (52)
 */	
	typedef struct _dwg_object_STYLE_CONTROL
	{
//TODO 
	} Dwg_Object_STYLE_CONTROL;

/**
 Struct for STYLE (53)
 */	
	typedef struct _dwg_object_STYLE
	{
//TODO 
	} Dwg_Object_STYLE;

// 54 and 55 are UNKNOWN OBJECTS

/**
 Struct for LTYPE_CONTROL (56)
 */	
	typedef struct _dwg_object_LTYPE_CONTROL
	{
//TODO 
	} Dwg_Object_LTYPE_CONTROL;

/**
 Struct for LTYPE (57)
 */	
	typedef struct _dwg_object_LTYPE
	{
//TODO 
	} Dwg_Object_LTYPE;

// 58 and 59 are UNKNOWN OBJECTS

/**
 Struct for VIEW_CONTROL (60)
 */	
	typedef struct _dwg_object_VIEW_CONTROL
	{
//TODO 
	} Dwg_Object_VIEW_CONTROL;

/**
 Struct for VIEW (61)
 */	
	typedef struct _dwg_object_VIEW
	{
//TODO 
	} Dwg_Object_VIEW;

/**
 Struct for UCS_CONTROL (62)
 */	
	typedef struct _dwg_object_UCS_CONTROL
	{
//TODO 
	} Dwg_Object_UCS_CONTROL;

/**
 Struct for UCS (63)
 */	
	typedef struct _dwg_object_UCS
	{
//TODO 
	} Dwg_Object_UCS;

/**
 Struct for VPORT_CONTROL (64)
 */	
	typedef struct _dwg_object_VPORT_CONTROL
	{
//TODO 
	} Dwg_Object_VPORT_CONTROL;

/**
 Struct for VPORT (65)
 */	
	typedef struct _dwg_object_VPORT
	{
//TODO 
	} Dwg_Object_VPORT;

/**
 Struct for APPID_CONTROL (66)
 */	
	typedef struct _dwg_object_APPID_CONTROL
	{
//TODO 
	} Dwg_Object_APPID_CONTROL;

/**
 Struct for APPID (67)
 */	
	typedef struct _dwg_object_APPID
	{
//TODO 
	} Dwg_Object_APPID;

/**
 Struct for DIMSTYLE_CONTROL (68)
 */	
	typedef struct _dwg_object_DIMSTYLE_CONTROL
	{
//TODO 
	} Dwg_Object_DIMSTYLE_CONTROL;

/**
 Struct for DIMSTYLE (69)
 */	
	typedef struct _dwg_object_DIMSTYLE
	{
//TODO 
	} Dwg_Object_DIMSTYLE;

/**
 Struct for VP_ENT_HDR_CTRL (70)
 */	
	typedef struct _dwg_object_VP_ENT_HDR_CTRL
	{
//TODO 
	} Dwg_Object_VP_ENT_HDR_CTRL;

/**
 Struct for VP_ENT_HDR (71)
 */	
	typedef struct _dwg_object_VP_ENT_HDR
	{
//TODO 
	} Dwg_Object_VP_ENT_HDR;

/**
 Struct for GROUP (72)
 */	
	typedef struct _dwg_object_GROUP
	{
//TODO 
	} Dwg_Object_GROUP;

/**
 Struct for MLINESTYLE (73)
 */	
	typedef struct _dwg_object_MLINESTYLE
	{
//TODO 
	} Dwg_Object_MLINESTYLE;

/**
 Struct for DICTIONARYVAR (varies)
 */	
	typedef struct _dwg_object_DICTIONARYVAR
	{
//TODO 
	} Dwg_Object_DICTIONARYVAR;

/**
 Struct for DICTIONARYWDLFT (varies)
 */	
	typedef struct _dwg_object_DICTIONARYWDLFT
	{
//TODO 
	} Dwg_Object_DICTIONARYWDLFT;

/**
 Struct for HATCH (varies)
 */	
	typedef struct _dwg_object_HATCH
	{
//TODO 
	} Dwg_Object_HATCH;

/**
 Struct for IDBUFFER (varies)
 */	
	typedef struct _dwg_object_IDBUFFER
	{
//TODO 
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
		long unsigned int class_version;
		struct
		{
		    double x;
		    double y;
		    double z;
		} pt0;
		struct
		{
		    double x;
		    double y;
		    double z;
		} uvec;
		struct
		{
		    double x;
		    double y;
		    double z;
		} vvec;
		struct
		{
		    double width;
		    double height;
		} size;
		unsigned int display_props;
		unsigned char clipping;
		unsigned char brightness;
		unsigned char contrast;
		unsigned char fade;
		unsigned int clip_boundary_type;
		struct
		{
		    double x;
		    double y;
		} boundary_pt0;
		struct
		{
		    double x;
		    double y;
		} boundary_pt1;
		unsigned long num_clip_verts;
		Dwg_Entity_IMAGE_clip_vert* clip_verts;
	} Dwg_Entity_IMAGE;

/**
 Struct for IMAGEDEF (varies)
 */	
	typedef struct _dwg_object_IMAGEDEF
	{
//TODO 
	} Dwg_Object_IMAGEDEF;

/**
 Struct for IMAGEDEFREACTOR (varies)
 */	
	typedef struct _dwg_object_IMAGEDEFREACTOR
	{
//TODO 
	} Dwg_Object_IMAGEDEFREACTOR;

/**
 Struct for LAYER_INDEX (varies)
 */	
	typedef struct _dwg_object_LAYER_INDEX
	{
//TODO 
	} Dwg_Object_LAYER_INDEX;

/**
 Struct for LAYOUT (varies)
 */
	typedef struct _dwg_object_LAYOUT
	{
		struct
		{
			char *agordo;
			char *printilo;
			unsigned int flags;
			double maldekstre;
			double malsupre;
			double dekstre;
			double supre;
			double width;
			double height;
			char *size;
			double dx;
			double dy;
			unsigned int unuoj;
			unsigned int rotacio;
			unsigned int type;
			double x_min;
			double y_min;
			double x_max;
			double y_max;
			char *name;
			struct
			{
				double A; // A:B (ekz: 1:10, 1:2, 50:1, ktp)
				double B;
				unsigned int type;
				double factor;
			} scale;
			char *stilfolio;
			double x0;
			double y0;
		} page;

		char *name;
		unsigned int ordo;
		unsigned int flags;
		double x0;
		double y0;
		double z0;
		double x_min;
		double y_min;
		double x_max;
		double y_max;
		struct
		{
			double x0;
			double y0;
			double z0;
		} enmeto;
		struct
		{
			double x0;
			double y0;
			double z0;
		} axis_X;
		struct
		{
			double x0;
			double y0;
			double z0;
		} axis_Y;
		double elevation;
		unsigned int rigardtype;
		struct
		{
			double x_min;
			double y_min;
			double z_min;
			double x_max;
			double y_max;
			double z_max;
		} limo;
	} Dwg_Object_LAYOUT;

/**
 Structures for LWPLINE
 */
	typedef struct _dwg_entity_LWPLINE_point
	{
		double x;
		double y;
	} Dwg_Entity_LWPLINE_point;

	typedef struct _dwg_entity_LWPLINE_width
	{
		double start;
		double end;
	} Dwg_Entity_LWPLINE_width;

	typedef struct _dwg_entity_LWPLINE
	{
		unsigned int flags;
		double const_width;
		double elevation;
		double thickness;
		struct
		{
		    double x;
		    double y;
		    double z;
		} normal;
		long unsigned int num_points;
		Dwg_Entity_LWPLINE_point* points;
		long unsigned int num_bulges;
		double* bulges;
		long unsigned int num_widths;
		Dwg_Entity_LWPLINE_width* widths;
	} Dwg_Entity_LWPLINE;


/**
 Struct for OLE2FRAME
 */
	typedef struct _dwg_entity_OLE2FRAME
	{
		unsigned int flags;
		unsigned int mode;
		long unsigned int data_length;
		unsigned char* data;
		unsigned char unknown;
	} Dwg_Entity_OLE2FRAME;


/**
 Struct for PROXY (varies)
 */	
	typedef struct _dwg_object_PROXY
	{
//TODO 
	} Dwg_Object_PROXY;


/**
 Struct for PLACEHOLDER (varies)
 */	
	typedef struct _dwg_object_PLACEHOLDER
	{
//TODO 
	} Dwg_Object_PLACEHOLDER;


/**
 Struct for RASTERVARIABLES (varies)
 */	
	typedef struct _dwg_object_RASTERVARIABLES
	{
//TODO 
	} Dwg_Object_RASTERVARIABLES;


/**
 Struct for SORTENTSTABLE (varies)
 */	
	typedef struct _dwg_object_SORTENTSTABLE
	{
//TODO 
	} Dwg_Object_SORTENTSTABLE;


/**
 Struct for SPATIAL_FILTER (varies)
 */	
	typedef struct _dwg_object_SPATIAL_FILTER
	{
//TODO 
	} Dwg_Object_SPATIAL_FILTER;

/**
 Struct for TABLE (varies)
 */	
	typedef struct _dwg_entity_TABLE
	{
//TODO 
	} Dwg_Entity_TABLE;

//NOT SURE ABOUT THIS ONE (IS IT OBJECT OR ENTITY?):
/**
 Struct for SPATIAL_INDEX (varies)
 */	
	typedef struct _dwg_object_SPATIAL_INDEX
	{
//TODO 
	} Dwg_Object_SPATIAL_INDEX;

//NOT SURE ABOUT THIS ONE (IS IT OBJECT OR ENTITY?):
/**
 Struct for VBA_PROJECT (varies)
 */	
	typedef struct _dwg_object_VBA_PROJECT
	{
//TODO 
	} Dwg_Object_VBA_PROJECT;

//NOT SURE ABOUT THIS ONE (IS IT OBJECT OR ENTITY?):
/**
 Struct for WIPEOUTVARIABLE (varies)
 */	
	typedef struct _dwg_object_WIPEOUTVARIABLE
	{
//TODO 
	} Dwg_Object_WIPEOUTVARIABLE;


/**
 Struct for XRECORD (varies)
 */	
	typedef struct _dwg_object_XRECORD
	{
//TODO 
	} Dwg_Object_XRECORD;


			Dwg_Object_PROXY *PROXY;
			Dwg_Object_PLACEHOLDER *PLACEHOLDER;
			Dwg_Object_RASTERVARIABLES *RASTERVARIABLES;
			Dwg_Object_SORTENTSTABLE *SORTENTSTABLE;
			Dwg_Object_SPATIAL_FILTER *SPATIAL_FILTER;


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
		} tio;

		long unsigned int bitsize;
		//Dwg_Handle handle;

		unsigned int extended_size;
		Dwg_Handle extended_handle;
		unsigned char *extended;

		unsigned char picture_exists;
		long unsigned int picture_size;
		unsigned char *picture;

		unsigned char entity_mode;
		long unsigned int num_reactors;
		unsigned char xdict_missing_flag;
		unsigned char isbylayerlt; 
		unsigned char nolinks;
		Dwg_Color color;
		double linetype_scale;
		unsigned char linetype_flags;
		unsigned char plotstyle_flags;
		unsigned char material_flags;
		unsigned char shadow_flags;
		unsigned int invisible;
		unsigned char lineweight;

		unsigned int num_handles;

		//Common Entity Handle Data
		Dwg_Object_Ref* subentity_ref_handle;
		Dwg_Object_Ref** reactors;
		Dwg_Object_Ref* xdicobjhandle;
		Dwg_Object_Ref* prev_entity;
		Dwg_Object_Ref* next_entity;
		Dwg_Object_Ref* layer;
		Dwg_Object_Ref* ltype;
		Dwg_Object_Ref* plotstyle;
		Dwg_Object_Ref* material;

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
			Dwg_Object_STYLE_CONTROL *STYLE_CONTROL;
			Dwg_Object_STYLE *STYLE;
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
			Dwg_Object_VP_ENT_HDR_CTRL *VP_ENT_HDR_CTRL;
			Dwg_Object_VP_ENT_HDR *VP_ENT_HDR;
			Dwg_Object_GROUP *GROUP;
			Dwg_Object_MLINESTYLE *MLINESTYLE;
			Dwg_Object_DICTIONARYVAR *DICTIONARYVAR;
			Dwg_Object_DICTIONARYWDLFT *DICTIONARYWDLFT;
			Dwg_Object_HATCH *HATCH;
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
//NOT SURE ABOUT THESE:
			Dwg_Object_SPATIAL_INDEX *SPATIAL_INDEX;
			Dwg_Object_VBA_PROJECT *VBA_PROJECT;
			Dwg_Object_WIPEOUTVARIABLE *WIPEOUTVARIABLE;

			Dwg_Object_XRECORD *XRECORD;
		} tio;

		long unsigned int bitsize;
		//Dwg_Handle handle;
		unsigned int extended_size;
		Dwg_Handle extended_handle;
		unsigned char *extended;
		long unsigned int num_reactors;
  	unsigned char xdic_missing_flag;
//		unsigned char picture_exists;

		unsigned int num_handles;
		Dwg_Handle *handleref;
	} Dwg_Object_Object;

/**
 General object structure
 */
	typedef struct _dwg_object
	{
		unsigned int size;
		unsigned int type;
		unsigned int ckr;

		Dwg_Object_Supertype supertype;
		union
		{
			Dwg_Object_Entity *entity;
			Dwg_Object_Object *object;
			unsigned char *unknown;
		} tio;

		Dwg_Handle handle;

		struct _dwg_strukturo *parent;

	} Dwg_Object;

/**
 Structure for classes
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
 Dwg_Chain sama kiel Bit_Chain, en "bite.h"
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
		unsigned char number;
		long unsigned int address;
		long unsigned int size;
	} Dwg_Section;
/**
 Main DWG struct
 */
	typedef struct _dwg_strukturo
	{
		struct
		{
		    unsigned int version;
			unsigned int codepage;
			unsigned int num_sections;
            Dwg_Section* section;
		} header;

#		define DWG_UNKNOWN1_KIOM 123
		Dwg_Chain unknown1;

		Dwg_Chain picture; 

//number of header variables:
//R13: 199
//R14: 203
//R2000: 233
//R2004: 251
//R2007: 297

#		define DWG_NUM_VARIABLES 297
		union
		{
			unsigned char bitoko;
			unsigned int dubitoko;
			long unsigned int kvarbitoko;
			double duglitajxo;
			double xyz[3];
			double xy[2];
			unsigned char *text;
			Dwg_Handle handle;
		} var[DWG_NUM_VARIABLES];

		unsigned int num_classes;
		Dwg_Class *class;

		long unsigned int num_objects;
		Dwg_Object *object;

		long unsigned int num_object_refs;
		Dwg_Object_Ref *object_ref;

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

	} Dwg_Structure;


/*--------------------------------------------------
 * Functions
 */

	int dwg_read_file (char *filename, Dwg_Structure * dwg);
    void dwg_free (Dwg_Structure * dwg);
    unsigned char* dwg_bmp (Dwg_Structure *, long *);
    
    double dwg_model_x_min(Dwg_Structure *);
    double dwg_model_x_max(Dwg_Structure *);
    double dwg_model_y_min(Dwg_Structure *);
    double dwg_model_y_max(Dwg_Structure *);
    double dwg_model_z_min(Dwg_Structure *);
    double dwg_model_z_max(Dwg_Structure *);
    double dwg_model_page_x_min(Dwg_Structure *);
    double dwg_model_page_x_max(Dwg_Structure *);
    double dwg_model_page_y_min(Dwg_Structure *);
    double dwg_model_page_y_max(Dwg_Structure *);

#ifdef __cplusplus
}
#endif

#endif
