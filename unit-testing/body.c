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
      printf("acis empty of body : %d", body->acis_empty);

      printf("version of body : %d", body->version);
     printf("acis data of body : %s", body->acis_data);
     printf("wireframe data of body : %d", body->wireframe_data_present);
     printf("point present of body : %d", body->point_present);
     printf("point of body : x = %f, y = %f, z = %f\t\n", body->point.x,
             body->point.y, body->point.z);
     printf("num isolines of body : %ld", body->num_isolines);
     printf("isoline present of body : %d", body->isoline_present);
     printf("num wires of body : %ld", body->num_wires);
     int i;
      for (i = 0; i < body->num_wires; i++)
        {
           printf("wire of body : %ld", body->wires[i].selection_marker);
        }
     printf("num sil of body : %ld", body->num_silhouettes);
     for (i = 0; i < body->num_silhouettes; i++)
        {
           printf("silhouette of body : %ld", body->silhouettes[i].vp_id);
        }

}
void
api_process(dwg_object *obj)
{
  int block_size_error, num_isolines_error, num_wires_error, acis_data_error,
      isoline_present_error, point_present_error, wireframe_data_present_error,
      version_error, acis_empty_error, point_error, acis2_empty_error, sil_error,
      wire_error, num_sil_error;

  unsigned int version;
  long block_size, num_isolines, num_wires, num_sil;
  char * acis_data;
  char wireframe_data_present, point_present, isoline_present; 
  unsigned char acis_empty, acis2_empty;
  dwg_point_3d point;
  dwg_ent_body *body = dwg_object_to_BODY(obj);
  dwg_ent_solid_wire *wire;
  dwg_ent_solid_silhouette *sil;

  acis_empty = dwg_ent_body_get_acis_empty(body, &acis_empty_error);
  if( acis_empty_error == 0 )
    {
      printf("acis empty of body : %d", acis_empty);
    }
  else
    {
      printf("error in reading acis empty");
    }

  version = dwg_ent_body_get_version(body, &version_error);
  if( version_error == 0 )
    {
      printf("version of body : %d", version);
    }
  else
    {
      printf("error in reading version");
    }

  acis_data = dwg_ent_body_get_acis_data(body, &acis_data_error); 
  if( acis_data_error == 0 )
    {
      printf("acis data of body : %s", acis_data);
    }
  else
    {
      printf("error in reading acis data");
    }

  wireframe_data_present = dwg_ent_body_get_wireframe_data_present(body,
                           &wireframe_data_present_error); 
  if( wireframe_data_present_error == 0 )
    {
      printf("wireframe data of body : %d", wireframe_data_present);
    }
  else
    {
      printf("error in reading wireframe data present");
    }

  point_present = dwg_ent_body_get_point_present(body, &point_present_error); 
  if( point_present_error == 0 )
    {
      printf("point present of body : %d", point_present);
    }
  else
    {
      printf("error in reading point present");
    } 
  dwg_ent_body_get_point(body, &point, &point_error); 
  if( point_error == 0 )
    {
      printf("point of body : x = %f, y = %f, z = %f\t\n", point.x, point.y,
              point.z);
    }
  else
    {
      printf("error in reading point");
    } 

  num_isolines = dwg_ent_body_get_num_isolines(body, &num_isolines_error);
  if( num_isolines_error == 0 )
    {
      printf("num isolines of body : %ld", num_isolines);
    }
  else
    {
      printf("error in reading num isolines");
    } 
  isoline_present = dwg_ent_body_get_isoline_present(body, &isoline_present_error);
  if( isoline_present_error == 0 )
    {
      printf("isoline present of body : %d", isoline_present);
    }
  else
    {
      printf("error in reading isoline present");
    } 

  num_wires = dwg_ent_body_get_num_wires(body, &num_wires_error);
  if( num_wires_error == 0 )
    {
      printf("num wires of body : %ld", num_wires);
    }
  else
    {
      printf("error in reading num wires");
    } 

  wire = dwg_ent_body_get_wire(body, &wire_error);
  if( wire_error == 0 )
    {
      int i;
      for (i = 0; i < num_wires; i++)
        {
           printf("wire of body : %ld", wire[i].selection_marker);
        }
    }
  else
    {
      printf("error in reading num wires");
    } 

  num_sil = dwg_ent_body_get_num_silhouettes(body, &num_sil_error);
  if( num_sil_error == 0 )
    {
      printf("num sil of body : %ld", num_sil);
    }
  else
    {
      printf("error in reading num silhouette");
    } 
  sil = dwg_ent_body_get_silhouette(body, &sil_error);
  if( sil_error == 0 )
    {
      int i;
      for (i = 0; i < num_sil; i++)
        {
           printf("silhouette of body : %ld", sil[i].vp_id);
        }
    }
  else
    {
      printf("error in reading silhouette");
    } 
    
}
