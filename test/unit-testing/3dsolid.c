#define DWG_TYPE DWG_TYPE_3DSOLID
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  BITCODE_BL i;
  // casts object to 3d solid entity
  dwg_ent_3dsolid *_3dsolid = obj->tio.entity->tio._3DSOLID;

  // prints acis empty value
  printf("acis empty of 3dsolid : " FORMAT_B "\n", _3dsolid->acis_empty);

  // prints version value
  printf("version of 3dsolid : " FORMAT_BS "\n", _3dsolid->version);

  // prints acis data value
  printf("acis data of 3dsolid : %s\n", _3dsolid->acis_data);

  // prints wireframe data
  printf("wireframe data of 3dsolid : " FORMAT_B "\n", _3dsolid->wireframe_data_present);

  // prints point present
  printf("point present of 3dsolid : " FORMAT_B "\n", _3dsolid->point_present);

  // prints point of 3dsolid
  printf("point of 3dsolid : x = %f, y = %f, z = %f\n", _3dsolid->point.x,
          _3dsolid->point.y, _3dsolid->point.z);

  // prints num isolines
  printf("num isolines of 3dsolid : " FORMAT_BL "\n", _3dsolid->num_isolines);

  // prints isoline present
  printf("isoline present of 3dsolid : " FORMAT_B "\n", _3dsolid->isoline_present);

  // prints num wires
  printf("num wires of 3dsolid : " FORMAT_BL "\n", _3dsolid->num_wires);
  for (i = 0; i < _3dsolid->num_wires; i++)
    {
      printf("wire of 3dsolid : " FORMAT_BL "\n", _3dsolid->wires[i].selection_marker);
    }

  // prints number of siulhouettes
  printf("num sil of 3dsolid : " FORMAT_BL "\n", _3dsolid->num_silhouettes);

  // returns silhouettes
   for (i = 0; i < _3dsolid->num_silhouettes; i++)
     {
       printf("silhouette of 3dsolid : " FORMAT_BL "\n", _3dsolid->silhouettes[i].vp_id);
     }

}
void
api_process(dwg_object *obj)
{
  BITCODE_BL i;
  int error;
  BITCODE_BS version;
  BITCODE_BL block_size, num_isolines, num_wires, num_sil;
  char * acis_data;
  BITCODE_B wireframe_data_present, point_present, isoline_present; 
  BITCODE_B acis_empty, acis2_empty;
  dwg_point_3d point;
  dwg_ent_solid_wire *wire;
  dwg_ent_solid_silhouette *sil;

  dwg_ent_3dsolid *_3dsolid =  obj->tio.entity->tio._3DSOLID;

  acis_empty = dwg_ent_3dsolid_get_acis_empty(_3dsolid, &error);
  if ( !error )
      printf("acis empty of 3dsolid : " FORMAT_B "\n", acis_empty);
  else
      printf("error in reading acis empty");

  // Returns version value
  version = dwg_ent_3dsolid_get_version(_3dsolid, &error);
  if ( !error )
      printf("version of 3dsolid : " FORMAT_BS "\n", version);
  else
      printf("error in reading version");
 
  // returns acis data value
  acis_data = dwg_ent_3dsolid_get_acis_data(_3dsolid, &error); 
  if ( !error )
      printf("acis data of 3dsolid : %s", acis_data);
  else
      printf("error in reading acis data");

  // Returns wireframe_data_present value
  wireframe_data_present = dwg_ent_3dsolid_get_wireframe_data_present(_3dsolid,
                           &error); 
  if ( !error )
      printf("wireframe data of 3dsolid : " FORMAT_B "\n", wireframe_data_present);
  else
      printf("error in reading wireframe data present");

  // returns point present value
  point_present = dwg_ent_3dsolid_get_point_present(_3dsolid, &error); 
  if ( !error )
      printf("point present of 3dsolid : " FORMAT_B "\n", point_present);
  else
      printf("error in reading point present");

  // Returns point values 
  dwg_ent_3dsolid_get_point(_3dsolid, &point, &error); 
  if ( !error )
      printf("point of 3dsolid : x = %f, y = %f, z = %f\n", point.x, point.y,
              point.z);
  else
      printf("error in reading point");

  // returns num_isolines
  num_isolines = dwg_ent_3dsolid_get_num_isolines(_3dsolid, &error);
  if ( !error )
      printf("num isolines of 3dsolid : " FORMAT_BL "\n", num_isolines);
  else
      printf("error in reading num isolines");

  // returns isolines_present value
  isoline_present = dwg_ent_3dsolid_get_isoline_present(_3dsolid, &error);
  if ( !error )
      printf("isoline present of 3dsolid : " FORMAT_B "\n", isoline_present);
  else
      printf("error in reading isoline present");

  // returns number of wires
  num_wires = dwg_ent_3dsolid_get_num_wires(_3dsolid, &error);
  if ( !error )
      printf("num wires of 3dsolid : " FORMAT_BL "\n", num_wires);
  else
      printf("error in reading num wires");
 
  // returns wire value
  wire = dwg_ent_3dsolid_get_wire(_3dsolid, &error);
  if ( !error ) // error checking
    {
      for (i = 0; i < num_wires; i++)
          printf("wire[%u] of 3dsolid : " FORMAT_BL "\n", i, wire[i].selection_marker);
    }
  else
      printf("error in reading num wires");

  // returns number of silhpuettes
  num_sil = dwg_ent_3dsolid_get_num_silhouettes(_3dsolid, &error);
  if ( !error )
      printf("num sil of 3dsolid : " FORMAT_BL "\n", num_sil);
  else
      printf("error in reading num silhouette");

  // returns silhouette value
  sil = dwg_ent_3dsolid_get_silhouette(_3dsolid, &error);
  if ( !error )
    {
      for (i = 0; i < num_sil; i++)
        printf("silhouette[%u] of 3dsolid : " FORMAT_BL "\n", i, sil[i].vp_id);
    }
  else
      printf("error in reading silhouette");
    
}
