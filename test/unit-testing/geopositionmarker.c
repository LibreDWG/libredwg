// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_GEOPOSITIONMARKER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version, annotative_data_size;
  BITCODE_3BD position;
  BITCODE_BD radius;
  BITCODE_BD landing_gap;
  BITCODE_T notes;
  BITCODE_RC text_alignment;
  BITCODE_B mtext_visible;
  BITCODE_B enable_frame_text, is_really_locked;
  Dwg_AcDbMTextObjectEmbedded mtext;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_ent_geopositionmarker *_obj = dwg_object_to_GEOPOSITIONMARKER (obj);

  CHK_ENTITY_TYPE (_obj, GEOPOSITIONMARKER, class_version, BS);
  CHK_ENTITY_3RD (_obj, GEOPOSITIONMARKER, position);
  CHK_ENTITY_TYPE (_obj, GEOPOSITIONMARKER, radius, BD);
  CHK_ENTITY_TYPE (_obj, GEOPOSITIONMARKER, landing_gap, BD);
  CHK_ENTITY_TYPE (_obj, GEOPOSITIONMARKER, text_alignment, RC);
  CHK_ENTITY_TYPE (_obj, GEOPOSITIONMARKER, mtext_visible, B);
  CHK_ENTITY_TYPE (_obj, GEOPOSITIONMARKER, enable_frame_text, B);
  CHK_ENTITY_UTF8TEXT (_obj, GEOPOSITIONMARKER, notes);

  CHK_ENTITY_TYPE (_obj, GEOPOSITIONMARKER, annotative_data_size, BS);
  CHK_ENTITY_TYPE (_obj, GEOPOSITIONMARKER, is_really_locked, B);

  if (_obj->enable_frame_text)
    {
      if (!dwg_dynapi_entity_value (_obj, "GEOPOSITIONMARKER", "mtext", &mtext,
                                    NULL))
        fail ("GEOPOSITIONMARKER.mtext");
      else
        {
          CHK_SUBCLASS_TYPE (mtext, AcDbMTextObjectEmbedded, attachment, BS);
          // CHK_SUBCLASS_3RD (mtext, AcDbMTextObjectEmbedded, ins_pt);
          CHK_SUBCLASS_3RD (mtext, AcDbMTextObjectEmbedded, x_axis_dir);
          CHK_SUBCLASS_TYPE (mtext, AcDbMTextObjectEmbedded, rect_height, BD);
          CHK_SUBCLASS_TYPE (mtext, AcDbMTextObjectEmbedded, rect_width, BD);
          CHK_SUBCLASS_TYPE (mtext, AcDbMTextObjectEmbedded, column_type, BS);
          CHK_SUBCLASS_TYPE (mtext, AcDbMTextObjectEmbedded,
                             num_column_heights, BS);
        }
    }
#endif
}
