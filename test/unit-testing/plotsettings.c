#define DWG_TYPE DWG_TYPE_PLOTSETTINGS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_T printer_cfg_file;
  BITCODE_T paper_size;
  BITCODE_BD left_margin;
  BITCODE_BD bottom_margin;
  BITCODE_BD right_margin;
  BITCODE_BD top_margin;
  BITCODE_BD paper_width;
  BITCODE_BD paper_height;
  BITCODE_T canonical_media_name;
  BITCODE_2BD_1 plot_origin;
  BITCODE_2BD_1 plot_window_ll;
  BITCODE_2BD_1 plot_window_ur;
  BITCODE_H plotview;
  BITCODE_TV plotview_name;
  BITCODE_BD paper_units;
  BITCODE_BD drawing_units;
  BITCODE_BS plot_flags;
  BITCODE_BS plot_paper_unit;
  BITCODE_BS plot_rotation_mode;
  BITCODE_BS plot_type;
  BITCODE_T stylesheet;
  BITCODE_BS std_scale_type;
  BITCODE_BD std_scale_factor;
  BITCODE_2BD_1 paper_image_origin;
  BITCODE_BS shadeplot_type;
  BITCODE_BS shadeplot_reslevel;
  BITCODE_BS shadeplot_customdpi;
  BITCODE_H shadeplot;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_plotsettings *plt = dwg_object_to_PLOTSETTINGS (obj);

  CHK_ENTITY_UTF8TEXT (plt, PLOTSETTINGS, printer_cfg_file);
  CHK_ENTITY_UTF8TEXT (plt, PLOTSETTINGS, paper_size);
  CHK_ENTITY_UTF8TEXT (plt, PLOTSETTINGS, canonical_media_name);
  CHK_ENTITY_UTF8TEXT (plt, PLOTSETTINGS, plotview_name);
  CHK_ENTITY_H (plt, PLOTSETTINGS, plotview);

  CHK_ENTITY_TYPE (plt, PLOTSETTINGS, plot_flags, BSx);
  CHK_ENTITY_TYPE (plt, PLOTSETTINGS, left_margin, BD);
  CHK_ENTITY_TYPE (plt, PLOTSETTINGS, bottom_margin, BD);
  CHK_ENTITY_TYPE (plt, PLOTSETTINGS, right_margin, BD);
  CHK_ENTITY_TYPE (plt, PLOTSETTINGS, top_margin, BD);
  CHK_ENTITY_TYPE (plt, PLOTSETTINGS, paper_width, BD);
  CHK_ENTITY_TYPE (plt, PLOTSETTINGS, paper_height, BD);
  CHK_ENTITY_2RD (plt, PLOTSETTINGS, plot_origin);
  CHK_ENTITY_TYPE (plt, PLOTSETTINGS, plot_paper_unit, BS);
  CHK_ENTITY_MAX (plt, PLOTSETTINGS, plot_paper_unit, BS, 2);
  CHK_ENTITY_TYPE (plt, PLOTSETTINGS, plot_rotation_mode, BS);
  CHK_ENTITY_MAX (plt, PLOTSETTINGS, plot_rotation_mode, BS, 3);
  CHK_ENTITY_TYPE (plt, PLOTSETTINGS, plot_type, BS);
  CHK_ENTITY_MAX (plt, PLOTSETTINGS, plot_type, BS, 5);
  CHK_ENTITY_2RD (plt, PLOTSETTINGS, plot_window_ll);
  CHK_ENTITY_2RD (plt, PLOTSETTINGS, plot_window_ur);
  CHK_ENTITY_TYPE (plt, PLOTSETTINGS, paper_units, BD);
  CHK_ENTITY_TYPE (plt, PLOTSETTINGS, drawing_units, BD);
  CHK_ENTITY_UTF8TEXT (plt, PLOTSETTINGS, stylesheet);
  CHK_ENTITY_TYPE (plt, PLOTSETTINGS, std_scale_type, BS);
  CHK_ENTITY_MAX (plt, PLOTSETTINGS, std_scale_type, BS, 32);
  CHK_ENTITY_TYPE (plt, PLOTSETTINGS, std_scale_factor, BD);
  CHK_ENTITY_2RD (plt, PLOTSETTINGS, paper_image_origin);
  // 2004+:
  CHK_ENTITY_TYPE (plt, PLOTSETTINGS, shadeplot_type, BS);
  CHK_ENTITY_MAX (plt, PLOTSETTINGS, shadeplot_type, BS, 5);
  CHK_ENTITY_TYPE (plt, PLOTSETTINGS, shadeplot_reslevel, BS);
  CHK_ENTITY_MAX (plt, PLOTSETTINGS, shadeplot_reslevel, BS, 5);
  CHK_ENTITY_TYPE (plt, PLOTSETTINGS, shadeplot_customdpi, BS);
  // 2007+:
  CHK_ENTITY_H (plt, PLOTSETTINGS, shadeplot);
}
