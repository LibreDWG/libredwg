#include "common.c"

/// Output processing function
void
output_process(dwg_object *obj);

/// Checks the respective DWG entity/object type and then calls the output_process()
void
output_object(dwg_object* obj){
  if (!obj)
    {
      printf("object is NULL\n");
      return;
    }

  if (dwg_get_type(obj)== DWG_TYPE_MLINE)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  // casting object to mline entity
  dwg_ent_mline *mline = dwg_object_to_MLINE(obj);

  // prints mline scale
  printf("scale of mline : %f\t\n",mline->scale);

  // prints mline just
  printf("just of mline : %d\t\n",mline->just);

  // prints mline extrusion
  printf("extrusion of mline : x = %f, y = %f, z = %f\t\n", 
          mline->extrusion.x, mline->extrusion.y, mline->extrusion.z);

  // prints mline base_point
  printf("base_point of mline : x = %f,y = %f,z = %f\t\n",
          mline->base_point.x, mline->base_point.y, mline->base_point.z);

  // prints number of lines
  printf("number of lines : %d\t\n", mline->num_lines);

  // prints number of verts
  printf("number of verts : %ud\t\n", mline->num_verts);

  int i = 0;
  for (i; i < mline->num_verts; i++)
    {
      printf("vertex of mline : x = %f, y = %f, z = %f\t\n", 
              mline->verts[i].vertex.x, mline->verts[i].vertex.y,
              mline->verts[i].vertex.z); 
    }

}

void
api_process(dwg_object *obj)
{
  int scale_error, just_error, ext_error, base_point_error, num_lines_error,
      num_verts_error, num_lines, verts_error ; // Error reporting
  float scale;
  char just;
  unsigned int oc, num_verts;
  dwg_point_3d base_point, ext;  //3d_points 
  dwg_ent_mline_vert * verts;
  // casting object to mline entity
  dwg_ent_mline *mline = dwg_object_to_MLINE(obj);

  // returns mline scale
  scale = dwg_ent_mline_get_scale(mline, &scale_error);
  if(scale_error == 0 ) // Error checking
    {  
      printf("scale of mline : %f\t\n",scale);
    }
  else
    {
      printf("error in reading scale \n");
    }

  // returns mline just
  just = dwg_ent_mline_get_just(mline, &just_error);
  if(just_error == 0 ) // error checking
    {
      printf("just of mline : %d\t\n",just);
    }
  else
    {
      printf("error in reading just \n");
    }

  // returns mline extrusion
  dwg_ent_mline_get_extrusion(mline, &ext,&ext_error);
  if(ext_error == 0 ) // error checking
    {
      printf("extrusion of mline : x = %f, y = %f, z = %f\t\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  // return mline base_point points
  dwg_ent_mline_get_base_point(mline, &base_point,&base_point_error);
  if(base_point_error == 0 ) // error checking
    {
      printf("base_point of mline : x = %f, y = %f, z = %f\t\n",
              base_point.x, base_point.y, base_point.z);
    }
  else
    {
      printf("error in reading base_point \n");
    }

  // returns mline num lines
  num_lines = dwg_ent_mline_get_num_lines(mline, &num_lines_error);
  if( num_lines_error == 0 ) // error checking
    {
      printf("num lines of mline : %d\t\n", num_lines);
    }
  else
    {
      printf("error in reading num lines \n");
    }

  // returns mline num verts
  num_verts = dwg_ent_mline_get_num_verts(mline, &num_verts_error);
  if(num_verts_error == 0 ) // error checking
    {
      printf("num verts of mline : %ud\t\n", num_verts);
    }
  else
    {
      printf("error in reading num verts \n");
    }

  verts = dwg_ent_mline_get_verts(mline, &verts_error);
  int i = 0;
  if(verts_error == 0)
    {
      for (i; i < num_verts; i++)
        {
          printf("vertex of mline : x = %f, y = %f, z = %f\t\n", 
                 verts[i].vertex.x, verts[i].vertex.y, verts[i].vertex.z); 
        }
    }
  else
    {
      printf("error in reading verts of mline");
    }
}
