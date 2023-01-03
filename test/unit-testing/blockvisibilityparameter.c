// looks stable
#define DWG_TYPE DWG_TYPE_BLOCKVISIBILITYPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCK1PTPARAMETER_fields;
  BITCODE_B is_initialized;
  BITCODE_B unknown_bool; // DXF 91, history_compression, history_required or
                          // is_visible?
  BITCODE_T blockvisi_name;
  BITCODE_T blockvisi_desc;
  BITCODE_BL i, num_blocks;
  BITCODE_H *blocks;
  BITCODE_BL num_states;
  Dwg_BLOCKVISIBILITYPARAMETER_state *states;
  BITCODE_H *params;
  // BITCODE_T cur_state_name;
  // BITCODE_BL cur_state;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_blockvisibilityparameter *_obj
      = dwg_object_to_BLOCKVISIBILITYPARAMETER (obj);

  CHK_EVALEXPR (BLOCKVISIBILITYPARAMETER);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKVISIBILITYPARAMETER, name);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYPARAMETER, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYPARAMETER, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYPARAMETER, eed1071, BL);
  // AcDbBlockParameter
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYPARAMETER, show_properties, B);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYPARAMETER, chain_actions, B);
  // AcDbBlock1PtParameter
  CHK_ENTITY_3RD (_obj, BLOCKVISIBILITYPARAMETER, def_pt);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYPARAMETER, num_propinfos, BL);
  CHK_ENTITY_MAX (_obj, BLOCKVISIBILITYPARAMETER, num_propinfos, BL, 2);
  BLOCKPARAMETER_PropInfo (prop1) BLOCKPARAMETER_PropInfo (prop2)
      // AcDbBlockVisibilityParameter
      CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYPARAMETER, is_initialized, B);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYPARAMETER, unknown_bool, B);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKVISIBILITYPARAMETER, blockvisi_name);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKVISIBILITYPARAMETER, blockvisi_desc);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYPARAMETER, num_blocks, BL);
  CHK_ENTITY_HV (_obj, BLOCKVISIBILITYPARAMETER, blocks, num_blocks);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYPARAMETER, num_states, BL);
  if (!dwg_dynapi_entity_value (_obj, "BLOCKVISIBILITYPARAMETER", "states",
                                &states, NULL))
    fail ("BLOCKVISIBILITYPARAMETER.states");
  else
    for (i = 0; i < num_states; i++)
      {
        CHK_SUBCLASS_UTF8TEXT (states[i], BLOCKVISIBILITYPARAMETER_state,
                               name);
        CHK_SUBCLASS_TYPE (states[i], BLOCKVISIBILITYPARAMETER_state,
                           num_blocks, BL);
        CHK_SUBCLASS_HV (states[i], BLOCKVISIBILITYPARAMETER_state, blocks,
                         states[i].num_blocks);
        CHK_SUBCLASS_TYPE (states[i], BLOCKVISIBILITYPARAMETER_state,
                           num_params, BL);
        CHK_SUBCLASS_HV (states[i], BLOCKVISIBILITYPARAMETER_state, params,
                         states[i].num_params);
      }
  // CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYPARAMETER, cur_state, B);
  // CHK_ENTITY_UTF8TEXT (_obj, BLOCKVISIBILITYPARAMETER, cur_state_name);
}
