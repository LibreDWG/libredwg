#define DWG_TYPE DWG_TYPE_SUNSTUDY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL class_version;
  BITCODE_T setup_name;
  BITCODE_T description;
  BITCODE_BL output_type;
  BITCODE_T sheet_set_name;
  BITCODE_B use_subset;
  BITCODE_T sheet_subset_name;
  BITCODE_B select_dates_from_calendar;
  BITCODE_BL num_dates;
  Dwg_SUNSTUDY_Dates* dates;
  BITCODE_B select_range_of_dates;
  BITCODE_BL start_time;
  BITCODE_BL end_time;
  BITCODE_BL interval;
  BITCODE_BL num_hours;
  BITCODE_B* hours;
  BITCODE_BL shade_plot_type;
  BITCODE_BL numvports;
  BITCODE_BL numrows;
  BITCODE_BL numcols;
  BITCODE_BD spacing;
  BITCODE_B  lock_viewports;
  BITCODE_B  label_viewports;
  BITCODE_H  page_setup_wizard;
  BITCODE_H  view;
  BITCODE_H  visualstyle;
  BITCODE_H  text_style;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_sunstudy *_obj = dwg_object_to_SUNSTUDY (obj);

  CHK_ENTITY_TYPE (_obj, SUNSTUDY, class_version, BL, class_version);
  CHK_ENTITY_UTF8TEXT (_obj, SUNSTUDY, setup_name, setup_name);
  CHK_ENTITY_UTF8TEXT (_obj, SUNSTUDY, description, description);
  CHK_ENTITY_TYPE (_obj, SUNSTUDY, output_type, BL, output_type);
  CHK_ENTITY_UTF8TEXT (_obj, SUNSTUDY, sheet_set_name, sheet_set_name);
  CHK_ENTITY_TYPE (_obj, SUNSTUDY, use_subset, B, use_subset);
  CHK_ENTITY_UTF8TEXT (_obj, SUNSTUDY, sheet_subset_name, sheet_subset_name);
  CHK_ENTITY_TYPE (_obj, SUNSTUDY, select_dates_from_calendar, B, select_dates_from_calendar);
  CHK_ENTITY_TYPE (_obj, SUNSTUDY, num_dates, BL, num_dates);
  //Dwg_SUNSTUDY_Dates* dates;
  CHK_ENTITY_TYPE (_obj, SUNSTUDY, select_range_of_dates, B, select_range_of_dates);
  CHK_ENTITY_TYPE (_obj, SUNSTUDY, start_time, BL, start_time);
  CHK_ENTITY_TYPE (_obj, SUNSTUDY, end_time, BL, end_time);
  CHK_ENTITY_TYPE (_obj, SUNSTUDY, interval, BL, interval);
  CHK_ENTITY_TYPE (_obj, SUNSTUDY, num_hours, BL, num_hours);
  //BITCODE_B* hours;
  CHK_ENTITY_TYPE (_obj, SUNSTUDY, shade_plot_type, BL, shade_plot_type);
  CHK_ENTITY_TYPE (_obj, SUNSTUDY, numvports, BL, numvports);
  CHK_ENTITY_TYPE (_obj, SUNSTUDY, numrows, BL, numrows);
  CHK_ENTITY_TYPE (_obj, SUNSTUDY, numcols, BL, numcols);
  CHK_ENTITY_TYPE (_obj, SUNSTUDY, spacing, BD, spacing);
  CHK_ENTITY_TYPE (_obj, SUNSTUDY, lock_viewports, B, lock_viewports);
  CHK_ENTITY_TYPE (_obj, SUNSTUDY, label_viewports, B, label_viewports);
  CHK_ENTITY_H (_obj, SUNSTUDY, page_setup_wizard, page_setup_wizard);
  CHK_ENTITY_H (_obj, SUNSTUDY, view, view);
  CHK_ENTITY_H (_obj, SUNSTUDY, visualstyle, visualstyle);
  CHK_ENTITY_H (_obj, SUNSTUDY, text_style, text_style);
#endif
}
