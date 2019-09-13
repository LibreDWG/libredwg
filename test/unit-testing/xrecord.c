#define DWG_TYPE DWG_TYPE_XRECORD
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL num_databytes;
  BITCODE_BS cloning_flags;
  BITCODE_BL num_xdata;
  Dwg_Resbuf *xdata;
  BITCODE_H parent;
  BITCODE_BL num_objid_handles;
  BITCODE_H *objid_handles;

  dwg_obj_xrecord *xrecord = dwg_object_to_XRECORD (obj);

  CHK_ENTITY_TYPE (xrecord, XRECORD, num_databytes, BL, num_databytes);
  CHK_ENTITY_TYPE (xrecord, XRECORD, cloning_flags, BS, cloning_flags);
  CHK_ENTITY_TYPE (xrecord, XRECORD, num_xdata, BL, num_xdata);
  if (!dwg_dynapi_entity_value (xrecord, "XRECORD", "xdata", &xdata, NULL))
    fail ("dwg_dynapi_entity_value");
#if 0
  else
      // TODO: is xdata is still a linked list, not an array
      for (BITCODE_BL i=0; i<num_xdata; i++)
        {
          if (xdata[i] && xrecord->xdata[i] &&
              !memcmp(&xdata[i], &xrecord->xdata[i], sizeof (Dwg_Resbuf)))
            pass ();
          else
            fail ("xrecord->xdata[%d]", i);
        }
#else
  if (xdata == xrecord->xdata)
    pass ();
  else
    fail ("xrecord->xdata[0]");
#endif
  CHK_ENTITY_TYPE (xrecord, XRECORD, num_objid_handles, BL, num_objid_handles);
  CHK_ENTITY_HV (xrecord, XRECORD, objid_handles, objid_handles,
                 num_objid_handles);
}
