#include "common.c"

void
output_process(dwg_object *obj);

void
output_object(dwg_object* obj){
  if (!obj)
    {
      printf("object is NULL\n");
      return;
    }

  if (dwg_get_type(obj)== DWG_TYPE_TRACE)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  dwg_ent_trace *trace = dwg_object_to_TRACE(obj);
  printf("elevation of trace : %f\t\n", trace->elevation);
  printf("thickness of trace : %f\t\n", trace->thickness);
  printf("extrusion of trace : x = %f, y = %f, z = %f\t\n",
          trace->extrusion.x, trace->extrusion.y, trace->extrusion.z);
  printf("corner1 of trace : x = %f, y = %f\t\n", trace->corner1.x,
          trace->corner1.y);
  printf("corner2 of trace : x = %f, y = %f\t\n", trace->corner2.x,
          trace->corner2.y);
  printf("corner3 of trace : x = %f, y = %f\t\n", trace->corner3.x,
          trace->corner3.y);
  printf("corner4 of trace : x = %f, y = %f\t\n", trace->corner4.x,
          trace->corner4.y);
}

void
api_process(dwg_object *obj)
{
  int elevation_error, ext_error, thickness_error, c1_error, c2_error, c3_error,
      c4_error;
  float thickness, elevation;
  dwg_point_3d ext;
  dwg_point_2d c1,c2,c3,c4;
  dwg_ent_trace *trace = dwg_object_to_TRACE(obj);

  elevation  = dwg_ent_trace_get_elevation(trace, &elevation_error);
  if(elevation_error == 0 )
    {
      printf("elevation of trace : %f\t\n", elevation);
    }
  else
    {
      printf("error in reading elevation\n");
    }
 
  thickness = dwg_ent_trace_get_thickness(trace, &thickness_error);
  if(thickness_error == 0 )
    {
      printf("thickness of trace : %f\t\n", thickness);
    }
  else
    {
      printf("error in reading thickness\n");
    }

  dwg_ent_trace_get_extrusion(trace, &ext, &ext_error);
  if(ext_error == 0 )
    {
      printf("extrusion of trace : x = %f, y = %f, z = %f\t\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  dwg_ent_trace_get_corner1(trace, &c1, &c1_error);
  if(c1_error == 0 )
    {
      printf("corner 1 of trace : x = %f, y = %f\t\n", c1.x, c1.y);
    }
  else
    {
      printf("error in reading corner 1 \n");
    }

  dwg_ent_trace_get_corner2(trace, &c2, &c2_error);
  if(c2_error == 0 )
    {
      printf("corner 2 of trace : x = %f, y = %f\t\n", c2.x, c2.y);
    }
  else
    {
      printf("error in reading corner 2 \n");
    }

  dwg_ent_trace_get_corner3(trace, &c3, &c3_error);
  if(c3_error == 0 )
    {
      printf("corner 3 of trace : x = %f, y = %f\t\n", c3.x, c3.y);
    }
  else
    {
      printf("error in reading corner 3 \n");
    }

  dwg_ent_trace_get_corner4(trace, &c4, &c4_error);
  if(c4_error == 0 )
    {
      printf("traces of trace : x = %f, y = %f\t\n", c4.x, c4.y);
    }
  else
    {
      printf("error in reading corner 4 \n");
    }

}
