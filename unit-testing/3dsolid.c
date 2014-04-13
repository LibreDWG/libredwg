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

  if (dwg_get_type(obj)== DWG_TYPE_3DSOLID)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  // casts object to 3d solid entity
  dwg_ent_3dsolid *_3dsolid = obj->tio.entity->tio._3DSOLID;

  // prints acis empty value
  printf("acis empty of 3dsolid : %d", _3dsolid->acis_empty);

  // prints version value
  printf("version of 3dsolid : %d", _3dsolid->version);

  // prints acis data value
  printf("acis data of 3dsolid : %s", _3dsolid->acis_data);

  // prints wireframe data
  printf("wireframe data of 3dsolid : %d", _3dsolid->wireframe_data_present);

  // prints point present
  printf("point present of 3dsolid : %d", _3dsolid->point_present);

  // prints point of 3dsolid
  printf("point of 3dsolid : x = %f, y = %f, z = %f\t\n", _3dsolid->point.x,
          _3dsolid->point.y, _3dsolid->point.z);

  // prints num isolines
  printf("num isolines of 3dsolid : %ld", _3dsolid->num_isolines);

  // prints isoline present
  printf("isoline present of 3dsolid : %d", _3dsolid->isoline_present);

  // prints num wires 
  printf("num wires of 3dsolid : %ld", _3dsolid->num_wires);
  int i;
    for (i = 0; i < _3dsolid->num_wires; i++)
      {
         printf("wire of 3dsolid : %ld", _3dsolid->wires[i].selection_marker);
      }

  // prints number of siulhouettes
  printf("num sil of 3dsolid : %ld", _3dsolid->num_silhouettes);

  // returns silhouettes
   for (i = 0; i < _3dsolid->num_silhouettes; i++)
     {
       printf("silhouette of 3dsolid : %ld", _3dsolid->silhouettes[i].vp_id);
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
  dwg_ent_3dsolid *_3dsolid =  obj->tio.entity->tio._3DSOLID;
  dwg_ent_solid_wire *wire;
  dwg_ent_solid_silhouette *sil;

  // returns acis_empty value
  acis_empty = dwg_ent_3dsolid_get_acis_empty(_3dsolid, &acis_empty_error);
  if( acis_empty_error == 0 ) // error reporting
    {
      printf("acis empty of 3dsolid : %d", acis_empty);
    }
  else
    {
      printf("error in reading acis empty");
    }

  // Returns version value
  version = dwg_ent_3dsolid_get_version(_3dsolid, &version_error);
  if( version_error == 0 ) // error reporting
    {
      printf("version of 3dsolid : %d", version);
    }
  else
    {
      printf("error in reading version");
    }
 
  // returns acis data value
  acis_data = dwg_ent_3dsolid_get_acis_data(_3dsolid, &acis_data_error); 
  if( acis_data_error == 0 ) // error reporting
    {
      printf("acis data of 3dsolid : %s", acis_data);
    }
  else
    {
      printf("error in reading acis data");
    }

  // Returns wireframe_data_present value
  wireframe_data_present = dwg_ent_3dsolid_get_wireframe_data_present(_3dsolid,
                           &wireframe_data_present_error); 
  if( wireframe_data_present_error == 0 ) // error reporting
    {
      printf("wireframe data of 3dsolid : %d", wireframe_data_present);
    }
  else
    {
      printf("error in reading wireframe data present");
    }

  // returns point present value
  point_present = dwg_ent_3dsolid_get_point_present(_3dsolid, &point_present_error); 
  if( point_present_error == 0 ) // error reporting
    {
      printf("point present of 3dsolid : %d", point_present);
    }
  else
    {
      printf("error in reading point present");
    }

  // Returns point values 
  dwg_ent_3dsolid_get_point(_3dsolid, &point, &point_error); 
  if( point_error == 0 ) // error reporting
    {
      printf("point of 3dsolid : x = %f, y = %f, z = %f\t\n", point.x, point.y,
              point.z);
    }
  else
    {
      printf("error in reading point");
    } 

  // returns num_isolines
  num_isolines = dwg_ent_3dsolid_get_num_isolines(_3dsolid, &num_isolines_error);
  if( num_isolines_error == 0 )
    {
      printf("num isolines of 3dsolid : %ld", num_isolines);
    }
  else
    {
      printf("error in reading num isolines");
    } 

  // returns isolines_present value
  isoline_present = dwg_ent_3dsolid_get_isoline_present(_3dsolid, &isoline_present_error);
  if( isoline_present_error == 0 )
    {
      printf("isoline present of 3dsolid : %d", isoline_present);
    }
  else
    {
      printf("error in reading isoline present");
    } 

  // returns number of wires
  num_wires = dwg_ent_3dsolid_get_num_wires(_3dsolid, &num_wires_error);
  if( num_wires_error == 0 )
    {
      printf("num wires of 3dsolid : %ld", num_wires);
    }
  else
    {
      printf("error in reading num wires");
    } 
 
  // returns wire value
  wire = dwg_ent_3dsolid_get_wire(_3dsolid, &wire_error);
  if( wire_error == 0 ) // error checking
    {
      int i;
      for (i = 0; i < num_wires; i++)
        {
           printf("wire of 3dsolid : %ld", wire[i].selection_marker);
        }
    }
  else
    {
      printf("error in reading num wires");
    } 

  // returns number of silhpuettes
  num_sil = dwg_ent_3dsolid_get_num_silhouettes(_3dsolid, &num_sil_error);
  if( num_sil_error == 0 ) // error check
    {
      printf("num sil of 3dsolid : %ld", num_sil);
    }
  else
    {
      printf("error in reading num silhouette");
    }

  // returns silhouette value
  sil = dwg_ent_3dsolid_get_silhouette(_3dsolid, &sil_error);
  if( sil_error == 0 ) // error checking
    {
      int i;
      for (i = 0; i < num_sil; i++)
        {
           printf("silhouette of 3dsolid : %ld", sil[i].vp_id);
        }
    }
  else
    {
      printf("error in reading silhouette");
    } 
    
}
