#define DWG_TYPE DWG_TYPE_OLE2FRAME
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  dwg_ent_ole2frame *ole2frame = dwg_object_to_OLE2FRAME (obj);

  printf ("flags of ole2frame : " FORMAT_BS "\n", ole2frame->flag);
  printf ("mode of ole2frame : " FORMAT_BS "\n", ole2frame->mode);
  printf ("data length of ole2frame : " FORMAT_BL "\n", ole2frame->data_length);
  printf ("data of ole2frame : %s\n", ole2frame->data);
}

void
api_process (dwg_object * obj)
{
  int error;
  BITCODE_BS flags, mode;
  BITCODE_BL data_length;
  char *data;

  dwg_ent_ole2frame *ole2frame = dwg_object_to_OLE2FRAME (obj);

  flags = dwg_ent_ole2frame_get_flag (ole2frame, &error);
  if (!error  && flags == ole2frame->flag)
    pass ("Working Properly");
  else
    fail ("error in reading flags");

  mode = dwg_ent_ole2frame_get_mode (ole2frame, &error);
  if (!error  && mode == ole2frame->mode)       // error check
    pass ("Working Properly");
  else
    fail ("error in reading mode");

  data_length = dwg_ent_ole2frame_get_data_length (ole2frame, &error);
  if (!error  && data_length == ole2frame->data_length) // error check
    pass ("Working Properly");
  else
    fail ("error in data length");

  data = dwg_ent_ole2frame_get_data (ole2frame, &error);
  if (!error  && ole2frame->data == data)       // error check
    pass ("Working Properly");
  else
    fail ("error in reading data");
}
