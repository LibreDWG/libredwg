#define DWG_TYPE DWG_TYPE_OLE2FRAME
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_ole2frame *ole2frame = dwg_object_to_OLE2FRAME(obj);

  printf("flags of ole2frame : %ud\n", ole2frame->flag);
  printf("mode of ole2frame : %ud\n", ole2frame->mode);
  printf("data length of ole2frame : " FORMAT_BL "\n", ole2frame->data_length); 
  printf("data of ole2frame : %s\n", ole2frame->data);
}

void
api_process(dwg_object *obj)
{
  int error;
  unsigned int flags, mode;
  BITCODE_BL data_length;
  char * data;
  dwg_ent_ole2frame *ole2frame = dwg_object_to_OLE2FRAME(obj);

  flags = dwg_ent_ole2frame_get_flag(ole2frame, &error);
  if ( !error )
    {  
      printf("flags of ole2frame : %ud\n", flags);
    }
  else
    {
      printf("error in reading flags  \n");
    }


  mode = dwg_ent_ole2frame_get_mode(ole2frame, &error);
  if ( !error )
    {
      printf("mode of ole2frame : %ud\n", mode);
    }
  else
    {
      printf("error in reading mode \n");
    }


  data_length = dwg_ent_ole2frame_get_data_length(ole2frame, &error);
  if ( !error )
    {  
      printf("flags of ole2frame : " FORMAT_BL "\n", data_length);
    }
  else
    {
      printf("error in data length \n");
    }


  data = dwg_ent_ole2frame_get_data(ole2frame, &error);
  if ( !error )
    {
      printf("data of ole2frame : %s\n", data);
    }
  else
    {
      printf("error in reading data \n");
    }


}
