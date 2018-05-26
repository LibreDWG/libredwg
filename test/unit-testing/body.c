#define DWG_TYPE DWG_TYPE_BODY
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  BITCODE_BL i;
  dwg_ent_body *body = dwg_object_to_BODY(obj);

  printf("acis empty of body : " FORMAT_B "\n", body->acis_empty);
  printf("version of body : " FORMAT_BS "\n", body->version);
  printf("acis data of body : %s", body->acis_data);
  printf("wireframe data of body : " FORMAT_B "\n", body->wireframe_data_present);
  printf("point present of body : " FORMAT_B "\n", body->point_present);
  printf("point of body : x = %f, y = %f, z = %f\n", body->point.x,
          body->point.y, body->point.z);
  printf("num isolines of body : " FORMAT_BL "\n", body->num_isolines);
  printf("isoline present of body : " FORMAT_B "\n", body->isoline_present);
  printf("num wires of body : " FORMAT_BL "\n", body->num_wires);
  for (i = 0; i < body->num_wires; i++)
    printf("wire[%u] of body : " FORMAT_BL "\n", i, body->wires[i].selection_marker);

  printf("num sil of body : " FORMAT_BL "\n", body->num_silhouettes);
  for (i = 0; i < body->num_silhouettes; i++)
    printf("silhouette[%u] of body : " FORMAT_BL "\n", i, body->silhouettes[i].vp_id);

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
  dwg_ent_body *body = dwg_object_to_BODY(obj);
  dwg_ent_solid_wire *wire;
  dwg_ent_solid_silhouette *sil;


  acis_empty = dwg_ent_body_get_acis_empty(body, &error);
  if ( !error )
    printf("acis empty of body : " FORMAT_B "\n", acis_empty);
  else
    printf("error in reading acis empty");

  version = dwg_ent_body_get_version(body, &error);
  if ( !error )
      printf("version of body : " FORMAT_B "\n", version);
  else
      printf("error in reading version");

  acis_data = dwg_ent_body_get_acis_data(body, &error); 
  if ( !error )
      printf("acis data of body : %s", acis_data);
  else
      printf("error in reading acis data");

  wireframe_data_present = dwg_ent_body_get_wireframe_data_present(body,
                           &error); 
  if ( !error )
      printf("wireframe data of body : " FORMAT_B "\n", wireframe_data_present);
  else
      printf("error in reading wireframe data present");

  point_present = dwg_ent_body_get_point_present(body, &error); 
  if ( !error )
      printf("point present of body : " FORMAT_B "\n", point_present);
  else
      printf("error in reading point present");

  dwg_ent_body_get_point(body, &point, &error); 
  if ( !error )
      printf("point of body : x = %f, y = %f, z = %f\n", point.x, point.y,
              point.z);
  else
      printf("error in reading point");

  num_isolines = dwg_ent_body_get_num_isolines(body, &error);
  if ( !error )
      printf("num isolines of body : " FORMAT_BL "\n", num_isolines);
  else
      printf("error in reading num isolines");

  isoline_present = dwg_ent_body_get_isoline_present(body, &error);
  if ( !error )
      printf("isoline present of body : " FORMAT_B "\n", isoline_present);
  else
      printf("error in reading isoline present");

  num_wires = dwg_ent_body_get_num_wires(body, &error);
  if ( !error )
      printf("num wires of body : " FORMAT_BL "\n", num_wires);
  else
      printf("error in reading num wires");

  wire = dwg_ent_body_get_wires(body, &error);
  if ( !error )
    {
      for (i = 0; i < num_wires; i++)
        printf("wire[%u] of body : " FORMAT_BL "\n", i, wire[i].selection_marker);
    }
  else
    printf("error in reading num wires");

  num_sil = dwg_ent_body_get_num_silhouettes(body, &error);
  if ( !error )
      printf("num sil of body : " FORMAT_BL "\n", num_sil);
  else
      printf("error in reading num silhouettes");

  sil = dwg_ent_body_get_silhouettes(body, &error);
  if ( !error )
    {
      for (i = 0; i < num_sil; i++)
        printf("silhouette[%u] of body : " FORMAT_BL "\n", i, sil[i].vp_id);
    }
  else
      printf("error in reading silhouettes");
    
}
