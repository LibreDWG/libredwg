#define DWG_TYPE DWG_TYPE_LAYOUT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  Dwg_Object_PLOTSETTINGS plotsettings;
  BITCODE_TV layout_name;
  BITCODE_BS tab_order;
  BITCODE_BS layout_flags;
  BITCODE_3DPOINT INSBASE;
  BITCODE_2DPOINT LIMMIN;
  BITCODE_2DPOINT LIMMAX;
  BITCODE_3DPOINT UCSORG;
  BITCODE_3DPOINT UCSXDIR;
  BITCODE_3DPOINT UCSYDIR;
  BITCODE_BD ucs_elevation;
  BITCODE_BS UCSORTHOVIEW;
  BITCODE_3DPOINT EXTMIN;
  BITCODE_3DPOINT EXTMAX;
  BITCODE_H block_header;
  BITCODE_H active_viewport;
  BITCODE_H base_ucs;
  BITCODE_H named_ucs;
  BITCODE_BL num_viewports;
  BITCODE_H *viewports;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_layout *layout = dwg_object_to_LAYOUT (obj);
  memset (&plotsettings, 0, sizeof (Dwg_Object_PLOTSETTINGS));

  CHK_SUBCLASS_UTF8TEXT (layout->plotsettings, PLOTSETTINGS, printer_cfg_file);
  CHK_SUBCLASS_UTF8TEXT (layout->plotsettings, PLOTSETTINGS, paper_size);
  CHK_SUBCLASS_UTF8TEXT (layout->plotsettings, PLOTSETTINGS,
                         canonical_media_name);
  CHK_SUBCLASS_UTF8TEXT (layout->plotsettings, PLOTSETTINGS, stylesheet);
  CHK_SUBCLASS_UTF8TEXT (layout->plotsettings, PLOTSETTINGS, plotview_name);
  CHK_SUBCLASS_H (layout->plotsettings, PLOTSETTINGS, plotview);
  CHK_SUBCLASS_TYPE (layout->plotsettings, PLOTSETTINGS, plot_flags, BSx);
  CHK_SUBCLASS_TYPE (layout->plotsettings, PLOTSETTINGS, left_margin, BD);
  CHK_SUBCLASS_TYPE (layout->plotsettings, PLOTSETTINGS, bottom_margin, BD);
  CHK_SUBCLASS_TYPE (layout->plotsettings, PLOTSETTINGS, right_margin, BD);
  CHK_SUBCLASS_TYPE (layout->plotsettings, PLOTSETTINGS, top_margin, BD);
  CHK_SUBCLASS_TYPE (layout->plotsettings, PLOTSETTINGS, paper_width, BD);
  CHK_SUBCLASS_TYPE (layout->plotsettings, PLOTSETTINGS, paper_height, BD);
  CHK_SUBCLASS_2RD (layout->plotsettings, PLOTSETTINGS, plot_origin);
  CHK_SUBCLASS_TYPE (layout->plotsettings, PLOTSETTINGS, plot_paper_unit, BS);
  CHK_SUBCLASS_MAX (layout->plotsettings, PLOTSETTINGS, plot_paper_unit, BS,
                    2);
  CHK_SUBCLASS_TYPE (layout->plotsettings, PLOTSETTINGS, plot_rotation_mode,
                     BS);
  CHK_SUBCLASS_MAX (layout->plotsettings, PLOTSETTINGS, plot_rotation_mode, BS,
                    3);
  CHK_SUBCLASS_TYPE (layout->plotsettings, PLOTSETTINGS, plot_type, BS);
  CHK_SUBCLASS_MAX (layout->plotsettings, PLOTSETTINGS, plot_type, BS, 5);
  CHK_SUBCLASS_2RD (layout->plotsettings, PLOTSETTINGS, plot_window_ll);
  CHK_SUBCLASS_2RD (layout->plotsettings, PLOTSETTINGS, plot_window_ur);
  CHK_SUBCLASS_TYPE (layout->plotsettings, PLOTSETTINGS, paper_units, BD);
  CHK_SUBCLASS_TYPE (layout->plotsettings, PLOTSETTINGS, drawing_units, BD);
  CHK_SUBCLASS_TYPE (layout->plotsettings, PLOTSETTINGS, std_scale_type, BS);
  CHK_SUBCLASS_MAX (layout->plotsettings, PLOTSETTINGS, std_scale_type, BS,
                    32);
  CHK_SUBCLASS_TYPE (layout->plotsettings, PLOTSETTINGS, std_scale_factor, BD);
  CHK_SUBCLASS_2RD (layout->plotsettings, PLOTSETTINGS, paper_image_origin);
  CHK_SUBCLASS_TYPE (layout->plotsettings, PLOTSETTINGS, shadeplot_type, BS);
  CHK_SUBCLASS_MAX (layout->plotsettings, PLOTSETTINGS, shadeplot_type, BS, 5);
  CHK_SUBCLASS_TYPE (layout->plotsettings, PLOTSETTINGS, shadeplot_reslevel,
                     BS);
  CHK_SUBCLASS_MAX (layout->plotsettings, PLOTSETTINGS, shadeplot_reslevel, BS,
                    5);
  CHK_SUBCLASS_TYPE (layout->plotsettings, PLOTSETTINGS, shadeplot_customdpi,
                     BS);
  CHK_SUBCLASS_H (layout->plotsettings, PLOTSETTINGS, shadeplot);

  CHK_ENTITY_UTF8TEXT (layout, LAYOUT, layout_name);
  CHK_ENTITY_TYPE (layout, LAYOUT, tab_order, BS);
  CHK_ENTITY_TYPE (layout, LAYOUT, layout_flags, BSx);
  CHK_ENTITY_3RD (layout, LAYOUT, INSBASE);
  CHK_ENTITY_2RD (layout, LAYOUT, LIMMIN);
  CHK_ENTITY_2RD (layout, LAYOUT, LIMMAX);
  CHK_ENTITY_3RD (layout, LAYOUT, UCSORG);
  CHK_ENTITY_3RD (layout, LAYOUT, UCSXDIR);
  CHK_ENTITY_3RD (layout, LAYOUT, UCSYDIR);
  CHK_ENTITY_TYPE (layout, LAYOUT, ucs_elevation, BD);
  CHK_ENTITY_TYPE (layout, LAYOUT, UCSORTHOVIEW, BS);
  CHK_ENTITY_MAX (layout, LAYOUT, UCSORTHOVIEW, BS, 6);
  CHK_ENTITY_3RD (layout, LAYOUT, EXTMIN);
  CHK_ENTITY_3RD (layout, LAYOUT, EXTMAX);

  CHK_ENTITY_H (layout, LAYOUT, block_header);
  CHK_ENTITY_H (layout, LAYOUT, active_viewport);
  CHK_ENTITY_H (layout, LAYOUT, base_ucs);
  CHK_ENTITY_H (layout, LAYOUT, named_ucs);
  CHK_ENTITY_TYPE (layout, LAYOUT, num_viewports, BL);
  CHK_ENTITY_HV (layout, LAYOUT, viewports, num_viewports);
}
