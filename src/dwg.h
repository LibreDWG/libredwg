/*****************************************************************************/
/*  LibDWG - Free DWG read-only library                                      */
/*  Copyright (C) 2008, 2009 Free Software Foundation, Inc.                  */
/*  Copyright (C) 2009 Rodrigo Rodrigues da Silva <rodrigopitanga@gmail.com> */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either versionn 3 of the License, or (at your option) any later versionn.  */
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
                DWG_TYPE_3DFACE = 0x1c,
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
 Struct for traktiloj.
 */
	typedef struct _dwg_traktilo
	{
		unsigned char code;
		unsigned char size;
		long unsigned int value;
	} Dwg_Traktilo;

/* OBJECTS *******************************************************************/
/**
 Struct for textj - UNUSED (0)
 */
	typedef int Dwg_Entity_UNUSED;

/**
 Struct for textj - TEXT (1)
 */
	typedef struct _dwg_entity_TEXT
	{
		unsigned char datumindik;
		double levigxo;
		double x0;
		double y0;
		struct
		{
			double x;
			double y;
			unsigned int h;
			unsigned int v;
		} gxisrandigo; //gxis = Until / randigo = Edge. UntilEdge == Snap? perhaps Trim!?
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
		double thickness;
		double klinang;
		double turnang;
		double height;
		double largxfaktoro;
		unsigned char *text;
		unsigned int generacio;
	} Dwg_Entity_TEXT;

/**
 Struct for atributoj - ATTRIB (2)
 */
	typedef struct _dwg_entity_ATTRIB
	{
		unsigned char datumindik;
		double levigxo;
		double x0;
		double y0;
		struct
		{
			double x;
			double y;
			unsigned int h;
			unsigned int v;
		} gxisrandigo;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
		double thickness;
		double klinang;
		double turnang;
		double height;
		double largxfaktoro;
		unsigned char *text;
		unsigned int generacio;
		unsigned char *etikedo;
		unsigned int kamplong; //neuzita
		unsigned char indikiloj;
	} Dwg_Entity_ATTRIB;

/**
 Struct for atributo-diendj - ATTDEF (3)
 */
	typedef struct _dwg_entity_ATTDEF
	{
		unsigned char datumindik;
		double levigxo;
		double x0;
		double y0;
		struct
		{
			double x;
			double y;
			unsigned int h;
			unsigned int v;
		} gxisrandigo;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
		double thickness;
		double klinang;
		double turnang;
		double height;
		double largxfaktoro;
		unsigned char *text;
		unsigned int generacio;
		unsigned char *etikedo;
		unsigned int kamplong; //neuzita
		unsigned char indikiloj;
		unsigned char *invitilo;
	} Dwg_Entity_ATTDEF;

/**
 Struct for blokoj - BLOCK (4)
 */
	typedef struct _dwg_entity_BLOCK
	{
		unsigned char *name;
	} Dwg_Entity_BLOCK;

/**
 Struct for blokendj - ENDBLK (5)
 */
	typedef struct _dwg_entity_ENDBLK
	{
	} Dwg_Entity_ENDBLK;

/**
 Struct for enmetoj - SEQEND (6)
 */
	typedef struct _dwg_entity_SEQNED
	{
	} Dwg_Entity_SEQEND;

/**
 Struct for enmetoj - INSERT (7)
 */
	typedef struct _dwg_entity_INSERT
	{
		double x0;
		double y0;
		double z0;
		unsigned char skalindik;
		struct
		{
			double x;
			double y;
			double z;
		} scale;
		double turnang;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
		unsigned char kun_attrib;
	} Dwg_Entity_INSERT;

/**
 Struct for multoblaj enmetoj - MINSERT (8)
 */
	typedef struct _dwg_entity_MINSERT
	{
		double x0;
		double y0;
		double z0;
		unsigned char skalindik;
		struct
		{
			double x;
			double y;
			double z;
		} scale;
		double turnang;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
		unsigned char kun_attrib;
		struct
		{
			unsigned int size;
			double dx;
		} kol;
		struct
		{
			unsigned int size;
			double dy;
		} lin;
	} Dwg_Entity_MINSERT;

/**
 Struct for ?? - ?? (9)
 */

/**
 Struct for verticoj - VERTEX_2D (10)
 */
	typedef struct _dwg_entity_VERTEX_2D
	{
		unsigned char indikiloj;
		double x0;
		double y0;
		double z0;
		double eklargxo;
		double finlargxo;
		double protub;
		double tangxdir;
	} Dwg_Entity_VERTEX_2D;

/**
 Struct for verticoj - VERTEX_3D (11)
 */
	typedef struct _dwg_entity_VERTEX_3D
	{
		unsigned char indikiloj;
		double x0;
		double y0;
		double z0;
	} Dwg_Entity_VERTEX_3D;

/**
 Struct for verticoj - VERTEX_MESH (12) - same as VERTEX_3D
 */
	typedef Dwg_Entity_VERTEX_3D Dwg_Entity_VERTEX_MESH;

/**
 Struct for verticoj - VERTEX_PFACE (13) - same as VERTEX_3D
 */
	typedef Dwg_Entity_VERTEX_3D Dwg_Entity_VERTEX_PFACE;

/**
 Struct for verticoj - VERTEX_PFACE_FACE (14)
 */
	typedef struct _dwg_entity_VERTEX_PFACE_FACE
	{
		unsigned int vertind[4];
	} Dwg_Entity_VERTEX_PFACE_FACE;

/**
 Struct for:  2D POLYLINE (15)
 */
	typedef struct _dwg_entity_POLYLINE_2D
	{
		unsigned int indikiloj;
		unsigned int kurbtype;
		double eklargxo;
		double finlargxo;
		double thickness;
		double levigxo;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
	} Dwg_Entity_POLYLINE_2D;

/**
 Struct for:  3D POLYLINE (16)
 */
	typedef struct _dwg_entity_POLYLINE_3D
	{
		unsigned char indikiloj_1;
		unsigned char indikiloj_2;
	} Dwg_Entity_POLYLINE_3D;

/**
 Struct for arkoj - ARC (17)
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
		double ekangulo;
		double finangulo;
	} Dwg_Entity_ARC;

/**
 Struct for cirkloj - CIRCLE (18)
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
 Struct for linioj - LINE (19)
 */
	typedef struct _dwg_entity_LINE
	{
		unsigned char nur_2D;
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
 Struct for puktoj - POINT (27)
 */
	typedef struct _dwg_entity_POINT
	{
		double x0;
		double y0;
		double z0;
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
	} Dwg_Entity_3D_FACE;

/**
 Struct for:  POLYLINE (PFACE) (29)
 */
	typedef struct _dwg_entity_POLYLINE_PFACE
	{
		unsigned int numverts;
		unsigned int numfaces;
		
	} Dwg_Entity_POLILYNE_PFACE;

/**
 Struct for:  POLYLINE (MESH) (30)
 */
	typedef struct _dwg_entity_POLYLINE_MESH
	{
		unsigned int flags;
		unsigned int curve_type;
		unsigned int m_vert_count;
		unsigned int n_vert_count;
		unsigned int m_density;
		unsigned int n_density;
		
	} Dwg_Entity_POLILYNE_MESH;

/**
 Struct for:  SOLID (31)
 */

	typedef struct _dwg_entity_SOLID
	{
		double thickness;
		double elevation;
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
		double elevation;
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
		
	} Dwg_Entity_SHAPE;
/**
 Struct for:  VIEWPORT ENTITY (34)
 */

	typedef struct _dwg_entity_VIEWPORT_ENTITY
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
                //add handles

	} Dwg_Entity_VIEWPORT_ENTITY;

/**
 Struct for elipsoj - ELLIPSE (35)
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
		double radiusproporcio;
		double ekangulo;
		double finangulo;
	} Dwg_Entity_ELLIPSE;

/**
 Struct for radioj - RAY (40)
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
 Struct for textj - MTEXT (40)
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
		double largxeco;
		double height;
		unsigned int kunmeto;
		unsigned int direkto;
		double etendo;
		double etendlargxo;
		char *text;
		unsigned int linispaco_stilo;
		unsigned int linispaco_faktoro;
		unsigned char ia_bit;
	} Dwg_Entity_MTEXT;

/**
 Struct for tavoloj - LAYER (51)
 */
	typedef struct _dwg_object_LAYER
	{
		char *name;
		unsigned char bit64;
		unsigned int xrefi;
		unsigned char xrefdep;
		unsigned int ecoj;
		unsigned int colour;
	} Dwg_Object_LAYER;

/**
 Struct for arangxo - LAYOUT (502?)
 */
	typedef struct _dwg_object_LAYOUT
	{
		struct
		{
			char *agordo;
			char *printilo;
			unsigned int indikiloj;
			double maldekstre;
			double malsupre;
			double dekstre;
			double supre;
			double largxeco;
			double height;
			char *size;
			double dx;
			double dy;
			unsigned int unuoj;
			unsigned int rotacio;
			unsigned int type;
			double x_min;
			double y_min;
			double x_maks;
			double y_maks;
			char *name;
			struct
			{
				double A; // A:B (ekz: 1:10, 1:2, 50:1, ktp)
				double B;
				unsigned int type;
				double faktoro;
			} scale;
			char *stilfolio;
			double x0;
			double y0;
		} pagxo;

		char *name;
		unsigned int ordo;
		unsigned int indikiloj;
		double x0;
		double y0;
		double z0;
		double x_min;
		double y_min;
		double x_maks;
		double y_maks;
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
		} akso_X;
		struct
		{
			double x0;
			double y0;
			double z0;
		} akso_Y;
		double levigxo;
		unsigned int rigardtype;
		struct
		{
			double x_min;
			double y_min;
			double z_min;
			double x_maks;
			double y_maks;
			double z_maks;
		} limo;
	} Dwg_Object_LAYOUT;

/* OBJECTS - END ************************************************************/

/**
 Strukturo de rikordoj por atributoj de entityj.
 */
	typedef struct _dwg_object_entity
	{
		union
		{
			Dwg_Entity_UNUSED *UNUSED;
			Dwg_Entity_TEXT *TEXT;
			Dwg_Entity_ATTRIB *ATTRIB;
			Dwg_Entity_ATTDEF *ATTDEF;
			Dwg_Entity_BLOCK *BLOCK;
			Dwg_Entity_ENDBLK *ENDBLK;
			Dwg_Entity_INSERT *INSERT;
			Dwg_Entity_MINSERT *MINSERT;
			Dwg_Entity_VERTEX_2D *VERTEX_2D;
			Dwg_Entity_VERTEX_3D *VERTEX_3D;
			Dwg_Entity_VERTEX_PFACE_FACE *VERTEX_PFACE_FACE;
			Dwg_Entity_POLYLINE_2D *POLYLINE_2D;
			Dwg_Entity_POLYLINE_3D *POLYLINE_3D;
			Dwg_Entity_ARC *ARC;
			Dwg_Entity_CIRCLE *CIRCLE;
			Dwg_Entity_LINE *LINE;
			Dwg_Entity_POINT *POINT;
			Dwg_Entity_ELLIPSE *ELLIPSE;
			Dwg_Entity_RAY *RAY;
			Dwg_Entity_MTEXT *MTEXT;
		} tio;

		long unsigned int bitsize;
		Dwg_Traktilo traktilo;

		unsigned int kromdat_size;
		Dwg_Traktilo kromdat_trakt;
		unsigned char *kromdat;

		unsigned char picture_ekzistas;
		long unsigned int picture_size;
		unsigned char *picture;

		unsigned char regime;
		long unsigned int reagilo_size;
		unsigned char senligiloj;
		unsigned int colour;
		double linitypescale;
		unsigned char linitype;
		unsigned char printstilo;
		unsigned int malvidebleco;
		unsigned char linithickness;

		unsigned int traktref_size;
		Dwg_Traktilo *traktref;
	} Dwg_Object_Entity;

/**
 Strukturo de rikordoj por atributoj de ordinaraj objectj.
 */
	typedef struct _dwg_object_object
	{
		union
		{
			Dwg_Object_LAYER *LAYER;
			Dwg_Object_LAYOUT *LAYOUT;
		} tio;

		long unsigned int bitsize;
		Dwg_Traktilo traktilo;

		unsigned int kromdat_size;
		Dwg_Traktilo kromdat_trakt;
		unsigned char *kromdat;

		long unsigned int reagilo_size;

		unsigned int traktref_size;
		Dwg_Traktilo *traktref;
	} Dwg_Object_Object;

/**
 Gxenerala strukturo de rikordoj por objectj.
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
			unsigned char *unknownjxo;
		} tio;

		long unsigned int trakt;
	} Dwg_Object;

/**
 Strukturo de rikordoj por classj.
 */
	typedef struct _dwg_class
	{
		unsigned int number;
		unsigned int version;
		unsigned char *appname;
		unsigned char *cppname;
		unsigned char *dxfname;
		unsigned char estisfantomo;
		unsigned int eroid;
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
 Cxefa strukturo de la dwg-datenaro.
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

#		define DWG_NUM_VARIABLES 233
		union
		{
			unsigned char bitoko;
			unsigned int dubitoko;
			long unsigned int kvarbitoko;
			double duglitajxo;
			double xyz[3];
			double xy[2];
			unsigned char *text;
			Dwg_Traktilo traktilo;
		} var[DWG_NUM_VARIABLES];

		unsigned int num_classes;
		Dwg_Class *class;

		long unsigned int num_objects;
		Dwg_Object *object;

		struct
		{
			unsigned char unknownjxo[6];
			struct
			{
				int size;
				unsigned char chain[4];
			} traktrik[14];
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
