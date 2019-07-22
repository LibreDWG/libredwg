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
      obj = get_next_owned_entity (hdr, obj);
    }
}

/// Iterate over both modelspace and paperspace blocks
void
output_test (dwg_data *dwg)
{
  unsigned int i, num_hdr_objs;
  int error;
  dwg_object *obj;
  dwg_obj_block_header *_hdr;
  dwg_obj_block_control *_ctrl;
  dwg_object_ref **hdr_refs;

  _hdr = dwg_get_block_header (dwg, &error);
  _ctrl = dwg_block_header_get_block_control (_hdr, &error);
  /*
  hdr_refs = dwg_obj_block_control_get_block_headers(ctrl, &error);
  num_hdr_objs = dwg_obj_block_control_get_num_entries(ctrl, &error);
  */
  output_BLOCK_HEADER (dwg_obj_block_control_get_model_space (_ctrl, &error));
  output_BLOCK_HEADER (dwg_obj_block_control_get_paper_space (_ctrl, &error));
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
  printf ("Unit-testing type %d:\n", DWG_TYPE);
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
  if (dwg_dynapi_entity_value (ent, #name, #field, &hdl, NULL)) { \
    if (hdl == ent->field) \
      printf ("ok " #name "." #field ":\t(%x.%d.%lX)\n", hdl->handleref.code, \
              hdl->handleref.size, hdl->handleref.value); \
    else { \
      printf ("not ok " #name "." #field ":\t(%x.%d.%lX)\n", hdl->handleref.code, \
              hdl->handleref.size, hdl->handleref.value); \
      exit (1); \
    } \
  } \
  else { \
    printf ("not ok in reading " #name "." #field "\n"); \
    exit (1); \
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

// allow old deprecated API
GCC31_DIAG_IGNORE (-Wdeprecated-declarations)
GCC46_DIAG_IGNORE (-Wdeprecated-declarations)
