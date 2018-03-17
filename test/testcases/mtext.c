#define DWG_TYPE DWG_TYPE_MTEXT
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  // casting object to mtext entity
  dwg_ent_mtext *mtext = dwg_object_to_MTEXT (obj);


  printf ("extends ht of mtext : %f\n", mtext->extends_ht);
  printf ("extends wid of mtext : %f\n", mtext->extends_wid);
  printf ("drawing dir of mtext : " FORMAT_BS "\n", mtext->drawing_dir);
  printf ("linespace style of mtext : " FORMAT_BS "\n", mtext->linespace_style);
  printf ("attachment of mtext : " FORMAT_BS "\n", mtext->attachment);
  printf ("text of mtext : %s\n", mtext->text);
  printf ("rect height of mtext : %f\n", mtext->rect_height);
  printf ("rect width of mtext : %f\n", mtext->rect_width);
  printf ("linespace factor of mtext : %f\n", mtext->linespace_factor);
  printf ("extrusion of mtext : x = %f, y = %f, z = %f\n",
	  mtext->extrusion.x, mtext->extrusion.y, mtext->extrusion.z);
  printf ("insertion point of mtext : x = %f, y = %f, z = %f\n",
	  mtext->insertion_pt.x, mtext->insertion_pt.y,
	  mtext->insertion_pt.z);
  printf ("x_axis_dir of mtext : x = %f, y = %f, z = %f\n",
	  mtext->x_axis_dir.x, mtext->x_axis_dir.y, mtext->x_axis_dir.z);
}

void
api_process (dwg_object * obj)
{
  int error;	// Error reporting
  BITCODE_BD rect_height, rect_width, text_height, extends_ht, extends_wid,
    linespace_factor;
  unsigned int attachment, drawing_dir, linespace_style;
  char *text;
  dwg_point_3d ins_pt, ext, x_axis_dir;	//3d_points 

  // casting object to mtext entity
  dwg_ent_mtext *mtext = dwg_object_to_MTEXT (obj);


  extends_ht = dwg_ent_mtext_get_extends_ht (mtext, &error);
  if (!error  && extends_ht == mtext->extends_ht)	// Error checking
    pass ("Working Properly");
  else
    fail ("error in reading extends ht");


  extends_wid = dwg_ent_mtext_get_extends_wid (mtext, &error);
  if (!error  && extends_wid == mtext->extends_wid)	// Error checking
    pass ("Working Properly");
  else
    fail ("error in reading extends wid");


  drawing_dir = dwg_ent_mtext_get_drawing_dir (mtext, &error);
  if (!error  && drawing_dir == mtext->drawing_dir)	// Error checking
    pass ("Working Properly");
  else
    fail ("error in reading drawing dir");


  linespace_style = dwg_ent_mtext_get_linespace_style (mtext,
						       &error);
  if (!error  && linespace_style == mtext->linespace_style)	// Error checking
    pass ("Working Properly");
  else
    fail ("error in reading linespace style");


  attachment = dwg_ent_mtext_get_attachment (mtext, &error);
  if (!error  && attachment == mtext->attachment)	// Error checking
    pass ("Working Properly");
  else
    fail ("error in reading attachment");


  text = dwg_ent_mtext_get_text (mtext, &error);
  if (!error  && !strcmp (text, mtext->text))	// Error checking
    pass ("Working Properly");
  else
    fail ("error in reading text");


  rect_height = dwg_ent_mtext_get_rect_height (mtext, &error);
  if (!error  && rect_height == mtext->rect_height)	// Error checking
    pass ("Working Properly");
  else
    fail ("error in reading rect_height");


  rect_width = dwg_ent_mtext_get_rect_width (mtext, &error);
  if (!error  && rect_width == mtext->rect_width)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading rect_width");


  linespace_factor = dwg_ent_mtext_get_linespace_factor (mtext,
							 &error);
  if (!error  && linespace_factor == mtext->linespace_factor)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading linespace factor");


  dwg_ent_mtext_get_extrusion (mtext, &ext, &error);
  if (!error  && ext.x == mtext->extrusion.x && ext.y == mtext->extrusion.y && ext.z == mtext->extrusion.z)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");

  dwg_ent_mtext_get_insertion_pt (mtext, &ins_pt, &error);
  if (!error  && ins_pt.x == mtext->insertion_pt.x && ins_pt.y == mtext->insertion_pt.y && ins_pt.z == mtext->insertion_pt.z)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading insertion point");

  // return mtext x_axis_dir points
  dwg_ent_mtext_get_x_axis_dir (mtext, &x_axis_dir, &error);
  if (!error  && x_axis_dir.x == mtext->x_axis_dir.x && x_axis_dir.y == mtext->x_axis_dir.y && x_axis_dir.z == mtext->x_axis_dir.z)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading x_axis_dir");

}
