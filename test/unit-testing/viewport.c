#define DWG_TYPE DWG_TYPE_VIEWPORT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  dwg_point_3d center, view_target, view_direction;
  BITCODE_BD width;
  BITCODE_BD height;
  BITCODE_RS on_off;
  BITCODE_RS id;
  BITCODE_BD view_twist;
  BITCODE_BD view_height;
  BITCODE_BD lens_length;
  BITCODE_BD front_clip_z;
  BITCODE_BD back_clip_z;
  BITCODE_BD snap_angle;
  dwg_point_2d view_center, snap_base, snap_spacing, grid_spacing;
  BITCODE_BS circle_zoom;
  BITCODE_BS grid_major;
  BITCODE_BL num_frozen_layers;
  BITCODE_BL status_flag;
  BITCODE_TV style_sheet;
  BITCODE_RC render_mode;
  BITCODE_B ucs_at_origin;
  BITCODE_B ucs_per_viewport;
  dwg_point_3d ucs_origin, ucs_x_axis, ucs_y_axis;
  BITCODE_BD ucs_elevation;
  BITCODE_BS ucs_ortho_view_type;
  BITCODE_BS shadeplot_mode;
  BITCODE_B use_default_lights;
  BITCODE_RC default_lighting_type;
  BITCODE_BD brightness;
  BITCODE_BD contrast;
  BITCODE_CMC ambient_color;

  dwg_ent_viewport *viewport = dwg_object_to_VIEWPORT (obj);

  CHK_ENTITY_3RD_W_OLD (viewport, VIEWPORT, center, center);
  CHK_ENTITY_3RD_W_OLD (viewport, VIEWPORT, view_target, view_target);
  CHK_ENTITY_3RD_W_OLD (viewport, VIEWPORT, view_direction, view_direction);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, width, BD, width);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, height, BD, height);
  CHK_ENTITY_TYPE (viewport, VIEWPORT, on_off, RS, on_off);
  CHK_ENTITY_TYPE (viewport, VIEWPORT, id, RS, id);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, view_twist, BD, view_twist);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, view_height, BD, view_height);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, lens_length, BD, lens_length);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, front_clip_z, BD, front_clip_z);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, back_clip_z, BD, back_clip_z);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, snap_angle, BD, snap_angle);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, circle_zoom, BS, circle_zoom);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, grid_major, BS, grid_major);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, num_frozen_layers, BL, num_frozen_layers);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, status_flag, BL, status_flag);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, style_sheet, TV, style_sheet);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, render_mode, RC, render_mode);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, ucs_at_origin, B, ucs_at_origin);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, ucs_per_viewport, B, ucs_per_viewport);
  CHK_ENTITY_3RD_W_OLD (viewport, VIEWPORT, ucs_origin, ucs_origin);
  CHK_ENTITY_3RD_W_OLD (viewport, VIEWPORT, ucs_x_axis, ucs_x_axis);
  CHK_ENTITY_3RD_W_OLD (viewport, VIEWPORT, ucs_y_axis, ucs_y_axis);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, ucs_elevation, BD, ucs_elevation);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, ucs_ortho_view_type, BS, ucs_ortho_view_type);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, shadeplot_mode, BS, shadeplot_mode);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, use_default_lights, B, use_default_lights);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, default_lighting_type, RC, default_lighting_type);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, brightness, BD, brightness);
  CHK_ENTITY_TYPE_W_OLD (viewport, VIEWPORT, contrast, BD, contrast);
  //CHK_ENTITY_TYPE (viewport, VIEWPORT, ambient_color, CMC, ambient_color);
}
