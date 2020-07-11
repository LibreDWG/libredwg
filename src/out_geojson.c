/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2023 Free Software Foundation, Inc.                   */
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
/* FIXME: Arc, Circle, Ellipsis, Bulge (Curve) arc_split.
 * TODO: ocs/ucs transforms, explode of inserts?
 *       NOCOMMA:
 *         We really have to add the comma before, not after, and special case
 *         the first field, not the last to omit the comma.
 *       GeoJSON 2008 or newer RFC7946
 * https://tools.ietf.org/html/rfc7946#appendix-B For the new format we need to
 * follow the right-hand rule for orientation (counterclockwise polygons).
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#define IS_PRINT
#include "dwg.h"
#define DWG_LOGLEVEL DWG_LOGLEVEL_NONE
#include "logging.h"
#include "dwg_api.h"

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "decode.h"
#include "out_json.h"
#include "geom.h"

/* the current version per spec block */
// static unsigned int cur_ver = 0;

/* https://tools.ietf.org/html/rfc7946#section-11.2 recommends.
   Set via --with-geojson-precision=rfc */
#undef FORMAT_RD
#ifndef GEOJSON_PRECISION
#  define GEOJSON_PRECISION 6
#endif
#define FORMAT_RD "%0." _XSTR (GEOJSON_PRECISION) "f"
// #define FORMAT_RD "%f"
#undef FORMAT_BD
#define FORMAT_BD FORMAT_RD

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
             "coordinates": [
               [ 370.858611, 730.630303 ],
               [ 450.039756, 619.219273 ]
             ]
           }
       },
     ], ...
   }
 *
 * MACROS
 */

#define ACTION geojson

#define PREFIX                                                                \
  for (int _i = 0; _i < dat->bit; _i++)                                       \
    {                                                                         \
      fprintf (dat->fh, "  ");                                                \
    }
#define ARRAY                                                                 \
  {                                                                           \
    PREFIX fprintf (dat->fh, "[\n");                                          \
    dat->bit++;                                                               \
  }
#define SAMEARRAY                                                             \
  {                                                                           \
    PREFIX fprintf (dat->fh, "[");                                            \
    dat->bit++;                                                               \
  }
#define ENDARRAY                                                              \
  {                                                                           \
    dat->bit--;                                                               \
    PREFIX fprintf (dat->fh, "],\n");                                         \
  }
#define LASTENDARRAY                                                          \
  {                                                                           \
    dat->bit--;                                                               \
    PREFIX fprintf (dat->fh, "]\n");                                          \
  }
#define HASH                                                                  \
  {                                                                           \
    PREFIX fprintf (dat->fh, "{\n");                                          \
    dat->bit++;                                                               \
  }
#define SAMEHASH                                                              \
  {                                                                           \
    fprintf (dat->fh, "{\n");                                                 \
    dat->bit++;                                                               \
  }
#define ENDHASH                                                               \
  {                                                                           \
    dat->bit--;                                                               \
    PREFIX fprintf (dat->fh, "},\n");                                         \
  }
#define LASTENDHASH                                                           \
  {                                                                           \
    dat->bit--;                                                               \
    PREFIX fprintf (dat->fh, "}\n");                                          \
  }
#define SECTION(name)                                                         \
  {                                                                           \
    PREFIX fprintf (dat->fh, "\"%s\": [\n", #name);                           \
    dat->bit++;                                                               \
  }
#define ENDSEC() ENDARRAY
#define OLD_NOCOMMA fseek (dat->fh, -2, SEEK_CUR)
#define NOCOMMA assert (0 = "NOCOMMA")
// guaranteed non-null str
#define PAIR_Sc(name, str)                                                    \
  {                                                                           \
    const size_t len = strlen (str);                                          \
    if (len < 42)                                                             \
      {                                                                       \
        const size_t _len = 6 * len + 1;                                      \
        char _buf[256];                                                       \
        PREFIX fprintf (dat->fh, "\"" #name "\": \"%s\",\n",                  \
                        json_cquote (_buf, str, _len, dat->codepage));        \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        const size_t _len = 6 * len + 1;                                      \
        char *_buf = (char *)malloc (_len);                                   \
        PREFIX fprintf (dat->fh, "\"" #name "\": \"%s\",\n",                  \
                        json_cquote (_buf, str, _len, dat->codepage));        \
        free (_buf);                                                          \
      }                                                                       \
  }
#define PAIR_S(name, str)                                                     \
  if (str)                                                                    \
  PAIR_Sc (name, str)
#define PAIR_D(name, value)                                                   \
  {                                                                           \
    PREFIX fprintf (dat->fh, "\"" #name "\": %d,\n", value);                  \
  }
// guaranteed non-null str
#define LASTPAIR_Sc(name, value)                                              \
  {                                                                           \
    PREFIX fprintf (dat->fh, "\"" #name "\": \"%s\"\n", value);               \
  }
#define LASTPAIR_S(name, value)                                               \
  if (value)                                                                  \
    {                                                                         \
      PREFIX fprintf (dat->fh, "\"" #name "\": \"%s\"\n", value);             \
    }
#define PAIR_NULL(name)                                                       \
  {                                                                           \
    PREFIX fprintf (dat->fh, "\"" #name "\": null,\n");                       \
  }
#define LASTPAIR_NULL(name)                                                   \
  {                                                                           \
    PREFIX fprintf (dat->fh, "\"" #name "\": null\n");                        \
  }
#define KEY(name)                                                             \
  {                                                                           \
    PREFIX fprintf (dat->fh, "\"" #name "\": ");                              \
  }
#define GEOMETRY(name)                                                        \
  {                                                                           \
    KEY (geometry);                                                           \
    SAMEHASH;                                                                 \
    PAIR_S (type, #name)                                                      \
  }
#define ENDGEOMETRY LASTENDHASH

// #define VALUE(value,type,dxf)
//     fprintf(dat->fh, FORMAT_##type, value)
// #define VALUE_RC(value,dxf) VALUE(value, RC, dxf)

#define FIELD(name, type, dxf)
#define _FIELD(name, type, value)
#define ENT_FIELD(name, type, value)
#define FIELD_CAST(name, type, cast, dxf) FIELD (name, cast, dxf)
#define FIELD_TRACE(name, type)
#define FIELD_TEXT(name, str)
#define FIELD_TEXT_TU(name, wstr)

#define FIELD_VALUE(name) _obj->name
#define ANYCODE -1
// todo: only the name, not the ref
#define FIELD_HANDLE(name, handle_code, dxf)
#define FIELD_DATAHANDLE(name, code, dxf)
#define FIELD_HANDLE_N(name, vcount, handle_code, dxf)
#define FIELD_B(name, dxf) FIELD (name, B, dxf)
#define FIELD_BB(name, dxf) FIELD (name, BB, dxf)
#define FIELD_3B(name, dxf) FIELD (name, 3B, dxf)
#define FIELD_BS(name, dxf) FIELD (name, BS, dxf)
#define FIELD_BL(name, dxf) FIELD (name, BL, dxf)
#define FIELD_BLL(name, dxf) FIELD (name, BLL, dxf)
#define FIELD_BD(name, dxf) FIELD (name, BD, dxf)
#define FIELD_RC(name, dxf) FIELD (name, RC, dxf)
#define FIELD_RS(name, dxf) FIELD (name, RS, dxf)
#define FIELD_RD(name, dxf) FIELD_BD (name, dxf)
#define FIELD_RL(name, dxf) FIELD (name, RL, dxf)
#define FIELD_RLL(name, dxf) FIELD (name, RLL, dxf)
#define FIELD_MC(name, dxf) FIELD (name, MC, dxf)
#define FIELD_MS(name, dxf) FIELD (name, MS, dxf)
#define FIELD_TF(name, len, dxf) FIELD_TEXT (name, _obj->name)
#define FIELD_TFF(name, len, dxf) FIELD_TEXT (name, _obj->name)
#define FIELD_TV(name, dxf) FIELD_TEXT (name, _obj->name)
#define FIELD_TU(name, dxf) FIELD_TEXT_TU (name, (BITCODE_TU)_obj->name)
#define FIELD_T(name, dxf)
//  { if (dat->version >= R_2007) { FIELD_TU(name, dxf); }
//    else                        { FIELD_TV(name, dxf); } }
#define FIELD_BT(name, dxf) FIELD (name, BT, dxf)
#define FIELD_4BITS(name, dxf) FIELD (name, 4BITS, dxf)
#define FIELD_BE(name, dxf) FIELD_3RD (name, dxf)
#define FIELD_2DD(name, def, dxf)
#define FIELD_3DD(name, def, dxf)
#define FIELD_2RD(name, dxf)
#define FIELD_2BD(name, dxf)
#define FIELD_2BD_1(name, dxf)
#define FIELD_3RD(name, dxf) ;
#define FIELD_3BD(name, dxf)
#define FIELD_3BD_1(name, dxf)
#define FIELD_DD(name, _default, dxf)

#define _VALUE_RD(value) fprintf (dat->fh, FORMAT_RD, value)
#ifdef IS_RELEASE
#  define VALUE_RD(value)                                                     \
    {                                                                         \
      if (bit_isnan (value))                                                  \
        _VALUE_RD (0.0);                                                      \
      else                                                                    \
        _VALUE_RD (value);                                                    \
    }
#else
#  define VALUE_RD(value) _VALUE_RD (value)
#endif
#define VALUE_2DPOINT(px, py)                                                 \
  {                                                                           \
    PREFIX fprintf (dat->fh, "[ ");                                           \
    VALUE_RD (px);                                                            \
    fprintf (dat->fh, ", ");                                                  \
    VALUE_RD (py);                                                            \
    fprintf (dat->fh, " ],\n");                                               \
  }
#define LASTVALUE_2DPOINT(px, py)                                             \
  {                                                                           \
    PREFIX fprintf (dat->fh, "[ ");                                           \
    VALUE_RD (px);                                                            \
    fprintf (dat->fh, ", ");                                                  \
    VALUE_RD (py);                                                            \
    fprintf (dat->fh, " ]\n");                                                \
  }
#define FIELD_2DPOINT(name) VALUE_2DPOINT (_obj->name.x, _obj->name.y)
#define LASTFIELD_2DPOINT(name) LASTVALUE_2DPOINT (_obj->name.x, _obj->name.y)
#define VALUE_3DPOINT(px, py, pz)                                             \
  {                                                                           \
    PREFIX fprintf (dat->fh, "[ ");                                           \
    VALUE_RD (px);                                                            \
    fprintf (dat->fh, ", ");                                                  \
    VALUE_RD (py);                                                            \
    if (pz != 0.0)                                                            \
      {                                                                       \
        fprintf (dat->fh, ", ");                                              \
        VALUE_RD (pz);                                                        \
      }                                                                       \
    fprintf (dat->fh, " ],\n");                                               \
  }
#define LASTVALUE_3DPOINT(px, py, pz)                                         \
  {                                                                           \
    PREFIX fprintf (dat->fh, "[ ");                                           \
    VALUE_RD (px);                                                            \
    fprintf (dat->fh, ", ");                                                  \
    VALUE_RD (py);                                                            \
    if (pz != 0.0)                                                            \
      {                                                                       \
        fprintf (dat->fh, ", ");                                              \
        VALUE_RD (pz);                                                        \
      }                                                                       \
    fprintf (dat->fh, " ]\n");                                                \
  }
#define FIELD_3DPOINT(name)                                                   \
  {                                                                           \
    if (_obj->name.z != 0.0)                                                  \
      VALUE_3DPOINT (_obj->name.x, _obj->name.y, _obj->name.z)                \
    else                                                                      \
      FIELD_2DPOINT (name)                                                    \
  }
#define LASTFIELD_3DPOINT(name)                                               \
  {                                                                           \
    if (_obj->name.z != 0.0)                                                  \
      LASTVALUE_3DPOINT (_obj->name.x, _obj->name.y, _obj->name.z)            \
    else                                                                      \
      LASTFIELD_2DPOINT (name)                                                \
  }

#define FIELD_CMC(name, dxf1, dxf2)
#define FIELD_TIMEBLL(name, dxf)

// FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size, dxf)                                 \
  ARRAY;                                                                      \
  for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                       \
    {                                                                         \
      PREFIX fprintf (dat->fh, "\"" #name "\": " FORMAT_##type "%s\n",        \
                      _obj->name[vcount],                                     \
                      vcount == (BITCODE_BL)size - 1 ? "" : ",");             \
    }                                                                         \
  ENDARRAY;

#define FIELD_VECTOR_T(name, type, size, dxf)                                 \
  ARRAY;                                                                      \
  if (!(IS_FROM_TU (dat)))                                                    \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          PREFIX fprintf (dat->fh, "\"" #name "\": \"%s\"%s\n",               \
                          _obj->name[vcount],                                 \
                          vcount == (BITCODE_BL)_obj->size - 1 ? "" : ",");   \
        }                                                                     \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        FIELD_TEXT_TU (name, _obj->name[vcount]);                             \
    }                                                                         \
  ENDARRAY;

#define FIELD_VECTOR(name, type, size, dxf)                                   \
  FIELD_VECTOR_N (name, type, _obj->size, dxf)

#define FIELD_2RD_VECTOR(name, size, dxf)
#define FIELD_2DD_VECTOR(name, size, dxf)

#define FIELD_3DPOINT_VECTOR(name, size, dxf)                                 \
  ARRAY;                                                                      \
  for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)                 \
    {                                                                         \
      if (vcount == (BITCODE_BL)_obj->size - 1)                               \
        LASTFIELD_3DPOINT (name[vcount], dxf)                                 \
      else                                                                    \
        FIELD_3DPOINT (name[vcount], dxf)                                     \
    }                                                                         \
  ENDARRAY;

#define WARN_UNSTABLE_CLASS                                                   \
  LOG_WARN ("Unstable Class %s %d %s (0x%x%s) -@%" PRIuSIZE,                  \
            is_entity ? "entity" : "object", klass->number, dxfname,          \
            klass->proxyflag, klass->is_zombie ? "is_zombie" : "",            \
            obj->address + obj->size)

// ensure counter-clockwise orientation of a closed polygon. 2d only.
static int
normalize_polygon_orient (BITCODE_BL numpts, dwg_point_2d **const pts_p)
{
  double sum = 0.0;
  dwg_point_2d *pts = *pts_p;
  // check orientation
  for (unsigned i = 0; i < numpts - 1; i++)
    {
      sum += (pts[i + 1].x - pts[i].x) * (pts[i + 1].y + pts[i].y);
    }
  if (sum > 0.0) // if clockwise
    {
      // reverse and return a copy
      unsigned last = numpts - 1;
      dwg_point_2d *newpts
          = (dwg_point_2d *)malloc (numpts * sizeof (BITCODE_2RD));
      // fprintf (stderr, "%u pts, sum %f: reverse orient\n", numpts, sum);
      for (unsigned i = 0; i < numpts; i++)
        {
          newpts[i].x = pts[last - i].x;
          newpts[i].y = pts[last - i].y;
        }
      *pts_p = newpts;
      return 1;
    }
  else
    {
      // fprintf (stderr, "%u pts, sum %f: keep orient\n", numpts, sum);
      return 0;
    }
}

// common properties
static void
dwg_geojson_feature (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
                     const char *restrict subclass)
{
  int error;
  char *name;
  char tmp[64];

  PAIR_Sc (type, "Feature");
  snprintf (tmp, sizeof (tmp), FORMAT_HV, obj->handle.value);
  PAIR_Sc (id, tmp);
  KEY (properties);
  SAMEHASH;
  PAIR_S (SubClasses, subclass);
  if (obj->supertype == DWG_SUPERTYPE_ENTITY)
    {
      Dwg_Object *layer
          = obj->tio.entity->layer ? obj->tio.entity->layer->obj : NULL;
      if (layer
          && (layer->fixedtype == DWG_TYPE_LAYER
              || layer->fixedtype == DWG_TYPE_DICTIONARY))
        {
          name = dwg_obj_table_get_name (layer, &error);
          if (!error)
            {
              PAIR_S (Layer, name);
              if (IS_FROM_TU (dat))
                free (name);
            }
        }

      // See #95: index as int or rgb as hexstring
      if (dat->version >= R_2004
          && (obj->tio.entity->color.method == 0xc3     // Truecolor
              || obj->tio.entity->color.method == 0xc2) // Entity
          && obj->tio.entity->color.index == 256)
        {
          snprintf (tmp, sizeof (tmp), "#%06X",
                    obj->tio.entity->color.rgb & 0xffffff);
          PAIR_Sc (Color, tmp);
        }
      else if ((obj->tio.entity->color.index != 256)
               || (dat->version >= R_2004
                   && obj->tio.entity->color.method != 0xc0 // ByLayer
                   && obj->tio.entity->color.method != 0xc1 // ByBlock
                   && obj->tio.entity->color.method != 0xc8 // none
                   ))
        {
          // no names for the first palette entries yet.
          PAIR_D (Color, obj->tio.entity->color.index);
        }

      name = dwg_ent_get_ltype_name (obj->tio.entity, &error);
      if (!error && strNE (name, "ByLayer")) // skip the default
        {
          PAIR_S (Linetype, name);
          if (IS_FROM_TU (dat))
            free (name);
        }
    }

  // if has notes and opt. an mtext frame_text
  if (obj->fixedtype == DWG_TYPE_GEOPOSITIONMARKER)
    {
      Dwg_Entity_GEOPOSITIONMARKER *_obj
          = obj->tio.entity->tio.GEOPOSITIONMARKER;
      if (IS_FROM_TU (dat))
        {
          char *utf8 = bit_convert_TU ((BITCODE_TU)_obj->notes);
          PAIR_S (Text, utf8)
          free (utf8);
        }
      else
        {
          PAIR_S (Text, _obj->notes)
        }
    }
  else if (obj->fixedtype == DWG_TYPE_TEXT)
    {
      Dwg_Entity_TEXT *_obj = obj->tio.entity->tio.TEXT;
      if (IS_FROM_TU (dat))
        {
          char *utf8 = bit_convert_TU ((BITCODE_TU)_obj->text_value);
          PAIR_S (Text, utf8)
          free (utf8);
        }
      else
        {
          PAIR_S (Text, _obj->text_value)
        }
    }
  else if (obj->fixedtype == DWG_TYPE_MTEXT)
    {
      Dwg_Entity_MTEXT *_obj = obj->tio.entity->tio.MTEXT;
      if (IS_FROM_TU (dat))
        {
          char *utf8 = bit_convert_TU ((BITCODE_TU)_obj->text);
          PAIR_S (Text, utf8)
          free (utf8);
        }
      else
        {
          PAIR_S (Text, _obj->text)
        }
    }
  else if (obj->fixedtype == DWG_TYPE_INSERT)
    {
      Dwg_Entity_INSERT *_obj = obj->tio.entity->tio.INSERT;
      Dwg_Object *hdr = dwg_ref_get_object (_obj->block_header, &error);
      if (!error && hdr && hdr->fixedtype == DWG_TYPE_BLOCK_HEADER)
        {
          Dwg_Object_BLOCK_HEADER *_hdr = hdr->tio.object->tio.BLOCK_HEADER;
          char *text;
          if (IS_FROM_TU (dat))
            text = bit_convert_TU ((BITCODE_TU)_hdr->name);
          else
            text = _hdr->name;
          if (text)
            {
              PAIR_S (name, text);
              if (IS_FROM_TU (dat))
                free (text);
            }
        }
    }
  else if (obj->fixedtype == DWG_TYPE_MINSERT)
    {
      Dwg_Entity_MINSERT *_obj = obj->tio.entity->tio.MINSERT;
      Dwg_Object *hdr = dwg_ref_get_object (_obj->block_header, &error);
      if (!error && hdr && hdr->fixedtype == DWG_TYPE_BLOCK_HEADER)
        {
          Dwg_Object_BLOCK_HEADER *_hdr = hdr->tio.object->tio.BLOCK_HEADER;
          char *text;
          if (dat->version >= R_2007)
            text = bit_TU_to_utf8 ((BITCODE_TU)_hdr->name);
          else
            text = _hdr->name;
          if (text)
            {
              PAIR_S (name, text);
              if (dat->version >= R_2007)
                free (text);
            }
        }
    }
  // PAIR_NULL(ExtendedEntity);
  snprintf (tmp, sizeof (tmp), FORMAT_HV, obj->handle.value);
  LASTPAIR_Sc (EntityHandle, tmp);
  ENDHASH;
}

#define FEATURE(subclass, obj)                                                \
  HASH;                                                                       \
  dwg_geojson_feature (dat, obj, #subclass)
#define ENDFEATURE                                                            \
  if (is_last)                                                                \
    LASTENDHASH                                                               \
  else                                                                        \
    ENDHASH

static int
dwg_geojson_LWPOLYLINE (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
                        int is_last)
{
  BITCODE_BL j, last_j;
  Dwg_Entity_LWPOLYLINE *_obj = obj->tio.entity->tio.LWPOLYLINE;
  dwg_point_2d *pts = (dwg_point_2d *)_obj->points;
  if (!_obj->points)
    return 1;

  FEATURE (AcDbEntity : AcDbLwPolyline, obj);
  // TODO bulges, splines, ...

  // if closed and num_points > 3 use a Polygon
  if (_obj->flag & 512 && _obj->num_points > 3)
    {
      int changed
          = normalize_polygon_orient (_obj->num_points, &pts); // RFC7946
      GEOMETRY (Polygon)
      KEY (coordinates);
      ARRAY;
      ARRAY;
      for (j = 0; j < _obj->num_points; j++)
        VALUE_2DPOINT (pts[j].x, pts[j].y)
      LASTVALUE_2DPOINT (pts[0].x, pts[0].y);
      LASTENDARRAY;
      LASTENDARRAY;
      if (changed)
        free (pts);
    }
  else
    {
      GEOMETRY (LineString)
      KEY (coordinates);
      ARRAY;
      last_j = _obj->num_points - 1;
      for (j = 0; j < last_j; j++)
        VALUE_2DPOINT (pts[j].x, pts[j].y);
      LASTVALUE_2DPOINT (pts[last_j].x, pts[last_j].y);
      LASTENDARRAY;
    }
  ENDGEOMETRY;
  ENDFEATURE;
  return 1;
}

/* returns 0 if object could be printed
 */
static int
dwg_geojson_variable_type (Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
                           Dwg_Object *restrict obj, int is_last)
{
  int i;
  char *dxfname;
  Dwg_Class *klass;
  int is_entity;

  i = obj->fixedtype - 500;
  if (i < 0 || i >= (int)dwg->num_classes)
    return 0;
  if (obj->fixedtype == DWG_TYPE_UNKNOWN_ENT
      || obj->fixedtype == DWG_TYPE_UNKNOWN_OBJ)
    return DWG_ERR_UNHANDLEDCLASS;

  klass = &dwg->dwg_class[i];
  if (!klass || !klass->dxfname)
    return DWG_ERR_INTERNALERROR;
  dxfname = klass->dxfname;
  // almost always false
  is_entity = dwg_class_is_entity (klass);

  if (strEQc (dxfname, "LWPOLYLINE"))
    {
      return dwg_geojson_LWPOLYLINE (dat, obj, is_last);
    }
  /*
  if (strEQc (dxfname, "GEODATA"))
    {
      Dwg_Object_GEODATA *_obj = obj->tio.object->tio.GEODATA;
      WARN_UNSTABLE_CLASS;
      FEATURE (AcDbObject : AcDbGeoData, obj);
      // which fields? transformation for the world-coordinates?
      // crs links of type proj4, ogcwkt, esriwkt or such?
      ENDFEATURE;
      return 0;
    }
  */
  if (strEQc (dxfname, "GEOPOSITIONMARKER"))
    {
      Dwg_Entity_GEOPOSITIONMARKER *_obj
          = obj->tio.entity->tio.GEOPOSITIONMARKER;
      WARN_UNSTABLE_CLASS;
      // now even with text
      FEATURE (AcDbEntity : AcDbGeoPositionMarker, obj);
      GEOMETRY (Point);
      KEY (coordinates);
      if (fabs (_obj->position.z) > 0.000001)
        VALUE_3DPOINT (_obj->position.x, _obj->position.y, _obj->position.z)
      else
        VALUE_2DPOINT (_obj->position.x, _obj->position.y);
      ENDGEOMETRY;
      ENDFEATURE;
      return 1;
    }

  return 0;
}

static int
dwg_geojson_object (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
                    int is_last)
{
  switch (obj->fixedtype)
    {
    case DWG_TYPE_INSERT: // Just the insertion point yet
      {
        Dwg_Entity_INSERT *_obj = obj->tio.entity->tio.INSERT;
        FEATURE (AcDbEntity : AcDbBlockReference, obj);
        // TODO: explode insert into a GeometryCollection
        GEOMETRY (Point);
        KEY (coordinates);
        LASTFIELD_3DPOINT (ins_pt);
        ENDGEOMETRY;
        ENDFEATURE;
        return 1;
      }
    case DWG_TYPE_MINSERT:
      // a grid of INSERT's (named points)
      // dwg_geojson_MINSERT(dat, obj);
      LOG_TRACE ("MINSERT not yet supported")
      break;
    case DWG_TYPE_POLYLINE_2D:
      {
        int error;
        BITCODE_BL j, numpts;
        // bool is_polygon = false;
        int changed = 0;
        dwg_point_2d *pts, *orig;
        Dwg_Entity_POLYLINE_2D *_obj = obj->tio.entity->tio.POLYLINE_2D;
        numpts = dwg_object_polyline_2d_get_numpoints (obj, &error);
        if (error || !numpts)
          return 0;
        pts = dwg_object_polyline_2d_get_points (obj, &error);
        if (error || !pts)
          return 0;
        // TODO bulges needs explosion into lines. divided by polyline curve
        // smoothness (default 8)

        // if closed and num_points > 3 use a Polygon
        FEATURE (AcDbEntity : AcDbPolyline, obj);
        if (_obj->flag & 512 && numpts > 3)
          {
            orig = pts; // pts is already a new copy
            changed = normalize_polygon_orient (numpts, &pts); // RFC7946
            if (changed)
              free (orig);
            GEOMETRY (Polygon)
            KEY (coordinates);
            ARRAY;
            ARRAY;
            for (j = 0; j < numpts; j++)
              VALUE_2DPOINT (pts[j].x, pts[j].y)
            LASTVALUE_2DPOINT (pts[0].x, pts[0].y);
            LASTENDARRAY;
            LASTENDARRAY;
            if (changed)
              free (pts);
          }
        else
          {
            GEOMETRY (LineString)
            KEY (coordinates);
            ARRAY;
            for (j = 0; j < numpts; j++)
              {
                if (j == numpts - 1)
                  LASTVALUE_2DPOINT (pts[j].x, pts[j].y)
                else
                  VALUE_2DPOINT (pts[j].x, pts[j].y);
              }
            free (pts);
            LASTENDARRAY;
          }
        ENDGEOMETRY;
        ENDFEATURE;
        return 1;
      }
    case DWG_TYPE_POLYLINE_3D:
      {
        int error;
        BITCODE_BL j, numpts;
        dwg_point_3d *pts;
        numpts = dwg_object_polyline_3d_get_numpoints (obj, &error);
        if (error || !numpts)
          return 0;
        pts = dwg_object_polyline_3d_get_points (obj, &error);
        if (error || !pts)
          return 0;
        FEATURE (AcDbEntity : AcDbPolyline, obj);
        GEOMETRY (LineString);
        KEY (coordinates);
        ARRAY;
        for (j = 0; j < numpts; j++)
          {
            if (j == numpts - 1)
              {
                LASTVALUE_3DPOINT (pts[j].x, pts[j].y, pts[j].z);
              }
            else
              {
                VALUE_3DPOINT (pts[j].x, pts[j].y, pts[j].z);
              }
          }
        free (pts);
        LASTENDARRAY;
        ENDGEOMETRY;
        ENDFEATURE;
        return 1;
      }
    case DWG_TYPE_ARC:
      // dwg_geojson_ARC(dat, obj);
      if (1)
        {
          Dwg_Entity_ARC *_obj = obj->tio.entity->tio.ARC;
          const int viewres = 1000;
          BITCODE_2BD ctr = { _obj->center.x, _obj->center.y };
          BITCODE_2BD *pts;
          int num_pts;
          double end_angle = _obj->end_angle;
          // viewres is for 2PI. we need anglediff(deg)/2PI
          while (end_angle - _obj->start_angle < 1e-6)
            end_angle += M_PI;
          num_pts
              = (int)trunc (viewres / rad2deg (end_angle - _obj->start_angle));
          if (num_pts > 10000 || num_pts < 0)
            {
              LOG_ERROR ("Invalid angles");
              return DWG_ERR_VALUEOUTOFBOUNDS;
            }
          num_pts = MIN (num_pts, 120);
          pts = (BITCODE_2BD *)malloc (num_pts * sizeof (BITCODE_2BD));
          if (!pts)
            {
              LOG_ERROR ("Out of memory");
              return DWG_ERR_OUTOFMEM;
            }
          // explode into line segments. divided by VIEWRES (default 1000)
          arc_split (pts, num_pts, ctr, _obj->start_angle, _obj->end_angle,
                     _obj->radius);
          FEATURE (AcDbEntity : AcDbArc, obj);
          GEOMETRY (Polygon)
          KEY (coordinates);
          ARRAY;
          ARRAY;
          for (int j = 0; j < num_pts; j++)
            {
              VALUE_2DPOINT (pts[j].x, pts[j].y)
            }
          LASTVALUE_2DPOINT (pts[0].x, pts[0].y);
          LASTENDARRAY;
          LASTENDARRAY;
          ENDGEOMETRY;
          ENDFEATURE;
          free (pts);
        }
      else
        LOG_TRACE ("ARC not yet supported")
      break;
    case DWG_TYPE_CIRCLE:
      // dwg_geojson_CIRCLE(dat, obj);
      if (1)
        {
          Dwg_Entity_CIRCLE *_obj = obj->tio.entity->tio.CIRCLE;
          // const int viewres = 1000; //dwg->header_vars.VIEWRES;
          BITCODE_2BD ctr = { _obj->center.x, _obj->center.y };
          // double res = viewres / 360.0;
          int num_pts = 120;
          BITCODE_2BD *pts
              = (BITCODE_2BD *)malloc (num_pts * sizeof (BITCODE_2BD));
          arc_split (pts, num_pts, ctr, 0, M_PI * 2.0, _obj->radius);
          FEATURE (AcDbEntity : AcDbCircle, obj);
          GEOMETRY (Polygon)
          KEY (coordinates);
          ARRAY;
          ARRAY;
          for (int j = 0; j < num_pts; j++)
            {
              VALUE_2DPOINT (pts[j].x, pts[j].y)
            }
          LASTVALUE_2DPOINT (pts[0].x, pts[0].y);
          LASTENDARRAY;
          LASTENDARRAY;
          ENDGEOMETRY;
          ENDFEATURE;
          free (pts);
        }
      else
        LOG_TRACE ("CIRCLE not yet supported")
      break;
    case DWG_TYPE_LINE:
      {
        Dwg_Entity_LINE *_obj = obj->tio.entity->tio.LINE;
        FEATURE (AcDbEntity : AcDbLine, obj);
        GEOMETRY (LineString);
        KEY (coordinates);
        ARRAY;
        FIELD_3DPOINT (start);
        LASTFIELD_3DPOINT (end);
        LASTENDARRAY;
        ENDGEOMETRY;
        ENDFEATURE;
        return 1;
      }
    case DWG_TYPE_POINT:
      {
        Dwg_Entity_POINT *_obj = obj->tio.entity->tio.POINT;
        FEATURE (AcDbEntity : AcDbPoint, obj);
        GEOMETRY (Point);
        KEY (coordinates);
        if (fabs (_obj->z) > 0.000001)
          {
            LASTVALUE_3DPOINT (_obj->x, _obj->y, _obj->z);
          }
        else
          {
            LASTVALUE_2DPOINT (_obj->x, _obj->y);
          }
        ENDGEOMETRY;
        ENDFEATURE;
        return 1;
      }
    case DWG_TYPE__3DFACE:
      // really a Polygon
      // dwg_geojson__3DFACE(dat, obj);
      LOG_TRACE ("3DFACE not yet supported")
      break;
    case DWG_TYPE_POLYLINE_PFACE:
      // dwg_geojson_POLYLINE_PFACE(dat, obj);
      LOG_TRACE ("POLYLINE_PFACE not yet supported")
      break;
    case DWG_TYPE_POLYLINE_MESH:
      // dwg_geojson_POLYLINE_MESH(dat, obj);
      LOG_TRACE ("POLYLINE_MESH not yet supported")
      break;
    case DWG_TYPE_SOLID:
      // dwg_geojson_SOLID(dat, obj);
      LOG_TRACE ("SOLID not yet supported")
      break;
    case DWG_TYPE_TRACE:
      // dwg_geojson_TRACE(dat, obj);
      LOG_TRACE ("TRACE not yet supported")
      break;
    case DWG_TYPE_ELLIPSE:
      // dwg_geojson_ELLIPSE(dat, obj);
      LOG_TRACE ("ELLIPSE not yet supported")
      break;
    case DWG_TYPE_SPLINE:
      // dwg_geojson_SPLINE(dat, obj);
      LOG_TRACE ("SPLINE not yet supported")
      break;
    case DWG_TYPE_HATCH:
      // dwg_geojson_HATCH(dat, obj);
      break;
    case DWG_TYPE__3DSOLID:
      // dwg_geojson__3DSOLID(dat, obj);
      break;
    case DWG_TYPE_REGION:
      // dwg_geojson_REGION(dat, obj);
      break;
    case DWG_TYPE_BODY:
      // dwg_geojson_BODY(dat, obj);
      break;
    case DWG_TYPE_RAY:
      // dwg_geojson_RAY(dat, obj);
      LOG_TRACE ("RAY not yet supported")
      break;
    case DWG_TYPE_XLINE:
      // dwg_geojson_XLINE(dat, obj);
      LOG_TRACE ("XLINE not yet supported")
      break;
    case DWG_TYPE_TEXT:
      {
        // add Text property to a point
        Dwg_Entity_TEXT *_obj = obj->tio.entity->tio.TEXT;
        FEATURE (AcDbEntity : AcDbText, obj);
        GEOMETRY (Point);
        KEY (coordinates);
        LASTFIELD_2DPOINT (ins_pt);
        ENDGEOMETRY;
        ENDFEATURE;
        return 1;
      }
    case DWG_TYPE_MTEXT:
      {
        // add Text property to a point
        Dwg_Entity_MTEXT *_obj = obj->tio.entity->tio.MTEXT;
        FEATURE (AcDbEntity : AcDbMText, obj);
        GEOMETRY (Point);
        KEY (coordinates);
        LASTFIELD_3DPOINT (ins_pt);
        ENDGEOMETRY;
        ENDFEATURE;
        return 1;
      }
    case DWG_TYPE_MLINE:
      // dwg_geojson_MLINE(dat, obj);
      LOG_TRACE ("MLINE not yet supported")
      break;
    case DWG_TYPE_LWPOLYLINE:
      return dwg_geojson_LWPOLYLINE (dat, obj, is_last);
    default:
      if (obj->parent && dat->version > R_12
          && obj->fixedtype != obj->parent->layout_type)
        return dwg_geojson_variable_type (obj->parent, dat, obj, is_last);
    }
  return 0;
}

static int
geojson_entities_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_BL i;
  int success;
  SECTION (features);
  for (i = 0; i < dwg->num_objects; i++)
    {
      int is_last = i == dwg->num_objects - 1;
      Dwg_Object *obj = &dwg->object[i];
      success = dwg_geojson_object (dat, obj, is_last);
      if (is_last && !success) // needed for the LASTFEATURE comma. end with an
                               // empty dummy
        {
          HASH PAIR_Sc (type, "Feature");
          PAIR_NULL (properties);
          LASTPAIR_NULL (geometry);
          LASTENDHASH;
        }
    }
  ENDSEC (); // because afterwards is always the final geocoding object
  return 0;
}

EXPORT int
dwg_write_geojson (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  // const int minimal = dwg->opts & DWG_OPTS_MINIMAL;
  char date[12] = "YYYY-MM-DD";
  time_t rawtime;

  if (!dwg->num_objects || !dat->fh)
    goto fail;

  HASH;
  PAIR_Sc (type, "FeatureCollection");

  // array of features
  if (geojson_entities_write (dat, dwg))
    goto fail;

  KEY (geocoding);
  HASH;
  time (&rawtime);
  strftime (date, 12, "%Y-%m-%d", localtime (&rawtime));
  PAIR_Sc (creation_date, date);
  KEY (generator);
  HASH;
  KEY (author);
  HASH;
  LASTPAIR_Sc (name, "dwgread");
  ENDHASH;
  PAIR_Sc (package, PACKAGE_NAME);
  LASTPAIR_Sc (version, PACKAGE_VERSION);
  LASTENDHASH;
  // PAIR_S(license, "?");
  LASTENDHASH;

  LASTENDHASH;
  return 0;
fail:
  return 1;
}

#undef IS_PRINT
