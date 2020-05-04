#define DWG_TYPE DWG_TYPE_VPORT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_RS flag;
  BITCODE_T name;
  BITCODE_RS used;
  BITCODE_B xrefref;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;
  BITCODE_BD VIEWSIZE;  // really the view height
  BITCODE_BD viewwidth; // in DWG r13+, needed to calc. aspect_ratio
  BITCODE_BD aspect_ratio; // DXF 41 = viewwidth / VIEWSIZE
  BITCODE_2RD VIEWCTR;
  BITCODE_3BD view_target;
  BITCODE_3BD VIEWDIR;
  BITCODE_BD view_twist;
  BITCODE_BD lens_length;
  BITCODE_BD front_clip;
  BITCODE_BD back_clip;
  BITCODE_4BITS VIEWMODE;
  BITCODE_RC render_mode;
  BITCODE_B use_default_lights;
  BITCODE_RC default_lightning_type;
  BITCODE_BD brightness;
  BITCODE_BD contrast;
  BITCODE_CMC ambient_color;
  BITCODE_2RD lower_left;
  BITCODE_2RD upper_right;
  BITCODE_B UCSFOLLOW;
  BITCODE_BS circle_zoom; /* circle sides: nr of tesselations */
  BITCODE_B FASTZOOM;
  BITCODE_RC UCSICON;
  BITCODE_B GRIDMODE;
  BITCODE_2RD GRIDUNIT;
  BITCODE_B SNAPMODE;
  BITCODE_B SNAPSTYLE;
  BITCODE_BS SNAPISOPAIR;
  BITCODE_BD SNAPANG;
  BITCODE_2RD SNAPBASE;
  BITCODE_2RD SNAPUNIT;
  BITCODE_B unknown;
  BITCODE_B UCSVP;
  BITCODE_3BD ucs_origin;
  BITCODE_3BD ucs_x_axis;
  BITCODE_3BD ucs_y_axis;
  BITCODE_BD ucs_elevation;
  BITCODE_BS ucs_orthografic_type;
  BITCODE_BS grid_flags;
  BITCODE_BS grid_major;
  BITCODE_H null_handle;
  BITCODE_H background;
  BITCODE_H visualstyle;
  BITCODE_H sun;
  BITCODE_H named_ucs;
  BITCODE_H base_ucs;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_vport *_obj = dwg_object_to_VPORT (obj);

  CHK_ENTITY_TYPE (_obj, VPORT, flag, RC, flag);
  CHK_ENTITY_UTF8TEXT (_obj, VPORT, name, name);
  CHK_ENTITY_TYPE (_obj, VPORT, used, RS, used);
  CHK_ENTITY_TYPE (_obj, VPORT, xrefref, B, xrefref);
  CHK_ENTITY_TYPE (_obj, VPORT, xrefindex_plus1, BS, xrefindex_plus1);
  CHK_ENTITY_TYPE (_obj, VPORT, xrefdep, B, xrefdep);

  CHK_ENTITY_TYPE (_obj, VPORT, VIEWSIZE, BD, VIEWSIZE);  // really the view height
  CHK_ENTITY_TYPE (_obj, VPORT, viewwidth, BD, viewwidth); // in DWG r13+, needed to calc. aspect_ratio
  CHK_ENTITY_TYPE (_obj, VPORT, aspect_ratio, BD, aspect_ratio); // DXF 41 = viewwidth / VIEWSIZE
  CHK_ENTITY_2RD (_obj, VPORT, VIEWCTR, VIEWCTR);
  CHK_ENTITY_3RD (_obj, VPORT, view_target, view_target);
  CHK_ENTITY_3RD (_obj, VPORT, VIEWDIR, VIEWDIR);
  CHK_ENTITY_TYPE (_obj, VPORT, view_twist, BD, view_twist);
  CHK_ENTITY_TYPE (_obj, VPORT, lens_length, BD, lens_length);
  CHK_ENTITY_TYPE (_obj, VPORT, front_clip, BD, front_clip);
  CHK_ENTITY_TYPE (_obj, VPORT, back_clip, BD, back_clip);
  CHK_ENTITY_TYPE (_obj, VPORT, VIEWMODE, 4BITS, VIEWMODE);
  CHK_ENTITY_TYPE (_obj, VPORT, render_mode, RC, render_mode);
  CHK_ENTITY_TYPE (_obj, VPORT, use_default_lights, B, use_default_lights);
  CHK_ENTITY_TYPE (_obj, VPORT, default_lightning_type, RC, default_lightning_type);
  CHK_ENTITY_TYPE (_obj, VPORT, brightness, BD, brightness);
  CHK_ENTITY_TYPE (_obj, VPORT, contrast, BD, contrast);
  CHK_ENTITY_CMC (_obj, VPORT, ambient_color, ambient_color);
  CHK_ENTITY_2RD (_obj, VPORT, lower_left, lower_left);
  CHK_ENTITY_2RD (_obj, VPORT, upper_right, upper_right);
  CHK_ENTITY_TYPE (_obj, VPORT, UCSFOLLOW, B, UCSFOLLOW);
  CHK_ENTITY_TYPE (_obj, VPORT, circle_zoom, BS, circle_zoom); /* circle sides: nr of tesselations */
  CHK_ENTITY_TYPE (_obj, VPORT, FASTZOOM, B, FASTZOOM);
  CHK_ENTITY_TYPE (_obj, VPORT, UCSICON, RC, UCSICON);
  CHK_ENTITY_TYPE (_obj, VPORT, GRIDMODE, B, GRIDMODE);
  CHK_ENTITY_2RD (_obj, VPORT, GRIDUNIT, GRIDUNIT);
  CHK_ENTITY_TYPE (_obj, VPORT, SNAPMODE, B, SNAPMODE);
  CHK_ENTITY_TYPE (_obj, VPORT, SNAPSTYLE, B, SNAPSTYLE);
  CHK_ENTITY_TYPE (_obj, VPORT, SNAPISOPAIR, BS, SNAPISOPAIR);
  CHK_ENTITY_TYPE (_obj, VPORT, SNAPANG, BD, SNAPANG);
  CHK_ENTITY_2RD (_obj, VPORT, SNAPBASE, SNAPBASE);
  CHK_ENTITY_2RD (_obj, VPORT, SNAPUNIT, SNAPUNIT);
  CHK_ENTITY_TYPE (_obj, VPORT, unknown, B, unknown);
  CHK_ENTITY_TYPE (_obj, VPORT, UCSVP, B, UCSVP);
  CHK_ENTITY_3RD (_obj, VPORT, ucs_origin, ucs_origin);
  CHK_ENTITY_3RD (_obj, VPORT, ucs_x_axis, ucs_x_axis);
  CHK_ENTITY_3RD (_obj, VPORT, ucs_y_axis, ucs_y_axis);
  CHK_ENTITY_TYPE (_obj, VPORT, ucs_elevation, BD, ucs_elevation);
  CHK_ENTITY_TYPE (_obj, VPORT, ucs_orthografic_type, BS, ucs_orthografic_type);
  CHK_ENTITY_TYPE (_obj, VPORT, grid_flags, BS, grid_flags);
  CHK_ENTITY_TYPE (_obj, VPORT, grid_major, BS, grid_major);
  CHK_ENTITY_H (_obj, VPORT, null_handle, null_handle);
  CHK_ENTITY_H (_obj, VPORT, background, background);
  CHK_ENTITY_H (_obj, VPORT, visualstyle, visualstyle);
  CHK_ENTITY_H (_obj, VPORT, sun, sun);
  CHK_ENTITY_H (_obj, VPORT, named_ucs, named_ucs);
  CHK_ENTITY_H (_obj, VPORT, base_ucs, base_ucs);
}
