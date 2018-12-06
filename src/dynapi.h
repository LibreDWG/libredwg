#include "config.h"
#include <dwg.h>

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#define OFF(st,f,def) offsetof(st, f)
#else
#define OFF(st,f,def) def
#endif
