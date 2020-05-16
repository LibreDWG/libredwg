// TODO unstable
#define DWG_TYPE DWG_TYPE_TABLESTYLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version;
  BITCODE_T name;
  BITCODE_BS flags;
  BITCODE_BS flow_direction;
  BITCODE_BD horiz_cell_margin;
  BITCODE_BD vert_cell_margin;
  BITCODE_B is_title_suppressed;
  BITCODE_B is_header_suppressed;
  BITCODE_BL i, num_rowstyles;
  Dwg_TABLESTYLE_rowstyles *rowstyles;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_tablestyle *tblstyle = dwg_object_to_TABLESTYLE (obj);

  CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, class_version, BL);
  CHK_ENTITY_UTF8TEXT (tblstyle, TABLESTYLE, name);
  CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, flow_direction, BS);
  CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, horiz_cell_margin, BD);
  CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, vert_cell_margin, BD);
  CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, is_title_suppressed, B);
  CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, is_header_suppressed, B);
  CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, num_rowstyles, BL);
  if (num_rowstyles != 3)
    fail ("TABLESTYLE.num_rowstyles %d != 3", num_rowstyles);
  if (!dwg_dynapi_entity_value (tblstyle, "TABLESTYLE", "rowstyles", &rowstyles, NULL))
    fail ("TABLESTYLE.rowstyles");

  if (dwg_version >= R_2010)
    {
      printf ("skip: TABLESTYLE r2010+ not yet implemented");
    }
  for (i = 0; i < num_rowstyles; i++)
    {
      CHK_SUBCLASS_H (rowstyles[i], TABLESTYLE_rowstyles, text_style);
      CHK_SUBCLASS_TYPE (rowstyles[i], TABLESTYLE_rowstyles, text_height, BD);
      CHK_SUBCLASS_TYPE (rowstyles[i], TABLESTYLE_rowstyles, text_alignment, BS);
      CHK_SUBCLASS_CMC (rowstyles[i], TABLESTYLE_rowstyles, text_color);
      CHK_SUBCLASS_CMC (rowstyles[i], TABLESTYLE_rowstyles, fill_color);
      CHK_SUBCLASS_TYPE (rowstyles[i], TABLESTYLE_rowstyles, has_bgcolor, B);
      if (rowstyles[i].num_borders != 6)
        fail ("TABLESTYLE.rowstyles[%d].num_borders %d != 6", i, rowstyles[i].num_borders);
      if (rowstyles[i].borders)
        for (int j = 0; j < 6; j++)
          {
            CHK_SUBCLASS_TYPE (rowstyles[i].borders[j], TABLESTYLE_border, linewt, BSd);
            CHK_SUBCLASS_TYPE (rowstyles[i].borders[j], TABLESTYLE_border, visible, B);
            CHK_SUBCLASS_CMC (rowstyles[i].borders[j], TABLESTYLE_border, color);
          }
      if (dwg_version >= R_2007)
        {
          CHK_SUBCLASS_TYPE (rowstyles[i], TABLESTYLE_rowstyles, data_type, BL);
          CHK_SUBCLASS_TYPE (rowstyles[i], TABLESTYLE_rowstyles, unit_type, BL);
          CHK_SUBCLASS_UTF8TEXT (rowstyles[i], TABLESTYLE_rowstyles, format_string);
        }
    }
}
