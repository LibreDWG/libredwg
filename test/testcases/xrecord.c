#define DWG_TYPE DWG_TYPE_XRECORD
#include "common.c"
#include <dejagnu.h>

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL i, num_databytes, num_xdata, num_objid_handles;
  BITCODE_BS cloning_flags;
  dwg_object_ref *parent;

  dwg_obj_xrecord *xrecord = dwg_object_to_XRECORD (obj);

  num_databytes = dwg_obj_xrecord_get_num_databytes (xrecord, &error);
  if (!error && num_databytes == xrecord->num_databytes)
    pass ("Working Properly");
  else
    fail ("error in reading num_databytes");

  cloning_flags = dwg_obj_xrecord_get_cloning_flags (xrecord, &error);
  if (!error && cloning_flags == xrecord->cloning_flags)
    pass ("Working Properly");
  else
    fail ("error in reading cloning_flags");

  num_xdata = dwg_obj_xrecord_get_num_xdata (xrecord, &error);
  if (!error && num_xdata == xrecord->num_xdata)
    pass ("Working Properly");
  else
    fail ("error in reading num_xdata");

#if 0
  for (i=0; i<num_xdata; i++)
    {
      Dwg_Eed *eed = dwg_obj_xrecord_get_eed (xrecord, i, &error);
      if (!error && eed[0] == xrecord->eed[i])
        pass ("Working Properly");
      else
        fail ("error in reading eed[%u]", i);
    }
#else
  {
    Dwg_Resbuf *xdata = dwg_obj_xrecord_get_xdata (xrecord, &error);
    if (!error && xdata == xrecord->xdata)
      pass ("Working Properly");
    else
      fail ("error in reading xdata");
  }
#endif

  num_objid_handles = dwg_obj_xrecord_get_num_objid_handles (xrecord, &error);
  if (!error && num_objid_handles == xrecord->num_objid_handles)
    pass ("Working Properly");
  else
    fail ("error in reading num_objid_handles");

  parent = dwg_obj_xrecord_get_ownerhandle (xrecord, &error);
  if (!error && parent == xrecord->ownerhandle)
    pass ("Working Properly");
  else
    fail ("error in reading ownerhandle");
}
