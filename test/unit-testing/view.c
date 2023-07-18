#define DWG_TYPE DWG_TYPE_VIEW
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

  BITCODE_BD VIEWSIZE;
  BITCODE_BD view_width;
  BITCODE_BD aspect_ratio;
  BITCODE_2RD VIEWCTR;
  BITCODE_3BD view_target;
  BITCODE_3BD VIEWDIR;
  BITCODE_BD twist_angle;
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

  BITCODE_B is_pspace;
  BITCODE_B associated_ucs;
  BITCODE_3BD ucsorg;
  BITCODE_3BD ucsxdir;
  BITCODE_3BD ucsydir;
  BITCODE_BD ucs_elevation;
  BITCODE_BS UCSORTHOVIEW;
  BITCODE_B is_camera_plottable;
  BITCODE_H base_ucs;
  BITCODE_H named_ucs;
  BITCODE_H livesection;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_view *_obj = dwg_object_to_VIEW (obj);

  CHK_ENTITY_TYPE (_obj, VIEW, flag, RC);
  CHK_ENTITY_UTF8TEXT (_obj, VIEW, name);
  CHK_ENTITY_TYPE (_obj, VIEW, used, RSd);
  CHK_ENTITY_TYPE (_obj, VIEW, is_xref_ref, B);
  CHK_ENTITY_TYPE (_obj, VIEW, is_xref_resolved, BS);
  CHK_ENTITY_TYPE (_obj, VIEW, is_xref_dep, B);
  CHK_ENTITY_H (_obj, VIEW, xref);

  CHK_ENTITY_TYPE (_obj, VIEW, VIEWSIZE, BD);
  CHK_ENTITY_TYPE (_obj, VIEW, view_width, BD);
  CHK_ENTITY_TYPE (_obj, VIEW, aspect_ratio, BD);
  CHK_ENTITY_2RD (_obj, VIEW, VIEWCTR);
  CHK_ENTITY_3RD (_obj, VIEW, view_target);
  CHK_ENTITY_3RD (_obj, VIEW, VIEWDIR);
  CHK_ENTITY_TYPE (_obj, VIEW, twist_angle, BD);
  CHK_ENTITY_TYPE (_obj, VIEW, lens_length, BD);
  CHK_ENTITY_TYPE (_obj, VIEW, front_clip_z, BD);
  CHK_ENTITY_TYPE (_obj, VIEW, back_clip_z, BD);
  CHK_ENTITY_TYPE (_obj, VIEW, VIEWMODE, 4BITS);
  CHK_ENTITY_TYPE (_obj, VIEW, render_mode, RC);
  CHK_ENTITY_TYPE (_obj, VIEW, use_default_lights, B);
  CHK_ENTITY_TYPE (_obj, VIEW, default_lightning_type, RC);
  CHK_ENTITY_TYPE (_obj, VIEW, brightness, BD);
  CHK_ENTITY_TYPE (_obj, VIEW, contrast, BD);
  CHK_ENTITY_CMC (_obj, VIEW, ambient_color);
  CHK_ENTITY_H (_obj, VIEW, background);
  CHK_ENTITY_H (_obj, VIEW, visualstyle);
  CHK_ENTITY_H (_obj, VIEW, sun);

  CHK_ENTITY_TYPE (_obj, VIEW, is_pspace, B);
  CHK_ENTITY_TYPE (_obj, VIEW, associated_ucs, B);
  CHK_ENTITY_3RD (_obj, VIEW, ucsorg);
  CHK_ENTITY_3RD (_obj, VIEW, ucsxdir);
  CHK_ENTITY_3RD (_obj, VIEW, ucsydir);
  CHK_ENTITY_TYPE (_obj, VIEW, ucs_elevation, BD);
  CHK_ENTITY_TYPE (_obj, VIEW, UCSORTHOVIEW, BS);
  CHK_ENTITY_TYPE (_obj, VIEW, is_camera_plottable, B);
  CHK_ENTITY_H (_obj, VIEW, base_ucs);
  CHK_ENTITY_H (_obj, VIEW, named_ucs);
  CHK_ENTITY_H (_obj, VIEW, livesection);
}
