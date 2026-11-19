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
/* TODO: explode of inserts?
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

// The attributes of an INSERT or MINSERT, as in their structs
typedef struct _geojson_attribs
{
  BITCODE_B has_attribs;
  BITCODE_BL num_owned;
  BITCODE_H first_attrib;
  BITCODE_H last_attrib;
  BITCODE_H *attribs;
} geojson_attribs;

static void
geojson_attribs_of (const Dwg_Object *restrict obj,
                    geojson_attribs *restrict a)
{
  memset (a, 0, sizeof (*a));
  if (obj->fixedtype == DWG_TYPE_INSERT
      || obj->fixedtype == DWG_TYPE_ACIDBLOCKREFERENCE)
    {
      const Dwg_Entity_INSERT *_obj = obj->tio.entity->tio.INSERT;
      a->has_attribs = _obj->has_attribs;
      a->num_owned = _obj->num_owned;
      a->first_attrib = _obj->first_attrib;
      a->last_attrib = _obj->last_attrib;
      a->attribs = _obj->attribs;
    }
  else if (obj->fixedtype == DWG_TYPE_MINSERT)
    {
      const Dwg_Entity_MINSERT *_obj = obj->tio.entity->tio.MINSERT;
      a->has_attribs = _obj->has_attribs;
      a->num_owned = _obj->num_owned;
      a->first_attrib = _obj->first_attrib;
      a->last_attrib = _obj->last_attrib;
      a->attribs = _obj->attribs;
    }
}

// until R2000 the ATTRIBs are linked from first_attrib to last_attrib
static Dwg_Object *
geojson_linked_attrib (const geojson_attribs *restrict a,
                       const Dwg_Object *restrict prev)
{
  if (!prev)
    return a->first_attrib ? a->first_attrib->obj : NULL;
  if (a->last_attrib && prev != a->last_attrib->obj)
    return dwg_next_object (prev);
  return NULL;
}

// since R2004 the ATTRIBs are in a vector
static Dwg_Object *
geojson_vector_attrib (const geojson_attribs *restrict a, const BITCODE_BL i)
{
  return i < a->num_owned && a->attribs && a->attribs[i] ? a->attribs[i]->obj
                                                         : NULL;
}

/* The attribute number i after prev, NULL after the last one, as in
   out_dxf.c. Before R13 the ATTRIBs follow the INSERT up to its SEQEND. */
static Dwg_Object *
geojson_next_attrib (const Dwg_Object *restrict obj,
                     const geojson_attribs *restrict a,
                     const Dwg_Object *restrict prev, const BITCODE_BL i)
{
  const Dwg_Version_Type version = obj->parent->header.version;
  Dwg_Object *o;
  if (!a->has_attribs)
    return NULL;
  if (version >= R_2004a)
    o = geojson_vector_attrib (a, i);
  else if (version < R_13b1)
    o = dwg_next_object (prev ? prev : obj);
  else
    o = geojson_linked_attrib (a, prev);
  return o && o->fixedtype == DWG_TYPE_ATTRIB ? o : NULL;
}

// a copy of a text as UTF-8 for TU, else as is
static char *
geojson_text (Bit_Chain *restrict dat, const BITCODE_T text)
{
  char *s;
  size_t len;
  if (!text)
    return NULL;
  if (IS_FROM_TU (dat))
    return bit_convert_TU ((BITCODE_TU)text);
  len = strlen (text);
  s = (char *)malloc (len + 1);
  if (s)
    memcpy (s, text, len + 1);
  return s;
}

// a tag must not repeat a key of the feature or an earlier tag
static int
geojson_key_used (const char *restrict key, char *const *restrict tags,
                  const BITCODE_BL num_tags)
{
  static const char *const keys[]
      = { "SubClasses", "Layer", "Color",       "Linetype",
          "Text",       "name",  "EntityHandle" };
  BITCODE_BL i;
  for (i = 0; i < sizeof (keys) / sizeof (keys[0]); i++)
    if (strcmp (key, keys[i]) == 0)
      return 1;
  for (i = 0; i < num_tags; i++)
    if (strcmp (key, tags[i]) == 0)
      return 1;
  return 0;
}

// "key": "value", both quoted
static void
geojson_pair (Bit_Chain *restrict dat, const char *restrict key,
              const char *restrict value)
{
  const size_t klen = 6 * strlen (key) + 1;
  const size_t vlen = 6 * strlen (value) + 1;
  char *k = (char *)malloc (klen);
  char *v = (char *)malloc (vlen);
  if (k && v)
    {
      PREFIX fprintf (dat->fh, "\"%s\": \"%s\",\n",
                      json_cquote (k, key, klen, dat->codepage),
                      json_cquote (v, value, vlen, dat->codepage));
    }
  free (k);
  free (v);
}

/* The attributes of an INSERT or MINSERT as "TAG": "value" properties, as
   asked in GH #849. An empty value is written as "", a repeated tag is
   skipped. */
static void
geojson_insert_attribs (Bit_Chain *restrict dat,
                        const Dwg_Object *restrict obj)
{
  geojson_attribs a;
  const Dwg_Object *o = NULL;
  char **tags = NULL;
  BITCODE_BL i, num_tags = 0;

  geojson_attribs_of (obj, &a);
  for (i = 0; (o = geojson_next_attrib (obj, &a, o, i)) != NULL; i++)
    {
      const Dwg_Entity_ATTRIB *_obj = o->tio.entity->tio.ATTRIB;
      char *tag = geojson_text (dat, _obj->tag);
      char *value;
      char **more;
      if (!tag || !*tag || geojson_key_used (tag, tags, num_tags)
          || !(more
               = (char **)realloc (tags, (num_tags + 1) * sizeof (char *))))
        {
          free (tag);
          continue;
        }
      tags = more;
      tags[num_tags++] = tag;
      value = geojson_text (dat, _obj->text_value);
      geojson_pair (dat, tag, value ? value : "");
      free (value);
    }
  for (i = 0; i < num_tags; i++)
    free (tags[i]);
  free (tags);
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
  else if (obj->fixedtype == DWG_TYPE_INSERT
           || obj->fixedtype == DWG_TYPE_ACIDBLOCKREFERENCE)
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
      geojson_insert_attribs (dat, obj);
    }
  else if (obj->fixedtype == DWG_TYPE_MINSERT)
    {
      Dwg_Entity_MINSERT *_obj = obj->tio.entity->tio.MINSERT;
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
      geojson_insert_attribs (dat, obj);
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

static int calc_curve_segments (Dwg_Data *restrict dwg, BITCODE_BD radius,
                                BITCODE_BD angle_span);

/* The n vertices of a polyline with its bulged segments split into arc
   points.  For a closed polyline the closing segment is included, but not
   its end, the first vertex.  Returns a malloc'ed array of *num_p points,
   or NULL if out of memory. */
static dwg_point_2d *
bulge_points (Dwg_Data *restrict dwg, const dwg_point_2d *restrict points,
              const double *restrict bulges, const BITCODE_BL n,
              const int closed, BITCODE_BL *restrict num_p)
{
  const BITCODE_BL num_segs = closed ? n : n - 1;
  size_t total = closed ? 0 : 1; // an open one ends with its last vertex
  size_t k = 0;
  BITCODE_BL j;
  BITCODE_2BD *pts;

  // pass 1: count the points of each segment
  for (j = 0; j < num_segs; j++)
    {
      BITCODE_2BD p1 = { points[j].x, points[j].y };
      BITCODE_2BD p2 = { points[(j + 1) % n].x, points[(j + 1) % n].y };
      BITCODE_2BD ctr;
      double radius, start_angle, sweep;
      if (bulge_to_arc (p1, p2, bulges[j], &ctr, &radius, &start_angle,
                        &sweep))
        total += calc_curve_segments (dwg, radius, fabs (sweep));
      else
        total++;
    }
  pts = (BITCODE_2BD *)malloc (total * sizeof (BITCODE_2BD));
  if (!pts)
    return NULL;
  // pass 2: same segment counts as above
  for (j = 0; j < num_segs; j++)
    {
      BITCODE_2BD p1 = { points[j].x, points[j].y };
      BITCODE_2BD p2 = { points[(j + 1) % n].x, points[(j + 1) % n].y };
      BITCODE_2BD ctr;
      double radius, start_angle, sweep;
      int num = 1;
      if (bulge_to_arc (p1, p2, bulges[j], &ctr, &radius, &start_angle,
                        &sweep))
        num = calc_curve_segments (dwg, radius, fabs (sweep));
      k += bulge_split (&pts[k], num, p1, p2, bulges[j]);
    }
  if (!closed)
    {
      pts[k].x = points[n - 1].x;
      pts[k].y = points[n - 1].y;
      k++;
    }
  *num_p = (BITCODE_BL)k;
  return (dwg_point_2d *)pts;
}

// True if the OCS of ext is the WCS: (0,0,1), or (0,0,0) before R13
static int
geojson_is_wcs (const BITCODE_BE ext)
{
  return ext.x == 0.0 && ext.y == 0.0 && (ext.z == 1.0 || ext.z == 0.0);
}

// The n OCS points of an entity at elevation, in place to WCS
static void
geojson_ocs_points (dwg_point_2d *pts, const BITCODE_BL n,
                    const BITCODE_BE ext, const double elevation)
{
  BITCODE_BL j;
  if (geojson_is_wcs (ext))
    return;
  for (j = 0; j < n; j++)
    {
      BITCODE_3DPOINT p, w;
      p.x = pts[j].x;
      p.y = pts[j].y;
      p.z = elevation;
      transform_OCS (&w, p, ext);
      pts[j].x = w.x;
      pts[j].y = w.y;
    }
}

// The n points in WCS: in place in *own if set, else in a new copy *own,
// not in the points of the drawing.  pts if out of memory.
static dwg_point_2d *
geojson_wcs_points (dwg_point_2d *pts, dwg_point_2d **own, const BITCODE_BL n,
                    const BITCODE_BE ext, const double elevation)
{
  if (geojson_is_wcs (ext))
    return pts;
  if (!*own && (*own = (dwg_point_2d *)malloc (n * sizeof (*pts))))
    memcpy (*own, pts, n * sizeof (*pts));
  if (!*own)
    return pts;
  geojson_ocs_points (*own, n, ext, elevation);
  return *own;
}

// The n points of a CIRCLE in WCS, counterclockwise as RFC 7946 asks: a
// mirrored OCS turns them clockwise, so reverse them
static void
geojson_circle_wcs (BITCODE_2BD *pts, const int n,
                    const Dwg_Entity_CIRCLE *_obj)
{
  int j;
  geojson_ocs_points ((dwg_point_2d *)pts, (BITCODE_BL)n, _obj->extrusion,
                      _obj->center.z);
  if (!transform_OCS_2d_is_mirrored (_obj->extrusion))
    return;
  for (j = 0; j < n / 2; j++)
    {
      const BITCODE_2BD t = pts[j];
      pts[j] = pts[n - 1 - j];
      pts[n - 1 - j] = t;
    }
}

static int
dwg_geojson_LWPOLYLINE (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
                        int is_last)
{
  BITCODE_BL j, last_j;
  Dwg_Entity_LWPOLYLINE *_obj = obj->tio.entity->tio.LWPOLYLINE;
  dwg_point_2d *pts = (dwg_point_2d *)_obj->points;
  dwg_point_2d *bulge_pts = NULL;
  BITCODE_BL num_pts = _obj->num_points;
  if (!_obj->points || !num_pts)
    return 1;
  if (_obj->bulges && _obj->num_bulges == num_pts)
    {
      bulge_pts = bulge_points (obj->parent, pts, _obj->bulges, num_pts,
                                _obj->flag & 512 ? 1 : 0, &num_pts);
      if (bulge_pts)
        pts = bulge_pts;
      else
        num_pts = _obj->num_points;
    }
  pts = geojson_wcs_points (pts, &bulge_pts, num_pts, _obj->extrusion,
                            _obj->elevation);

  FEATURE (AcDbEntity : AcDbLwPolyline, obj);
  // TODO splines, ...

  // if closed and num_points > 3 use a Polygon
  if (_obj->flag & 512 && num_pts > 3)
    {
      int changed = normalize_polygon_orient (num_pts, &pts); // RFC7946
      GEOMETRY (Polygon)
      KEY (coordinates);
      ARRAY;
      ARRAY;
      for (j = 0; j < num_pts; j++)
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
      last_j = num_pts - 1;
      for (j = 0; j < last_j; j++)
        VALUE_2DPOINT (pts[j].x, pts[j].y);
      LASTVALUE_2DPOINT (pts[last_j].x, pts[last_j].y);
      LASTENDARRAY;
    }
  ENDGEOMETRY;
  ENDFEATURE;
  free (bulge_pts);
  return 1;
}

// The tolerance of the points on a SPLINE curve, relative to its size.
#define SPLINE_GEOJSON_TOL 1e-3

/* A SPLINE as a LineString of points on its curve, within SPLINE_GEOJSON_TOL
   of it.  If the SPLINE cannot be evaluated, its fit points or else its
   control points.  Returns 0 if there is nothing to write. */
static int
dwg_geojson_SPLINE (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
                    int is_last)
{
  Dwg_Entity_SPLINE *_obj = obj->tio.entity->tio.SPLINE;
  BITCODE_3DPOINT *curve = NULL;
  const BITCODE_3DPOINT *pts;
  int j, num_pts = spline_flatten (_obj, SPLINE_GEOJSON_TOL, &curve);

  if (num_pts >= 2)
    pts = curve;
  else if (_obj->num_fit_pts > 0 && _obj->fit_pts)
    {
      num_pts = _obj->num_fit_pts;
      pts = _obj->fit_pts;
    }
  else if (_obj->num_ctrl_pts > 0 && _obj->ctrl_pts
           && (curve = (BITCODE_3DPOINT *)malloc (_obj->num_ctrl_pts
                                                  * sizeof (*curve))))
    {
      // the control points have a parent pointer: copy x, y, z
      for (j = 0; j < (int)_obj->num_ctrl_pts; j++)
        {
          curve[j].x = _obj->ctrl_pts[j].x;
          curve[j].y = _obj->ctrl_pts[j].y;
          curve[j].z = _obj->ctrl_pts[j].z;
        }
      num_pts = (int)_obj->num_ctrl_pts;
      pts = curve;
    }
  else
    return 0;
  FEATURE (AcDbEntity : AcDbSpline, obj);
  GEOMETRY (LineString);
  KEY (coordinates);
  ARRAY;
  for (j = 0; j < num_pts - 1; j++)
    VALUE_2DPOINT (pts[j].x, pts[j].y)
  LASTVALUE_2DPOINT (pts[num_pts - 1].x, pts[num_pts - 1].y)
  LASTENDARRAY;
  ENDGEOMETRY;
  ENDFEATURE;
  free (curve);
  return 1;
}

/* A spline-fit POLYLINE_2D: points on its curve in its OCS, within
   SPLINE_GEOJSON_TOL, as a malloc'ed array of *num_p points; a closed one
   without its last point, the first.  NULL if it has no curve. */
static dwg_point_2d *
polyline_2d_spline_points (const Dwg_Object *obj, BITCODE_BL *num_p)
{
  const int closed = obj->tio.entity->tio.POLYLINE_2D->flag & 1;
  BITCODE_3DPOINT *bez, *curve;
  dwg_point_2d *pts;
  int error, degree, j, num;
  const int k = dwg_object_polyline_2d_get_spline (obj, &bez, &degree, &error);

  if (!k)
    return NULL;
  num = bezier_chain_flatten (bez, k, degree, SPLINE_GEOJSON_TOL, &curve);
  free (bez);
  if (closed)
    num--;
  pts = num >= 2 ? (dwg_point_2d *)malloc (num * sizeof (dwg_point_2d)) : NULL;
  for (j = 0; pts && j < num; j++)
    {
      pts[j].x = curve[j].x;
      pts[j].y = curve[j].y;
    }
  free (curve);
  if (pts)
    *num_p = (BITCODE_BL)num;
  return pts;
}

// The tolerance of the curves of a HATCH boundary, relative to its size.
#define HATCH_GEOJSON_TOL 1e-3

// A HATCH boundary path as a closed ring of points.
typedef struct _hatch_ring
{
  BITCODE_2BD *pts;
  int num;
  int depth; // the number of rings it lies in
  int role;  // 1: an area, 0: a hole, -1: not filled
  int outer; // for a hole: the index of its area
} hatch_ring;

// True if ring i lies in ring j, also where it touches it.
static int
ring_in_ring (const hatch_ring *r, const int i, const int j)
{
  return j != i && polygon_in_polygon (r[i].pts, r[i].num, r[j].pts, r[j].num);
}

/* The role of a ring by its nesting depth and the HATCH style: Normal (0)
   fills by parity, an island in a hole is an area again; Outer (1) fills
   the outermost area only, with its holes; Ignore (2) fills the outermost
   area, ignoring all islands. */
static int
hatch_ring_role (const int depth, const int style)
{
  if (style == 2)
    return depth == 0 ? 1 : -1;
  if (style == 1)
    return depth == 0 ? 1 : (depth == 1 ? 0 : -1);
  return depth % 2 == 0;
}

// The depth and role of each ring, and the area of each hole: the area
// one level up which contains it.
static void
hatch_ring_roles (hatch_ring *r, const int n, const int style)
{
  int i, j;
  for (i = 0; i < n; i++)
    {
      for (j = 0, r[i].depth = 0; j < n; j++)
        r[i].depth += ring_in_ring (r, i, j);
      r[i].role = hatch_ring_role (r[i].depth, style);
    }
  for (i = 0; i < n; i++)
    for (j = 0, r[i].outer = -1; r[i].role == 0 && j < n; j++)
      if (r[j].role == 1 && r[j].depth == r[i].depth - 1
          && ring_in_ring (r, i, j))
        r[i].outer = j;
}

// Writes a ring, counter-clockwise if ccw, else clockwise (RFC 7946).
static void
geojson_ring (Bit_Chain *restrict dat, const hatch_ring *r, const int ccw,
              const int is_last)
{
  const int reverse = (polygon_area (r->pts, r->num) > 0.0) != ccw;
  int i;
  ARRAY;
  for (i = 0; i < r->num; i++)
    {
      const BITCODE_2BD *p = &r->pts[reverse ? r->num - 1 - i : i];
      if (i < r->num - 1)
        VALUE_2DPOINT (p->x, p->y)
      else
        LASTVALUE_2DPOINT (p->x, p->y)
    }
  if (is_last)
    LASTENDARRAY
  else
    ENDARRAY
}

// Writes the polygon of the area ring a with its holes.
static void
geojson_polygon (Bit_Chain *restrict dat, const hatch_ring *r, const int n,
                 const int a, const int is_last)
{
  int i, last = a;
  for (i = 0; i < n; i++)
    if (r[i].role == 0 && r[i].outer == a)
      last = i;
  ARRAY;
  geojson_ring (dat, &r[a], 1, last == a);
  for (i = 0; i < n; i++)
    if (r[i].role == 0 && r[i].outer == a)
      geojson_ring (dat, &r[i], 0, i == last);
  if (is_last)
    LASTENDARRAY
  else
    ENDARRAY
}

// The rings of the boundary paths of a HATCH in WCS, skipping the
// boundaries around texts (flag 8), which are no geometry.  Returns their
// number.
static int
hatch_rings (const Dwg_Entity_HATCH *_obj, hatch_ring *r)
{
  BITCODE_BL j;
  int n = 0;
  for (j = 0; j < _obj->num_paths; j++)
    if (!(_obj->paths[j].flag & 8)
        && (r[n].num = hatch_path_points (&_obj->paths[j], HATCH_GEOJSON_TOL,
                                          &r[n].pts)))
      {
        geojson_ocs_points ((dwg_point_2d *)r[n].pts, (BITCODE_BL)r[n].num,
                            _obj->extrusion, _obj->elevation);
        n++;
      }
  return n;
}

// Writes the coordinates: a Polygon for one area, else a MultiPolygon.
static void
geojson_hatch_coordinates (Bit_Chain *restrict dat, const hatch_ring *r,
                           const int n, const int num_areas)
{
  int i, k = 0;
  if (num_areas == 1)
    {
      for (i = 0; r[i].role != 1; i++)
        ;
      geojson_polygon (dat, r, n, i, 1);
      return;
    }
  ARRAY;
  for (i = 0; i < n; i++)
    if (r[i].role == 1)
      geojson_polygon (dat, r, n, i, ++k == num_areas);
  LASTENDARRAY;
}

/* A HATCH as a Polygon, or a MultiPolygon for several areas, with holes
   by its style.  Returns 0 if there is nothing to write. */
static int
dwg_geojson_HATCH (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
                   int is_last)
{
  Dwg_Entity_HATCH *_obj = obj->tio.entity->tio.HATCH;
  hatch_ring *r;
  int n, num_areas = 0, i;

  if (!_obj->paths || !_obj->num_paths
      || !(r = (hatch_ring *)calloc (_obj->num_paths, sizeof (hatch_ring))))
    return 0;
  n = hatch_rings (_obj, r);
  hatch_ring_roles (r, n, _obj->style);
  for (i = 0; i < n; i++)
    num_areas += r[i].role == 1;
  if (num_areas)
    {
      FEATURE (AcDbEntity : AcDbHatch, obj);
      if (num_areas == 1)
        GEOMETRY (Polygon)
      else
        GEOMETRY (MultiPolygon)
      KEY (coordinates);
      geojson_hatch_coordinates (dat, r, n, num_areas);
      ENDGEOMETRY;
      ENDFEATURE;
    }
  for (i = 0; i < n; i++)
    free (r[i].pts);
  free (r);
  return num_areas ? 1 : 0;
}

/* An ELLIPSE as a Polygon if it is closed, else as a LineString of its arc.
   The center and the major axis are in WCS, the minor axis is
   axis_ratio * (extrusion x major axis).  The angles are the parameter u
   of center + major cos u + minor sin u, not polar angles.  */
static int
dwg_geojson_ELLIPSE (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
                     int is_last)
{
  Dwg_Entity_ELLIPSE *_obj = obj->tio.entity->tio.ELLIPSE;
  const BITCODE_3BD a = _obj->sm_axis;
  const BITCODE_3BD n = _obj->extrusion;
  const double bx = _obj->axis_ratio * (n.y * a.z - n.z * a.y);
  const double by = _obj->axis_ratio * (n.z * a.x - n.x * a.z);
  double span = _obj->end_angle - _obj->start_angle;
  BITCODE_2BD *pts;
  int j, num_pts, closed;

  if (!isfinite (span))
    {
      LOG_ERROR ("Invalid angles");
      return DWG_ERR_VALUEOUTOFBOUNDS;
    }
  // the span is in (0, 2 pi]: 0 to 0 and 0 to 2 pi are both closed
  span = fmod (span, 2 * M_PI);
  if (span <= 0.0)
    span += 2 * M_PI;
  // also closed if the angles are a few ulp off a full turn, e.g. from
  // pi to 3 pi + 4e-15, which leaves a span of 4e-15
  closed = span < 1e-9 || span > 2 * M_PI - 1e-9;
  if (closed)
    span = 2 * M_PI;
  num_pts = calc_curve_segments (obj->parent, hypot (a.x, a.y), span);
  pts = (BITCODE_2BD *)malloc ((num_pts + 1) * sizeof (BITCODE_2BD));
  if (!pts)
    {
      LOG_ERROR ("Out of memory");
      return DWG_ERR_OUTOFMEM;
    }
  // each u from the start, so that the last point is the end point
  for (j = 0; j <= num_pts; j++)
    {
      const double u = _obj->start_angle + span * j / num_pts;
      pts[j].x = _obj->center.x + a.x * cos (u) + bx * sin (u);
      pts[j].y = _obj->center.y + a.y * cos (u) + by * sin (u);
    }
  if (closed)
    pts[num_pts] = pts[0];
  FEATURE (AcDbEntity : AcDbEllipse, obj);
  if (closed)
    {
      GEOMETRY (Polygon);
      KEY (coordinates);
      ARRAY;
    }
  else
    {
      GEOMETRY (LineString);
      KEY (coordinates);
    }
  ARRAY;
  for (j = 0; j < num_pts; j++)
    VALUE_2DPOINT (pts[j].x, pts[j].y)
  LASTVALUE_2DPOINT (pts[num_pts].x, pts[num_pts].y)
  LASTENDARRAY;
  if (closed)
    LASTENDARRAY;
  ENDGEOMETRY;
  ENDFEATURE;
  free (pts);
  return 1;
}

#define SET_PT(dst, src)                                                      \
  do                                                                          \
    {                                                                         \
      (dst).x = (src).x;                                                      \
      (dst).y = (src).y;                                                      \
      (dst).z = (src).z;                                                      \
    }                                                                         \
  while (0)

enum
{
  GJ_LINESTRING,
  GJ_POLYGON,
  GJ_MULTIPOINT
};

static void
geojson_point (Bit_Chain *restrict dat, const dwg_point_3d *restrict p,
               const int has_z, const int last)
{
  if (has_z)
    {
      if (last)
        LASTVALUE_3DPOINT (p->x, p->y, p->z)
      else
        VALUE_3DPOINT (p->x, p->y, p->z)
    }
  else
    {
      if (last)
        LASTVALUE_2DPOINT (p->x, p->y)
      else
        VALUE_2DPOINT (p->x, p->y)
    }
}

static int
points_have_z (const dwg_point_3d *restrict pts, const BITCODE_BL n)
{
  for (BITCODE_BL j = 0; j < n; j++)
    if (fabs (pts[j].z) > 0.000001)
      return 1;
  return 0;
}

/* A feature with a LineString, a Polygon (a closed ring) or a MultiPoint of
   n points. */
static int
geojson_points (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
                const char *restrict subclass, const int is_last,
                const int type, const dwg_point_3d *restrict pts,
                const BITCODE_BL n)
{
  const int has_z = points_have_z (pts, n);
  BITCODE_BL j;
  HASH;
  dwg_geojson_feature (dat, obj, subclass);
  if (type == GJ_POLYGON)
    GEOMETRY (Polygon)
  else if (type == GJ_MULTIPOINT)
    GEOMETRY (MultiPoint)
  else
    GEOMETRY (LineString)
  KEY (coordinates);
  ARRAY;
  if (type == GJ_POLYGON)
    ARRAY;
  for (j = 0; j < n; j++)
    geojson_point (dat, &pts[j], has_z, j == n - 1);
  LASTENDARRAY;
  if (type == GJ_POLYGON)
    LASTENDARRAY;
  ENDGEOMETRY;
  ENDFEATURE;
  return 1;
}

/* A feature with a MultiLineString or a MultiPolygon of nrings rings. */
static int
geojson_rings (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
               const char *restrict subclass, const int is_last,
               const int polygons, const dwg_point_3d *const *restrict rings,
               const BITCODE_BL *restrict counts, const BITCODE_BL nrings)
{
  BITCODE_BL i, j;
  int has_z = 0;
  for (i = 0; i < nrings; i++)
    has_z |= points_have_z (rings[i], counts[i]);
  HASH;
  dwg_geojson_feature (dat, obj, subclass);
  if (polygons)
    GEOMETRY (MultiPolygon)
  else
    GEOMETRY (MultiLineString)
  KEY (coordinates);
  ARRAY;
  for (i = 0; i < nrings; i++)
    {
      if (polygons)
        ARRAY;
      ARRAY;
      for (j = 0; j < counts[i]; j++)
        geojson_point (dat, &rings[i][j], has_z, j == counts[i] - 1);
      if (i == nrings - 1)
        LASTENDARRAY
      else
        ENDARRAY
      if (polygons)
        {
          if (i == nrings - 1)
            LASTENDARRAY
          else
            ENDARRAY
        }
    }
  LASTENDARRAY;
  ENDGEOMETRY;
  ENDFEATURE;
  return 1;
}

/* SOLID and TRACE: the corners are 1, 2, 4, 3 around, in the OCS */
static int
geojson_solid (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
               const char *restrict subclass, const int is_last,
               const BITCODE_2RD c1, const BITCODE_2RD c2,
               const BITCODE_2RD c3, const BITCODE_2RD c4,
               const BITCODE_BD elevation, const BITCODE_3BD extrusion)
{
  const BITCODE_2RD c[4] = { c1, c2, c4, c3 };
  dwg_point_3d pts[5];
  int j;
  for (j = 0; j < 4; j++)
    {
      BITCODE_3DPOINT p = { c[j].x, c[j].y, elevation }, q;
      transform_OCS (&q, p, extrusion);
      pts[j].x = q.x;
      pts[j].y = q.y;
      pts[j].z = q.z;
    }
  pts[4] = pts[0];
  return geojson_points (dat, obj, subclass, is_last, GJ_POLYGON, pts, 5);
}

/* The faces of a polyface mesh: a MultiPolygon. The vertices are the
   VERTEX_PFACE, the faces the VERTEX_PFACE_FACE with 1-based vertex
   indices, negative if invisible. */
static int
geojson_polyline_pface (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                        Dwg_Object *restrict obj, const int is_last)
{
  Dwg_Entity_POLYLINE_PFACE *_obj = obj->tio.entity->tio.POLYLINE_PFACE;
  dwg_point_3d *verts = NULL, *ring_pts = NULL;
  const dwg_point_3d **rings = NULL;
  BITCODE_BL *counts = NULL;
  BITCODE_BL i, nv = 0, nf = 0;
  int ret = 0;
  if (!_obj->vertex || !_obj->num_owned)
    return 0;
  verts = (dwg_point_3d *)calloc (_obj->num_owned, sizeof (dwg_point_3d));
  ring_pts = (dwg_point_3d *)calloc (5 * (size_t)_obj->num_owned,
                                     sizeof (dwg_point_3d));
  rings = (const dwg_point_3d **)calloc (_obj->num_owned, sizeof (void *));
  counts = (BITCODE_BL *)calloc (_obj->num_owned, sizeof (BITCODE_BL));
  if (!verts || !ring_pts || !rings || !counts)
    goto done;
  for (i = 0; i < _obj->num_owned; i++)
    {
      Dwg_Object *vobj = dwg_ref_object (dwg, _obj->vertex[i]);
      if (!vobj)
        continue;
      if (vobj->fixedtype == DWG_TYPE_VERTEX_PFACE)
        {
          Dwg_Entity_VERTEX_PFACE *v = vobj->tio.entity->tio.VERTEX_PFACE;
          SET_PT (verts[nv], v->point);
          nv++;
        }
      else if (vobj->fixedtype == DWG_TYPE_VERTEX_PFACE_FACE)
        {
          Dwg_Entity_VERTEX_PFACE_FACE *f
              = vobj->tio.entity->tio.VERTEX_PFACE_FACE;
          dwg_point_3d *r = &ring_pts[5 * (size_t)nf];
          BITCODE_BL n = 0;
          for (int k = 0; k < 4; k++)
            {
              const int idx = abs (f->vertind[k]);
              if (idx >= 1 && (BITCODE_BL)idx <= nv)
                r[n++] = verts[idx - 1];
            }
          if (n >= 3)
            {
              r[n++] = r[0];
              rings[nf] = r;
              counts[nf++] = n;
            }
        }
    }
  if (nf)
    ret = geojson_rings (dat, obj, "AcDbEntity : AcDbPolyFaceMesh", is_last, 1,
                         rings, counts, nf);
done:
  free (verts);
  free (ring_pts);
  free (rings);
  free (counts);
  return ret;
}

/* A polygon mesh of m x n vertices: a MultiLineString of the m rows and
   n columns. */
static int
geojson_polyline_mesh (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                       Dwg_Object *restrict obj, const int is_last)
{
  Dwg_Entity_POLYLINE_MESH *_obj = obj->tio.entity->tio.POLYLINE_MESH;
  const BITCODE_BL m = _obj->num_m_verts, n = _obj->num_n_verts;
  dwg_point_3d *verts = NULL, *lines = NULL;
  const dwg_point_3d **rings = NULL;
  BITCODE_BL *counts = NULL;
  BITCODE_BL i, j, nv = 0;
  int ret = 0;
  if (!_obj->vertex || !m || !n || m * n > _obj->num_owned)
    return 0;
  verts = (dwg_point_3d *)calloc ((size_t)m * n, sizeof (dwg_point_3d));
  lines = (dwg_point_3d *)calloc (2 * (size_t)m * n, sizeof (dwg_point_3d));
  rings = (const dwg_point_3d **)calloc (m + n, sizeof (void *));
  counts = (BITCODE_BL *)calloc (m + n, sizeof (BITCODE_BL));
  if (!verts || !lines || !rings || !counts)
    goto done;
  for (i = 0; i < _obj->num_owned && nv < m * n; i++)
    {
      Dwg_Object *vobj = dwg_ref_object (dwg, _obj->vertex[i]);
      if (vobj && vobj->fixedtype == DWG_TYPE_VERTEX_MESH)
        {
          SET_PT (verts[nv], vobj->tio.entity->tio.VERTEX_MESH->point);
          nv++;
        }
    }
  if (nv < m * n)
    goto done;
  for (i = 0; i < m; i++) // the rows
    {
      dwg_point_3d *r = &lines[(size_t)i * n];
      for (j = 0; j < n; j++)
        r[j] = verts[i * n + j];
      rings[i] = r;
      counts[i] = n;
    }
  for (j = 0; j < n; j++) // the columns
    {
      dwg_point_3d *r = &lines[(size_t)m * n + (size_t)j * m];
      for (i = 0; i < m; i++)
        r[i] = verts[i * n + j];
      rings[m + j] = r;
      counts[m + j] = m;
    }
  ret = geojson_rings (dat, obj, "AcDbEntity : AcDbPolygonMesh", is_last, 0,
                       rings, counts, m + n);
done:
  free (verts);
  free (lines);
  free (rings);
  free (counts);
  return ret;
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

  i = obj->type - 500;
  if (i < 0 || i >= (int)dwg->num_classes)
    return 0;
  // returns whether printed, not an error
  if (obj->fixedtype == DWG_TYPE_UNKNOWN_ENT
      || obj->fixedtype == DWG_TYPE_UNKNOWN_OBJ)
    return 0;

  klass = &dwg->dwg_class[i];
  if (!klass || !klass->dxfname)
    return 0;
  dxfname = klass->dxfname;
  // almost always false
  is_entity = dwg_class_is_entity (klass);

  // the class must name the object's own type
  if (strEQc (dxfname, "LWPOLYLINE") && obj->fixedtype == DWG_TYPE_LWPOLYLINE)
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
  if (strEQc (dxfname, "GEOPOSITIONMARKER")
      && obj->fixedtype == DWG_TYPE_GEOPOSITIONMARKER)
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

/*
 * Calculate number of polygon segments for a curve (arc/circle/ellipse)
 * based on the drawing extent (zoom-level) and radius.
 * Implements approach similar to circle.js: steps ~ sqrt(radius/extent)
 * with angle_span in radians.
 */
static int
calc_curve_segments (Dwg_Data *restrict dwg, BITCODE_BD radius,
                     BITCODE_BD angle_span)
{
  double extent, scale;
  int num_pts;
  extent = fmax (fabs (dwg->header_vars.LIMMAX.x - dwg->header_vars.LIMMIN.x),
                 fabs (dwg->header_vars.LIMMAX.y - dwg->header_vars.LIMMIN.y));
  if (extent < 1e-12)
    extent = 1.0;
  // Scale: more segments for larger radius relative to extent.
  // angle_span / (2 * M_PI) gives the fraction of a full circle.
  // sqrt(radius / extent) scales with feature size.
  scale = sqrt (fabs (radius) / extent);
  num_pts = (int)(128.0 * scale * (angle_span / (2 * M_PI)));
  if (num_pts < 16)
    num_pts = 16;
  if (num_pts > 256)
    num_pts = 256;
  return num_pts;
}

static int
dwg_geojson_object (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                    Dwg_Object *restrict obj, int is_last)
{
  switch (obj->fixedtype)
    {
    case DWG_TYPE_INSERT: // Just the insertion point yet
    case DWG_TYPE_ACIDBLOCKREFERENCE:
      {
        Dwg_Entity_INSERT *_obj = obj->tio.entity->tio.INSERT;
        BITCODE_3DPOINT pt;
        transform_OCS (&pt, _obj->ins_pt, _obj->extrusion);
        FEATURE (AcDbEntity : AcDbBlockReference, obj);
        // TODO: explode insert into a GeometryCollection
        GEOMETRY (Point);
        KEY (coordinates);
        LASTVALUE_3DPOINT (pt.x, pt.y, pt.z);
        ENDGEOMETRY;
        ENDFEATURE;
        return 1;
      }
    case DWG_TYPE_MINSERT:
      {
        // a grid of INSERT's: a MultiPoint
        Dwg_Entity_MINSERT *_obj = obj->tio.entity->tio.MINSERT;
        const double c = cos (_obj->rotation), s = sin (_obj->rotation);
        const size_t n = (size_t)_obj->num_cols * _obj->num_rows;
        dwg_point_3d *pts;
        BITCODE_BS r, col;
        size_t k = 0;
        int ret;
        if (!n || n > 1000000)
          return 0;
        pts = (dwg_point_3d *)malloc (n * sizeof (dwg_point_3d));
        if (!pts)
          return 0;
        for (r = 0; r < _obj->num_rows; r++)
          for (col = 0; col < _obj->num_cols; col++)
            {
              BITCODE_3DPOINT p = _obj->ins_pt, q;
              const double dx = col * _obj->col_spacing;
              const double dy = r * _obj->row_spacing;
              p.x += dx * c - dy * s;
              p.y += dx * s + dy * c;
              transform_OCS (&q, p, _obj->extrusion);
              pts[k].x = q.x;
              pts[k].y = q.y;
              pts[k++].z = q.z;
            }
        ret = geojson_points (dat, obj, "AcDbEntity : AcDbMInsertBlock",
                              is_last, GJ_MULTIPOINT, pts, (BITCODE_BL)n);
        free (pts);
        return ret;
      }
    case DWG_TYPE_POLYLINE_2D:
      {
        int error;
        BITCODE_BL j, numpts, num_bulge_pts;
        // bool is_polygon = false;
        int changed = 0;
        dwg_point_2d *pts, *orig, *spline;
        double *bulges;
        Dwg_Entity_POLYLINE_2D *_obj = obj->tio.entity->tio.POLYLINE_2D;
        numpts = dwg_object_polyline_2d_get_numpoints (obj, &error);
        if (error || !numpts)
          return 0;
        spline
            = _obj->flag & 4 ? polyline_2d_spline_points (obj, &numpts) : NULL;
        pts = spline ? spline
                     : dwg_object_polyline_2d_get_points (obj, &error);
        if (error || !pts)
          {
            free (pts);
            return 0;
          }
        bulges
            = spline ? NULL : dwg_object_polyline_2d_get_bulges (obj, &error);
        if (bulges)
          {
            dwg_point_2d *bulge_pts = bulge_points (
                dwg, pts, bulges, numpts, _obj->flag & 1, &num_bulge_pts);
            if (bulge_pts)
              {
                free (pts);
                pts = bulge_pts;
                numpts = num_bulge_pts;
              }
            free (bulges);
          }
        geojson_ocs_points (pts, numpts, _obj->extrusion, _obj->elevation);

        // if closed and num_points > 3 use a Polygon
        FEATURE (AcDbEntity : AcDbPolyline, obj);
        if (_obj->flag & 1 && numpts > 3)
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
      {
        Dwg_Entity_ARC *_obj = obj->tio.entity->tio.ARC;
        BITCODE_2BD ctr = { _obj->center.x, _obj->center.y };
        BITCODE_2BD *pts;
        double end_angle = angle_normalize (_obj->end_angle);
        double angle_span;
        int num_pts;
        if (end_angle < _obj->start_angle)
          end_angle += 2 * M_PI;
        angle_span = end_angle - _obj->start_angle;
        num_pts = calc_curve_segments (dwg, _obj->radius, angle_span);
        if (num_pts <= 0 || num_pts > 10000)
          {
            LOG_ERROR ("Invalid angles");
            return DWG_ERR_VALUEOUTOFBOUNDS;
          }
        pts = (BITCODE_2BD *)malloc ((num_pts + 1) * sizeof (BITCODE_2BD));
        if (!pts)
          {
            LOG_ERROR ("Out of memory");
            return DWG_ERR_OUTOFMEM;
          }
        arc_split (pts, num_pts, ctr, _obj->start_angle, _obj->end_angle,
                   _obj->radius);
        // arc_split stops one step before the end point
        pts[num_pts].x = ctr.x + _obj->radius * cos (_obj->end_angle);
        pts[num_pts].y = ctr.y + _obj->radius * sin (_obj->end_angle);
        geojson_ocs_points ((dwg_point_2d *)pts, num_pts + 1, _obj->extrusion,
                            _obj->center.z);
        FEATURE (AcDbEntity : AcDbArc, obj);
        GEOMETRY (LineString)
        KEY (coordinates);
        ARRAY;
        for (int j = 0; j < num_pts; j++)
          {
            VALUE_2DPOINT (pts[j].x, pts[j].y)
          }
        LASTVALUE_2DPOINT (pts[num_pts].x, pts[num_pts].y);
        LASTENDARRAY;
        ENDGEOMETRY;
        ENDFEATURE;
        free (pts);
        return 1;
      }
    case DWG_TYPE_CIRCLE:
      {
        Dwg_Entity_CIRCLE *_obj = obj->tio.entity->tio.CIRCLE;
        BITCODE_2BD ctr = { _obj->center.x, _obj->center.y };
        int num_pts = calc_curve_segments (dwg, _obj->radius, 2 * M_PI);
        BITCODE_2BD *pts
            = (BITCODE_2BD *)malloc (num_pts * sizeof (BITCODE_2BD));
        if (!pts)
          {
            LOG_ERROR ("Out of memory");
            return DWG_ERR_OUTOFMEM;
          }
        arc_split (pts, num_pts, ctr, 0, M_PI * 2.0, _obj->radius);
        geojson_circle_wcs (pts, num_pts, _obj);
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
        return 1;
      }
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
      {
        // really a Polygon, the 4th corner is the 3rd for triangles
        Dwg_Entity__3DFACE *_obj = obj->tio.entity->tio._3DFACE;
        dwg_point_3d pts[5];
        BITCODE_BL n = 3;
        SET_PT (pts[0], _obj->corner1);
        SET_PT (pts[1], _obj->corner2);
        SET_PT (pts[2], _obj->corner3);
        if (_obj->corner4.x != _obj->corner3.x
            || _obj->corner4.y != _obj->corner3.y
            || _obj->corner4.z != _obj->corner3.z)
          {
            SET_PT (pts[n], _obj->corner4);
            n++;
          }
        pts[n++] = pts[0];
        return geojson_points (dat, obj, "AcDbEntity : AcDbFace", is_last,
                               GJ_POLYGON, pts, n);
      }
    case DWG_TYPE_POLYLINE_PFACE:
      return geojson_polyline_pface (dat, dwg, obj, is_last);
    case DWG_TYPE_POLYLINE_MESH:
      return geojson_polyline_mesh (dat, dwg, obj, is_last);
    case DWG_TYPE_SOLID:
      {
        Dwg_Entity_SOLID *_obj = obj->tio.entity->tio.SOLID;
        return geojson_solid (dat, obj, "AcDbEntity : AcDbSolid", is_last,
                              _obj->corner1, _obj->corner2, _obj->corner3,
                              _obj->corner4, _obj->elevation, _obj->extrusion);
      }
    case DWG_TYPE_TRACE:
      {
        Dwg_Entity_TRACE *_obj = obj->tio.entity->tio.TRACE;
        return geojson_solid (dat, obj, "AcDbEntity : AcDbTrace", is_last,
                              _obj->corner1, _obj->corner2, _obj->corner3,
                              _obj->corner4, _obj->elevation, _obj->extrusion);
      }
    case DWG_TYPE_ELLIPSE:
      return dwg_geojson_ELLIPSE (dat, obj, is_last);
    case DWG_TYPE_SPLINE:
      return dwg_geojson_SPLINE (dat, obj, is_last);
    case DWG_TYPE_HATCH:
      return dwg_geojson_HATCH (dat, obj, is_last);
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
    case DWG_TYPE_XLINE:
      {
        // infinite lines: the point and the unit direction
        Dwg_Entity_RAY *_obj = obj->tio.entity->tio.RAY;
        dwg_point_3d pts[2];
        SET_PT (pts[0], _obj->point);
        pts[1].x = _obj->point.x + _obj->vector.x;
        pts[1].y = _obj->point.y + _obj->vector.y;
        pts[1].z = _obj->point.z + _obj->vector.z;
        return geojson_points (dat, obj,
                               obj->fixedtype == DWG_TYPE_RAY
                                   ? "AcDbEntity : AcDbRay"
                                   : "AcDbEntity : AcDbXline",
                               is_last, GJ_LINESTRING, pts, 2);
      }
    case DWG_TYPE_TEXT:
      {
        // add Text property to a point
        Dwg_Entity_TEXT *_obj = obj->tio.entity->tio.TEXT;
        dwg_point_2d pt;
        pt.x = _obj->ins_pt.x;
        pt.y = _obj->ins_pt.y;
        geojson_ocs_points (&pt, 1, _obj->extrusion, _obj->elevation);
        FEATURE (AcDbEntity : AcDbText, obj);
        GEOMETRY (Point);
        KEY (coordinates);
        LASTVALUE_2DPOINT (pt.x, pt.y);
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
      {
        Dwg_Entity_MLINE *_obj = obj->tio.entity->tio.MLINE;
        const int closed = _obj->flags & 2;
        dwg_point_3d *pts;
        BITCODE_BL j, n = _obj->num_verts;
        int ret;
        if (!_obj->verts || n < 2)
          return 0;
        pts = (dwg_point_3d *)malloc ((n + 1) * sizeof (dwg_point_3d));
        if (!pts)
          return 0;
        for (j = 0; j < n; j++)
          {
            BITCODE_3DPOINT q;
            transform_OCS (&q, _obj->verts[j].vertex, _obj->extrusion);
            pts[j].x = q.x;
            pts[j].y = q.y;
            pts[j].z = q.z;
          }
        if (closed)
          pts[n++] = pts[0];
        ret = geojson_points (dat, obj, "AcDbEntity : AcDbMline", is_last,
                              GJ_LINESTRING, pts, n);
        free (pts);
        return ret;
      }
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
      success = dwg_geojson_object (dat, dwg, obj, is_last);
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
  // DXF/JSON input keeps T strings as TV, as in dwg_write_dxf
  dat->opts |= dwg->opts & DWG_OPTS_IN;

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
