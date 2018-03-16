#include "common.c"

/// Checks the respective DWG entity/object type and then calls the output_process()
void
output_object(dwg_object* obj)
{
  if (!obj)
    {
      printf("object is NULL\n");
      return;
    }

  if (dwg_get_type(obj)== DWG_TYPE_MTEXT)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  // casting object to mtext entity
  dwg_ent_mtext *mtext = dwg_object_to_MTEXT(obj);

  // prints extends ht
  printf("extends ht of mtext : %f\n", mtext->extends_ht);

  // prints extends wid
  printf("extends wid of mtext : %f\n", mtext->extends_wid);

  // prints drawing dir
  printf("drawing dir of mtext : %ud\n", mtext->drawing_dir);

  // prints linespace style
  printf("linespace style of mtext : %ud\n", mtext->linespace_style);

  // prints attachment
  printf("attachment of mtext : %ud\n", mtext->attachment);

  // prints text value
  printf("text of mtext : %s\n", mtext->text);
 
  // prints mtext rect_height
  printf("rect height of mtext : %f\n",mtext->rect_height);

  // prints mtext rect_width
  printf("rect width of mtext : %f\n",mtext->rect_width);

  // prints mtext linespace factor
  printf("linespace factor of mtext : %f\n", mtext->linespace_factor);

  // prints mtext extrusion
  printf("extrusion of mtext : x = %f, y = %f, z = %f\n", 
          mtext->extrusion.x, mtext->extrusion.y, mtext->extrusion.z);

  // prints mtext extrusion
  printf("insertion point of mtext : x = %f, y = %f, z = %f\n", 
          mtext->insertion_pt.x, mtext->insertion_pt.y,
          mtext->insertion_pt.z);

  // prints mtext x_axis_dir
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

  // casting object to mtext entity
  dwg_ent_mtext *mtext = dwg_object_to_MTEXT(obj);

  // returns mtext drawing dir
  extends_ht = dwg_ent_mtext_get_extends_ht(mtext, &error);
  if ( !error )
    {  
      printf("extends ht of mtext : %f\n", extends_ht);
    }
  else
    {
      printf("error in reading extends ht \n");
    }

  // returns mtext drawing dir
  extends_wid = dwg_ent_mtext_get_extends_wid(mtext, &error);
  if ( !error )
    {  
      printf("extends wid of mtext : %f\n", extends_wid);
    }
  else
    {
      printf("error in reading extends wid \n");
    }

  // returns mtext drawing dir
  drawing_dir = dwg_ent_mtext_get_drawing_dir(mtext, &error);
  if ( !error )
    {  
      printf("drawing dir of mtext : %ud\n", drawing_dir);
    }
  else
    {
      printf("error in reading drawing dir \n");
    }

  // returns mtext linespace_style
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

  // returns mtext attachment
  attachment = dwg_ent_mtext_get_attachment(mtext, &error);
  if ( !error )
    {  
      printf("attachment of mtext : %ud\n", attachment);
    }
  else
    {
      printf("error in reading attachment \n");
   }

  // returns mtext rect_height
  text = dwg_ent_mtext_get_text(mtext, &error);
  if ( !error )
    {  
      printf("text of mtext : %s\n", text);
    }
  else
    {
      printf("error in reading text \n");
    }

  // returns mtext rect_height
  rect_height = dwg_ent_mtext_get_rect_height(mtext, &error);
  if ( !error )
    {  
      printf("Radius of mtext : %f\n",rect_height);
    }
  else
    {
      printf("error in reading rect_height \n");
    }

  // returns mtext rect_width
  rect_width = dwg_ent_mtext_get_rect_width(mtext, &error);
  if ( !error )
    {
      printf("Thickness of mtext : %f\n",rect_width);
    }
  else
    {
      printf("error in reading rect_width \n");
    }

  // returns mtext linespace factor 
  linespace_factor = dwg_ent_mtext_get_linespace_factor(mtext,
                     &error);
  if ( !error )
    {
      printf("linespace factor of mtext : %f\n", linespace_factor);
    }
  else
    {
      printf("error in reading linespace factor \n");
    }

  // returns mtext extrusion
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
