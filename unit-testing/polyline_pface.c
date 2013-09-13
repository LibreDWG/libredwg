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

  if (dwg_get_type(obj)== DWG_TYPE_POLYLINE_PFACE)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{

  // casts dwg object to polyline pface
  dwg_ent_polyline_pface *polyline_pface = dwg_object_to_POLYLINE_PFACE(obj);

  // prints num verts polyline
  printf("num verts of polyline_pface : %ud\t\n",
          polyline_pface->numverts);

  // prints owned object count
  printf("owned obj count of polyline_pface : %ld\t\n",
          polyline_pface->owned_obj_count);

  // prints num faces
  printf("num faces of polyline_pface : %ud\t\n", polyline_pface->numfaces);
 
}

void
api_process(dwg_object *obj)
{
  int num_verts_error, owned_obj_error, num_faces_error;
  unsigned int num_verts, num_faces;
  long owned_obj_count;
  dwg_ent_polyline_pface *polyline_pface = dwg_object_to_POLYLINE_PFACE(obj);

  // returns num verts
  num_verts = dwg_ent_polyline_pface_get_numverts(polyline_pface, &num_verts_error);
  if(num_verts_error == 0 ) // error check
    {  
      printf("Radius of polyline_pface : %ud\t\n",num_verts);
    }
  else
    {
      printf("error in reading num_verts \n");
    }

  // returns owned object count
  owned_obj_count = dwg_ent_polyline_pface_get_owned_obj_count(polyline_pface,
                    &owned_obj_error);
  if(owned_obj_error == 0 ) // error check
    {
      printf("owned object count of polyline_pface : %ld\t\n", owned_obj_count);
    }
  else
    {
      printf("error in reading owned_obj_count \n");
    }

  // returns num faces
  num_faces = dwg_ent_polyline_pface_get_numfaces(polyline_pface,
              &num_faces_error);
  if(num_faces_error == 0 ) // error check
    {
      printf("num faces of polyline_pface : %ud\t\n", num_faces);
    }
  else
    {
      printf("error in reading num faces \n");
    }


}
