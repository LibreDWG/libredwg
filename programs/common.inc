/* -*- c -*- */
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
 * common.c: common programs functions. included, not linked.
 * written by Reini Urban
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* returns number of consumed args */
int
verbosity (int argc, char **argv, int i, unsigned int *opts)
{
  unsigned int loglevel = 2;
  char log_str[4];
  if (!strcmp (argv[i], "--verbose") || // --verbose 2 or --verbose
      !strcmp (argv[i], "-v"))          // -v 0 or -v
    {
      int num_args;
      if (argc > i)
        {
          long l = strtol (argv[i + 1], NULL, 10);
          if (l < 0 || l > 10)
            {
              printf ("Invalid verbosity %s\n", argv[i + 1]); // -vbla
              exit (help ());
              return 1;
            }
          loglevel = (unsigned int)l;
          num_args = 2;
        }
      else
        {
          num_args = 1;
          loglevel = 2;
        }
      *opts = loglevel;
      sprintf (log_str, "%d", loglevel);
#if defined(USE_TRACING) && defined(HAVE_SETENV)
      setenv ("LIBREDWG_TRACE", log_str, 1);
#endif
      return num_args;
    }

  if (!strncmp (argv[i], "-v", 2)) // -v0
    {
      char *opt = argv[i];
      long l = strtol (&opt[2], NULL, 10);
      if (l < 0 || l > 10)
        {
          printf ("Invalid verbosity %s\n", opt); // -vbla
          exit (help ());
          return 0;
        }
      loglevel = (unsigned int)l;
      *opts = loglevel;
      sprintf (log_str, "%d", loglevel);
#if defined(USE_TRACING) && defined(HAVE_SETENV)
      setenv ("LIBREDWG_TRACE", log_str, 1);
#endif
      return 1;
    }

  printf ("Invalid verbosity %s\n", argv[i]); // -vbla
  exit (1);
  return 0;
}
