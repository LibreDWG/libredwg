/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwg2dxf.c: save a DWG as DXF.
 * optionally as a different version.
 *
 * written by Reini Urban
 */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "../src/config.h"
#include <dwg.h>
#include "../src/common.h"
#include "../src/logging.h"
#include "suffix.c"

int minimal = 0;
FILE *fh;
char buf[4096];

int usage() {
  printf("\nUsage:\tdwg2dxf [-as-rxxxx] [-m] <input_file.dwg> [<output_file.dxf>]\n");
  return 1;
}

#define HEADER_VAR(name, code, value)\
  {\
    fprintf (fh, "  9\n$" #name "\n%3i\n", code);\
    snprintf (buf, 4096, "%s\n", dxf_format (code));\
    fprintf (fh, buf, value);\
  }
#define HEADER_VALUE(name, code)\
  {\
    fprintf (fh, "  9\n$" #name "\n%3i\n", code);\
    snprintf (buf, 4096, "%s\n", dxf_format (code));\
    fprintf (fh, buf, dwg->header_vars.name);\
  }
#define POINT_3D(name, var, c1, c2, c3)\
  {\
    fprintf (fh, "  9\n$" #name "\n");\
    fprintf (fh, "%3i\n%-16.15g\n", c1, dwg->var.x);\
    fprintf (fh, "%3i\n%-16.15g\n", c2, dwg->var.y);\
    fprintf (fh, "%3i\n%-16.15g\n", c3, dwg->var.z);\
  }
#define POINT_2D(name, var, c1, c2) \
  {\
    fprintf (fh, "  9\n$" #name "\n");\
    fprintf (fh, "%3i\n%-16.15g\n", c1, dwg->var.x);\
    fprintf (fh, "%3i\n%-16.15g\n", c2, dwg->var.y);\
  }
#define SECTION(section) fprintf(fh, "  0\nSECTION\n  2\n" #section "\n")
#define ENDSEC()         fprintf(fh, "  0\nENDSEC\n")
#define RECORD(record)   fprintf(fh, "  0\n" #record "\n")
#define VALUE(code, value) \
  {\
    snprintf (buf, 4096, "%3i\n%s\n", code, dxf_format (code));\
    fprintf(fh, buf, value);\
  }

char *
dxf_format (int code)
{
  if (0 <= code && code < 5)
    return "%s";
  if (code == 5)
    return "%X";
  if (5 < code && code < 10)
    return "%s";
  if (code < 60)
    return "%-16.15g";
  if (code < 80)
    return "%6i";
  if (89 < code && code < 100)
    return "%9li";
  if (code == 100)
    return "%s";
  if (code == 102)
    return "%s";
  if (code == 105)
    return "%X";
  if (105 < code && code < 148)
    return "%-16.15g";
  if (169 < code && code < 180)
    return "%6i";
  if (269 < code && code < 300)
    return "%6i";
  if (299 < code && code < 320)
    return "%s";
  if (319 < code && code < 370)
    return "%X";
  if (369 < code && code < 390)
    return "%6i";
  if (389 < code && code < 400)
    return "%X";
  if (399 < code && code < 410)
    return "%6i";
  if (409 < code && code < 420)
    return "%s";
  if (code == 999)
    return "%s";
  if (999 < code && code < 1010)
    return "%s";
  if (1009 < code && code < 1060)
    return "%-16.15g";
  if (1059 < code && code < 1071)
    return "%6i";
  if (code == 1071)
    return "%9li";

  return "(unknown code)";
}

// see https://www.autodesk.com/techpubs/autocad/acad2000/dxf/header_section_group_codes_dxf_02.htm
static int
dxf_header_write (FILE *fh, Dwg_Data * dwg)
{
  SECTION(HEADER);

  HEADER_VAR (ACADVER, 1, version_codes[dwg->header.version]);
  if (minimal) {
    HEADER_VALUE (HANDSEED, 5);
    ENDSEC();
    return 0;
  }
  HEADER_VAR (ACADMAINTVER, 70, dwg->header.maint_version);
  if (dwg->header.codepage == 30 || dwg->header.codepage == 0) {
    HEADER_VAR (DWGCODEPAGE, 3, "ANSI_1252");
  }
  else if (dwg->header.version >= R_2007) {
    HEADER_VAR (DWGCODEPAGE, 3, "UTF-8");
  }
  else {
    // some asian or eastern-european codepage
    // see https://pythonhosted.org/ezdxf/dxfinternals/fileencoding.html
    LOG_WARN("Unknown codepage %d, assuming ANSI_1252", dwg->header.codepage);
    HEADER_VAR (DWGCODEPAGE, 3, "ANSI_1252");
  }

  POINT_3D (INSBASE, header_vars.INSBASE_MSPACE, 10, 20, 30);
  POINT_3D (EXTMIN, header_vars.EXTMIN_MSPACE, 10, 20, 30);
  POINT_3D (EXTMAX, header_vars.EXTMAX_MSPACE, 10, 20, 30);
  POINT_2D (LIMMIN, header_vars.LIMMIN_MSPACE, 10, 20);
  POINT_2D (LIMMAX, header_vars.LIMMAX_MSPACE, 10, 20);

  //...

  ENDSEC();
  return 0;
}

static int
dxf_classes_write (FILE *fh, Dwg_Data * dwg)
{
  return 0;
}

static int
dxf_tables_write (FILE *fh, Dwg_Data * dwg)
{
  return 0;
}

static int
dxf_blocks_write (FILE *fh, Dwg_Data * dwg)
{
  return 0;
}

static int
dxf_entities_write (FILE *fh, Dwg_Data * dwg)
{
  return 0;
}

static int
dxf_objects_write (FILE *fh, Dwg_Data * dwg)
{
  return 0;
}

static int
dxf_preview_write (FILE *fh, Dwg_Data * dwg)
{
  return 0;
}

static int
dwg_write_dxf(char *filename, Dwg_Data * dwg)
{
  FILE *fh;
  struct stat attrib;

  if (!stat (filename, &attrib))
    {
      LOG_ERROR("The file already exists. We won't overwrite it.");
      return -1;
    }
  fh = fopen (filename, "w");
  if (!fh)
    {
      LOG_ERROR("Failed to create the file: %s\n", filename);
      return -1;
    }
  VALUE(999, PACKAGE_STRING);
  // a minimal header requires only $ACADVER, $HANDSEED
  // see https://pythonhosted.org/ezdxf/dxfinternals/filestructure.html
  if (dwg->header.version >= R_13) {
    if (dxf_header_write (fh, dwg))
      goto fail;
    if (dxf_classes_write (fh, dwg))
      goto fail;
    if (dxf_tables_write (fh, dwg))
      goto fail;
    if (dxf_blocks_write (fh, dwg))
      goto fail;
  }

  if (dxf_entities_write (fh, dwg))
    goto fail;

  if (dwg->header.version >= R_13)
    if (dxf_objects_write (fh, dwg))
      goto fail;
  if (dwg->header.version >= R_2000)
    if (dxf_preview_write (fh, dwg))
      goto fail;

  fclose(fh);
  return 0;
 fail:
  fclose(fh);
  return 1;
}

int
main (int argc, char *argv[])
{
  int error;
  Dwg_Data dwg;
  char* filename_in;
  const char *version = NULL;
  char* filename_out = NULL;
  Dwg_Version_Type dwg_version;

  // check args
  if (argc < 2)
    return usage();
  filename_in = argv[1];

  if (argc > 2 && !strncmp(argv[1], "-as-r", 5))
    {
      const char *opt = argv[1];
      if (!strcmp(opt, "-as-r13") ||
          !strcmp(opt, "-as-r14") ||
          !strcmp(opt, "-as-r2000") ||
          !strcmp(opt, "-as-r2004") ||
          !strcmp(opt, "-as-r2007") ||
          !strcmp(opt, "-as-r2010") ||
          !strcmp(opt, "-as-r2013") ||
          !strcmp(opt, "-as-r2018"))
        {
          version = &opt[4];
          if (!strcmp(version, "r13"))
            dwg_version = R_13;
          else if (!strcmp(version, "r14"))
            dwg_version = R_14;
          else if (!strcmp(version, "r2000"))
            dwg_version = R_2000;
          else if (!strcmp(version, "r2004"))
            dwg_version = R_2004;
          else if (!strcmp(version, "r2007"))
            dwg_version = R_2007;
          else if (!strcmp(version, "r2010"))
            dwg_version = R_2010;
          else if (!strcmp(version, "r2013"))
            dwg_version = R_2013;
          else if (!strcmp(version, "r2018"))
            dwg_version = R_2018;
          filename_in = argv[2];
          argc--;
        }
      else
        {
          fprintf(stderr, "Invalid option %s\n", opt);
          return usage();
        }
    }
  if (argc > 2 && !strcmp(argv[1], "-m"))
    {
      minimal = 1;
      argc--;
    }
  if (argc > 2)
    filename_out = argv[2];
  else
    {
      filename_out = suffix (argv[1], "dxf");
    }
  
  if (strcmp(filename_in, filename_out) == 0)
    return usage();

  printf("Reading DWG file %s\n", filename_in);
  error = dwg_read_file(filename_in, &dwg);
  if (error)
      printf("READ ERROR\n");

  printf("Writing DXF file %s", filename_out);
  if (version) {
    printf(" as %s\n", version);
    if (dwg.header.from_version != dwg.header.version)
      dwg.header.from_version = dwg.header.version;
    //else keep from_version = 0
    dwg.header.version = dwg_version;
  } else {
    printf("\n");
  }
  
  printf("TODO: dump as DXF\n");
  error = dwg_write_dxf(filename_out, &dwg);
  if (error)
      printf("WRITE ERROR\n");
  dwg_free(&dwg);
  return error;
}
