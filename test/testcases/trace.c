#define DWG_TYPE DWG_TYPE_TRACE
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  dwg_ent_trace *trace = dwg_object_to_TRACE (obj);

  printf ("elevation of trace : %f\n", trace->elevation);
  printf ("thickness of trace : %f\n", trace->thickness);
  printf ("extrusion of trace : x = %f, y = %f, z = %f\n",
	  trace->extrusion.x, trace->extrusion.y, trace->extrusion.z);
  printf ("corner1 of trace : x = %f, y = %f\n", trace->corner1.x,
	  trace->corner1.y);
  printf ("corner2 of trace : x = %f, y = %f\n", trace->corner2.x,
	  trace->corner2.y);
  printf ("corner3 of trace : x = %f, y = %f\n", trace->corner3.x,
	  trace->corner3.y);
  printf ("corner4 of trace : x = %f, y = %f\n", trace->corner4.x,
	  trace->corner4.y);
}

void
api_process (dwg_object * obj)
{
  int error;
  BITCODE_BD thickness, elevation;
  dwg_point_3d ext;
  dwg_point_2d c1, c2, c3, c4;
  dwg_ent_trace *trace = dwg_object_to_TRACE (obj);


  elevation = dwg_ent_trace_get_elevation (trace, &error);
  if (!error  && elevation == trace->elevation)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading elevation");


  thickness = dwg_ent_trace_get_thickness (trace, &error);
  if (!error  && thickness == trace->thickness)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading thickness");


  dwg_ent_trace_get_extrusion (trace, &ext, &error);
  if (!error  && ext.x == trace->extrusion.x && ext.y == trace->extrusion.y && ext.z == trace->extrusion.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");


  dwg_ent_trace_get_corner1 (trace, &c1, &error);
  if (!error  && trace->corner1.x == c1.x && trace->corner1.y == c1.y)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading corner 1");


  dwg_ent_trace_get_corner2 (trace, &c2, &error);
  if (!error  && trace->corner2.x == c2.x && trace->corner2.y == c2.y)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading corner 2");


  dwg_ent_trace_get_corner3 (trace, &c3, &error);
  if (!error  && trace->corner3.x == c3.x && trace->corner3.y == c3.y)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading corner 3");


  dwg_ent_trace_get_corner4 (trace, &c4, &error);
  if (!error  && trace->corner4.x == c4.x && trace->corner4.y == c4.y)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading corner 4");


}
