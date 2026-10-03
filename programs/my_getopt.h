#include "../src/config.h"

#ifndef HAVE_GETOPT_H

#  ifndef MY_GETOPT_H
#    define MY_GETOPT_H

/* Defined once, in getopt.c, with the initial values the original BSD getopt
   gives them at file scope. A single GETOPT_EXTERN macro cannot carry the
   initialisers: written that way they would define the variables in every
   translation unit including this header. Leaving them uninitialised here is
   what led to opterr and optind being assigned inside getopt() instead, which
   reset the scan position to argv[1] on every call and made any option make
   the caller's getopt loop run forever. */
#    ifdef GETOPT_C
int opterr = 1, /* if error message should be printed */
    optind = 1, /* index into parent argv vector */
    optopt,     /* character checked for validity */
    optreset;   /* reset getopt */
char *optarg;   /* argument associated with option */
#    else
extern int opterr, optind, optopt, optreset;
extern char *optarg;
#    endif

int getopt (int nargc, char *const nargv[], const char *ostr);
#  endif

#else
#  include <getopt.h>
#endif
