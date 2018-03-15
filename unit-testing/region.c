#include "common.c"

void
output_object(dwg_object* obj)
{
  if (!obj)
    {
      printf("object is NULL\n");
      return;
    }

  if (dwg_get_type(obj)== DWG_TYPE_REGION)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  unsigned long i;
  // casts dwg object to region entity
  dwg_ent_region *region = dwg_object_to_REGION(obj);

  printf("acis empty of region : %d\n", region->acis_empty);
  printf("version of region : %d\n", region->version);
  printf("acis data of region : %s\n", region->acis_data);
  printf("wireframe data of region : %d\n", region->wireframe_data_present);
  printf("point present of region : %d\n", region->point_present);
  printf("point of region : x = %f, y = %f, z = %f\n", region->point.x,
          region->point.y, region->point.z);
  printf("num isolines of region : %ld\n", region->num_isolines);
  printf("isoline present of region : %d\n", region->isoline_present);
  printf("num wires of region : %ld\n", region->num_wires);
  for (i = 0; i < region->num_wires; i++)
    {
      printf("wire of region : %ld\n", region->wires[i].selection_marker);
    }
  printf("num_silhouettes of region : %ld\n", region->num_silhouettes);
  for (i = 0; i < region->num_silhouettes; i++)
    {
      printf("silhouette of region : %ld\n", region->silhouettes[i].vp_id);
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
  dwg_ent_region *region = dwg_object_to_REGION(obj);
  dwg_ent_solid_wire *wire;
  dwg_ent_solid_silhouette *sil;

  // returns acis empty
  acis_empty = dwg_ent_region_get_acis_empty(region, &acis_empty_error);
  if( acis_empty_error == 0 ) // error check
    {
      printf("acis empty of region : %d\n", acis_empty);
    }
  else
    {
      printf("error in reading acis empty\n");
    }

  // returns version
  version = dwg_ent_region_get_version(region, &version_error);
  if( version_error == 0 ) // error check
    {
      printf("version of region : %d\n", version);
    }
  else
    {
      printf("error in reading version\n");
    }

  // returns acis data
  acis_data = dwg_ent_region_get_acis_data(region, &acis_data_error); 
  if( acis_data_error == 0 ) // error check
    {
      printf("acis data of region : %s\n", acis_data);
    }
  else
    {
      printf("error in reading acis data\n");
    }

  // returns wireframe data
  wireframe_data_present = dwg_ent_region_get_wireframe_data_present(region,
                           &wireframe_data_present_error); 
  if( wireframe_data_present_error == 0 ) // error check
    {
      printf("wireframe data of region : %d\n", wireframe_data_present);
    }
  else
    {
      printf("error in reading wireframe data present\n");
    }

  // returns point present
  point_present = dwg_ent_region_get_point_present(region, &point_present_error); 
  if( point_present_error == 0 ) // error check
    {
      printf("point present of region : %d\n", point_present);
    }
  else
    {
      printf("error in reading point present\n");
    } 

  // returns point values
  dwg_ent_region_get_point(region, &point, &point_error); 
  if( point_error == 0 ) // error check
    {
      printf("point of region : x = %f, y = %f, z = %f\t\n", point.x, point.y,
              point.z);
    }
  else
    {
      printf("error in reading point\n");
    } 

  // returns num isolines
  num_isolines = dwg_ent_region_get_num_isolines(region, &num_isolines_error);
  if( num_isolines_error == 0 ) // error check
    {
      printf("num isolines of region : %ld\n", num_isolines);
    }
  else
    {
      printf("error in reading num isolines\n");
    }

  // returns isoline present
  isoline_present = dwg_ent_region_get_isoline_present(region, &isoline_present_error);
  if( isoline_present_error == 0 ) // error check
    {
      printf("isoline present of region : %d\n", isoline_present);
    }
  else
    {
      printf("error in reading isoline present\n");
    } 

  // returns num wires
  num_wires = dwg_ent_region_get_num_wires(region, &num_wires_error);
  if( num_wires_error == 0 ) // error check
    {
      printf("num wires of region : %ld\n", num_wires);
    }
  else
    {
      printf("error in reading num wires\n");
    } 

  // returns wire
  wire = dwg_ent_region_get_wire(region, &wire_error);
  if( wire_error == 0 ) // error check
    {
      int i;
      for (i = 0; i < num_wires; i++)
        {
           printf("wire of region : %ld\n", wire[i].selection_marker);
        }
    }
  else
    {
      printf("error in reading num wires\n");
    } 

  // returns num sil houettes
  num_sil = dwg_ent_region_get_num_silhouettes(region, &num_sil_error);
  if( num_sil_error == 0 ) // error checking
    {
      printf("num_silhouettes of region : %ld\n", num_sil);
    }
  else
    {
      printf("error in reading num silhouette\n");
    } 

  // returns sil houettes
  sil = dwg_ent_region_get_silhouette(region, &sil_error);
  if( sil_error == 0 )
    {
      int i;
      for (i = 0; i < num_sil; i++)
        {
           printf("silhouette of region : %ld\n", sil[i].vp_id);
        }
    }
  else
    {
      printf("error in reading silhouette\n");
    } 
    
}
