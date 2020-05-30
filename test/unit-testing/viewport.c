#define DWG_TYPE DWG_TYPE_VIEWPORT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  dwg_point_3d center, view_target, VIEWDIR;
  BITCODE_BD width;
  BITCODE_BD height;
  BITCODE_RS on_off;
  BITCODE_RS id;

  dwg_point_2d VIEWCTR, SNAPBASE, SNAPUNIT, GRIDUNIT;
  BITCODE_BD twist_angle;
  BITCODE_BD VIEWSIZE;
  BITCODE_BD lens_length;
  BITCODE_BD front_clip_z;
  BITCODE_BD back_clip_z;
  BITCODE_BD SNAPANG;
  BITCODE_BS circle_zoom;
  BITCODE_BS grid_major;
  BITCODE_BL num_frozen_layers;
  BITCODE_BL status_flag;
  BITCODE_TV style_sheet;
  BITCODE_RC render_mode;
  BITCODE_B ucs_at_origin;
  BITCODE_B UCSVP;
  dwg_point_3d ucsorg, ucsxdir, ucsydir;
  BITCODE_BD ucs_elevation;
  BITCODE_BS UCSORTHOVIEW;
  BITCODE_BS shadeplot_mode;
  BITCODE_B use_default_lights;
  BITCODE_RC default_lighting_type;
  BITCODE_BD brightness;
  BITCODE_BD contrast;
  BITCODE_CMC ambient_color;
  BITCODE_H vport_entity_header;
  BITCODE_H *frozen_layers;
  BITCODE_H clip_boundary;
  BITCODE_H named_ucs;
  BITCODE_H base_ucs;
  BITCODE_H background;
  BITCODE_H visualstyle;
  BITCODE_H shadeplot;
  BITCODE_H sun;

  dwg_ent_viewport *viewport = dwg_object_to_VIEWPORT (obj);

  CHK_ENTITY_3RD_W_OLD (viewport, VIEWPORT, center);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, width, BD);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, height, BD);
  CHK_ENTITY_3RD_W_OLD (viewport, VIEWPORT, view_target);
  CHK_ENTITY_3RD_W_OLD (viewport, VIEWPORT, VIEWDIR);
  CHK_ENTITY_TYPE (viewport, VIEWPORT, on_off, RS);
  CHK_ENTITY_TYPE (viewport, VIEWPORT, id, RS);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, twist_angle, BD);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, VIEWSIZE, BD);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, lens_length, BD);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, front_clip_z, BD);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, back_clip_z, BD);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, SNAPANG, BD);
  CHK_ENTITY_MAX (viewport, VIEWPORT, SNAPANG, BD, MAX_ANGLE);
  CHK_ENTITY_2RD (viewport, VIEWPORT, VIEWCTR);
  CHK_ENTITY_2RD (viewport, VIEWPORT, SNAPBASE);
  CHK_ENTITY_2RD (viewport, VIEWPORT, SNAPUNIT);
  CHK_ENTITY_2RD (viewport, VIEWPORT, GRIDUNIT);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, circle_zoom, BS);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, grid_major, BS);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, num_frozen_layers, BL);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, status_flag, BL);
  CHK_ENTITY_UTF8TEXT_W_OLD (viewport, VIEWPORT, style_sheet);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, render_mode, RC);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, ucs_at_origin, B);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, UCSVP, B);
  CHK_ENTITY_3RD_W_OLD (viewport, VIEWPORT, ucsorg);
  CHK_ENTITY_3RD_W_OLD (viewport, VIEWPORT, ucsxdir);
  CHK_ENTITY_3RD_W_OLD (viewport, VIEWPORT, ucsydir);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, ucs_elevation, BD);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, UCSORTHOVIEW, BS);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, shadeplot_mode, BS);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, use_default_lights, B);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, default_lighting_type, RC);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, brightness, BD);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, contrast, BD);
  CHK_ENTITY_CMC (viewport, VIEWPORT, ambient_color);
  CHK_ENTITY_H (viewport, VIEWPORT, background);
  CHK_ENTITY_H (viewport, VIEWPORT, visualstyle);
  CHK_ENTITY_H (viewport, VIEWPORT, sun);

  CHK_ENTITY_H (viewport, VIEWPORT, vport_entity_header);
  CHK_ENTITY_HV (viewport, VIEWPORT, frozen_layers, num_frozen_layers);
  CHK_ENTITY_H (viewport, VIEWPORT, clip_boundary);
  CHK_ENTITY_H (viewport, VIEWPORT, named_ucs);
  CHK_ENTITY_H (viewport, VIEWPORT, base_ucs);
  CHK_ENTITY_H (viewport, VIEWPORT, shadeplot);
}
