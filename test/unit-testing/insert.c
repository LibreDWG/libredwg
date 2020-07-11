#define DWG_TYPE DWG_TYPE_INSERT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  dwg_point_3d ins_pt, extrusion, scale;
  double rotation;
  BITCODE_BB scale_flag;
  BITCODE_B has_attribs;
  BITCODE_BL num_owned;
  BITCODE_H block_header, first_attrib, last_attrib, seqend, *attribs;

  Dwg_Version_Type version = obj->parent->header.version;
  dwg_ent_insert *insert = dwg_object_to_INSERT (obj);

  CHK_ENTITY_3RD_W_OLD (insert, INSERT, ins_pt);
  CHK_ENTITY_3RD_W_OLD (insert, INSERT, scale);
  CHK_ENTITY_TYPE_W_OLD (insert, INSERT, rotation, BD);
  CHK_ENTITY_MAX (insert, INSERT, rotation, BD, MAX_ANGLE);
  CHK_ENTITY_3RD_W_OLD (insert, INSERT, extrusion);
  CHK_ENTITY_TYPE (insert, INSERT, scale_flag, BB);
  CHK_ENTITY_TYPE (insert, INSERT, has_attribs, B);
  if (version >= R_2004)
    {
      CHK_ENTITY_TYPE_W_OLD (insert, INSERT, num_owned, BL);
    }

  CHK_ENTITY_H (insert, INSERT, block_header);
  if (insert->has_attribs)
    {
      if (version <= R_2000)
        {
          CHK_ENTITY_H (insert, INSERT, first_attrib);
          CHK_ENTITY_H (insert, INSERT, last_attrib);
        }
      if (version >= R_2004)
        {
          CHK_ENTITY_HV (insert, INSERT, attribs, num_owned);
        }
      CHK_ENTITY_H (insert, INSERT, seqend);
    }
}
