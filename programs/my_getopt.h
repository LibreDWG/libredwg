#include "../src/config.h"

#ifndef HAVE_GETOPT_H

#  ifndef MY_GETOPT_H
#    define MY_GETOPT_H

#    ifdef GETOPT_C
#      define GETOPT_EXTERN
#    else
#      define GETOPT_EXTERN extern
#    endif

GETOPT_EXTERN
int opterr,                   /* if error message should be printed */
    optind,                   /* index into parent argv vector */
    optopt,                   /* character checked for validity */
    optreset;                 /* reset getopt */
GETOPT_EXTERN char *optarg;   /* argument associated with option */

int getopt (int nargc, char *const nargv[], const char *ostr);
#  endif

#else
#  include <getopt.h>
#endif
