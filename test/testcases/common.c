#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/stat.h>

#include "dwg.h"
#include "dwg_api.h"

/// read DWG file
int test_code (char *filename);

/// iterate over objects of a block
void output_BLOCK_HEADER (dwg_object_ref * ref);

/// checks the dwg type and calls output_process
void output_object (dwg_object * obj);

/// blocks to be iterated over
void output_test (dwg_data * dwg);

/// Main output function that prints to the terminal
void output_process (dwg_object * obj);

// optional callback
void low_level_process (dwg_object * obj);

/// API based processing function declaration
void api_process (dwg_object * obj);

/// API based printing function declaration
void print_api (dwg_object * obj);

/// Main function
int
main (int argc, char *argv[])
{
  char *input = getenv ("INPUT");
  struct stat attrib;

  if (! input)
    {
      input = malloc(sizeof("example_2000.dwg"));
      strcpy(input, "example_2000.dwg");
      if (stat(input, &attrib))
        {
          fprintf (stderr, "Env var INPUT not defined, %s not found\n", input);
          return EXIT_FAILURE;
        }
    }
  return test_code (input);
}

/// This function is used to read the DWG file
int
test_code (char *filename)
{
  int error;
  dwg_data dwg;

  error = dwg_read_file (filename, &dwg);

  if (!error)
    {
      output_test (&dwg);
      dwg_free (&dwg);
    }

  /* This value is the return value for `main',
     so clamp it to either 0 or 1.  */
  return error ? 1 : 0;
}


/// This function is used to iterate over the objects in a block
void
output_BLOCK_HEADER (dwg_object_ref * ref)
{
  dwg_object *obj, *variable_obj;
  dwg_obj_block_header *hdr;
  int error;

  obj = dwg_obj_ref_get_object (ref, &error);
  if (!ref)
    {
      fprintf (stderr,
        "Found null object reference. Could not output an SVG symbol for this BLOCK\n");
      return;
    }
  if (!obj)
    {
      fprintf (stderr, "Found null ref->obj\n");
      return;
    }
  if (!dwg_object_to_object (obj, &error))
    {
      fprintf (stderr, "Found null ref->obj->tio.object\n");
      return;
    }

  hdr = dwg_object_to_BLOCK_HEADER (obj);
  variable_obj = get_first_owned_object (obj, hdr);

  while (variable_obj)
    {
      output_object (variable_obj);
      variable_obj = get_next_owned_object (obj, variable_obj, hdr);
    }

}

/// Function for blocks to be iterated
void
output_test (dwg_data * dwg)
{
  unsigned int num_hdr_objs;
  int error;
  dwg_obj_block_header *hdr;
  dwg_obj_block_control *ctrl;
  dwg_object_ref **hdr_refs;

  hdr = dwg_get_block_header (dwg, &error);
  ctrl = dwg_block_header_get_block_control (hdr, &error);

  //hdr_refs = dwg_obj_block_control_get_block_headers (ctrl, &error);
  //num_hdr_objs = dwg_obj_block_control_get_num_entries (ctrl, &error);

  output_BLOCK_HEADER (dwg_obj_block_control_get_model_space (ctrl, &error));
  output_BLOCK_HEADER (dwg_obj_block_control_get_paper_space (ctrl, &error));

}

#ifdef DWG_TYPE
/// drive output_process
void
output_object(dwg_object* obj){
  if (!obj)
    {
      printf("object is NULL\n");
      return;
    }

  if (dwg_get_type(obj) == DWG_TYPE)
    {
      output_process(obj);
    }
}
#endif

/// Main output function that prints to the terminal
void
output_process (dwg_object * obj)
{
  print_api (obj);
}


/// API based printing function
void
print_api (dwg_object * obj)
{
  api_process (obj);
}
