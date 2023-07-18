#define DWG_TYPE DWG_TYPE_VPORT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_RC flag;
  BITCODE_T name;
  BITCODE_RSd used;
  BITCODE_B is_xref_ref;
  BITCODE_BS is_xref_resolved;
  BITCODE_B is_xref_dep;
  BITCODE_H xref;

  BITCODE_BD VIEWSIZE;     // really the view height
  BITCODE_BD view_width;   // in DWG r13+, needed to calc. aspect_ratio
  BITCODE_BD aspect_ratio; // DXF 41 = view_width / VIEWSIZE
  BITCODE_2RD VIEWCTR;
  BITCODE_3BD view_target;
  BITCODE_3BD VIEWDIR;
  BITCODE_BD view_twist;
  BITCODE_BD lens_length;
  BITCODE_BD front_clip_z;
  BITCODE_BD back_clip_z;
  BITCODE_4BITS VIEWMODE;
  BITCODE_RC render_mode;
  BITCODE_B use_default_lights;
  BITCODE_RC default_lightning_type;
  BITCODE_BD brightness;
  BITCODE_BD contrast;
  BITCODE_CMC ambient_color;
  BITCODE_H background;
  BITCODE_H visualstyle;
  BITCODE_H sun;

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
  BITCODE_B ucs_at_origin;
  BITCODE_B UCSVP;
  BITCODE_3BD ucsorg;
  BITCODE_3BD ucsxdir;
  BITCODE_3BD ucsydir;
  BITCODE_BD ucs_elevation;
  BITCODE_BS UCSORTHOVIEW;
  BITCODE_BS grid_flags;
  BITCODE_BS grid_major;
  BITCODE_H named_ucs;
  BITCODE_H base_ucs;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_vport *_obj = dwg_object_to_VPORT (obj);

  CHK_ENTITY_TYPE (_obj, VPORT, flag, RC);
  CHK_ENTITY_UTF8TEXT (_obj, VPORT, name);
  CHK_ENTITY_TYPE (_obj, VPORT, used, RSd);
  CHK_ENTITY_TYPE (_obj, VPORT, is_xref_ref, B);
  CHK_ENTITY_TYPE (_obj, VPORT, is_xref_resolved, BS);
  CHK_ENTITY_TYPE (_obj, VPORT, is_xref_dep, B);
  CHK_ENTITY_H (_obj, VPORT, xref);

  CHK_ENTITY_TYPE (_obj, VPORT, VIEWSIZE, BD); // really the view height
  CHK_ENTITY_TYPE (_obj, VPORT, view_width,
                   BD); // in DWG r13+, needed to calc. aspect_ratio
  CHK_ENTITY_TYPE (_obj, VPORT, aspect_ratio,
                   BD); // DXF 41 = view_width / VIEWSIZE
  CHK_ENTITY_2RD (_obj, VPORT, VIEWCTR);
  CHK_ENTITY_3RD (_obj, VPORT, view_target);
  CHK_ENTITY_3RD (_obj, VPORT, VIEWDIR);
  CHK_ENTITY_TYPE (_obj, VPORT, view_twist, BD);
  CHK_ENTITY_TYPE (_obj, VPORT, lens_length, BD);
  CHK_ENTITY_TYPE (_obj, VPORT, front_clip_z, BD);
  CHK_ENTITY_TYPE (_obj, VPORT, back_clip_z, BD);
  CHK_ENTITY_TYPE (_obj, VPORT, VIEWMODE, 4BITS);
  CHK_ENTITY_TYPE (_obj, VPORT, render_mode, RC);
  CHK_ENTITY_TYPE (_obj, VPORT, use_default_lights, B);
  CHK_ENTITY_TYPE (_obj, VPORT, default_lightning_type, RC);
  CHK_ENTITY_TYPE (_obj, VPORT, brightness, BD);
  CHK_ENTITY_TYPE (_obj, VPORT, contrast, BD);
  CHK_ENTITY_CMC (_obj, VPORT, ambient_color);
  CHK_ENTITY_H (_obj, VPORT, background);
  CHK_ENTITY_H (_obj, VPORT, visualstyle);
  CHK_ENTITY_H (_obj, VPORT, sun);

  CHK_ENTITY_2RD (_obj, VPORT, lower_left);
  CHK_ENTITY_2RD (_obj, VPORT, upper_right);
  CHK_ENTITY_TYPE (_obj, VPORT, UCSFOLLOW, B);
  CHK_ENTITY_TYPE (_obj, VPORT, circle_zoom,
                   BS); /* circle sides: nr of tesselations */
  CHK_ENTITY_TYPE (_obj, VPORT, FASTZOOM, B);
  CHK_ENTITY_TYPE (_obj, VPORT, UCSICON, RC);
  CHK_ENTITY_TYPE (_obj, VPORT, GRIDMODE, B);
  CHK_ENTITY_2RD (_obj, VPORT, GRIDUNIT);
  CHK_ENTITY_TYPE (_obj, VPORT, SNAPMODE, B);
  CHK_ENTITY_TYPE (_obj, VPORT, SNAPSTYLE, B);
  CHK_ENTITY_TYPE (_obj, VPORT, SNAPISOPAIR, BS);
  CHK_ENTITY_TYPE (_obj, VPORT, SNAPANG, BD);
  CHK_ENTITY_2RD (_obj, VPORT, SNAPBASE);
  CHK_ENTITY_2RD (_obj, VPORT, SNAPUNIT);
  CHK_ENTITY_TYPE (_obj, VPORT, ucs_at_origin, B);
  CHK_ENTITY_TYPE (_obj, VPORT, UCSVP, B);
  CHK_ENTITY_3RD (_obj, VPORT, ucsorg);
  CHK_ENTITY_3RD (_obj, VPORT, ucsxdir);
  CHK_ENTITY_3RD (_obj, VPORT, ucsydir);
  CHK_ENTITY_TYPE (_obj, VPORT, ucs_elevation, BD);
  CHK_ENTITY_TYPE (_obj, VPORT, UCSORTHOVIEW, BS);
  CHK_ENTITY_TYPE (_obj, VPORT, grid_flags, BS);
  CHK_ENTITY_TYPE (_obj, VPORT, grid_major, BS);
  CHK_ENTITY_H (_obj, VPORT, named_ucs);
  CHK_ENTITY_H (_obj, VPORT, base_ucs);
}
