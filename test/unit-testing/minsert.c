#define DWG_TYPE DWG_TYPE_MINSERT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  dwg_point_3d ins_pt, extrusion, scale;
  double rotation;
  BITCODE_BB scale_flag;
  BITCODE_B has_attribs;
  double col_spacing, row_spacing;
  BITCODE_BL num_owned;
  BITCODE_BS num_rows, num_cols;
  BITCODE_H block_header, first_attrib, last_attrib, seqend, *attribs;

  Dwg_Version_Type version = obj->parent->header.version;
  dwg_ent_minsert *minsert = dwg_object_to_MINSERT (obj);

  CHK_ENTITY_3RD_W_OLD (minsert, MINSERT, ins_pt);
  CHK_ENTITY_3RD_W_OLD (minsert, MINSERT, scale);
  CHK_ENTITY_TYPE (minsert, MINSERT, scale_flag, BB);
  CHK_ENTITY_TYPE_W_OLD (minsert, MINSERT, rotation, BD);
  CHK_ENTITY_MAX (minsert, MINSERT, rotation, BD, MAX_ANGLE);
  CHK_ENTITY_3RD_W_OLD (minsert, MINSERT, extrusion);
  CHK_ENTITY_TYPE (minsert, MINSERT, has_attribs, B);
  CHK_ENTITY_TYPE_W_OLD (minsert, MINSERT, num_owned, BL);
  CHK_ENTITY_TYPE_W_OLD (minsert, MINSERT, num_cols, BS);
  CHK_ENTITY_TYPE_W_OLD (minsert, MINSERT, num_rows, BS);
  CHK_ENTITY_TYPE_W_OLD (minsert, MINSERT, col_spacing, BD);
  CHK_ENTITY_TYPE_W_OLD (minsert, MINSERT, row_spacing, BD);

  CHK_ENTITY_H (minsert, MINSERT, block_header);
  if (minsert->has_attribs)
    {
      if (version <= R_2000)
        {
          CHK_ENTITY_H (minsert, MINSERT, first_attrib);
          CHK_ENTITY_H (minsert, MINSERT, last_attrib);
        }
      if (version >= R_2004)
        {
          CHK_ENTITY_HV (minsert, MINSERT, attribs, num_owned);
        }
      CHK_ENTITY_H (minsert, MINSERT, seqend);
    }
}
