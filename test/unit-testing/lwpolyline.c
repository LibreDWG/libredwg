#define DWG_TYPE DWG_TYPE_LWPOLYLINE
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  BITCODE_BL i;

  dwg_ent_lwpolyline *lwpolyline = dwg_object_to_LWPOLYLINE(obj);

  printf("normal of lwpolyline : x = %f, y = %f, z = %f\n",
          lwpolyline->normal.x, lwpolyline->normal.y, lwpolyline->normal.z);
  printf("const width of lwpolyline : %f\n", lwpolyline->const_width);
  printf("elevation of lwpolyline : %f\n", lwpolyline->elevation);
  printf("thickness of lwpolyline : %f\n", lwpolyline->thickness);
  printf("num width of lwpolyline : " FORMAT_BL "\n", lwpolyline->num_widths);
  printf("num bulges of lwpolyline : " FORMAT_BL "\n", lwpolyline->num_bulges);
  printf("num points of lwpolyline : " FORMAT_BL "\n", lwpolyline->num_points);
  printf("flag of lwpolyline : %x\n", lwpolyline->flag);
  for ( i = 0; i < lwpolyline->num_bulges; i++ )
    printf("bulge[%d] of lwpolyline : %f\n", (int)i, lwpolyline->bulges[i]);
  for ( i = 0; i < lwpolyline->num_points; i++ )
    printf("point[%d] of lwpolyline : x =%f\ty = %f\n",
           (int)i, lwpolyline->points[i].x, lwpolyline->points[i].y);
  for ( i = 0; i < lwpolyline->num_widths; i++ )
    printf("widths[%d] of lwpolyline : x =%f\ty = %f\n",
           (int)i, lwpolyline->widths[i].start, lwpolyline->widths[i].end);
}

void
api_process(dwg_object *obj)
{
  int error;
  BITCODE_BL num_points, num_bulges, num_widths;
  dwg_point_3d normal;
  char flags;
  double const_width, elevation, thickness;
  double * bulges;
  dwg_point_2d *points;
  dwg_lwpolyline_widths *width; 

  dwg_ent_lwpolyline *lwpolyline = dwg_object_to_LWPOLYLINE(obj);

  dwg_ent_lwpolyline_get_normal(lwpolyline, &normal, &error);
  if ( !error )
      printf("normal of lwpolyline : x = %f, y = %f, z = %f\n",
              normal.x, normal.y, normal.z);
  else
      printf("error in reading normal");

  const_width = dwg_ent_lwpolyline_get_const_width(lwpolyline, &error);
  if ( !error )
     printf("const width of lwpolyline : %f\n", const_width);
  else
     printf("error in reading const width");

  elevation = dwg_ent_lwpolyline_get_elevation(lwpolyline, &error);
  if ( !error )
     printf("elevation of lwpolyline : %f\n", elevation);
  else
     printf("error in reading elevation");

  thickness = dwg_ent_lwpolyline_get_thickness(lwpolyline, &error);
  if ( !error )
     printf("thickness of lwpolyline : %f\n", thickness);
  else
     printf("error in reading thickness");

  num_widths = dwg_ent_lwpolyline_get_numwidths(lwpolyline, &error);
  if ( !error )
     printf("num width of lwpolyline : " FORMAT_BL "\n", num_widths);
  else
     printf("error in reading num width");

  num_bulges = dwg_ent_lwpolyline_get_numbulges(lwpolyline, &error);
  if ( !error )
     printf("num bulges of lwpolyline : " FORMAT_BL "\n", num_bulges);
  else
     printf("error in reading num bulge");

  num_points = dwg_ent_lwpolyline_get_numpoints(lwpolyline, &error);
  if ( !error )
     printf("num points of lwpolyline : " FORMAT_BL "\n", num_points);
  else
     printf("error in reading num points");

  flags = dwg_ent_lwpolyline_get_flag(lwpolyline, &error);
  if ( !error )
     printf("flag of lwpolyline : %x\n", flags);
  else
     printf("error in reading flag");  

  bulges = dwg_ent_lwpolyline_get_bulges(lwpolyline, &error);
  if ( !error )
   {
     unsigned long i;
     for ( i = 0;i < lwpolyline->num_bulges; i++ )
       printf("bulge[%d] of lwpolyline : %f\n", (int)i, bulges[i]);
   }
  else
   {
     printf("error in reading bulges \n");
   } 


  points = dwg_ent_lwpolyline_get_points(lwpolyline, &error);
  if ( !error )
   {
     unsigned long i;
     for ( i = 0; i < lwpolyline->num_points ; i++ )
       printf("point[%d] of lwpolyline : x =%f\ty = %f\n",
              (int)i, points[i].x, points[i].y);
   }
  else
   {
     printf("error in reading points \n");
   }


  width = dwg_ent_lwpolyline_get_widths(lwpolyline, &error);
  if ( !error )
   {
     unsigned long i;
     for ( i = 0;i < lwpolyline->num_widths ; i++ )
       printf("widths[%d] of lwpolyline : x =%f\ty = %f\n",
              (int)i, width[i].start, width[i].end);
   }
  else
   {
     printf("error in reading widths \n");
   } 
}
