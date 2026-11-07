#define DWG_TYPE DWG_TYPE_ACIDBLOCKREFERENCE
#include "tests_common.c"

/* Inventor's AcIdBlockReference. An INSERT with its own class name. */
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
  Dwg_Entity_ACIDBLOCKREFERENCE *acid = dwg_object_to_ACIDBLOCKREFERENCE (obj);

  CHK_ENTITY_3RD (acid, ACIDBLOCKREFERENCE, ins_pt);
  CHK_ENTITY_3RD (acid, ACIDBLOCKREFERENCE, scale);
  CHK_ENTITY_TYPE (acid, ACIDBLOCKREFERENCE, rotation, BD);
  CHK_ENTITY_MAX (acid, ACIDBLOCKREFERENCE, rotation, BD, MAX_ANGLE);
  CHK_ENTITY_3RD (acid, ACIDBLOCKREFERENCE, extrusion);
  CHK_ENTITY_TYPE (acid, ACIDBLOCKREFERENCE, scale_flag, BB);
  CHK_ENTITY_TYPE (acid, ACIDBLOCKREFERENCE, has_attribs, B);
  if (version >= R_2004)
    {
      CHK_ENTITY_TYPE (acid, ACIDBLOCKREFERENCE, num_owned, BL);
    }

  CHK_ENTITY_H (acid, ACIDBLOCKREFERENCE, block_header);
  if (acid->has_attribs)
    {
      if (version <= R_2000)
        {
          CHK_ENTITY_H (acid, ACIDBLOCKREFERENCE, first_attrib);
          CHK_ENTITY_H (acid, ACIDBLOCKREFERENCE, last_attrib);
        }
      if (version >= R_2004)
        {
          CHK_ENTITY_HV (acid, ACIDBLOCKREFERENCE, attribs, num_owned);
        }
      CHK_ENTITY_H (acid, ACIDBLOCKREFERENCE, seqend);
    }
}
