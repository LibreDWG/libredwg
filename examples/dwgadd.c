/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2020-2022 Free Software Foundation, Inc.                   */
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
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>

#include <dwg.h>
#include "common.h"
#include "decode.h"
#include "encode.h"
#include "bits.h"
#define DWG_LOGLEVEL loglevel
#include "logging.h"
#include <dwg_api.h>
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
#  define SZ ,119
#  define FMT_NAME "%[a-zA-Z0-9_]"
#  define FMT_TBL "\"%[a-zA-Z0-9._ -]\""
#  define FMT_PATH "\"%[a-zA-Z0-9_. \\-]\""
#  define FMT_ANY "\"%s\""
#else
#  define SSCANF_S sscanf
#  define SZ
#  define FMT_NAME "%119[a-zA-Z0-9_]"
#  define FMT_TBL "\"%119[a-zA-Z0-9._ -]\""
#  define FMT_PATH "\"%119[a-zA-Z0-9_. \\-]\""
#  define FMT_ANY "\"%119s\""
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
  exit (0);
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
  if (num_pts > 5000)
    exit (0);
  pts = calloc (num_pts, 16);
  if (!pts)
    exit (0);
  for (unsigned i = 0; i < num_pts; i++)
    {
      if (SSCANF_S (p, "(%lf %lf)", &pts[i].x, &pts[i].y))
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
  if (num_pts > 5000)
    exit (0);
  pts = calloc (num_pts, 24);
  if (!pts)
    exit (0);
  for (unsigned i = 0; i < num_pts; i++)
    {
      if (SSCANF_S (p, "(%lf %lf %lf)", &pts[i].x, &pts[i].y, &pts[i].z))
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
      *pp = p;
      return pts;
    }
  else
    {
      free (pts);
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
  lastent_t ent;
  char text[120];
  char prompt[120];
  char tag[120];
  char default_text[120];

  if (!dat->chain)
    abort ();
  dwg = *dwgp;
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
          if (SSCANF_S (p, "readdwg " FMT_PATH, &text[0] SZ))
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
          if (SSCANF_S (p, "readdxf " FMT_PATH, &text[0] SZ))
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
          if (SSCANF_S (p, "readjson " FMT_PATH, &text[0] SZ))
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
  if (SSCANF_S (p, #var "." FMT_NAME " = %d.%d.%X", &s1[0] SZ, &i1, &i2, &u)) \
    {                                                                         \
      BITCODE_H hdl;                                                          \
      if (!ent.u.var || ent.type != DWG_TYPE_##name)                          \
        fn_error ("Invalid type " #var ". Empty or wrong type\n");            \
      hdl = dwg_add_handleref (dwg, i1, u, NULL);                             \
      dwg_dynapi_entity_set_value (ent.u.var, #name, s1, hdl, 0);             \
    }                                                                         \
  else if (SSCANF_S (p, #var "." FMT_NAME " = %d", &s1[0] SZ, &i1))           \
    {                                                                         \
      if (!ent.u.var || ent.type != DWG_TYPE_##name)                          \
        fn_error ("Invalid type " #var ". Empty or wrong type\n");            \
      dwg_dynapi_entity_set_value (ent.u.var, #name, s1, &i1, 0);             \
    }                                                                         \
  else if (SSCANF_S (p, #var "." FMT_NAME " = %lf", &s1[0] SZ, &f1))          \
    {                                                                         \
      if (!ent.u.var || ent.type != DWG_TYPE_##name)                          \
        fn_error ("Invalid type " #var ". Empty or wrong type\n");            \
      dwg_dynapi_entity_set_value (ent.u.var, #name, s1, &f1, 0);             \
    }                                                                         \
  else if (SSCANF_S (p, #var "." FMT_NAME " = " FMT_ANY, &s1[0] SZ,           \
                     &text[0] SZ))                                            \
    {                                                                         \
      if (strlen (text) && text[strlen (text) - 1] == '"')                    \
        text[strlen (text) - 1] = '\0';                                       \
      if (!ent.u.var || ent.type != DWG_TYPE_##name)                          \
        fn_error ("Invalid type " #var ". Empty or wrong type\n");            \
      dwg_dynapi_entity_set_value (ent.u.var, #name, s1, text, 1);            \
    }

      if (SSCANF_S (p, "pspace"))
        {
          Dwg_Object *pspace = dwg_paper_space_object (dwg);
          hdr = pspace->tio.object->tio.BLOCK_HEADER;
        }
      else if (SSCANF_S (p, "mspace"))
        {
          hdr = mspace->tio.object->tio.BLOCK_HEADER;
        }
      else if (SSCANF_S (p, "attdef %lf %d " FMT_TBL " (%lf %lf %lf) " FMT_TBL " " FMT_TBL,
                         &height, &flags, &prompt[0] SZ, &pt1.x, &pt1.y,
                         &pt1.z, &tag[0] SZ, &default_text[0] SZ)) {
        if (version < R_2_0b)
          fn_error ("Invalid entity ATTDEF\n");
        else
          ent = (lastent_t){ .u.attdef = dwg_add_ATTDEF (hdr, height, flags,
                                                         prompt, &pt1, tag,
                                                         default_text),
                             .type = DWG_TYPE_ATTDEF };
      } else
        SET_ENT (attdef, ATTDEF)
      else if (SSCANF_S (p, "line (%lf %lf %lf) (%lf %lf %lf)", &pt1.x, &pt1.y,
                         &pt1.z, &pt2.x, &pt2.y, &pt2.z))
        ent = (lastent_t){ .u.line = dwg_add_LINE (hdr, &pt1, &pt2),
                           .type = DWG_TYPE_LINE };
      else
        SET_ENT (line, LINE)
      else if (SSCANF_S (p, "ray (%lf %lf %lf) (%lf %lf %lf)", &pt1.x, &pt1.y,
                         &pt1.z, &pt2.x, &pt2.y, &pt2.z))
      {
        if (version <= R_11)
          fn_error ("Invalid entity RAY\n");
        else
          ent = (lastent_t){ .u.ray = dwg_add_RAY (hdr, &pt1, &pt2),
                             .type = DWG_TYPE_RAY };
      }
      else SET_ENT (
          ray, RAY) else if (SSCANF_S (p, "xline (%lf %lf %lf) (%lf %lf %lf)",
                                       &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y,
                                       &pt2.z))
      {
        if (version <= R_11)
          fn_error ("Invalid entity XLINE\n");
        else
          ent = (lastent_t){ .u.xline = dwg_add_XLINE (hdr, &pt1, &pt2),
                             .type = DWG_TYPE_XLINE };
      }
      else SET_ENT (xline, XLINE) else if (SSCANF_S (p,
                                                     "text " FMT_ANY
                                                     " (%lf %lf %lf) %lf",
                                                     &text[0] SZ, &pt1.x,
                                                     &pt1.y, &pt1.z, &height))
      {
        if (strlen (text) && text[strlen (text) - 1] == '"')
          text[strlen (text) - 1] = '\0'; // strip the \"
        ent = (lastent_t){ .u.text = dwg_add_TEXT (hdr, text, &pt1, height),
                           .type = DWG_TYPE_TEXT };
      }
      else SET_ENT (
          text, TEXT) else if (SSCANF_S (p, "mtext (%lf %lf %lf) %lf " FMT_ANY,
                                         &pt1.x, &pt1.y, &pt1.z, &height,
                                         &text[0] SZ))
      {
        if (strlen (text) && text[strlen (text) - 1] == '"')
          text[strlen (text) - 1] = '\0'; // strip the \"
        if (version <= R_11)
          fn_error ("Invalid entity MTEXT\n");
        else
          ent = (lastent_t){ .u.mtext
                             = dwg_add_MTEXT (hdr, &pt1, height, text),
                             .type = DWG_TYPE_MTEXT };
      }
      else SET_ENT (
          mtext, MTEXT) else if (SSCANF_S (p, "block " FMT_TBL, &text[0] SZ))
      {
        if (strlen (text) && text[strlen (text) - 1] == '"')
          text[strlen (text) - 1] = '\0'; // strip the \"
        ent = (lastent_t){ .u.block = dwg_add_BLOCK (hdr, text),
                           .type = DWG_TYPE_BLOCK };
      }
      else SET_ENT (block, BLOCK) else if (memBEGINc (p, "endblk\n"))
          dwg_add_ENDBLK (hdr);
      else if (SSCANF_S (p, "insert (%lf %lf %lf) " FMT_TBL " %lf %lf %lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, &text[0] SZ, &scale.x,
                         &scale.y, &scale.z, &rot)) ent
          = (lastent_t){ .u.insert
                         = dwg_add_INSERT (hdr, &pt1, text, scale.x, scale.y,
                                           scale.z, deg2rad (rot)),
                         .type = DWG_TYPE_INSERT };
      else SET_ENT (insert,
                    INSERT) else if (SSCANF_S (p,
                                               "minsert (%lf %lf %lf) " FMT_TBL
                                               " %lf %lf %lf %lf %d %d "
                                               "%lf %lf",
                                               &pt1.x, &pt1.y, &pt1.z,
                                               &text[0] SZ, &scale.x, &scale.y,
                                               &scale.z, &rot, &i1, &i2, &f1,
                                               &f2))
      {
        if (version <= R_11)
          fn_error ("Invalid entity MINSERT\n");
        else
          ent = (lastent_t){ .u.minsert = dwg_add_MINSERT (
                                 hdr, &pt1, text, scale.x, scale.y, scale.z,
                                 deg2rad (rot), i1, i2, f1, f2),
                             .type = DWG_TYPE_MINSERT };
      }
      else SET_ENT (minsert,
                    MINSERT) else if (SSCANF_S (p, "point (%lf %lf %lf)",
                                                &pt1.x, &pt1.y, &pt1.z)) ent
          = (lastent_t){ .u.point = dwg_add_POINT (hdr, &pt1),
                         .type = DWG_TYPE_POINT };
      else SET_ENT (point,
                    POINT) else if (SSCANF_S (p, "circle (%lf %lf %lf) %lf",
                                              &pt1.x, &pt1.y, &pt1.z, &f1)) ent
          = (lastent_t){ .u.circle = dwg_add_CIRCLE (hdr, &pt1, f1),
                         .type = DWG_TYPE_CIRCLE };
      else SET_ENT (circle,
                    CIRCLE) else if (SSCANF_S (p,
                                               "arc (%lf %lf %lf) %lf %lf %lf",
                                               &pt1.x, &pt1.y, &pt1.z, &f1,
                                               &f2, &height)) ent
          = (lastent_t){ .u.arc = dwg_add_ARC (hdr, &pt1, f1, deg2rad (f2), deg2rad (height)),
                         .type = DWG_TYPE_ARC };
      else SET_ENT (arc, ARC) else if (
          SSCANF_S (p,
                    "dimension_aligned (%lf %lf %lf) (%lf %lf %lf) (%lf "
                    "%lf %lf)",
                    &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &pt3.x,
                    &pt3.y, &pt3.z)) ent
          = (lastent_t){ .u.dimali
                         = dwg_add_DIMENSION_ALIGNED (hdr, &pt1, &pt2, &pt3),
                         .type = DWG_TYPE_DIMENSION_ALIGNED };
      else SET_ENT (dimali, DIMENSION_ALIGNED) else if (
          SSCANF_S (p,
                    "dimension_linear (%lf %lf %lf) (%lf %lf %lf) (%lf %lf "
                    "%lf) %lf",
                    &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &pt3.x,
                    &pt3.y, &pt3.z, &rot)) ent
          = (lastent_t){ .u.dimlin = dwg_add_DIMENSION_LINEAR (
                             hdr, &pt1, &pt2, &pt3, deg2rad (rot)),
                         .type = DWG_TYPE_DIMENSION_LINEAR };
      else SET_ENT (dimlin, DIMENSION_LINEAR) else if (
          SSCANF_S (p,
                    "dimension_ang2ln (%lf %lf %lf) (%lf %lf %lf) (%lf %lf "
                    "%lf) (%lf %lf %lf)",
                    &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &pt3.x,
                    &pt3.y, &pt3.z, &pt4.x, &pt4.y, &pt4.z)) ent
          = (lastent_t){ .u.dimang2ln = dwg_add_DIMENSION_ANG2LN (
                             hdr, &pt1, &pt2, &pt3, &pt4),
                         .type = DWG_TYPE_DIMENSION_ANG2LN };
      else SET_ENT (dimang2ln, DIMENSION_ANG2LN) else if (
          SSCANF_S (p,
                    "dimension_ang3pt (%lf %lf %lf) (%lf %lf %lf) (%lf %lf "
                    "%lf) (%lf %lf %lf)",
                    &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &pt3.x,
                    &pt3.y, &pt3.z, &pt4.x, &pt4.y, &pt4.z)) ent
          = (lastent_t){ .u.dimang3pt = dwg_add_DIMENSION_ANG3PT (
                             hdr, &pt1, &pt2, &pt3, &pt4),
                         .type = DWG_TYPE_DIMENSION_ANG3PT };
      else SET_ENT (dimang3pt, DIMENSION_ANG3PT) else if (
          SSCANF_S (p, "dimension_diameter (%lf %lf %lf) (%lf %lf %lf) %lf",
                    &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &len)) ent
          = (lastent_t){ .u.dimdia
                         = dwg_add_DIMENSION_DIAMETER (hdr, &pt1, &pt2, len),
                         .type = DWG_TYPE_DIMENSION_DIAMETER };
      else SET_ENT (dimdia, DIMENSION_DIAMETER) else if (
          SSCANF_S (p, "dimension_ordinate (%lf %lf %lf) (%lf %lf %lf) %d",
                    &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &i1)) ent
          = (lastent_t){ .u.dimord = dwg_add_DIMENSION_ORDINATE (
                             hdr, &pt1, &pt2, i1 ? true : false),
                         .type = DWG_TYPE_DIMENSION_ORDINATE };
      else SET_ENT (dimord, DIMENSION_ORDINATE) else if (
          SSCANF_S (p, "dimension_radius (%lf %lf %lf) (%lf %lf %lf) %lf",
                    &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &len)) ent
          = (lastent_t){ .u.dimrad
                         = dwg_add_DIMENSION_RADIUS (hdr, &pt1, &pt2, len),
                         .type = DWG_TYPE_DIMENSION_RADIUS };
      else SET_ENT (dimrad, DIMENSION_RADIUS) else if (
          SSCANF_S (p,
                    "3dface (%lf %lf %lf) (%lf %lf %lf) (%lf %lf "
                    "%lf) (%lf %lf %lf)",
                    &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &pt3.x,
                    &pt3.y, &pt3.z, &pt4.x, &pt4.y, &pt4.z)) ent
          = (lastent_t){ .u._3dface
                         = dwg_add_3DFACE (hdr, &pt1, &pt2, &pt3, &pt4),
                         .type = DWG_TYPE__3DFACE };
      else if (SSCANF_S (p,
                         "3dface (%lf %lf %lf) (%lf %lf %lf) (%lf %lf "
                         "%lf)",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z,
                         &pt3.x, &pt3.y, &pt3.z)) ent
          = (lastent_t){ .u._3dface
                         = dwg_add_3DFACE (hdr, &pt1, &pt2, &pt3, NULL),
                         .type = DWG_TYPE__3DFACE };
      else SET_ENT (_3dface, _3DFACE) else if (
          SSCANF_S (p, "solid (%lf %lf %lf) (%lf %lf) (%lf %lf)  (%lf %lf)",
                    &pt1.x, &pt1.y, &pt1.z, &p2.x, &p2.y, &p3.x, &p3.y, &p4.x,
                    &p4.y)) ent
          = (lastent_t){ .u.solid = dwg_add_SOLID (hdr, &pt1, &p2, &p3, &p4),
                         .type = DWG_TYPE_SOLID };
      else SET_ENT (solid, SOLID) else if (
          SSCANF_S (p, "trace (%lf %lf %lf) (%lf %lf) (%lf %lf)  (%lf %lf)",
                    &pt1.x, &pt1.y, &pt1.z, &p2.x, &p2.y, &p3.x, &p3.y, &p4.x,
                    &p4.y)) ent
          = (lastent_t){ .u.trace = dwg_add_TRACE (hdr, &pt1, &p2, &p3, &p4),
                         .type = DWG_TYPE_TRACE };
      else SET_ENT (trace,
                    TRACE) else if (SSCANF_S (p, "polyline_2d %d ((%lf %lf)",
                                              &i1, &pt1.x, &pt1.y))
      {
        dwg_point_2d *pts = scan_pts2d (i1, &p);
        if (i1 && pts)
          {
            ent = (lastent_t){ .u.polyline_2d
                               = dwg_add_POLYLINE_2D (hdr, i1, pts),
                               .type = DWG_TYPE_POLYLINE_2D };
            free (pts);
          }
      }
      else SET_ENT (
          polyline_2d,
          POLYLINE_2D) else if (SSCANF_S (p, "polyline_3d %d ((%lf %lf %lf)",
                                          &i1, &pt1.x, &pt1.y, &pt1.z))
      {
        dwg_point_3d *pts = scan_pts3d (i1, &p);
        if (i1 && pts)
          {
            ent = (lastent_t){ .u.polyline_3d
                               = dwg_add_POLYLINE_3D (hdr, i1, pts),
                               .type = DWG_TYPE_POLYLINE_3D };
            free (pts);
          }
      }
      else SET_ENT (
          polyline_3d,
          POLYLINE_3D) else if (SSCANF_S (p,
                                          "polyline_mesh %d %d ((%lf %lf %lf)",
                                          &i1, &i2, &pt1.x, &pt1.y, &pt1.z))
      {
        dwg_point_3d *pts = scan_pts3d (i1 * i2, &p);
        if (i1 && i2 && pts)
          {
            ent = (lastent_t){ .u.polyline_mesh
                               = dwg_add_POLYLINE_MESH (hdr, i1, i2, pts),
                               .type = DWG_TYPE_POLYLINE_MESH };
            free (pts);
          }
      }
      else SET_ENT (polyline_mesh,
                    POLYLINE_MESH) else if (SSCANF_S (p,
                                                      "dictionary " FMT_TBL
                                                      " " FMT_TBL " %u",
                                                      &text[0] SZ, &s1[0] SZ,
                                                      &u)) ent
          = (lastent_t){ .u.dictionary = dwg_add_DICTIONARY (dwg, text, s1,
                                                             (unsigned long)u),
                         .type = DWG_TYPE_DICTIONARY };
      else SET_ENT (
          dictionary,
          DICTIONARY) else if (ent.type == DWG_TYPE_DICTIONARY
                               && SSCANF_S (p, "xrecord dictionary " FMT_TBL,
                                            &text[0] SZ)) ent
          = (lastent_t){ .u.xrecord = dwg_add_XRECORD (ent.u.dictionary, text),
                         .type = DWG_TYPE_SOLID };
      else SET_ENT (xrecord,
                    XRECORD) else if (SSCANF_S (p,
                                                "shape " FMT_PATH
                                                " (%lf %lf %lf) %lf %lf",
                                                &text[0] SZ, &pt1.x, &pt1.y,
                                                &pt1.z, &scale.x, &rot)) ent
          = (lastent_t){ .u.shape = dwg_add_SHAPE (hdr, text, &pt1, scale.x,
                                                   deg2rad (rot)),
                         .type = DWG_TYPE_SHAPE };
      else SET_ENT (shape, SHAPE) else if (SSCANF_S (p, "viewport " FMT_TBL,
                                                     &text[0] SZ)) ent
          = (lastent_t){ .u.viewport = dwg_add_VIEWPORT (hdr, text),
                         .type = DWG_TYPE_SOLID };
      else SET_ENT (
          viewport,
          VIEWPORT) else if (SSCANF_S (p, "ellipse (%lf %lf %lf) %lf %lf",
                                       &pt1.x, &pt1.y, &pt1.z, &f1, &f2))
      {
        if (version <= R_11)
          fn_error ("Invalid entity ELLIPSE\n");
        else
          ent = (lastent_t){ .u.ellipse = dwg_add_ELLIPSE (hdr, &pt1, f1, f2),
                             .type = DWG_TYPE_ELLIPSE };
      }
      else SET_ENT (ellipse,
                    ELLIPSE) else if (SSCANF_S (p, "spline %d ((%lf %lf %lf)",
                                                &i1, &pt1.x, &pt1.y, &pt1.z))
      {
        if (version <= R_11)
          fn_error ("Invalid entity SPLINE\n");
        else
          {
            dwg_point_3d *fitpts = scan_pts3d (i1, &p);
            if (i1 && fitpts
                && SSCANF_S (p, ") (%lf %lf %lf) (%lf %lf %lf)", &pt2.x,
                             &pt2.y, &pt2.z, &pt3.x, &pt3.y, &pt3.z))
              {
                ent = (lastent_t){ .u.spline = dwg_add_SPLINE (hdr, i1, fitpts,
                                                               &pt2, &pt3),
                                   .type = DWG_TYPE_SPLINE };
              }
            free (fitpts);
          }
      }
      else SET_ENT (
          spline, SPLINE) else if (ent.type == DWG_TYPE_MTEXT
                                   && SSCANF_S (p, "leader %d ((%lf %lf %lf)",
                                                &i1, &pt1.x, &pt1.y, &pt1.z))
      {
        if (version <= R_11)
          fn_error ("Invalid entity LEADER\n");
        else
          {
            dwg_point_3d *pts = scan_pts3d (i1, &p);
            if (i1 && pts && SSCANF_S (p, ") mtext %d", &i2))
              {
                ent = (lastent_t){ .u.leader = dwg_add_LEADER (
                                       hdr, i1, pts, ent.u.mtext, i2),
                                   .type = DWG_TYPE_LEADER };
              }
            free (pts);
          }
      }
      else SET_ENT (leader,
                    LEADER) else if (SSCANF_S (p,
                                               "tolerance " FMT_TBL
                                               " (%lf %lf %lf) (%lf %lf %lf)",
                                               &text[0] SZ, &pt1.x, &pt1.y,
                                               &pt1.z, &pt2.x, &pt2.y, &pt2.z))
      {
        if (version <= R_11)
          fn_error ("Invalid entity TOLERANCE\n");
        else
          ent = (lastent_t){ .u.tolerance
                             = dwg_add_TOLERANCE (hdr, text, &pt1, &pt2),
                             .type = DWG_TYPE_TOLERANCE };
      }
      else SET_ENT (
          tolerance,
          TOLERANCE) else if (SSCANF_S (p, "mlinestyle " FMT_TBL, &text[0] SZ))
      {
        if (version <= R_11)
          fn_error ("Invalid entity MLINESTYLE\n");
        else
          ent = (lastent_t){ .u.mlinestyle = dwg_add_MLINESTYLE (dwg, text),
                             .type = DWG_TYPE_MLINESTYLE };
      }
      else SET_ENT (
          mlinestyle,
          MLINESTYLE) else if (SSCANF_S (p, "layer " FMT_TBL, &text[0] SZ)) ent
          = (lastent_t){ .u.layer = dwg_add_LAYER (dwg, text),
                         .type = DWG_TYPE_LAYER };
      else SET_ENT (layer, LAYER) else if (SSCANF_S (p, "style " FMT_TBL,
                                                     &text[0] SZ)) ent
          = (lastent_t){ .u.style = dwg_add_STYLE (dwg, text),
                         .type = DWG_TYPE_STYLE };
      else SET_ENT (style, STYLE) else if (SSCANF_S (p, "ltype " FMT_TBL,
                                                     &text[0] SZ)) ent
          = (lastent_t){ .u.ltype = dwg_add_LTYPE (dwg, text),
                         .type = DWG_TYPE_LTYPE };
      else SET_ENT (ltype, LTYPE) else if (SSCANF_S (p, "view " FMT_TBL,
                                                     &text[0] SZ)) ent
          = (lastent_t){ .u.view = dwg_add_VIEW (dwg, text),
                         .type = DWG_TYPE_VIEW };
      else SET_ENT (
          view, VIEW) else if (SSCANF_S (p, "vport " FMT_TBL, &text[0] SZ)) ent
          = (lastent_t){ .u.vport = dwg_add_VPORT (dwg, text),
                         .type = DWG_TYPE_VPORT };
      else SET_ENT (vport, VPORT) else if (SSCANF_S (p, "dimstyle " FMT_TBL,
                                                     &text[0] SZ)) ent
          = (lastent_t){ .u.dimstyle = dwg_add_DIMSTYLE (dwg, text),
                         .type = DWG_TYPE_DIMSTYLE };
      else SET_ENT (dimstyle, DIMSTYLE) else if (SSCANF_S (p, "group " FMT_TBL,
                                                           &text[0] SZ))
      {
        if (version <= R_11)
          fn_error ("Invalid entity GROUP\n");
        else
          ent = (lastent_t){ .u.group = dwg_add_GROUP (dwg, text),
                             .type = DWG_TYPE_GROUP };
      }
      else SET_ENT (group, GROUP) else if (
          SSCANF_S (p,
                    "ucs (%lf %lf %lf) (%lf %lf %lf) (%lf %lf %lf) " FMT_TBL,
                    &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &pt3.x,
                    &pt3.y, &pt3.z, &text[0] SZ)) ent
          = (lastent_t){ .u.ucs = dwg_add_UCS (dwg, &pt1, &pt2, &pt3, text),
                         .type = DWG_TYPE_UCS };
      else SET_ENT (ucs, UCS) else if (ent.type == DWG_TYPE_VIEWPORT
                                       && SSCANF_S (p,
                                                    "layout viewport " FMT_TBL
                                                    " " FMT_ANY,
                                                    &text[0] SZ, &s1[0] SZ))
      {
        Dwg_Object *obj = dwg_ent_generic_to_object (ent.u.viewport, &error);
        if (strlen (s1) && text[strlen (s1) - 1] == '"')
          text[strlen (s1) - 1] = '\0'; // strip the \"
        if (!error)
          ent = (lastent_t){ .u.layout = dwg_add_LAYOUT (obj, text, s1),
                             .type = DWG_TYPE_LAYOUT };
      }
      else if (SSCANF_S (p, "torus (%lf %lf %lf) (%lf %lf %lf) %lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &f1,
                         &f2))
      {
        if (version <= R_11)
          fn_error ("Invalid entity TORUS\n");
        else
          ent = (lastent_t){ .u._3dsolid
                             = dwg_add_TORUS (hdr, &pt1, &pt2, f1, f2),
                             .type = DWG_TYPE__3DSOLID };
      }
      else if (SSCANF_S (p, "sphere (%lf %lf %lf) (%lf %lf %lf) %lf", &pt1.x,
                         &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &f1))
      {
        if (version <= R_11)
          fn_error ("Invalid entity SPHERE\n");
        else
          ent = (lastent_t){ .u._3dsolid
                             = dwg_add_SPHERE (hdr, &pt1, &pt2, f1),
                             .type = DWG_TYPE__3DSOLID };
      }
      else if (SSCANF_S (
                   p, "cylinder (%lf %lf %lf) (%lf %lf %lf) %lf %lf %lf %lf",
                   &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &height,
                   &f1, &f2, &len))
      {
        if (version <= R_11)
          fn_error ("Invalid entity CYLINDER\n");
        else
          ent = (lastent_t){ .u._3dsolid = dwg_add_CYLINDER (
                                 hdr, &pt1, &pt2, height, f1, f2, len),
                             .type = DWG_TYPE__3DSOLID };
      }
      else if (SSCANF_S (p, "cone (%lf %lf %lf) (%lf %lf %lf) %lf %lf %lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z,
                         &height, &f1, &f2, &len))
      {
        if (version <= R_11)
          fn_error ("Invalid entity CONE\n");
        else
          ent = (lastent_t){ .u._3dsolid = dwg_add_CONE (hdr, &pt1, &pt2,
                                                         height, f1, f2, len),
                             .type = DWG_TYPE__3DSOLID };
      }
      else if (SSCANF_S (p, "wedge (%lf %lf %lf) (%lf %lf %lf) %lf %lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &len,
                         &f1, &height))
      {
        if (version <= R_11)
          fn_error ("Invalid entity wedge\n");
        else
          ent = (lastent_t){ .u._3dsolid = dwg_add_WEDGE (hdr, &pt1, &pt2, len,
                                                          f1, height),
                             .type = DWG_TYPE__3DSOLID };
      }
      else if (SSCANF_S (p, "box (%lf %lf %lf) (%lf %lf %lf) %lf %lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &len,
                         &f1, &height))
      {
        if (version <= R_11)
          fn_error ("Invalid entity BOX\n");
        else
          ent = (lastent_t){ .u._3dsolid
                             = dwg_add_BOX (hdr, &pt1, &pt2, len, f1, height),
                             .type = DWG_TYPE__3DSOLID };
      }
      else if (SSCANF_S (p,
                         "pyramid (%lf %lf %lf) (%lf %lf %lf) %lf %d %lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z,
                         &height, &i1, &f1, &f2))
      {
        if (version <= R_11)
          fn_error ("Invalid entity PYRAMID\n");
        else
          ent = (lastent_t){ .u._3dsolid = dwg_add_PYRAMID (
                                 hdr, &pt1, &pt2, height, i1, f1, f2),
                             .type = DWG_TYPE__3DSOLID };
      }
      else SET_ENT (_3dsolid,
                    _3DSOLID) else if (SSCANF_S (p, "HEADER." FMT_NAME " = %d",
                                                 &s1[0] SZ, &i1))
          dwg_dynapi_header_set_value (dwg, s1, &i1, 0);
      else if (SSCANF_S (p, "HEADER." FMT_NAME " = %lf", &s1[0] SZ, &f1))
          dwg_dynapi_header_set_value (dwg, s1, &f1, 0);
      else if (SSCANF_S (p, "HEADER." FMT_NAME " = " FMT_ANY, &s1[0] SZ,
                         &text[0] SZ))
      {
        if (strlen (text) && text[strlen (text) - 1] == '"')
          text[strlen (text) - 1] = '\0'; // strip the \"
        dwg_dynapi_header_set_value (dwg, s1, text, 1);
      }

      p = next_line (p, end);
    }
  // if we added at least one object
  return (dwg->num_objects - orig_num > 0 ? 0 : 1);
}
