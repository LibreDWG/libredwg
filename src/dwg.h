/*****************************************************************************/
/*  LibDWG - Free DWG read-only library                                      */
/*  Copyright (C) 2008, 2009 Free Software Foundation, Inc.                  */
/*  Copyright (C) 2009 Rodrigo Rodrigues da Silva <rodrigopitanga@gmail.com> */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
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
		DWG_SUPERTYPE_ESTAJXO,
		DWG_SUPERTYPE_ORDINARAJXO
	} Dwg_Object_Supertype;

/**
 Object types that exist in dwg-files.
 */
	typedef enum DWG_OBJECT_TYPE
	{
		DWG_TYPE_UNUSED = 0,
		DWG_TYPE_TEXT = 1,
		DWG_TYPE_ATTRIB = 2,
		DWG_TYPE_ATTDEF = 3,
		DWG_TYPE_BLOCK = 4,
		DWG_TYPE_ENDBLK = 5,
		DWG_TYPE_SEQEND = 6,
		DWG_TYPE_INSERT = 7,
		DWG_TYPE_MINSERT = 8,
		DWG_TYPE_VERTEX_2D = 10,
		DWG_TYPE_VERTEX_3D = 11,
		DWG_TYPE_VERTEX_MESH = 12,
		DWG_TYPE_VERTEX_PFACE = 13,
		DWG_TYPE_VERTEX_PFACE_FACE = 14,
		DWG_TYPE_POLYLINE_2D = 15,
		DWG_TYPE_POLYLINE_3D = 16,
		DWG_TYPE_ARC = 17,
		DWG_TYPE_CIRCLE = 18,
		DWG_TYPE_LINE = 19,
		DWG_TYPE_POINT = 27,
		DWG_TYPE_ELLIPSE = 35,
		DWG_TYPE_RAY = 40,
		DWG_TYPE_XLINE = 41,
		DWG_TYPE_DICTIONARY = 42,
		DWG_TYPE_MTEXT = 44,
		DWG_TYPE_BLOCK_CONTROL = 48,
		DWG_TYPE_BLOCK_HEADER = 49,
		DWG_TYPE_LAYER_CONTROL = 50,
		DWG_TYPE_LAYER = 51,
		DWG_TYPE_SHAPEFILE_CONTROL = 52,
		DWG_TYPE_SHAPEFILE = 53,
		DWG_TYPE_LINETYPE_CONTROL = 56,
		DWG_TYPE_LTYPE = 56,
		DWG_TYPE_VIEW_CONTROL = 60,
		DWG_TYPE_UCS_CONTROL = 62,
		DWG_TYPE_TABLE_VIEWPORT = 64,
		DWG_TYPE_VPORT = 64,
		DWG_TYPE_TABLE_APPID = 66,
		DWG_TYPE_APPID = 67,
		DWG_TYPE_DIMSTYLE_CONTROL = 68,
		DWG_TYPE_DIMSTYLE = 69,
		DWG_TYPE_VIEWPORT_ENTITY_CONTROL = 70,
		DWG_TYPE_MLINESTYLE = 73,
		DWG_TYPE_FREMDA = 79
	} Dwg_Object_Type;

/**
 Struct for traktiloj.
 */
	typedef struct _dwg_traktilo
	{
		unsigned char kodo;
		unsigned char kiom;
		long unsigned int value;
	} Dwg_Traktilo;

/* OBJEKTOJ *******************************************************************/
/**
 Struct for textj - UNUSED (0)
 */
	typedef int Dwg_Estajxo_UNUSED;

/**
 Struct for textj - TEXT (1)
 */
	typedef struct _dwg_estajxo_TEXT
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
	} Dwg_Estajxo_TEXT;

/**
 Struct for atributoj - ATTRIB (2)
 */
	typedef struct _dwg_estajxo_ATTRIB
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
	} Dwg_Estajxo_ATTRIB;

/**
 Struct for atributo-difinoj - ATTDEF (3)
 */
	typedef struct _dwg_estajxo_ATTDEF
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
	} Dwg_Estajxo_ATTDEF;

/**
 Struct for blokoj - BLOCK (4)
 */
	typedef struct _dwg_estajxo_BLOCK
	{
		unsigned char *nomo;
	} Dwg_Estajxo_BLOCK;

/**
 Struct for blokfinoj - ENDBLK (5)
 */
	typedef struct _dwg_estajxo_ENDBLK
	{
	} Dwg_Estajxo_ENDBLK;

/**
 Struct for enmetoj - SEQEND (6)
 */
	typedef struct _dwg_estajxo_SEQNED
	{
	} Dwg_Estajxo_SEQEND;

/**
 Struct for enmetoj - INSERT (7)
 */
	typedef struct _dwg_estajxo_INSERT
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
		} skalo;
		double turnang;
		struct
		{
			double x;
			double y;
			double z;
		} extrusion;
		unsigned char kun_attrib;
	} Dwg_Estajxo_INSERT;

/**
 Struct for multoblaj enmetoj - MINSERT (8)
 */
	typedef struct _dwg_estajxo_MINSERT
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
		} skalo;
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
			unsigned int kiom;
			double dx;
		} kol;
		struct
		{
			unsigned int kiom;
			double dy;
		} lin;
	} Dwg_Estajxo_MINSERT;

/**
 Struct for ?? - ?? (9)
 */

/**
 Struct for verticoj - VERTEX_2D (10)
 */
	typedef struct _dwg_estajxo_VERTEX_2D
	{
		unsigned char indikiloj;
		double x0;
		double y0;
		double z0;
		double eklargxo;
		double finlargxo;
		double protub;
		double tangxdir;
	} Dwg_Estajxo_VERTEX_2D;

/**
 Struct for verticoj - VERTEX_3D (11)
 */
	typedef struct _dwg_estajxo_VERTEX_3D
	{
		unsigned char indikiloj;
		double x0;
		double y0;
		double z0;
	} Dwg_Estajxo_VERTEX_3D;

/**
 Struct for verticoj - VERTEX_MESH (12) - same as VERTEX_3D
 */
	typedef Dwg_Estajxo_VERTEX_3D Dwg_Estajxo_VERTEX_MESH;

/**
 Struct for verticoj - VERTEX_PFACE (13) - same as VERTEX_3D
 */
	typedef Dwg_Estajxo_VERTEX_3D Dwg_Estajxo_VERTEX_PFACE;

/**
 Struct for verticoj - VERTEX_PFACE_FACE (14)
 */
	typedef struct _dwg_estajxo_VERTEX_PFACE_FACE
	{
		unsigned int vertind[4];
	} Dwg_Estajxo_VERTEX_PFACE_FACE;

/**
 Struct for:  2D POLYLINE (15)
 */
	typedef struct _dwg_estajxo_POLYLINE_2D
	{
		unsigned int indikiloj;
		unsigned int kurbtipo;
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
	} Dwg_Estajxo_POLYLINE_2D;

/**
 Struct for:  3D POLYLINE (16)
 */
	typedef struct _dwg_estajxo_POLYLINE_3D
	{
		unsigned char indikiloj_1;
		unsigned char indikiloj_2;
	} Dwg_Estajxo_POLYLINE_3D;

/**
 Struct for arkoj - ARC (17)
 */
	typedef struct _dwg_estajxo_ARC
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
	} Dwg_Estajxo_ARC;

/**
 Struct for cirkloj - CIRCLE (18)
 */
	typedef struct _dwg_estajxo_CIRCLE
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
	} Dwg_Estajxo_CIRCLE;

/**
 Struct for linioj - LINE (19)
 */
	typedef struct _dwg_estajxo_LINE
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
	} Dwg_Estajxo_LINE;

/**
 Struct for puktoj - POINT (27)
 */
	typedef struct _dwg_estajxo_POINT
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
	} Dwg_Estajxo_POINT;

/**
 Struct for:  3D FACE (28)
 */
	typedef struct _dwg_estajxo_3D_FACE
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
	} Dwg_Estajxo_3D_FACE;

/**
 Struct for elipsoj - ELLIPSE (35)
 */
	typedef struct _dwg_estajxo_ELLIPSE
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
	} Dwg_Estajxo_ELLIPSE;

/**
 Struct for radioj - RAY (40)
 */
	typedef struct _dwg_estajxo_RAY
	{
		double x0;
		double y0;
		double z0;
		double x1;
		double y1;
		double z1;
	} Dwg_Estajxo_RAY;

/**
 Struct for textj - MTEXT (40)
 */
	typedef struct _dwg_estajxo_MTEXT
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
		unsigned char ia_bito;
	} Dwg_Estajxo_MTEXT;

/**
 Struct for tavoloj - LAYER (51)
 */
	typedef struct _dwg_ordinarajxo_LAYER
	{
		char *nomo;
		unsigned char bito64;
		unsigned int xrefi;
		unsigned char xrefdep;
		unsigned int ecoj;
		unsigned int colour;
	} Dwg_Ordinarajxo_LAYER;

/**
 Struct for arangxo - LAYOUT (502?)
 */
	typedef struct _dwg_ordinarajxo_LAYOUT
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
			char *grandeco;
			double dx;
			double dy;
			unsigned int unuoj;
			unsigned int rotacio;
			unsigned int tipo;
			double x_min;
			double y_min;
			double x_maks;
			double y_maks;
			char *nomo;
			struct
			{
				double A; // A:B (ekz: 1:10, 1:2, 50:1, ktp)
				double B;
				unsigned int tipo;
				double faktoro;
			} skalo;
			char *stilfolio;
			double x0;
			double y0;
		} pagxo;

		char *nomo;
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
		unsigned int rigardtipo;
		struct
		{
			double x_min;
			double y_min;
			double z_min;
			double x_maks;
			double y_maks;
			double z_maks;
		} limo;
	} Dwg_Ordinarajxo_LAYOUT;

/* OBJEKTOJ - FINO ************************************************************/

/**
 Strukturo de rikordoj por atributoj de estajxoj.
 */
	typedef struct _dwg_object_estajxo
	{
		union
		{
			Dwg_Estajxo_UNUSED *UNUSED;
			Dwg_Estajxo_TEXT *TEXT;
			Dwg_Estajxo_ATTRIB *ATTRIB;
			Dwg_Estajxo_ATTDEF *ATTDEF;
			Dwg_Estajxo_BLOCK *BLOCK;
			Dwg_Estajxo_ENDBLK *ENDBLK;
			Dwg_Estajxo_INSERT *INSERT;
			Dwg_Estajxo_MINSERT *MINSERT;
			Dwg_Estajxo_VERTEX_2D *VERTEX_2D;
			Dwg_Estajxo_VERTEX_3D *VERTEX_3D;
			Dwg_Estajxo_VERTEX_PFACE_FACE *VERTEX_PFACE_FACE;
			Dwg_Estajxo_POLYLINE_2D *POLYLINE_2D;
			Dwg_Estajxo_POLYLINE_3D *POLYLINE_3D;
			Dwg_Estajxo_ARC *ARC;
			Dwg_Estajxo_CIRCLE *CIRCLE;
			Dwg_Estajxo_LINE *LINE;
			Dwg_Estajxo_POINT *POINT;
			Dwg_Estajxo_ELLIPSE *ELLIPSE;
			Dwg_Estajxo_RAY *RAY;
			Dwg_Estajxo_MTEXT *MTEXT;
		} tio;

		long unsigned int bitgrandeco;
		Dwg_Traktilo traktilo;

		unsigned int kromdat_kiom;
		Dwg_Traktilo kromdat_trakt;
		unsigned char *kromdat;

		unsigned char picture_ekzistas;
		long unsigned int picture_kiom;
		unsigned char *picture;

		unsigned char regime;
		long unsigned int reagilo_kiom;
		unsigned char senligiloj;
		unsigned int colour;
		double linitiposkalo;
		unsigned char linitipo;
		unsigned char printstilo;
		unsigned int malvidebleco;
		unsigned char linithickness;

		unsigned int traktref_kiom;
		Dwg_Traktilo *traktref;
	} Dwg_Object_Estajxo;

/**
 Strukturo de rikordoj por atributoj de ordinaraj objectj.
 */
	typedef struct _dwg_object_ordinarajxo
	{
		union
		{
			Dwg_Ordinarajxo_LAYER *LAYER;
			Dwg_Ordinarajxo_LAYOUT *LAYOUT;
		} tio;

		long unsigned int bitgrandeco;
		Dwg_Traktilo traktilo;

		unsigned int kromdat_kiom;
		Dwg_Traktilo kromdat_trakt;
		unsigned char *kromdat;

		long unsigned int reagilo_kiom;

		unsigned int traktref_kiom;
		Dwg_Traktilo *traktref;
	} Dwg_Object_Ordinarajxo;

/**
 Gxenerala strukturo de rikordoj por objectj.
 */
	typedef struct _dwg_object
	{
		unsigned int grandeco;
		unsigned int tipo;
		unsigned int ckr;

		Dwg_Object_Supertype supertipo;
		union
		{
			Dwg_Object_Estajxo *estajxo;
			Dwg_Object_Ordinarajxo *ordinarajxo;
			unsigned char *nekonatajxo;
		} tio;

		long unsigned int trakt;
	} Dwg_Objekto;

/**
 Strukturo de rikordoj por klasoj.
 */
	typedef struct _dwg_klaso
	{
		unsigned int numero;
		unsigned int versio;
		unsigned char *apnomo;
		unsigned char *cpliplinomo;
		unsigned char *dxfnomo;
		unsigned char estisfantomo;
		unsigned int eroid;
	} Dwg_Klaso;

/**
 Dwg_Cxeno sama kiel Bit_Cxeno, en "bite.h"
 */
	typedef struct _dwg_chain
	{
		unsigned char *chain;
		long unsigned int kiom;
		long unsigned int bajto;
		unsigned char bito;
	} Dwg_Cxeno;

/**
 Cxefa strukturo de la dwg-datenaro.
 */
	typedef struct _dwg_strukturo
	{
		struct
		{
			char versio[6];
			unsigned int kodpagxo;
			unsigned int sekcio_kiom;
			struct
			{
				unsigned char numero;
				long unsigned int adresilo;
				long unsigned int grandeco;
			} sekcio[6];
		} kapo;

#		define DWG_NEKONATA1_KIOM 123
		Dwg_Cxeno nekonata1;

		Dwg_Cxeno picture;

#		define DWG_KIOM_VARIABLOJ 233
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
		} var[DWG_KIOM_VARIABLOJ];

		unsigned int klaso_kiom;
		Dwg_Klaso *klaso;

		long unsigned int object_kiom;
		Dwg_Objekto *object;

		struct
		{
			unsigned char nekonatajxo[6];
			struct
			{
				int kiom;
				unsigned char chain[4];
			} traktrik[14];
		} duakapo;

		long unsigned int mezuro;

		unsigned int dwg_ot_layout;

	} Dwg_Structure;


/*--------------------------------------------------
 * Functions
 */

	int dwg_read_file (char *filename, Dwg_Structure * dwg);

	void dwg_print (Dwg_Structure * dwg);

#ifdef __cplusplus
}
#endif

#endif
