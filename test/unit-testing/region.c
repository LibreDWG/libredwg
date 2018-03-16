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

  printf("acis empty of region :" FORMAT_BS "\n", region->acis_empty);
  printf("version of region :" FORMAT_BS "\n", region->version);
  printf("acis data of region : %s\n", region->acis_data);
  printf("wireframe data of region :" FORMAT_BS "\n", region->wireframe_data_present);
  printf("point present of region :" FORMAT_BS "\n", region->point_present);
  printf("point of region : x = %f, y = %f, z = %f\n", region->point.x,
          region->point.y, region->point.z);
  printf("num isolines of region :" FORMAT_BL "\n", region->num_isolines);
  printf("isoline present of region :" FORMAT_BS "\n", region->isoline_present);
  printf("num wires of region :" FORMAT_BL "\n", region->num_wires);
  for (i = 0; i < region->num_wires; i++)
    {
      printf("wire of region :" FORMAT_BL "\n", region->wires[i].selection_marker);
    }
  printf("num_silhouettes of region :" FORMAT_BL "\n", region->num_silhouettes);
  for (i = 0; i < region->num_silhouettes; i++)
    {
      printf("silhouette of region :" FORMAT_BL "\n", region->silhouettes[i].vp_id);
    }
}

void
api_process(dwg_object *obj)
{
  int error;
  BITCODE_BL i;
  BITCODE_BS version;
  BITCODE_BL block_size, num_isolines, num_wires, num_sil;
  char * acis_data;
  BITCODE_B wireframe_data_present, point_present, isoline_present; 
  BITCODE_B acis_empty, acis2_empty;
  dwg_point_3d point;
  dwg_ent_region *region = dwg_object_to_REGION(obj);
  dwg_ent_solid_wire *wire;
  dwg_ent_solid_silhouette *sil;

  // returns acis empty
  acis_empty = dwg_ent_region_get_acis_empty(region, &error);
  if ( !error )
    {
      printf("acis empty of region :" FORMAT_BS "\n", acis_empty);
    }
  else
    {
      printf("error in reading acis empty\n");
    }

  // returns version
  version = dwg_ent_region_get_version(region, &error);
  if ( !error )
    {
      printf("version of region :" FORMAT_BS "\n", version);
    }
  else
    {
      printf("error in reading version\n");
    }

  // returns acis data
  acis_data = dwg_ent_region_get_acis_data(region, &error); 
  if ( !error )
    {
      printf("acis data of region : %s\n", acis_data);
    }
  else
    {
      printf("error in reading acis data\n");
    }

  // returns wireframe data
  wireframe_data_present = dwg_ent_region_get_wireframe_data_present(region,
                           &error); 
  if ( !error )
    {
      printf("wireframe data of region :" FORMAT_BS "\n", wireframe_data_present);
    }
  else
    {
      printf("error in reading wireframe data present\n");
    }

  // returns point present
  point_present = dwg_ent_region_get_point_present(region, &error); 
  if ( !error )
    {
      printf("point present of region :" FORMAT_BS "\n", point_present);
    }
  else
    {
      printf("error in reading point present\n");
    } 

  // returns point values
  dwg_ent_region_get_point(region, &point, &error); 
  if ( !error )
    {
      printf("point of region : x = %f, y = %f, z = %f\n", point.x, point.y,
              point.z);
    }
  else
    {
      printf("error in reading point\n");
    } 

  // returns num isolines
  num_isolines = dwg_ent_region_get_num_isolines(region, &error);
  if ( !error )
    {
      printf("num isolines of region :" FORMAT_BL "\n", num_isolines);
    }
  else
    {
      printf("error in reading num isolines\n");
    }

  // returns isoline present
  isoline_present = dwg_ent_region_get_isoline_present(region, &error);
  if ( !error )
    {
      printf("isoline present of region :" FORMAT_BS "\n", isoline_present);
    }
  else
    {
      printf("error in reading isoline present\n");
    } 

  // returns num wires
  num_wires = dwg_ent_region_get_num_wires(region, &error);
  if ( !error )
    {
      printf("num wires of region :" FORMAT_BL "\n", num_wires);
    }
  else
    {
      printf("error in reading num wires\n");
    } 

  // returns wire
  wire = dwg_ent_region_get_wire(region, &error);
  if ( !error )
    {
      for (i = 0; i < num_wires; i++)
        printf("wire of region :" FORMAT_BL "\n", wire[i].selection_marker);
    }
  else
    {
      printf("error in reading num wires\n");
    } 

  // returns num sil houettes
  num_sil = dwg_ent_region_get_num_silhouettes(region, &error);
  if ( !error )
    {
      printf("num_silhouettes of region :" FORMAT_BL "\n", num_sil);
    }
  else
    {
      printf("error in reading num silhouette\n");
    } 

  // returns sil houettes
  sil = dwg_ent_region_get_silhouette(region, &error);
  if ( !error )
    {
      for (i = 0; i < num_sil; i++)
        printf("silhouette of region :" FORMAT_BL "\n", sil[i].vp_id);
    }
  else
    {
      printf("error in reading silhouette\n");
    } 
    
}
