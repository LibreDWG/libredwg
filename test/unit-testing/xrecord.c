#define DWG_TYPE DWG_TYPE_XRECORD
#include "common.c"

void
low_level_process (dwg_object *obj)
{
  unsigned int i;
  dwg_obj_xrecord *xrecord = dwg_object_to_XRECORD (obj);

  printf ("xrecord name: %s\n", obj->dxfname);
  printf ("xrecord num_databytes: " FORMAT_BL "\n", xrecord->num_databytes);
  printf ("xrecord cloning_flags: " FORMAT_BS "\n", xrecord->cloning_flags);
  printf ("xrecord num_xdata: %u\n", xrecord->num_xdata);
  /*
  for (i=0; i<xrecord->num_xdata; i++)
    {
      printf("xrecord eed[%u] code=%d size=%d\n", xrecord->eed[i].code,
  xrecord->eed[i].size);
    }
  */
}

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
  // xdata
  CHK_ENTITY_TYPE (xrecord, XRECORD, num_objid_handles, BL, num_objid_handles);
  // for objid_handles[] H
}
