#define DWG_TYPE DWG_TYPE_MTEXT
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_mtext *mtext = dwg_object_to_MTEXT(obj);

  printf("extends ht of mtext : %f\n", mtext->extents_height);
  printf("extends wid of mtext : %f\n", mtext->extents_width);
  printf("drawing dir of mtext : %ud\n", mtext->drawing_dir);
  printf("linespace style of mtext : %ud\n", mtext->linespace_style);
  printf("attachment of mtext : %ud\n", mtext->attachment);
  printf("text of mtext : %s\n", mtext->text);
  printf("rect height of mtext : %f\n",mtext->rect_height);
  printf("rect width of mtext : %f\n",mtext->rect_width);
  printf("linespace factor of mtext : %f\n", mtext->linespace_factor);
  printf("extrusion of mtext : x = %f, y = %f, z = %f\n",
          mtext->extrusion.x, mtext->extrusion.y, mtext->extrusion.z);
  printf("insertion point of mtext : x = %f, y = %f, z = %f\n",
          mtext->insertion_pt.x, mtext->insertion_pt.y,
          mtext->insertion_pt.z);
  printf("x_axis_dir of mtext : x = %f,y = %f,z = %f\n",
          mtext->x_axis_dir.x, mtext->x_axis_dir.y, mtext->x_axis_dir.z);
}

void
api_process(dwg_object *obj)
{
  int error;
  double rect_height, rect_width, text_height, extends_ht, extends_wid,
        linespace_factor;
  unsigned int attachment, drawing_dir, linespace_style;
  char * text;
  dwg_point_3d ins_pt, ext, x_axis_dir;  //3d_points

  dwg_ent_mtext *mtext = dwg_object_to_MTEXT(obj);

  extends_ht = dwg_ent_mtext_get_extents_height(mtext, &error);
  if ( !error )
    printf("extends ht of mtext : %f\n", extends_ht);
  else
    printf("in reading extends ht \n");


  extends_wid = dwg_ent_mtext_get_extents_width(mtext, &error);
  if ( !error )
    printf("extends wid of mtext : %f\n", extends_wid);
  else
    printf("in reading extends wid \n");


  drawing_dir = dwg_ent_mtext_get_drawing_dir(mtext, &error);
  if ( !error )
    printf("drawing dir of mtext : %ud\n", drawing_dir);
  else
    printf("in reading drawing dir \n");


  linespace_style = dwg_ent_mtext_get_linespace_style(mtext,
                    &error);
  if ( !error )
    {
      printf("linespace style of mtext : %ud\n", linespace_style);
    }
  else
    {
      printf("error in reading linespace style \n");
   }


  attachment = dwg_ent_mtext_get_attachment(mtext, &error);
  if ( !error )
    {
      printf("attachment of mtext : %ud\n", attachment);
    }
  else
    {
      printf("error in reading attachment \n");
   }


  text = dwg_ent_mtext_get_text(mtext, &error);
  if ( !error )
    printf("text of mtext : %s\n", text);
  else
    printf("in reading text \n");


  rect_height = dwg_ent_mtext_get_rect_height(mtext, &error);
  if ( !error )
    printf("Radius of mtext : %f\n",rect_height);
  else
    printf("in reading rect_height \n");


  rect_width = dwg_ent_mtext_get_rect_width(mtext, &error);
  if ( !error )
    printf("Thickness of mtext : %f\n",rect_width);
  else
    printf("in reading rect_width \n");


  linespace_factor = dwg_ent_mtext_get_linespace_factor(mtext,
                     &error);
  if ( !error )
    printf("linespace factor of mtext : %f\n", linespace_factor);
  else
    printf("in reading linespace factor \n");


  dwg_ent_mtext_get_extrusion(mtext, &ext,
                              &error);
  if ( !error )
    {
      printf("extrusion of mtext : x = %f, y = %f, z = %f\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  dwg_ent_mtext_get_insertion_pt(mtext, &ins_pt,
                                 &error);
  if ( !error )
    {
      printf("insertion point of mtext : x = %f, y = %f, z = %f\n",
              ins_pt.x, ins_pt.y, ins_pt.z);
    }
  else
    {
      printf("error in reading insertion point \n");
    }

  // return mtext x_axis_dir points
  dwg_ent_mtext_get_x_axis_dir(mtext, &x_axis_dir,
                               &error);
  if ( !error )
    {
      printf("x_axis_dir of mtext : x = %f, y = %f, z = %f\n",
              x_axis_dir.x, x_axis_dir.y, x_axis_dir.z);
    }
  else
    {
      printf("error in reading x_axis_dir \n");
    }

}
