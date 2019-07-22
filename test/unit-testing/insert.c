#define DWG_TYPE DWG_TYPE_INSERT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  dwg_point_3d ins_pt, ext, scale;
  double rot_angle;
  BITCODE_BB scale_flag;
  BITCODE_B has_attribs;
  BITCODE_BL num_owned;
  BITCODE_H block_header, first_attrib, last_attrib, seqend, *attrib_handles;

  dwg_ent_insert *insert = dwg_object_to_INSERT (obj);
  Dwg_Version_Type version = obj->parent->header.version;
 
  CHK_ENTITY_3RD_W_OLD (insert, INSERT, ins_pt, ins_pt);
  CHK_ENTITY_3RD_W_OLD (insert, INSERT, scale, scale);
  CHK_ENTITY_TYPE_W_OLD (insert, INSERT, rotation, BD, rot_angle);
  CHK_ENTITY_3RD_W_OLD (insert, INSERT, extrusion, ext);
  CHK_ENTITY_TYPE (insert, INSERT, scale_flag, BB, scale_flag);
  CHK_ENTITY_TYPE (insert, INSERT, has_attribs, B, has_attribs);
  if (version >= R_2004) {
    CHK_ENTITY_TYPE_W_OLD (insert, INSERT, num_owned, BL, num_owned);
  }

  CHK_ENTITY_H (insert, INSERT, block_header, block_header);
  if (insert->has_attribs) {
    if (version <= R_2000) {
      CHK_ENTITY_H (insert, INSERT, first_attrib, first_attrib);
      CHK_ENTITY_H (insert, INSERT, last_attrib, last_attrib);
    }
    if (version >= R_2004 && insert->num_owned) {
      //CHK_ENTITY_H (insert, INSERT, attrib_handles, attrib_handles[0]);
    }
    CHK_ENTITY_H (insert, INSERT, seqend, seqend);
  }
}
