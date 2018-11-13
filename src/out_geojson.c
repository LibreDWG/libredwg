/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * out_geojson.c: write as GeoJSON
 * written by Reini Urban
 */
/* TODO: Arc, Circle, Ellipsis, Bulge (Curve) tessellation.
 *       ocs/ucs transforms, explode of inserts?
 *       NOCOMMA and \n not with stdout. stdout is line-buffered,
 *       so NOCOMMA cannot backup past the previous \n to delete the comma.
 *       We really have to add the comma before, not after, and special case the first
 *       field, not the last to omit the comma.
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "decode.h"
#include "out_json.h"

#define DWG_LOGLEVEL DWG_LOGLEVEL_NONE
#include "logging.h"
#include "dwg_api.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;


/*--------------------------------------------------------------------------------
 * See http://geojson.org/geojson-spec.html
 * Arc, AttributeDefinition, BlockReference, Ellipse, Hatch, Line,
   MText, Point, Polyline, Spline, Text =>
 * Point, LineString, Polygon, MultiPoint, MultiLineString, MultiPolygon
 * { "type": "FeatureCollection",
     "features": [
       { "type": "Feature",
         "properties":
           { "Layer": "SomeLayer",
             "SubClasses": "AcDbEntity:AcDbLine",
             "ExtendedEntity": null,
             "Linetype": null,
             "EntityHandle": "8B",
             "Text": null
           }, 
         "geometry":
           { "type": "LineString",
             "coordinates": [ [ 370.858611653430728, 730.630303522043732, 0.0 ], [ 450.039756420260289, 619.219273076899071, 0.0 ] ] 
           }
       },
     ], ...
   }
 *
 * MACROS
 */

#define ACTION geojson
#define IS_PRINT

#define PREFIX   for (int _i=0; _i<dat->bit; _i++) { fprintf (dat->fh, "  "); }
#define ARRAY     { PREFIX fprintf (dat->fh, "[\n"); dat->bit++; }
#define SAMEARRAY { PREFIX fprintf (dat->fh, "["); dat->bit++; }
#define ENDARRAY  { dat->bit--; PREFIX fprintf (dat->fh, "],\n"); }
#define LASTENDARRAY { dat->bit--; PREFIX fprintf (dat->fh, "]\n"); }
#define HASH      { PREFIX fprintf (dat->fh, "{\n"); dat->bit++; }
#define SAMEHASH  { fprintf (dat->fh, "{\n"); dat->bit++; }
#define ENDHASH   { dat->bit--; PREFIX fprintf (dat->fh, "},\n"); }
#define LASTENDHASH { dat->bit--; PREFIX fprintf (dat->fh, "}\n"); }
#define SECTION(name) { PREFIX fprintf (dat->fh, "\"%s\": [\n", #name); dat->bit++; }
#define ENDSEC()  ENDARRAY
#define NOCOMMA   fseek(dat->fh, -2, SEEK_CUR)
#define PAIR_S(name, value) \
    { PREFIX fprintf(dat->fh, "\"" #name "\": \"%s\",\n", value); }
#define LASTPAIR_S(name, value) \
    { PREFIX fprintf(dat->fh, "\"" #name "\": \"%s\"\n", value); }
#define PAIR_NULL(name) \
    { PREFIX fprintf(dat->fh, "\"" #name "\": null,\n"); }
#define KEY(name) \
    { PREFIX fprintf(dat->fh, "\"" #name "\": "); }
#define GEOMETRY(name) \
    { KEY(geometry); SAMEHASH; \
    PAIR_S(type, #name) }
#define ENDGEOMETRY LASTENDHASH

//#define VALUE(value,type,dxf)
//    fprintf(dat->fh, FORMAT_##type, value)
//#define VALUE_RC(value,dxf) VALUE(value, RC, dxf)

#define FIELD(name,type,dxf)
//    PREFIX fprintf(dat->fh, "\"" #name "\": " FORMAT_##type ",\n", _obj->name)
#define _FIELD(name,type,value)
//    PREFIX fprintf(dat->fh, "\"" #name "\": " FORMAT_##type ",\n", obj->name)
#define ENT_FIELD(name,type,value)
//    PREFIX fprintf(dat->fh, "\"" #name "\": " FORMAT_##type ",\n", _ent->name)
#define FIELD_CAST(name,type,cast,dxf) FIELD(name,cast,dxf)
#define FIELD_TRACE(name,type)
#define FIELD_TEXT(name,str) 
#define FIELD_TEXT_TU(name,wstr) 

#define FIELD_VALUE(name) _obj->name
#define ANYCODE -1
// todo: only the name, not the ref
#define FIELD_HANDLE(name, handle_code, dxf)
#define FIELD_DATAHANDLE(name, code, dxf)
#define FIELD_HANDLE_N(name, vcount, handle_code, dxf)
#define FIELD_B(name,dxf)   FIELD(name, B, dxf)
#define FIELD_BB(name,dxf)  FIELD(name, BB, dxf)
#define FIELD_3B(name,dxf)  FIELD(name, 3B, dxf)
#define FIELD_BS(name,dxf)  FIELD(name, BS, dxf)
#define FIELD_BL(name,dxf)  FIELD(name, BL, dxf)
#define FIELD_BLL(name,dxf) FIELD(name, BLL, dxf)
#define FIELD_BD(name,dxf)  FIELD(name, BD, dxf)
#define FIELD_RC(name,dxf)  FIELD(name, RC, dxf)
#define FIELD_RS(name,dxf)  FIELD(name, RS, dxf)
#define FIELD_RD(name,dxf)  FIELD(name, RD, dxf)
#define FIELD_RL(name,dxf)  FIELD(name, RL, dxf)
#define FIELD_RLL(name,dxf) FIELD(name, RLL, dxf)
#define FIELD_MC(name,dxf)  FIELD(name, MC, dxf)
#define FIELD_MS(name,dxf)  FIELD(name, MS, dxf)
#define FIELD_TF(name,len,dxf)  FIELD_TEXT(name, _obj->name)
#define FIELD_TFF(name,len,dxf) FIELD_TEXT(name, _obj->name)
#define FIELD_TV(name,dxf)      FIELD_TEXT(name, _obj->name)
#define FIELD_TU(name,dxf)      FIELD_TEXT_TU(name, (BITCODE_TU)_obj->name)
#define FIELD_T(name,dxf)
//  { if (dat->version >= R_2007) { FIELD_TU(name, dxf); }
//    else                        { FIELD_TV(name, dxf); } }
#define FIELD_BT(name,dxf)    FIELD(name, BT, dxf);
#define FIELD_4BITS(name,dxf) FIELD(name,4BITS,dxf)
#define FIELD_BE(name,dxf)    FIELD_3RD(name,dxf)
#define FIELD_DD(name, _default, dxf) \
    PREFIX fprintf(dat->fh, FORMAT_DD ", ", _obj->name)
#define FIELD_2DD(name, d1, d2, dxf)
#define FIELD_3DD(name, def, dxf)
#define FIELD_2RD(name,dxf) 
#define FIELD_2BD(name,dxf)
#define FIELD_2BD_1(name,dxf)
#define FIELD_3RD(name,dxf) ;
#define FIELD_3BD(name,dxf)
#define FIELD_3BD_1(name,dxf)
#define VALUE_2DPOINT(px, py) \
  PREFIX fprintf(dat->fh, "[ " FORMAT_DD ", " FORMAT_DD " ],\n", px, py)
#define LASTVALUE_2DPOINT(px, py) \
  PREFIX fprintf(dat->fh, "[ " FORMAT_DD ", " FORMAT_DD " ]\n", px, py)
#define FIELD_2DPOINT(name)     VALUE_2DPOINT(_obj->name.x, _obj->name.y)
#define LASTFIELD_2DPOINT(name) LASTVALUE_2DPOINT(_obj->name.x, _obj->name.y)
#define VALUE_3DPOINT(px, py, pz) \
    PREFIX fprintf(dat->fh, "[ " FORMAT_DD ", " FORMAT_DD ", " FORMAT_DD " ],\n", \
            px, py, pz)
#define LASTVALUE_3DPOINT(px, py, pz) \
    PREFIX fprintf(dat->fh, "[ " FORMAT_DD ", " FORMAT_DD ", " FORMAT_DD " ]\n", \
            px, py, pz)
#define FIELD_3DPOINT(name)     VALUE_3DPOINT(_obj->name.x, _obj->name.y, _obj->name.z)
#define LASTFIELD_3DPOINT(name) LASTVALUE_3DPOINT(_obj->name.x, _obj->name.y, _obj->name.z)
#define FIELD_CMC(name,dxf1,dxf2)
#define FIELD_TIMEBLL(name,dxf)

//FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size, dxf)\
    ARRAY; \
    for (vcount=0; vcount < (BITCODE_BL)size; vcount++)\
      {\
        PREFIX fprintf(dat->fh, "\"" #name "\": " FORMAT_##type ",\n", _obj->name[vcount]); \
      }\
    if (size) NOCOMMA;\
    ENDARRAY;
#define FIELD_VECTOR_T(name, size, dxf)\
    ARRAY; \
    PRE (R_2007) { \
      for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++) { \
        PREFIX fprintf(dat->fh, "\"" #name "\": \"%s\",\n", _obj->name[vcount]); \
      }\
    } else { \
      for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++)\
        FIELD_TEXT_TU(name, _obj->name[vcount]); \
    } \
    if (_obj->size) NOCOMMA;\
    ENDARRAY;

#define FIELD_VECTOR(name, type, size, dxf) FIELD_VECTOR_N(name, type, _obj->size, dxf)

#define FIELD_2RD_VECTOR(name, size, dxf)\
  ARRAY;\
  for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++)\
    {\
      FIELD_2RD(name[vcount], dxf);\
    }\
  if (_obj->size) NOCOMMA;\
  ENDARRAY;

#define FIELD_2DD_VECTOR(name, size, dxf)\
  ARRAY;\
  FIELD_2RD(name[0], 0);\
  for (vcount = 1; vcount < (BITCODE_BL)_obj->size; vcount++)\
    {\
      FIELD_2DD(name[vcount], FIELD_VALUE(name[vcount - 1].x), FIELD_VALUE(name[vcount - 1].y), dxf);\
    }\
  if (_obj->size) NOCOMMA;\
  ENDARRAY;

#define FIELD_3DPOINT_VECTOR(name, size, dxf)\
  ARRAY;\
  for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++)\
    {\
      FIELD_3DPOINT(name[vcount], dxf);\
    }\
  if (_obj->size) NOCOMMA;\
  ENDARRAY;

#define WARN_UNSTABLE_CLASS \
      LOG_WARN("Unstable Class %s %d %s (0x%x%s) -@%ld", is_entity ? "entity" : "object",\
               klass->number, dxfname, klass->proxyflag,\
               klass->wasazombie ? " was proxy" : "",\
               obj->address + obj->size)

// common properties
static void
dwg_geojson_feature(Bit_Chain *restrict dat, Dwg_Object *restrict obj,
                    const char *restrict subclass)
{
  char tmp[64];
  PAIR_S(type, "Feature");
  KEY(properties);
  SAMEHASH;
  PAIR_S(Layer, "0"); //TODO layer name
    PAIR_S(SubClasses, subclass);
    PAIR_NULL(ExtendedEntity);
    PAIR_NULL(Linetype);

    sprintf(tmp, "%lX", obj->handle.value);
    PAIR_S(EntityHandle, tmp);
    //TODO if has name or text
    if (obj->type == DWG_TYPE_GEOPOSITIONMARKER) {
      Dwg_Entity_GEOPOSITIONMARKER *_obj = obj->tio.entity->tio.GEOPOSITIONMARKER;
      PAIR_S(Text, _obj->text);
    }
    else
      PAIR_NULL(Text);
  ENDHASH;
}

#define FEATURE(subclass, obj) HASH; dwg_geojson_feature(dat, obj, #subclass)
#define ENDFEATURE ENDHASH

/* returns 0 if object could be printed
 */
static int
dwg_geojson_variable_type(Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
                          Dwg_Object *restrict obj)
{
  int i;
  char *dxfname;
  Dwg_Class *klass;
  int is_entity;

  i = obj->type - 500;
  if (i < 0 || i >= (int)dwg->num_classes)
    return DWG_ERR_INVALIDTYPE;

  klass = &dwg->dwg_class[i];
  if (!klass || ! klass->dxfname)
    return DWG_ERR_INTERNALERROR;
  dxfname = klass->dxfname;
  // almost always false
  is_entity = dwg_class_is_entity(klass);
  
  if (!strcmp(dxfname, "LWPOLYLINE"))
    {
      BITCODE_BL j;
      Dwg_Entity_LWPOLYLINE *_obj = obj->tio.entity->tio.LWPOLYLINE;
      //TODO: if closed and num_points > 3 use a Polygon
      FEATURE(AcDbEntity:AcDbLwPolyline, obj);
      GEOMETRY(LineString);
      KEY(coordinates);
        ARRAY;
        for (j = 0; j < _obj->num_points-1 ; j++)
        {
          FIELD_2DPOINT(points[j]);
        }
        LASTFIELD_2DPOINT(points[j+1]);
        LASTENDARRAY;
      ENDGEOMETRY;
      ENDFEATURE;
      return 0;
    }
  if (!strcmp(dxfname, "GEODATA"))
    {
      Dwg_Object_GEODATA *_obj = obj->tio.object->tio.GEODATA;
      WARN_UNSTABLE_CLASS;
      FEATURE(AcDbObject:AcDbGeoData, obj);
      //which fields?
      ENDFEATURE;
      return 0;
    }
  if (!strcmp(dxfname, "GEOPOSITIONMARKER"))
    {
      Dwg_Entity_GEOPOSITIONMARKER *_obj = obj->tio.entity->tio.GEOPOSITIONMARKER;
      WARN_UNSTABLE_CLASS;
      // now even with text
      FEATURE(AcDbEntity:AcDbGeoPositionMarker, obj);
      GEOMETRY(Point);
      KEY(coordinates);
      if (fabs(_obj->position.z) > 0.000001)
        {
          VALUE_3DPOINT(_obj->position.x, _obj->position.y, _obj->position.z);
        }
      else
        {
          VALUE_2DPOINT(_obj->position.x, _obj->position.y);
        }
      ENDGEOMETRY;
      ENDFEATURE;
      return 0;
    }

  return DWG_ERR_UNHANDLEDCLASS;
}

static void
dwg_geojson_object(Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  switch (obj->type)
    {
    case DWG_TYPE_INSERT:
      {
        Dwg_Entity_INSERT *_obj = obj->tio.entity->tio.INSERT;
        FEATURE(AcDbEntity:AcDbBlockReference, obj);
        //TODO: explode insert into a GeometryCollection
        GEOMETRY(Point);
        KEY(coordinates);
        ARRAY; LASTFIELD_3DPOINT(ins_pt); LASTENDARRAY;
        ENDGEOMETRY;
        ENDFEATURE;
      }
      break;
    case DWG_TYPE_MINSERT:
      //dwg_geojson_MINSERT(dat, obj);
      break;
    case DWG_TYPE_VERTEX_2D:
      //dwg_geojson_VERTEX_2D(dat, obj);
      break;
    case DWG_TYPE_VERTEX_3D:
      //dwg_geojson_VERTEX_3D(dat, obj);
      break;
    case DWG_TYPE_VERTEX_MESH:
      //dwg_geojson_VERTEX_MESH(dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE:
      //dwg_geojson_VERTEX_PFACE(dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE_FACE:
      //dwg_geojson_VERTEX_PFACE_FACE(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_2D:
      {
        int error;
        BITCODE_BL j, numpts;
        dwg_point_2d *pts;
        FEATURE(AcDbEntity:AcDbPolyline, obj);
        GEOMETRY(MultiLineString);
        KEY(coordinates);
          ARRAY;
          numpts = dwg_object_polyline_2d_get_numpoints(obj, &error);
          pts = dwg_object_polyline_2d_get_points(obj, &error);
          for (j=0; j<numpts; j++)
            {
              if (j == numpts-1)
                {
                  LASTVALUE_2DPOINT(pts[j].x, pts[j].y);
                }
              else
                {
                  VALUE_2DPOINT(pts[j].x, pts[j].y);
                }
            }
          LASTENDARRAY;
        ENDGEOMETRY;
        ENDFEATURE;
      }
      break;
    case DWG_TYPE_POLYLINE_3D:
      {
        int error;
        BITCODE_BL j, numpts;
        dwg_point_3d *pts;
        FEATURE(AcDbEntity:AcDbPolyline, obj);
        GEOMETRY(MultiLineString);
        KEY(coordinates);
          ARRAY;
          numpts = dwg_object_polyline_3d_get_numpoints(obj, &error);
          pts = dwg_object_polyline_3d_get_points(obj, &error);
          for (j=0; j<numpts; j++)
            {
              if (j == numpts-1)
                {
                  LASTVALUE_3DPOINT(pts[j].x, pts[j].y, pts[j].z);
                }
              else
                {
                  VALUE_3DPOINT(pts[j].x, pts[j].y, pts[j].z);
                }
            }
          LASTENDARRAY;
        ENDGEOMETRY;
        ENDFEATURE;
      }
      break;
    case DWG_TYPE_ARC:
      //dwg_geojson_ARC(dat, obj);
      break;
    case DWG_TYPE_CIRCLE:
      //dwg_geojson_CIRCLE(dat, obj);
      break;
    case DWG_TYPE_LINE:
      {
        Dwg_Entity_LINE *_obj = obj->tio.entity->tio.LINE;
        FEATURE(AcDbEntity:AcDbLine, obj);
        GEOMETRY(LineString);
        KEY(coordinates);
          ARRAY;
          FIELD_3DPOINT(start); LASTFIELD_3DPOINT(end);
          LASTENDARRAY;
        ENDGEOMETRY;
        ENDFEATURE;
      }
      break;
    case DWG_TYPE_POINT:
      {
        Dwg_Entity_POINT *_obj = obj->tio.entity->tio.POINT;
        FEATURE(AcDbEntity:AcDbPoint, obj);
        GEOMETRY(Point);
        KEY(coordinates);
        if (fabs(_obj->z) > 0.000001)
          {
            VALUE_3DPOINT(_obj->x, _obj->y, _obj->z);
          }
        else
          {
            VALUE_2DPOINT(_obj->x, _obj->y);
          }
        ENDGEOMETRY;
        ENDFEATURE;
      }
      break;
    case DWG_TYPE__3DFACE:
      //dwg_geojson__3DFACE(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_PFACE:
      //dwg_geojson_POLYLINE_PFACE(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_MESH:
      //dwg_geojson_POLYLINE_MESH(dat, obj);
      break;
    case DWG_TYPE_SOLID:
      //dwg_geojson_SOLID(dat, obj);
      break;
    case DWG_TYPE_TRACE:
      //dwg_geojson_TRACE(dat, obj);
      break;
    case DWG_TYPE_ELLIPSE:
      //dwg_geojson_ELLIPSE(dat, obj);
      break;
    case DWG_TYPE_SPLINE:
      //dwg_geojson_SPLINE(dat, obj);
      break;
    case DWG_TYPE_REGION:
      //dwg_geojson_REGION(dat, obj);
      break;
    case DWG_TYPE_HATCH:
      //dwg_geojson_HATCH(dat, obj);
      break;
    case DWG_TYPE__3DSOLID:
      //dwg_geojson__3DSOLID(dat, obj);
      break; /* Check the type of the object
              */
    case DWG_TYPE_BODY:
      //dwg_geojson_BODY(dat, obj);
      break;
    case DWG_TYPE_RAY:
      //dwg_geojson_RAY(dat, obj);
      break;
    case DWG_TYPE_XLINE:
      //dwg_geojson_XLINE(dat, obj);
      break;
    case DWG_TYPE_MTEXT:
      //dwg_geojson_MTEXT(dat, obj);
      break;
    case DWG_TYPE_MLINE:
      //dwg_geojson_MLINE(dat, obj);
      break;
    case DWG_TYPE_LWPOLYLINE:
      //dwg_geojson_LWPOLYLINE(dat, obj);
      break;
    default:
      if (obj->type != obj->parent->layout_number)
        dwg_geojson_variable_type(obj->parent, dat, obj);
    }
}

static int
geojson_entities_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_BL i;

  SECTION(features);
  for (i=0; i < dwg->num_objects; i++)
    {
      Dwg_Object *obj = &dwg->object[i];
      dwg_geojson_object(dat, obj);
    }
  NOCOMMA;
  ENDSEC();
  return 0;
}

EXPORT int
dwg_write_geojson(Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  //const int minimal = dwg->opts & 0x10;
  char date[12] = "YYYY-MM-DD";
  time_t rawtime;

  HASH;
  PAIR_S(type, "FeatureCollection");

  //array of features
  if (geojson_entities_write (dat, dwg))
    goto fail;

  KEY(geocoding);
  HASH;
    time(&rawtime);
    strftime(date, 12, "%Y-%m-%d", localtime(&rawtime));
    PAIR_S(creation_date, date);
    KEY(generator);
    HASH;
      KEY(author); HASH; PAIR_S(name, "dwgread"); ENDHASH;
      PAIR_S(package, PACKAGE_NAME);
      LASTPAIR_S(version, PACKAGE_VERSION);
    LASTENDHASH;
    //PAIR_S(license, "?");
  LASTENDHASH;

  LASTENDHASH;
  return 0;
 fail:
  return 1;
}

#undef IS_PRINT
