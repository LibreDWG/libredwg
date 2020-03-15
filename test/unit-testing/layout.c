#define DWG_TYPE DWG_TYPE_LAYOUT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  // AcDbPlotSettings:
  BITCODE_TV page_setup_name;
  BITCODE_TV printer_or_config;
  BITCODE_BS plot_layout_flags;
  BITCODE_BD left_margin;
  BITCODE_BD bottom_margin;
  BITCODE_BD right_margin;
  BITCODE_BD top_margin;
  BITCODE_BD paper_width;
  BITCODE_BD paper_height;
  BITCODE_TV paper_size;
  BITCODE_2BD_1 plot_origin;
  BITCODE_BS plot_paper_unit;
  BITCODE_BS plot_rotation;
  BITCODE_BS plot_type;
  BITCODE_2BD_1 plot_window_ll;
  BITCODE_2BD_1 plot_window_ur;
  BITCODE_TV plot_view_name;
  BITCODE_BD paper_units;
  BITCODE_BD drawing_units;
  BITCODE_TV current_style_sheet;
  BITCODE_BS scale_type;
  BITCODE_BD scale_factor;
  BITCODE_2BD_1 paper_image_origin;
  BITCODE_BS shade_plot_mode;
  BITCODE_BS shade_plot_reslevel;
  BITCODE_BS shade_plot_customdpi;

  // AcDbLayout:
  BITCODE_TV layout_name;
  BITCODE_BS tab_order;
  BITCODE_BS flag;
  BITCODE_3DPOINT ucs_origin;
  BITCODE_2DPOINT minimum_limits;
  BITCODE_2DPOINT maximum_limits;
  BITCODE_3DPOINT ins_point;
  BITCODE_3DPOINT ucs_x_axis;
  BITCODE_3DPOINT ucs_y_axis;
  BITCODE_BD elevation;
  BITCODE_BS orthoview_type;
  BITCODE_3DPOINT extent_min;
  BITCODE_3DPOINT extent_max;
  BITCODE_H plotview;
  BITCODE_H visualstyle;
  BITCODE_H block_header;
  BITCODE_H active_viewport;
  BITCODE_H base_ucs;
  BITCODE_H named_ucs;
  BITCODE_BL num_viewports;
  BITCODE_H* viewports;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_layout *layout = dwg_object_to_LAYOUT (obj);

  CHK_ENTITY_UTF8TEXT (layout, LAYOUT, page_setup_name, page_setup_name);
  CHK_ENTITY_UTF8TEXT (layout, LAYOUT, printer_or_config, printer_or_config);
  CHK_ENTITY_UTF8TEXT (layout, LAYOUT, paper_size, paper_size);
  CHK_ENTITY_UTF8TEXT (layout, LAYOUT, plot_view_name, plot_view_name);
  CHK_ENTITY_UTF8TEXT (layout, LAYOUT, current_style_sheet, current_style_sheet);
  CHK_ENTITY_H (layout, LAYOUT, plotview, plotview);

  CHK_ENTITY_TYPE (layout, LAYOUT, plot_layout_flags, BS, plot_layout_flags);
  CHK_ENTITY_TYPE (layout, LAYOUT, left_margin, BD, left_margin);
  CHK_ENTITY_TYPE (layout, LAYOUT, bottom_margin, BD, bottom_margin);
  CHK_ENTITY_TYPE (layout, LAYOUT, right_margin, BD, right_margin);
  CHK_ENTITY_TYPE (layout, LAYOUT, top_margin, BD, top_margin);
  CHK_ENTITY_TYPE (layout, LAYOUT, paper_width, BD, paper_width);
  CHK_ENTITY_TYPE (layout, LAYOUT, paper_height, BD, paper_height);
  CHK_ENTITY_2RD (layout, LAYOUT, plot_origin, plot_origin);
  CHK_ENTITY_TYPE (layout, LAYOUT, plot_paper_unit, BS, plot_paper_unit);
  CHK_ENTITY_MAX (layout, LAYOUT, plot_paper_unit, BS, 2);
  CHK_ENTITY_TYPE (layout, LAYOUT, plot_rotation, BS, plot_rotation);
  CHK_ENTITY_TYPE (layout, LAYOUT, plot_type, BS, plot_type);
  CHK_ENTITY_2RD (layout, LAYOUT, plot_window_ll, plot_window_ll);
  CHK_ENTITY_2RD (layout, LAYOUT, plot_window_ur, plot_window_ur);
  CHK_ENTITY_TYPE (layout, LAYOUT, paper_units, BD, paper_units);
  CHK_ENTITY_TYPE (layout, LAYOUT, drawing_units, BD, drawing_units);
  CHK_ENTITY_TYPE (layout, LAYOUT, scale_type, BS, scale_type);
  CHK_ENTITY_TYPE (layout, LAYOUT, scale_factor, BD, scale_factor);
  CHK_ENTITY_2RD (layout, LAYOUT, paper_image_origin, paper_image_origin);
  CHK_ENTITY_TYPE (layout, LAYOUT, shade_plot_mode, BS, shade_plot_mode);
  CHK_ENTITY_MAX (layout, LAYOUT, shade_plot_mode, BS, 5);
  CHK_ENTITY_TYPE (layout, LAYOUT, shade_plot_reslevel, BS, shade_plot_reslevel);
  CHK_ENTITY_MAX (layout, LAYOUT, shade_plot_reslevel, BS, 5);
  CHK_ENTITY_TYPE (layout, LAYOUT, shade_plot_customdpi, BS, shade_plot_customdpi);

  CHK_ENTITY_TYPE (layout, LAYOUT, layout_name, TV, layout_name);
  CHK_ENTITY_TYPE (layout, LAYOUT, tab_order, BS, tab_order);
  CHK_ENTITY_TYPE (layout, LAYOUT, flag, BS, flag);
  CHK_ENTITY_3RD (layout, LAYOUT, ucs_origin, ucs_origin);
  CHK_ENTITY_2RD (layout, LAYOUT, minimum_limits, minimum_limits);
  CHK_ENTITY_2RD (layout, LAYOUT, maximum_limits, maximum_limits);
  CHK_ENTITY_3RD (layout, LAYOUT, ins_point,  ins_point);
  CHK_ENTITY_3RD (layout, LAYOUT, ucs_x_axis, ucs_x_axis);
  CHK_ENTITY_3RD (layout, LAYOUT, ucs_y_axis, ucs_y_axis);
  CHK_ENTITY_TYPE (layout, LAYOUT, elevation, BD, elevation);
  CHK_ENTITY_TYPE (layout, LAYOUT, orthoview_type, BS, orthoview_type);
  CHK_ENTITY_MAX (layout, LAYOUT, orthoview_type, BS, 6);
  CHK_ENTITY_3RD (layout, LAYOUT, extent_min, extent_min);
  CHK_ENTITY_3RD (layout, LAYOUT, extent_max, extent_max);

  CHK_ENTITY_H (layout, LAYOUT, visualstyle, visualstyle);
  CHK_ENTITY_H (layout, LAYOUT, block_header, block_header);
  CHK_ENTITY_H (layout, LAYOUT, active_viewport, active_viewport);
  CHK_ENTITY_H (layout, LAYOUT, base_ucs, base_ucs);
  CHK_ENTITY_H (layout, LAYOUT, named_ucs, named_ucs); 
}
