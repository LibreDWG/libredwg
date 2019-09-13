#include "../../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/stat.h>
#include "../../src/common.h"

#include "dwg.h"
#include "dwg_api.h"
#include "tests_common.h"

dwg_data g_dwg;

/// This function declaration reads the DWG file
int test_code (const char *filename);

/// Return the name of a handle
char *handle_name (const Dwg_Data *restrict dwg, Dwg_Object_Ref *restrict hdl);

/// Declaration of function to iterate over objects of a block
void output_BLOCK_HEADER (dwg_object_ref *ref);

/// Declaration for function that checks the dwg type and calls output_process
void output_object (dwg_object *obj);

/// Function declaration for blocks to be iterated over
void output_test (dwg_data *dwg);

void output_process (dwg_object *obj);

/// Low Level processing function declaration
void low_level_process (dwg_object *obj);

/// API based processing function declaration
void api_process (dwg_object *obj);

void api_common_entity (dwg_object *obj);

void api_common_object (dwg_object *obj);

/// API based printing function declaration
void print_api (dwg_object *obj);

/// Low level printing function declaration
void print_low_level (dwg_object *obj);

/// Main function
int
main (int argc, char *argv[])
{
  char *input = getenv ("INPUT");
  int error = 0;

  if (!input)
    {
      char **ptr;
      struct stat attrib;
      const char *const files[]
          = { "example_2000.dwg", "example_2004.dwg", "example_2007.dwg",
              "example_2010.dwg", "example_2013.dwg", "example_2018.dwg",
              "example_r14.dwg",
              //"2000/PolyLine2D.dwg",
              //"2007/PolyLine3D.dwg",
              NULL };
      const char *prefix = "../test-data/";

      /* ../configure out-of-tree. find srcdir */
      if (stat (prefix, &attrib))
        prefix = "../../../test/test-data/";
      if (stat (prefix, &attrib)) /* dir not found. one more level up */
        prefix = "../../../../test/test-data/";

      for (ptr = (char **)&files[0]; *ptr; ptr++)
        {
          if (stat (*ptr, &attrib))
            {
              char tmp[80];
              strcpy (tmp, prefix);
              strcat (tmp, *ptr);
              if (stat (tmp, &attrib))
                fprintf (stderr, "Env var INPUT not defined, %s not found\n",
                         tmp);
              else
                error += test_code (tmp);
            }
          else
            error += test_code (*ptr);
        }
      if (!numpassed () && !numfailed ())
        {
          char tmp[80];
          if (DWG_TYPE == DWG_TYPE_POLYLINE_2D || DWG_TYPE == DWG_TYPE_SEQEND
              || DWG_TYPE == DWG_TYPE_VERTEX_2D)
            {
              strcpy (tmp, prefix);
              strcat (tmp, "2000/PolyLine2D.dwg");
              error += test_code (tmp);
            }
          if (DWG_TYPE == DWG_TYPE_POLYLINE_MESH
              || DWG_TYPE == DWG_TYPE_VERTEX_MESH || DWG_TYPE == DWG_TYPE_TRACE
              || DWG_TYPE == DWG_TYPE_DIMENSION_ANG3PT
              || DWG_TYPE == DWG_TYPE_DIMENSION_DIAMETER
              || DWG_TYPE == DWG_TYPE_DIMENSION_RADIUS
              || DWG_TYPE == DWG_TYPE_OLE2FRAME)
            {
              strcpy (tmp, prefix);
              strcat (tmp, "2000/TS1.dwg");
              error += test_code (tmp);
            }
        }
    }
  else
    error = test_code (input);

#ifdef DWG_TYPE
  if (!numpassed () && !numfailed ())
    printf ("TODO no coverage for this type %d\n", DWG_TYPE);
#endif
  return error;
}

/// read the DWG file
int
test_code (const char *filename)
{
  int error;

#ifdef DWG_TYPE
  printf ("Testing with %s:\n", filename);
#endif
  error = dwg_read_file (filename, &g_dwg);
  if (error < DWG_ERR_CRITICAL)
    {
      output_test (&g_dwg);
      dwg_free (&g_dwg);
    }

  /* This value is the return value for `main',
     so clamp it to either 0 or 1.  */
  error = (error >= DWG_ERR_CRITICAL || numfailed () > 0) ? 1 : 0;
  if (error)
    printf ("%s failed\n", filename);
  return error;
}

/// This function is used to iterate over the objects in a block
void
output_BLOCK_HEADER (dwg_object_ref *ref)
{
  dwg_object *hdr, *obj;
  int error;

  if (!ref)
    {
      fprintf (stderr, "output_BLOCK_HEADER: null argument\n");
      return;
    }
  hdr = dwg_ref_get_object (ref, &error);
  if (!hdr)
    {
      fprintf (stderr, "Found null reference object\n");
      return;
    }

  obj = get_first_owned_entity (hdr);
  while (obj)
    {
      output_object (obj);
      if (dwg_obj_has_subentity (obj))
        {
          Dwg_Object *owner = obj;
          obj = get_first_owned_subentity (owner);
          while (obj)
            {
              output_object (obj);
              obj = get_next_owned_subentity (owner, obj);
            }
          obj = get_next_owned_entity (hdr, owner);
        }
      else
        obj = get_next_owned_entity (hdr, obj);
    }
}

/// Iterate over both modelspace and paperspace blocks
void
output_test (dwg_data *dwg)
{
  int error;
  dwg_object *obj;
  dwg_obj_block_header *_hdr;
  dwg_obj_block_control *_ctrl;
  dwg_object_ref *ref, **hdr_refs;

  _hdr = dwg_get_block_header (dwg, &error);
  _ctrl = dwg_block_header_get_block_control (_hdr, &error);

  /* process all owned entities */
  ref = dwg_obj_block_control_get_model_space (_ctrl, &error);
  if (!error)
    output_BLOCK_HEADER (ref);
  ref = dwg_obj_block_control_get_paper_space (_ctrl, &error);
  if (!error)
    output_BLOCK_HEADER (ref);

#ifdef DWG_TYPE
  if (!numpassed () && !numfailed ())
    {
      obj = &dwg->object[0];
      while ((obj = dwg_next_object (obj)))
        {
          // printf ("%s [%d]\n", obj->name, obj->index);
          if (obj->type == DWG_TYPE || obj->fixedtype == DWG_TYPE)
            {
              output_object (obj);
            }
        }
    }
    /* also process blocks? we better find DWGs with these */
#  if 0
  if (DWG_TYPE == DWG_TYPE_ATTDEF ||
      DWG_TYPE == DWG_TYPE_BLOCK ||
      DWG_TYPE == DWG_TYPE_BODY ||
      DWG_TYPE == DWG_TYPE_CIRCLE ||
      DWG_TYPE == DWG_TYPE_DIMENSION_ANG3PT ||
      DWG_TYPE == DWG_TYPE_DIMENSION_DIAMETER ||
      DWG_TYPE == DWG_TYPE_DIMENSION_RADIUS ||
      DWG_TYPE == DWG_TYPE_ENDBLK ||
      DWG_TYPE == DWG_TYPE_MINSERT ||
      DWG_TYPE == DWG_TYPE_OLE2FRAME ||
      DWG_TYPE == DWG_TYPE_POLYLINE_2D ||
      DWG_TYPE == DWG_TYPE_POLYLINE_MESH ||
      DWG_TYPE == DWG_TYPE_SEQEND ||
      DWG_TYPE == DWG_TYPE_SHAPE ||
      DWG_TYPE == DWG_TYPE_SOLID ||
      DWG_TYPE == DWG_TYPE_TRACE ||
      DWG_TYPE == DWG_TYPE_VERTEX_2D ||
      DWG_TYPE == DWG_TYPE_VERTEX_MESH ||
      DWG_TYPE == DWG_TYPE_XRECORD
      )
    {
      /* and now also all subtypes and entities in blocks */
      unsigned int i;
      unsigned int num_hdr_objs = dwg_obj_block_control_get_num_entries(_ctrl, &error);
      if (error || !num_hdr_objs)
        return;
      hdr_refs = dwg_obj_block_control_get_block_headers(_ctrl, &error);
      if (error)
        return;
      for (i = 0; i < num_hdr_objs; i++)
        {
          if (hdr_refs[i])
            output_BLOCK_HEADER (hdr_refs[i]);
        }
      free (hdr_refs);
    }
#  endif
#endif
}

/// Main output function that prints to the terminal
void
output_process (dwg_object *obj)
{
  // print_low_level (obj);
  print_api (obj);
}

#ifdef DWG_TYPE
void
output_object (dwg_object *obj)
{
  if (!obj)
    {
      printf ("object is NULL\n");
      return;
    }
  if (obj->type == DWG_TYPE || obj->fixedtype == DWG_TYPE)
    {
      output_process (obj);
    }
}
#endif

/// Low level printing function
void
print_low_level (dwg_object *obj)
{
  printf ("\n");
  printf ("Printed via low level access:\n");
  // low_level_process (obj);
  printf ("\n");
}

/// API based printing function
void
print_api (dwg_object *obj)
{
#ifdef DWG_TYPE
  printf ("Unit-testing type %d %s:\n", DWG_TYPE, obj->name);
#else
  printf ("Test dwg_api and dynapi:\n");
#endif
  api_process (obj);

  if (obj->supertype == DWG_SUPERTYPE_ENTITY)
    api_common_entity (obj);
  else if (obj->supertype == DWG_SUPERTYPE_OBJECT)
    api_common_object (obj);
  printf ("\n");
}

#define CHK_COMMON_TYPE(ent, field, type, value)                              \
  {                                                                           \
    if (dwg_dynapi_common_value (ent, #field, &value, NULL))                  \
      {                                                                       \
        if (value == ent->parent->field)                                      \
          pass (); /*(#field ":\t" FORMAT_##type, value);*/                   \
        else                                                                  \
          fail (#field ":\t" FORMAT_##type, value);                           \
      }                                                                       \
    else                                                                      \
      fail (#field);                                                          \
  }

#define CHK_COMMON_H(ent, field, hdl)                                         \
  {                                                                           \
    if (!dwg_dynapi_common_value (ent, #field, &hdl, NULL))                   \
      fail (#field);                                                          \
    else if (!hdl)                                                            \
      pass ();                                                                \
    else                                                                      \
      {                                                                       \
        char *_hdlname = dwg_dynapi_handle_name (ent->parent->dwg, hdl);      \
        if (hdl == (BITCODE_H)ent->parent->field)                             \
          ok (#field ": %s " FORMAT_REF, _hdlname ?: "", ARGS_REF (hdl));     \
        else                                                                  \
          fail (#field ": %s " FORMAT_REF, _hdlname ?: "", ARGS_REF (hdl));   \
      }                                                                       \
  }

#define CHK_COMMON_HV(ent, field, hdlp, num)                                  \
  if (!dwg_dynapi_common_value (ent, #field, &hdlp, NULL))                    \
    fail (#field);                                                            \
  else if (!hdlp)                                                             \
    pass ();                                                                  \
  else                                                                        \
    {                                                                         \
      for (int _i = 0; _i < (int)(num); _i++)                                 \
        {                                                                     \
          BITCODE_H _hdl = hdlp[_i];                                          \
          char *_hdlname = dwg_dynapi_handle_name (ent->parent->dwg, _hdl);   \
          if (_hdl == ent->parent->field[_i])                                 \
            {                                                                 \
              ok (#field "[%d]: %s " FORMAT_REF, _i, _hdlname ?: "",          \
                  ARGS_REF (_hdl));                                           \
            }                                                                 \
          else                                                                \
            {                                                                 \
              fail (#field "[%d]: %s " FORMAT_REF, _i, _hdlname ?: "",        \
                    ARGS_REF (_hdl));                                         \
            }                                                                 \
        }                                                                     \
    }

void
api_common_entity (dwg_object *obj)
{
  BITCODE_BB entmode;
  BITCODE_H handle;
  BITCODE_BL num_reactors, num_eed;
  BITCODE_H *reactors;
  BITCODE_B xdic_missing_flag, has_ds_binary_data, preview_exists;
  BITCODE_RC linewt;
  BITCODE_RL preview_size_rl;
  BITCODE_BLL preview_size;
  BITCODE_BD ltype_scale;
  BITCODE_BS invisible;
  Dwg_Object_Entity *_ent = obj->tio.entity;
  Dwg_Entity_LINE *ent = obj->tio.entity->tio.LINE;
  Dwg_Version_Type version = obj->parent->header.version;

  CHK_COMMON_TYPE (ent, entmode, BB, entmode)
  CHK_COMMON_TYPE (ent, preview_exists, B, preview_exists)
  if (preview_exists)
    {
      if (version > R_2010)
        CHK_COMMON_TYPE (ent, preview_size, BLL, preview_size)
      else
        CHK_COMMON_TYPE (ent, preview_size, RL, preview_size_rl);
    }
  if (entmode == 3 || entmode == 0)
    CHK_COMMON_H (ent, ownerhandle, handle);
  CHK_COMMON_H (ent, layer, handle);
  if (version < R_2000)
    {
      if (_ent->isbylayerlt)
        CHK_COMMON_H (ent, ltype, handle);
      if (!_ent->nolinks)
        {
          CHK_COMMON_H (ent, prev_entity, handle);
          CHK_COMMON_H (ent, next_entity, handle);
        }
    }
  else
    {
      if (_ent->ltype_flags == 3)
        CHK_COMMON_H (ent, ltype, handle);
    }
  CHK_COMMON_TYPE (ent, linewt, RC, linewt);
  CHK_COMMON_TYPE (ent, ltype_scale, BD, ltype_scale);
  if (version >= R_2007)
    {
      if (_ent->material_flags == 3)
        CHK_COMMON_H (ent, material, handle);
      if (_ent->shadow_flags == 3)
        CHK_COMMON_H (ent, shadow, handle);
    }
  if (version >= R_2000)
    {
      if (_ent->plotstyle_flags == 3)
        CHK_COMMON_H (ent, plotstyle, handle);
    }
  if (version >= R_2010)
    {
      if (_ent->has_full_visualstyle)
        CHK_COMMON_H (ent, full_visualstyle, handle);
      if (_ent->has_face_visualstyle)
        CHK_COMMON_H (ent, face_visualstyle, handle);
      if (_ent->has_edge_visualstyle)
        CHK_COMMON_H (ent, edge_visualstyle, handle);
    }
  CHK_COMMON_TYPE (ent, invisible, BS, invisible);

  CHK_COMMON_TYPE (ent, xdic_missing_flag, B, xdic_missing_flag);
  if (!xdic_missing_flag)
    CHK_COMMON_H (ent, xdicobjhandle, handle);
  CHK_COMMON_TYPE (ent, num_reactors, BL, num_reactors);
  CHK_COMMON_HV (ent, reactors, reactors, num_reactors);
  CHK_COMMON_TYPE (ent, has_ds_binary_data, B, has_ds_binary_data);
  CHK_COMMON_TYPE (ent, num_eed, BL, num_eed);
}

#define CHK_ENTITY_UTF8TEXT(ent, name, field, value)                          \
  if (dwg_dynapi_entity_utf8text (ent, #name, #field, &value, NULL))          \
    ok (#name "." #field ":\t\"%s\"", value);                                 \
  else                                                                        \
    {                                                                         \
      Dwg_Version_Type _dwg_version = ent->parent->dwg->header.version;       \
      if (_dwg_version < R_2007)                                              \
        fail (#name "." #field ":\t\"%s\"", value);                           \
      else                                                                    \
        fail (#name "." #field);                                              \
    }

#define CHK_ENTITY_TYPE(ent, name, field, type, value)                        \
  if (!dwg_dynapi_entity_value (ent, #name, #field, &value, NULL))            \
    fail (#name "." #field);                                                  \
  else                                                                        \
    {                                                                         \
      if (value == ent->field)                                                \
        ok (#name "." #field ":\t" FORMAT_##type, value);                     \
      else                                                                    \
        fail (#name "." #field ":\t" FORMAT_##type, value);                   \
    }

#define CHK_ENTITY_H(ent, name, field, hdl)                                   \
  {                                                                           \
    if (!dwg_dynapi_entity_value (ent, #name, #field, &hdl, NULL))            \
      fail (#name "." #field);                                                \
    else if (!hdl)                                                            \
      pass ();                                                                \
    else                                                                      \
      {                                                                       \
        char *_hdlname = dwg_dynapi_handle_name (obj->parent, hdl);           \
        if (hdl == ent->field)                                                \
          {                                                                   \
            ok (#name "." #field ": %s " FORMAT_REF, _hdlname ?: "",          \
                ARGS_REF (hdl));                                              \
          }                                                                   \
        else                                                                  \
          {                                                                   \
            fail (#name "." #field ": %s " FORMAT_REF, _hdlname ?: "",        \
                  ARGS_REF (hdl));                                            \
          }                                                                   \
      }                                                                       \
  }

#define CHK_ENTITY_HV(ent, name, field, hdlp, num)                            \
  if (!dwg_dynapi_entity_value (ent, #name, #field, &hdlp, NULL))             \
    fail (#name "." #field);                                                  \
  else if (!hdlp)                                                             \
    pass ();                                                                  \
  else                                                                        \
    {                                                                         \
      for (int _i = 0; _i < (int)(num); _i++)                                 \
        {                                                                     \
          BITCODE_H _hdl = hdlp[_i];                                          \
          char *_hdlname = dwg_dynapi_handle_name (obj->parent, _hdl);        \
          if (_hdl == ent->field[_i])                                         \
            {                                                                 \
              ok (#name "." #field "[%d]: %s " FORMAT_REF, _i,                \
                  _hdlname ?: "", ARGS_REF (_hdl));                           \
            }                                                                 \
          else                                                                \
            {                                                                 \
              fail (#name "." #field "[%d]: %s " FORMAT_REF, _i,              \
                    _hdlname ?: "", ARGS_REF (_hdl));                         \
            }                                                                 \
        }                                                                     \
    }

#define CHK_ENTITY_2RD(ent, name, field, value)                               \
  if (!dwg_dynapi_entity_value (ent, #name, #field, &value, NULL))            \
    fail (#name "." #field);                                                  \
  else                                                                        \
    {                                                                         \
      if (value.x == ent->field.x && value.y == ent->field.y)                 \
        ok (#name "." #field ":\t(%f, %f)", value.x, value.y);                \
      else                                                                    \
        fail (#name "." #field ":\t(%f, %f)", value.x, value.y);              \
    }

#define CHK_ENTITY_3RD(ent, name, field, value)                               \
  if (!dwg_dynapi_entity_value (ent, #name, #field, &value, NULL))            \
    fail (#name "." #field);                                                  \
  else                                                                        \
    {                                                                         \
      if (value.x == ent->field.x && value.y == ent->field.y                  \
          && value.z == ent->field.z)                                         \
        ok (#name "." #field ":\t(%f, %f, %f)", value.x, value.y, value.z);   \
      else                                                                    \
        fail (#name "." #field ":\t(%f, %f, %f)", value.x, value.y, value.z); \
    }

#define _DWGAPI_ENT_NAME(name, field) dwg_ent_##name##_get_##field
#define DWGAPI_ENT_NAME(ent, field) _DWGAPI_ENT_NAME (ent, field)

#define CHK_ENTITY_UTF8TEXT_W_OLD(ent, name, field, value)                    \
  CHK_ENTITY_UTF8TEXT (ent, name, field, value);                              \
  {                                                                           \
    Dwg_Version_Type _dwg_version = ent->parent->dwg->header.version;         \
    if (_dwg_version < R_2007 && ent->field                                   \
        && ((strcmp (DWGAPI_ENT_NAME (ent, field) (ent, &error), value)       \
             || error)))                                                      \
      fail ("old API dwg_ent_" #ent "_get_" #field ": \"%s\"", value);        \
  }

#define CHK_ENTITY_TYPE_W_OLD(ent, name, field, type, value)                  \
  CHK_ENTITY_TYPE (ent, name, field, type, value);                            \
  if (DWGAPI_ENT_NAME (ent, field) (ent, &error) != value || error)           \
  fail ("old API dwg_ent_" #ent "_get_" #field)

#define CHK_ENTITY_2RD_W_OLD(ent, name, field, value)                         \
  CHK_ENTITY_2RD (ent, name, field, value);                                   \
  {                                                                           \
    dwg_point_2d _pt2d;                                                       \
    DWGAPI_ENT_NAME (ent, field) (ent, &_pt2d, &error);                       \
    if (error || memcmp (&value, &_pt2d, sizeof (value)))                     \
      fail ("old API dwg_ent_" #ent "_get_" #field);                          \
  }

#define CHK_ENTITY_3RD_W_OLD(ent, name, field, value)                         \
  CHK_ENTITY_3RD (ent, name, field, value);                                   \
  {                                                                           \
    dwg_point_3d _pt3d;                                                       \
    DWGAPI_ENT_NAME (ent, field) (ent, &_pt3d, &error);                       \
    if (error || memcmp (&value, &_pt3d, sizeof (value)))                     \
      fail ("old API dwg_ent_" #ent "_get_" #field);                          \
  }

void
api_common_object (dwg_object *obj)
{
  BITCODE_H handle;
  BITCODE_BL num_reactors, num_eed;
  BITCODE_H *reactors;
  BITCODE_B xdic_missing_flag, has_ds_binary_data;
  Dwg_Object_Object *obj_obj = obj->tio.object;
  Dwg_Object_LAYER *_obj = obj->tio.object->tio.LAYER;

  CHK_COMMON_H (_obj, ownerhandle, handle);
  CHK_COMMON_TYPE (_obj, xdic_missing_flag, B, xdic_missing_flag);
  if (!xdic_missing_flag)
    CHK_COMMON_H (_obj, xdicobjhandle, handle);
  CHK_COMMON_TYPE (_obj, num_reactors, BL, num_reactors);
  CHK_COMMON_HV (_obj, reactors, reactors, num_reactors);
  CHK_COMMON_H (_obj, handleref, handle);
  CHK_COMMON_TYPE (_obj, has_ds_binary_data, B, has_ds_binary_data);
  CHK_COMMON_TYPE (_obj, num_eed, BL, num_eed);

#if 0
  //TODO handle is not available via dwg_dynapi_common_value()
  if (!dwg_dynapi_common_value (_obj, "handleref", &handle, NULL)
      || !handle)
    fail ("%s.handleref: %lX", obj->name, obj->handle.value);
  else
    {
      char *_hdlname = dwg_dynapi_handle_name (obj->parent, handle);
      if (!memcmp (&handle->handleref, &obj->handle, sizeof (handle->handleref)))
        {
          ok ("%s.handleref: %s (%x.%d.%lX)", obj->name,
              _hdlname ? : "",
              handle->handleref.code,
              handle->handleref.size, handle->handleref.value);
          }
        else
          {
            fail ("%s.handleref: %s (%x.%d.%lX)", obj->name,
                  _hdlname ? : "",
                  handle->handleref.code,
                  handle->handleref.size, handle->handleref.value);
          }
    }
#endif
}

// allow old deprecated API
GCC31_DIAG_IGNORE (-Wdeprecated-declarations)
GCC46_DIAG_IGNORE (-Wdeprecated-declarations)
