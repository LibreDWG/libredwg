#define DWG_TYPE DWG_TYPE_MINSERT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  dwg_point_3d ins_pt, ext, scale;
  double rot_angle;
  BITCODE_BB scale_flag;
  BITCODE_B has_attribs;
  double col_spacing, row_spacing;
  BITCODE_BL num_owned;
  BITCODE_BS num_rows, num_cols;
  BITCODE_H block_header, first_attrib, last_attrib, seqend, *attrib_handles;

  Dwg_Version_Type version = obj->parent->header.version;
  dwg_ent_minsert *minsert = dwg_object_to_MINSERT (obj);

  CHK_ENTITY_3RD_W_OLD (minsert, MINSERT, ins_pt, ins_pt);
  CHK_ENTITY_3RD_W_OLD (minsert, MINSERT, scale, scale);
  CHK_ENTITY_TYPE (minsert, MINSERT, scale_flag, BB, scale_flag);
  CHK_ENTITY_TYPE_W_OLD (minsert, MINSERT, rotation, BD, rot_angle);
  CHK_ENTITY_3RD_W_OLD (minsert, MINSERT, extrusion, ext);
  CHK_ENTITY_TYPE (minsert, MINSERT, has_attribs, B, has_attribs);
  CHK_ENTITY_TYPE_W_OLD (minsert, MINSERT, num_owned, BL, num_owned);
  CHK_ENTITY_TYPE_W_OLD (minsert, MINSERT, num_cols, BS, num_cols);
  CHK_ENTITY_TYPE_W_OLD (minsert, MINSERT, num_rows, BS, num_rows);
  CHK_ENTITY_TYPE_W_OLD (minsert, MINSERT, col_spacing, BD, col_spacing);
  CHK_ENTITY_TYPE_W_OLD (minsert, MINSERT, row_spacing, BD, row_spacing);

  CHK_ENTITY_H (minsert, MINSERT, block_header, block_header);
  if (minsert->has_attribs)
    {
      if (version <= R_2000)
        {
          CHK_ENTITY_H (minsert, MINSERT, first_attrib, first_attrib);
          CHK_ENTITY_H (minsert, MINSERT, last_attrib, last_attrib);
        }
      if (version >= R_2004)
        {
          CHK_ENTITY_HV (minsert, MINSERT, attrib_handles, attrib_handles,
                         num_owned);
        }
      CHK_ENTITY_H (minsert, MINSERT, seqend, seqend);
    }
}
