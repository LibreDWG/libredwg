#define DWG_TYPE DWG_TYPE_LWPLINE
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  BITCODE_BL i;

  dwg_ent_lwpline *lwpline = dwg_object_to_LWPLINE(obj);

  printf("normal of lwpline : x = %f, y = %f, z = %f\n",
          lwpline->normal.x, lwpline->normal.y, lwpline->normal.z);
  printf("const width of lwpline : %f\n", lwpline->const_width);
  printf("elevation of lwpline : %f\n", lwpline->elevation);
  printf("thickness of lwpline : %f\n", lwpline->thickness);
  printf("num width of lwpline : " FORMAT_BL "\n", lwpline->num_widths);
  printf("num bulges of lwpline : " FORMAT_BL "\n", lwpline->num_bulges);
  printf("num points of lwpline : " FORMAT_BL "\n", lwpline->num_points);
  printf("flag of lwpline : %x\n", lwpline->flags);
  for ( i = 0; i < lwpline->num_bulges; i++ )
    {
      printf("bulge[%d] of lwpline : %f\n", (int)i, lwpline->bulges[i]);
    }
  for ( i = 0; i < lwpline->num_points; i++ )
    {
      printf("point[%d] of lwpline : x =%f\ty = %f\n",
             (int)i, lwpline->points[i].x, lwpline->points[i].y);
    }
  for ( i = 0; i < lwpline->num_widths; i++ )
    {
      printf("widths[%d] of lwpline : x =%f\ty = %f\n",
             (int)i, lwpline->widths[i].start, lwpline->widths[i].end);
    }
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
  dwg_lwpline_widths *width; 

  dwg_ent_lwpline *lwpline = dwg_object_to_LWPLINE(obj);


  dwg_ent_lwpline_get_normal(lwpline, &normal,
                             &error);
  if ( !error )
    {
      printf("normal of lwpline : x = %f, y = %f, z = %f\n",
              normal.x, normal.y, normal.z);
    }
  else
    {
      printf("error in reading normal");
    }


  const_width = dwg_ent_lwpline_get_const_width(lwpline, &error);
  if ( !error )
   {
     printf("const width of lwpline : %f\n", const_width);
   }
  else
   {
     printf("error in reading const width");
   }


  elevation = dwg_ent_lwpline_get_elevation(lwpline, &error);
  if ( !error )
   {
     printf("elevation of lwpline : %f\n", elevation);
   }
  else
   {
     printf("error in reading elevation");
   }


  thickness = dwg_ent_lwpline_get_thickness(lwpline, &error);
  if ( !error )
   {
     printf("thickness of lwpline : %f\n", thickness);
   }
  else
   {
     printf("error in reading thickness");
   }


  num_widths = dwg_ent_lwpline_get_num_widths(lwpline, &error);
  if ( !error )
   {
     printf("num width of lwpline : " FORMAT_BL "\n", num_widths);
   }
  else
   {
     printf("error in reading num width");
   }


  num_bulges = dwg_ent_lwpline_get_num_bulges(lwpline, &error);
  if ( !error )
   {
     printf("num bulges of lwpline : " FORMAT_BL "\n", num_bulges);
   }
  else
   {
     printf("error in reading num bulge");
   }


  num_points = dwg_ent_lwpline_get_num_points(lwpline, &error);
  if ( !error )
   {
     printf("num points of lwpline : " FORMAT_BL "\n", num_points);
   }
  else
   {
     printf("error in reading num points");
   }


  flags = dwg_ent_lwpline_get_flags(lwpline, &error);
  if ( !error )
   {
     printf("flag of lwpline : %x\n", flags);
   }
  else
   {
     printf("error in reading flag");
   }
  

  bulges = dwg_ent_lwpline_get_bulges(lwpline, &error);
  if ( !error )
   {
     unsigned long i;
     for ( i = 0;i < lwpline->num_bulges; i++ )
       {
         printf("bulge[%d] of lwpline : %f\n", (int)i, bulges[i]);
       }
   }
  else
   {
     printf("error in reading bulges \n");
   } 


  points = dwg_ent_lwpline_get_points(lwpline, &error);
  if ( !error )
   {
     unsigned long i;
     for ( i = 0;i < lwpline->num_points ; i++ )
       {
         printf("point[%d] of lwpline : x =%f\ty = %f\n", (int)i, points[i].x, points[i].y);
       }
   }
  else
   {
     printf("error in reading points \n");
   }


  width = dwg_ent_lwpline_get_widths(lwpline, &error);
  if ( !error )
   {
     unsigned long i;
     for ( i = 0;i < lwpline->num_widths ; i++ )
       {
         printf("widths[%d] of lwpline : x =%f\ty = %f\n",
                 (int)i, width[i].start, width[i].end);
       }
   }
  else
   {
     printf("error in reading widths \n");
   } 
}
