// TODO coverage
#define DWG_TYPE DWG_TYPE_PERSUBENTMGR
#include "tests_common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL class_version;
  BITCODE_BL unknown_0;
  BITCODE_BL unknown_2;
  BITCODE_BL unknown_bl1;
  BITCODE_BL i, j, num_records;
  BITCODE_BL num_history_ids;
  BITCODE_BL *history_ids;

  dwg_obj_persubentmgr *_obj = dwg_object_to_PERSUBENTMGR (obj);

  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, class_version, BL);
  CHK_ENTITY_MAX (_obj, PERSUBENTMGR, class_version, BL, 2);
  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, unknown_0, BL);
  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, unknown_2, BL);
  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, unknown_bl1, BL);
  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, num_records, BL);
  for (i = 0; i < num_records; i++)
    {
      CHK_SUBCLASS_TYPE (_obj->records[i], PERSUBENTMGR_Record, record_type,
                         BL);
      CHK_SUBCLASS_TYPE (_obj->records[i], PERSUBENTMGR_Record, unknown_bl,
                         BL);
      CHK_SUBCLASS_TYPE (_obj->records[i], PERSUBENTMGR_Record,
                         num_history_ids, BL);
      CHK_SUBCLASS_VECTOR_TYPE (_obj->records[i], PERSUBENTMGR_Record,
                                history_ids, _obj->records[i].num_history_ids,
                                BL);
      CHK_SUBCLASS_TYPE (_obj->records[i], PERSUBENTMGR_Record,
                         num_data_values, BL);
      CHK_SUBCLASS_VECTOR_TYPE (_obj->records[i], PERSUBENTMGR_Record,
                                data_values, _obj->records[i].num_data_values,
                                BL);
      if (_obj->records[i].parent != _obj)
        fail ("PERSUBENTMGR.records[%u].parent", i);
    }
  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, num_history_ids, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, PERSUBENTMGR, history_ids, num_history_ids,
                          BL);
}
