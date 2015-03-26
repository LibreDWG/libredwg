#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <dwg.h>
#include <dwg_api.h>

/// This function Declaration reads DWG file
int test_code (char *filename);

/// Declaration of function to iterate over objects of a block
void output_BLOCK_HEADER (dwg_object_ref * ref);

/// Declaration for function that checks the dwg type and calls output_process
void output_object (dwg_object * obj);

/// Function declaration for blocks to be iterated over
void output_test (dwg_data * dwg);

/// API based processing function declaration
void api_process (dwg_object * obj);

/// API based printing function declaration
void print_api (dwg_object * obj);

/// Main function
int
main (int argc, char *argv[])
{
  return test_code ("example.dwg");
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
  unsigned long abs_ref;
  obj = dwg_obj_reference_get_object (ref, &error);
  abs_ref = dwg_obj_ref_get_abs_ref (ref, &error);
  if (!ref)
    {
      fprintf (stderr,
	       "Found null object reference. Could not output an SVG symbol for this BLOCK_HEADER\n");
      return;
    }
  if (!obj)
    {
      fprintf (stderr, "Found null ref->obj\n");
      return;
    }

  /* TODO: Review.  (This check avoids a segfault, but it is
     still unclear whether or not the condition is valid.)  */
  if (!dwg_object_to_object (obj, &error))
    {
      fprintf (stderr, "Found null ref->obj->tio.object\n");
      return;
    }

  hdr = dwg_object_to_BLOCK_HEADER (obj);

/*  printf(
      "\t<g id=\"symbol-%lu\" >\n\t\t<!-- %s -->\n", abs_ref, 
      dwg_obj_block_header_get_name(hdr, &error));
*/
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
  unsigned int i, num_hdr_objs, error;
  dwg_object *obj;
  dwg_obj_block_header *hdr;
  dwg_obj_block_control *ctrl;
  dwg_object_ref **hdr_refs;

  hdr = dwg_get_block_header (dwg, &error);
  ctrl = dwg_block_header_get_block_control (hdr, &error);

  hdr_refs = dwg_obj_block_control_get_block_headers (ctrl, &error);

  num_hdr_objs = dwg_obj_block_control_get_num_entries (ctrl, &error);

  output_BLOCK_HEADER (dwg_obj_block_control_get_model_space (ctrl, &error));
  output_BLOCK_HEADER (dwg_obj_block_control_get_paper_space (ctrl, &error));

}

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
