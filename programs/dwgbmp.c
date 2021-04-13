/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009, 2018, 2019 Free Software Foundation, Inc.            */
/*  Copyright (C) 2010 Thien-Thi Nguyen                                      */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwgbmp.c: get the bmp thumbnail in a dwg file.
 * not the WMF.
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Thien-Thi Nguyen
 * modified by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_getopt.h"
#ifdef HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#endif

#include "common.h"
#include <dwg.h>
#include "suffix.inc"

static int opts = 0;
static int force_free = 0;

static int
usage (void)
{
  printf ("\nUsage: dwgbmp [-v[0-9]] DWGFILE [BMPFILE]\n");
  return 1;
}
static int
opt_version (void)
{
  printf ("dwgbmp %s\n", PACKAGE_VERSION);
  return 0;
}
static int
help (void)
{
  printf ("\nUsage: dwgbmp [OPTION]... DWGFILE [thumbnailfile]\n");
  printf ("Extract the DWG thumbnail image as BMP, WMF or PNG.\n");
  printf ("Default thumbnailfile: DWGFILE with the proper extension.\n"
          "\n");
#ifdef HAVE_GETOPT_LONG
  printf ("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf ("           --help           display this help and exit\n");
  printf ("           --version        output version information and exit\n"
          "\n");
#else
  printf ("  -v[0-9]     verbosity\n");
  printf ("  -h          display this help and exit\n");
  printf ("  -i          output version information and exit\n"
          "\n");
#endif
  printf ("GNU LibreDWG online manual: "
          "<https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

#ifdef HAVE_LIBGC
#  define bmp_free_dwg(x)
#else
static void
bmp_free_dwg (Dwg_Data *dwg)
{
#  if defined __SANITIZE_ADDRESS__ || __has_feature(address_sanitizer)
  {
    char *asanenv = getenv ("ASAN_OPTIONS");
    if (!asanenv)
      force_free = 1;
    // detect_leaks is enabled by default. see if it's turned off
    else if (strstr (asanenv, "detect_leaks=0") == NULL) /* not found */
      force_free = 1;
  }
#  endif
  // really huge DWG's need endlessly here.
  if ((dwg->header.version && dwg->num_objects < 1000) || force_free
#  ifdef HAVE_VALGRIND_VALGRIND_H
      || (RUNNING_ON_VALGRIND)
#  endif
  )
    dwg_free (dwg);
}
#endif

#pragma pack(1)

static int
get_bmp (char *dwgfile, char *bmpfile, bool must_free)
{
  unsigned char *data;
  int error;
  BITCODE_RL size;
  BITCODE_RC type = 0;
  size_t retval;
  FILE *fh;
  Dwg_Data dwg;
  struct _BITMAP_HEADER
  {
    char magic[2];
    BITCODE_RL file_size;
    BITCODE_RL reserved;
    BITCODE_RL offset;
  } bmp_h;
  const char *typenames[] = { "", "header", "bmp", "wmf", "", "", "png" };
  const char *typename;

  memset (&dwg, 0, sizeof (Dwg_Data));
  dwg.opts = opts;
  /* Read dwg data */
  error = dwg_read_file (dwgfile, &dwg);
  if (error >= DWG_ERR_CRITICAL)
    {
      fprintf (stderr, "Unable to read file %s. ERROR 0x%x\n", dwgfile, error);
      if (must_free)
        FREE (bmpfile);
      bmp_free_dwg (&dwg);
      return error;
    }

  /* Get DIB bitmap data */
  data = dwg_bmp (&dwg, &size, &type);
  if (type <= 6)
    typename = typenames[type];
  else
    typename = "";
  if (!data)
    {
      fprintf (stderr, "No thumbnail image in %s\n", dwgfile);
      if (must_free)
        FREE (bmpfile);
      bmp_free_dwg (&dwg);
      return 0;
    }
  if (!*typename)
    {
      fprintf (stderr, "Unknown thumbnail type %u in %s\n", type, dwgfile);
      bmp_free_dwg (&dwg);
      return 0;
    }
  if (size < 1)
    {
      fprintf (stderr, "Empty thumbnail data in %s\n", dwgfile);
      if (must_free)
        FREE (bmpfile);
      bmp_free_dwg (&dwg);
      return -3;
    }
  if (size > dwg.thumbnail.size)
    {
      fprintf (stderr,
               "Invalid thumbnail data in %s,"
               " size " FORMAT_RL " > %" PRIuSIZE "\n",
               dwgfile, size, dwg.thumbnail.size);
      if (must_free)
        FREE (bmpfile);
      bmp_free_dwg (&dwg);
      return -3;
    }
  if (type != 2 && *typename)
    {
      if (must_free)
        FREE (bmpfile);
      bmpfile = suffix (dwgfile, typename);
    }

  fh = fopen (bmpfile, "w");
  if (!fh)
    {
      fprintf (stderr, "Unable to write thumbnail file '%s'\n", bmpfile);
      if (must_free)
        FREE (bmpfile);
      bmp_free_dwg (&dwg);
      return -4;
    }

  if (type == 2)
    {
      /* Write bmp file header */
      bmp_h.magic[0] = 'B';
      bmp_h.magic[1] = 'M';
      bmp_h.file_size = 14 + size; // file header + DIB data
      bmp_h.reserved = 0;
      bmp_h.offset
          = 14 + 40 + 4 * 256; // file header + DIB header + color table
      retval = fwrite (&bmp_h.magic[0], sizeof (char), 2, fh);
      if (!retval)
        {
          bmp_free_dwg (&dwg);
          perror ("writing BMP magic");
          fclose (fh);
          return 1;
        }
      retval = fwrite (&bmp_h.file_size, 4, 3, fh);
      if (!retval)
        {
          if (must_free)
            FREE (bmpfile);
          bmp_free_dwg (&dwg);
          perror ("writing BMP file_size");
          fclose (fh);
          return 1;
        }
    }

  /* Write data: DIB header + bitmap, resp. the others */
  retval = fwrite (data, sizeof (char), size, fh);
  fclose (fh);
  if (!retval)
    {
      if (must_free)
        FREE (bmpfile);
      bmp_free_dwg (&dwg);
      perror ("writing thumbnail data");
      return 1;
    }

  printf ("Success. Written thumbnail image to '%s'\n", bmpfile);
  bmp_free_dwg (&dwg);
  if (must_free)
    FREE (bmpfile);
  return 0;
}

int
main (int argc, char *argv[])
{
  int i = 1, error;
  char *dwgfile, *bmpfile;
  int c;
  bool must_free;
#ifdef HAVE_GETOPT_LONG
  int option_index = 0;
  static struct option long_options[]
      = { { "verbose", 1, &opts, 1 }, // optional
          { "help", 0, 0, 0 },
          { "version", 0, 0, 0 },
          { "force-free", 0, 0, 0 },
          { NULL, 0, NULL, 0 } };
#endif

  GC_INIT ();
  if (argc < 2)
    return usage ();

  while
#ifdef HAVE_GETOPT_LONG
      ((c = getopt_long (argc, argv, ":v::h", long_options, &option_index))
       != -1)
#else
      ((c = getopt (argc, argv, ":v::hi")) != -1)
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
          if (!strcmp (long_options[option_index].name, "version"))
            return opt_version ();
          if (!strcmp (long_options[option_index].name, "help"))
            return help ();
          if (!strcmp (long_options[option_index].name, "force-free"))
            force_free = 1;
          break;
#else
        case 'i':
          return opt_version ();
#endif
        case 'v': // support -v3 and -v
          i = (optind > 0 && optind < argc) ? optind - 1 : 1;
          if (!memcmp (argv[i], "-v", 2))
            {
              opts = argv[i][2] ? argv[i][2] - '0' : 1;
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
  if (i >= argc)
    return usage ();

  dwgfile = argv[i];
  if (i == argc - 2)
    bmpfile = argv[i + 1];
  else
    bmpfile = suffix (dwgfile, "bmp");
  must_free = i != argc - 2;
  return get_bmp (dwgfile, bmpfile, must_free);
}
