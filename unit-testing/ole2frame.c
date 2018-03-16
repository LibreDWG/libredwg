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

  // casts dwg object to ole2frame entity
  dwg_ent_ole2frame *ole2frame = dwg_object_to_OLE2FRAME(obj);

  // prints flags
  printf("flags of ole2frame : %ud\n", ole2frame->flags);

  // prints mode
  printf("mode of ole2frame : %ud\n", ole2frame->mode);

  // prints data length
  printf("data length of ole2frame : " FORMAT_BL "\n", ole2frame->data_length); 

  // prints data
  printf("data of ole2frame : %s\n", ole2frame->data);
}

void
api_process(dwg_object *obj)
{
  int error;
  unsigned int flags, mode;
  long data_length;
  char * data;
  dwg_ent_ole2frame *ole2frame = dwg_object_to_OLE2FRAME(obj);

  // returns flags
  flags = dwg_ent_ole2frame_get_flags(ole2frame, &error);
  if ( !error )
    {  
      printf("flags of ole2frame : %ud\n", flags);
    }
  else
    {
      printf("error in reading flags  \n");
    }

  // returns mode
  mode = dwg_ent_ole2frame_get_mode(ole2frame, &error);
  if ( !error )
    {
      printf("mode of ole2frame : %ud\n", mode);
    }
  else
    {
      printf("error in reading mode \n");
    }

  // returns length of data
  data_length = dwg_ent_ole2frame_get_data_length(ole2frame, &error);
  if ( !error )
    {  
      printf("flags of ole2frame : " FORMAT_BL "\n", data_length);
    }
  else
    {
      printf("error in data length \n");
    }

  // returns data
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
