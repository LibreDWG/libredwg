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
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../src/config.h"
#include <dwg.h>
#include "../src/common.h"
#include "../src/bits.h"
#include "../src/logging.h"
#include "suffix.inc"
static int help(void);
int verbosity(int argc, char **argv, int i, unsigned int *opts);
#include "common.inc"

int minimal = 0;
int binary = 0;
//FILE *fh;
char buf[4096];
/* the current version per spec block */
static unsigned int cur_ver = 0;

const char *dxf_format (int code);

static int usage(void) {
  printf("\nUsage: dwg2dxf [-v[N]] [-as-rNNNN] [-m|--minimal] [-b|--binary] <input_file.dwg> [<output_file.dxf>]\n");
  return 1;
}
static int opt_version(void) {
  printf("dwg2dxf %s\n", PACKAGE_VERSION);
  return 0;
}
static int help(void) {
  printf("\nUsage: dwg2dxf [OPTION]... DWGFILE [DXFFILE]\n");
  printf("Converts the DWG to a DXF.\n");
  printf("Default DXFFILE: DWGFILE with .dxf extension.\n"
         "\n");
  printf("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf("  -as-rNNNN                 save as version\n");
  printf("           Valid versions:\n");
  printf("             r12, r14, r2000, r2004\n");
  printf("           Planned versions:\n");
  printf("             r9, r10, r11, r2007, r2010, r2013, r2018\n");
  printf("  -m, --minimal             only $ACADVER, HANDSEED and ENTITIES\n");
  printf("  -b, --binary              save as binary DXF\n"); //not yet
  printf("      --help                display this help and exit\n");
  printf("      --version             output version information and exit\n"
         "\n");
  printf("GNU LibreDWG online manual: <https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

#define VAR(name, code, value)                  \
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
#define HEADER_3D(name)\
  POINT_3D (name, header_vars.name, 10, 20, 30);
#define HEADER_2D(name)\
  POINT_2D (name, header_vars.name, 10, 20);
#define SECTION(section) fprintf(fh, "  0\nSECTION\n  2\n" #section "\n")
#define ENDSEC()         fprintf(fh, "  0\nENDSEC\n")
#define RECORD(record)   fprintf(fh, "  0\n" #record "\n")
#define VALUE(code, value) \
  {\
    snprintf (buf, 4096, "%3i\n%s\n", code, dxf_format (code));\
    fprintf(fh, buf, value);\
  }
#define HANDLE_NAME(name, code, section) \
  {\
    Dwg_Object_Ref *ref = dwg->header_vars.name;\
    snprintf (buf, 4096, "  9\n$" #name "\n%3i\n%s\n", code, dxf_format (code));\
    /*if (ref && !ref->obj) ref->obj = dwg_resolve_handle(dwg, ref->absolute_ref); */ \
    fprintf(fh, buf, ref && ref->obj \
      ? ref->obj->tio.object->tio.section->entry_name : ""); \
  }

const char *
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

GCC_DIAG_IGNORE(-Wformat-nonliteral)

// see https://www.autodesk.com/techpubs/autocad/acad2000/dxf/header_section_group_codes_dxf_02.htm
static int
dxf_header_write (FILE *fh, Dwg_Data * dwg)
{
  double ms;
  struct Dwg_Header *dat = &dwg->header;
  const char* codepage =
    (dwg->header.codepage == 30 || dwg->header.codepage == 0)
    ? "ANSI_1252"
    : (dwg->header.version >= R_2007)
      ? "UTF-8"
      : "ANSI_1252";

  SECTION(HEADER);

  VAR (ACADVER, 1, version_codes[dwg->header.version]);
  if (minimal) {
    HEADER_VALUE (HANDSEED, 5);
    ENDSEC();
    return 0;
  }
  if (dwg->header.codepage != 30 &&
      dwg->header.codepage != 0 &&
      dwg->header.version < R_2007) {
    // some asian or eastern-european codepage
    // see https://pythonhosted.org/ezdxf/dxfinternals/fileencoding.html
    LOG_WARN("Unknown codepage %d, assuming ANSI_1252", dwg->header.codepage);
  }
  SINCE(R_13) {
    VAR (ACADMAINTVER, 70, dwg->header.maint_version);
    VAR (DWGCODEPAGE, 3, codepage);
  }
  HEADER_3D (INSBASE);
  HEADER_3D (EXTMIN);
  HEADER_3D (EXTMAX);
  HEADER_2D (LIMMIN);
  HEADER_2D (LIMMAX);

  HEADER_VALUE (ORTHOMODE, 70);
  HEADER_VALUE (REGENMODE, 70);
  HEADER_VALUE (FILLMODE, 70);
  HEADER_VALUE (QTEXTMODE, 70);
  HEADER_VALUE (MIRRTEXT, 70);
  UNTIL(R_14) {
    HEADER_VALUE (DRAGMODE, 70);
  }
  HEADER_VALUE (LTSCALE, 40);
  UNTIL(R_14) {
    HEADER_VALUE (OSMODE, 70);
  }
  HEADER_VALUE (ATTMODE, 70);
  HEADER_VALUE (TEXTSIZE, 40);
  HEADER_VALUE (TRACEWID, 40);

  HANDLE_NAME (TEXTSTYLE, 7, SHAPEFILE);
  HANDLE_NAME (CLAYER, 8, LAYER);
  HANDLE_NAME (CELTYPE, 6, LTYPE);
  HEADER_VALUE (CECOLOR, 62);
  SINCE(R_13) {
    HEADER_VALUE (CELTSCALE, 40);
    UNTIL(R_14) {
      HEADER_VALUE (DELOBJ, 70);
    }
    HEADER_VALUE (DISPSILH, 70); // this is WIREFRAME
    HEADER_VALUE (DIMSCALE, 40);
  }
  HEADER_VALUE (DIMASZ, 40);
  HEADER_VALUE (DIMEXO, 40);
  HEADER_VALUE (DIMDLI, 40);
  HEADER_VALUE (DIMRND, 40);
  HEADER_VALUE (DIMDLE, 40);
  HEADER_VALUE (DIMEXE, 40);
  //HEADER_VALUE (DIMTP, 40);
  //HEADER_VALUE (DIMTM, 40);
  HEADER_VALUE (DIMTXT, 40);
  HEADER_VALUE (DIMCEN, 40);
  HEADER_VALUE (DIMTSZ, 40);
  HEADER_VALUE (DIMTOL, 70);
  HEADER_VALUE (DIMLIM, 70);
  HEADER_VALUE (DIMTIH, 70);
  HEADER_VALUE (DIMTOH, 70);
  HEADER_VALUE (DIMSE1, 70);
  HEADER_VALUE (DIMSE2, 70);
  HEADER_VALUE (DIMTAD, 70);
  HEADER_VALUE (DIMZIN, 70);
  HANDLE_NAME (DIMBLK, 1, BLOCK_HEADER);
  HEADER_VALUE (DIMASO, 70);
  HEADER_VALUE (DIMSHO, 70);
  VERSIONS(R_13, R_14) {
    HEADER_VALUE (DIMSAV, 70); //?
  }
  HEADER_VALUE (DIMPOST, 1);
  HEADER_VALUE (DIMAPOST, 1);
  HEADER_VALUE (DIMALT, 70);
  HEADER_VALUE (DIMALTD, 70);
  HEADER_VALUE (DIMALTF, 40);
  HEADER_VALUE (DIMLFAC, 40);
  HEADER_VALUE (DIMTOFL, 70);
  HEADER_VALUE (DIMTVP, 40);
  HEADER_VALUE (DIMTIX, 70);
  HEADER_VALUE (DIMSOXD, 70);
  HEADER_VALUE (DIMSAH, 70);
  HANDLE_NAME (DIMBLK1, 1,  BLOCK_HEADER);
  HANDLE_NAME (DIMBLK2, 1,  BLOCK_HEADER);
  HANDLE_NAME (DIMSTYLE, 2, DIMSTYLE);
  HEADER_VALUE (DIMCLRD, 70);
  HEADER_VALUE (DIMCLRE, 70);
  HEADER_VALUE (DIMCLRT, 70);
  HEADER_VALUE (DIMTFAC, 40);
  HEADER_VALUE (DIMGAP, 40);
  SINCE(R_13) {
    HEADER_VALUE (DIMJUST, 70);
    HEADER_VALUE (DIMSD1, 70);
    HEADER_VALUE (DIMSD2, 70);
    HEADER_VALUE (DIMTOLJ, 70);
    HEADER_VALUE (DIMTZIN, 70);
    HEADER_VALUE (DIMALTZ, 70);
    HEADER_VALUE (DIMALTTZ, 70);
    HEADER_VALUE (DIMUPT, 70);
    HEADER_VALUE (DIMDEC, 70);
    HEADER_VALUE (DIMTDEC, 70);
    HEADER_VALUE (DIMALTU, 70);
    HEADER_VALUE (DIMALTTD, 70);
    HANDLE_NAME (DIMTXSTY, 7, SHAPEFILE);
    HEADER_VALUE (DIMAUNIT, 70);
  }
  SINCE(R_2000) {
    HEADER_VALUE (DIMADEC, 70);
    HEADER_VALUE (DIMALTRND, 40);
    HEADER_VALUE (DIMAZIN, 70);
    HEADER_VALUE (DIMDSEP, 70);
    HEADER_VALUE (DIMATFIT, 70);
    HEADER_VALUE (DIMFRAC, 70);
    HANDLE_NAME (DIMLDRBLK, 1, BLOCK_HEADER);
    HEADER_VALUE (DIMLUNIT, 70);
    HEADER_VALUE (DIMLWD, 70);
    HEADER_VALUE (DIMLWE, 70);
    HEADER_VALUE (DIMTMOVE, 70);
  }
  HEADER_VALUE (LUNITS, 70);
  HEADER_VALUE (LUPREC, 70);
  HEADER_VALUE (SKETCHINC, 40);
  HEADER_VALUE (FILLETRAD, 40);
  HEADER_VALUE (AUNITS, 70);
  HEADER_VALUE (AUPREC, 70);
  HEADER_VALUE (MENU, 1);
  HEADER_VALUE (ELEVATION, 40);
  HEADER_VALUE (PELEVATION, 40);
  HEADER_VALUE (THICKNESS, 40);
  HEADER_VALUE (LIMCHECK, 70);
  UNTIL(R_14) {
      HEADER_VALUE (BLIPMODE, 70);
    }
  HEADER_VALUE (CHAMFERA, 40);
  HEADER_VALUE (CHAMFERB, 40);
  SINCE(R_13) {
    HEADER_VALUE (CHAMFERC, 40);
    HEADER_VALUE (CHAMFERD, 40);
  }
  HEADER_VALUE (SKPOLY, 70);

  ms = (double)dwg->header_vars.TDCREATE.ms;
  VAR (TDCREATE, 40, dwg->header_vars.TDCREATE.days + ms);
  SINCE(R_13) {
    VAR (TDUCREATE, 40, dwg->header_vars.TDCREATE.days + ms);
  }
  ms = (double)dwg->header_vars.TDUPDATE.ms;
  VAR (TDUPDATE, 40, dwg->header_vars.TDUPDATE.days + ms);
  SINCE(R_13) {
    VAR (TDUUPDATE, 40, dwg->header_vars.TDUPDATE.days + ms);
  }
  ms = (double)dwg->header_vars.TDINDWG.ms;
  VAR (TDINDWG, 40, dwg->header_vars.TDINDWG.days + ms);
  ms = (double)dwg->header_vars.TDUSRTIMER.ms;
  VAR (TDUSRTIMER, 40, dwg->header_vars.TDUSRTIMER.days + ms);

  //HEADER_VALUE (USRTIMER, 70); // 1
  HEADER_VALUE (ANGBASE, 50);
  HEADER_VALUE (ANGDIR, 70);
  HEADER_VALUE (PDMODE, 70);
  HEADER_VALUE (PDSIZE, 40);
  HEADER_VALUE (PLINEWID, 40);
  UNTIL(R_14) {
    HEADER_VALUE (COORDS, 70); // 2
  }
  HEADER_VALUE (SPLFRAME, 70);
  HEADER_VALUE (SPLINETYPE, 70);
  HEADER_VALUE (SPLINESEGS, 70);
  UNTIL(R_14) {
    HEADER_VALUE (ATTDIA, 70); //default 1
    HEADER_VALUE (ATTREQ, 70); //default 1
    HEADER_VALUE (HANDLING, 70); //default 1
  }

  HEADER_VALUE (HANDSEED, 5); //default: 20000, before r13: 0xB8BC

  HEADER_VALUE (SURFTAB1, 70); // 6
  HEADER_VALUE (SURFTAB2, 70); // 6
  HEADER_VALUE (SURFTYPE, 70); // 6
  HEADER_VALUE (SURFU, 70); // 6
  HEADER_VALUE (SURFV, 70); // 6
  SINCE(R_13) {
    HANDLE_NAME (UCSBASE, 2, UCS);
  }
  HANDLE_NAME (UCSNAME, 2, UCS);
  HEADER_3D (UCSORG);
  HEADER_3D (UCSXDIR);
  HEADER_3D (UCSYDIR);
  HANDLE_NAME (UCSORTHOREF, 2, UCS);
  HEADER_VALUE (UCSORTHOVIEW, 70);
  HEADER_3D (UCSORGTOP);
  HEADER_3D (UCSORGBOTTOM);
  HEADER_3D (UCSORGLEFT);
  HEADER_3D (UCSORGRIGHT);
  HEADER_3D (UCSORGFRONT);
  HEADER_3D (UCSORGBACK);

  HANDLE_NAME (PUCSBASE, 2, UCS);
  HANDLE_NAME (PUCSNAME, 2, UCS);
  HEADER_3D (PUCSORG);
  HEADER_3D (PUCSXDIR);
  HEADER_3D (PUCSYDIR);
  //HANDLE_NAME (PUCSORTHOREF, 2, UCS);
  HEADER_VALUE (PUCSORTHOVIEW, 70);
  HEADER_3D (PUCSORGTOP);
  HEADER_3D (PUCSORGBOTTOM);
  HEADER_3D (PUCSORGLEFT);
  HEADER_3D (PUCSORGRIGHT);
  HEADER_3D (PUCSORGFRONT);
  HEADER_3D (PUCSORGBACK);

  HEADER_VALUE (USERI1, 70);
  HEADER_VALUE (USERI2, 70);
  HEADER_VALUE (USERI3, 70);
  HEADER_VALUE (USERI4, 70);
  HEADER_VALUE (USERI5, 70);
  HEADER_VALUE (USERR1, 40);
  HEADER_VALUE (USERR2, 40);
  HEADER_VALUE (USERR3, 40);
  HEADER_VALUE (USERR4, 40);
  HEADER_VALUE (USERR5, 40);

  HEADER_VALUE (WORLDVIEW, 70);
  //VERSION(R_13) {
  //  HEADER_VALUE (WIREFRAME, 70); //Undocumented
  //}
  HEADER_VALUE (SHADEDGE, 70);
  HEADER_VALUE (SHADEDIF, 70);
  HEADER_VALUE (TILEMODE, 70);
  HEADER_VALUE (MAXACTVP, 70);

  HEADER_3D (PINSBASE);
  HEADER_VALUE (PLIMCHECK, 70);
  HEADER_3D (PEXTMIN);
  HEADER_3D (PEXTMAX);
  HEADER_2D (PLIMMIN);
  HEADER_2D (PLIMMAX);

  HEADER_VALUE (UNITMODE, 70);
  HEADER_VALUE (VISRETAIN, 70);
  VERSIONS(R_13, R_14) {
    HEADER_VALUE (DELOBJ, 70);
  }
  HEADER_VALUE (PLINEGEN, 70);
  HEADER_VALUE (PSLTSCALE, 70);
  HEADER_VALUE (TREEDEPTH, 70);
  UNTIL(R_11) {
    VAR (DWGCODEPAGE, 3, codepage);
  }
  VERSIONS(R_14, R_2000) {
    HEADER_VALUE (PROXYGRAPHICS, 70);
  }
  //HANDLE_NAME (CMLSTYLE, 2, MLINESTYLE); //default: Standard
  HEADER_VALUE (CMLJUST, 70);
  HEADER_VALUE (CMLSCALE, 40); //default: 20
  VERSIONS(R_13, R_14) {
    HEADER_VALUE (SAVEIMAGES, 70);
  }
  SINCE(R_2000) {
    HEADER_VALUE (PROXYGRAPHICS, 70);
    //HEADER_VALUE (MEASUREMENT, 70, (dwg->header_vars.MEASUREMENT ? 1 : 0));

    HEADER_VALUE (CELWEIGHT, 370);
    HEADER_VALUE (ENDCAPS, 280);
    HEADER_VALUE (JOINSTYLE, 280);
    HEADER_VALUE (LWDISPLAY, 290);
    HEADER_VALUE (INSUNITS, 70);
    HEADER_VALUE (HYPERLINKBASE, 1);
    HEADER_VALUE (STYLESHEET, 1);
    HEADER_VALUE (XEDIT, 290);
    HEADER_VALUE (CEPSNTYPE, 380);

    if (dwg->header_vars.CEPSNTYPE == 3)
    {
      HANDLE_NAME (CPSNID, 390, LTYPE);
    }
    HEADER_VALUE (PSTYLEMODE, 290);
    HEADER_VALUE (FINGERPRINTGUID, 2);
    HEADER_VALUE (VERSIONGUID, 2);
    HEADER_VALUE (EXTNAMES, 290);
    HEADER_VALUE (PSVPSCALE, 40);
    HEADER_VALUE (OLESTARTUP, 290);
  }

  ENDSEC();
  return 0;
}

static int
dxf_classes_write (FILE *fh, Dwg_Data * dwg)
{
  unsigned int i;

  SECTION(HEADER);
  for (i=0; i < dwg->num_classes; i++)
    {
      RECORD(CLASS);
      VALUE (1, dwg->dwg_class[i].dxfname);
      VALUE (2, dwg->dwg_class[i].cppname);
      VALUE (3, dwg->dwg_class[i].appname);
      VALUE (90, dwg->dwg_class[i].proxyflag);
      VALUE (280, dwg->dwg_class[i].wasazombie);
      // Is-an-entity. 1f2 for entities, 1f3 for objects
      VALUE (281, dwg->dwg_class[i].item_class_id == 0x1F2 ? 1 : 0);
    }
  ENDSEC();
  return 0;
}

static int
dxf_tables_write (FILE *fh, Dwg_Data * dwg)
{
  (void)dwg;

  SECTION(TABLES);
  //...
  ENDSEC();
  return 0;
}

static int
dxf_blocks_write (FILE *fh, Dwg_Data * dwg)
{
  (void)dwg;

  SECTION(BLOCKS);
  //...
  ENDSEC();
  return 0;
}

static int
dxf_entities_write (FILE *fh, Dwg_Data * dwg)
{
  (void)dwg;

  SECTION(ENTITIES);
  //...
  ENDSEC();
  return 0;
}

static int
dxf_objects_write (FILE *fh, Dwg_Data * dwg)
{
  (void)dwg;

  SECTION(OBJECTS);
  //...
  ENDSEC();
  return 0;
}

static int
dxf_preview_write (FILE *fh, Dwg_Data * dwg)
{
  (void)fh; (void)dwg;
  //...
  return 0;
}

static int
dwg_write_dxf(char *filename, Dwg_Data * dwg)
{
  FILE *fh;
  struct stat attrib;
  struct Dwg_Header *dat = &dwg->header;

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

  // a minimal header requires only $ACADVER, $HANDSEED, and then ENTITIES
  // see https://pythonhosted.org/ezdxf/dxfinternals/filestructure.html
  SINCE(R_13)
  {
    if (dxf_header_write (fh, dwg))
      goto fail;

    SINCE(R_2000) {
      if (dxf_classes_write (fh, dwg))
        goto fail;
    }

    if (dxf_tables_write (fh, dwg))
      goto fail;

    if (dxf_blocks_write (fh, dwg))
      goto fail;
  }

  if (dxf_entities_write (fh, dwg))
    goto fail;

  SINCE(R_13) {
    if (dxf_objects_write (fh, dwg))
      goto fail;
  }

  if (dwg->header.version >= R_2000 && !minimal) {
    if (dxf_preview_write (fh, dwg))
      goto fail;
  }

  fclose(fh);
  return 0;
 fail:
  fclose(fh);
  return 1;
}

GCC_DIAG_RESTORE

int
main (int argc, char *argv[])
{
  int i = 1;
  unsigned int opts = 1; //loglevel 1
  int error;
  Dwg_Data dwg;
  char* filename_in;
  const char *version = NULL;
  char* filename_out = NULL;
  Dwg_Version_Type dwg_version;

  // check args
  if (argc < 2)
    return usage();
#if defined(USE_TRACING) && defined(HAVE_SETENV)
  setenv("LIBREDWG_TRACE", "1", 0);
#endif

  if (argc > 2 &&
      (!strcmp(argv[i], "--verbose") ||
       !strncmp(argv[i], "-v", 2)))
    {
      int num_args = verbosity(argc, argv, i, &opts);
      dwg.opts = opts;
      argc -= num_args;
      i += num_args;
    }
  if (argc > 2 && !strncmp(argv[i], "-as-r", 5))
    {
      const char *opt = argv[i];
      dwg_version = dwg_version_as(&opt[4]);
      if (dwg_version == R_INVALID)
        {
          fprintf(stderr, "Invalid version %s\n", opt);
          return usage();
        }
      version = &opt[4];
      argc--;
      i++;
    }
  if (argc > 2 && (!strcmp(argv[i], "-m") || !strcmp(argv[i], "--minimal")))
    {
      minimal = 1;
      argc--;
    }
  if (argc > 2 && (!strcmp(argv[i], "-b") || !strcmp(argv[i], "--binary")))
    {
      binary = 1;
      argc--;
      i++;
    }
  if (argc > 1 && !strcmp(argv[i], "--help"))
    return help();
  if (argc > 1 && !strcmp(argv[i], "--version"))
    return opt_version();

  filename_in = argv[i];
  if (argc > 2)
    filename_out = argv[i+1];
  else
    filename_out = suffix (filename_in, "dxf");
  
  if (strcmp(filename_in, filename_out) == 0)
    {
      if (filename_out != argv[2])
        free (filename_out);
      return usage();
    }

  dwg.opts = opts;
  printf("Reading DWG file %s\n", filename_in);
  error = dwg_read_file(filename_in, &dwg);
  if (error)
      printf("READ ERROR\n");

  printf("Writing DXF file %s", filename_out);
  if (version)
    {
      printf(" as %s\n", version);
      if (dwg.header.from_version != dwg.header.version)
        dwg.header.from_version = dwg.header.version;
      //else keep from_version = 0
      dwg.header.version = dwg_version;
    }
  else
    {
      printf("\n");
    }
  
  printf("TODO: write to DXF not yet completed\n");
  error = dwg_write_dxf(filename_out, &dwg);
  if (error)
      printf("WRITE ERROR\n");

  if (filename_out != argv[2])
    free (filename_out);
  dwg_free(&dwg);
  return error;
}
