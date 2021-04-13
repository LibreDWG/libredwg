#include "../../src/config.h"
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _NETBSD_SOURCE
#define __BSD_VISIBLE 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/stat.h>
#ifdef __APPLE__
#  define _DARWIN_C_SOURCE /* for DT_DIR */
#endif
#include <dirent.h>
#include "dwg.h"
#define DWG_LOGLEVEL loglevel
#include "../../src/logging.h"
#include "dwg_api.h"
#include "../../src/common.h"
#include "../../src/classes.h"
static int silent;
#include "tests_common.h"

dwg_data g_dwg;
const char *stability;
int g_counter;
#define MAX_COUNTER 6
int g_countmax = MAX_COUNTER;

// in radians. angles are not normalized. max observed: 10.307697 in
// ELLIPSE.end_angle
#define MAX_ANGLE 12.57

// needs Dwg_Version_Type dwg_version = obj->parent->header.version;
#undef PRE
#undef VERSION
#undef VERSIONS
#undef UNTIL
#undef SINCE
#define VERSION(version) if (dwg_version == version)
#define VERSIONS(v1, v2) if (dwg_version >= v1 && dwg_version <= v2)
#define PRE(version) if (dwg_version < version)
#define UNTIL(version) if (dwg_version <= version)
#define SINCE(version) if (dwg_version >= version)
#define LATER_VERSIONS else

/// test a DWG file
int test_code (const char *path, const char *filename, int cov);
int test_code_nodist (const char *path, const char *filename, int cov);

/// test all DWG's in a subdir
int test_subdirs (const char *dirname, int cov);

/// Return the name of a handle
char *handle_name (const Dwg_Data *restrict dwg, Dwg_Object_Ref *restrict hdl);

/// iterate over objects of a block
void output_BLOCK_HEADER (const Dwg_Data *restrict dwg, dwg_object_ref *ref);

/// Check the dwg type and calls output_process
void output_object (dwg_object *obj);

/// blocks to be iterated over
void output_test (dwg_data *dwg);

void output_process (dwg_object *obj);

/// Low Level processing function declaration
void low_level_process (dwg_object *obj);

/// API based processing function declaration
void api_process (dwg_object *obj);

void api_common_entity (dwg_object *obj);

void api_common_object (dwg_object *obj);

/// API based printing function declaration
void print_api (dwg_object *obj);

/// Low level printing function declaration
void print_low_level (dwg_object *obj);

/// Main function
int
main (int argc, char *argv[])
{
  char *input = getenv ("INPUT");
  char *dir = NULL;
  int error = 0;
  int i = 1, cov = 1;
  // #ifdef USE_TRACING
  char *trace = getenv ("LIBREDWG_TRACE");
  char *skip;
  silent = is_make_silent ();
  if (trace)
    loglevel = atoi (trace);
  else
    loglevel = silent ? 0 : 2;
  // #endif
  skip = getenv ("LESS_TESTS");
  if (skip && atoi (skip))
    return 0;

#ifdef DWG_TYPE
  if (is_type_stable (DWG_TYPE))
    stability = "stable";
  else if (is_type_unstable (DWG_TYPE))
    stability = "unstable";
  else if (is_type_debugging (DWG_TYPE))
    stability = "debugging";
  else if (is_type_unhandled (DWG_TYPE))
    stability = "unhandled";
#endif

  if (argc > i)
    {
      // don't warn/error on no coverage. for unit_testing_all.sh
      if (strEQc (argv[i], "-n"))
        {
          cov = 0;
          i++;
        }
      // --all, not stopping at 6
      else if (strEQc (argv[i], "-an") || strEQc (argv[i], "--all"))
        {
          g_countmax = 1000;
          cov = 0;
          i++;
        }
      else if (strEQc (argv[i], "-a"))
        {
          g_countmax = 1000;
          i++;
        }
      // process subdirs
      if (argc > i && *argv[i] != '-')
        dir = argv[i];
    }

  if (!input)
    {
      char **ptr;
      struct stat attrib;
      const char *const files[]
          = { "example_2000.dwg", "example_2004.dwg", "example_2007.dwg",
              "example_2010.dwg", "example_2013.dwg", "example_2018.dwg",
              "example_r14.dwg", "example_r13.dwg",
#ifdef USE_WRITE
              "r11/ACEB10.dwg", "r11/entities-3d.dwg", "r10/entities.dwg",
              "r9/entities.dwg", "r2.10/entities.dwg", "r2.6/entities.dwg",
              "r1.4/entities.dwg",
#endif
              /* TODO
                "example_r11.dwg"    "example_r10.dwg",  "example_r9.dwg",
                "example_r2_10.dwg", "example_r2_6.dwg", "example_r1_4.dwg"
              */
              //"2000/PolyLine2D.dwg",
              //"2007/PolyLine3D.dwg",
              NULL };
      const char *prefix = "../test-data";

      if (dir)
        {
          if (!stat (dir, &attrib) && S_ISDIR (attrib.st_mode))
#ifndef HAVE_SCANDIR
            fprintf (stderr, "dir argument not supported yet on this platform "
                             "(missing scandir)\n");
#else
            return test_subdirs (dir, cov);
#endif
        }

      /* ../configure out-of-tree. find srcdir */
      if (stat (prefix, &attrib))
        prefix = "../../../test/test-data";
      if (stat (prefix, &attrib)) /* dir not found. one more level up */
        prefix = "../../../../test/test-data";

      for (ptr = (char **)&files[0]; *ptr; ptr++)
        {
          if (stat (*ptr, &attrib))
            {
              char tmp[80];
              strncpy (tmp, prefix, sizeof (tmp));
              strncat (tmp, "/", sizeof (prefix) - 1);
              strncat (tmp, *ptr,
                       sizeof (tmp) - sizeof (prefix) - strlen (*ptr) - 1);
              if (stat (tmp, &attrib))
                fprintf (stderr, "Env var INPUT not defined, %s not found\n",
                         tmp);
              else
                error += test_code (prefix, *ptr, cov);
            }
          else
            error += test_code (prefix, *ptr, cov);
        }
      if (DWG_TYPE == DWG_TYPE_ATTDEF)
        {
          error += test_code_nodist (prefix,
                                     "../test-old/2018/from_ACadSharp/"
                                     "AecObjects.dwg",
                                     cov);
        }
      // if (DWG_TYPE == DWG_TYPE_MULTILEADER)
      //  {
      //    error += test_code_nodist (prefix, "../issues/gh518/9.dwg", cov);
      //  }
      // if no coverage
      if (!numpassed () && !numfailed ())
        {
          char tmp[80];
          if (DWG_TYPE == DWG_TYPE_POLYLINE_2D || DWG_TYPE == DWG_TYPE_SEQEND
              || DWG_TYPE == DWG_TYPE_VERTEX_2D
              || DWG_TYPE == DWG_TYPE_CSACDOCUMENTOPTIONS
              || DWG_TYPE == DWG_TYPE_LAYOUTPRINTCONFIG)
            {
              error += test_code (prefix, "2000/PolyLine2D.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_POLYLINE_3D
              || DWG_TYPE == DWG_TYPE_VERTEX_3D)
            {
              error += test_code (prefix, "2000/PolyLine3D.dwg", cov);
              error += test_code (prefix, "2004/PolyLine3D.dwg", cov);
              error += test_code (prefix, "2018/PolyLine3D.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_POLYLINE_MESH
              || DWG_TYPE == DWG_TYPE_VERTEX_MESH || DWG_TYPE == DWG_TYPE_TRACE
              || DWG_TYPE == DWG_TYPE_DIMENSION_ANG3PT
              || DWG_TYPE == DWG_TYPE_DIMENSION_DIAMETER
              || DWG_TYPE == DWG_TYPE_DIMENSION_RADIUS
              || DWG_TYPE == DWG_TYPE_OLE2FRAME)
            {
              error += test_code (prefix, "2000/TS1.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_IMAGE || DWG_TYPE == DWG_TYPE_IMAGEDEF
              || DWG_TYPE == DWG_TYPE_IMAGEDEF_REACTOR
              || DWG_TYPE == DWG_TYPE_LEADER)
            {
              error += test_code (prefix, "2000/Leader.dwg", cov);
              error += test_code (prefix, "2004/Leader.dwg", cov);
              error += test_code (prefix, "2018/Leader.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_HATCH)
            {
              error += test_code (prefix, "2004/HatchG.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_BODY)
            {
              error += test_code_nodist (prefix,
                                         "../test-old/2000/from_cadforum.cz/"
                                         "Transformer_Oil_Filling_Chamber.dwg",
                                         cov);
              error += test_code_nodist (
                  prefix,
                  "../test-old/r14/from_cadforum.cz/plaza_light_dual.dwg",
                  cov);
            }
          if (DWG_TYPE == DWG_TYPE_UCS)
            {
              error += test_code_nodist (prefix, "r14/plaza_light_dual.dwg",
                                         cov);
            }
          if (DWG_TYPE == DWG_TYPE_SHAPE)
            {
              error += test_code_nodist (
                  prefix, "../test-old/r13/from_lx_viewer/small.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_MINSERT)
            {
              error += test_code_nodist (
                  prefix, "../test-old/r13/from_lx_viewer/small.dwg", cov);
              error += test_code_nodist (prefix, "2000/passenger_drone.dwg",
                                         cov);
              error += test_code_nodist (prefix, "2018/MInsert.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_PDFUNDERLAY
              || DWG_TYPE == DWG_TYPE_PDFDEFINITION)
            {
              error += test_code (prefix, "2004/Underlay.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_LIGHT || DWG_TYPE == DWG_TYPE_VISUALSTYLE)
            {
              error += test_code_nodist (prefix, "2004/Visualstyle.dwg", cov);
              error += test_code_nodist (prefix, "2018/Visualstyle.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_GEODATA)
            {
              error += test_code (prefix, "2010/gh209_1.dwg", cov);
              error += test_code_nodist (
                  prefix, "../test-old/2018/from_ACadSharp/geoloc.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_PLOTSETTINGS)
            {
              error += test_code_nodist (prefix, "2013/gh109_1.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_DBCOLOR)
            {
              error += test_code_nodist (prefix, "2004/dbcolor.dwg", cov);
              error += test_code_nodist (prefix, "2004/Truecolor.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_HELIX)
            {
              error += test_code (prefix, "2004/Helix.dwg", cov);
              error += test_code (prefix, "2018/Helix.dwg", cov);
              error += test_code_nodist (prefix, "2000/work.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_ASSOCPLANESURFACEACTIONBODY
              || DWG_TYPE == DWG_TYPE_ASSOCACTION
              || DWG_TYPE == DWG_TYPE_ASSOCDEPENDENCY
              || DWG_TYPE == DWG_TYPE_ASSOCEDGEACTIONPARAM
              || DWG_TYPE == DWG_TYPE_ASSOCEXTRUDEDSURFACEACTIONBODY
              || DWG_TYPE == DWG_TYPE_ASSOCLOFTEDSURFACEACTIONBODY
              || DWG_TYPE == DWG_TYPE_ASSOCPATHACTIONPARAM
              || DWG_TYPE == DWG_TYPE_ASSOCPERSSUBENTMANAGER
              || DWG_TYPE == DWG_TYPE_ASSOCREVOLVEDSURFACEACTIONBODY
              || DWG_TYPE == DWG_TYPE_ASSOCSWEPTSURFACEACTIONBODY
              || DWG_TYPE == DWG_TYPE_EXTRUDEDSURFACE
              || DWG_TYPE == DWG_TYPE_LOFTEDSURFACE
              || DWG_TYPE == DWG_TYPE_NURBSURFACE
              || DWG_TYPE == DWG_TYPE_REVOLVEDSURFACE
              || DWG_TYPE == DWG_TYPE_SWEPTSURFACE
              || DWG_TYPE == DWG_TYPE_MESH)
            {
              error += test_code (prefix, "2004/Surface.dwg", cov);
              error += test_code_nodist (prefix, "2018/Surface.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_PLANESURFACE)
            {
              error += test_code (prefix, "2004/Surface.dwg", cov);
              error += test_code_nodist (
                  prefix, "2010/visualization_-_conference_room.dwg", cov);
              error += test_code_nodist (
                  prefix, "2010/visualization_-_aerial.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_ASSOCNETWORK)
            {
              error += test_code (prefix, "2000/Constraints.dwg", cov);
              error += test_code (prefix, "2004/Constraints.dwg", cov);
              error += test_code (prefix, "2007/Constraints.dwg", cov);
              error += test_code (prefix, "2010/Constraints.dwg", cov);
              error += test_code (prefix, "2013/Constraints.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_BLKREFOBJECTCONTEXTDATA)
            {
              error += test_code_nodist (prefix, "2004/Visualstyle.dwg", cov);
              error += test_code_nodist (prefix, "2018/Visualstyle.dwg", cov);
              error += test_code_nodist (
                  prefix, "2010/ACI_20160321_A_30_east.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_LEADEROBJECTCONTEXTDATA)
            {
              error += test_code (prefix, "2000/Leader.dwg", cov);
              error += test_code (prefix, "2004/Leader.dwg", cov);
              error += test_code (prefix, "2013/gh55-ltype.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_IDBUFFER)
            {
              error += test_code_nodist (prefix, "r14/missing_xref.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_MLEADEROBJECTCONTEXTDATA
              || DWG_TYPE == DWG_TYPE_MTEXTOBJECTCONTEXTDATA
              || DWG_TYPE == DWG_TYPE_TEXTOBJECTCONTEXTDATA)
            {
              error += test_code_nodist (
                  prefix, "2010/ACI_20160321_A_30_east.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_MTEXTATTRIBUTEOBJECTCONTEXTDATA)
            {
              error += test_code (prefix, "2013/gh44-error.dwg", cov);
              error += test_code_nodist (prefix, "2013/gh55-ltype.dwg", cov);
              error += test_code_nodist (
                  prefix, "2010/ACI_20160321_A_30_east.dwg", cov);
              error += test_code_nodist (
                  prefix, "2018/ACI_20160321_A_30_east.dwg", cov);
              error += test_code_nodist (
                  prefix,
                  "../test-old/2018/from_autocad_2021/Annotation_-_Metric.dwg",
                  cov);
              error += test_code_nodist (prefix,
                                         "../test-old/2018/from_autocad_2021/"
                                         "Annotation_-_Imperial.dwg",
                                         cov);
              error += test_code_nodist (
                  prefix,
                  "../test-old/2018/from_autocad_2021/Civil_-_Metric.dwg",
                  cov);
              error += test_code_nodist (
                  prefix,
                  "../test-old/2018/from_autocad_2021/Civil_-_Imperial.dwg",
                  cov);
              error += test_code_nodist (
                  prefix,
                  "../test-old/2018/from_autocad_2021/Electrical_-_Metric.dwg",
                  cov);
              error += test_code_nodist (prefix,
                                         "../test-old/2018/from_autocad_2021/"
                                         "Electrical_-_Imperial.dwg",
                                         cov);
              error += test_code_nodist (
                  prefix,
                  "../test-old/2018/from_autocad_2021/Mechanical_-_Metric.dwg",
                  cov);
              error += test_code_nodist (prefix,
                                         "../test-old/2018/from_autocad_2021/"
                                         "Mechanical_-_Imperial.dwg",
                                         cov);
              error += test_code_nodist (
                  prefix,
                  "../test-old/2018/from_autocad_2021/Structural_-_Metric.dwg",
                  cov);
              error += test_code_nodist (prefix,
                                         "../test-old/2018/from_autocad_2021/"
                                         "Structural_-_Imperial.dwg",
                                         cov);
            }
          if (DWG_TYPE == DWG_TYPE_FIELD || DWG_TYPE == DWG_TYPE_FIELDLIST)
            {
              error += test_code (prefix, "2000/TS1.dwg", cov);
              error += test_code_nodist (prefix, "2010/5151-024.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_DATALINK)
            {
              error += test_code_nodist (prefix, "2010/5151-019.dwg", cov);
              error += test_code_nodist (prefix, "2010/5151-022.dwg", cov);
              error += test_code_nodist (prefix, "2010/5151-023.dwg", cov);
              error += test_code_nodist (prefix, "2010/5151-024.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_DATATABLE)
            {
              error += test_code (prefix, "2000/TS1.dwg", cov);
              error += test_code_nodist (prefix, "r13/TS1.dwg", cov);
              error
                  += test_code_nodist (prefix, "2004/fr05_b101_ref.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_SORTENTSTABLE)
            {
              error += test_code (prefix, "2000/PolyLine2D.dwg", cov);
              error += test_code (prefix, "2004/Surface.dwg", cov);
              error += test_code (prefix, "r13/PolyLine2D.dwg", cov);
              error += test_code (prefix, "2004/Publish.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_SUN)
            {
              error += test_code_nodist (prefix, "2000/2.dwg", cov);
              error += test_code_nodist (prefix, "2000/3.dwg", cov);
              error += test_code_nodist (prefix, "2000/4.dwg", cov);
              error += test_code_nodist (prefix, "2000/5.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_SPATIAL_FILTER)
            {
              error += test_code (prefix, "2013/gh44-error.dwg", cov);
              if (g_countmax == 1000) // only with -a
                error += test_code_nodist (prefix, "../test-big/2007/big.dwg",
                                           cov);
            }
          if (DWG_TYPE == DWG_TYPE_SECTIONOBJECT
              || DWG_TYPE == DWG_TYPE_SECTION_MANAGER
              || DWG_TYPE == DWG_TYPE_SECTION_SETTINGS)
            {
              error += test_code (prefix, "2018/LiveSection1.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_ACSH_BOOLEAN_CLASS
              || DWG_TYPE == DWG_TYPE_ACSH_BOX_CLASS
              || DWG_TYPE == DWG_TYPE_ACSH_BREP_CLASS
              || DWG_TYPE == DWG_TYPE_ACSH_CHAMFER_CLASS
              || DWG_TYPE == DWG_TYPE_ACSH_CYLINDER_CLASS
              || DWG_TYPE == DWG_TYPE_ACSH_EXTRUSION_CLASS
              || DWG_TYPE == DWG_TYPE_ACSH_FILLET_CLASS
              || DWG_TYPE == DWG_TYPE_ACSH_HISTORY_CLASS
              || DWG_TYPE == DWG_TYPE_ACSH_SWEEP_CLASS
              || DWG_TYPE == DWG_TYPE_ACSH_TORUS_CLASS
              || DWG_TYPE == DWG_TYPE_ACSH_WEDGE_CLASS)
            {
              error += test_code (prefix, "2007/ATMOS-DC22S.dwg", cov);
              error += test_code_nodist (prefix, "2013/JW.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_ACSH_CONE_CLASS)
            {
              error += test_code (prefix, "2000/Cone.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_ACSH_LOFT_CLASS
              || DWG_TYPE == DWG_TYPE_ACSH_EXTRUSION_CLASS
              || DWG_TYPE == DWG_TYPE_ACSH_REVOLVE_CLASS
              || DWG_TYPE == DWG_TYPE_ACSH_SWEEP_CLASS
              || DWG_TYPE == DWG_TYPE_SKYLIGHT_BACKGROUND
              || DWG_TYPE == DWG_TYPE_MENTALRAYRENDERSETTINGS
              || DWG_TYPE == DWG_TYPE_RENDERGLOBAL)
            {
              error += test_code_nodist (
                  prefix, "2010/visualization_-_condominium_with_skylight.dwg",
                  cov);
            }
          if (DWG_TYPE == DWG_TYPE_ACSH_PYRAMID_CLASS)
            {
              error += test_code_nodist (prefix, "2000/Pyramid.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_ACSH_SPHERE_CLASS)
            {
              error += test_code_nodist (prefix, "2000/sphere.dwg", cov);
              error += test_code_nodist (
                  prefix, "2010/visualization_-_aerial.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_ASSOCVARIABLE
              || DWG_TYPE == DWG_TYPE_ASSOCVALUEDEPENDENCY
              || DWG_TYPE == DWG_TYPE_ASSOCGEOMDEPENDENCY
              || DWG_TYPE == DWG_TYPE_ASSOC2DCONSTRAINTGROUP
              || DWG_TYPE == DWG_TYPE_DYNAMICBLOCKPROXYNODE
              || DWG_TYPE == DWG_TYPE_BLOCKBASEPOINTPARAMETER
              || DWG_TYPE == DWG_TYPE_BLOCKFLIPACTION
              || DWG_TYPE == DWG_TYPE_BLOCKFLIPGRIP
              || DWG_TYPE == DWG_TYPE_BLOCKFLIPPARAMETER
              || DWG_TYPE == DWG_TYPE_BLOCKLINEARPARAMETER
              || DWG_TYPE == DWG_TYPE_BLOCKLINEARGRIP
              || DWG_TYPE == DWG_TYPE_BLOCKLOOKUPGRIP
              || DWG_TYPE == DWG_TYPE_BLOCKREPRESENTATION
              || DWG_TYPE == DWG_TYPE_BLOCKROTATEACTION
              || DWG_TYPE == DWG_TYPE_BLOCKROTATIONPARAMETER
              || DWG_TYPE == DWG_TYPE_BLOCKROTATIONGRIP
              || DWG_TYPE == DWG_TYPE_ASSOCDIMDEPENDENCYBODY)
            {
              error += test_code (prefix, "2018/Dynblocks.dwg", cov);
              error += test_code_nodist (prefix, "2004/Dynblocks.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_ASSOCARRAYACTIONBODY
              || DWG_TYPE == DWG_TYPE_ASSOCDIMDEPENDENCYBODY)
            {
              error
                  += test_code_nodist (prefix, "2004/planetarygear.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_ASSOC3POINTANGULARDIMACTIONBODY)
            {
              error += test_code_nodist (prefix,
                                         "../test-old/2013/from_cadforum.cz/"
                                         "TwistRibbnConstThick_2.dwg",
                                         cov);
            }
          if (DWG_TYPE == DWG_TYPE_BLOCKSCALEACTION
              || DWG_TYPE == DWG_TYPE_BLOCKMOVEACTION
              || DWG_TYPE == DWG_TYPE_BLOCKSTRETCHACTION)
            {
              error += test_code (prefix, "2018/Dynblocks.dwg", cov);
              error += test_code_nodist (prefix, "2010/sun_and_sky_demo.dwg",
                                         cov);
            }
          if (DWG_TYPE == DWG_TYPE_BLOCKARRAYACTION)
            {
              error += test_code_nodist (
                  prefix, "../test-old/2004/from_uloz.to/Klánovice_RD_A.dwg",
                  cov);
            }
          if (DWG_TYPE == DWG_TYPE_LOAD)
            {
#ifdef USE_WRITE
              error += test_code (prefix, "r1.4/entities.dwg", cov);
#endif
            }
          if (DWG_TYPE == DWG_TYPE_DGNDEFINITION)
            {
              error += test_code_nodist (
                  prefix,
                  "../test-old/2004/from_uloz.to/Klánovice_RD_A_situace.dwg",
                  cov);
            }
          if (DWG_TYPE == DWG_TYPE_BLOCKPOLARGRIP
              || DWG_TYPE == DWG_TYPE_BLOCKPOLARPARAMETER
              || DWG_TYPE == DWG_TYPE_BLOCKPOLARSTRETCHACTION)
            {
              error += test_code_nodist (
                  prefix,
                  "../test-old/2018/from_autocad_2021/Annotation_-_Metric.dwg",
                  cov);
              error += test_code_nodist (prefix,
                                         "../test-old/2018/from_autocad_2021/"
                                         "Annotation_-_Imperial.dwg",
                                         cov);
            }
          if (DWG_TYPE == DWG_TYPE_BLOCKVISIBILITYPARAMETER
              || DWG_TYPE == DWG_TYPE_BLOCKVISIBILITYGRIP
              || DWG_TYPE == DWG_TYPE_BLOCKGRIPLOCATIONCOMPONENT)
            {
              error += test_code (prefix, "2018/Dynblocks.dwg", cov);
              error += test_code (prefix, "2013/gh44-error.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_BLOCKHORIZONTALCONSTRAINTPARAMETER
              || DWG_TYPE == DWG_TYPE_BLOCKVERTICALCONSTRAINTPARAMETER
              || DWG_TYPE == DWG_TYPE_BLOCKPARAMDEPENDENCYBODY)
            // missing BLOCKRADIALCONSTRAINTPARAMETER
            // BLOCKALIGNEDCONSTRAINTPARAMETER
            //   BLOCKDIAMETRICCONSTRAINTPARAMETER
            //   BLOCKLINEARCONSTRAINTPARAMETER BLOCKANGULARCONSTRAINTPARAMETER
            {
              error += test_code_nodist (
                  prefix,
                  "../test-old/2000/1/Ashraf_Basic_File-1_Feet_input_2.dwg",
                  cov);
            }
          if (DWG_TYPE == DWG_TYPE_BLOCKALIGNMENTPARAMETER
              || DWG_TYPE == DWG_TYPE_BLOCKALIGNMENTGRIP)
            {
              error += test_code (prefix, "2018/Dynblocks.dwg", cov);
              error += test_code_nodist (prefix, "2013/flipped.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_DYNAMICBLOCKPURGEPREVENTER)
            {
              error += test_code_nodist (prefix, "2013/gh55-ltype.dwg", cov);
              error += test_code_nodist (prefix,
                                         "2007/anchor_dynamic_block.dwg", cov);
              error += test_code_nodist (prefix, "2010/sun_and_sky_demo.dwg",
                                         cov);
            }
          if (DWG_TYPE == DWG_TYPE_EVALUATION_GRAPH
              || DWG_TYPE == DWG_TYPE_BLOCKGRIPLOCATIONCOMPONENT
              || DWG_TYPE == DWG_TYPE_BLOCKPOINTPARAMETER
              || DWG_TYPE == DWG_TYPE_ALDIMOBJECTCONTEXTDATA
              || DWG_TYPE == DWG_TYPE_MTEXTOBJECTCONTEXTDATA)
            {
              error += test_code (prefix, "2013/gh44-error.dwg", cov);
              error += test_code_nodist (
                  prefix, "2007/blocks_and_tables_-_metric.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_BLOCKLOOKUPACTION
              || DWG_TYPE == DWG_TYPE_BLOCKLOOKUPPARAMETER
              || DWG_TYPE == DWG_TYPE_BLOCKLOOKUPGRIP
              || DWG_TYPE == DWG_TYPE_BLOCKXYPARAMETER)
            {
              error += test_code_nodist (prefix, "2010/sun_and_sky_demo.dwg",
                                         cov);
            }
          if (DWG_TYPE == DWG_TYPE_BLOCKXYGRIP)
            {
              error += test_code_nodist (prefix, "2010/sun_and_sky_demo.dwg",
                                         cov);
              error += test_code_nodist (
                  prefix, "2007/blocks_and_tables_-_metric.dwg", cov);
              error += test_code_nodist (
                  prefix, "2007/blocks_and_tables_-_imperial.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_VBA_PROJECT
              || DWG_TYPE == DWG_TYPE_PROXY_OBJECT
              || DWG_TYPE == DWG_TYPE_PROXY_ENTITY)
            {
              error += test_code_nodist (
                  prefix, // but here in section not in object.
                  "../test-old/2013/from_upcommons.upc.edu/DRAWINGS.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_SOLID_BACKGROUND)
            {
              error += test_code_nodist (prefix, "../issues/gh695/2.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_LARGE_RADIAL_DIMENSION)
            {
              error += test_code_nodist (
                  prefix, "../test-old/2018/from_ezdxf/uncommon.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_RAPIDRTRENDERSETTINGS)
            {
              error += test_code (prefix, "2013/gh44-error.dwg", cov);
              error += test_code (prefix, "2013/gh109_1.dwg", cov);
              error += test_code_nodist (prefix, "2013/JW.dwg", cov);
              error += test_code_nodist (prefix, "2013/gh55-ltype.dwg", cov);
              error += test_code_nodist (prefix, "2013/stelprdb1144445.dwg",
                                         cov);
              error += test_code_nodist (prefix, "2013/nrcs141p2_034463.dwg",
                                         cov);
              error += test_code_nodist (prefix, "2018/redwg1.dwg", cov);
              if (g_countmax == 1000) // only with -a
                error += test_code_nodist (
                    prefix, "../test-big/2004/double_free_example.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_RENDERENVIRONMENT)
            {
              error += test_code_nodist (
                  prefix, "../test-old/2007/from_uloz.to/VBK_MODEL1.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_RENDERENTRY)
            {
              error += test_code_nodist (prefix, "2007/ATMOS-DC22S.dwg", cov);
              error += test_code_nodist (
                  prefix, "2010/visualization_-_conference_room.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_POINTCLOUDCOLORMAP)
            {
              if (g_countmax == 1000) // only with -a
                error += test_code_nodist (
                    prefix, "../test-big/2004/double_free_example.dwg", cov);
            }
          if (DWG_TYPE == DWG_TYPE_RASTERVARIABLES)
            {
              error += test_code_nodist (prefix, "2000/passenger_drone.dwg",
                                         cov);
              error += test_code_nodist (prefix, "2004/PLANO_MASSANASSA.dwg",
                                         cov);
              error += test_code_nodist (prefix, "2007/SALLE_DES_MACHINES.dwg",
                                         cov);
              error += test_code_nodist (prefix, "2013/nrcs141p2_034463.dwg",
                                         cov);
            }
          if (DWG_TYPE == DWG_TYPE_SPATIAL_INDEX)
            {
              error += test_code_nodist (
                  prefix,
                  "../test-old/2004/from_uloz.to/00_005_POHLADY_Kl_A.dwg",
                  cov);
              error += test_code_nodist (
                  prefix, "../test-old/2018/from_ACadSharp/AecObjects.dwg",
                  cov);
            }
          if (DWG_TYPE == DWG_TYPE_OBJECT_PTR)
            {
              error += test_code_nodist (
                  prefix, "../test-old/r13c3/from_autocad_r14/asesmp.dwg",
                  cov);
            }
          if (DWG_TYPE == DWG_TYPE_ACMECOMMANDHISTORY
              || DWG_TYPE == DWG_TYPE_ACMESCOPE
              || DWG_TYPE == DWG_TYPE_ACMESTATEMGR)
            {
              error += test_code_nodist (prefix, "2013/JW.dwg", cov);
              error += test_code_nodist (
                  prefix,
                  "../test-old/2010/AutoCAD_Mechanical_2019/Wheel_casing.dwg",
                  cov);
              error += test_code_nodist (
                  prefix, "../test-old/2010/AutoCAD_Mechanical_2019/tray.dwg",
                  cov);
              error += test_code_nodist (
                  prefix,
                  "../test-old/2010/AutoCAD_Mechanical_2019/"
                  "Trolley_Structure.dwg",
                  cov);
              error += test_code_nodist (
                  prefix,
                  "../test-old/2010/AutoCAD_Mechanical_2019/"
                  "robot_handling_cell.dwg",
                  cov);
              error += test_code_nodist (
                  prefix,
                  "../test-old/2010/AutoCAD_Mechanical_2019/Pump_wheel.dwg",
                  cov);
              error += test_code_nodist (
                  prefix,
                  "../test-old/2010/AutoCAD_Mechanical_2019/Pump_cover.dwg",
                  cov);
              error += test_code_nodist (
                  prefix,
                  "../test-old/2010/AutoCAD_Mechanical_2019/LEVER_DETAIL.dwg",
                  cov);
              error += test_code_nodist (
                  prefix,
                  "../test-old/2010/AutoCAD_Mechanical_2019/GRIPPER.dwg", cov);
              error += test_code_nodist (
                  prefix,
                  "../test-old/2010/AutoCAD_Mechanical_2019/"
                  "GRIPPER_ASSEMBLY_NEW.dwg",
                  cov);
              error += test_code_nodist (
                  prefix,
                  "../test-old/2010/AutoCAD_Mechanical_2019/"
                  "Gear_Pump_Subassy.dwg",
                  cov);
              error += test_code_nodist (
                  prefix,
                  "../test-old/2010/AutoCAD_Mechanical_2019/Drive_shaft.dwg",
                  cov);
              error += test_code_nodist (
                  prefix,
                  "../test-old/2010/AutoCAD_Mechanical_2019/Bottom_plate.dwg",
                  cov);
              error += test_code_nodist (
                  prefix, "../test-old/2013/from_uloz.to/model-mechanical.dwg",
                  cov);
              error += test_code_nodist (
                  prefix, "../test-old/2010/from_cadforum.cz/AMSTLSHAP2D.dwg",
                  cov);
            }
#ifdef DEBUG_CLASSES
          if (DWG_TYPE == DWG_TYPE_ABSHDRAWINGSETTING
              || DWG_TYPE == DWG_TYPE_AEC_VARS_DWG_SETUP
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_CONFIG
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_SET
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_2D_SECTION
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_ANCHOR_BUB_TO_GRID
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_ANCHOR_BUB_TO_GRID_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_ANCHOR_ENT_TO_NODE
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_ANCHOR_TAG_TO_ENT
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_BDG_ELEVLINE_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_BDG_SECTIONLINE_PLAN
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_BDG_SECTIONLINE_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_BDG_SECTION_SUBDIV
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_CEILING_GRID
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_CEILING_GRID_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_CEILING_GRID_RCP
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_CLIP_VOLUME_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_CLIP_VOLUME_PLAN
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_CLIP_VOLUME_RESULT_SUBDIV
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_CLIP_VOLUME_RESULT
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_COLUMN_GRID
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_COLUMN_GRID_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_CURTAIN_WALL_LAYOUT_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_CURTAIN_WALL_LAYOUT_PLAN
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_CURTAIN_WALL_UNIT_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_CURTAIN_WALL_UNIT_PLAN
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_DIM_GROUP_PLAN
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_DISPLAYTHEME
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_DOOR_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_DOOR_NOMINAL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_DOOR_PLAN
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_EDITINPLACEPROFILE
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_ENT_REF
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_LAYOUT_CURVE
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_LAYOUT_GRID2D
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_LAYOUT_GRID3D
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_MASKBLOCK_REF
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_MASKBLOCK_REF_RCP
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_MASS_ELEM_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_MASS_ELEM_SCHEM
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_MASS_GROUP_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_MASS_GROUP_PLAN
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_MATERIAL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_MVBLOCK_REF
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_MVBLOCK_REF_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_OPENING
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_OPENING_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_POLYGON_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_RAILING_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_RAILING_PLAN
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_ROOF_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_ROOF_PLAN
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_ROOFSLAB_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_ROOFSLAB_PLAN
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_SCHEDULE_TABLE
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_SLAB_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_SLAB_PLAN
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_SLICE
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_SPACE_DECOMPOSED
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_SPACE_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_STAIR_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_STAIR_PLAN
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_WALL_GRAPH
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_WALL_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_WALL_PLAN
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_WALL_SCHEM
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_WINDOW_ASSEMBLY_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_WINDOW_ASSEMBLY_PLAN
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPWINDOWASSEMBLYPLAN100
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPBDGELEVLINEPLAN100
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPBDGELEVLINEPLAN50
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPBDGSECTIONLINEPLAN100
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPBDGSECTIONLINEPLAN50
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPCEILINGGRIDPLAN100
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPCEILINGGRIDPLAN50
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPCOLUMNGRIDPLAN100
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPCOLUMNGRIDPLAN50
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN100
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN50
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPCURTAINWALLUNITPLAN100
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPCURTAINWALLUNITPLAN50
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPMVBLOCKREFPLAN100
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPMVBLOCKREFPLAN50
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPROOFPLAN100
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPROOFPLAN50
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPROOFSLABPLAN100
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPROOFSLABPLAN50
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPSLABPLAN100
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPSLABPLAN50
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPSPACEPLAN100
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPSPACEPLAN50
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPWALLPLAN100
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPWALLPLAN50
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPWINDOWASSEMBLYPLAN100
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPWINDOWASSEMBLYPLAN50
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPZONE100
              || DWG_TYPE == DWG_TYPE_AEC_AECDBDISPREPZONE50
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_WINDOW_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_WINDOW_NOMINAL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_WINDOW_PLAN
              || DWG_TYPE == DWG_TYPE_AEC_DISP_REP_ZONE
              || DWG_TYPE == DWG_TYPE_AECS_DISP_REP_MEMBER_LOGICAL
              || DWG_TYPE == DWG_TYPE_AECS_DISP_REP_MEMBER_MODEL_DESIGN
              || DWG_TYPE == DWG_TYPE_AECS_DISP_REP_MEMBER_PLAN_DESIGN
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_ENT
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_LAYOUT_GRID2D
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_CLIP_VOLUME_RESULT
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_GRID_ASSEMBLY_PLAN
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_GRID_ASSEMBLY_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_CLIP_VOLUME
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_ROOFSLAB
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_POLYGON_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_OPENING
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_SLICE
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_DOOR
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_DISPLAYTHEME
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_SLAB
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_ROOF
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_SPACE_DECOMPOSED
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_RAILING_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_RAILING_PLAN
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_WINDOW
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_ZONE
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_SPACE_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_WALL_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_WALL_PLAN
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_WALL_SCHEM
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_MASKBLOCK
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_STAIR_PLAN
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_STAIR_PLAN_OVERLAPPING
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_EDITINPLACEPROFILE_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_MATERIAL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_MEMBER
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_MEMBER_LOGICAL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_DIM
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_MASS_ELEM_MODEL
              || DWG_TYPE == DWG_TYPE_AEC_DISP_PROPS_LAYOUT_CURVE
              || DWG_TYPE == DWG_TYPE_AEC_VARS_ARCHBASE
              || DWG_TYPE == DWG_TYPE_AEC_VARS_DWG_SETUP
              || DWG_TYPE == DWG_TYPE_AEC_VARS_MUNICH
              || DWG_TYPE == DWG_TYPE_BINRECORD || 0)
            {
              error += test_code_nodist (
                  prefix,
                  "../test-old/2004/from_uloz.to/00_005_POHLADY_Kl_A.dwg",
                  cov);
            }
#endif
          if (DWG_TYPE == DWG_TYPE_ASSOCBLENDSURFACEACTIONBODY
              || DWG_TYPE == DWG_TYPE_ASSOCEXTENDSURFACEACTIONBODY
              || DWG_TYPE == DWG_TYPE_ASSOCFILLETSURFACEACTIONBODY
              || DWG_TYPE == DWG_TYPE_ASSOCNETWORKSURFACEACTIONBODY
              || DWG_TYPE == DWG_TYPE_ASSOCOFFSETSURFACEACTIONBODY
              || DWG_TYPE == DWG_TYPE_ASSOCPATCHSURFACEACTIONBODY
              || DWG_TYPE == DWG_TYPE_ASSOCTRIMSURFACEACTIONBODY
              || DWG_TYPE == DWG_TYPE_IBL_BACKGROUND
              || DWG_TYPE == DWG_TYPE_IMAGE_BACKGROUND
              || DWG_TYPE == DWG_TYPE_GRADIENT_BACKGROUND
              || DWG_TYPE == DWG_TYPE_GROUND_PLANE_BACKGROUND
              || DWG_TYPE == DWG_TYPE_CAMERA || DWG_TYPE == DWG_TYPE_DUMMY
              || DWG_TYPE == DWG_TYPE_INDEX
              || DWG_TYPE == DWG_TYPE_LARGE_RADIAL_DIMENSION
              || DWG_TYPE == DWG_TYPE_LAYER_INDEX
              || DWG_TYPE == DWG_TYPE_LAYERFILTER
              || DWG_TYPE == DWG_TYPE_LIGHTLIST
              || DWG_TYPE == DWG_TYPE_LONG_TRANSACTION
              || DWG_TYPE == DWG_TYPE_OLEFRAME
              // || DWG_TYPE == DWG_TYPE_PROXY_ENTITY
              // || DWG_TYPE == DWG_TYPE_PROXY_OBJECT
              || DWG_TYPE == DWG_TYPE_RENDERSETTINGS
              || DWG_TYPE == DWG_TYPE_DGNUNDERLAY
              || DWG_TYPE == DWG_TYPE_DWFUNDERLAY
              || DWG_TYPE == DWG_TYPE_DWFDEFINITION
              || DWG_TYPE == DWG_TYPE_ANGDIMOBJECTCONTEXTDATA
              || DWG_TYPE == DWG_TYPE_ANNOTSCALEOBJECTCONTEXTDATA
              || DWG_TYPE == DWG_TYPE_ARCALIGNEDTEXT
              || DWG_TYPE == DWG_TYPE_ASSOCMLEADERACTIONBODY
              || DWG_TYPE == DWG_TYPE_ASSOCARRAYMODIFYACTIONBODY
              || DWG_TYPE == DWG_TYPE_ASSOCEDGECHAMFERACTIONBODY
              || DWG_TYPE == DWG_TYPE_ASSOCEDGEFILLETACTIONBODY
              || DWG_TYPE == DWG_TYPE_ASSOCRESTOREENTITYSTATEACTIONBODY
              || DWG_TYPE == DWG_TYPE_ASSOCORDINATEDIMACTIONBODY
              || DWG_TYPE == DWG_TYPE_ASSOCROTATEDDIMACTIONBODY
              || DWG_TYPE == DWG_TYPE_ASSOCACTIONPARAM
              || DWG_TYPE == DWG_TYPE_ASSOCASMBODYACTIONPARAM
              || DWG_TYPE == DWG_TYPE_ASSOCCOMPOUNDACTIONPARAM
              || DWG_TYPE == DWG_TYPE_ASSOCFACEACTIONPARAM
              || DWG_TYPE == DWG_TYPE_ASSOCOBJECTACTIONPARAM
              || DWG_TYPE == DWG_TYPE_ASSOCPOINTREFACTIONPARAM
              || DWG_TYPE == DWG_TYPE_ASSOCARRAYMODIFYPARAMETERS
              || DWG_TYPE == DWG_TYPE_ASSOCARRAYPATHPARAMETERS
              || DWG_TYPE == DWG_TYPE_ASSOCARRAYPOLARPARAMETERS
              || DWG_TYPE == DWG_TYPE_ASSOCARRAYRECTANGULARPARAMETERS
              || DWG_TYPE == DWG_TYPE_CONTEXTDATAMANAGER
              || DWG_TYPE == DWG_TYPE_CURVEPATH
              || DWG_TYPE == DWG_TYPE_DMDIMOBJECTCONTEXTDATA
              || DWG_TYPE == DWG_TYPE_FCFOBJECTCONTEXTDATA
              || DWG_TYPE == DWG_TYPE_GEOMAPIMAGE
              || DWG_TYPE == DWG_TYPE_GEOPOSITIONMARKER
              || DWG_TYPE == DWG_TYPE_MOTIONPATH
              || DWG_TYPE == DWG_TYPE_MPOLYGON
              || DWG_TYPE == DWG_TYPE_NAVISWORKSMODEL
              || DWG_TYPE == DWG_TYPE_NAVISWORKSMODELDEF
              || DWG_TYPE == DWG_TYPE_ORDDIMOBJECTCONTEXTDATA
              || DWG_TYPE == DWG_TYPE_PARTIAL_VIEWING_INDEX
              || DWG_TYPE == DWG_TYPE_POINTCLOUD
              || DWG_TYPE == DWG_TYPE_POINTCLOUDEX
              || DWG_TYPE == DWG_TYPE_POINTCLOUDDEF
              || DWG_TYPE == DWG_TYPE_POINTCLOUDDEFEX
              || DWG_TYPE == DWG_TYPE_POINTCLOUDDEF_REACTOR
              || DWG_TYPE == DWG_TYPE_POINTCLOUDDEF_REACTOR_EX
              || DWG_TYPE == DWG_TYPE_POINTPATH
              || DWG_TYPE == DWG_TYPE_POLARGRIPENTITY
              || DWG_TYPE == DWG_TYPE_RADIMOBJECTCONTEXTDATA
              || DWG_TYPE == DWG_TYPE_RADIMLGOBJECTCONTEXTDATA
              || DWG_TYPE == DWG_TYPE_RTEXT || DWG_TYPE == DWG_TYPE_SUNSTUDY
              || DWG_TYPE == DWG_TYPE_TVDEVICEPROPERTIES
              || DWG_TYPE == DWG_TYPE_VISIBILITYPARAMETERENTITY
              || DWG_TYPE == DWG_TYPE_VISIBILITYGRIPENTITY
              || DWG_TYPE == DWG_TYPE_ALIGNMENTPARAMETERENTITY
              || DWG_TYPE == DWG_TYPE_BASEPOINTPARAMETERENTITY
              || DWG_TYPE == DWG_TYPE_BLOCKALIGNEDCONSTRAINTPARAMETER
              || DWG_TYPE == DWG_TYPE_BLOCKANGULARCONSTRAINTPARAMETER
              || DWG_TYPE == DWG_TYPE_BLOCKDIAMETRICCONSTRAINTPARAMETER
              || DWG_TYPE == DWG_TYPE_BLOCKLINEARCONSTRAINTPARAMETER
              || DWG_TYPE == DWG_TYPE_BLOCKPROPERTIESTABLE
              || DWG_TYPE == DWG_TYPE_BLOCKPROPERTIESTABLEGRIP
              || DWG_TYPE == DWG_TYPE_BLOCKRADIALCONSTRAINTPARAMETER
              || DWG_TYPE == DWG_TYPE_BLOCKUSERPARAMETER
              || DWG_TYPE == DWG_TYPE_FLIPPARAMETERENTITY
              || DWG_TYPE == DWG_TYPE_LINEARPARAMETERENTITY
              || DWG_TYPE == DWG_TYPE_POINTPARAMETERENTITY
              || DWG_TYPE == DWG_TYPE_ROTATIONPARAMETERENTITY
              || DWG_TYPE == DWG_TYPE_XYPARAMETERENTITY)
            {
              ; // missing
            }
        }
#ifdef DWG_TYPE
      if (cov && !numpassed () && !numfailed ())
        printf ("TODO no coverage for %s\n", dwg_type_name (DWG_TYPE));
#endif
    }
  else
    error = test_code (NULL, input, cov);

  return error;
}

/// test all DWG's in all subdirs. not on windows yet
int
test_subdirs (const char *dir, int cov)
{
  int error = 0, n;
  struct stat attrib;
  struct dirent **namelist;

#ifdef HAVE_SCANDIR
  n = scandir (dir, &namelist, NULL, NULL);
  if (n < 0)
    {
      perror ("scandir");
      exit (1);
    }
  while (n--)
    {
      char *elem = namelist[n]->d_name;

      if (namelist[n]->d_type == DT_DIR && *elem != '.')
        {
          char path[256];
          path[255] = '\0';
          strncpy (path, dir, 254);
          strncat (path, "/", 254);
          strncat (path, elem, 254);
          path[255] = '\0';

          error += test_subdirs (path, cov);
        }
      if (namelist[n]->d_type == DT_REG
          && (strstr (elem, ".dwg") || strstr (elem, ".DWG")))
        {
          error += test_code (dir, elem, cov);
        }

      FREE (namelist[n]);
    }
  FREE (namelist);
#endif
  return error;
}

#ifdef IS_RELEASE
/* Do not test not distributed DWG's.
   This would skip existing files, but still report FAIL.
*/
int
test_code_nodist (const char *dir, const char *filename, int cov)
{
  return 0;
}
#else
int
test_code_nodist (const char *dir, const char *filename, int cov)
{
  return test_code (dir, filename, cov);
}
#endif

/// test a DWG file
int
test_code (const char *dir, const char *filename, int cov)
{
  int error;
  char path[256];
  struct stat attrib;

  path[255] = '\0';
  if (dir)
    {
      strncpy (path, dir, 254);
      strncat (path, "/", 254);
      strncat (path, filename, 254);
    }
  else
    strncpy (path, filename, 254);
  path[255] = '\0';

#ifdef DWG_TYPE
  // only process if no coverage yet, or when we are crossing the unicode
  // boundary.
  if (strstr (path, "2018") || strstr (path, "2007")
      || (!numpassed () && !numfailed ()))
    {
      if (cov)
        LOG_INFO ("Testing with %s:\n", path)
    }
  else if (cov)
    {
      LOG_INFO ("Skipping %s:\n", path)
      return 0;
    }
  if (stat (path, &attrib))
    {
      LOG_INFO ("file not found:\n")
      return 0;
    }
#endif
  error = dwg_read_file (path, &g_dwg);
  if (error < DWG_ERR_CRITICAL)
    output_test (&g_dwg);
  dwg_free (&g_dwg);
  /* This value is the return value for `main',
     so clamp it to either 0 or 1.  */
  error = (error >= DWG_ERR_CRITICAL || numfailed () > 0) ? 1 : 0;
#ifdef DWG_TYPE
  if (is_type_unstable (DWG_TYPE) || is_type_debugging (DWG_TYPE))
    {
      if (cov && error)
        LOG_INFO ("%s failed (TODO: unstable)\n", path);
      return 0;
    }
  else // some exceptions, because we dont want to publish all our test-cases.
       // test-data is already too big.
    if (DWG_TYPE == DWG_TYPE_IDBUFFER || DWG_TYPE == DWG_TYPE_ACSH_SPHERE_CLASS
        || DWG_TYPE == DWG_TYPE_BLOCKGRIPLOCATIONCOMPONENT
        || DWG_TYPE == DWG_TYPE_BLOCKBASEPOINTPARAMETER)
      {
        if (cov && error)
          LOG_INFO ("%s failed (TODO: skipped)\n", path);
        return 0;
      }
#endif
  if (cov && error)
    LOG_WARN ("%s failed", path);
  return error;
}

/// Iterate over the objects in a block
void
output_BLOCK_HEADER (const Dwg_Data *restrict dwg, dwg_object_ref *ref)
{
  dwg_object *hdr, *obj;
  int error;
  g_counter = 0;

  if (!ref)
    return;
  hdr = dwg_ref_get_object (ref, &error);
  if (!hdr)
    {
      hdr = dwg_resolve_handle (dwg, ref->absolute_ref);
      return;
    }

  obj = get_first_owned_entity (hdr);
  while (obj)
    {
      output_object (obj);
      if (dwg_obj_has_subentity (obj))
        {
          Dwg_Object *owner = obj;
          obj = get_first_owned_subentity (owner);
          while (obj)
            {
              output_object (obj);
              obj = get_next_owned_subentity (owner, obj);
            }
          obj = get_next_owned_entity (hdr, owner);
        }
      else
        obj = get_next_owned_entity (hdr, obj);
    }
}

/// Iterate over both modelspace and paperspace blocks
/// And if nothing found, over all other blocks.
void
output_test (dwg_data *dwg)
{
  int error;
  dwg_object *obj;
  dwg_obj_block_header *_hdr;
  dwg_obj_block_control *_ctrl;
  dwg_object_ref *ref, **hdr_refs;

  if (!dwg)
    return;
#ifdef USE_DEPRECATED_API
  dwg_api_init_version (dwg);
#endif
  _hdr = dwg_get_block_header (dwg, &error);
  if (!_hdr || error)
    return;
  _ctrl = dwg_block_header_get_block_control (_hdr, &error);
  if (!_ctrl || error)
    return;

#ifndef DWG_TYPE
  /* process all owned entities */
  ref = dwg_obj_block_control_get_model_space (_ctrl, &error);
  if (!error)
    {
      LOG_INFO ("mspace\n");
      output_BLOCK_HEADER (dwg, ref);
    }
  ref = dwg_obj_block_control_get_paper_space (_ctrl, &error);
  if (!error)
    {
      LOG_INFO ("pspace\n");
      output_BLOCK_HEADER (dwg, ref);
    }
#endif

#ifdef DWG_TYPE
  if (DWG_TYPE == DWG_TYPE_BLOCK_CONTROL)
    {
      for (unsigned i = 0; i < dwg->num_objects; i++)
        {
          obj = &dwg->object[i];
          LOG_TRACE ("  %s [%d]\n", obj->name, obj->index);
          if (obj->fixedtype == DWG_TYPE)
            {
              g_counter++;
              LOG_INFO ("  %s [%d] (%s)\n", obj->name, obj->index, stability);
              output_process (obj);
            }
        }
    }
  else
    {
      obj = &dwg->object[0];
      while ((obj = dwg_next_object (obj)))
        {
          LOG_TRACE ("  %s [%d]\n", obj->name, obj->index);
          // printf ("%s [%d]\n", obj->name, obj->index);
          if (obj->fixedtype == DWG_TYPE)
            {
              g_counter++;
              LOG_INFO ("  %s [%d] (%s)\n", obj->name, obj->index, stability);
              output_process (obj);
            }
        }
    }
  /* also process blocks? we better find DWGs with these */
  if (!numpassed () && !numfailed ())
    {
      /* and now also all subtypes and entities in blocks */
      unsigned int j;
      unsigned int num_hdr_objs
          = dwg_obj_block_control_get_num_entries (_ctrl, &error);
      if (error || !num_hdr_objs)
        return;
      hdr_refs = dwg_obj_block_control_get_block_headers (_ctrl, &error);
      if (error)
        return;
      for (j = 0; j < num_hdr_objs; j++)
        {
          if (hdr_refs[j])
            output_BLOCK_HEADER (dwg, hdr_refs[j]);
        }
      FREE (hdr_refs);
    }
#endif
}

/// Main output function that prints to the terminal
void
output_process (dwg_object *obj)
{
  // print_low_level (obj);
  print_api (obj);
}

#ifdef DWG_TYPE
void
output_object (dwg_object *obj)
{
  if (!obj)
    {
      LOG_INFO ("object is NULL\n");
      return;
    }
  LOG_TRACE ("  %s [%d]\n", obj->name, obj->index);
  if (obj->fixedtype == DWG_TYPE)
    {
      g_counter++;
      output_process (obj);
    }
}
#endif

/// Low level printing function (ignored)
void
print_low_level (dwg_object *obj)
{
  printf ("\n");
  printf ("Printed via low level access:\n");
  // low_level_process (obj);
  printf ("\n");
}

/// API based printing function
void
print_api (dwg_object *obj)
{
#ifdef DWG_TYPE
  LOG_INFO ("Unit-testing type %d %s [%d] (%s):\n", DWG_TYPE, obj->name,
            g_counter, stability);
#else
  LOG_INFO ("Test dwg_api and dynapi [%d]:\n", g_counter);
#endif
  api_process (obj);

  if (obj->supertype == DWG_SUPERTYPE_ENTITY
      && obj->fixedtype != DWG_TYPE_UNKNOWN_ENT)
    api_common_entity (obj);
  else if (obj->supertype == DWG_SUPERTYPE_OBJECT
           && obj->fixedtype != DWG_TYPE_UNKNOWN_OBJ)
    api_common_object (obj);
  if (g_counter <= g_countmax)
    LOG_INFO ("\n");
}

#define CHK_COMMON_TYPE(ent, field, type)                                     \
  {                                                                           \
    if (dwg_dynapi_common_value (ent, #field, &field, NULL))                  \
      {                                                                       \
        if (field == ent->parent->field)                                      \
          pass (); /*(#field ":\t" FORMAT_##type, field);*/                   \
        else                                                                  \
          fail (#field ":\t" FORMAT_##type, field);                           \
      }                                                                       \
    else                                                                      \
      fail (#field);                                                          \
  }
#define CHK_COMMON_TYPE2(ent, field, type, value)                             \
  {                                                                           \
    if (dwg_dynapi_common_value (ent, #field, &value, NULL))                  \
      {                                                                       \
        if (value == ent->parent->field)                                      \
          pass (); /*(#field ":\t" FORMAT_##type, value);*/                   \
        else                                                                  \
          fail (#field ":\t" FORMAT_##type, value);                           \
      }                                                                       \
    else                                                                      \
      fail (#field);                                                          \
  }

#define CHK_COMMON_H(ent, field)                                              \
  {                                                                           \
    if (!dwg_dynapi_common_value (ent, #field, &field, NULL))                 \
      fail (#field);                                                          \
    else if (!field)                                                          \
      pass ();                                                                \
    else                                                                      \
      {                                                                       \
        int alloced = 0;                                                      \
        char *_hdlname = dwg_dynapi_handle_name (dwg, field, &alloced);       \
        if (field == (BITCODE_H)ent->parent->field)                           \
          {                                                                   \
            if (g_counter > g_countmax)                                       \
              pass ();                                                        \
            else                                                              \
              ok (#field ": %s " FORMAT_REF, _hdlname ? _hdlname : "",        \
                  ARGS_REF (field));                                          \
          }                                                                   \
        else                                                                  \
          fail (#field ": %s " FORMAT_REF, _hdlname ? _hdlname : "",          \
                ARGS_REF (field));                                            \
        if (alloced)                                                          \
          FREE (_hdlname);                                                    \
      }                                                                       \
  }

#define CHK_COMMON_HV(ent, field, num)                                        \
  if (!dwg_dynapi_common_value (ent, #field, &field, NULL))                   \
    fail (#field);                                                            \
  else if (!field)                                                            \
    pass ();                                                                  \
  else                                                                        \
    {                                                                         \
      for (int _i = 0; _i < (int)(num); _i++)                                 \
        {                                                                     \
          BITCODE_H _hdl = field[_i];                                         \
          int alloced = 0;                                                    \
          char *_hdlname                                                      \
              = _hdl ? dwg_dynapi_handle_name (dwg, _hdl, &alloced) : NULL;   \
          if (_hdl == ent->parent->field[_i])                                 \
            {                                                                 \
              if (g_counter > g_countmax)                                     \
                pass ();                                                      \
              else                                                            \
                {                                                             \
                  if (_hdl)                                                   \
                    ok (#field "[%d]: %s " FORMAT_REF, _i, _hdlname ?: "",    \
                        ARGS_REF (_hdl));                                     \
                  else                                                        \
                    ok (#field "[%d]: NULL", _i);                             \
                }                                                             \
            }                                                                 \
          else                                                                \
            {                                                                 \
              if (_hdl)                                                       \
                fail (#field "[%d]: %s " FORMAT_REF, _i, _hdlname ?: "",      \
                      ARGS_REF (_hdl));                                       \
              else                                                            \
                {                                                             \
                  if (g_counter > g_countmax)                                 \
                    pass ();                                                  \
                  else                                                        \
                    ok (#field "[%d]: NULL", _i);                             \
                }                                                             \
            }                                                                 \
          if (alloced)                                                        \
            FREE (_hdlname);                                                  \
        }                                                                     \
    }

void
api_common_entity (dwg_object *obj)
{
  BITCODE_BB entmode;
  BITCODE_H ownerhandle;
  BITCODE_BL num_reactors, num_eed;
  BITCODE_H *reactors;
  BITCODE_B is_xdic_missing, has_ds_data, preview_exists;
  BITCODE_RC linewt;
  BITCODE_RL preview_size_rl;
  BITCODE_BLL preview_size;
  BITCODE_BD ltype_scale;
  BITCODE_BS invisible;
  BITCODE_H xdicobjhandle;
  BITCODE_H prev_entity;
  BITCODE_H next_entity;
  BITCODE_H layer;
  BITCODE_H ltype;
  BITCODE_H material;
  BITCODE_H shadow;
  BITCODE_H plotstyle;
  BITCODE_H full_visualstyle;
  BITCODE_H face_visualstyle;
  BITCODE_H edge_visualstyle;
  Dwg_Data *dwg = obj->parent;
  Dwg_Version_Type version = obj->parent->header.version;
  Dwg_Object_Entity *_ent = obj->tio.entity;
  Dwg_Entity_LINE *ent = obj->tio.entity->tio.LINE;

  CHK_COMMON_TYPE (ent, entmode, BB)
  CHK_COMMON_TYPE (ent, preview_exists, B)
  if (preview_exists)
    {
      if (version >= R_2010)
        CHK_COMMON_TYPE (ent, preview_size, BLL)
      else
        CHK_COMMON_TYPE2 (ent, preview_size, RL, preview_size_rl);
    }
  if (entmode == 3 || entmode == 0)
    CHK_COMMON_H (ent, ownerhandle);
  CHK_COMMON_H (ent, layer);
  if (version < R_2000)
    {
      if (_ent->isbylayerlt)
        CHK_COMMON_H (ent, ltype);
      if (!_ent->nolinks)
        {
          CHK_COMMON_H (ent, prev_entity);
          CHK_COMMON_H (ent, next_entity);
        }
    }
  else
    {
      if (_ent->ltype_flags == 3)
        CHK_COMMON_H (ent, ltype);
    }
  CHK_COMMON_TYPE (ent, linewt, RC);
  CHK_COMMON_TYPE (ent, ltype_scale, BD);
  if (version >= R_2007)
    {
      if (_ent->material_flags == 3)
        CHK_COMMON_H (ent, material);
      if (_ent->shadow_flags == 3)
        CHK_COMMON_H (ent, shadow);
    }
  if (version >= R_2000)
    {
      if (_ent->plotstyle_flags == 3)
        CHK_COMMON_H (ent, plotstyle);
    }
  if (version >= R_2010)
    {
      if (_ent->has_full_visualstyle)
        CHK_COMMON_H (ent, full_visualstyle);
      if (_ent->has_face_visualstyle)
        CHK_COMMON_H (ent, face_visualstyle);
      if (_ent->has_edge_visualstyle)
        CHK_COMMON_H (ent, edge_visualstyle);
    }
  CHK_COMMON_TYPE (ent, invisible, BS);

  CHK_COMMON_TYPE (ent, is_xdic_missing, B);
  if (!is_xdic_missing)
    CHK_COMMON_H (ent, xdicobjhandle);
  CHK_COMMON_TYPE (ent, num_reactors, BL);
  CHK_COMMON_HV (ent, reactors, num_reactors);
  CHK_COMMON_TYPE (ent, has_ds_data, B);
  CHK_COMMON_TYPE (ent, num_eed, BL);
}

#define _CHK_ENTITY_UTF8TEXT(ent, name, field)                                \
  if (dwg_dynapi_entity_utf8text (ent, #name, #field, &field, &isnew, NULL))  \
    {                                                                         \
      if (g_counter > g_countmax)                                             \
        pass ();                                                              \
      else                                                                    \
        ok (#name "." #field ":\t\"%s\"", field);                             \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      Dwg_Version_Type _dwg_version = ent->parent->dwg->header.version;       \
      if (_dwg_version < R_2007)                                              \
        fail (#name "." #field ":\t\"%s\"", field);                           \
      else                                                                    \
        fail (#name "." #field ":\t\"%s\"...", field);                        \
    }

#define CHK_ENTITY_UTF8TEXT(ent, name, field)                                 \
  _CHK_ENTITY_UTF8TEXT (ent, name, field);                                    \
  if (isnew)                                                                  \
  FREE (field)

#define CHK_ENTITY_TYPE(ent, name, field, typ)                                \
  if (!dwg_dynapi_entity_value (ent, #name, #field, &field, NULL))            \
    fail (#name "." #field);                                                  \
  else                                                                        \
    {                                                                         \
      if (field == ent->field)                                                \
        {                                                                     \
          if (g_counter > g_countmax)                                         \
            pass ();                                                          \
          else                                                                \
            ok (#name "." #field ":\t" FORMAT_##typ, field);                  \
        }                                                                     \
      else                                                                    \
        fail (#name "." #field ":\t" FORMAT_##typ " [" #typ "]", field);      \
    }
#define CHK_ENTITY_TYPE_CAST(ent, name, field, typ, cast)                     \
  {                                                                           \
    BITCODE_##cast _value;                                                    \
    if (!dwg_dynapi_entity_value (ent, #name, #field, &field, NULL))          \
      fail (#name "." #field);                                                \
    else                                                                      \
      {                                                                       \
        if (field == ent->field)                                              \
          {                                                                   \
            if (g_counter > g_countmax)                                       \
              pass ();                                                        \
            else                                                              \
              ok (#name "." #field ":\t" FORMAT_##typ, (BITCODE_##typ)field); \
          }                                                                   \
        else                                                                  \
          fail (#name "." #field ":\t" FORMAT_##typ " [" #typ "]",            \
                (BITCODE_##typ)field);                                        \
      }                                                                       \
  }

#define CHK_ENTITY_CMC(ent, name, field)                                      \
  if (!dwg_dynapi_entity_value (ent, #name, #field, &field, NULL))            \
    fail (#name "." #field);                                                  \
  else                                                                        \
    {                                                                         \
      if (memcmp (&field, &ent->field, sizeof (Dwg_Color)) == 0)              \
        {                                                                     \
          if (g_counter > g_countmax)                                         \
            pass ();                                                          \
          else                                                                \
            ok (#name "." #field ":\t" FORMAT_BSd, field.index);              \
        }                                                                     \
      else                                                                    \
        fail (#name "." #field ":\t" FORMAT_BSd " [CMC]", field.index);       \
    }

#define CHK_ENTITY_H(ent, name, field)                                        \
  {                                                                           \
    if (!dwg_dynapi_entity_value (ent, #name, #field, &field, NULL))          \
      fail (#name "." #field);                                                \
    else if (!field)                                                          \
      pass ();                                                                \
    else                                                                      \
      {                                                                       \
        int alloced = 0;                                                      \
        char *_hdlname                                                        \
            = dwg_dynapi_handle_name (obj->parent, field, &alloced);          \
        if (field == ent->field)                                              \
          {                                                                   \
            if (g_counter > g_countmax)                                       \
              pass ();                                                        \
            else                                                              \
              ok (#name "." #field ": %s " FORMAT_REF,                        \
                  _hdlname ? _hdlname : "", ARGS_REF (field));                \
          }                                                                   \
        else                                                                  \
          {                                                                   \
            fail (#name "." #field ": %s " FORMAT_REF,                        \
                  _hdlname ? _hdlname : "", ARGS_REF (field));                \
          }                                                                   \
        if (alloced)                                                          \
          FREE (_hdlname);                                                    \
      }                                                                       \
  }

#define CHK_ENTITY_HV(ent, name, field, num)                                  \
  if (!dwg_dynapi_entity_value (ent, #name, #field, &field, NULL))            \
    fail (#name "." #field);                                                  \
  else if (!field)                                                            \
    pass ();                                                                  \
  else                                                                        \
    {                                                                         \
      for (int _i = 0; _i < (int)(num); _i++)                                 \
        {                                                                     \
          BITCODE_H _hdl = field[_i];                                         \
          int alloced = 0;                                                    \
          char *_hdlname                                                      \
              = _hdl ? dwg_dynapi_handle_name (obj->parent, _hdl, &alloced)   \
                     : NULL;                                                  \
          if (_hdl == ent->field[_i])                                         \
            {                                                                 \
              if (g_counter > g_countmax)                                     \
                pass ();                                                      \
              else if (_hdl)                                                  \
                ok (#field "[%d]: %s " FORMAT_REF, _i, _hdlname ?: "",        \
                    ARGS_REF (_hdl));                                         \
              else                                                            \
                ok (#field "[%d]: NULL", _i);                                 \
            }                                                                 \
          else                                                                \
            {                                                                 \
              if (_hdl)                                                       \
                fail (#field "[%d]: %s " FORMAT_REF, _i, _hdlname ?: "",      \
                      ARGS_REF (_hdl));                                       \
              else                                                            \
                {                                                             \
                  if (g_counter > g_countmax)                                 \
                    pass ();                                                  \
                  else                                                        \
                    ok (#field "[%d]: NULL", _i);                             \
                }                                                             \
            }                                                                 \
          if (alloced)                                                        \
            FREE (_hdlname);                                                  \
        }                                                                     \
    }

// i must be defined as type of num
#define CHK_ENTITY_VECTOR_TYPE(ent, name, field, num, type)                   \
  if (!dwg_dynapi_entity_value (ent, #name, #field, &field, NULL))            \
    fail (#name "." #field);                                                  \
  else if (!field)                                                            \
    pass ();                                                                  \
  else                                                                        \
    {                                                                         \
      for (i = 0; i < (num); i++)                                             \
        {                                                                     \
          if (ent->field[i] != field[i])                                      \
            fail (#name "." #field "[%d]: " FORMAT_##type, i, field[i]);      \
          else                                                                \
            ok (#name "." #field "[%d]: " FORMAT_##type, i, field[i]);        \
        }                                                                     \
    }

#define CHK_ENTITY_VECTOR(ent, name, field, num)                              \
  if (!dwg_dynapi_entity_value (ent, #name, #field, &field, NULL))            \
    fail (#name "." #field);                                                  \
  else if (!field)                                                            \
    pass ();                                                                  \
  else                                                                        \
    {                                                                         \
      for (i = 0; i < (num); i++)                                             \
        {                                                                     \
          if (memcmp (&ent->field[i], &field[i], sizeof (field[i])))          \
            fail ("%s.%s[%d]:", #name, #field, i);                            \
          else                                                                \
            ok ("%s.%s[%d]:", #name, #field, i);                              \
        }                                                                     \
    }

#define CHK_ENTITY_BINARY(ent, name, field, size)                             \
  if (!dwg_dynapi_entity_value (ent, #name, #field, &field, NULL))            \
    fail (#name "." #field);                                                  \
  else if (!field)                                                            \
    pass ();                                                                  \
  else                                                                        \
    {                                                                         \
      if (memcmp (ent->field, field, size))                                   \
        fail ("%s.%s", #name, #field);                                        \
      else                                                                    \
        ok ("%s.%s", #name, #field);                                          \
    }

// i must be defined as type of num
#define CHK_ENTITY_3DPOINTS(ent, name, field, num)                            \
  if (!dwg_dynapi_entity_value (ent, #name, #field, &field, NULL))            \
    fail (#name "." #field);                                                  \
  else if (!field)                                                            \
    pass ();                                                                  \
  else                                                                        \
    {                                                                         \
      for (i = 0; i < (num); i++)                                             \
        {                                                                     \
          ok ("%s.%s[%d]: (%f, %f, %f)", #name, #field, i, field[i].x,        \
              field[i].y, field[i].z);                                        \
          if (memcmp (&ent->field[i], &field[i], sizeof (field[i])))          \
            fail ("%s.%s[%d]: memcmp", #name, #field, i);                     \
        }                                                                     \
    }
// i must be defined as type of num
#define CHK_ENTITY_2DPOINTS(ent, name, field, num)                            \
  if (!dwg_dynapi_entity_value (ent, #name, #field, &field, NULL))            \
    fail (#name "." #field);                                                  \
  else if (!field)                                                            \
    pass ();                                                                  \
  else                                                                        \
    {                                                                         \
      for (i = 0; i < (num); i++)                                             \
        {                                                                     \
          ok ("%s.%s[%d]: (%f, %f)", #name, #field, i, field[i].x,            \
              field[i].y);                                                    \
          if (memcmp (&ent->field[i], &field[i], sizeof (field[i])))          \
            fail ("%s.%s[%d]: memcmp", #name, #field, i);                     \
        }                                                                     \
    }

#define CHK_ENTITY_2RD(ent, name, field)                                      \
  if (!dwg_dynapi_entity_value (ent, #name, #field, &field, NULL))            \
    fail (#name "." #field);                                                  \
  else                                                                        \
    {                                                                         \
      if (field.x == ent->field.x && field.y == ent->field.y)                 \
        {                                                                     \
          if (g_counter > g_countmax)                                         \
            pass ();                                                          \
          else                                                                \
            ok (#name "." #field ":\t(%f, %f)", field.x, field.y);            \
        }                                                                     \
      else                                                                    \
        fail (#name "." #field ":\t(%f, %f)", field.x, field.y);              \
    }

#define CHK_ENTITY_3RD(ent, name, field)                                      \
  if (!dwg_dynapi_entity_value (ent, #name, #field, &field, NULL))            \
    fail (#name "." #field);                                                  \
  else                                                                        \
    {                                                                         \
      if (field.x == ent->field.x && field.y == ent->field.y                  \
          && field.z == ent->field.z)                                         \
        {                                                                     \
          if (g_counter > g_countmax)                                         \
            pass ();                                                          \
          else                                                                \
            ok (#name "." #field ":\t(%f, %f, %f)", field.x, field.y,         \
                field.z);                                                     \
        }                                                                     \
      else                                                                    \
        fail (#name "." #field ":\t(%f, %f, %f)", field.x, field.y, field.z); \
    }

#define CHK_ENTITY_TIMEBLL(ent, name, field)                                  \
  if (!dwg_dynapi_entity_value (ent, #name, #field, &field, NULL))            \
    fail (#name "." #field);                                                  \
  else                                                                        \
    ok ("%s.%s " FORMAT_BL "." FORMAT_BL, #name, #field, field.days, field.ms);

#define CHK_ENTITY_MAX(ent, name, field, type, _max)                          \
  if (field > _max)                                                           \
  fail ("Invalid " #name "." #field " " FORMAT_##type " > " #_max, field)

#define _DWGAPI_ENT_NAME(name, field) dwg_ent_##name##_get_##field
#define DWGAPI_ENT_NAME(ent, field) _DWGAPI_ENT_NAME (ent, field)
#define _DWGAPI_OBJ_NAME(name, field) dwg_obj_##name##_get_##field
#define DWGAPI_OBJ_NAME(ent, field) _DWGAPI_OBJ_NAME (ent, field)
#define DWGAPI_ENTOBJ_NAME(ent, field)                                        \
  (ent->parent->dwg->object[ent->parent->objid].supertype                     \
           == DWG_SUPERTYPE_OBJECT                                            \
       ? _DWGAPI_OBJ_NAME (ent, field)                                        \
       : _DWGAPI_ENT_NAME (ent, field))

#ifndef USE_DEPRECATED_API
#  define CHK_ENTITY_UTF8TEXT_W_OLD(ent, name, field)                         \
    CHK_ENTITY_UTF8TEXT (ent, name, field)
#  define CHK_ENTITY_UTF8TEXT_W_OBJ(ent, name, field)                         \
    CHK_ENTITY_UTF8TEXT (ent, name, field)
#else
#  define CHK_ENTITY_UTF8TEXT_W_OLD(ent, name, field)                         \
    _CHK_ENTITY_UTF8TEXT (ent, name, field);                                  \
    {                                                                         \
      Dwg_Version_Type _dwg_version = ent->parent->dwg->header.version;       \
      if (ent->field)                                                         \
        {                                                                     \
          char *old = DWGAPI_ENT_NAME (ent, field) (ent, &error);             \
          if (error || (old && strcmp (old, field)))                          \
            fail ("old API dwg_ent_" #ent "_get_" #field ": \"%s\"", old);    \
          if (_dwg_version >= R_2007)                                         \
            FREE (old);                                                       \
        }                                                                     \
      if (isnew)                                                              \
        FREE (field);                                                         \
    }
#  define CHK_ENTITY_UTF8TEXT_W_OBJ(ent, name, field)                         \
    _CHK_ENTITY_UTF8TEXT (ent, name, field);                                  \
    {                                                                         \
      Dwg_Version_Type _dwg_version = ent->parent->dwg->header.version;       \
      if (ent->field)                                                         \
        {                                                                     \
          char *old = DWGAPI_OBJ_NAME (ent, field) (ent, &error);             \
          if (error || (old && strcmp (old, field)))                          \
            fail ("old API dwg_obj_" #ent "_get_" #field ": \"%s\"", old);    \
          else                                                                \
            pass ();                                                          \
          if (_dwg_version >= R_2007)                                         \
            FREE (old);                                                       \
        }                                                                     \
      if (isnew)                                                              \
        FREE (field);                                                         \
    }
#endif

#ifndef USE_DEPRECATED_API
#  define CHK_ENTITY_TYPE_W_OLD(ent, name, field, type)                       \
    CHK_ENTITY_TYPE (ent, name, field, type)
#  define CHK_ENTITY_TYPE_W_OBJ(ent, name, field, type)                       \
    CHK_ENTITY_TYPE (ent, name, field, type)
#else
#  define CHK_ENTITY_TYPE_W_OLD(ent, name, field, type)                       \
    {                                                                         \
      BITCODE_##type old;                                                     \
      CHK_ENTITY_TYPE (ent, name, field, type);                               \
      old = DWGAPI_ENT_NAME (ent, field) (ent, &error);                       \
      if (error || old != field)                                              \
        fail ("old API dwg_ent_" #ent "_get_" #field ": " FORMAT_##type       \
              " != " FORMAT_##type,                                           \
              old, field);                                                    \
      else                                                                    \
        pass ();                                                              \
    }

#  define CHK_ENTITY_TYPE_W_OBJ(ent, name, field, type)                       \
    {                                                                         \
      BITCODE_##type old;                                                     \
      CHK_ENTITY_TYPE (ent, name, field, type);                               \
      old = DWGAPI_OBJ_NAME (ent, field) (ent, &error);                       \
      if (error || old != field)                                              \
        fail ("old API dwg_obj_" #ent "_get_" #field ": " FORMAT_##type       \
              " != " FORMAT_##type,                                           \
              old, field);                                                    \
      else                                                                    \
        pass ();                                                              \
    }
#endif

#ifndef USE_DEPRECATED_API
#  define CHK_ENTITY_2RD_W_OLD(ent, name, field)                              \
    CHK_ENTITY_2RD (ent, name, field)
#  define CHK_ENTITY_3RD_W_OLD(ent, name, field)                              \
    CHK_ENTITY_3RD (ent, name, field)
#else
#  define CHK_ENTITY_2RD_W_OLD(ent, name, field)                              \
    CHK_ENTITY_2RD (ent, name, field);                                        \
    {                                                                         \
      dwg_point_2d _pt2d;                                                     \
      DWGAPI_ENT_NAME (ent, field) (ent, &_pt2d, &error);                     \
      if (error || memcmp (&field, &_pt2d, sizeof (field)))                   \
        fail ("old API dwg_ent_" #ent "_get_" #field);                        \
      else                                                                    \
        pass ();                                                              \
    }
#  define CHK_ENTITY_3RD_W_OLD(ent, name, field)                              \
    CHK_ENTITY_3RD (ent, name, field);                                        \
    {                                                                         \
      dwg_point_3d _pt3d;                                                     \
      DWGAPI_ENT_NAME (ent, field) (ent, &_pt3d, &error);                     \
      if (error || memcmp (&field, &_pt3d, sizeof (field)))                   \
        fail ("old API dwg_ent_" #ent "_get_" #field);                        \
      else                                                                    \
        pass ();                                                              \
    }
#endif

#define CHK_SUBCLASS_TYPE(ptr, name, field, typ)                              \
  {                                                                           \
    BITCODE_##typ _value;                                                     \
    bool _ok;                                                                 \
    if (dwg_dynapi_entity_fields (#name))                                     \
      _ok = dwg_dynapi_entity_value (&ptr, #name, #field, &_value, NULL);     \
    else                                                                      \
      _ok = dwg_dynapi_subclass_value (&ptr, #name, #field, &_value, NULL);   \
    if (!_ok)                                                                 \
      fail (#name "." #field);                                                \
    else                                                                      \
      {                                                                       \
        if (ptr.field == _value)                                              \
          {                                                                   \
            if (g_counter > g_countmax)                                       \
              pass ();                                                        \
            else                                                              \
              ok (#name "." #field ":\t" FORMAT_##typ,                        \
                  (BITCODE_##typ)ptr.field);                                  \
          }                                                                   \
        else                                                                  \
          fail (#name "." #field ":\t" FORMAT_##typ " [" #typ "]",            \
                (BITCODE_##typ)ptr.field);                                    \
      }                                                                       \
  }
#define CHK_SUBCLASS_TYPE_CAST(ptr, name, field, typ, cast)                   \
  {                                                                           \
    BITCODE_##cast _value;                                                    \
    bool _ok;                                                                 \
    if (dwg_dynapi_entity_fields (#name))                                     \
      _ok = dwg_dynapi_entity_value (&ptr, #name, #field, &_value, NULL);     \
    else                                                                      \
      _ok = dwg_dynapi_subclass_value (&ptr, #name, #field, &_value, NULL);   \
    if (!_ok)                                                                 \
      fail (#name "." #field);                                                \
    else                                                                      \
      {                                                                       \
        if (ptr.field == _value)                                              \
          {                                                                   \
            if (g_counter > g_countmax)                                       \
              pass ();                                                        \
            else                                                              \
              ok (#name "." #field ":\t" FORMAT_##typ,                        \
                  (BITCODE_##typ)ptr.field);                                  \
          }                                                                   \
        else                                                                  \
          fail (#name "." #field ":\t" FORMAT_##typ " [" #typ "]",            \
                (BITCODE_##typ)ptr.field);                                    \
      }                                                                       \
  }
#define CHK_SUBCLASS_3RD(ptr, name, field)                                    \
  {                                                                           \
    BITCODE_3RD _value;                                                       \
    bool _ok;                                                                 \
    if (dwg_dynapi_entity_fields (#name))                                     \
      _ok = dwg_dynapi_entity_value (&ptr, #name, #field, &_value, NULL);     \
    else                                                                      \
      _ok = dwg_dynapi_subclass_value (&ptr, #name, #field, &_value, NULL);   \
    if (!_ok)                                                                 \
      fail (#name "." #field);                                                \
    else                                                                      \
      {                                                                       \
        if (_value.x == ptr.field.x && _value.y == ptr.field.y                \
            && _value.z == ptr.field.z)                                       \
          {                                                                   \
            if (g_counter > g_countmax)                                       \
              pass ();                                                        \
            else                                                              \
              ok (#name "." #field ":\t(%f, %f, %f)", _value.x, _value.y,     \
                  _value.z);                                                  \
          }                                                                   \
        else                                                                  \
          fail (#name "." #field ":\t(%f, %f, %f)", _value.x, _value.y,       \
                _value.z);                                                    \
      }                                                                       \
  }

#define CHK_SUBCLASS_3DPOINTS(ptr, name, field, num)                          \
  {                                                                           \
    BITCODE_3RD *_value = NULL;                                               \
    bool _ok;                                                                 \
    if (dwg_dynapi_entity_fields (#name))                                     \
      _ok = dwg_dynapi_entity_value (&ptr, #name, #field, &_value, NULL);     \
    else                                                                      \
      _ok = dwg_dynapi_subclass_value (&ptr, #name, #field, &_value, NULL);   \
    if (!_ok)                                                                 \
      fail (#name "." #field);                                                \
    else if (!_value)                                                         \
      pass ();                                                                \
    else                                                                      \
      {                                                                       \
        for (unsigned _i = 0; _i < (num); _i++)                               \
          {                                                                   \
            if (_value[_i].x == ptr.field[_i].x                               \
                && _value[_i].y == ptr.field[_i].y                            \
                && _value[_i].z == ptr.field[_i].z)                           \
              {                                                               \
                if (g_counter > g_countmax)                                   \
                  pass ();                                                    \
                else                                                          \
                  ok (#name "." #field "[%d]:\t(%f, %f, %f)", i,              \
                      _value[_i].x, _value[_i].y, _value[_i].z);              \
              }                                                               \
            else                                                              \
              fail (#name "." #field "[%d]:\t(%f, %f, %f)", i, _value[_i].x,  \
                    _value[_i].y, _value[_i].z);                              \
          }                                                                   \
      }                                                                       \
  }
#define CHK_SUBCLASS_2RD(ptr, name, field)                                    \
  {                                                                           \
    BITCODE_2RD _value;                                                       \
    bool _ok;                                                                 \
    if (dwg_dynapi_entity_fields (#name))                                     \
      _ok = dwg_dynapi_entity_value (&ptr, #name, #field, &_value, NULL);     \
    else                                                                      \
      _ok = dwg_dynapi_subclass_value (&ptr, #name, #field, &_value, NULL);   \
    if (!_ok)                                                                 \
      fail (#name "." #field);                                                \
    else                                                                      \
      {                                                                       \
        if (_value.x == ptr.field.x && _value.y == ptr.field.y)               \
          {                                                                   \
            if (g_counter > g_countmax)                                       \
              pass ();                                                        \
            else                                                              \
              ok (#name "." #field ":\t(%f, %f)", _value.x, _value.y);        \
          }                                                                   \
        else                                                                  \
          fail (#name "." #field ":\t(%f, %f)", _value.x, _value.y);          \
      }                                                                       \
  }
#define CHK_SUBCLASS_2DPOINTS(ptr, name, field, num)                          \
  {                                                                           \
    BITCODE_3RD *_value = NULL;                                               \
    bool _ok;                                                                 \
    if (dwg_dynapi_entity_fields (#name))                                     \
      _ok = dwg_dynapi_entity_value (&ptr, #name, #field, &_value, NULL);     \
    else                                                                      \
      _ok = dwg_dynapi_subclass_value (&ptr, #name, #field, &_value, NULL);   \
    if (!_ok)                                                                 \
      fail (#name "." #field);                                                \
    else if (!_value)                                                         \
      pass ();                                                                \
    else                                                                      \
      {                                                                       \
        for (unsigned _i = 0; _i < (num); _i++)                               \
          {                                                                   \
            if (_value[_i].x == ptr.field[_i].x                               \
                && _value[_i].y == ptr.field[_i].y)                           \
              {                                                               \
                if (g_counter > g_countmax)                                   \
                  pass ();                                                    \
                else                                                          \
                  ok (#name "." #field "[%d]:\t(%f, %f)", i, _value[_i].x,    \
                      _value[_i].y);                                          \
              }                                                               \
            else                                                              \
              fail (#name "." #field "[%d]:\t(%f, %f)", i, _value[_i].x,      \
                    _value[_i].y);                                            \
          }                                                                   \
      }                                                                       \
  }
#define CHK_SUBCLASS_H(ptr, name, field)                                      \
  {                                                                           \
    BITCODE_H _value;                                                         \
    bool _ok;                                                                 \
    if (dwg_dynapi_entity_fields (#name))                                     \
      _ok = dwg_dynapi_entity_value (&ptr, #name, #field, &_value, NULL);     \
    else                                                                      \
      _ok = dwg_dynapi_subclass_value (&ptr, #name, #field, &_value, NULL);   \
    if (!_ok)                                                                 \
      fail (#name "." #field);                                                \
    else                                                                      \
      {                                                                       \
        int alloced = 0;                                                      \
        char *_hdlname                                                        \
            = _value ? dwg_dynapi_handle_name (obj->parent, _value, &alloced) \
                     : NULL;                                                  \
        if (!_value)                                                          \
          {                                                                   \
            if (!ptr.field)                                                   \
              {                                                               \
                if (g_counter > g_countmax)                                   \
                  pass ();                                                    \
                else                                                          \
                  ok (#name "." #field ":\tNULL");                            \
              }                                                               \
            else                                                              \
              fail (#name "." #field ":\tNULL");                              \
          }                                                                   \
        else if (memcmp (&ptr.field, &_value, sizeof _value) == 0)            \
          {                                                                   \
            if (g_counter > g_countmax)                                       \
              pass ();                                                        \
            else                                                              \
              ok (#name "." #field ":\t %s " FORMAT_REF, _hdlname ?: "",      \
                  ARGS_REF (_value));                                         \
          }                                                                   \
        else                                                                  \
          fail (#name "." #field ":\t %s " FORMAT_REF, _hdlname ?: "",        \
                ARGS_REF (_value));                                           \
        if (alloced)                                                          \
          FREE (_hdlname);                                                    \
      }                                                                       \
  }
#define CHK_SUBCLASS_HV(ptr, name, field, num)                                \
  if (!dwg_dynapi_subclass_value (&ptr, #name, #field, &field, NULL))         \
    fail (#name "." #field);                                                  \
  else if (!field)                                                            \
    pass ();                                                                  \
  else                                                                        \
    {                                                                         \
      for (int _i = 0; _i < (int)(num); _i++)                                 \
        {                                                                     \
          BITCODE_H _hdl = field[_i];                                         \
          if (_hdl == ptr.field[_i])                                          \
            {                                                                 \
              if (g_counter > g_countmax)                                     \
                pass ();                                                      \
              else                                                            \
                ok (#name "." #field "[%d]: " FORMAT_REF, _i,                 \
                    ARGS_REF (_hdl));                                         \
            }                                                                 \
          else                                                                \
            {                                                                 \
              fail (#name "." #field "[%d]: " FORMAT_REF, _i,                 \
                    ARGS_REF (_hdl));                                         \
            }                                                                 \
        }                                                                     \
    }
#define CHK_SUBCLASS_VECTOR_TYPE(ptr, name, field, num, type)                 \
  {                                                                           \
    BITCODE_##type *_value = NULL;                                            \
    bool _ok;                                                                 \
    if (dwg_dynapi_entity_fields (#name))                                     \
      _ok = dwg_dynapi_entity_value (&ptr, #name, #field, &_value, NULL);     \
    else                                                                      \
      _ok = dwg_dynapi_subclass_value (&ptr, #name, #field, &_value, NULL);   \
    if (!_ok)                                                                 \
      fail (#name "." #field);                                                \
    else if (!_value)                                                         \
      pass ();                                                                \
    else                                                                      \
      {                                                                       \
        for (unsigned _i = 0; _i < (num); _i++)                               \
          {                                                                   \
            if (_value[_i] == ptr.field[_i])                                  \
              {                                                               \
                if (g_counter > g_countmax)                                   \
                  pass ();                                                    \
                else                                                          \
                  ok (#name "." #field "[%d]:\t " FORMAT_##type, _i,          \
                      _value[_i]);                                            \
              }                                                               \
            else                                                              \
              fail (#name "." #field "[%d]:\t " FORMAT_##type, _i,            \
                    _value[_i]);                                              \
          }                                                                   \
      }                                                                       \
  }
#define CHK_SUBCLASS_UTF8TEXT(ptr, name, field)                               \
  {                                                                           \
    BITCODE_TV _value;                                                        \
    bool _ok;                                                                 \
    if (dwg_dynapi_entity_fields (#name))                                     \
      _ok = dwg_dynapi_entity_value (&ptr, #name, #field, &_value, NULL);     \
    else                                                                      \
      _ok = dwg_dynapi_subclass_value (&ptr, #name, #field, &_value, NULL);   \
    if (_ok)                                                                  \
      {                                                                       \
        if (g_counter > g_countmax)                                           \
          pass ();                                                            \
        else                                                                  \
          ok (#name "." #field ":\t\"%s\"", _value);                          \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        if (dwg_version < R_2007)                                             \
          fail (#name "." #field ":\t\"%s\"", _value);                        \
        else                                                                  \
          fail (#name "." #field);                                            \
      }                                                                       \
  }
#define CHK_SUBCLASS_CMC(ptr, name, field)                                    \
  if (!dwg_dynapi_subclass_value (&ptr, #name, #field, &ptr.field, NULL))     \
    fail (#name "." #field);                                                  \
  else                                                                        \
    {                                                                         \
      if (g_counter > g_countmax)                                             \
        pass ();                                                              \
      else                                                                    \
        ok (#name "." #field ":\t%d", ptr.field.index);                       \
    }
#define CHK_SUBCLASS_MAX(ptr, name, field, type, _max)                        \
  if (ptr.field > _max)                                                       \
  fail ("Invalid " #name "." #field " " FORMAT_##type " > " #_max, ptr.field)

void
api_common_object (dwg_object *obj)
{
  BITCODE_H ownerhandle, xdicobjhandle, handleref;
  BITCODE_BL num_reactors, num_eed;
  BITCODE_H *reactors;
  BITCODE_B is_xdic_missing, has_ds_data;
  Dwg_Data *dwg = obj->parent;
  Dwg_Version_Type version = obj->parent->header.version;
  Dwg_Object_Object *obj_obj = obj->tio.object;
  Dwg_Object_LAYER *_obj = obj->tio.object->tio.LAYER;

  CHK_COMMON_H (_obj, ownerhandle);
  CHK_COMMON_TYPE (_obj, is_xdic_missing, B);
  if (!is_xdic_missing)
    CHK_COMMON_H (_obj, xdicobjhandle);
  CHK_COMMON_TYPE (_obj, num_reactors, BL);
  CHK_COMMON_HV (_obj, reactors, num_reactors);
  CHK_COMMON_H (_obj, handleref); //?
  CHK_COMMON_TYPE (_obj, has_ds_data, B);
  CHK_COMMON_TYPE (_obj, num_eed, BL);
}

#define CHK_EVALEXPR(type)                                                    \
  CHK_SUBCLASS_TYPE (_obj->evalexpr, EvalExpr, parentid, BLd);                \
  CHK_SUBCLASS_TYPE (_obj->evalexpr, EvalExpr, major, BL);                    \
  CHK_SUBCLASS_TYPE (_obj->evalexpr, EvalExpr, minor, BL);                    \
  /* variant_DXF type */                                                      \
  CHK_SUBCLASS_TYPE (_obj->evalexpr, EvalExpr, value_code, BSd);              \
  /* variant_value's */                                                       \
  switch (_obj->evalexpr.value_code)                                          \
    {                                                                         \
    case 40:                                                                  \
      CHK_SUBCLASS_TYPE (_obj->evalexpr, EvalExpr, value.num40, BD);          \
      break;                                                                  \
    case 10:                                                                  \
      CHK_SUBCLASS_2RD (_obj->evalexpr, EvalExpr, value.pt2d);                \
      break;                                                                  \
    case 11:                                                                  \
      CHK_SUBCLASS_3RD (_obj->evalexpr, EvalExpr, value.pt3d);                \
      break;                                                                  \
    case 1:                                                                   \
      CHK_SUBCLASS_UTF8TEXT (_obj->evalexpr, EvalExpr, value.text1);          \
      break;                                                                  \
    case 90:                                                                  \
      CHK_SUBCLASS_TYPE (_obj->evalexpr, EvalExpr, value.long90, BL);         \
      break;                                                                  \
    case 91:                                                                  \
      CHK_SUBCLASS_H (_obj->evalexpr, EvalExpr, value.handle91);              \
      break;                                                                  \
    case 70:                                                                  \
      CHK_SUBCLASS_TYPE (_obj->evalexpr, EvalExpr, value.short70, BS);        \
      break;                                                                  \
    case -9999:                                                               \
    default:                                                                  \
      break;                                                                  \
    }                                                                         \
  CHK_SUBCLASS_TYPE (_obj->evalexpr, EvalExpr, nodeid, BL)

#define CHK_ACSH_HISTORYNODE()                                                \
  CHK_SUBCLASS_TYPE (_obj->history_node, ACSH_HistoryNode, major, BL);        \
  CHK_SUBCLASS_TYPE (_obj->history_node, ACSH_HistoryNode, minor, BL);        \
  /* last 16x nums 40-55 */                                                   \
  if (!dwg_dynapi_subclass_value (&_obj->history_node, "ACSH_HistoryNode",    \
                                  "trans", &trans, NULL))                     \
    fail ("ACSH_HistoryNode.trans");                                          \
  else                                                                        \
    {                                                                         \
      bool good = 1;                                                          \
      for (int i = 0; i < 16; i += 4)                                         \
        {                                                                     \
          for (int j = i; j < i + 4; j++)                                     \
            { /* catches nan */                                               \
              if (trans[j] != _obj->history_node.trans[j])                    \
                good = 0;                                                     \
            }                                                                 \
          if (good)                                                           \
            ok ("ACSH_HistoryNode.trans[%d]: (%f, %f, %f, %f)", i, trans[i],  \
                trans[i + 1], trans[i + 2], trans[i + 3]);                    \
          else                                                                \
            fail ("ACSH_HistoryNode.trans[%d]: (%f, %f, %f, %f)", i,          \
                  trans[i], trans[i + 1], trans[i + 2], trans[i + 3]);        \
        }                                                                     \
    }                                                                         \
  CHK_SUBCLASS_CMC (_obj->history_node, ACSH_HistoryNode, color);             \
  CHK_SUBCLASS_TYPE (_obj->history_node, ACSH_HistoryNode, step_id, BL);      \
  CHK_SUBCLASS_H (_obj->history_node, ACSH_HistoryNode, material)

#define CHK_VALUEPARAM(numfield, valprefix)                                   \
  for (unsigned i = 0; i < _obj->numfield; i++)                               \
    {                                                                         \
      CHK_SUBCLASS_TYPE (_obj->valprefix[i], VALUEPARAM, class_version, BL);  \
      CHK_SUBCLASS_UTF8TEXT (_obj->valprefix[i], VALUEPARAM, name);           \
      CHK_SUBCLASS_TYPE (_obj->valprefix[i], VALUEPARAM, unit_type, BL);      \
      CHK_SUBCLASS_TYPE (_obj->valprefix[i], VALUEPARAM, num_vars, BL);       \
      CHK_SUBCLASS_H (_obj->valprefix[i], VALUEPARAM, controlled_objdep);     \
      for (unsigned j = 0; j < _obj->valprefix[i].num_vars; j++)              \
        {                                                                     \
          unsigned dxf = _obj->valprefix[i].vars[j].value.code;               \
          CHK_SUBCLASS_TYPE (_obj->valprefix[i].vars[j].value, EvalVariant,   \
                             code, BSd);                                      \
          switch (dwg_resbuf_value_type (dxf))                                \
            {                                                                 \
            case DWG_VT_REAL:                                                 \
              CHK_SUBCLASS_TYPE (_obj->valprefix[i].vars[j].value,            \
                                 EvalVariant, u.bd, BD);                      \
              break;                                                          \
            case DWG_VT_INT32:                                                \
              CHK_SUBCLASS_TYPE (_obj->valprefix[i].vars[j].value,            \
                                 EvalVariant, u.bl, BL);                      \
              break;                                                          \
            case DWG_VT_INT16:                                                \
              CHK_SUBCLASS_TYPE (_obj->valprefix[i].vars[j].value,            \
                                 EvalVariant, u.bs, BS);                      \
              break;                                                          \
            case DWG_VT_INT8:                                                 \
              CHK_SUBCLASS_TYPE (_obj->valprefix[i].vars[j].value,            \
                                 EvalVariant, u.rc, RC);                      \
              break;                                                          \
            case DWG_VT_STRING:                                               \
              CHK_SUBCLASS_UTF8TEXT (_obj->valprefix[i].vars[j].value,        \
                                     EvalVariant, u.text);                    \
              break;                                                          \
            case DWG_VT_HANDLE:                                               \
              CHK_SUBCLASS_H (_obj->valprefix[i].vars[j].value, EvalVariant,  \
                              u.handle);                                      \
              break;                                                          \
            case DWG_VT_BINARY:                                               \
            case DWG_VT_OBJECTID:                                             \
            case DWG_VT_POINT3D:                                              \
            case DWG_VT_INVALID:                                              \
            case DWG_VT_INT64:                                                \
            case DWG_VT_BOOL:                                                 \
            default:                                                          \
              fail ("Unknown VALUEPARAM.vars[%d].value.code %u", j, dxf);     \
            }                                                                 \
          CHK_SUBCLASS_H (_obj->valprefix[i], VALUEPARAM, controlled_objdep); \
        }                                                                     \
    }

// FIXME TYPE unused
#define CHK_ASSOCPARAMBASEDACTIONBODY(TYPE)                                   \
  PRE (R_2013)                                                                \
  {                                                                           \
    CHK_SUBCLASS_TYPE (_obj->pab, ASSOCPARAMBASEDACTIONBODY, version, BL);    \
    CHK_SUBCLASS_TYPE (_obj->pab, ASSOCPARAMBASEDACTIONBODY, minor, BL);      \
    CHK_SUBCLASS_TYPE (_obj->pab, ASSOCPARAMBASEDACTIONBODY, num_deps, BL);   \
    CHK_SUBCLASS_HV (_obj->pab, ASSOCPARAMBASEDACTIONBODY, deps,              \
                     _obj->pab.num_deps);                                     \
    CHK_SUBCLASS_TYPE (_obj->pab, ASSOCPARAMBASEDACTIONBODY, l4, BL);         \
    CHK_SUBCLASS_TYPE (_obj->pab, ASSOCPARAMBASEDACTIONBODY, l5, BL);         \
    CHK_SUBCLASS_H (_obj->pab, ASSOCPARAMBASEDACTIONBODY, assocdep);          \
    CHK_SUBCLASS_TYPE (_obj->pab, ASSOCPARAMBASEDACTIONBODY, num_values, BL); \
    CHK_VALUEPARAM (pab.num_values, pab.values);                              \
  }

#define BLOCKPARAMETER_PropInfo(_prop)                                        \
  {                                                                           \
    Dwg_BLOCKPARAMETER_connection *connections;                               \
    CHK_SUBCLASS_TYPE (_obj->_prop, BLOCKPARAMETER_PropInfo, num_connections, \
                       BL);                                                   \
    if (!dwg_dynapi_subclass_value (&_obj->_prop, "BLOCKPARAMETER_PropInfo",  \
                                    "connections", &connections, NULL))       \
      fail ("BLOCKPARAMETER." #_prop ".connections");                         \
    else                                                                      \
      for (i = 0; i < _obj->_prop.num_connections; i++)                       \
        {                                                                     \
          CHK_SUBCLASS_TYPE (_obj->_prop.connections[i],                      \
                             BLOCKPARAMETER_connection, code, BL);            \
          CHK_SUBCLASS_UTF8TEXT (_obj->_prop.connections[i],                  \
                                 BLOCKPARAMETER_connection, name);            \
        }                                                                     \
  }

// allow old deprecated API
GCC31_DIAG_IGNORE (-Wdeprecated - declarations)
GCC46_DIAG_IGNORE (-Wdeprecated - declarations)
