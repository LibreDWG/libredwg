#define DWG_TYPE DWG_TYPE_MLINE
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  unsigned int i;

  dwg_ent_mline *mline = dwg_object_to_MLINE(obj);

  printf("scale of mline : %f\n",mline->scale);
  printf("just of mline : " FORMAT_BS "\n",mline->just);
  printf("extrusion of mline : x = %f, y = %f, z = %f\n", 
          mline->extrusion.x, mline->extrusion.y, mline->extrusion.z);
  printf("base_point of mline : x = %f,y = %f,z = %f\n",
          mline->base_point.x, mline->base_point.y, mline->base_point.z);
  printf("number of lines : " FORMAT_BS "\n", mline->num_lines);
  printf("number of verts : %ud\n", mline->num_verts);

  for (i=0; i < mline->num_verts; i++)
    {
      printf("vertex of mline : x = %f, y = %f, z = %f\n", 
              mline->verts[i].vertex.x, mline->verts[i].vertex.y,
              mline->verts[i].vertex.z); 
    }

}

void
api_process(dwg_object *obj)
{
  int error;
  double scale;
  char just;
  unsigned int i, oc, num_verts, num_lines;
  dwg_point_3d base_point, ext;  //3d_points 
  dwg_ent_mline_vert * verts;
  // casting object to mline entity
  dwg_ent_mline *mline = dwg_object_to_MLINE(obj);


  scale = dwg_ent_mline_get_scale(mline, &error);
  if (!error)
    {  
      printf("scale of mline : %f\n",scale);
    }
  else
    {
      printf("error in reading scale \n");
    }


  just = dwg_ent_mline_get_just(mline, &error);
  if (!error)
    {
      printf("just of mline : " FORMAT_BS "\n",just);
    }
  else
    {
      printf("error in reading just \n");
    }


  dwg_ent_mline_get_extrusion(mline, &ext, &error);
  if (!error)
    {
      printf("extrusion of mline : x = %f, y = %f, z = %f\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  // return mline base_point points
  dwg_ent_mline_get_base_point(mline, &base_point, &error);
  if (!error)
    {
      printf("base_point of mline : x = %f, y = %f, z = %f\n",
              base_point.x, base_point.y, base_point.z);
    }
  else
    {
      printf("error in reading base_point \n");
    }


  num_lines = dwg_ent_mline_get_num_lines(mline, &error);
  if (!error)
    {
      printf("num lines of mline : " FORMAT_BS "\n", num_lines);
    }
  else
    {
      printf("error in reading num lines \n");
    }


  num_verts = dwg_ent_mline_get_num_verts(mline, &error);
  if (!error)
    {
      printf("num verts of mline : %ud\n", num_verts);
    }
  else
    {
      printf("error in reading num verts \n");
    }

  verts = dwg_ent_mline_get_verts(mline, &error);
  if (!error)
    {
      for (i=0; i < num_verts; i++)
        {
          printf("vertex of mline : x = %f, y = %f, z = %f\n", 
                 verts[i].vertex.x, verts[i].vertex.y, verts[i].vertex.z); 
        }
    }
  else
    {
      printf("error in reading verts of mline");
    }
}
