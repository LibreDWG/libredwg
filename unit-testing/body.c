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

  if (dwg_get_type(obj)== DWG_TYPE_BODY)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  dwg_ent_body *body = dwg_object_to_BODY(obj);
}

void
api_process(dwg_object *obj)
{
  int block_size_error, num_isolines_error, num_wires_error, acis_data_error,
      isoline_present_error, point_present_error, wireframe_data_present_error;
  unsigned int version;
  long block_size, num_isolines, num_wires;
  char * acis_data;
  char wireframe_data_present, point_present, isoline_present; 
  unsigned char acis_empty;
  dwg_point_3d point;
  dwg_ent_body *body = dwg_object_to_BODY(obj);

  block_size = dwg_ent_body_get_block_size(body, &block_size_error);
  if(block_size_error == 0 )
    {  
      printf("block size of body : %ld\t\n", block_size);
    }
  else
    {
      printf("error in reading block size \n");
    }

  num_isolines = dwg_ent_body_get_num_isolines(body, &num_isolines_error);
  if(num_isolines_error == 0 )
    {  
      printf("num isolines of body : %ld\t\n", num_isolines);
    }
  else
    {
      printf("error in reading num isolines \n");
    }

  num_wires = dwg_ent_body_get_num_wires(body, &num_wires_error);
  if(num_wires_error == 0 )
    {  
      printf("num wires of body : %ld\t\n", num_wires);
    }
  else
    {
      printf("error in reading num wires \n");
    }

  acis_data = dwg_ent_body_get_acis_data(body, &acis_data_error);
  if(acis_data_error == 0 )
    {  
      printf("acis data of body : %s\t\n", acis_data);
    }
  else
    {
      printf("error in reading acis data \n");
    }

  isoline_present = dwg_ent_body_get_isoline_present(body,
                     &isoline_present_error);
  if(isoline_present_error == 0 )
    {  
      printf("isolines present of body : %d\t\n", isoline_present);
    }
  else
    {
      printf("error in reading isolines present\n");
    }



}
