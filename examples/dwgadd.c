/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2020-2023 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwgadd.c: Interpret a small language to add entities to a DWG or DXF.
 * written by Reini Urban
 */

#include "../src/config.h"
#ifdef HAVE_SSCANF_S
#  define __STDC_WANT_LIB_EXT1__ 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>

#include <dwg.h>
#define DWG_LOGLEVEL loglevel
#include "logging.h"
#include <dwg_api.h>

#include "common.h"
#include "decode.h"
#include "encode.h"
#include "bits.h"
#include "classes.h"
#ifndef DISABLE_JSON
#  include "in_json.h"
#  include "out_json.h"
#endif
#ifndef DISABLE_DXF
#  include "out_dxf.h"
#  include "in_dxf.h"
#endif

static int dwg_add_dat (Dwg_Data **dwgp, Bit_Chain *dat);
static unsigned int loglevel;

// accepts only ASCII strings, for fuzzing only
#ifdef HAVE_SSCANF_S
#  define SSCANF_S sscanf_s
// clang-format off
#  define SZ ,119
// clang-format on
#  define FMT_NAME "%[a-zA-Z0-9_]"
#  define FMT_TAG "%[^ !]"
#  define FMT_TBL "\"%[a-zA-Z0-9._ -]\""
#  define FMT_PATH "\"%[a-zA-Z0-9_. \\-]\""
#  define FMT_ANY "\"%[^\"]"
#else
#  define SSCANF_S sscanf
#  define SZ
#  define FMT_NAME "%119[a-zA-Z0-9_]"
#  define FMT_TAG "%119[^ !]"
#  define FMT_TBL "\"%119[a-zA-Z0-9._ -]\""
#  define FMT_PATH "\"%119[a-zA-Z0-9_. \\-]\""
#  define FMT_ANY "\"%119[^\"]\""
#endif

static int
usage (void)
{
  printf ("\nUsage: dwgadd [-v[0-9]] [--as rNNNN] [--dxf|--binary|--json] -o "
          "OUTFILE "
          "ADDFILE\n");
  return 1;
}
static int
fn_version (void)
{
  printf ("dwgadd %s\n", PACKAGE_VERSION);
  return 0;
}

static void
log_p (unsigned level, char *p)
{
  char *n = strchr (p, '\n');
  int size = n ? n - p : (int)strlen (p);
  if (DWG_LOGLEVEL >= level)
    {
      HANDLER (OUTPUT, "%.*s\n", size, p);
    }
}

static int
help (void)
{
  printf ("\nUsage: dwgadd [OPTIONS] -o outfile addfile\n");
  printf ("Create a DWG (or DXF, JSON) by adding entities with instructions "
          "from a special dwgadd file.\n"
          "See `man 5 dwgadd`\n");
#ifndef DISABLE_DXF
  printf ("  --dxf:    write DXF, not DWG\n");
  printf ("  --binary: write binary DXF, not DWG\n");
#endif
#ifndef DISABLE_JSON
  printf ("  --json:   write JSON, not DWG\n");
#endif
#ifdef HAVE_GETOPT_LONG
  printf ("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf ("  --as rNNNN                save as version\n");
  printf ("           Valid versions:\n");
  printf ("             r12, r14, r2000 (default)\n");
  printf ("           Planned versions:\n");
  printf ("             r1.4, r2.6, r2.10, r9, r10, r11, r2004, r2007, r2010, "
          "r2013, r2018\n");
  printf ("  -o outfile, --file outfile (default: stdout)\n");
#else
  printf ("  -v[0-9]     verbosity\n");
  printf ("  -a rNNNN    save as version\n");
  printf ("              Valid versions:\n");
  printf ("                r12, r14, r2000 (default)\n");
  printf ("              Planned versions:\n");
  printf ("                r1.4, r2.6, r2.10, r9, r10, r11, r2004, r2007, "
          "r2010, r2013, r2018\n");
  printf ("  -o outfile (default: stdout)\n");
#endif
  printf ("\n"
          " --version        display the version and exit\n");
  printf (" --help           display this help and exit\n");
  return 0;
}

static int opts = 1;

int
main (int argc, char *argv[])
{
  int i = 0;
  Dwg_Data dwg;
  Dwg_Data *dwgp;
  Bit_Chain dat = { NULL, 0, 0, 0, 0 };
  Bit_Chain out_dat = { NULL, 0, 0, 0, 0 };
  const char *outfile = NULL;
  Dwg_Version_Type dwg_version = R_2000;
  // boolean options
  struct opt_s
  {
    unsigned dwg : 1;
    unsigned dxf : 1;
    unsigned binary : 1;
    unsigned json : 1;
    unsigned geojson : 1;
    unsigned verify : 1;
    unsigned force_free : 1;
  } opt;
  int retval = 0;
  int c;
  FILE *fp;
  struct stat attrib;
#ifdef HAVE_GETOPT_LONG
  int option_index = 0;
  static struct option long_options[]
      = { { "verbose", 1, &opts, 1 }, // optional
          { "file", 1, 0, 'o' },      { "as", 1, 0, 'a' },
#  ifndef DISABLE_JSON
          { "dxf", 0, 0, 0 },         { "binary", 0, 0, 0 },
#  endif
#  ifndef DISABLE_JSON
          { "json", 0, 0, 0 },        { "geojson", 0, 0, 0 },
#  endif
          { "verify", 0, 0, 0 },      { "help", 0, 0, 0 },
          { "version", 0, 0, 0 },     { "force-free", 0, 0, 0 },
          { NULL, 0, NULL, 0 } };
#endif

  if (argc <= 1 || !*argv[1])
    return usage ();
  memset (&opt, 0, sizeof (struct opt_s));

  while
#ifdef HAVE_GETOPT_LONG
      ((c = getopt_long (argc, argv, "a:v::o:h", long_options, &option_index))
       != -1)
#else
      ((c = getopt (argc, argv, "a:v::o:hi")) != -1)
#endif
    {
      if (c == -1)
        break;
      switch (c)
        {
        case ':': // missing arg
          if (optarg && !strcmp (optarg, "v"))
            {
              opts = 1;
              break;
            }
          fprintf (stderr, "%s: option '-%c' requires an argument\n", argv[0],
                   optopt);
          break;
#ifdef HAVE_GETOPT_LONG
        case 0:
          /* This option sets a flag */
          if (!strcmp (long_options[option_index].name, "verbose"))
            {
              if (opts < 0 || opts > 9)
                return usage ();
#  if defined(USE_TRACING) && defined(HAVE_SETENV)
              {
                char v[2];
                *v = opts + '0';
                *(v + 1) = 0;
                setenv ("LIBREDWG_TRACE", v, 1);
              }
#  endif
              break;
            }
          if (strEQc (long_options[option_index].name, "version"))
            return fn_version ();
          else if (strEQc (long_options[option_index].name, "help"))
            return help ();
          else if (strEQc (long_options[option_index].name, "force-free"))
            opt.force_free = 1;
#  ifndef DISABLE_DXF
          else if (strEQc (long_options[option_index].name, "dxf"))
            opt.dxf = 1;
          else if (strEQc (long_options[option_index].name, "binary"))
            opt.binary = 1;
#  endif
#  ifndef DISABLE_JSON
          else if (strEQc (long_options[option_index].name, "json"))
            opt.json = 1;
#  endif
          else if (!strcmp (long_options[option_index].name, "verify"))
            opt.verify = 1;
          break;
#else
        case 'i':
          return opt_version ();
#endif
        case 'o':
          outfile = optarg;
          break;
        case 'a':
          dwg_version = dwg_version_as (optarg);
          if (dwg_version == R_INVALID)
            {
              fprintf (stderr, "Invalid version '%s'\n", argv[i]);
              return usage ();
            }
          break;
        case 'v': // support -v3 and -v
          i = (optind > 0 && optind < argc) ? optind - 1 : 1;
          if (!memcmp (argv[i], "-v", 2))
            {
              opts = argv[i][2] ? argv[i][2] - '0' : 3;
            }
          if (opts < 0 || opts > 9)
            return usage ();
#if defined(USE_TRACING) && defined(HAVE_SETENV)
          {
            char v[2];
            *v = opts + '0';
            *(v + 1) = 0;
            setenv ("LIBREDWG_TRACE", v, 1);
          }
#endif
          break;
        case 'h':
          return help ();
        case '?':
          fprintf (stderr, "%s: invalid option '-%c' ignored\n", argv[0],
                   optopt);
          break;
        default:
          return usage ();
        }
    }
  i = optind;

  memset (&dwg, 0, sizeof (dwg));
  loglevel = opts & DWG_OPTS_LOGLEVEL;
  dwg.header.version = dwg_version;
  dwg.header.from_version = dwg_version;
  dwg.opts = opts;
  dat.chain = NULL;
  dat.size = 0;
  dat.opts = opts;
  fp = fopen (argv[i], "rb");
  if (!fp)
    {
      LOG_ERROR ("Could not read %s", argv[i])
      exit (1);
    }
  LOG_INFO ("dwgadd read %s\n", argv[i])
  dat_read_file (&dat, fp, argv[i]);
  if (dat.size == 0)
    {
      LOG_ERROR ("empty %s", argv[i])
      free (dat.chain);
      exit (1);
    }

  dwgp = &dwg;
  LOG_INFO ("dwgadd process %s\n", argv[i])
  if ((retval = dwg_add_dat (&dwgp, &dat)) == 0)
    {
      int error;
      fclose (fp);
      dwg = *dwgp;
      out_dat.byte = 0;
      out_dat.bit = 0;
      if (!outfile)
        {
#ifdef _WIN32
          outfile = "CON";
#else
          outfile = "/dev/stdout";
#endif
          if (opt.verify)
            LOG_ERROR ("Cannot --verify with %s\n", outfile)
          opt.verify = false;
        }
      LOG_INFO ("\ndwgadd write %s\n", outfile)
      out_dat.fh = fopen (outfile, "wb");
      if (!out_dat.fh)
        {
          LOG_ERROR ("Could not write %s", outfile)
          exit (1);
        }
      out_dat.from_version = dwg.header.from_version;
      out_dat.version = dwg.header.version;
      out_dat.opts = dwg.opts;

#ifndef DISABLE_DXF
      if (opt.dxf && opt.binary)
        error = dwg_write_dxfb (&out_dat, &dwg);
      else if (opt.dxf)
        error = dwg_write_dxf (&out_dat, &dwg);
      else
#endif
#ifndef DISABLE_JSON
          if (opt.json)
        error = dwg_write_json (&out_dat, &dwg);
      else if (opt.geojson)
        error = dwg_write_geojson (&out_dat, &dwg);
      else
#endif
        {
          if (out_dat.fh)
            fclose (out_dat.fh);
          opt.dwg = 1;
          unlink (outfile);
          error = dwg_write_file (outfile, &dwg);
        }
      if (error >= DWG_ERR_CRITICAL)
        {
          LOG_ERROR ("dwgadd -o %s failed with error 0x%x", outfile, error)
          retval = error;
        }
      if (!opt.dwg)
        fclose (out_dat.fh);
      free (out_dat.chain);
      free (dat.chain);
      dwg_free (&dwg);

      if (opt.verify)
        {
          dat.byte = 0;
          dat.bit = 0;
          dat.size = 0;
          dat.opts = opts;
          dat.fh = fopen (outfile, "rb");
          memset (&dwg, 0, sizeof (Dwg_Data));
          dwg.opts = opts;
          LOG_INFO ("\ndwgadd verify %s\n", outfile)
          dat_read_file (&dat, dat.fh, outfile);

#ifndef DISABLE_DXF
          if (opt.dxf && opt.binary)
            error = dwg_read_dxfb (&dat, &dwg);
          else if (opt.dxf)
            error = dwg_read_dxf (&dat, &dwg);
          else
#endif
#ifndef DISABLE_JSON
              if (opt.json)
            error = dwg_read_json (&dat, &dwg);
          else if (opt.geojson)
            LOG_ERROR ("--verify skipped with --geojson")
          else
#endif
            error = dwg_decode (&dat, &dwg); // dat -> dwg
          if (error >= DWG_ERR_CRITICAL)
            {
              LOG_ERROR ("--verify failed with error 0x%x", error)
              retval = error;
            }
          fclose (dat.fh);
          dwg_free (&dwg);
          free (dat.chain);
        }
    }
  else
    {
      fclose (fp);
      dwg_free (&dwg);
      free (dat.chain);
      LOG_ERROR ("dwgadd failed to add objects")
    }

  return retval;
}

static dwg_point_2d *
scan_pts2d (unsigned num_pts, char **pp)
{
  char *p = *pp;
  dwg_point_2d *pts;

  p = strchr (p, '(');
  if (!p)
    return NULL;
  p++;
  while (isspace(*p))
    p++;
  if (num_pts > 5000)
    exit (0);
  pts = (dwg_point_2d *)calloc (num_pts, 16);
  if (!pts)
    exit (0);
  for (unsigned i = 0; i < num_pts; i++)
    {
      while (isspace(*p))
        p++;
      if (2 == SSCANF_S (p, "(%lf %lf)", &pts[i].x, &pts[i].y))
        {
          p = strchr (p, ')');
          if (!p)
            break;
          p++;
        }
      else
        {
          *pp = p;
          p = NULL;
          break;
        }
    }

  if (p)
    {
      if (*p == ')')
        p++;
      while (isspace(*p) && *p != '\n')
        p++;
      *pp = p;
      return pts;
    }
  else
    {
      free (pts);
      return NULL;
    }
}

static dwg_point_3d *
scan_pts3d (unsigned num_pts, char **pp)
{
  char *p = *pp;
  dwg_point_3d *pts;

  p = strchr (p, '(');
  if (!p)
    return NULL;
  p++;
  while (isspace(*p))
    p++;
  if (num_pts > 5000)
    exit (0);
  pts = (dwg_point_3d *)calloc (num_pts, 24);
  if (!pts)
    exit (0);
  for (unsigned i = 0; i < num_pts; i++)
    {
      while (isspace(*p))
        p++;
      if (3 == SSCANF_S (p, "(%lf %lf %lf)", &pts[i].x, &pts[i].y, &pts[i].z))
        {
          p = strchr (p, ')');
          if (!p)
            break;
          p++;
        }
      else
        {
          *pp = p;
          p = NULL;
          break;
        }
    }

  if (p && num_pts)
    {
      if (*p == ')')
        p++;
      while (isspace(*p) && *p != '\n')
        p++;
      *pp = p;
      return pts;
    }
  else
    {
      free (pts);
      return NULL;
    }
}

static dwg_face *
scan_faces (unsigned num, char **pp)
{
  char *p = *pp;
  dwg_face *faces;

  p = strchr (p, '(');
  if (!p)
    return NULL;
  p++;
  while (isspace(*p))
    p++;
  if (num > 5000)
    exit (0);
  faces = (dwg_face *)calloc (num, 4 * sizeof (int));
  if (!faces)
    exit (0);
  for (unsigned i = 0; i < num; i++)
    {
      while (isspace(*p))
        p++;
      if (4 == SSCANF_S (p, "(%hd %hd %hd %hd)", &faces[i][0], &faces[i][1],
                         &faces[i][2], &faces[i][3]))
        {
          p = strchr (p, ')');
          if (!p)
            break;
          p++;
        }
      else
        {
          *pp = p;
          p = NULL;
          break;
        }
    }

  if (p && num)
    {
      if (*p == ')')
        p++;
      while (isspace(*p) && *p != '\n')
        p++;
      *pp = p;
      return faces;
    }
  else
    {
      free (faces);
      return NULL;
    }
}

static char *
next_line (char *restrict p, const char *restrict end)
{
  while (p < end && *p != '\n')
    p++;
  if (p < end)
    p++;
  return p;
}

static ATTRIBUTE_NORETURN void
fn_error (const char *msg)
{
  fprintf (stderr, "%s", msg);
  exit (1);
}

static int
dwg_add_dat (Dwg_Data **dwgp, Bit_Chain *dat)
{
  Dwg_Data *dwg = NULL;
  Dwg_Object *mspace;
  Dwg_Object_BLOCK_HEADER *hdr;
  const char *end;
  char *p;
  Dwg_Version_Type version;
  int error;
  int i = 0;
  int initial = 1;
  int imperial = 0;
  BITCODE_BL orig_num = 0;
  typedef struct
  {
    int type;
    union
    {
      Dwg_Entity_ATTDEF *attdef;
      Dwg_Entity_ATTRIB *attrib;
      Dwg_Entity_LINE *line;
      Dwg_Entity_RAY *ray;
      Dwg_Entity_XLINE *xline;
      Dwg_Entity_CIRCLE *circle;
      Dwg_Entity_ARC *arc;
      Dwg_Entity_POINT *point;
      Dwg_Entity_VIEWPORT *viewport;
      Dwg_Entity_TEXT *text;
      Dwg_Entity_MTEXT *mtext;
      Dwg_Entity_INSERT *insert;
      Dwg_Entity_MINSERT *minsert;
      Dwg_Entity_DIMENSION_LINEAR *dimlin;
      Dwg_Entity_DIMENSION_ALIGNED *dimali;
      Dwg_Entity_DIMENSION_DIAMETER *dimdia;
      Dwg_Entity_DIMENSION_RADIUS *dimrad;
      Dwg_Entity_DIMENSION_ORDINATE *dimord;
      Dwg_Entity_DIMENSION_ANG2LN *dimang2ln;
      Dwg_Entity_DIMENSION_ANG3PT *dimang3pt;
      Dwg_Entity_BLOCK *block;
      Dwg_Entity_ENDBLK *endblk;
      Dwg_Entity__3DFACE *_3dface;
      Dwg_Entity__3DSOLID *_3dsolid;
      Dwg_Entity_SOLID *solid;
      Dwg_Entity_TRACE *trace;
      Dwg_Entity_SHAPE *shape;
      Dwg_Entity_ELLIPSE *ellipse;
      Dwg_Entity_SPLINE *spline;
      Dwg_Entity_LEADER *leader;
      Dwg_Entity_MULTILEADER *mleader;
      Dwg_Entity_TOLERANCE *tolerance;
      Dwg_Entity_POLYLINE_2D *polyline_2d;
      Dwg_Entity_POLYLINE_3D *polyline_3d;
      Dwg_Entity_POLYLINE_MESH *polyline_mesh;
      Dwg_Entity_POLYLINE_PFACE *polyline_pface;
      Dwg_Entity_LWPOLYLINE *lwpolyline;
      Dwg_Entity_MLINE *mline;
      Dwg_Object_DICTIONARY *dictionary;
      Dwg_Object_XRECORD *xrecord;
      Dwg_Object_MLINESTYLE *mlinestyle;
      Dwg_Object_DIMSTYLE *dimstyle;
      Dwg_Object_UCS *ucs;
      Dwg_Object_LAYER *layer;
      Dwg_Object_STYLE *style;
      Dwg_Object_LTYPE *ltype;
      Dwg_Object_VPORT *vport;
      Dwg_Object_VIEW *view;
      Dwg_Object_LAYOUT *layout;
      Dwg_Object_GROUP *group;
    } u;
  } lastent_t;
  lastent_t ent, insert, dict, mtext;
  char text[120];
  char prompt[120];
  char tag[120];
  char default_text[120];
  const char *hdr_s = "mspace";

  if (!dat->chain)
    abort ();
  dwg = *dwgp;
  ent = (lastent_t){ .type = DWG_TYPE_UNUSED, .u.insert = NULL };
  insert = (lastent_t){ .type = DWG_TYPE_UNUSED, .u.insert = NULL };
  dict = (lastent_t){ .type = DWG_TYPE_UNUSED, .u.dictionary = NULL };
  mtext = (lastent_t){ .type = DWG_TYPE_UNUSED, .u.mtext = NULL };
  if (!dwg->header.version)
    version = R_2000;
  else
    version = dwg->header.version;
  memset (&ent, 0, sizeof (lastent_t));
  p = (char *)dat->chain;
  end = (char *)&dat->chain[dat->size - 1];

  // read dat line by line and call the matching add API
  while (p && p < end)
    {
      char s1[120];
      dwg_point_2d p2 = { 0.0, 0.0 };
      dwg_point_2d p3 = { 0.0, 0.0 };
      dwg_point_2d p4 = { 0.0, 0.0 };
      dwg_point_3d pt1 = { 0.0, 0.0, 0.0 };
      dwg_point_3d pt2 = { 0.0, 0.0, 0.0 };
      dwg_point_3d pt3 = { 0.0, 0.0, 0.0 };
      dwg_point_3d pt4 = { 0.0, 0.0, 0.0 };
      dwg_point_3d scale = { 0.0, 0.0, 0.0 };
      double height = 0.0, rot = 0.0, len = 0.0, f1 = 0.0, f2 = 0.0;
      int i1 = 0, i2 = 0, flags = 0;
      unsigned u = 0U;

      while (p < end && (*p == ' ' || *p == '\t'))
        p++; // skip spaces
      if (p == end)
        break;
      if (*p == '\n' || *p == '#')
        { // skip empty lines and comments
          p = next_line (p, end);
          continue;
        }

      if (memBEGINc (p, "readdwg"))
        {
          if (dwg)
            {
              LOG_ERROR ("readdwg seen, but DWG already exists");
              exit (1);
            }
          if (1 == SSCANF_S (p, "readdwg " FMT_PATH, text SZ))
            {
              LOG_INFO ("readdwg %s\n", text)
              if ((error = dwg_read_file (text, *dwgp)) > DWG_ERR_CRITICAL)
                {
                  LOG_ERROR ("Invalid readdwg \"%s\" => error 0x%x", text,
                             error);
                  exit (1);
                }
              dwg = *dwgp;
              p = next_line (p, end);
              continue;
            }
          else
            {
              LOG_ERROR ("readdwg syntax error");
              exit (1);
            }
        }
      else if (memBEGINc (p, "readdxf"))
        {
          if (dwg)
            {
              LOG_ERROR ("readdxf seen, but DWG already exists");
              exit (1);
            }
          if (1 == SSCANF_S (p, "readdxf " FMT_PATH, text SZ))
            {
              LOG_INFO ("readdxf %s\n", text)
              if ((error = dxf_read_file (text, *dwgp)) > DWG_ERR_CRITICAL)
                {
                  LOG_ERROR ("Invalid readdxf \"%s\" => error 0x%x", text,
                             error);
                  exit (1);
                }
              dwg = *dwgp;
              p = next_line (p, end);
              continue;
            }
          else
            {
              LOG_ERROR ("readdxf syntax error");
              exit (1);
            }
        }
      else if (memBEGINc (p, "readjson"))
        {
          if (dwg)
            {
              LOG_ERROR ("readjson seen, but DWG already exists");
              exit (1);
            }
          if (1 == SSCANF_S (p, "readjson " FMT_PATH, text SZ))
            {
              Bit_Chain in_dat = EMPTY_CHAIN (0);
              LOG_INFO ("readjson %s\n", text)
              in_dat.fh = fopen (text, "rb");
              if (in_dat.fh)
                dat_read_file (&in_dat, in_dat.fh, text);
              if (!in_dat.fh
                  || (error = dwg_read_json (&in_dat, *dwgp))
                         > DWG_ERR_CRITICAL)
                {
                  LOG_ERROR ("Invalid readjson \"%s\" => error 0x%x", text,
                             error);
                  exit (1);
                }
              fclose (in_dat.fh);
              dwg = *dwgp;
              p = next_line (p, end);
              continue;
            }
          else
            {
              LOG_ERROR ("readjson syntax error");
              exit (1);
            }
        }
      else if (memBEGINc (p, "imperial"))
        {
          if (!initial)
            {
              LOG_ERROR ("`imperial' directive out of header section");
              exit (1);
            }
          imperial = 1;
          p = next_line (p, end);
          continue;
        }
      else if (memBEGINc (p, "version"))
        {
          int i_ver;
          double f_ver;
          char s_ver[16];

          if (!initial)
            {
              LOG_ERROR ("`version' directive out of header section");
              exit (1);
            }

          i = SSCANF_S (p, "version %d", &i_ver);
          if (i)
            {
              snprintf (s_ver, 16, "r%d", i_ver);
              s_ver[15] = '\0';
              version = dwg_version_as (s_ver);
              p += strlen ("version ");
            }
          else if ((i = SSCANF_S (p, "version %lf", &f_ver)))
            {
              snprintf (s_ver, 16, "r%f", f_ver);
              s_ver[15] = '\0';
              version = dwg_version_as (s_ver);
            }
          if (!i || version >= R_AFTER)
            {
              fprintf (stderr, "Invalid version %.*s", 40, p);
              exit (1);
            }
          p = next_line (p, end);
          continue;
        }

      if (initial)
        {
          if (!dwg)
            dwg = dwg_new_Document (version, imperial, 0);
          else
            dwg_add_Document (dwg, imperial);
          *dwgp = dwg;

          mspace = dwg_model_space_object (dwg);
          hdr = mspace->tio.object->tio.BLOCK_HEADER;
          orig_num = dwg->num_objects;
          initial = 0;
        }

// set entity/object field values.
#define SET_ENT(var, name)                                                    \
  if (4                                                                       \
      == SSCANF_S (p, #var "." FMT_NAME " = %d.%d.%X\n", s1 SZ, &i1, &i2,     \
                   &u))                                                       \
    {                                                                         \
      BITCODE_H hdl;                                                          \
      if (!ent.u.var || ent.type != DWG_TYPE_##name)                          \
        {                                                                     \
          log_p (DWG_LOGLEVEL_ERROR, p);                                      \
          LOG_ERROR ("wrong last entity type 0x%x, needing " #var "\n",       \
                     ent.type);                                               \
          exit (1);                                                           \
        }                                                                     \
      hdl = dwg_add_handleref (dwg, i1, u, NULL);                             \
      dwg_dynapi_entity_set_value (ent.u.var, #name, s1, hdl, 0);             \
      LOG_TRACE (#var ".%s = %d.%d.%X\n", s1, i1, i2, u);                     \
    }                                                                         \
  else if (2 == SSCANF_S (p, #var "." FMT_NAME " = %d\n", s1 SZ, &i1))        \
    {                                                                         \
      if (!ent.u.var || ent.type != DWG_TYPE_##name)                          \
        {                                                                     \
          log_p (DWG_LOGLEVEL_ERROR, p);                                      \
          LOG_ERROR ("wrong last entity type 0x%x, needing " #var "\n",       \
                     ent.type);                                               \
          exit (1);                                                           \
        }                                                                     \
      if (dwg_dynapi_is_angle (#name, s1))                                    \
        f1 = deg2rad (i1);                                                    \
      dwg_dynapi_entity_set_value (ent.u.var, #name, s1, &f1, 0);             \
      LOG_TRACE (#var ".%s = %d\n", s1, i1);                                  \
    }                                                                         \
  else if (2 == SSCANF_S (p, #var "." FMT_NAME " = %lf\n", s1 SZ, &f1))       \
    {                                                                         \
      if (!ent.u.var || ent.type != DWG_TYPE_##name)                          \
        {                                                                     \
          log_p (DWG_LOGLEVEL_ERROR, p);                                      \
          LOG_ERROR ("wrong last entity type 0x%x, needing " #var "\n",       \
                     ent.type);                                               \
          exit (1);                                                           \
        }                                                                     \
      if (dwg_dynapi_is_angle (#name, s1))                                    \
        f1 = deg2rad (f1);                                                    \
      dwg_dynapi_entity_set_value (ent.u.var, #name, s1, &f1, 0);             \
      LOG_TRACE (#var ".%s = %f\n", s1, f1);                                  \
    }                                                                         \
  else if (2                                                                  \
           == SSCANF_S (p, #var "." FMT_NAME " = " FMT_ANY "\n", s1 SZ,       \
                        text SZ))                                             \
    {                                                                         \
      if (strlen (text) && text[strlen (text) - 1] == '"')                    \
        text[strlen (text) - 1] = '\0';                                       \
      if (!ent.u.var || ent.type != DWG_TYPE_##name)                          \
        {                                                                     \
          log_p (DWG_LOGLEVEL_ERROR, p);                                      \
          LOG_ERROR ("wrong last entity type 0x%x, needing " #var "\n",       \
                     ent.type);                                               \
          exit (1);                                                           \
        }                                                                     \
      dwg_dynapi_entity_set_value (ent.u.var, #name, s1, text, 1);            \
      LOG_TRACE (#var ".%s = \"%s\"\n", s1, text);                            \
    }

      if (memBEGINc (p, "pspace\n"))
        {
          Dwg_Object *pspace = dwg_paper_space_object (dwg);
          hdr = pspace->tio.object->tio.BLOCK_HEADER;
          hdr_s = "pspace";
          LOG_TRACE ("pspace\n");
        }
      else if (memBEGINc (p, "mspace\n"))
        {
          hdr = mspace->tio.object->tio.BLOCK_HEADER;
          hdr_s = "mspace";
          LOG_TRACE ("mspace\n");
        }
      else if (8 == SSCANF_S (p,
                         "attdef %lf %d " FMT_ANY " (%lf %lf %lf) " FMT_TAG
                         " " FMT_ANY,
                         &height, &flags, prompt SZ, &pt1.x, &pt1.y,
                         &pt1.z, tag SZ, default_text SZ))
        {
          if (version < R_2_0b)
            fn_error ("Invalid entity ATTDEF <r2.0b\n");
          LOG_TRACE ("add_ATTDEF %s %f %d \"%s\" (%f %f %f) %s \"%s\"\n",
                     hdr_s, height, flags, prompt, pt1.x, pt1.y, pt1.z,
                     tag, default_text);
          ent = (lastent_t){ .u.attdef
                             = dwg_add_ATTDEF (hdr, height, flags, prompt,
                                               &pt1, tag, default_text),
                             .type = DWG_TYPE_ATTDEF };
        }
      else
        // clang-format off
        SET_ENT (attdef, ATTDEF)
      // clang-format on
      else if (7 == SSCANF_S (p,
                         "attrib %lf %d (%lf %lf %lf) " FMT_TAG
                         " " FMT_ANY,
                         &height, &flags, &pt1.x, &pt1.y,
                         &pt1.z, tag SZ, text SZ))
        {
          if (version < R_2_0b)
            fn_error ("Invalid entity ATTRIB <r2.0b\n");
          if (insert.type == DWG_TYPE_UNUSED)
            {
              log_p (DWG_LOGLEVEL_ERROR, p);
              fn_error ("Missing INSERT for ATTRIB\n");
            }
          LOG_TRACE ("add_ATTRIB insert %f %d (%f %f %f) %s \"%s\"\n",
                     height, flags, pt1.x, pt1.y, pt1.z, tag, text);
          ent = (lastent_t){ .u.attrib
                             = dwg_add_ATTRIB (insert.u.insert, height,
                                               flags, &pt1, tag, text),
                             .type = DWG_TYPE_ATTRIB };
        }
      else
        // clang-format off
        SET_ENT (attrib, ATTRIB)
      // clang-format on
      else if (6 == SSCANF_S (p, "line (%lf %lf %lf) (%lf %lf %lf)", &pt1.x, &pt1.y,
                         &pt1.z, &pt2.x, &pt2.y, &pt2.z))
        {
          LOG_TRACE ("add_LINE %s (%f %f %f) (%f %f %f)\n",
                     hdr_s, pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z);
          ent = (lastent_t){ .u.line = dwg_add_LINE (hdr, &pt1, &pt2),
                         .type = DWG_TYPE_LINE };
        }
      else
          // clang-format off
        SET_ENT (line, LINE)
      // clang-format on
      else if (6 == SSCANF_S (p, "ray (%lf %lf %lf) (%lf %lf %lf)", &pt1.x, &pt1.y,
                         &pt1.z, &pt2.x, &pt2.y, &pt2.z))
      {
        if (version <= R_11)
          fn_error ("Invalid entity RAY <r13\n");
        LOG_TRACE ("add_RAY %s (%f %f %f) (%f %f %f)\n",
                   hdr_s, pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z);
        ent = (lastent_t){ .u.ray = dwg_add_RAY (hdr, &pt1, &pt2),
                           .type = DWG_TYPE_RAY };
      }
      else
          // clang-format off
        SET_ENT (ray, RAY)
      // clang-format on
      else if (6 == SSCANF_S (p, "xline (%lf %lf %lf) (%lf %lf %lf)", &pt1.x,
                         &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z))
      {
        if (version <= R_11)
          fn_error ("Invalid entity XLINE\n");
        LOG_TRACE ("add_XLINE %s (%f %f %f) (%f %f %f)\n",
                   hdr_s, pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z);
        ent = (lastent_t){ .u.xline = dwg_add_XLINE (hdr, &pt1, &pt2),
                           .type = DWG_TYPE_XLINE };
      }
      else
          // clang-format off
        SET_ENT (xline, XLINE)
      // clang-format on
      else if ((i = SSCANF_S (p, "text " FMT_ANY " (%lf %lf %lf) %lf\n", text,
                              &pt1.x, &pt1.y, &pt1.z, &height)) >= 5)
      {
        if (strlen (text) && text[strlen (text) - 1] == '"')
          text[strlen (text) - 1] = '\0'; // strip the \"
        LOG_TRACE ("add_TEXT %s %s (%f %f %f) %f\n", hdr_s, text, pt1.x,
                   pt1.y, pt1.z, height);
        ent = (lastent_t){ .u.text = dwg_add_TEXT (hdr, text, &pt1, height),
                           .type = DWG_TYPE_TEXT };
      }
      else
          // clang-format off
        SET_ENT (text, TEXT)
      // clang-format on
      else if (5 == SSCANF_S (p, "mtext (%lf %lf %lf) %lf " FMT_ANY, &pt1.x, &pt1.y,
                         &pt1.z, &height, text SZ))
      {
        if (strlen (text) && text[strlen (text) - 1] == '"')
          text[strlen (text) - 1] = '\0'; // strip the \"
        if (version <= R_11)
          fn_error ("Invalid entity MTEXT\n");
        LOG_TRACE ("add_MTEXT %s (%f %f %f) %f \"%s\"\n",
                   hdr_s, pt1.x, pt1.y, pt1.z, height, text);
        mtext = ent = (lastent_t){ .u.mtext = dwg_add_MTEXT (hdr, &pt1,
                                                             height, text),
                                   .type = DWG_TYPE_MTEXT };
      }
      else
        // clang-format off
        SET_ENT (mtext, MTEXT)
        // clang-format on
      else if (1 == SSCANF_S (p, "block " FMT_TBL, text SZ))
      {
        if (strlen (text) && text[strlen (text) - 1] == '"')
          text[strlen (text) - 1] = '\0'; // strip the \"
        ent = (lastent_t){ .u.block = dwg_add_BLOCK (hdr, text),
                           .type = DWG_TYPE_BLOCK };
      }
      else
        // clang-format off
        SET_ENT (block, BLOCK)
        // clang-format on
      else if (memBEGINc (p, "endblk\n"))
        {
          LOG_TRACE ("add_ENDBLK\n");
          dwg_add_ENDBLK (hdr);
        }
      else
          // clang-format off
        SET_ENT (endblk, ENDBLK)
      // clang-format on
      else if (8 == SSCANF_S (p, "insert (%lf %lf %lf) " FMT_TBL " %lf %lf %lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, text SZ, &scale.x,
                         &scale.y, &scale.z, &rot))
      {
        LOG_TRACE ("add_INSERT %s (%f %f %f) \"%s\" %f %f %f %f\n", hdr_s,
                   pt1.x, pt1.y, pt1.z, text, scale.x, scale.y, scale.z,
                   deg2rad (rot));
        insert = ent = (lastent_t){ .u.insert = dwg_add_INSERT (
                                        hdr, &pt1, text, scale.x, scale.y,
                                        scale.z, deg2rad (rot)),
                                    .type = DWG_TYPE_INSERT };
      }
      else
          // clang-format off
        SET_ENT (insert, INSERT)
      // clang-format on
      else if (12 == SSCANF_S (p,
                         "minsert (%lf %lf %lf) " FMT_TBL
                         " %lf %lf %lf %lf %d %d "
                         "%lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, text SZ, &scale.x,
                         &scale.y, &scale.z, &rot, &i1, &i2, &f1, &f2))
      {
        if (version <= R_11)
          fn_error ("Invalid entity MINSERT\n");
        LOG_TRACE (
            "add_MINSERT %s (%f %f %f) \"%s\" %f %f %f %f %d %d %f %f\n",
            hdr_s, pt1.x, pt1.y, pt1.z, text, scale.x, scale.y, scale.z,
            deg2rad (rot), i1, i2, f1, f2);
        insert = ent
            = (lastent_t){ .u.minsert = dwg_add_MINSERT (
                               hdr, &pt1, text, scale.x, scale.y, scale.z,
                               deg2rad (rot), i1, i2, f1, f2),
                           .type = DWG_TYPE_MINSERT };
      }
      else
          // clang-format off
        SET_ENT (minsert, MINSERT)
      // clang-format on
      else if (3 == SSCANF_S (p, "point (%lf %lf %lf)", &pt1.x, &pt1.y, &pt1.z))
      {
        LOG_TRACE ("add_POINT %s (%f %f %f)\n", hdr_s, pt1.x, pt1.y, pt1.z);
        ent = (lastent_t){ .u.point = dwg_add_POINT (hdr, &pt1),
                           .type = DWG_TYPE_POINT };
      }
      else
          // clang-format off
        SET_ENT (point, POINT)
      // clang-format on
      else if (4 == SSCANF_S (p, "circle (%lf %lf %lf) %lf", &pt1.x, &pt1.y,
                              &pt1.z, &f1))
      {
        LOG_TRACE ("add_CIRCLE %s (%f %f %f) %f\n", hdr_s, pt1.x, pt1.y, pt1.z, f1);
        ent = (lastent_t){ .u.circle = dwg_add_CIRCLE (hdr, &pt1, f1),
                           .type = DWG_TYPE_CIRCLE };
      }
      else
          // clang-format off
        SET_ENT (circle, CIRCLE)
      // clang-format on
      else if (6 == SSCANF_S (p, "arc (%lf %lf %lf) %lf %lf %lf", &pt1.x, &pt1.y,
                              &pt1.z, &len, &f1, &f2))
      {
        LOG_TRACE ("add_ARC %s (%f %f %f) %f %f %f\n", hdr_s, pt1.x, pt1.y, pt1.z,
                   len, deg2rad (f1), deg2rad (f2));
        ent = (lastent_t){ .u.arc = dwg_add_ARC (hdr, &pt1, len, deg2rad (f1),
                                                 deg2rad (f2)),
                           .type = DWG_TYPE_ARC };
      }
      else
          // clang-format off
        SET_ENT (arc, ARC)
      // clang-format on
      else if (9 == SSCANF_S (p,
                         "dimension_aligned (%lf %lf %lf) (%lf %lf %lf) (%lf "
                         "%lf %lf)",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z,
                         &pt3.x, &pt3.y, &pt3.z))
      {
        LOG_TRACE ("add_DIMENSION_ALIGNED %s (%f %f %f) (%f %f %f) (%f %f %f)\n",
                   hdr_s, pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z,
                   pt3.x, pt3.y, pt3.z);
        ent = (lastent_t){ .u.dimali
                           = dwg_add_DIMENSION_ALIGNED (hdr, &pt1, &pt2, &pt3),
                           .type = DWG_TYPE_DIMENSION_ALIGNED };
      }
      else
          // clang-format off
        SET_ENT (dimali, DIMENSION_ALIGNED)
      // clang-format on
      else if (10 == SSCANF_S (
                   p,
                   "dimension_linear (%lf %lf %lf) (%lf %lf %lf) (%lf %lf "
                   "%lf) %lf",
                   &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &pt3.x,
                   &pt3.y, &pt3.z, &rot))
      {
        LOG_TRACE ("add_DIMENSION_LINEAR %s (%f %f %f) (%f %f %f) (%f %f %f) %f\n",
                   hdr_s, pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z,
                   pt3.x, pt3.y, pt3.z, deg2rad (rot));
        ent = (lastent_t){ .u.dimlin = dwg_add_DIMENSION_LINEAR (
                               hdr, &pt1, &pt2, &pt3, deg2rad (rot)),
                           .type = DWG_TYPE_DIMENSION_LINEAR };
      }
      else
          // clang-format off
        SET_ENT (dimlin, DIMENSION_LINEAR)
      // clang-format on
      else if (12 == SSCANF_S (
                   p,
                   "dimension_ang2ln (%lf %lf %lf) (%lf %lf %lf) (%lf %lf "
                   "%lf) (%lf %lf %lf)",
                   &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &pt3.x,
                   &pt3.y, &pt3.z, &pt4.x, &pt4.y, &pt4.z))
      {
        LOG_TRACE ("add_DIMENSION_ANG2LN %s (%f %f %f) (%f %f %f) (%f %f %f) "
                   "(%f %f %f)\n",
                   hdr_s, pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z, pt3.x,
                   pt3.y, pt3.z, pt4.x, pt4.y, pt4.z);
        ent = (lastent_t){ .u.dimang2ln = dwg_add_DIMENSION_ANG2LN (
                               hdr, &pt1, &pt2, &pt3, &pt4),
                           .type = DWG_TYPE_DIMENSION_ANG2LN };
      }
      else
          // clang-format off
        SET_ENT (dimang2ln, DIMENSION_ANG2LN)
      // clang-format on
      else if (12 == SSCANF_S (
                   p,
                   "dimension_ang3pt (%lf %lf %lf) (%lf %lf %lf) (%lf %lf "
                   "%lf) (%lf %lf %lf)",
                   &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &pt3.x,
                   &pt3.y, &pt3.z, &pt4.x, &pt4.y, &pt4.z))
      {
        LOG_TRACE ("add_DIMENSION_ANG3PT %s (%f %f %f) (%f %f %f) (%f %f %f) "
                   "(%f %f %f)\n",
                   hdr_s, pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z, pt3.x,
                   pt3.y, pt3.z, pt4.x, pt4.y, pt4.z);
        ent = (lastent_t){ .u.dimang3pt = dwg_add_DIMENSION_ANG3PT (
                               hdr, &pt1, &pt2, &pt3, &pt4),
                           .type = DWG_TYPE_DIMENSION_ANG3PT };
      }
      else
          // clang-format off
        SET_ENT (dimang3pt, DIMENSION_ANG3PT)
      // clang-format on
      else if (7 == SSCANF_S (p,
                         "dimension_diameter (%lf %lf %lf) (%lf %lf %lf) %lf",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &len))
      {
        LOG_TRACE ("add_DIMENSION_DIAMETER %s (%f %f %f) (%f %f %f) %f\n",
                   hdr_s, pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z, len);
        ent = (lastent_t){ .u.dimdia
                           = dwg_add_DIMENSION_DIAMETER (hdr, &pt1, &pt2, len),
                           .type = DWG_TYPE_DIMENSION_DIAMETER };
      }
      else
          // clang-format off
        SET_ENT (dimdia, DIMENSION_DIAMETER)
      // clang-format on
      else if (7 == SSCANF_S (p,
                         "dimension_ordinate (%lf %lf %lf) (%lf %lf %lf) %d",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &i1))
      {
        LOG_TRACE ("add_DIMENSION_ORDINATE %s (%f %f %f) (%f %f %f) %s\n",
                   hdr_s, pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z,
                   i1 ? "true" : "false");
        ent = (lastent_t){ .u.dimord = dwg_add_DIMENSION_ORDINATE (
                               hdr, &pt1, &pt2, i1 ? true : false),
                           .type = DWG_TYPE_DIMENSION_ORDINATE };
      }
      else
          // clang-format off
        SET_ENT (dimord, DIMENSION_ORDINATE)
      // clang-format on
      else if (7 == SSCANF_S (p, "dimension_radius (%lf %lf %lf) (%lf %lf %lf) %lf",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &len))
      {
        LOG_TRACE ("add_DIMENSION_RADIUS %s (%f %f %f) (%f %f %f) %f\n",
                   hdr_s, pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z, len);
        ent = (lastent_t){ .u.dimrad
                         = dwg_add_DIMENSION_RADIUS (hdr, &pt1, &pt2, len),
                         .type = DWG_TYPE_DIMENSION_RADIUS };
      }
      else
          // clang-format off
        SET_ENT (dimrad, DIMENSION_RADIUS)
      // clang-format on
      else if (12 == SSCANF_S (p,
                         "3dface (%lf %lf %lf) (%lf %lf %lf) (%lf %lf "
                         "%lf) (%lf %lf %lf)",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z,
                         &pt3.x, &pt3.y, &pt3.z, &pt4.x, &pt4.y, &pt4.z))
      {
        LOG_TRACE ("add_3DFACE %s (%f %f %f) (%f %f %f) (%f %f %f) (%f %f %f)\n",
                   hdr_s, pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z, pt3.x,
                   pt3.y, pt3.z, pt4.x, pt4.y, pt4.z);
        ent = (lastent_t){ .u._3dface
                           = dwg_add_3DFACE (hdr, &pt1, &pt2, &pt3, &pt4),
                           .type = DWG_TYPE__3DFACE };
      }
      else if (9 == SSCANF_S (p,
                         "3dface (%lf %lf %lf) (%lf %lf %lf) (%lf %lf "
                         "%lf)",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z,
                         &pt3.x, &pt3.y, &pt3.z))
      {
        LOG_TRACE ("add_3DFACE %s (%f %f %f) (%f %f %f) (%f %f %f) NULL\n",
                   hdr_s, pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z, pt3.x,
                   pt3.y, pt3.z);
        ent = (lastent_t){ .u._3dface
                           = dwg_add_3DFACE (hdr, &pt1, &pt2, &pt3, NULL),
                           .type = DWG_TYPE__3DFACE };
      }
      else
          // clang-format off
        SET_ENT (_3dface, _3DFACE)
      // clang-format on
      else if (9 == SSCANF_S (p,
                         "solid (%lf %lf %lf) (%lf %lf) (%lf %lf)  (%lf %lf)",
                         &pt1.x, &pt1.y, &pt1.z, &p2.x, &p2.y, &p3.x, &p3.y,
                         &p4.x, &p4.y))
      {
        LOG_TRACE ("add_SOLID %s (%f %f %f) (%f %f %f) (%f %f %f) (%f %f %f)\n",
                   hdr_s, pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z, pt3.x,
                   pt3.y, pt3.z, pt4.x, pt4.y, pt4.z);
        ent = (lastent_t){ .u.solid = dwg_add_SOLID (hdr, &pt1, &p2, &p3, &p4),
                           .type = DWG_TYPE_SOLID };
      }
      else
          // clang-format off
        SET_ENT (solid, SOLID)
      // clang-format on
      else if (9 == SSCANF_S (p,
                         "trace (%lf %lf %lf) (%lf %lf) (%lf %lf)  (%lf %lf)",
                         &pt1.x, &pt1.y, &pt1.z, &p2.x, &p2.y, &p3.x, &p3.y,
                         &p4.x, &p4.y))
        {
          LOG_TRACE ("add_TRACE %s (%f %f %f) (%f %f) (%f %f) (%f %f)\n",
                     hdr_s, pt1.x, pt1.y, pt1.z, p2.x, p2.y, p3.x, p3.y,
                     p4.x, p4.y);
          ent = (lastent_t){ .u.trace = dwg_add_TRACE (hdr, &pt1, &p2, &p3, &p4),
                             .type = DWG_TYPE_TRACE };
        }
      else
          // clang-format off
        SET_ENT (trace, TRACE)
      // clang-format on
      else if (3 == SSCANF_S (p, "polyline_2d %d ((%lf %lf)", &i1, &pt1.x, &pt1.y))
      {
        dwg_point_2d *pts = scan_pts2d (i1, &p);
        if (i1 && pts)
          {
            LOG_TRACE ("add_POLYLINE_2D %s %d ((%f %f)", hdr_s, i1, pt1.x,
                       pt1.y);
            for (i = 1; i < i1; i++)
              {
                LOG_TRACE (" (%f %f)", pts[i].x, pts[i].y);
              }
            LOG_TRACE (")\n");
            ent = (lastent_t){ .u.polyline_2d
                               = dwg_add_POLYLINE_2D (hdr, i1, pts),
                               .type = DWG_TYPE_POLYLINE_2D };
            free (pts);
          }
      }
      else
          // clang-format off
        SET_ENT (polyline_2d, POLYLINE_2D)
      // clang-format on
      else if (4 == SSCANF_S (p, "polyline_3d %d ((%lf %lf %lf)", &i1, &pt1.x,
                              &pt1.y, &pt1.z))
      {
        dwg_point_3d *pts = scan_pts3d (i1, &p);
        if (i1 && pts)
          {
            LOG_TRACE ("add_POLYLINE_3D %s %d ((%f %f %f)", hdr_s, i1, pt1.x,
                       pt1.y, pt1.z);
            for (i = 1; i < i1; i++)
              {
                LOG_TRACE (" (%f %f %f)", pts[i].x, pts[i].y, pts[i].z);
              }
            LOG_TRACE (")\n");
            ent = (lastent_t){ .u.polyline_3d
                               = dwg_add_POLYLINE_3D (hdr, i1, pts),
                               .type = DWG_TYPE_POLYLINE_3D };
            free (pts);
          }
      }
      else
          // clang-format off
        SET_ENT (polyline_3d, POLYLINE_3D)
      // clang-format on
      else if (5 == SSCANF_S (p, "polyline_mesh %d %d ((%lf %lf %lf)", &i1, &i2,
                              &pt1.x, &pt1.y, &pt1.z))
      {
        dwg_point_3d *pts = scan_pts3d (i1 * i2, &p);
        if (i1 && i2 && pts)
          {
            LOG_TRACE ("add_POLYLINE_MESH %s %d %d ((%f %f %f)",
                       hdr_s, i1, i2, pt1.x, pt1.y, pt1.z);
            for (i = 1; i < i1; i++)
              {
                LOG_TRACE (" (%f %f %f)", pts[i].x, pts[i].y, pts[i].z);
              }
            LOG_TRACE (")\n");
            ent = (lastent_t){ .u.polyline_mesh
                               = dwg_add_POLYLINE_MESH (hdr, i1, i2, pts),
                               .type = DWG_TYPE_POLYLINE_MESH };
            free (pts);
          }
      }
      else
          // clang-format off
        SET_ENT (polyline_mesh, POLYLINE_MESH)
      // clang-format on
      else if (5 == SSCANF_S (p, "polyline_pface %d %d ((%lf %lf %lf)", &i1, &i2,
                              &pt1.x, &pt1.y, &pt1.z))
      {
        int face[4];
        dwg_face *faces = NULL;
        dwg_point_3d *verts = scan_pts3d (i1, &p);
        if (4 == SSCANF_S (p, "((%d %d %d %d)", &face[0], &face[1], &face[2],
                           &face[3]))
          faces = scan_faces (i2, &p);
        else if (i2) // else no faces
          {
            log_p (DWG_LOGLEVEL_ERROR, p);
            LOG_ERROR ("reading faces");
          }
        if (i1 && i2 && verts && faces)
          {
            LOG_TRACE ("add_POLYLINE_PFACE %s %d %d ((%f %f %f)",
                       hdr_s, i1, i2, pt1.x, pt1.y, pt1.z);
            for (i=1; i < i1; i++)
              {
                LOG_TRACE (" (%f %f %f)", verts[i].x, verts[i].y, verts[i].z);
              }
            LOG_TRACE (") (");
            for (i=0; i < i2; i++)
              {
                LOG_TRACE (" (%d %d %d %d)", faces[i][0], faces[i][1],
                           faces[i][2], faces[i][3]);
              }
            LOG_TRACE (")\n");
            ent = (lastent_t){ .u.polyline_pface
                               = dwg_add_POLYLINE_PFACE (hdr, i1, i2, verts, faces),
                               .type = DWG_TYPE_POLYLINE_PFACE };
            free (verts);
            free (faces);
          }
      }
      else
          // clang-format off
        SET_ENT (polyline_pface, POLYLINE_PFACE)
      // clang-format on
      else if (3 == SSCANF_S (p, "lwpolyline %d ((%lf %lf)", &i1, &pt1.x, &pt1.y))
      {
        dwg_point_2d *pts = scan_pts2d (i1, &p);
        if (i1 && pts)
          {
            LOG_TRACE ("add_LWPOLYLINE %s %d ((%f %f)",
                       hdr_s, i1, pt1.x, pt1.y);
            for (i = 1; i < i1; i++)
              {
                LOG_TRACE (" (%f %f)", pts[i].x, pts[i].y);
              }
            LOG_TRACE (")\n");
            ent = (lastent_t){ .u.lwpolyline
                               = dwg_add_LWPOLYLINE (hdr, i1, pts),
                               .type = DWG_TYPE_LWPOLYLINE };
            free (pts);
          }
      }
      else
        // clang-format off
        SET_ENT (lwpolyline, LWPOLYLINE)
        // clang-format on
      else if (4 == SSCANF_S (p, "mline %d ((%lf %lf %lf)", &i1, &pt1.x,
                              &pt1.y, &pt1.z))
      {
        dwg_point_3d *pts = scan_pts3d (i1, &p);
        if (i1 && pts)
          {
            LOG_TRACE ("add_MLINE %s %d ((%f %f %f)", hdr_s, i1, pt1.x,
                       pt1.y, pt1.z);
            for (i = 1; i < i1; i++)
              {
                LOG_TRACE (" (%f %f %f)", pts[i].x, pts[i].y, pts[i].z);
              }
            LOG_TRACE (")\n");
            ent = (lastent_t){ .u.mline = dwg_add_MLINE (hdr, i1, pts),
                               .type = DWG_TYPE_MLINE };
            free (pts);
          }
      }
      else
        // clang-format off
        SET_ENT (mline, MLINE)
        // clang-format on
      else if (3 == SSCANF_S (p, "dictionary " FMT_TBL " " FMT_TBL " %u",
                              text SZ, s1 SZ, &u))
      {
        LOG_TRACE ("add_DICTIONARY \"%s\" \"%s\" %u\n", text, s1, u);
        dict = ent = (lastent_t){ .u.dictionary = dwg_add_DICTIONARY (
                                      dwg, text, s1, (unsigned long)u),
                                  .type = DWG_TYPE_DICTIONARY };
      }
      else
          // clang-format off
        SET_ENT (dictionary, DICTIONARY)
      // clang-format on
      else if (1 == SSCANF_S (p, "xrecord dictionary " FMT_TBL, text SZ))
      {
        if (dict.type != DWG_TYPE_DICTIONARY)
          fn_error ("xrecord: missing dictionary\n");
        LOG_TRACE ("add_XRECORD dictionary \"%s\"\n", text);
        ent = (lastent_t){ .u.xrecord
                             = dwg_add_XRECORD (dict.u.dictionary, text),
                           .type = DWG_TYPE_XRECORD };
      }
      else
          // clang-format off
        SET_ENT (xrecord, XRECORD)
      // clang-format on
      else if (6 == SSCANF_S (p, "shape " FMT_PATH " (%lf %lf %lf) %lf %lf",
                              text SZ, &pt1.x, &pt1.y, &pt1.z, &scale.x, &rot))
      {
        LOG_TRACE ("add_SHAPE %s \"%s\" (%f %f %f) %f %f\n", hdr_s, text,
                   pt1.x, pt1.y, pt1.z, scale.x, deg2rad (rot));
        ent = (lastent_t){ .u.shape = dwg_add_SHAPE (hdr, text, &pt1, scale.x,
                                                     deg2rad (rot)),
                           .type = DWG_TYPE_SHAPE };
      }
      else
          // clang-format off
        SET_ENT (shape, SHAPE)
      // clang-format on
      else if (1 == SSCANF_S (p, "viewport " FMT_TBL, text SZ))
      {
        LOG_TRACE ("add_VIEWPORT %s \"%s\"\n", hdr_s, text);
        ent = (lastent_t){ .u.viewport = dwg_add_VIEWPORT (hdr, text),
                           .type = DWG_TYPE_VIEWPORT };
      }
      else
          // clang-format off
        SET_ENT (viewport, VIEWPORT)
      // clang-format on
      else if (5 == SSCANF_S (p, "ellipse (%lf %lf %lf) %lf %lf", &pt1.x, &pt1.y,
                         &pt1.z, &f1, &f2))
      {
        if (version <= R_11)
          fn_error ("Invalid entity ELLIPSE <r13\n");
        LOG_TRACE ("add_ELLIPSE %s (%f %f %f) %f %f\n", hdr_s, pt1.x,
                   pt1.y, pt1.z, f1, f2);
        ent = (lastent_t){ .u.ellipse = dwg_add_ELLIPSE (hdr, &pt1, f1, f2),
                           .type = DWG_TYPE_ELLIPSE };
      }
      else
          // clang-format off
        SET_ENT (ellipse, ELLIPSE)
      // clang-format on
      else if (4
               == SSCANF_S (p, "spline %d ((%lf %lf %lf)", &i1, &pt1.x, &pt1.y,
                            &pt1.z))
      {
        dwg_point_3d *fitpts;
        if (version <= R_11)
          fn_error ("Invalid entity SPLINE <r13\n");
        fitpts = scan_pts3d (i1, &p);
        if (i1 && fitpts
            && SSCANF_S (p, ") (%lf %lf %lf) (%lf %lf %lf)", &pt2.x, &pt2.y,
                         &pt2.z, &pt3.x, &pt3.y, &pt3.z))
          {
            LOG_TRACE ("add_SPLINE %s %d ((%f %f %f)", hdr_s, i1, pt1.x, pt1.y,
                       pt1.z);
            for (i = 1; i < i1; i++)
              {
                LOG_TRACE (" (%f %f %f)", fitpts[i].x, fitpts[i].y,
                           fitpts[i].z);
              }
            LOG_TRACE (" (%f %f %f) (%f %f %f)\n", pt2.x, pt2.y, pt2.z, pt3.x,
                       pt3.y, pt3.z);
            ent = (lastent_t){ .u.spline
                               = dwg_add_SPLINE (hdr, i1, fitpts, &pt2, &pt3),
                               .type = DWG_TYPE_SPLINE };
          }
        free (fitpts);
      }
      else
          // clang-format off
        SET_ENT (spline, SPLINE)
      // clang-format on
      else if (ent.type == DWG_TYPE_MTEXT
               && SSCANF_S (p, "leader %d ((%lf %lf %lf)", &i1, &pt1.x, &pt1.y,
                            &pt1.z))
      {
        dwg_point_3d *pts;
        if (version <= R_11)
          fn_error ("Invalid entity LEADER <r13\n");
        if (mtext.type != DWG_TYPE_MTEXT)
          fn_error ("LEADER: Missing mtext\n");
        pts = scan_pts3d (i1, &p);
        if (i1 && pts && SSCANF_S (p, ") mtext %d", &i2))
          {
            LOG_TRACE ("add_LEADER %s %d ((%f %f %f)", hdr_s, i1, pt1.x, pt1.y,
                       pt1.z);
            for (i = 1; i < i1; i++)
              {
                LOG_TRACE (" (%f %f %f)", pts[i].x, pts[i].y, pts[i].z);
              }
            LOG_TRACE (" mtext %d\n", i2);
            ent = (lastent_t){ .u.leader = dwg_add_LEADER (hdr, i1, pts,
                                                           mtext.u.mtext, i2),
                               .type = DWG_TYPE_LEADER };
          }
        free (pts);
      }
      else
          // clang-format off
        SET_ENT (leader, LEADER)
      // clang-format on
      else if (7 == SSCANF_S (p,
                         "tolerance " FMT_TBL " (%lf %lf %lf) (%lf %lf %lf)",
                         text SZ, &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y,
                         &pt2.z))
      {
        if (version <= R_11)
          fn_error ("Invalid entity TOLERANCE <r13\n");
        LOG_TRACE ("add_TOLERANCE %s \"%s\" (%f %f %f) (%f %f %f)\n", hdr_s, text,
                   pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z);
        ent = (lastent_t){ .u.tolerance
                           = dwg_add_TOLERANCE (hdr, text, &pt1, &pt2),
                           .type = DWG_TYPE_TOLERANCE };
      }
      else
          // clang-format off
        SET_ENT (tolerance, TOLERANCE)
      // clang-format on
      else if (1 == SSCANF_S (p, "mlinestyle " FMT_TBL, text SZ))
      {
        if (version <= R_11)
          fn_error ("Invalid entity MLINESTYLE <r13\n");
        LOG_TRACE ("add_MLINESTYLE \"%s\"\n", text);
        ent = (lastent_t){ .u.mlinestyle = dwg_add_MLINESTYLE (dwg, text),
                           .type = DWG_TYPE_MLINESTYLE };
      }
      else
          // clang-format off
        SET_ENT (mlinestyle, MLINESTYLE)
      // clang-format on
      else if (1 == SSCANF_S (p, "layer " FMT_TBL, text SZ))
      {
        LOG_TRACE ("add_LAYER \"%s\"\n", text);
        ent = (lastent_t){ .u.layer = dwg_add_LAYER (dwg, text),
                           .type = DWG_TYPE_LAYER };
      }
      else
          // clang-format off
        SET_ENT (layer, LAYER)
      // clang-format on
      else if (1 == SSCANF_S (p, "style " FMT_TBL, text SZ))
      {
        LOG_TRACE ("add_STYLE \"%s\"\n", text);
        ent = (lastent_t){ .u.style = dwg_add_STYLE (dwg, text),
                           .type = DWG_TYPE_STYLE };
      }
      else
          // clang-format off
        SET_ENT (style, STYLE)
      // clang-format on
      else if (1 == SSCANF_S (p, "ltype " FMT_TBL, text SZ))
      {
        LOG_TRACE ("add_LTYPE \"%s\"\n", text);
        ent = (lastent_t){ .u.ltype = dwg_add_LTYPE (dwg, text),
                           .type = DWG_TYPE_LTYPE };
      }
      else
          // clang-format off
        SET_ENT (ltype, LTYPE)
      // clang-format on
      else if (1 == SSCANF_S (p, "view " FMT_TBL, text SZ))
      {
        LOG_TRACE ("add_VIEW \"%s\"\n", text);
        ent = (lastent_t){ .u.view = dwg_add_VIEW (dwg, text),
                           .type = DWG_TYPE_VIEW };
      }
      else
          // clang-format off
        SET_ENT (view, VIEW)
      // clang-format on
      else if (1 == SSCANF_S (p, "vport " FMT_TBL, text SZ))
      {
        LOG_TRACE ("add_VPORT \"%s\"\n", text);
        ent = (lastent_t){ .u.vport = dwg_add_VPORT (dwg, text),
                           .type = DWG_TYPE_VPORT };
      }
      else
        // clang-format off
        SET_ENT (vport, VPORT)
        // clang-format on
      else if (1 == SSCANF_S (p, "dimstyle " FMT_TBL, text SZ))
      {
        if (version < R_11)
          fn_error ("Invalid table DIMSTYLE <r11\n");
        LOG_TRACE ("add_DIMSTYLE \"%s\"\n", text);
        ent = (lastent_t){ .u.dimstyle = dwg_add_DIMSTYLE (dwg, text),
                           .type = DWG_TYPE_DIMSTYLE };
      }
      else
        // clang-format off
        SET_ENT (dimstyle, DIMSTYLE)
        // clang-format on
      else if (1 == SSCANF_S (p, "group " FMT_TBL, text SZ))
      {
        if (version <= R_11)
          fn_error ("Invalid object GROUP < r13\n");
        LOG_TRACE ("add_GROUP \"%s\"\n", text);
        ent = (lastent_t){ .u.group = dwg_add_GROUP (dwg, text),
                           .type = DWG_TYPE_GROUP };
      }
      else
          // clang-format off
        SET_ENT (group, GROUP)
      // clang-format on
      else if (10 == SSCANF_S (
                   p, "ucs (%lf %lf %lf) (%lf %lf %lf) (%lf %lf %lf) " FMT_TBL,
                   &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &pt3.x,
                   &pt3.y, &pt3.z, text SZ))
      {
        if (version < R_10)
          fn_error ("Invalid table UCS <r10\n");
        LOG_TRACE ("add_UCS (%f %f %f) (%f %f %f) (%f %f %f) \"%s\"\n", pt1.x,
                   pt1.y, pt1.z, pt2.x, pt2.y, pt2.z, pt3.x, pt3.y, pt3.z,
                   text);
        ent = (lastent_t){ .u.ucs = dwg_add_UCS (dwg, &pt1, &pt2, &pt3, text),
                           .type = DWG_TYPE_UCS };
      }
      else
          // clang-format off
        SET_ENT (ucs, UCS)
      // clang-format on
      else if (2 == SSCANF_S (p, "layout viewport " FMT_TBL " " FMT_ANY,
                              text SZ, s1 SZ))
      {
        Dwg_Object *obj = dwg_ent_generic_to_object (ent.u.viewport, &error);
        if (version <= R_11)
          fn_error ("Invalid object LAYOUT <r13\n");
        if (ent.type != DWG_TYPE_VIEWPORT)
          fn_error ("layout viewport: last entity is not a viewport\n");
        if (strlen (s1) && text[strlen (s1) - 1] == '"')
          text[strlen (s1) - 1] = '\0'; // strip the \"
        if (!error)
          ent = (lastent_t){ .u.layout = dwg_add_LAYOUT (obj, text, s1),
                             .type = DWG_TYPE_LAYOUT };
      }
      else if (8 == SSCANF_S (p, "torus (%lf %lf %lf) (%lf %lf %lf) %lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &f1,
                         &f2))
      {
        if (version <= R_11)
          fn_error ("Invalid entity TORUS <r13\n");
        LOG_TRACE ("add_TORUS %s (%f %f %f) (%f %f %f) %f %f\n", hdr_s,
                   pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z, f1, f2);
        ent = (lastent_t){ .u._3dsolid
                           = dwg_add_TORUS (hdr, &pt1, &pt2, f1, f2),
                           .type = DWG_TYPE__3DSOLID };
      }
      else if (7 == SSCANF_S (p, "sphere (%lf %lf %lf) (%lf %lf %lf) %lf", &pt1.x,
                         &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &f1))
      {
        if (version <= R_11)
          fn_error ("Invalid entity SPHERE <r13\n");
        LOG_TRACE ("add_SPHERE %s (%f %f %f) (%f %f %f) %f\n", hdr_s,
                   pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z, f1);
        ent = (lastent_t){ .u._3dsolid
                             = dwg_add_SPHERE (hdr, &pt1, &pt2, f1),
                             .type = DWG_TYPE__3DSOLID };
      }
      else if (10 == SSCANF_S (
                   p, "cylinder (%lf %lf %lf) (%lf %lf %lf) %lf %lf %lf %lf",
                   &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &height,
                   &f1, &f2, &len))
      {
        if (version <= R_11)
          fn_error ("Invalid entity CYLINDER <r13\n");
        LOG_TRACE ("add_CYLINDER %s (%f %f %f) (%f %f %f) %f %f %f %f", hdr_s,
                   pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z, height, f1, f2,
                   len);
        ent = (lastent_t){ .u._3dsolid = dwg_add_CYLINDER (
                               hdr, &pt1, &pt2, height, f1, f2, len),
                           .type = DWG_TYPE__3DSOLID };
      }
      else if (10 == SSCANF_S (p, "cone (%lf %lf %lf) (%lf %lf %lf) %lf %lf %lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z,
                         &height, &f1, &f2, &len))
      {
        if (version <= R_11)
          fn_error ("Invalid entity CONE <r13\n");
        LOG_TRACE ("add_CONE %s (%f %f %f) (%f %f %f) %f %f %f %f", hdr_s,
                   pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z, height, f1, f2,
                   len);
        ent = (lastent_t){ .u._3dsolid = dwg_add_CONE (hdr, &pt1, &pt2,
                                                       height, f1, f2, len),
                             .type = DWG_TYPE__3DSOLID };
      }
      else if (9 == SSCANF_S (p, "wedge (%lf %lf %lf) (%lf %lf %lf) %lf %lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &len,
                         &f1, &height))
      {
        if (version <= R_11)
          fn_error ("Invalid entity WEDGE <r13\n");
        LOG_TRACE ("add_WEDGE %s (%f %f %f) (%f %f %f) %f %f %f", hdr_s,
                   pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z, len, f1,
                   height);
        ent = (lastent_t){ .u._3dsolid = dwg_add_WEDGE (hdr, &pt1, &pt2, len,
                                                        f1, height),
                           .type = DWG_TYPE__3DSOLID };
      }
      else if (9 == SSCANF_S (p, "box (%lf %lf %lf) (%lf %lf %lf) %lf %lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &len,
                         &f1, &height))
      {
        if (version <= R_11)
          fn_error ("Invalid entity BOX <r13\n");
        LOG_TRACE ("add_BOX %s (%f %f %f) (%f %f %f) %f %f %f", hdr_s,
                   pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z, len, f1,
                   height);
        ent = (lastent_t){ .u._3dsolid
                             = dwg_add_BOX (hdr, &pt1, &pt2, len, f1, height),
                           .type = DWG_TYPE__3DSOLID };
      }
      else if (10 == SSCANF_S (p,
                         "pyramid (%lf %lf %lf) (%lf %lf %lf) %lf %d %lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z,
                         &height, &i1, &f1, &f2))
      {
        if (version <= R_11)
          fn_error ("Invalid entity PYRAMID <r13\n");
        LOG_TRACE ("add_PYRAMID %s (%f %f %f) (%f %f %f) %f %d %f %f", hdr_s,
                   pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z, height, i1, f1, f2);
        ent = (lastent_t){ .u._3dsolid = dwg_add_PYRAMID (
                                 hdr, &pt1, &pt2, height, i1, f1, f2),
                             .type = DWG_TYPE__3DSOLID };
      }
      else
          // clang-format off
        SET_ENT (_3dsolid, _3DSOLID)
      // clang-format on
      else if (2 == SSCANF_S (p, "HEADER." FMT_NAME " = %d\n", s1 SZ, &i1))
        dwg_dynapi_header_set_value (dwg, s1, &i1, 0);
      else if (2 == SSCANF_S (p, "HEADER." FMT_NAME " = %lf\n", s1 SZ, &f1))
        dwg_dynapi_header_set_value (dwg, s1, &f1, 0);
      else if (1 == SSCANF_S (p, "HEADER." FMT_NAME " = " FMT_ANY "\n", s1 SZ,
                              text SZ))
      {
        if (strlen (text) && text[strlen (text) - 1] == '"')
          text[strlen (text) - 1] = '\0'; // strip the \"
        dwg_dynapi_header_set_value (dwg, s1, text, 1);
      }
      else
        {
          char *n = strchr (p, '\n');
          int size = n ? n - p : (int)strlen (p);
          LOG_WARN ("Ignored %.*s", size, p)
        }

      p = next_line (p, end);
    }
  // if we added at least one object
  return (dwg->num_objects - orig_num > 0 ? 0 : 1);
}
