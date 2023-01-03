// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_DATATABLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS flags;
  BITCODE_BL i, num_cols;
  BITCODE_BL num_rows;
  BITCODE_T table_name;
  Dwg_DATATABLE_column *cols;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_datatable *_obj = dwg_object_to_DATATABLE (obj);

  CHK_ENTITY_TYPE (_obj, DATATABLE, flags, BS);
  CHK_ENTITY_TYPE (_obj, DATATABLE, num_cols, BL);
  CHK_ENTITY_TYPE (_obj, DATATABLE, num_rows, BL);
  CHK_ENTITY_UTF8TEXT (_obj, DATATABLE, table_name);
  if (!dwg_dynapi_entity_value (_obj, "DATATABLE", "cols", &cols, NULL))
    fail ("DATATABLE.cells");
  else
    for (i = 0; i < num_cols; i++)
      {
        CHK_SUBCLASS_TYPE (cols[i], DATATABLE_column, type, BL);
        CHK_SUBCLASS_UTF8TEXT (cols[i], DATATABLE_column, text);
        // ..
      }
#endif
}
