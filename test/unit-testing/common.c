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

dwg_data g_dwg;

/// This function declaration reads the DWG file
int test_code (char *filename);

/// Return the name of a handle
char *handle_name (const Dwg_Data *restrict dwg,
                   Dwg_Object_Ref *restrict hdl);

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

/// dynapi based processing function declaration
void dynapi_process (dwg_object *obj);

/// API based printing function declaration
void print_api (dwg_object *obj);

/// Low level printing function declaration
void print_low_level (dwg_object *obj);

/// Main function
int
main (int argc, char *argv[])
{
  char *input = getenv ("INPUT");
  struct stat attrib;

  if (!input)
    {
      input = (char *)"example_2000.dwg"; // todo:
                                          // ../test-data/example_2018.dwg
      if (stat (input, &attrib))
        {
          fprintf (stderr, "Env var INPUT not defined, %s not found\n", input);
          return EXIT_FAILURE;
        }
    }

  return test_code (input);
}

/// read the DWG file
int
test_code (char *filename)
{
  int error;

  error = dwg_read_file (filename, &g_dwg);
  if (error < DWG_ERR_CRITICAL)
    {
      output_test (&g_dwg);
      dwg_free (&g_dwg);
    }

  /* This value is the return value for `main',
     so clamp it to either 0 or 1.  */
  return error >= DWG_ERR_CRITICAL ? 1 : 0;
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
  if (DWG_TYPE == DWG_TYPE_ATTDEF)
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
#endif
}

/// Main output function that prints to the terminal
void
output_process (dwg_object *obj)
{
  //print_low_level (obj);
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

  if (dwg_object_get_type (obj) == DWG_TYPE)
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
  //low_level_process (obj);
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
  printf ("\n");
}

#define CHK_ENTITY_UTF8TEXT(ent, name, field, value) \
  if (dwg_dynapi_entity_utf8text (ent, #name, #field, &value, NULL)) \
    printf ("ok " #name "." #field ":\t\"%s\"\n", value); \
  else { \
    printf ("not ok in reading " #name "." #field "\n"); \
    exit (1); \
  }

#define CHK_ENTITY_TYPE(ent, name, field, type, value) \
  if (dwg_dynapi_entity_value (ent, #name, #field, &value, NULL)) { \
    if (value == ent->field) \
      printf ("ok " #name "." #field ":\t" FORMAT_##type "\n", value); \
    else { \
      printf ("not ok " #name "." #field ":\t" FORMAT_##type "\n", value); \
      exit (1); \
    } \
  } \
  else { \
    printf ("not ok in reading " #name "." #field "\n"); \
    exit (1); \
  }

#define CHK_ENTITY_H(ent, name, field, hdl) \
  { \
    if (dwg_dynapi_entity_value (ent, #name, #field, &hdl, NULL)) {     \
      char *_hdlname = dwg_dynapi_handle_name (obj->parent, hdl);       \
      if (hdl == ent->field)                                            \
        {                                                               \
          printf ("ok " #name "." #field ":\t");                        \
          if (_hdlname) printf ("%s ", _hdlname);                       \
          printf ("(%x.%d.%lX)\n", hdl->handleref.code,                 \
                    hdl->handleref.size, hdl->handleref.value);         \
        }                                                               \
      else                                                              \
        {                                                               \
          printf ("not ok " #name "." #field ":\t");                    \
          if (_hdlname) printf ("%s ", _hdlname);                       \
          printf ("(%x.%d.%lX)\n", hdl->handleref.code,                 \
                    hdl->handleref.size, hdl->handleref.value);         \
          exit (1);                                                     \
        }                                                               \
    }                                                                   \
    else                                                                \
      {                                                                 \
        printf ("not ok in reading " #name "." #field "\n");            \
        exit (1);                                                       \
      }                                                                 \
  }

#define CHK_ENTITY_2RD(ent, name, field, value) \
  if (dwg_dynapi_entity_value (ent, #name, #field, &value, NULL)) { \
    if (value.x == ent->field.x && value.y == ent->field.y) \
      printf ("ok " #name "." #field ":\t(%f, %f)\n", value.x, value.y); \
    else { \
      printf ("not ok " #name "." #field ":\t(%f, %f)\n", value.x, value.y); \
      exit (1); \
    } \
  } \
  else { \
    printf ("not ok in reading " #name "." #field "\n"); \
    exit (1); \
  }

#define CHK_ENTITY_3RD(ent, name, field, value) \
  if (dwg_dynapi_entity_value (ent, #name, #field, &value, NULL)) { \
    if (value.x == ent->field.x && \
        value.y == ent->field.y &&  \
        value.z == ent->field.z) \
      printf ("ok " #name "." #field ":\t(%f, %f, %f)\n", value.x, value.y, \
              value.z);                                                 \
    else \
      printf ("not ok " #name "." #field ":\t(%f, %f, %f)\n", value.x, value.y, \
              value.z);                                                 \
  } \
  else \
    printf ("not ok in reading " #name "." #field "\n")

#define _DWGAPI_ENT_NAME(name, field) dwg_ent_ ## name ## _get_ ##field
#define DWGAPI_ENT_NAME(ent, field) _DWGAPI_ENT_NAME(ent, field)

#define CHK_ENTITY_TYPE_W_OLD(ent, name, field, type, value) \
  CHK_ENTITY_TYPE(ent, name, field, type, value); \
  if (DWGAPI_ENT_NAME(ent, field) (ent, &error) != value || error) \
    { \
      printf ("Error with old API dwg_ent_" #ent "_get_ " #field "\n"); \
      exit (1); \
    }

#define CHK_ENTITY_2RD_W_OLD(ent, name, field, value) \
  CHK_ENTITY_2RD(ent, name, field, value); \
  { \
    dwg_point_2d _pt2d; \
    DWGAPI_ENT_NAME(ent, field) (ent, &_pt2d, &error);     \
    if (error || memcmp (&value, &_pt2d, sizeof (value))) \
      { \
        printf ("Error with old API dwg_ent_" #ent "_get_ " #field "\n"); \
        exit (1); \
      } \
  }

#define CHK_ENTITY_3RD_W_OLD(ent, name, field, value) \
  CHK_ENTITY_3RD(ent, name, field, value); \
  { \
    dwg_point_3d _pt3d; \
    DWGAPI_ENT_NAME(ent, field) (ent, &_pt3d, &error);     \
    if (error || memcmp (&value, &_pt3d, sizeof (value))) \
      { \
        printf ("Error with old API dwg_ent_" #ent "_get_ " #field "\n"); \
        exit (1); \
      } \
  }

// allow old deprecated API
GCC31_DIAG_IGNORE (-Wdeprecated-declarations)
GCC46_DIAG_IGNORE (-Wdeprecated-declarations)
