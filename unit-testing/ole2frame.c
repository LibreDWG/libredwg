#include "common.c"

void
output_process(dwg_object *obj);

void
output_object(dwg_object* obj){
  if (!obj)
    {
      printf("object is NULL\n");
      return;
    }

  if (dwg_get_type(obj)== DWG_TYPE_OLE2FRAME)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  dwg_ent_ole2frame *ole2frame = dwg_object_to_OLE2FRAME(obj);
  printf("flags of ole2frame : %ud\t\n", ole2frame->flags);
  printf("get mode of ole2frame : %ud\t\n", ole2frame->mode);
  printf("data length of ole2frame : %ld\t\n", ole2frame->data_length); 
  printf("data of ole2frame : %s\t\n", ole2frame->data);
}

void
api_process(dwg_object *obj)
{
  int flags_error, mode_error, data_length_error, data_error;
  unsigned int flags, mode;
  long data_length;
  char * data;
  dwg_ent_ole2frame *ole2frame = dwg_object_to_OLE2FRAME(obj);

  flags = dwg_ent_ole2frame_get_flags(ole2frame, &flags_error);
  if(flags_error == 0 )
    {  
      printf("flags of ole2frame : %ud\t\n", flags);
    }
  else
    {
      printf("error in reading flags  \n");
    }

  mode = dwg_ent_ole2frame_get_mode(ole2frame, &mode_error);
  if(mode_error == 0 )
    {
      printf("mode of ole2frame : %ud\t\n", mode);
    }
  else
    {
      printf("error in reading mode \n");
    }
  data_length = dwg_ent_ole2frame_get_data_length(ole2frame, &data_length_error);
  if(data_length_error == 0 )
    {  
      printf("flags of ole2frame : %ld\t\n", data_length);
    }
  else
    {
      printf("error in data length \n");
    }

  data = dwg_ent_ole2frame_get_data(ole2frame, &data_error);
  if(data_error == 0 )
    {
      printf("data of ole2frame : %s\t\n", data);
    }
  else
    {
      printf("error in reading data \n");
    }


}
