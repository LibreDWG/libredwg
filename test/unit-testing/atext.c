//DEBUGGING
#define DWG_TYPE DWG_TYPE_ATEXT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error = 0, isnew;
  // TEXT:
  //BITCODE_RD elevation, thickness, rotation, height, oblique_ang, width_factor,
  //    rdvalue;
  //BITCODE_BS generation, vert_alignment, horiz_alignment, bsvalue;
  //BITCODE_RC dataflags, rcvalue;
  //char *text_value;
  //dwg_point_3d ext;
  //dwg_point_2d pt2d, ins_pt, alignment_pt;
  //ATEXT:
  BITCODE_D2T text_size;	/*!< DXF 42 */
  BITCODE_D2T xscale;		/*!< DXF 41 */
  BITCODE_D2T char_spacing;	/*!< DXF 43 */
  BITCODE_T style;		/*!< DXF 7 */
  BITCODE_T t2;			/*!< DXF 2 */
  BITCODE_T t3;			/*!< DXF 3 */
  BITCODE_T text_value;		/*!< DXF 1 */
  BITCODE_D2T offset_from_arc;	/*!< DXF 44 */
  BITCODE_D2T right_offset;	/*!< DXF 45 */
  BITCODE_D2T left_offset;	/*!< DXF 46 */
  BITCODE_3BD center;		/*!< DXF 10 */
  BITCODE_BD radius;		/*!< DXF 40 */
  BITCODE_BD start_angle;	/*!< DXF 50 */
  BITCODE_BD end_angle;		/*!< DXF 51 */
  BITCODE_3BD extrusion;
  BITCODE_BL color_index;
  BITCODE_BS is_reverse;	/*!< DXF 70 */
  BITCODE_BS text_direction;	/*!< DXF 71 */
  BITCODE_BS alignment;		/*!< DXF 72 */
  BITCODE_BS text_position;	/*!< DXF 73 */
  BITCODE_BS font_19;		/*!< DXF 74 */
  BITCODE_BS bs2;		/*!< DXF 75 */
  BITCODE_BS is_underlined;	/*!< DXF 76 */
  BITCODE_BS bs1;		/*!< DXF 77 */
  BITCODE_BS font;		/*!< DXF 78 */
  BITCODE_BS is_shx;		/*!< DXF 79 */
  BITCODE_BS wizard_flag;	/*!< DXF 280 */
  BITCODE_H arc_handle;		/*!< DXF 330 */
  
  Dwg_Version_Type version = obj->parent->header.version;
  dwg_ent_atext *atext = dwg_object_to_ATEXT (obj);

  CHK_ENTITY_UTF8TEXT (atext, ATEXT, text_value, text_value);
  CHK_ENTITY_UTF8TEXT (atext, ATEXT, text_size, text_size);
  CHK_ENTITY_UTF8TEXT (atext, ATEXT, xscale, xscale);
  CHK_ENTITY_UTF8TEXT (atext, ATEXT, char_spacing, char_spacing);
  CHK_ENTITY_UTF8TEXT (atext, ATEXT, style, style);
  CHK_ENTITY_UTF8TEXT (atext, ATEXT, t2, t2);
  CHK_ENTITY_UTF8TEXT (atext, ATEXT, t3, t3);
  CHK_ENTITY_UTF8TEXT (atext, ATEXT, offset_from_arc, offset_from_arc);
  CHK_ENTITY_UTF8TEXT (atext, ATEXT, right_offset, right_offset);
  CHK_ENTITY_UTF8TEXT (atext, ATEXT, left_offset, left_offset);
  
  CHK_ENTITY_2RD (atext, ATEXT, center, center);
  CHK_ENTITY_3RD (atext, ATEXT, extrusion, extrusion);
  CHK_ENTITY_TYPE (atext, ATEXT, radius, BD, radius);
  CHK_ENTITY_TYPE (atext, ATEXT, start_angle, BD, start_angle);
  CHK_ENTITY_MAX (atext, ATEXT, start_angle, BD, 6.284);
  CHK_ENTITY_TYPE (atext, ATEXT, end_angle, BD, end_angle);
  CHK_ENTITY_MAX (atext, ATEXT, end_angle, BD, 6.284);
  
  CHK_ENTITY_TYPE (atext, ATEXT, color_index, BL, color_index);
  CHK_ENTITY_TYPE (atext, ATEXT, is_reverse, BS, is_reverse);
  CHK_ENTITY_TYPE (atext, ATEXT, text_direction, BS, text_direction);
  CHK_ENTITY_TYPE (atext, ATEXT, alignment, BS, alignment);
  CHK_ENTITY_TYPE (atext, ATEXT, text_position, BS, text_position);
  CHK_ENTITY_TYPE (atext, ATEXT, font_19, BS, font_19);
  CHK_ENTITY_TYPE (atext, ATEXT, bs2, BS, bs2);
  CHK_ENTITY_TYPE (atext, ATEXT, is_underlined, BS, is_underlined);
  CHK_ENTITY_TYPE (atext, ATEXT, bs1, BS, bs1);
  CHK_ENTITY_TYPE (atext, ATEXT, font, BS, font);
  CHK_ENTITY_TYPE (atext, ATEXT, is_shx, BS, is_shx);
  CHK_ENTITY_TYPE (atext, ATEXT, wizard_flag, BS, wizard_flag);
  
  CHK_ENTITY_TYPE (atext, ATEXT, alignment, BS, alignment);
  CHK_ENTITY_UTF8TEXT (atext, ATEXT, style, style);
}
