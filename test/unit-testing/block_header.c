#define DWG_TYPE DWG_TYPE_BLOCK_HEADER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_RC flag;
  BITCODE_TV name;
  BITCODE_RS used;
  BITCODE_B xrefref;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;

  BITCODE_BL __iterator;
  BITCODE_RC flag2; /* preR13 */
  BITCODE_RS flag3; /* preR13 */
  BITCODE_B anonymous;    /* flag 70 bit 1 */
  BITCODE_B hasattrs;     /* flag 70 bit 2 */
  BITCODE_B blkisxref;    /* flag 70 bit 3 */
  BITCODE_B xrefoverlaid; /* flag 70 bit 4 */
  BITCODE_B loaded_bit;   /* flag 70 bit 6 */
  BITCODE_BL num_owned;
  BITCODE_3DPOINT base_pt;
  BITCODE_TV xref_pname;
  BITCODE_RL i, num_inserts;
  BITCODE_TV description;
  BITCODE_BL preview_size; /* no DXF. BLL? */
  BITCODE_TF preview;      /* DXF 310. Called PreviewIcon */
  BITCODE_BS insert_units;
  BITCODE_B explodable;
  BITCODE_RC block_scaling;
  BITCODE_H null_handle;
  BITCODE_H block_entity;
  BITCODE_H first_entity;
  BITCODE_H last_entity;
  BITCODE_H* entities;
  BITCODE_H endblk_entity;
  BITCODE_H* inserts;
  BITCODE_H layout;

  //Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_block_header *block_header = dwg_object_to_BLOCK_HEADER (obj);

  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, flag, RC, flag);
  CHK_ENTITY_UTF8TEXT_W_OBJ (block_header, BLOCK_HEADER, name, name);
  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, used, RS, used);
  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, xrefref, B, xrefref);
  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, xrefindex_plus1, BS, xrefindex_plus1);
  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, xrefdep, B, xrefdep);

  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, flag2, RC, flag2); /* preR13 */
  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, flag3, RS, flag3); /* preR13 */
  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, anonymous, B, anonymous);    /* flag 70 bit 1 */
  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, hasattrs, B, hasattrs);     /* flag 70 bit 2 */
  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, blkisxref, B, blkisxref);    /* flag 70 bit 3 */
  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, xrefoverlaid, B, xrefoverlaid); /* flag 70 bit 4 */
  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, loaded_bit, B, loaded_bit);   /* flag 70 bit 6 */
  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, num_owned, BL, num_owned);
  CHK_ENTITY_3RD (block_header, BLOCK_HEADER, base_pt, base_pt);
  CHK_ENTITY_UTF8TEXT (block_header, BLOCK_HEADER, xref_pname, xref_pname);
  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, num_inserts, RL, num_inserts);
  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, description, TV, description);
  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, preview_size, BL, preview_size);
  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, preview, TF, preview);
  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, insert_units, BS, insert_units);
  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, explodable, B, explodable);
  CHK_ENTITY_TYPE (block_header, BLOCK_HEADER, block_scaling, RC, block_scaling);
  CHK_ENTITY_UTF8TEXT (block_header, BLOCK_HEADER, description, description);
  CHK_ENTITY_H (block_header, BLOCK_HEADER, null_handle, null_handle);
  CHK_ENTITY_H (block_header, BLOCK_HEADER, block_entity, block_entity);
  CHK_ENTITY_H (block_header, BLOCK_HEADER, first_entity, first_entity);
  CHK_ENTITY_H (block_header, BLOCK_HEADER, last_entity, last_entity);
  if (!dwg_dynapi_entity_value (block_header, "BLOCK_HEADER", "entities", &entities, NULL))
    fail ("BLOCK_HEADER.entities");
  for (i = 0; i < num_owned; i++)
    {
      ok ("BLOCK_HEADER.entities[%d]: " FORMAT_REF, i, ARGS_REF (entities[i]));
    }
  CHK_ENTITY_H (block_header, BLOCK_HEADER, endblk_entity, endblk_entity);
  if (!dwg_dynapi_entity_value (block_header, "BLOCK_HEADER", "inserts", &inserts, NULL))
    fail ("BLOCK_HEADER.inserts");
  for (i = 0; i < num_inserts; i++)
    {
      ok ("BLOCK_HEADER.inserts[%d]: " FORMAT_REF, i, ARGS_REF (inserts[i]));
    }
  CHK_ENTITY_H (block_header, BLOCK_HEADER, layout, layout);
}
