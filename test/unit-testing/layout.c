#define DWG_TYPE DWG_TYPE_LAYOUT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;

  // AcDbPlotSettings:
  BITCODE_T printer_cfg_file;
  BITCODE_T paper_size;
  BITCODE_T canonical_media_name;
  BITCODE_BS plot_flags;
  BITCODE_BD left_margin;
  BITCODE_BD bottom_margin;
  BITCODE_BD right_margin;
  BITCODE_BD top_margin;
  BITCODE_BD paper_width;
  BITCODE_BD paper_height;
  BITCODE_2BD_1 plot_origin;
  BITCODE_BS plot_paper_unit;
  BITCODE_BS plot_rotation_mode;
  BITCODE_BS plot_type;
  BITCODE_2BD_1 plot_window_ll;
  BITCODE_2BD_1 plot_window_ur;
  BITCODE_H plotview;
  BITCODE_TV plotview_name;
  BITCODE_BD paper_units;
  BITCODE_BD drawing_units;
  BITCODE_TV stylesheet;
  BITCODE_BS std_scale_type;
  BITCODE_BD std_scale_factor;
  BITCODE_2BD_1 paper_image_origin;
  BITCODE_BS shadeplot_mode;
  BITCODE_BS shadeplot_reslevel;
  BITCODE_BS shadeplot_customdpi;
  BITCODE_H  shadeplot;

  // AcDbLayout:
  BITCODE_T layout_name;
  BITCODE_BS tab_order;
  BITCODE_BS layout_flags;
  BITCODE_3DPOINT INSBASE;
  BITCODE_2DPOINT LIMMIN;
  BITCODE_2DPOINT LIMMAX;
  BITCODE_3DPOINT UCSORG;
  BITCODE_3DPOINT UCSXDIR;
  BITCODE_3DPOINT UCSYDIR;
  BITCODE_BD ucs_elevation;
  BITCODE_BS ucs_orthoview_type;
  BITCODE_3DPOINT EXTMIN;
  BITCODE_3DPOINT EXTMAX;
  BITCODE_H block_header;
  BITCODE_H active_viewport;
  BITCODE_H base_ucs;
  BITCODE_H named_ucs;
  BITCODE_BL num_viewports;
  BITCODE_H* viewports;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_layout *layout = dwg_object_to_LAYOUT (obj);

  CHK_ENTITY_UTF8TEXT (layout, LAYOUT, printer_cfg_file);
  CHK_ENTITY_UTF8TEXT (layout, LAYOUT, paper_size);
  CHK_ENTITY_UTF8TEXT (layout, LAYOUT, canonical_media_name);
  CHK_ENTITY_UTF8TEXT (layout, LAYOUT, stylesheet);
  CHK_ENTITY_UTF8TEXT (layout, LAYOUT, plotview_name);
  CHK_ENTITY_H (layout, LAYOUT, plotview);

  CHK_ENTITY_TYPE (layout, LAYOUT, plot_flags, BSx);
  CHK_ENTITY_TYPE (layout, LAYOUT, left_margin, BD);
  CHK_ENTITY_TYPE (layout, LAYOUT, bottom_margin, BD);
  CHK_ENTITY_TYPE (layout, LAYOUT, right_margin, BD);
  CHK_ENTITY_TYPE (layout, LAYOUT, top_margin, BD);
  CHK_ENTITY_TYPE (layout, LAYOUT, paper_width, BD);
  CHK_ENTITY_TYPE (layout, LAYOUT, paper_height, BD);
  CHK_ENTITY_2RD (layout, LAYOUT, plot_origin);
  CHK_ENTITY_TYPE (layout, LAYOUT, plot_paper_unit, BS);
  CHK_ENTITY_MAX (layout, LAYOUT, plot_paper_unit, BS, 2);
  CHK_ENTITY_TYPE (layout, LAYOUT, plot_rotation_mode, BS);
  CHK_ENTITY_MAX (layout, LAYOUT, plot_rotation_mode, BS, 3);
  CHK_ENTITY_TYPE (layout, LAYOUT, plot_type, BS);
  CHK_ENTITY_2RD (layout, LAYOUT, plot_window_ll);
  CHK_ENTITY_2RD (layout, LAYOUT, plot_window_ur);
  CHK_ENTITY_TYPE (layout, LAYOUT, paper_units, BD);
  CHK_ENTITY_TYPE (layout, LAYOUT, drawing_units, BD);
  CHK_ENTITY_TYPE (layout, LAYOUT, std_scale_type, BS);
  CHK_ENTITY_TYPE (layout, LAYOUT, std_scale_factor, BD);
  CHK_ENTITY_2RD (layout, LAYOUT, paper_image_origin);
  CHK_ENTITY_TYPE (layout, LAYOUT, shadeplot_mode, BS);
  CHK_ENTITY_MAX (layout, LAYOUT, shadeplot_mode, BS, 5);
  CHK_ENTITY_TYPE (layout, LAYOUT, shadeplot_reslevel, BS);
  CHK_ENTITY_MAX (layout, LAYOUT, shadeplot_reslevel, BS, 5);
  CHK_ENTITY_TYPE (layout, LAYOUT, shadeplot_customdpi, BS);
  CHK_ENTITY_H (layout, LAYOUT, shadeplot);

  CHK_ENTITY_TYPE (layout, LAYOUT, layout_name, T);
  CHK_ENTITY_TYPE (layout, LAYOUT, tab_order, BS);
  CHK_ENTITY_TYPE (layout, LAYOUT, layout_flags, BSx);
  CHK_ENTITY_3RD (layout, LAYOUT, INSBASE);
  CHK_ENTITY_2RD (layout, LAYOUT, LIMMIN);
  CHK_ENTITY_2RD (layout, LAYOUT, LIMMAX);
  CHK_ENTITY_3RD (layout, LAYOUT, UCSORG);
  CHK_ENTITY_3RD (layout, LAYOUT, UCSXDIR);
  CHK_ENTITY_3RD (layout, LAYOUT, UCSYDIR);
  CHK_ENTITY_TYPE (layout, LAYOUT, ucs_elevation, BD);
  CHK_ENTITY_TYPE (layout, LAYOUT, ucs_orthoview_type, BS);
  CHK_ENTITY_MAX (layout, LAYOUT, ucs_orthoview_type, BS, 6);
  CHK_ENTITY_3RD (layout, LAYOUT, EXTMIN);
  CHK_ENTITY_3RD (layout, LAYOUT, EXTMAX);

  CHK_ENTITY_H (layout, LAYOUT, block_header);
  CHK_ENTITY_H (layout, LAYOUT, active_viewport);
  CHK_ENTITY_H (layout, LAYOUT, base_ucs);
  CHK_ENTITY_H (layout, LAYOUT, named_ucs);
  CHK_ENTITY_TYPE (layout, LAYOUT, num_viewports, BL);
  CHK_ENTITY_HV (layout, LAYOUT, viewports, num_viewports);
}
