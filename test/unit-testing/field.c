#define DWG_TYPE DWG_TYPE_FIELD
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_TV id;         /* 1 */
  BITCODE_TV code;       /* 2,3 */
  BITCODE_BL num_childs; /* 90 */
  BITCODE_H *childs, *hdls;
  BITCODE_BL num_objects;           /* 97 */
  BITCODE_H *objects;               /* code:5, 331 */
  BITCODE_TV format;                /* 4, until r2004 only */
  BITCODE_BL evaluation_option;     /* 91 */
  BITCODE_BL filing_option;         /* 92 */
  BITCODE_BL field_state;           /* 94 */
  BITCODE_BL evaluation_status;     /* 95 */
  BITCODE_BL evaluation_error_code; /* 96 */
  BITCODE_TV evaluation_error_msg;  /* 300 */
  // Dwg_TABLE_value value;
  BITCODE_TV value_string;        /* 301,9 */
  BITCODE_BL value_string_length; /* 98 ODA bug: TV */
  BITCODE_BL i, num_childval;     /* 93 */
  Dwg_FIELD_ChildValue *childval;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_field *_obj = dwg_object_to_FIELD (obj);

  CHK_ENTITY_UTF8TEXT (_obj, FIELD, id);
  CHK_ENTITY_UTF8TEXT (_obj, FIELD, code);
  CHK_ENTITY_TYPE (_obj, FIELD, num_childs, BL);
  if (!dwg_dynapi_entity_value (_obj, "FIELD", "childs", &childs, NULL))
    fail ("FIELD.childs");
  hdls = _obj->childs;
  for (i = 0; i < num_childs; i++)
    {
      if (hdls[i] == childs[i])
        ok ("FIELD.childs[%d]: " FORMAT_REF, i, ARGS_REF (childs[i]));
      else
        fail ("FIELD.childs[%d]: " FORMAT_REF, i, ARGS_REF (childs[i]));
    }
  CHK_ENTITY_TYPE (_obj, FIELD, num_objects, BL);
  if (!dwg_dynapi_entity_value (_obj, "FIELD", "objects", &objects, NULL))
    fail ("FIELD.objects");
  hdls = _obj->objects;
  for (i = 0; i < num_objects; i++)
    {
      if (hdls[i] == objects[i])
        ok ("FIELD.objects[%d]: " FORMAT_REF, i, ARGS_REF (objects[i]));
      else
        fail ("FIELD.objects[%d]: " FORMAT_REF, i, ARGS_REF (objects[i]));
    }
  CHK_ENTITY_UTF8TEXT (_obj, FIELD, format);
  CHK_ENTITY_TYPE (_obj, FIELD, evaluation_option, BL);
  CHK_ENTITY_TYPE (_obj, FIELD, filing_option, BL);
  CHK_ENTITY_TYPE (_obj, FIELD, field_state, BL);
  CHK_ENTITY_TYPE (_obj, FIELD, evaluation_status, BL);
  CHK_ENTITY_TYPE (_obj, FIELD, evaluation_error_code, BL);
  CHK_ENTITY_UTF8TEXT (_obj, FIELD, evaluation_error_msg);
  // Dwg_TABLE_value value;
  CHK_ENTITY_UTF8TEXT (_obj, FIELD, value_string);
  CHK_ENTITY_TYPE (_obj, FIELD, value_string_length, BL);
  CHK_ENTITY_TYPE (_obj, FIELD, num_childval, BL);
  if (!dwg_dynapi_entity_value (_obj, "FIELD", "childval", &childval, NULL))
    fail ("FIELD.childval");
  for (i = 0; i < num_childval; i++)
    {
      CHK_SUBCLASS_UTF8TEXT (childval[i], FIELD_ChildValue, key);
      // embedded value
    }
}
