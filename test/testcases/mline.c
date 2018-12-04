#define DWG_TYPE DWG_TYPE_MLINE
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  BITCODE_BS i;

  dwg_ent_mline *mline = dwg_object_to_MLINE (obj);

  printf ("scale of mline : %f\n", mline->scale);
  printf ("just of mline : " FORMAT_RC "\n", mline->justification);
  printf ("extrusion of mline : x = %f, y = %f, z = %f\n",
          mline->extrusion.x, mline->extrusion.y, mline->extrusion.z);
  printf ("base_point of mline : x = %f, y = %f, z = %f\n",
          mline->base_point.x, mline->base_point.y, mline->base_point.z);
  printf ("number of lines : " FORMAT_RC "\n", mline->num_lines);
  printf ("number of verts : " FORMAT_BS "\n", mline->num_verts);

  for (i = 0; i < mline->num_verts; i++)
    printf ("vertex of mline : x = %f, y = %f, z = %f\n",
            mline->verts[i].vertex.x, mline->verts[i].vertex.y,
            mline->verts[i].vertex.z);
}

void
api_process (dwg_object * obj)
{
  int error;
  BITCODE_BD scale;
  char just;
  BITCODE_BS oc, num_verts, num_lines;
  dwg_point_3d base_point, ext;
  dwg_mline_vertex *verts;

  dwg_ent_mline *mline = dwg_object_to_MLINE (obj);

  scale = dwg_ent_mline_get_scale (mline, &error);
  if (!error  && scale == mline->scale) // Error checking
    pass ("Working Properly");
  else
    fail ("error in reading scale");


  just = dwg_ent_mline_get_justification (mline, &error);
  if (!error  && just == mline->justification)
    pass ("Working Properly");
  else
    fail ("error in reading just");


  dwg_ent_mline_get_extrusion (mline, &ext, &error);
  if (!error  && ext.x == mline->extrusion.x && ext.y == mline->extrusion.y && ext.z == mline->extrusion.z)
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");

  // return mline base_point points
  dwg_ent_mline_get_base_point (mline, &base_point, &error);
  if (!error  && mline->base_point.x == base_point.x && mline->base_point.y == base_point.y && mline->base_point.z == base_point.z)     // error checking
    pass ("Working Properly");
  else
    fail ("error in reading base_point");

  num_lines = dwg_ent_mline_get_num_lines (mline, &error);
  if (!error  && num_lines == mline->num_lines) // error checking
    pass ("Working Properly");
  else
    fail ("error in reading num lines");

  num_verts = dwg_ent_mline_get_num_verts (mline, &error);
  if (!error  && num_verts == mline->num_verts) // error checking
    pass ("Working Properly");
  else
    fail ("error in reading num verts");

  verts = dwg_ent_mline_get_verts (mline, &error);
  if (!error )
    {
      BITCODE_BS i, matches = 1;
      for (i=0; i < num_verts; i++)
        {
          if (mline->verts[i].vertex.x != verts[i].vertex.x
              || mline->verts[i].vertex.y != verts[i].vertex.y
              || mline->verts[i].vertex.z != verts[i].vertex.z)
            {
              matches = 0;
            }
        }
      if (matches)
        {
          pass ("Working Properly");
        }
      else
        {
          fail ("error in reading verts of mline");
        }
    }
  else
    {
      fail ("error in reading verts of mline");
    }
}
