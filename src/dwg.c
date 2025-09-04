/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2010,2018-2025 Free Software Foundation, Inc.         */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwg.c: main functions and API
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Anderson Pierre Cardoso
 * modified by Reini Urban
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include "../programs/my_stat.h"
// strings.h or string.h
#ifdef AX_STRCASECMP_HEADER
#  include AX_STRCASECMP_HEADER
#endif
#ifdef HAVE_LIBGEN_H
#  include <libgen.h> // basename
#else
char *basename (char *);
#endif

#include "bits.h"
#include "common.h"
#include "decode.h"
#include "dwg.h"
#include "hash.h"
#include "dynapi.h"
#include "classes.h"
#ifdef USE_WRITE
#  include "encode.h"
#endif
#include "free.h"

/* The logging level per .o */
static unsigned int loglevel;
#ifdef USE_TRACING
/* This flag means we have checked the environment variable
   LIBREDWG_TRACE and set `loglevel' appropriately.  */
static bool env_var_checked_p;
#endif /* USE_TRACING */
#define DWG_LOGLEVEL loglevel
#include "logging.h"

/*------------------------------------------------------------------------------
 * Imports bypassing its headers.
 * We dont want to import in_dxf.h as it is redefining FORMAT_BD.
 */
#ifndef DISABLE_DXF
EXPORT int dwg_read_dxf (Bit_Chain *restrict dat, Dwg_Data *restrict dwg);
EXPORT int dwg_read_dxfb (Bit_Chain *restrict dat, Dwg_Data *restrict dwg);
#endif

/*------------------------------------------------------------------------------
 * Internal functions
 */
// used in in_dxf.c, encode.c
BITCODE_H
dwg_find_tablehandle_silent (Dwg_Data *restrict dwg, const char *restrict name,
                             const char *restrict table);
BITCODE_RLL dwg_new_handseed (Dwg_Data *restrict dwg);
// used in encode.c
void dwg_set_handle_size (Dwg_Handle *restrict hdl);
void dwg_downgrade_MLINESTYLE (Dwg_Object_MLINESTYLE *o);
void dwg_upgrade_MLINESTYLE (Dwg_Data *restrict dwg,
                             Dwg_Object_MLINESTYLE *restrict o);

void ordered_ref_add (Dwg_Data *dwg, Dwg_Object_Ref *ref);
const Dwg_Object_Ref *ordered_ref_find (Dwg_Data *dwg, const BITCODE_RC code,
                                        const unsigned long absref);
static void dwg_init_handseed (Dwg_Data *dwg);

/*------------------------------------------------------------------------------
 * Public functions
 */

EXPORT int
dat_read_file (Bit_Chain *restrict dat, FILE *restrict fp,
               const char *restrict filename)
{
  size_t size;
  if (!dat->size && fp)
    {
      struct_stat_t attrib;
      int fd = fileno (fp);
      if (fd >= 0 && !fstat (fd, &attrib))
        dat->size = attrib.st_size;
    }
  dat->chain = (unsigned char *)calloc (1, dat->size + 1);
  if (!dat->chain)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("Not enough memory.\n")
      fclose (fp);
      dat->fh = NULL;
      return DWG_ERR_OUTOFMEM;
    }

  size = fread (dat->chain, sizeof (char), dat->size, fp);
  if (size != dat->size)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("Could not read file (%" PRIuSIZE " out of %" PRIuSIZE
                 "): %s\n",
                 size, dat->size, filename)
      fclose (fp);
      free (dat->chain);
      dat->chain = NULL;
      dat->fh = NULL;
      dat->size = 0;
      return DWG_ERR_IOERROR;
    }
  // ensure zero-termination for strstr, strtol, ...
  dat->chain[dat->size] = '\0';
  return 0;
}

// fast bulk-read when we known the size
EXPORT int
dat_read_size (Bit_Chain *restrict dat)
{
  if (!dat->chain)
    dat->chain = (unsigned char *)calloc (1, dat->size + 2);
  else
    dat->chain = (unsigned char *)realloc (dat->chain, dat->size + 2);
  if (!dat->chain)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("Not enough memory");
      fclose (dat->fh);
      return DWG_ERR_OUTOFMEM;
    }
  if (fread (dat->chain, 1, dat->size, dat->fh) != dat->size)
    {
      fclose (dat->fh);
      free (dat->chain);
      dat->chain = NULL;
      return DWG_ERR_IOERROR;
    }
  dat->chain[dat->size] = '\0'; // ensure zero-termination
  return 0;
}

EXPORT int
dat_read_stream (Bit_Chain *restrict dat, FILE *restrict fp)
{
  size_t size = 0;
  loglevel = dat->opts & DWG_OPTS_LOGLEVEL;

  do
    {
      if (dat->chain)
        dat->chain = (unsigned char *)realloc (dat->chain, dat->size + 4096);
      else
        {
          dat->chain = (unsigned char *)calloc (1, 4096);
          dat->size = 0;
        }
      if (!dat->chain)
        {
          LOG_ERROR ("Not enough memory.\n");
          fclose (fp);
          dat->fh = NULL;
          return DWG_ERR_OUTOFMEM;
        }
      size = fread (&dat->chain[dat->size], sizeof (char), 4096, fp);
      dat->size += size;
    }
  while (size == 4096);

  if (dat->size == 0)
    {
      LOG_ERROR ("Could not read from stream (%" PRIuSIZE " out of %" PRIuSIZE
                 ")\n",
                 size, dat->size);
      fclose (fp);
      free (dat->chain);
      dat->fh = NULL;
      dat->chain = NULL;
      return DWG_ERR_IOERROR;
    }

  // clear the slack and realloc
  size = dat->size & 0xfff;
  if (size)
    memset (&dat->chain[dat->size], 0, 0xfff - size);
  dat->chain = (unsigned char *)realloc (dat->chain, dat->size + 1);
  // ensure NULL termination, for sscanf, strtol and friends.
  dat->chain[dat->size] = '\0';
  return 0;
}

/** dwg_read_file
 * returns 0 on success.
 *
 * everything in dwg is cleared
 * and then either read from dat, or set to a default.
 */
EXPORT int
dwg_read_file (const char *restrict filename, Dwg_Data *restrict dwg)
{
  FILE *fp;
  struct_stat_t attrib;
  Bit_Chain bit_chain = { 0 };
  int error;

  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  memset (dwg, 0, sizeof (Dwg_Data));
  dwg->opts = loglevel;

  if (strEQc (filename, "-"))
    {
      fp = stdin;
    }
  else
    {
      if (stat (filename, &attrib))
        {
          LOG_ERROR ("File not found: %s\n", filename);
          return DWG_ERR_IOERROR;
        }
      if (!(S_ISREG (attrib.st_mode)
#ifndef _WIN32
            || S_ISLNK (attrib.st_mode)
#endif
                ))
        {
          LOG_ERROR ("Illegal input file %s\n", filename);
          return DWG_ERR_IOERROR;
        }
      fp = fopen (filename, "rb");
    }
  if (!fp)
    {
      LOG_ERROR ("Could not open file: %s\n", filename)
      return DWG_ERR_IOERROR;
    }

  /* Load whole file into memory, even if streamed (for now)
   */
  memset (&bit_chain, 0, sizeof (Bit_Chain));
  if (fp == stdin)
    {
      error = dat_read_stream (&bit_chain, fp);
      if (error >= DWG_ERR_CRITICAL)
        return error;
    }
  else
    {
#ifdef HAVE_SYS_STAT_H
      bit_chain.size = attrib.st_size;
#endif
      error = dat_read_file (&bit_chain, fp, filename);
      if (error >= DWG_ERR_CRITICAL)
        return error;
    }
  fclose (fp);

  /* Decode the dwg structure */
  error = dwg_decode (&bit_chain, dwg);
  if (error >= DWG_ERR_CRITICAL)
    {
      LOG_ERROR ("Failed to decode file: %s 0x%x\n", filename, error)
      free (bit_chain.chain);
      bit_chain.chain = NULL;
      bit_chain.size = 0;
      return error;
    }

  // TODO: does dwg hold any char* pointers to the bit_chain or are they all
  // copied?
  free (bit_chain.chain);
  bit_chain.chain = NULL;
  bit_chain.size = 0;

  return error;
}

#if !defined(DISABLE_DXF) && defined(USE_WRITE)
/** dxf_read_file
 * returns 0 on success.
 *
 * detects binary or ascii file.
 * everything in dwg is cleared
 * and then either read from dat, or set to a default.
 */
EXPORT int
dxf_read_file (const char *restrict filename, Dwg_Data *restrict dwg)
{
  int error;
  FILE *fp;
  struct_stat_t attrib;
  size_t size;
  Bit_Chain dat = { 0 };
  Dwg_Version_Type version;

  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;

  if (!filename || stat (filename, &attrib))
    {
      LOG_ERROR ("File not found: %s\n", filename ? filename : "(null)")
      return DWG_ERR_IOERROR;
    }
  if (!(S_ISREG (attrib.st_mode)
#  ifndef _WIN32
        || S_ISLNK (attrib.st_mode)
#  endif
            ))
    {
      LOG_ERROR ("Error: %s\n", filename)
      return DWG_ERR_IOERROR;
    }
  fp = fopen (filename, "rb");
  if (!fp)
    {
      LOG_ERROR ("Could not open file: %s\n", filename)
      return DWG_ERR_IOERROR;
    }

  /* Load whole file into memory
   */
  version = dwg->header.version;
  memset (dwg, 0, sizeof (Dwg_Data));
  dwg->opts = loglevel | DWG_OPTS_INDXF;
  dwg->header.version = version;

  memset (&dat, 0, sizeof (Bit_Chain));
#  ifdef HAVE_SYS_STAT_H
  dat.size = attrib.st_size;
#  endif
  dat.chain = (unsigned char *)calloc (1, dat.size + 2);
  if (!dat.chain)
    {
      LOG_ERROR ("Not enough memory.\n");
      fclose (fp);
      return DWG_ERR_OUTOFMEM;
    }
  dat.byte = 0;
  dat.bit = 0;
  dat.from_version = dwg->header.from_version;
  dat.version = dwg->header.version;
  dat.opts = dwg->opts;

  size = fread (dat.chain, sizeof (char), dat.size, fp);
  fclose (fp);
  if (size != dat.size)
    {
      LOG_ERROR ("Could not read the entire file (%" PRIuSIZE
                 " out of %" PRIuSIZE "): %s\n",
                 size, dat.size, filename)
      free (dat.chain);
      dat.chain = NULL;
      dat.size = 0;
      return DWG_ERR_IOERROR;
    }
  /*
0
SECTION
2
ENTITIES
0
ENDSEC
   */
  if (size < 31)
    {
      LOG_ERROR ("File %s too small, %" PRIuSIZE " byte.\n", filename, size)
      free (dat.chain);
      dat.chain = NULL;
      dat.size = 0;
      return DWG_ERR_IOERROR;
    }
  // properly end the buffer for strtol()/... readers
  if (dat.chain[size - 1] != '\n')
    {
      dat.chain[size] = '\n';
      dat.size++;
    }
  dat.chain[size] = '\0';

  /* Fail on DWG */
  if (!memcmp (dat.chain, "AC10", 4) || !memcmp (dat.chain, "AC1.", 4)
      || !memcmp (dat.chain, "AC2.10", 4) || !memcmp (dat.chain, "MC0.0", 4))
    {
      LOG_ERROR ("This is a DWG, not a DXF file: %s\n", filename)
      free (dat.chain);
      dat.chain = NULL;
      dat.size = 0;
      return DWG_ERR_INVALIDDWG;
    }
  /* See if binary or ascii */
  if (!memcmp (dat.chain, "AutoCAD Binary DXF",
               sizeof ("AutoCAD Binary DXF") - 1))
    {
      dat.byte = 22;
      error = dwg_read_dxfb (&dat, dwg);
    }
  else
    error = dwg_read_dxf (&dat, dwg);

  dwg->opts |= (DWG_OPTS_INDXF | loglevel);
  if (error >= DWG_ERR_CRITICAL)
    {
      LOG_ERROR ("Failed to decode DXF file: %s\n", filename)
      free (dat.chain);
      dat.chain = NULL;
      dat.size = 0;
      return error;
    }

  // TODO: does dwg hold any char* pointers to the dat or are they all copied?
  free (dat.chain);
  dat.chain = NULL;
  dat.size = 0;

  return 0;
}
#endif /* DISABLE_DXF */

#ifdef USE_WRITE
/** Encode the DWG struct into dat (in memory).
 * Needs 2x DWG heap, dwg + dat.
 */
EXPORT int
dwg_write_file (const char *restrict filename, const Dwg_Data *restrict dwg)
{
  FILE *fh;
  struct_stat_t attrib;
  Bit_Chain dat = { 0 };
  int error;

  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  assert (filename);
  assert (dwg);
  dat.opts = dwg->opts;
  dat.version = (Dwg_Version_Type)dwg->header.version;
  dat.from_version = (Dwg_Version_Type)dwg->header.from_version;
  dat.codepage = dwg->header.codepage;

  // json HACK. no wide chars from JSON, because we just encode to R_2000
  if (dwg->opts & (DWG_OPTS_INJSON | DWG_OPTS_INDXF))
    dat.from_version = dat.version;

  if (dwg->header.version <= R_2000 && dwg->header.from_version > R_2000)
    dwg_fixup_BLOCKS_entities ((Dwg_Data *)dwg);

  dat.size = 0;
  error = dwg_encode ((Dwg_Data *)dwg, &dat);
  if (error >= DWG_ERR_CRITICAL)
    {
      LOG_ERROR ("Failed to encode Dwg_Data\n");
      /* In development we want to look at the corpses */
#  ifdef IS_RELEASE
      if (dat.size > 0)
        {
          free (dat.chain);
          dat.chain = NULL;
          dat.size = 0;
        }
      return error;
#  endif
    }

  // try opening the output file in write mode
  if (!stat (filename, &attrib)
#  ifdef _WIN32
      && strNE (filename, "NUL") && strNE (filename, "CON")
#  else
      && strNE (filename, "/dev/null") && strNE (filename, "/dev/stdout")
#  endif
  )
    {
      LOG_ERROR ("The file already exists. We won't overwrite it.")
      return error | DWG_ERR_IOERROR;
    }
  fh = fopen (filename, "wb");
  if (!fh || !dat.chain)
    {
      LOG_ERROR ("Failed to create the file: %s\n", filename)
      return error | DWG_ERR_IOERROR;
    }

  // Write the data into the file
  if (fwrite (dat.chain, sizeof (char), dat.size, fh) != dat.size)
    {
      LOG_ERROR ("Failed to write data into the file: %s\n", filename)
      fclose (fh);
      free (dat.chain);
      dat.chain = NULL;
      dat.size = 0;
      return error | DWG_ERR_IOERROR;
    }
  fclose (fh);

  if (dat.size > 0)
    {
      free (dat.chain);
      dat.chain = NULL;
      dat.size = 0;
    }

  return error;
}
#endif /* USE_WRITE */

/* THUMBNAIL IMAGE DATA (R13C3+).
   Supports multiple preview pictures.
   Currently 3 types: BMP, WMF and PNG. returns the size of the image.
 */
EXPORT unsigned char *
dwg_bmp (const Dwg_Data *restrict dwg, BITCODE_RL *restrict size,
         BITCODE_RC *restrict typep)
{
  BITCODE_RC i, num_headers, type = 0;
  int found;
  BITCODE_RL header_size, address = 0, osize;
  Bit_Chain dat = { 0 };

  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  *size = 0;
  assert (dwg);
  // copy the chain data. bit_* needs a full chain with opts and version
  dat = *(Bit_Chain *)&dwg->thumbnail;
  if (!dat.size || !dat.chain)
    {
      LOG_INFO ("no THUMBNAIL Image Data\n")
      return NULL;
    }
  // dat.byte = 0; sentinel at 16
  dat.bit = 0;
  dat.opts = dwg->opts;
  dat.from_version = dwg->header.from_version;
  dat.version = dwg->header.version;
  dat.fh = NULL;

#ifdef USE_TRACING
  /* Before starting, set the logging level, but only do so once.  */
  if (!env_var_checked_p)
    {
      char *probe = getenv ("LIBREDWG_TRACE");
      if (probe)
        loglevel = atoi (probe);
      env_var_checked_p = true;
    }
#endif /* USE_TRACING */

  LOG_TRACE ("Thumbnail: " FORMAT_RL "\n", dwg->header.thumbnail_address);
  osize = bit_read_RL (&dat); /* overall size of all images */
  LOG_TRACE ("overall size: " FORMAT_RL " [RL]\n", osize);
  if (osize > (dat.size - 4))
    {
      LOG_ERROR ("Preview overflow > %" PRIuSIZE, dat.size - 4);
      return NULL;
    }
  num_headers = bit_read_RC (&dat);
  LOG_INFO ("num_headers: %d [RC]\n", (int)num_headers)

  found = 0;
  header_size = 0;
  for (i = 0; i < num_headers; i++)
    {
      if (dat.byte > dat.size)
        {
          LOG_ERROR ("Preview overflow");
          break;
        }
      type = bit_read_RC (&dat);
      *typep = type;
      LOG_TRACE ("\t[%i] Code: %i [RC]\n", i, type)
      address = bit_read_RL (&dat);
      if (type == 1)
        {
          BITCODE_RL h_size = bit_read_RL (&dat);
          LOG_TRACE ("\t\tHeader data start: " FORMAT_RL " [RL]\n", address)
          header_size += h_size;
          LOG_TRACE ("\t\tHeader data size: " FORMAT_RL " [RL] (" FORMAT_RL
                     ")\n",
                     h_size, header_size)
        }
      else if (type == 2 && found == 0)
        {
          *size = bit_read_RL (&dat);
          found = 1;
          LOG_TRACE ("\t\tBMP data start: " FORMAT_RL " [RL]\n", address)
          LOG_INFO ("\t\tBMP size: %i [RL]\n", *size)
          if (*size > (dat.size - 4))
            {
              LOG_ERROR ("BMP thumbnail overflow > %" PRIuSIZE, dat.size - 4);
              return NULL;
            }
        }
      else if (type == 3)
        {
          osize = bit_read_RL (&dat);
          *size = osize;
          LOG_TRACE ("\t\tWMF data start: " FORMAT_RL " [RL]\n", address)
          LOG_INFO ("\t\tWMF size: %i [RL]\n", osize)
        }
      else if (type == 6) // PNG default since r2013
        {
          osize = bit_read_RL (&dat);
          *size = osize;
          LOG_TRACE ("\t\tPNG data start: " FORMAT_RL " [RL]\n", address)
          LOG_INFO ("\t\tPNG size: %i [RL]\n", osize)
        }
      else
        {
          osize = bit_read_RL (&dat);
          LOG_TRACE ("\t\tData start: " FORMAT_RL " [RL]\n", address)
          LOG_TRACE ("\t\tSize of unknown type %i: %i [RL]\n", type, osize)
        }
    }
  dat.byte += header_size;
  if (*size)
    LOG_TRACE ("Image offset: %" PRIuSIZE "\n", dat.byte);
  if (header_size + *size > dat.size)
    {
      LOG_ERROR ("Preview overflow " FORMAT_RL " + " FORMAT_RL " > %" PRIuSIZE,
                 header_size, *size, dat.size);
      *size = 0;
      return NULL;
    }

  if (*size > 0)
    return (dat.chain + dat.byte);
  else
    return NULL;
}

EXPORT double
dwg_model_x_min (const Dwg_Data *dwg)
{
  assert (dwg);
  return dwg->header_vars.EXTMIN.x;
}

EXPORT double
dwg_model_x_max (const Dwg_Data *dwg)
{
  assert (dwg);
  return dwg->header_vars.EXTMAX.x;
}

EXPORT double
dwg_model_y_min (const Dwg_Data *dwg)
{
  assert (dwg);
  return dwg->header_vars.EXTMIN.y;
}

EXPORT double
dwg_model_y_max (const Dwg_Data *dwg)
{
  assert (dwg);
  return dwg->header_vars.EXTMAX.y;
}

EXPORT double
dwg_model_z_min (const Dwg_Data *dwg)
{
  assert (dwg);
  return dwg->header_vars.EXTMIN.z;
}

EXPORT double
dwg_model_z_max (const Dwg_Data *dwg)
{
  assert (dwg);
  return dwg->header_vars.EXTMAX.z;
}

EXPORT double
dwg_page_x_min (const Dwg_Data *dwg)
{
  assert (dwg);
  return dwg->header_vars.EXTMIN.x;
}

EXPORT double
dwg_page_x_max (const Dwg_Data *dwg)
{
  assert (dwg);
  return dwg->header_vars.PEXTMAX.x;
}

EXPORT double
dwg_page_y_min (const Dwg_Data *dwg)
{
  assert (dwg);
  return dwg->header_vars.PEXTMIN.y;
}

EXPORT double
dwg_page_y_max (const Dwg_Data *dwg)
{
  assert (dwg);
  return dwg->header_vars.PEXTMAX.y;
}

EXPORT unsigned int
dwg_get_layer_count (const Dwg_Data *dwg)
{
  Dwg_Object *ctrl;
  assert (dwg);
  ctrl = dwg_get_first_object (dwg, DWG_TYPE_LAYER_CONTROL);
  if (ctrl && ctrl->tio.object && ctrl->tio.object->tio.LAYER_CONTROL)
    return ctrl->tio.object->tio.LAYER_CONTROL->num_entries;
  else
    return 0;
}

EXPORT Dwg_Object_LAYER **
dwg_get_layers (const Dwg_Data *dwg)
{
  unsigned int i;
  unsigned int num_layers = dwg_get_layer_count (dwg);
  Dwg_Object_LAYER_CONTROL *_ctrl;
  Dwg_Object_LAYER **layers;
  Dwg_Object *ctrl;

  assert (dwg);
  ctrl = dwg_get_first_object (dwg, DWG_TYPE_LAYER_CONTROL);
  if (!ctrl || !ctrl->tio.object || !ctrl->tio.object->tio.LAYER_CONTROL)
    return NULL;
  _ctrl = ctrl->tio.object->tio.LAYER_CONTROL;
  assert (_ctrl);
  layers
      = (Dwg_Object_LAYER **)calloc (num_layers, sizeof (Dwg_Object_LAYER *));
  for (i = 0; i < num_layers; i++)
    {
      Dwg_Object *obj = dwg_ref_object ((Dwg_Data *)dwg, _ctrl->entries[i]);
      if (obj && obj->fixedtype == DWG_TYPE_LAYER)
        layers[i] = obj->tio.object->tio.LAYER;
    }
  return layers;
}

BITCODE_BL
dwg_get_object_num_objects (const Dwg_Data *dwg)
{
  assert (dwg);
  return dwg->num_objects - dwg->num_entities;
}

BITCODE_BL
dwg_get_num_objects (const Dwg_Data *dwg)
{
  assert (dwg);
  return dwg->num_objects;
}

BITCODE_BL
dwg_get_num_entities (const Dwg_Data *dwg)
{
  assert (dwg);
  return dwg->num_entities;
}

/** Returns a copy of all entities */
EXPORT Dwg_Object_Entity **
dwg_get_entities (const Dwg_Data *dwg)
{
  BITCODE_BL i, ent_count = 0;
  Dwg_Object_Entity **entities;

  assert (dwg);
  entities = (Dwg_Object_Entity **)calloc (dwg_get_num_entities (dwg),
                                           sizeof (Dwg_Object_Entity *));
  for (i = 0; i < dwg->num_objects; i++)
    {
      if (dwg->object[i].supertype == DWG_SUPERTYPE_ENTITY)
        {
          entities[ent_count] = dwg->object[i].tio.entity;
          ent_count++;
          assert (ent_count < dwg->num_objects);
        }
    }
  return entities;
}

EXPORT Dwg_Object_LAYER *
dwg_get_entity_layer (const Dwg_Object_Entity *ent)
{
  Dwg_Object *obj;
  // we'd really need the dwg_api or dwg to check dirty_refs
  if (ent && ent->layer && (obj = ent->layer->obj)
      && obj->fixedtype == DWG_TYPE_LAYER)
    return obj->tio.object->tio.LAYER;
  else
    // TODO: empty means default layer 0
    return NULL;
}

EXPORT Dwg_Object *
dwg_next_object (const Dwg_Object *obj)
{
  Dwg_Data *dwg;
  if (!obj)
    return NULL;
  dwg = obj->parent;
  if ((obj->index + 1) > (dwg->num_objects - 1))
    return NULL;
  return &dwg->object[obj->index + 1];
}

/**
 * Find an object given its handle
 */
EXPORT Dwg_Object *
dwg_ref_object (Dwg_Data *restrict dwg, Dwg_Object_Ref *restrict ref)
{
  if (!ref)
    return NULL;
  if (dwg->dirty_refs)
    dwg_resolve_objectrefs_silent (dwg);
  if (ref->obj && !dwg->dirty_refs)
    {
      if (ref->obj->handle.value
          == ref->absolute_ref) // not a wrong sort_ents import
        return ref->obj;
      else
        {
          LOG_WARN ("Wrong ref_object: " FORMAT_HV " != " FORMAT_RLLx,
                    ref->obj->handle.value, ref->absolute_ref);
          ref->obj = NULL;
          // dwg_resolve_objectrefs_silent (dwg);
        }
    }
  // if (dwg->header.from_version < R_12 && !ref->absolute_ref)
  //   { // resolve r11_idx to absolute_ref, looking up in the table entries
  //     LOG_WARN ("Cannot resolve r11_idx %u", ref->r11_idx)
  //   }
  //  Without obj we don't get an absolute_ref from relative OFFSETOBJHANDLE
  //  handle types.
  if ((ref->handleref.code < 6 && dwg_resolve_handleref (ref, NULL))
      || ref->absolute_ref)
    {
      Dwg_Object *obj = dwg_resolve_handle (dwg, ref->absolute_ref);
      if (!dwg->dirty_refs && obj)
        ref->obj = obj;
      return obj;
    }
  else
    return NULL;
}

/**
 * Find an object given its handle and relative base object.
 * OFFSETOBJHANDLE, handleref.code > 6.
 */
EXPORT Dwg_Object *
dwg_ref_object_relative (Dwg_Data *restrict dwg, Dwg_Object_Ref *restrict ref,
                         const Dwg_Object *restrict obj)
{
  if (ref->obj && !dwg->dirty_refs)
    return ref->obj;
  if (dwg_resolve_handleref (ref, obj))
    {
      Dwg_Object *o = dwg_resolve_handle (dwg, ref->absolute_ref);
      if (!dwg->dirty_refs && o)
        ref->obj = o;
      return o;
    }
  else
    return NULL;
}

/**
 * Find a pointer to an object given its absolute id (handle).
 * TODO: Check and update each handleref obj cache.
 * Note that absref 0 is illegal here, I think.
 */
EXPORT Dwg_Object *
dwg_resolve_handle (const Dwg_Data *dwg, const BITCODE_RLL absref)
{
  uint64_t i;
  if (!absref) // illegal usage
    return NULL;
  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  i = hash_get (dwg->object_map, absref);
  if (i != HASH_NOT_FOUND)
    LOG_HANDLE ("[object_map{" FORMAT_HV "} => " FORMAT_BLL "] ", absref, i);
  if (i == HASH_NOT_FOUND
      || (BITCODE_BL)i >= dwg->num_objects) // the latter being an invalid
                                            // handle (read from DWG)
    {
      // ignore warning on invalid handles. These are warned earlier already
      if (absref && dwg->header_vars.HANDSEED
          && absref < dwg->header_vars.HANDSEED->absolute_ref)
        {
          LOG_WARN ("Object handle not found " FORMAT_BLL "/" FORMAT_RLL
                    " in " FORMAT_BL " objects of max " FORMAT_RLL " handles",
                    absref, absref, dwg->num_objects,
                    dwg->header_vars.HANDSEED->absolute_ref);
        }
      else
        {
          LOG_WARN ("Object handle not found " FORMAT_BLL "/" FORMAT_RLL,
                    absref, absref);
        }
      return NULL;
    }
  LOG_INSANE ("[resolve " FORMAT_HV " => " FORMAT_BLL "] ", absref, i);
  return &dwg->object[i]; // allow value 0
}

/**
 * Silent variant of dwg_resolve_handle
 */
EXPORT Dwg_Object *
dwg_resolve_handle_silent (const Dwg_Data *dwg, const BITCODE_RLL absref)
{
  uint64_t i;
  if (!absref) // illegal usage
    return NULL;
  i = hash_get (dwg->object_map, absref);
  if (i == HASH_NOT_FOUND
      || (BITCODE_BL)i >= dwg->num_objects) // the latter being an invalid
                                            // handle (read from DWG)
    return NULL;
  return &dwg->object[i]; // allow value 0
}

EXPORT Dwg_Object *
dwg_ref_object_silent (Dwg_Data *restrict dwg, Dwg_Object_Ref *restrict ref)
{
  if (!ref)
    return NULL;
  if (dwg->dirty_refs)
    dwg_resolve_objectrefs_silent (dwg);
  if (dwg->dirty_refs == 0 && ref->obj != NULL)
    return ref->obj;
  if ((ref->handleref.code < 6
       && dwg_resolve_handleref ((Dwg_Object_Ref *)ref, NULL))
      || ref->absolute_ref)
    {
      Dwg_Object *obj = dwg_resolve_handle_silent (dwg, ref->absolute_ref);
      if (obj)
        ref->obj = obj;
      return obj;
    }
  else
    return NULL;
}

/* set ref->absolute_ref from obj, for a subsequent dwg_resolve_handle() */
EXPORT int
dwg_resolve_handleref (Dwg_Object_Ref *restrict ref,
                       const Dwg_Object *restrict obj)
{
  /*
   * With TYPEDOBJHANDLE 2-5 the code indicates the type of ownership:
   *   2 Soft owner
   *   3 Hard owner
   *   4 Soft pointer
   *   5 Hard pointer
   * With OFFSETOBJHANDLE >5 the code 4 handle is stored as an offset from some
   * other handle.
   */
  switch (ref->handleref.code)
    {
    // implicit code: 4
    case 6:
      ref->absolute_ref = (obj->handle.value + 1);
      break;
    case 8:
      ref->absolute_ref = (obj->handle.value - 1);
      break;
    case 10:
      ref->absolute_ref = (obj->handle.value + ref->handleref.value);
      break;
    case 12:
      ref->absolute_ref = (obj->handle.value - ref->handleref.value);
      break;
    case 2:
    case 3:
    case 4:
    case 5:
      ref->absolute_ref = ref->handleref.value;
      break;
    case 0: // ignore?
      ref->absolute_ref = ref->handleref.value;
      break;
    default:
      ref->absolute_ref = ref->handleref.value;
      if (!loglevel && obj && obj->parent)
        loglevel = obj->parent->opts & DWG_OPTS_LOGLEVEL;
      LOG_WARN ("Invalid handle pointer code %d", ref->handleref.code);
      return 0;
    }
  return 1;
}

/** Returns the block_control for the DWG,
    containing the list of all blocks headers.
*/
EXPORT Dwg_Object_BLOCK_CONTROL *
dwg_block_control (Dwg_Data *dwg)
{
  if (!dwg->block_control.parent)
    {
      Dwg_Object *obj;
      Dwg_Object_Ref *ctrl = dwg->header_vars.BLOCK_CONTROL_OBJECT;
      if (!ctrl)
        dwg->header_vars.BLOCK_CONTROL_OBJECT = ctrl =
          dwg_find_table_control (dwg, "BLOCK_CONTROL");
      if (!ctrl || !(obj = dwg_ref_object (dwg, ctrl))
          || obj->fixedtype != DWG_TYPE_BLOCK_CONTROL)
        {
          LOG_ERROR (
              "dwg.block_control and HEADER.BLOCK_CONTROL_OBJECT missing");
          return NULL;
        }
      else
        {
          dwg->block_control = *obj->tio.object->tio.BLOCK_CONTROL;
        }
    }
  return &(dwg->block_control);
}

/** Returns the model space block object for the DWG.
    On r2010 and r2013 it could be different to the canonical
    dwg->block_control.model_space.
*/
EXPORT Dwg_Object_Ref *
dwg_model_space_ref (Dwg_Data *dwg)
{
  Dwg_Object *obj;
  Dwg_Object_BLOCK_CONTROL *block_control;
  if (dwg->dirty_refs)
    dwg_resolve_objectrefs_silent (dwg);
  if (dwg->header_vars.BLOCK_RECORD_MSPACE && !dwg->dirty_refs
      && dwg->header_vars.BLOCK_RECORD_MSPACE->obj)
    return dwg->header_vars.BLOCK_RECORD_MSPACE;
  if (dwg->block_control.model_space && !dwg->dirty_refs
      && dwg->block_control.model_space->obj)
    {
      dwg->header_vars.BLOCK_RECORD_MSPACE = dwg->block_control.model_space;
      return dwg->block_control.model_space;
    }
  block_control = dwg_block_control (dwg);
  if (block_control && block_control->model_space && !dwg->dirty_refs
      && block_control->model_space->obj)
    {
      dwg->block_control.model_space = block_control->model_space;
      dwg->header_vars.BLOCK_RECORD_MSPACE = block_control->model_space;
      return block_control->model_space;
    }
  obj = dwg_get_first_object (dwg, DWG_TYPE_BLOCK_CONTROL);
  if (!obj)
    return NULL;
  block_control = obj->tio.object->tio.BLOCK_CONTROL;
  if (block_control && block_control->model_space && !dwg->dirty_refs
      && block_control->model_space->obj)
    {
      dwg->block_control.model_space = block_control->model_space;
      dwg->header_vars.BLOCK_RECORD_MSPACE = block_control->model_space;
      return block_control->model_space;
    }
  return NULL;
}

/** Returns the paper space block object for the DWG.
 */
EXPORT Dwg_Object_Ref *
dwg_paper_space_ref (Dwg_Data *dwg)
{
  if (dwg->dirty_refs)
    dwg_resolve_objectrefs_silent (dwg);
  if (dwg->header_vars.BLOCK_RECORD_PSPACE && !dwg->dirty_refs
      && dwg->header_vars.BLOCK_RECORD_PSPACE->obj)
    return dwg->header_vars.BLOCK_RECORD_PSPACE;
  return dwg->block_control.paper_space && !dwg->dirty_refs
                 && dwg->block_control.paper_space->obj
             ? dwg->block_control.paper_space
             : NULL;
}

/** Returns the model space block object for the DWG.
 */
EXPORT Dwg_Object *
dwg_model_space_object (Dwg_Data *dwg)
{
  Dwg_Object *obj;
  Dwg_Object_Ref *msref = dwg_model_space_ref (dwg);
  Dwg_Object_BLOCK_CONTROL *ctrl;

  if (msref && (obj = dwg_ref_object (dwg, msref))
      && obj->fixedtype == DWG_TYPE_BLOCK_HEADER)
    return obj;
  ctrl = dwg_block_control (dwg);
  msref = ctrl ? ctrl->model_space : NULL;
  if (msref && (obj = dwg_ref_object (dwg, msref))
      && obj->fixedtype == DWG_TYPE_BLOCK_HEADER)
    {
      if (!dwg->header_vars.BLOCK_RECORD_MSPACE)
        dwg->header_vars.BLOCK_RECORD_MSPACE = msref;
      return obj;
    }
  msref = dwg->header_vars.BLOCK_RECORD_MSPACE;
  if (msref && (obj = dwg_ref_object (dwg, msref))
      && obj->fixedtype == DWG_TYPE_BLOCK_HEADER)
    {
      if (ctrl)
        ctrl->model_space = msref;
      return obj;
    }
  if (!dwg->object_map) // for dwg_add_Document()
    dwg->object_map = hash_new (100);
  // TODO <= r11
  obj = dwg_resolve_handle (dwg, dwg->header.version >= R_2000 ? 0x1F : 0x17);
  if (obj && obj->fixedtype == DWG_TYPE_BLOCK_HEADER)
    return obj;
  obj = dwg_find_first_type (dwg, DWG_TYPE_BLOCK_HEADER);
  if (obj && obj->fixedtype == DWG_TYPE_BLOCK_HEADER)
    return obj;
  else
    return NULL;
}

/** Returns the paper space block object for the DWG.
 */
EXPORT Dwg_Object *
dwg_paper_space_object (Dwg_Data *dwg)
{
  Dwg_Object *obj;
  Dwg_Object_Ref *psref = dwg_paper_space_ref (dwg);
  Dwg_Object_BLOCK_CONTROL *ctrl;

  if (psref && (obj = dwg_ref_object (dwg, psref))
      && obj->fixedtype == DWG_TYPE_BLOCK_HEADER)
    return obj;
  ctrl = dwg_block_control (dwg);
  psref = ctrl ? ctrl->paper_space : NULL;
  if (psref && (obj = dwg_ref_object (dwg, psref))
      && obj->fixedtype == DWG_TYPE_BLOCK_HEADER)
    {
      if (!dwg->header_vars.BLOCK_RECORD_PSPACE)
        dwg->header_vars.BLOCK_RECORD_PSPACE = psref;
      return obj;
    }
  psref = dwg->header_vars.BLOCK_RECORD_PSPACE;
  if (psref && (obj = dwg_ref_object (dwg, psref))
      && obj->fixedtype == DWG_TYPE_BLOCK_HEADER)
    {
      if (ctrl)
        ctrl->paper_space = psref;
      return obj;
    }
  else
    return NULL;
}

/** Returns the first entity owned by the block hdr, or NULL.
 */
EXPORT Dwg_Object *
get_first_owned_entity (const Dwg_Object *hdr)
{
  const Dwg_Version_Type version = hdr->parent->header.version;
  Dwg_Object_BLOCK_HEADER *_hdr = hdr->tio.object->tio.BLOCK_HEADER;
  if (hdr->fixedtype != DWG_TYPE_BLOCK_HEADER)
    {
      LOG_ERROR ("Invalid BLOCK_HEADER type %d", hdr->fixedtype);
      return NULL;
    }

  if (R_13b1 <= version && version <= R_2000)
    {
      Dwg_Data *dwg = hdr->parent;
      if (dwg->dirty_refs)
        dwg_resolve_objectrefs_silent (dwg);
      /* With r2000 we rather follow the next_entity chain */
      return _hdr->first_entity ? _hdr->first_entity->obj : NULL;
    }
  else if (version >= R_2004 || version < R_13b1)
    {
      _hdr->__iterator = 0;
      if (_hdr->entities && _hdr->num_owned && _hdr->entities[0])
        {
          Dwg_Data *dwg = hdr->parent;
          Dwg_Object *obj = dwg_ref_object (dwg, _hdr->entities[0]);
          if (version < R_13b1 && obj && obj->fixedtype == DWG_TYPE_JUMP)
            return dwg_resolve_jump (obj);
          else
            return obj;
        }
      else
        return NULL;
    }

  // TODO: preR13 block table
  LOG_ERROR ("Unsupported version %s\n", dwg_version_type (version));
  return NULL;
}

/** Returns the next entity owned by mspace or pspace or NULL.
 */
EXPORT Dwg_Object *
dwg_next_entity (const Dwg_Object *restrict obj)
{
  Dwg_Object_Ref *restrict next;

  if (obj == NULL || obj->parent == NULL
      || obj->supertype != DWG_SUPERTYPE_ENTITY)
    return NULL;
  if (obj->parent->header.version < R_2004
      && obj->parent->header.version > R_12)
    {
      if (!obj->tio.entity) // decoding error
        goto next_obj;
      next = obj->tio.entity->next_entity;
      if (next && next->absolute_ref)
        {
          Dwg_Object *next_obj = dwg_ref_object_silent (obj->parent, next);
          return (obj == next_obj
                  || next_obj->supertype != DWG_SUPERTYPE_ENTITY)
                     ? NULL
                     : next_obj;
        }
      else
        goto next_obj;
    }
  else
    {
    next_obj:
      obj = dwg_next_object (obj);
      while (obj && obj->supertype != DWG_SUPERTYPE_ENTITY)
        {
          obj = dwg_next_object (obj);
        }
      return (Dwg_Object *)obj;
    }
}

/** Returns the next entity owned by the block hdr, or NULL.
 *  Not subentities: ATTRIB, VERTEX.
 */
EXPORT Dwg_Object *
get_next_owned_entity (const Dwg_Object *restrict hdr,
                       const Dwg_Object *restrict current)
{
  Dwg_Data *dwg = hdr->parent;
  const Dwg_Version_Type version = dwg->header.version;
  Dwg_Object_BLOCK_HEADER *_hdr = hdr->tio.object->tio.BLOCK_HEADER;
  if (hdr->fixedtype != DWG_TYPE_BLOCK_HEADER)
    {
      LOG_ERROR ("Invalid BLOCK_HEADER type %d", hdr->fixedtype);
      return NULL;
    }

  if (R_13b1 <= version && version <= R_2000)
    {
      Dwg_Object *obj;
      if (_hdr->last_entity == NULL
          || current == _hdr->last_entity->obj)
        return NULL;
      obj = dwg_next_entity (current);
      while (obj
             && (obj->fixedtype == DWG_TYPE_ATTDEF
                 || obj->fixedtype == DWG_TYPE_ATTRIB
                 || obj->fixedtype == DWG_TYPE_VERTEX_2D
                 || obj->fixedtype == DWG_TYPE_VERTEX_3D
                 || obj->fixedtype == DWG_TYPE_VERTEX_MESH
                 || obj->fixedtype == DWG_TYPE_VERTEX_PFACE
                 || obj->fixedtype == DWG_TYPE_VERTEX_PFACE_FACE))
        {
          obj = dwg_next_entity (obj);
          // this may happen with r2000 attribs
          if (obj && obj->tio.entity != NULL
              && obj->tio.entity->ownerhandle != NULL
              && obj->tio.entity->ownerhandle->absolute_ref
                     != hdr->handle.value)
            obj = NULL;
          if (obj == _hdr->last_entity->obj) // early exit
            return obj;
        }
      return obj;
    }
  else if (version >= R_2004 || version < R_13b1)
    {
      Dwg_Object *obj;
      Dwg_Object_Ref *ref;
      _hdr->__iterator++;
      if (_hdr->__iterator == _hdr->num_owned)
        return NULL;
      ref = _hdr->entities ? _hdr->entities[_hdr->__iterator] : NULL;
      obj = ref ? dwg_ref_object (dwg, ref) : NULL;
      if (version < R_13b1 && obj && obj->fixedtype == DWG_TYPE_JUMP)
        return dwg_resolve_jump (obj);
      else
        return obj;
    }

  LOG_ERROR ("Unsupported version %s\n", dwg_version_type (version));
  return NULL;
}

/** Returns the first subentity owned by the insert or polyline.
 */
EXPORT Dwg_Object *
get_first_owned_subentity (const Dwg_Object *owner)
{
  Dwg_Data *dwg = owner->parent;
  const Dwg_Version_Type version = dwg->header.version;
  const unsigned int type = owner->fixedtype;
  if (type == DWG_TYPE_INSERT)
    {
      Dwg_Entity_INSERT *_obj = owner->tio.entity->tio.INSERT;
      if (version >= R_13b1 && version <= R_2000)
        return _obj->first_attrib ? _obj->first_attrib->obj : NULL;
      else
        return _obj->attribs && _obj->attribs[0]
                   ? dwg_ref_object (dwg, _obj->attribs[0])
                   : NULL;
    }
  else if (type == DWG_TYPE_MINSERT)
    {
      Dwg_Entity_MINSERT *_obj = owner->tio.entity->tio.MINSERT;
      if (version >= R_13b1 && version <= R_2000)
        return _obj->first_attrib ? dwg_ref_object (dwg, _obj->first_attrib)
                                  : NULL;
      else
        return _obj->attribs && _obj->attribs[0]
                   ? dwg_ref_object (dwg, _obj->attribs[0])
                   : NULL;
    }
  else if (type == DWG_TYPE_POLYLINE_2D || type == DWG_TYPE_POLYLINE_3D
           || type == DWG_TYPE_POLYLINE_PFACE
           || type == DWG_TYPE_POLYLINE_MESH)
    {
      // guaranteed structure
      Dwg_Entity_POLYLINE_2D *_obj = owner->tio.entity->tio.POLYLINE_2D;
      if (version >= R_13b1 && version <= R_2000)
        return _obj->first_vertex ? dwg_ref_object (dwg, _obj->first_vertex)
                                  : NULL;
      else
        return _obj->vertex && _obj->vertex[0]
                   ? dwg_ref_object (dwg, _obj->vertex[0])
                   : NULL;
    }
  else
    {
      LOG_ERROR ("Wrong type %d, has no subentity", type);
    }
  return NULL;
}

/** Returns the next subentity owned by the object.
 */
EXPORT Dwg_Object *
get_next_owned_subentity (const Dwg_Object *restrict owner,
                          const Dwg_Object *restrict current)
{
  Dwg_Data *dwg = owner->parent;
  Dwg_Version_Type version = dwg->header.version;
  const Dwg_Object_Type type = (const Dwg_Object_Type)owner->fixedtype;
  Dwg_Object_Entity *ent = owner->tio.entity;
  Dwg_Object *obj = dwg_next_object (current);

  if (type == DWG_TYPE_INSERT)
    {
      Dwg_Entity_INSERT *_obj = owner->tio.entity->tio.INSERT;
      if (version >= R_13b1 && version <= R_2000)
        return (_obj->last_attrib && current != _obj->last_attrib->obj
                && obj->fixedtype == DWG_TYPE_ATTRIB)
                   ? obj
                   : NULL;
      else
        {
          ent->__iterator++;
          if (ent->__iterator == _obj->num_owned)
            {
              ent->__iterator = 0;
              return NULL;
            }
          else
            return _obj->attribs
                       ? dwg_ref_object (dwg, _obj->attribs[ent->__iterator])
                       : NULL;
        }
    }
  else if (type == DWG_TYPE_MINSERT)
    {
      Dwg_Entity_MINSERT *_obj = owner->tio.entity->tio.MINSERT;
      if (version >= R_13b1 && version <= R_2000)
        return (_obj->last_attrib && current != _obj->last_attrib->obj
                && obj->fixedtype == DWG_TYPE_ATTRIB)
                   ? obj
                   : NULL;
      else
        {
          ent->__iterator++;
          if (ent->__iterator == _obj->num_owned)
            {
              ent->__iterator = 0;
              return NULL;
            }
          else
            return _obj->attribs
                       ? dwg_ref_object (dwg, _obj->attribs[ent->__iterator])
                       : NULL;
        }
    }
  else if (type == DWG_TYPE_POLYLINE_2D || type == DWG_TYPE_POLYLINE_3D
           || type == DWG_TYPE_POLYLINE_PFACE
           || type == DWG_TYPE_POLYLINE_MESH)
    {
      // guaranteed structure
      Dwg_Entity_POLYLINE_2D *_obj = owner->tio.entity->tio.POLYLINE_2D;
      if (version >= R_13b1 && version <= R_2000)
        return (_obj->last_vertex && current != _obj->last_vertex->obj) ? obj
                                                                        : NULL;
      else
        {
          ent->__iterator++;
          if (ent->__iterator == _obj->num_owned)
            {
              ent->__iterator = 0;
              return NULL;
            }
          else
            return _obj->vertex
                       ? dwg_ref_object (dwg, _obj->vertex[ent->__iterator])
                       : NULL;
        }
    }
  else
    {
      LOG_ERROR ("Wrong type %d, has no subentity", type);
    }
  return NULL;
}

/** Returns the BLOCK entity owned by the block hdr.
 *  Only NULL on illegal hdr argument or dwg version.
 */
EXPORT Dwg_Object *
get_first_owned_block (const Dwg_Object *hdr)
{
  Dwg_Data *dwg = hdr->parent;
  Dwg_Version_Type version = dwg->header.version;
  const Dwg_Object_BLOCK_HEADER *restrict _hdr
      = hdr->tio.object->tio.BLOCK_HEADER;
  if (hdr->fixedtype != DWG_TYPE_BLOCK_HEADER)
    {
      LOG_ERROR ("Invalid BLOCK_HEADER type %d", hdr->fixedtype);
      return NULL;
    }

  if (_hdr->block_entity)
    return dwg_ref_object (dwg, _hdr->block_entity);
  else
    {
      Dwg_Object *obj = (Dwg_Object *)hdr;
      while (obj && obj->fixedtype != DWG_TYPE_BLOCK)
        obj = dwg_next_object (obj);
      return obj;
    }
  LOG_ERROR ("Unsupported version %s\n", dwg_version_type (version));
  return NULL;
}

/** Returns the next block object after current owned by the block hdr, or
 *  NULL.
 */
EXPORT Dwg_Object *
get_next_owned_block (const Dwg_Object *restrict hdr,
                      const Dwg_Object *restrict current)
{
  Dwg_Data *dwg = hdr->parent;
  Dwg_Version_Type version = dwg->header.version;
  const Dwg_Object_BLOCK_HEADER *restrict _hdr
      = hdr->tio.object->tio.BLOCK_HEADER;
  if (hdr->fixedtype != DWG_TYPE_BLOCK_HEADER)
    {
      LOG_ERROR ("Invalid BLOCK_HEADER type %d", hdr->fixedtype);
      return NULL;
    }

  if (1 || version >= R_13b1)
    {
      if (!_hdr->endblk_entity
          || current->handle.value >= _hdr->endblk_entity->absolute_ref)
        return NULL;
      return dwg_next_object (current);
    }

  LOG_ERROR ("Unsupported version %s\n", dwg_version_type (version));
  return NULL;
}

/** Returns the next block object until last_entity
 *  after current owned by the block hdr, or NULL.
 */
EXPORT Dwg_Object *
get_next_owned_block_entity (const Dwg_Object *restrict hdr,
                             const Dwg_Object *restrict current)
{
  Dwg_Data *dwg;
  Dwg_Version_Type version;
  Dwg_Object_BLOCK_HEADER *restrict _hdr;

  if (hdr->fixedtype != DWG_TYPE_BLOCK_HEADER)
    {
      LOG_ERROR ("Invalid BLOCK_HEADER type %d", hdr->fixedtype);
      return NULL;
    }

  dwg = hdr->parent;
  version = dwg->header.version;
  _hdr = hdr->tio.object->tio.BLOCK_HEADER;

  if (R_13b1 <= version && version <= R_2000)
    {
      /* With r2000 we rather follow the next_entity chain. It may jump around
       * the linked list. */
      if (!_hdr->last_entity
          || current->handle.value == _hdr->last_entity->absolute_ref)
        return NULL;
      return dwg_next_entity (current);
    }
  if (version > R_2000 || version < R_13b1)
    {
      Dwg_Object *obj;
      Dwg_Object_Ref *ref;
      _hdr->__iterator++;
      if (_hdr->__iterator == _hdr->num_owned)
        return NULL;
      ref = _hdr->entities ? _hdr->entities[_hdr->__iterator] : NULL;
      obj = ref ? dwg_ref_object (dwg, ref) : NULL;
      if (version < R_13b1 && obj && obj->fixedtype == DWG_TYPE_JUMP)
        return dwg_resolve_jump (obj);
      else
        return obj;
    }

  LOG_ERROR ("Unsupported version %s\n", dwg_version_type (version));
  return NULL;
}

/** Returns the last ENDBLK entity owned by the block hdr.
 *  Only NULL on illegal hdr argument or dwg version.
 */
EXPORT Dwg_Object *
get_last_owned_block (const Dwg_Object *restrict hdr)
{
  Dwg_Data *dwg = hdr->parent;
  Dwg_Object_BLOCK_HEADER *restrict _hdr = hdr->tio.object->tio.BLOCK_HEADER;
  const Dwg_Version_Type version = dwg->header.version;
  if (hdr->fixedtype != DWG_TYPE_BLOCK_HEADER)
    {
      LOG_ERROR ("Invalid BLOCK_HEADER type %d", hdr->fixedtype);
      return NULL;
    }

  if (1 || version >= R_13b1)
    {
      if (_hdr->endblk_entity && _hdr->endblk_entity->obj)
        return _hdr->endblk_entity->obj;
      else
        {
          Dwg_Object *obj = (Dwg_Object *)hdr;
          while (obj && obj->fixedtype != DWG_TYPE_ENDBLK)
            obj = dwg_next_object (obj);
          if (obj && obj->fixedtype == DWG_TYPE_ENDBLK)
            {
              if (!_hdr->endblk_entity)
                {
                  _hdr->endblk_entity
                      = (BITCODE_H)calloc (1, sizeof (Dwg_Object_Ref));
                  if (_hdr->endblk_entity)
                    {
                      _hdr->endblk_entity->obj = obj;
                      _hdr->endblk_entity->handleref.value
                          = _hdr->endblk_entity->absolute_ref
                          = obj->handle.value;
                    }
                }
              else if (!_hdr->endblk_entity->obj)
                _hdr->endblk_entity->obj = obj;
            }
          return obj;
        }
    }

  LOG_ERROR ("Unsupported version %s\n", dwg_version_type (version));
  return NULL;
}

EXPORT Dwg_Object *
dwg_get_first_object (const Dwg_Data *dwg, const Dwg_Object_Type type)
{
  for (unsigned i = 0; i < dwg->num_objects; i++)
    {
      Dwg_Object *const obj = &dwg->object[i];
      if (obj->fixedtype == type && obj->tio.object
          && obj->tio.object->tio.APPID)
        return obj;
    }
  return NULL;
}

EXPORT Dwg_Object *
dwg_get_next_object (const Dwg_Data *dwg, const Dwg_Object_Type type,
                     const BITCODE_RL index)
{
  for (unsigned i = index; i < dwg->num_objects; i++)
    {
      Dwg_Object *const obj = &dwg->object[i];
      if (obj->fixedtype == type && obj->tio.object
          && obj->tio.object->tio.APPID)
        return obj;
    }
  return NULL;
}

EXPORT int
dwg_class_is_entity (const Dwg_Class *restrict klass)
{
  return (klass != NULL && (int)klass->item_class_id == 0x1f2) ? 1 : 0;
}

EXPORT int
dwg_obj_is_control (const Dwg_Object *obj)
{
  const unsigned int type = obj->fixedtype;
  return (obj->supertype == DWG_SUPERTYPE_OBJECT)
         && (type == DWG_TYPE_BLOCK_CONTROL || type == DWG_TYPE_LAYER_CONTROL
             || type == DWG_TYPE_STYLE_CONTROL
             || type == DWG_TYPE_LTYPE_CONTROL || type == DWG_TYPE_VIEW_CONTROL
             || type == DWG_TYPE_UCS_CONTROL || type == DWG_TYPE_VPORT_CONTROL
             || type == DWG_TYPE_APPID_CONTROL
             || type == DWG_TYPE_DIMSTYLE_CONTROL
             || type == DWG_TYPE_VX_CONTROL);
}

EXPORT int
dwg_obj_is_table (const Dwg_Object *obj)
{
  const unsigned int type = obj->fixedtype;
  return (obj->supertype == DWG_SUPERTYPE_OBJECT)
         && (type == DWG_TYPE_BLOCK_HEADER || type == DWG_TYPE_LAYER
             || type == DWG_TYPE_STYLE || type == DWG_TYPE_LTYPE
             || type == DWG_TYPE_VIEW || type == DWG_TYPE_UCS
             || type == DWG_TYPE_VPORT || type == DWG_TYPE_APPID
             || type == DWG_TYPE_DIMSTYLE || type == DWG_TYPE_VX_TABLE_RECORD);
}

EXPORT int
dwg_obj_is_subentity (const Dwg_Object *obj)
{
  const unsigned int type = obj->fixedtype;
  return (obj->supertype == DWG_SUPERTYPE_ENTITY)
         && (type == DWG_TYPE_ATTRIB || type == DWG_TYPE_VERTEX_2D
             || type == DWG_TYPE_VERTEX_3D || type == DWG_TYPE_VERTEX_MESH
             || type == DWG_TYPE_VERTEX_PFACE
             || type == DWG_TYPE_VERTEX_PFACE_FACE);
}

EXPORT int
dwg_obj_has_subentity (const Dwg_Object *obj)
{
  const unsigned int type = obj->fixedtype;
  return (obj->supertype == DWG_SUPERTYPE_ENTITY)
         && (type == DWG_TYPE_INSERT || type == DWG_TYPE_MINSERT
             || type == DWG_TYPE_POLYLINE_2D || type == DWG_TYPE_POLYLINE_3D
             || type == DWG_TYPE_POLYLINE_PFACE
             || type == DWG_TYPE_POLYLINE_MESH);
}

// All entities deriving from 3DSOLID/AcDbModelerGeometry
EXPORT int
dwg_obj_is_3dsolid (const Dwg_Object *obj)
{
  const Dwg_Object_Type type = obj->fixedtype;
  return (obj->supertype == DWG_SUPERTYPE_OBJECT
          && (type == DWG_TYPE_ACSH_BREP_CLASS
              || type == DWG_TYPE_ASSOCASMBODYACTIONPARAM))
         || (obj->supertype == DWG_SUPERTYPE_ENTITY
             && (type == DWG_TYPE__3DSOLID || type == DWG_TYPE_REGION
                 || type == DWG_TYPE_BODY || type == DWG_TYPE_EXTRUDEDSURFACE
                 || type == DWG_TYPE_LOFTEDSURFACE
                 || type == DWG_TYPE_NURBSURFACE
                 || type == DWG_TYPE_PLANESURFACE
                 || type == DWG_TYPE_REVOLVEDSURFACE
                 || type == DWG_TYPE_SWEPTSURFACE));
}

EXPORT int
dwg_obj_is_acsh (const Dwg_Object *obj)
{
  const Dwg_Object_Type type = obj->fixedtype;
  return (obj->supertype == DWG_SUPERTYPE_OBJECT
          && (type == DWG_TYPE_ACSH_BOOLEAN_CLASS
              || type == DWG_TYPE_ACSH_BOX_CLASS
              || type == DWG_TYPE_ACSH_BREP_CLASS
              || type == DWG_TYPE_ACSH_CHAMFER_CLASS
              || type == DWG_TYPE_ACSH_CONE_CLASS
              || type == DWG_TYPE_ACSH_CYLINDER_CLASS
              || type == DWG_TYPE_ACSH_EXTRUSION_CLASS
              || type == DWG_TYPE_ACSH_FILLET_CLASS
              || type == DWG_TYPE_ACSH_HISTORY_CLASS
              || type == DWG_TYPE_ACSH_LOFT_CLASS
              || type == DWG_TYPE_ACSH_PYRAMID_CLASS
              || type == DWG_TYPE_ACSH_REVOLVE_CLASS
              || type == DWG_TYPE_ACSH_SPHERE_CLASS
              || type == DWG_TYPE_ACSH_SWEEP_CLASS
              || type == DWG_TYPE_ACSH_TORUS_CLASS
              || type == DWG_TYPE_ACSH_WEDGE_CLASS));
}

EXPORT Dwg_Section_Type
dwg_section_type (const char *restrict name)
{
  if (name == NULL)
    return SECTION_UNKNOWN; // but could also be INFO or SYSTEM_MAP
  else if (strEQc (name, "AcDb:Header"))
    return SECTION_HEADER;
  else if (strEQc (name, "AcDb:Classes"))
    return SECTION_CLASSES;
  else if (strEQc (name, "AcDb:SummaryInfo"))
    return SECTION_SUMMARYINFO;
  else if (strEQc (name, "AcDb:Preview"))
    return SECTION_PREVIEW;
  else if (strEQc (name, "AcDb:VBAProject"))
    return SECTION_VBAPROJECT;
  else if (strEQc (name, "AcDb:AppInfo"))
    return SECTION_APPINFO;
  else if (strEQc (name, "AcDb:FileDepList"))
    return SECTION_FILEDEPLIST;
  else if (strEQc (name, "AcDb:RevHistory"))
    return SECTION_REVHISTORY;
  else if (strEQc (name, "AcDb:Security"))
    return SECTION_SECURITY;
  else if (strEQc (name, "AcDb:AcDbObjects"))
    return SECTION_OBJECTS;
  else if (strEQc (name, "AcDb:ObjFreeSpace"))
    return SECTION_OBJFREESPACE;
  else if (strEQc (name, "AcDb:Template"))
    return SECTION_TEMPLATE;
  else if (strEQc (name, "AcDb:Handles"))
    return SECTION_HANDLES;
  else if (strEQc (name, "AcDb:AcDsPrototype_1b"))
    // r2013+
    return SECTION_ACDS; // 0xc or 0xd
  else if (strEQc (name, "AcDb:AuxHeader"))
    return SECTION_AUXHEADER;
  else if (strEQc (name, "AcDb:Signature"))
    return SECTION_SIGNATURE;
  else if (strEQc (name, "AcDb:AppInfoHistory"))
    // AC1021
    return SECTION_APPINFOHISTORY;
  return SECTION_UNKNOWN;
}

EXPORT Dwg_Section_Type
dwg_section_wtype (const DWGCHAR *restrict wname)
{
  DWGCHAR *wp;
  char name[24];
  uint16_t c;
  int i = 0;

  if (wname == NULL)
    return SECTION_UNKNOWN; // but could also be INFO or SYSTEM_MAP
  wp = (DWGCHAR *)wname;
  while ((c = *wp++))
    {
      name[i++] = (char)(c & 0xff);
      if (i >= 24)
        return SECTION_UNKNOWN;
    }
  name[i] = '\0';
  return dwg_section_type (name);
}

static const char *const dwg_section_r2004_names[] = {
  "UNKNOWN",               // 0
  "AcDb:Header",           // 1
  "AcDb:AuxHeader",        // 2
  "AcDb:Classes",          // 3
  "AcDb:Handles",          // 4
  "AcDb:Template",         // 5
  "AcDb:ObjFreeSpace",     // 6
  "AcDb:AcDbObjects",      // 7
  "AcDb:RevHistory",       // 8
  "AcDb:SummaryInfo",      // 9
  "AcDb:Preview",          // 10
  "AcDb:AppInfo",          // 11
  "AcDb:AppInfoHistory",   // 12
  "AcDb:FileDepList",      // 13
  "AcDb:Security",         // 14
  "AcDb:VBAProject",       // 15
  "AcDb:Signature",        // 16
  "AcDb:AcDsPrototype_1b", // 17
  "INFO",                  // 18
  "SYSTEM_MAP",            // 19
};
static const char *const dwg_section_r13_names[] = {
  "Header",       // 0
  "Classes",      // 1
  "Handles",      // 2
  "ObjFreeSpace", // 3
  "Template",     // 4
  "AuxHeader",    // 5
  "Thumbnail"     // 6
};
static const char *const dwg_section_r11_names[] = {
  "HEADER",   // 0
  "BLOCK",    // 1
  "LAYER",    // 2
  "STYLE",    // 3
  "LTYPE",    // 4
  "VIEW",     // 5
  "UCS",      // 6
  "VPORT",    // 7
  "APPID",    // 8
  "DIMSTYLE", // 9
  "VX"        // 10
};

const char *
dwg_section_name (const Dwg_Data *dwg, const unsigned int sec_id)
{
  if (dwg->header.version >= R_2004)
    { // Dwg_Section_Type
      return (sec_id <= SECTION_SYSTEM_MAP) ? dwg_section_r2004_names[sec_id]
                                            : NULL;
    }
  else if (dwg->header.version > R_11)
    { // Dwg_Section_Type_r13
      return (sec_id <= SECTION_THUMBNAIL_R13) ? dwg_section_r13_names[sec_id]
                                               : NULL;
    }
  else
    { // Dwg_Section_Type_r11
      return (sec_id <= SECTION_VX) ? dwg_section_r11_names[sec_id] : NULL;
    }
}

EXPORT enum RESBUF_VALUE_TYPE
dwg_resbuf_value_type (short gc)
{
  if (gc >= 300)
    {
      if (gc >= 440)
        {
          if (gc >= 1000) // 1000-1071
            {
              if (gc == 1004)
                return DWG_VT_BINARY;
              if (gc <= 1009)
                return DWG_VT_STRING;
              if (gc <= 1039)
                return DWG_VT_POINT3D;
              if (gc <= 1042)
                return DWG_VT_REAL;
              if (gc <= 1069)
                return DWG_VT_POINT3D;
              if (gc <= 1070)
                return DWG_VT_INT16;
              if (gc == 1071)
                return DWG_VT_INT32;
            }
          else // 440-999
            {
              if (gc <= 459)
                return DWG_VT_INT32;
              if (gc <= 469)
                return DWG_VT_REAL;
              if (gc <= 479)
                return DWG_VT_STRING;
              if (gc <= 998)
                return DWG_VT_INVALID;
              if (gc == 999)
                return DWG_VT_STRING; // lgtm [cpp/constant-comparison]
            }
        }
      else // <440
        {
          if (gc >= 390) // 390-439
            {
              if (gc <= 399)
                return DWG_VT_HANDLE;
              if (gc <= 409)
                return DWG_VT_INT16;
              if (gc <= 419)
                return DWG_VT_STRING;
              if (gc <= 429)
                return DWG_VT_INT32;
              if (gc <= 439)
                return DWG_VT_STRING; // lgtm [cpp/constant-comparison]
            }
          else // 330-389
            {
              if (gc <= 309)
                return DWG_VT_STRING;
              if (gc <= 319)
                return DWG_VT_BINARY;
              if (gc <= 329)
                return DWG_VT_HANDLE;
              /* 330-340 SoftPointer, 340-349 HardPointer
                 350-359 SoftOwner, 360-369 HardOwner */
              if (gc <= 369)
                return DWG_VT_OBJECTID;
              if (gc <= 389)
                return DWG_VT_INT16; // lgtm [cpp/constant-comparison]
            }
        }
    }
  else if (gc >= 105)
    {
      if (gc >= 210) // 210-299
        {
          if (gc <= 269)
            return DWG_VT_POINT3D;
          if (gc <= 279)
            return DWG_VT_INT16;
          if (gc <= 289)
            return DWG_VT_INT8;
          if (gc <= 299)
            return DWG_VT_BOOL; // lgtm [cpp/constant-comparison]
        }
      else // 105-209
        {
          if (gc == 105)
            return DWG_VT_HANDLE;
          if (gc <= 109)
            return DWG_VT_INVALID;
          if (gc <= 139)
            return DWG_VT_POINT3D;
          if (gc <= 149)
            return DWG_VT_REAL;
          if (gc <= 169) // e.g. REQUIREDVERSIONS 160 r2013+
            return DWG_VT_INT64;
          if (gc <= 179)
            return DWG_VT_INT16;
          if (gc <= 209)
            return DWG_VT_INVALID; // lgtm [cpp/constant-comparison]
        }
    }
  else // <105
    {
      if (gc >= 38) // 38-102
        {
          if (gc <= 59)
            return DWG_VT_REAL;
          if (gc <= 79)
            return DWG_VT_INT16;
          if (gc <= 99)
            return DWG_VT_INT32;
          if (gc <= 102)
            return DWG_VT_STRING;
        }
      else // 0-37
        {
          if (gc < 0)
            return DWG_VT_HANDLE;
          if (gc <= 4)
            return DWG_VT_STRING;
          if (gc == 5)
            return DWG_VT_HANDLE;
          if (gc <= 9)
            return DWG_VT_STRING; // but 9 never TU
          if (gc <= 37)
            return DWG_VT_POINT3D; // lgtm [cpp/constant-comparison]
        }
    }
  return DWG_VT_INVALID;
}

// See acdb.h: 100th of a mm, enum of
const int lweights[] = { 0,
                         5,
                         9,
                         13,
                         15,
                         18,
                         20,
                         25,
                         30,
                         35,
                         40,
                         50,
                         53,
                         60,
                         70,
                         80,
                         90,
                         100,
                         106,
                         120,
                         /*20:*/ 140,
                         158,
                         200,
                         211,
                         /*illegal/reserved:*/ 0,
                         0,
                         0,
                         0,
                         /*28:*/ 0,  // 0x1c
                         /*29:*/ -1, // 0x1d BYLAYER
                         -2,         // BYBLOCK
                         -3 };       // BYLWDEFAULT

EXPORT int
dxf_cvt_lweight (const BITCODE_BSd value)
{
  return lweights[value % 32];
}

EXPORT BITCODE_BSd
dxf_revcvt_lweight (const int lw)
{
  for (BITCODE_BSd i = 0; i < (BITCODE_BSd)ARRAY_SIZE (lweights); i++)
    if (lweights[i] == lw)
      return i;
  return 0;
}

void
dwg_set_handle_size (Dwg_Handle *restrict hdl)
{
  if (hdl->value)
    {
      int i;
      unsigned char *val;
      memset (&val, 0, sizeof (val));
      val = (unsigned char *)&hdl->value;
#ifdef WORDS_BIGENDIAN
      // the first NULL
      for (i = 0; i < (int)sizeof (hdl->value); i++)
        if (!val[i])
          break;
#else
      // vs. the last non-null
      for (i = sizeof (hdl->value) - 1; i >= 0; i--)
        if (val[i])
          break;
#endif
      hdl->size = i + 1;
      // printf("handle_size %lu => %u\n", hdl->value, hdl->size);
    }
  else
    hdl->size = 0;
}

/** For encode:
 *  May need obj to shorten the code to a relative offset, but not in
 *  header_vars. There obj is NULL.
 */
EXPORT int
dwg_add_handle (Dwg_Handle *restrict hdl, const BITCODE_RC code,
                const BITCODE_RLL absref, const Dwg_Object *restrict obj)
{
  int offset = obj ? (absref - (int)obj->handle.value) : 0;
  hdl->code = code;
  hdl->value = absref;
  if (obj && (code == 0 || !offset) && absref) // only if same obj
    {
      Dwg_Data *dwg = obj->parent;
      uint64_t hashkey;
      assert (dwg);
      loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
      if (!dwg->object_map) // for dwg_add_document()
        dwg->object_map = hash_new (100);
      // check if this handle already exists
      hashkey = hash_get (dwg->object_map, absref);
      if (HASH_NOT_FOUND != hashkey && hashkey != obj->index)
        {
          LOG_ERROR ("Duplicate handle " FORMAT_HV " for object " FORMAT_RL
                     " already points to object %" PRIu64,
                     absref, obj->index, hashkey);
          return 1;
        }
      else if (HASH_NOT_FOUND == hashkey)
        hash_set (dwg->object_map, absref, (uint64_t)obj->index);
      LOG_HANDLE ("object_map{" FORMAT_HV "} = %u\n", absref, obj->index);
    }

  dwg_set_handle_size (hdl);
  if ((code == 4 || code > 5) && obj && absref)
    {
      // change code to 6.0.0 or 8.0.0
      if (offset == 1)
        {
          hdl->code = 6;
          hdl->value = 0;
          hdl->size = 0;
        }
      else if (offset == -1)
        {
          hdl->code = 8;
          hdl->value = 0;
          hdl->size = 0;
        }
      else if (offset > 0)
        {
          hdl->code = 10;
          hdl->value = offset;
          dwg_set_handle_size (hdl);
        }
      else if (offset < 0)
        {
          hdl->code = 12;
          if ((unsigned)offset == 0x80000000) // invalid negation
            {
              LOG_ERROR ("HANDLE 12 overflow for object " FORMAT_RL, obj->index);
              hdl->value = 0;
              return 1;
            }
          else
            hdl->value = -offset;
          dwg_set_handle_size (hdl);
        }
    }
  return 0;
}

// Returns an existing ref with the same ownership (hard/soft, owner/pointer)
// or creates it. May return a freshly allocated ref via dwg_new_ref.
EXPORT Dwg_Object_Ref *
dwg_add_handleref (Dwg_Data *restrict dwg, const BITCODE_RC code,
                   const BITCODE_RLL absref, const Dwg_Object *restrict obj)
{
  Dwg_Object_Ref *ref;
  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  // ENTITY, DICTIONARY, XRECORD or class may need to be relative.
  // prev_entity/next_entity also
  // GROUP needs to be absolute. DICTIONARYVAR absolute.
  // skip the search for existing absolute ref then.
  if (code > 5
      || (code == 4 && obj
          && ((obj->supertype == DWG_SUPERTYPE_ENTITY
               || obj->fixedtype == DWG_TYPE_DICTIONARY
               || obj->fixedtype == DWG_TYPE_XRECORD
               || (obj->fixedtype > DWG_TYPE_GROUP
                   && obj->fixedtype != DWG_TYPE_DICTIONARYVAR)))))
    ;
  else if (dwg->header.from_version > R_12 || absref)
    {
      // search of this code-absref pair already exists
      // lsearch is slow,use bsearch
      /*
      for (BITCODE_BL i = 0; i < dwg->num_object_refs; i++)
        {
          Dwg_Object_Ref *refi = dwg->object_ref[i];
          if (refi->absolute_ref == absref && refi->handleref.code == code)
            {
              LOG_HANDLE ("[existing handleref " FORMAT_REF "][%u] ",
                          ARGS_REF (refi), (unsigned)i)
              return refi;
            }
        }
      */
      Dwg_Object_Ref *refi
          = (Dwg_Object_Ref *)ordered_ref_find (dwg, code, absref);
      if (NULL != refi)
        {
          LOG_HANDLE ("[use handleref " FORMAT_REF "] ", ARGS_REF (refi))
          return refi;
        }
    }
  // else create a new global ref
  ref = dwg_new_ref (dwg);
  ref->handleref.is_global = 1;
  dwg_add_handle (&ref->handleref, code, absref, obj);
  if (dwg->header.from_version <= R_12)
    {
      ref->handleref.size = 2;
      if (dwg->header_vars.HANDSEED && dwg->header_vars.HANDSEED->absolute_ref
          && absref > dwg->header_vars.HANDSEED->absolute_ref)
        {
          dwg->header_vars.HANDSEED->absolute_ref = absref;
        }
    }
  ref->absolute_ref = absref;
  ref->obj = NULL;
  LOG_HANDLE ("[add handleref " FORMAT_REF "] ", ARGS_REF (ref))
  // fill ->obj later
  ordered_ref_add (dwg, ref);
  return ref;
}

/** Return a link to the global ref or a new one. Or a NULLHDL. */
EXPORT Dwg_Object_Ref *
dwg_dup_handleref (Dwg_Data *restrict dwg, const Dwg_Object_Ref *restrict ref)
{
  if (ref)
    return dwg_add_handleref (dwg, ref->handleref.code, ref->absolute_ref,
                              NULL);
  else
    return dwg_add_handleref (dwg, 5, 0, NULL);
}

// Creates a non-global, free'able handle ref.
EXPORT Dwg_Object_Ref *
dwg_add_handleref_free (const BITCODE_RC code, const BITCODE_RLL absref)
{
  Dwg_Object_Ref *ref = (Dwg_Object_Ref *)calloc (1, sizeof (Dwg_Object_Ref));
  dwg_add_handle (&ref->handleref, code, absref, NULL);
  return ref;
}

BITCODE_RLL
dwg_new_handseed (Dwg_Data *restrict dwg)
{
  if (!dwg->header_vars.HANDSEED)
    dwg_init_handseed (dwg);
  dwg->header_vars.HANDSEED->handleref.value++;
  dwg->header_vars.HANDSEED->absolute_ref++;
  return dwg->header_vars.HANDSEED->absolute_ref - 1;
}

// Not checking the header_vars entry, only searching the objects
// Returning a hardowner ref (code 3) to it, as stored in header_vars.
EXPORT BITCODE_H
dwg_find_table_control (Dwg_Data *restrict dwg, const char *restrict table)
{
  BITCODE_BL i;
  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  for (i = 0; i < dwg->num_objects; i++)
    {
      if (dwg->object[i].name && strEQ (dwg->object[i].name, table))
        {
          Dwg_Handle *hdl = &dwg->object[i].handle;
          if (hdl->value)
            return dwg_add_handleref (dwg, 3, hdl->value, NULL);
          else
            {
              // probably importing from a minimal DXF
              LOG_TRACE ("dwg_find_table_control: table control object %s has "
                         "no handle\n",
                         table)
              return NULL;
            }
        }
    }
  // if we haven't read all objects yet, ignore this error
  LOG_TRACE ("dwg_find_table_control: table control object %s not found\n",
             table)
  return NULL;
}

// Searching for a named dictionary entry, name is utf8.
// Returning a hardpointer ref (5) to it, as stored in header_vars.
// Usually another dictionary.
EXPORT BITCODE_H
dwg_find_dictionary (Dwg_Data *restrict dwg, const char *restrict name)
{
  // The NOD (Named Object Dict) must be always the very first DICTIONARY
  Dwg_Object_DICTIONARY *nod;
  Dwg_Object *obj = dwg_get_first_object (dwg, DWG_TYPE_DICTIONARY);
  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  if (!obj || !obj->tio.object || obj->fixedtype != DWG_TYPE_DICTIONARY)
    {
      LOG_ERROR ("dwg_find_dictionary: 1st NOD DICTIONARY not found")
      return NULL;
    }
  nod = obj->tio.object->tio.DICTIONARY;
  for (BITCODE_BL j = 0; j < nod->numitems; j++)
    {
      ATTRIBUTE_ALIGNED (2) BITCODE_T u8;
      if (!nod->texts || !nod->itemhandles)
        continue;
      u8 = nod->texts[j];
      if (!u8)
        continue;
      if (IS_FROM_TU_DWG (dwg))
        u8 = bit_convert_TU ((BITCODE_TU)u8);
      if (u8 && strEQ (u8, name))
        {
          Dwg_Object_Ref *ref = nod->itemhandles[j];
          if (!ref)
            continue;
          // relative? (8.0.0, 6.0.0, ...)
          dwg_resolve_handleref (ref, obj);
          if (IS_FROM_TU_DWG (dwg))
            free (u8);
          return dwg_add_handleref (dwg, 5, ref->absolute_ref, NULL);
        }
      if (IS_FROM_TU_DWG (dwg))
        free (u8);
    }
  LOG_TRACE ("dwg_find_dictionary: DICTIONARY with %s not found\n", name)
  return NULL;
}

// find the named dict entry
EXPORT BITCODE_H
dwg_find_dicthandle (Dwg_Data *restrict dwg, BITCODE_H dict,
                     const char *restrict name)
{
  BITCODE_BL i;
  Dwg_Object_DICTIONARY *_obj;
  Dwg_Object *obj = dwg_resolve_handle (dwg, dict->absolute_ref);

  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  if (!obj || !obj->tio.object)
    {
      LOG_TRACE ("dwg_find_dicthandle: Could not resolve dict " FORMAT_REF
                 "\n",
                 ARGS_REF (dict));
      return NULL;
    }
  if (obj->fixedtype != DWG_TYPE_DICTIONARY)
    {
      LOG_ERROR ("dwg_find_dicthandle: dict not a DICTIONARY\n");
      return NULL;
    }

  _obj = obj->tio.object->tio.DICTIONARY;
  if (!_obj->numitems)
    return NULL;
  for (i = 0; i < _obj->numitems; i++)
    {
      ATTRIBUTE_ALIGNED (2) BITCODE_T *texts = _obj->texts;
      BITCODE_H *hdlv = _obj->itemhandles;

      if (!hdlv || !texts || !texts[i])
        continue;
      if (IS_FROM_TU_DWG (dwg))
        {
          if (bit_eq_TU (name, (BITCODE_TU)texts[i]))
            return hdlv[i];
        }
      else
        {
          if (strEQ (name, texts[i]))
            return hdlv[i];
        }
    }
  return NULL;
}

// find dict entry and match its name
EXPORT BITCODE_H
dwg_find_dicthandle_objname (Dwg_Data *restrict dwg, BITCODE_H dict,
                             const char *restrict name)
{
  BITCODE_BL i;
  Dwg_Object_DICTIONARY *_obj;
  Dwg_Object *obj = dwg_resolve_handle (dwg, dict->absolute_ref);

  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  if (!obj || !obj->tio.object)
    {
      LOG_TRACE ("dwg_find_dicthandle: Could not resolve dict " FORMAT_REF
                 "\n",
                 ARGS_REF (dict));
      return NULL;
    }
  if (obj->fixedtype != DWG_TYPE_DICTIONARY)
    {
      LOG_ERROR ("dwg_find_dicthandle: dict not a DICTIONARY\n");
      return NULL;
    }

  _obj = obj->tio.object->tio.DICTIONARY;
  if (!_obj->numitems)
    return 0;
  for (i = 0; i < _obj->numitems; i++)
    {
      char *hdlname;
      BITCODE_H *hdlv = _obj->itemhandles;
      Dwg_Object *hobj;
      Dwg_Object_APPID *_o; // just some random type
      int isnew = 0;
      bool ok;

      if (!hdlv || !hdlv[i])
        continue;
      hobj = dwg_resolve_handle (dwg, hdlv[i]->absolute_ref);
      if (!hobj || !hobj->tio.object || !hobj->tio.object->tio.APPID
          || !hobj->name)
        continue;
      _o = hobj->tio.object->tio.APPID;
      ok = dwg_dynapi_entity_utf8text (_o, hobj->name, "name", &hdlname,
                                       &isnew, NULL);
      LOG_HANDLE (" %s.%s[%d] => %s.name: %s\n", obj->name, "entries", i,
                  hobj->name, hdlname ? hdlname : "NULL");
      if (ok && hdlname
          && (strEQ (name, hdlname) || !strcasecmp (name, hdlname)))
        {
          if (isnew)
            free (hdlname);
          return hdlv[i];
        }
      if (ok && isnew && hdlname)
        free (hdlname);
    }
  return NULL;
}

/* Return the first object of the given type. */
Dwg_Object *
dwg_find_first_type (const Dwg_Data *restrict dwg,
                     const enum DWG_OBJECT_TYPE type)
{
  for (BITCODE_BL i = 0; i < dwg->num_objects; i++)
    {
      if (dwg->object[i].fixedtype == type)
        return &dwg->object[i];
    }
  return NULL;
}

/* Return the last object of the given type. */
Dwg_Object *
dwg_find_last_type (const Dwg_Data *restrict dwg,
                    const enum DWG_OBJECT_TYPE type)
{
  if (dwg->num_objects)
    {
      for (BITCODE_BL i = dwg->num_objects - 1; i != 0; i--)
        {
          if (dwg->object[i].fixedtype == type)
            return &dwg->object[i];
        }
      if (dwg->object[0].fixedtype == type)
        return &dwg->object[0];
    }
  return NULL;
}

/* Return the handle of the first object of the given type. */
Dwg_Handle *
dwg_find_first_type_handle (const Dwg_Data *restrict dwg,
                            const enum DWG_OBJECT_TYPE type)
{
  for (BITCODE_BL i = 0; i < dwg->num_objects; i++)
    {
      if (dwg->object[i].fixedtype == type)
        return &dwg->object[i].handle;
    }
  return NULL;
}

BITCODE_H
dwg_find_tablehandle_silent (Dwg_Data *restrict dwg, const char *restrict name,
                             const char *restrict table)
{
  BITCODE_H ref;
  int oldopts = dwg->opts;
  dwg->opts &= ~(DWG_OPTS_LOGLEVEL);
  loglevel = 0;

  ref = dwg_find_tablehandle (dwg, name, table);

  dwg->opts = oldopts;
  loglevel = oldopts & DWG_OPTS_LOGLEVEL;
  return ref;
}

// return the matching _CONTROL table, DICTIONARY entry, or NULL
EXPORT BITCODE_H
dwg_ctrl_table (Dwg_Data *restrict dwg, const char *restrict table)
{
  BITCODE_H ctrl = NULL;
  Dwg_Header_Variables *vars = &dwg->header_vars;

  if (!dwg || !table)
    return NULL;
  if (strEQc (table, "BLOCK"))
    {
      if (!(ctrl = vars->BLOCK_CONTROL_OBJECT))
        vars->BLOCK_CONTROL_OBJECT = ctrl
            = dwg_find_table_control (dwg, "BLOCK_CONTROL");
    }
  else if (strEQc (table, "LAYER"))
    {
      if (!(ctrl = vars->LAYER_CONTROL_OBJECT))
        vars->LAYER_CONTROL_OBJECT = ctrl
            = dwg_find_table_control (dwg, "LAYER_CONTROL");
    }
  else if (strEQc (table, "STYLE"))
    {
      if (!(ctrl = vars->STYLE_CONTROL_OBJECT))
        vars->STYLE_CONTROL_OBJECT = ctrl
            = dwg_find_table_control (dwg, "STYLE_CONTROL");
    }
  else if (strEQc (table, "LTYPE"))
    {
      if (!(ctrl = vars->LTYPE_CONTROL_OBJECT))
        vars->LTYPE_CONTROL_OBJECT = ctrl
            = dwg_find_table_control (dwg, "LTYPE_CONTROL");
    }
  else if (strEQc (table, "VIEW"))
    {
      if (!(ctrl = vars->VIEW_CONTROL_OBJECT))
        vars->VIEW_CONTROL_OBJECT = ctrl
            = dwg_find_table_control (dwg, "VIEW_CONTROL");
    }
  else if (strEQc (table, "UCS"))
    {
      if (!(ctrl = vars->UCS_CONTROL_OBJECT))
        vars->UCS_CONTROL_OBJECT = ctrl
            = dwg_find_table_control (dwg, "UCS_CONTROL");
    }
  else if (strEQc (table, "VPORT"))
    {
      if (!(ctrl = vars->VPORT_CONTROL_OBJECT))
        vars->VPORT_CONTROL_OBJECT = ctrl
            = dwg_find_table_control (dwg, "VPORT_CONTROL");
    }
  else if (strEQc (table, "APPID"))
    {
      if (!(ctrl = vars->APPID_CONTROL_OBJECT))
        vars->APPID_CONTROL_OBJECT = ctrl
            = dwg_find_table_control (dwg, "APPID_CONTROL");
    }
  // TODO ACAD_DSTYLE_DIM* are probably different handles
  else if (strEQc (table, "DIMSTYLE") || memBEGINc (table, "ACAD_DSTYLE_DIM"))
    {
      if (!(ctrl = vars->DIMSTYLE_CONTROL_OBJECT))
        vars->DIMSTYLE_CONTROL_OBJECT = ctrl
            = dwg_find_table_control (dwg, "DIMSTYLE_CONTROL");
    }
  else if (strEQc (table, "VX"))
    {
      if (!(ctrl = vars->VX_CONTROL_OBJECT))
        vars->VX_CONTROL_OBJECT = ctrl
            = dwg_find_table_control (dwg, "VX_CONTROL");
    }
  else if (strEQc (table, "GROUP"))
    {
      if (!(ctrl = vars->DICTIONARY_ACAD_GROUP))
        vars->DICTIONARY_ACAD_GROUP = ctrl
            = dwg_find_dictionary (dwg, "ACAD_GROUP");
    }
  else if (strEQc (table, "MLSTYLE") || strEQc (table, "MLINESTYLE"))
    {
      if (!(ctrl = vars->DICTIONARY_ACAD_MLINESTYLE))
        vars->DICTIONARY_ACAD_MLINESTYLE = ctrl
            = dwg_find_dictionary (dwg, "ACAD_MLINESTYLE");
    }
  else if (strEQc (table, "MLEADERSTYLE") || strEQc (table, "ACAD_MLEADERVER"))
    {
      ctrl = dwg_find_dictionary (dwg, "ACAD_MLEADERSTYLE");
    }
  else if (strEQc (table, "NAMED_OBJECT"))
    // The very first DICTIONARY 0.1.C with all the names
    {
      if (!(ctrl = vars->DICTIONARY_NAMED_OBJECT))
        vars->DICTIONARY_NAMED_OBJECT = ctrl
            = dwg_add_handleref (dwg, 3, 0xC, NULL);
    }
  else if (strEQc (table, "LAYOUT"))
    {
      if (!(ctrl = vars->DICTIONARY_LAYOUT))
        vars->DICTIONARY_LAYOUT = ctrl
            = dwg_find_dictionary (dwg, "ACAD_LAYOUT");
    }
  else if (strEQc (table, "PLOTSETTINGS"))
    {
      if (!(ctrl = vars->DICTIONARY_PLOTSETTINGS))
        vars->DICTIONARY_PLOTSETTINGS = ctrl
            = dwg_find_dictionary (dwg, "ACAD_PLOTSETTINGS");
    }
  else if (strEQc (table, "PLOTSTYLENAME"))
    {
      if (!(ctrl = vars->DICTIONARY_PLOTSTYLENAME))
        vars->DICTIONARY_PLOTSTYLENAME = ctrl
            = dwg_find_dictionary (dwg, "ACAD_PLOTSTYLENAME");
    }
  // TODO but maybe the mappers are different
  else if (strEQc (table, "MATERIAL")
           || memBEGINc (table, "ACAD_MATERIAL_MAPPER"))
    {
      if (!(ctrl = vars->DICTIONARY_MATERIAL))
        vars->DICTIONARY_MATERIAL = ctrl
            = dwg_find_dictionary (dwg, "ACAD_MATERIAL");
    }
  else if (strEQc (table, "COLOR"))
    {
      if (!(ctrl = vars->DICTIONARY_COLOR))
        vars->DICTIONARY_COLOR = ctrl
            = dwg_find_dictionary (dwg, "ACAD_COLOR");
    }
  else if (strEQc (table, "VISUALSTYLE"))
    {
      if (!(ctrl = vars->DICTIONARY_VISUALSTYLE))
        vars->DICTIONARY_VISUALSTYLE = ctrl
            = dwg_find_dictionary (dwg, "ACAD_VISUALSTYLE");
    }
  else if (strEQc (table, "LIGHTLIST"))
    {
      if (!(ctrl = vars->DICTIONARY_LIGHTLIST))
        vars->DICTIONARY_LIGHTLIST = ctrl
            = dwg_find_dictionary (dwg, "ACAD_LIGHTLIST");
    }
  else
    {
      LOG_ERROR ("dwg_ctrl_table: Unsupported table %s", table);
      return 0;
    }
  return ctrl;
}

// Search for name in associated table, and return its handle.
// Note that newer tables, like MATERIAL are stored in a DICTIONARY instead.
// Note that we cannot set the ref->obj here, as it may still move by realloc
// dwg->object[]
// See also the silent variant: dwg_find_tablehandle_silent()
EXPORT BITCODE_H
dwg_find_tablehandle (Dwg_Data *restrict dwg, const char *restrict name,
                      const char *restrict table)
{
  BITCODE_BL i, num_entries = 0;
  BITCODE_H ctrl = NULL, *hdlv = NULL;
  Dwg_Object *obj;
  Dwg_Object_APPID_CONTROL *_obj; // just some random generic type
  Dwg_Header_Variables *vars = &dwg->header_vars;

  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  if (!dwg || !name || !table)
    return NULL;
  // look for the _CONTROL table, and search for name in all entries
  ctrl = dwg_ctrl_table (dwg, table);
  if (strEQc (table, "LTYPE"))
    {
      if (strEQc (name, "BYLAYER") || strEQc (name, "ByLayer"))
        {
          if (vars->LTYPE_BYLAYER)
            return vars->LTYPE_BYLAYER;
        }
      else if (strEQc (name, "BYBLOCK") || strEQc (name, "ByBlock"))
        {
          if (vars->LTYPE_BYBLOCK)
            return vars->LTYPE_BYBLOCK;
        }
      else if (strEQc (name, "CONTINUOUS") || strEQc (name, "Continuous"))
        {
          if (vars->LTYPE_CONTINUOUS)
            return vars->LTYPE_CONTINUOUS;
        }
    }
  if (!ctrl)
    { // TODO: silently search table_control. header_vars can be empty
      LOG_TRACE ("dwg_find_tablehandle: Empty header_vars table %s\n", table);
      return NULL;
    }
  obj = dwg_resolve_handle (dwg, ctrl->absolute_ref);
  if (!obj)
    {
      LOG_TRACE ("dwg_find_tablehandle: Could not resolve table %s\n", table);
      return NULL;
    }
  if (obj->fixedtype == DWG_TYPE_DICTIONARY)
    return dwg_find_dicthandle_objname (dwg, ctrl, name);
  if (!dwg_obj_is_control (obj))
    {
      LOG_ERROR ("dwg_find_tablehandle: Could not resolve CONTROL object %s "
                 "for table %s",
                 obj->name, table);
      return NULL;
    }
  _obj = obj->tio.object->tio.APPID_CONTROL; // just random type
  if (strEQc (table, "APPID"))
    {
      num_entries = _obj->num_entries;
      hdlv = _obj->entries;
    }
  else
    {
      dwg_dynapi_entity_value (_obj, obj->name, "num_entries", &num_entries,
                               NULL);
      dwg_dynapi_entity_value (_obj, obj->name, "entries", &hdlv, NULL);
    }
  if (!num_entries)
    return NULL;
  if (!hdlv)
    {
      LOG_ERROR ("No %s.entries but %u num_entries\n", table,
                 (unsigned)num_entries);
      return NULL;
    }
  for (i = 0; i < num_entries; i++)
    {
      char *hdlname;
      Dwg_Object *hobj;
      Dwg_Object_APPID *_o;
      int isnew = 0;
      bool ok;

      if (!hdlv[i])
        continue;
      LOG_INSANE ("%s.entries[%u/%u]: resolve_handle " FORMAT_HV "\n",
                  obj->name, i, num_entries, hdlv[i]->absolute_ref);
      hobj = dwg_resolve_handle (dwg, hdlv[i]->absolute_ref);
      if (!hobj || !hobj->tio.object || !hobj->tio.object->tio.APPID)
        continue;
      _o = hobj->tio.object->tio.APPID;
      ok = dwg_dynapi_entity_utf8text (_o, hobj->name, "name", &hdlname,
                                       &isnew, NULL);
      LOG_HANDLE (" %s.%s[%d] => %s.name: %s\n", obj->name, "entries", i,
                  hobj->name, hdlname ? hdlname : "NULL");
      if (ok && hdlname
          && (strEQ (name, hdlname) || !strcasecmp (name, hdlname)))
        {
          LOG_INSANE ("Found %s\n", name);
          if (isnew)
            free (hdlname);
          return hdlv[i];
        }
      if (ok && isnew && hdlname)
        free (hdlname);
    }

  LOG_INSANE ("Not found in %u APPID entries\n", num_entries);
  return NULL;
}

// Search for the table entry and return its handle.
// Note that newer tables, like MATERIAL are stored in a DICTIONARY instead.
EXPORT BITCODE_H
dwg_find_tablehandle_index (Dwg_Data *restrict dwg, const int index,
                            const char *restrict table)
{
  BITCODE_BL num_entries = 0;
  BITCODE_H ctrl = NULL, *hdlv = NULL;
  Dwg_Object *obj;
  Dwg_Object_APPID_CONTROL *_obj; // just some random generic type
  Dwg_Header_Variables *vars = &dwg->header_vars;

  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  if (!dwg || !table)
    return NULL;
  // look for the _CONTROL table, and search for name in all entries
  ctrl = dwg_ctrl_table (dwg, table);
  if (strEQc (table, "LTYPE"))
    {
      if (index == 32767)
        {
          if (vars->LTYPE_BYLAYER)
            return vars->LTYPE_BYLAYER;
        }
      else if (index == 32766)
        {
          if (vars->LTYPE_BYBLOCK)
            return vars->LTYPE_BYBLOCK;
        }
      else if (index == 0)
        {
          if (vars->LTYPE_CONTINUOUS)
            return vars->LTYPE_CONTINUOUS;
        }
    }
  if (!ctrl)
    { // TODO: silently search table_control. header_vars can be empty
      LOG_TRACE ("dwg_find_tablehandle: Empty header_vars table %s\n", table);
      return NULL;
    }
  obj = dwg_resolve_handle (dwg, ctrl->absolute_ref);
  if (!obj)
    {
      LOG_TRACE ("dwg_find_tablehandle: Could not resolve table %s\n", table);
      return NULL;
    }
  // if (obj->fixedtype == DWG_TYPE_DICTIONARY)
  //  return dwg_find_dicthandle_objname (dwg, ctrl, name);
  if (!dwg_obj_is_control (obj))
    {
      LOG_ERROR (
          "dwg_find_tablehandle_index: Could not resolve CONTROL object %s "
          "for table %s",
          obj->name, table);
      return NULL;
    }
  _obj = obj->tio.object->tio.APPID_CONTROL; // just random type
  if (strEQc (table, "APPID"))
    {
      num_entries = _obj->num_entries;
      hdlv = _obj->entries;
    }
  else
    {
      dwg_dynapi_entity_value (_obj, obj->name, "num_entries", &num_entries,
                               NULL);
      dwg_dynapi_entity_value (_obj, obj->name, "entries", &hdlv, NULL);
    }
  if (!num_entries)
    return NULL;
  if (!hdlv)
    {
      LOG_ERROR ("No %s.entries but %u num_entries\n", table,
                 (unsigned)num_entries);
      return NULL;
    }
  if (index < (int)num_entries)
    {
      if (hdlv[index])
        LOG_INSANE ("%s.entries[%u/%u]: " FORMAT_HV "\n", obj->name, index,
                    num_entries, hdlv[index]->absolute_ref);
      return hdlv[index];
    }
  LOG_INSANE ("Not found in %u %s entries\n", num_entries, table);
  return NULL;
}

// Search for handle in associated table, and return its name. (as UTF-8)
// Always returns a copy.
EXPORT char *
dwg_handle_name (Dwg_Data *restrict dwg, const char *restrict table,
                 const BITCODE_H restrict handle)
{
  BITCODE_BL i, num_entries = 0;
  BITCODE_H ctrl = NULL, *hdlv = NULL;
  Dwg_Object *obj;
  Dwg_Object_APPID_CONTROL *_obj; // just some random generic type
  // Dwg_Header_Variables *vars = &dwg->header_vars;

  if (!dwg || !table || !handle)
    return NULL;
  if (dwg->header.from_version < R_12 && !handle->absolute_ref)
    ;
  else if (!handle->absolute_ref)
    return NULL;
  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  // look for the _CONTROL table, and search for name in all entries
  ctrl = dwg_ctrl_table (dwg, table);
  if (!ctrl)
    { // TODO: silently search table_control. header_vars can be empty
      LOG_TRACE ("dwg_handle_name: Empty header_vars table %s\n", table);
      return 0;
    }
  obj = dwg_resolve_handle (dwg, ctrl->absolute_ref);
  if (!obj)
    {
      LOG_TRACE ("dwg_handle_name: Could not resolve table %s\n", table);
      return 0;
    }
  // if (obj->fixedtype == DWG_TYPE_DICTIONARY)
  //   return dwg_find_dicthandle_objname (dwg, ctrl, name);
  if (!dwg_obj_is_control (obj))
    {
      LOG_ERROR ("dwg_handle_name: Could not resolve CONTROL object %s "
                 "for table %s",
                 obj->name, table);
      return 0;
    }
  _obj = obj->tio.object->tio.APPID_CONTROL; // just a random type
  dwg_dynapi_entity_value (_obj, obj->name, "num_entries", &num_entries, NULL);
  if (!num_entries)
    return NULL;
  dwg_dynapi_entity_value (_obj, obj->name, "entries", &hdlv, NULL);
  if (!hdlv)
    return NULL;
  for (i = 0; i < num_entries; i++)
    {
      char *hdlname, *name;
      Dwg_Object *hobj;
      Dwg_Object_APPID *_o;
      int isnew = 0;
      bool ok;

      if (!hdlv[i])
        continue;
      hobj = dwg_resolve_handle (dwg, hdlv[i]->absolute_ref);
      if (!hobj || !hobj->tio.object || !hobj->tio.object->tio.APPID)
        continue;
      if (dwg->header.from_version > R_12)
        {
          if (hdlv[i]->absolute_ref != handle->absolute_ref)
            continue;
        }
      else
        {
          if (i != (BITCODE_BL)handle->r11_idx)
            continue;
        }
      _o = hobj->tio.object->tio.APPID;
      name = hobj->name;
      /* For BLOCK search the BLOCK entities instead.
         The BLOCK_HEADER has only the abbrevated name, but we want "*D30", not
         "*D" */
      if (strEQc (table, "BLOCK") && hobj->fixedtype == DWG_TYPE_BLOCK_HEADER)
        {
          Dwg_Object_BLOCK_HEADER *_bh = hobj->tio.object->tio.BLOCK_HEADER;
          Dwg_Object *bo = dwg_ref_object (dwg, _bh->block_entity);
          if (bo != NULL && bo->fixedtype == DWG_TYPE_BLOCK)
            {
              _o = (Dwg_Object_APPID *)bo->tio.entity->tio.BLOCK;
              name = bo->name;
            }
        }
      ok = dwg_dynapi_entity_utf8text (_o, name, "name", &hdlname, &isnew,
                                       NULL);
      LOG_HANDLE (" %s.%s[%d] => %s.name: %s\n", obj->name, "entries", i,
                  hobj->name, hdlname ? hdlname : "NULL");
      if (ok)
        {
          if (!isnew && hdlname)
            return strdup (hdlname);
          else
            return hdlname;
        }
      else
        return NULL;
    }
  return NULL;
}

/* Returns the string value of the member of the AcDbVariableDictionary.
   NULL if not found.
   The name is ascii. E.g. LIGHTINGUNITS => "0" */
EXPORT char *
dwg_variable_dict (Dwg_Data *restrict dwg, const char *restrict name)
{
  BITCODE_H var_dict = NULL;
  BITCODE_H var;
  Dwg_Object *obj;
  Dwg_Object_DICTIONARYVAR *_obj;

  var_dict = dwg_find_dictionary (dwg, "AcDbVariableDictionary");
  if (!var_dict)
    return NULL;
  var = dwg_find_dicthandle (dwg, var_dict, name);
  if (!var)
    return NULL;
  obj = dwg_ref_object_silent (dwg, var);
  if (!obj || obj->fixedtype != DWG_TYPE_DICTIONARYVAR)
    return NULL;
  _obj = obj->tio.object->tio.DICTIONARYVAR;
  return _obj->strvalue;
}

static bool
xdata_string_match (Dwg_Data *restrict dwg, Dwg_Resbuf *restrict xdata,
                    int type, char *restrict str)
{
  if (xdata->type != type)
    return 0;
  if (!IS_FROM_TU_DWG (dwg))
    {
      return strEQ (xdata->value.str.u.data, str);
    }
  else
    {
      return memcmp (xdata->value.str.u.wdata, str, xdata->value.str.size * 2)
             == 0;
    }
}

static bool
is_extnames_xrecord (Dwg_Data *restrict dwg, Dwg_Object *restrict xrec,
                     Dwg_Object *restrict xdic)
{
  const int16_t w[8] = { 'E', 'X', 'T', 'N', 'A', 'M', 'E', 'S' };
  const char *extnames
      = dwg->header.from_version < R_2007 ? "EXTNAMES" : (const char *)w;

  return (xrec && xdic && dwg && xrec->fixedtype == DWG_TYPE_XRECORD
          && xrec->tio.object->ownerhandle
          && xrec->tio.object->ownerhandle->absolute_ref == xdic->handle.value
          && xrec->tio.object->tio.XRECORD->num_xdata >= 2
          && xrec->tio.object->tio.XRECORD->xdata
          && xdata_string_match (dwg, xrec->tio.object->tio.XRECORD->xdata,
                                 102, (char *)extnames));
}

// Return a table EXTNAME or NULL. extnames only exist for r13-r14 dwgs
EXPORT char *
dwg_find_table_extname (Dwg_Data *restrict dwg, Dwg_Object *restrict obj)
{
  char *name;
  Dwg_Object *xdic;
  Dwg_Object_DICTIONARY *_xdic;
  Dwg_Object_Ref *xdicref;
  Dwg_Object *xrec = NULL;
  Dwg_Object_XRECORD *_xrec;
  Dwg_Resbuf *xdata;
  BITCODE_BL i;

  // (GH #167) via DICTIONARY ACAD_XREC_ROUNDTRIP to XRECORD EXTNAMES
  if (!dwg_obj_is_table (obj))
    return NULL;
  // HACK: we can guarantee that the table name is always the first field. See
  // dwg_obj_table_get_name().
  name = obj->tio.object->tio.LAYER->name;
  xdicref = obj->tio.object->xdicobjhandle;
  if (!xdicref)
    return NULL;
  xdic = dwg_ref_object (dwg, xdicref);
  if (!xdic || xdic->fixedtype != DWG_TYPE_DICTIONARY)
    return NULL;
  _xdic = xdic->tio.object->tio.DICTIONARY;
  if (_xdic->numitems < 1 || !_xdic->texts[0])
    return NULL;
  if (xdic->tio.object->ownerhandle->absolute_ref != obj->handle.value)
    return NULL;
  for (i = 0; i < _xdic->numitems; i++)
    {
      if (strEQc (_xdic->texts[i], "ACAD_XREC_ROUNDTRIP"))
        break;
    }
  if (i == _xdic->numitems) // not found
    return NULL;
  xrec = dwg_ref_object (dwg, _xdic->itemhandles[i]);
  if (!xrec || !is_extnames_xrecord (dwg, xrec, xdic))
    return NULL;

  _xrec = xrec->tio.object->tio.XRECORD;
  xdata = _xrec->xdata;
  xdata = xdata->nextrb;
  if (xdata->type == 1) // pairs of 1: old name, 2: new name
    {
      // step to the matching name
    cmp:
      if (!xdata_string_match (dwg, xdata, 1, name))
        {
          xdata = xdata->nextrb;
          while (xdata && xdata->type != 1 && xdata->type != 102)
            xdata = xdata->nextrb;
          if (xdata)
            goto cmp;
        }
      if (!xdata)
        return NULL;
      xdata = xdata->nextrb;
      if (xdata->type == 2) // new name
        {
          if (!IS_FROM_TU_DWG (dwg))
            return xdata->value.str.u.data;
          else
            return (char *)xdata->value.str.u.wdata;
        }
    }

  return NULL;
}

static const Dwg_RGB_Palette rgb_palette[256] = {
  { 0x00, 0x00, 0x00 }, // 0
  { 0xFF, 0x00, 0x00 }, { 0xFF, 0xFF, 0x00 }, { 0x00, 0xFF, 0x00 },
  { 0x00, 0xFF, 0xFF }, { 0x00, 0x00, 0xFF }, // 5
  { 0xFF, 0x00, 0xFF }, { 0xFF, 0xFF, 0xFF }, { 0x41, 0x41, 0x41 },
  { 0x80, 0x80, 0x80 }, { 0xFF, 0x00, 0x00 }, // 10
  { 0xFF, 0xAA, 0xAA }, { 0xBD, 0x00, 0x00 }, { 0xBD, 0x7E, 0x7E },
  { 0x81, 0x00, 0x00 }, { 0x81, 0x56, 0x56 }, // 15
  { 0x68, 0x00, 0x00 }, { 0x68, 0x45, 0x45 }, { 0x4F, 0x00, 0x00 },
  { 0x4F, 0x35, 0x35 }, { 0xFF, 0x3F, 0x00 }, // 20
  { 0xFF, 0xBF, 0xAA }, { 0xBD, 0x2E, 0x00 }, { 0xBD, 0x8D, 0x7E },
  { 0x81, 0x1F, 0x00 }, { 0x81, 0x60, 0x56 }, // 25
  { 0x68, 0x19, 0x00 }, { 0x68, 0x4E, 0x45 }, { 0x4F, 0x13, 0x00 },
  { 0x4F, 0x3B, 0x35 }, { 0xFF, 0x7F, 0x00 }, // 30
  { 0xFF, 0xD4, 0xAA }, { 0xBD, 0x5E, 0x00 }, { 0xBD, 0x9D, 0x7E },
  { 0x81, 0x40, 0x00 }, { 0x81, 0x6B, 0x56 }, // 35
  { 0x68, 0x34, 0x00 }, { 0x68, 0x56, 0x45 }, { 0x4F, 0x27, 0x00 },
  { 0x4F, 0x42, 0x35 }, { 0xFF, 0xBF, 0x00 }, // 40
  { 0xFF, 0xEA, 0xAA }, { 0xBD, 0x8D, 0x00 }, { 0xBD, 0xAD, 0x7E },
  { 0x81, 0x60, 0x00 }, { 0x81, 0x76, 0x56 }, // 45
  { 0x68, 0x4E, 0x00 }, { 0x68, 0x5F, 0x45 }, { 0x4F, 0x3B, 0x00 },
  { 0x4F, 0x49, 0x35 }, { 0xFF, 0xFF, 0x00 }, // 50
  { 0xFF, 0xFF, 0xAA }, { 0xBD, 0xBD, 0x00 }, { 0xBD, 0xBD, 0x7E },
  { 0x81, 0x81, 0x00 }, { 0x81, 0x81, 0x56 }, // 55
  { 0x68, 0x68, 0x00 }, { 0x68, 0x68, 0x45 }, { 0x4F, 0x4F, 0x00 },
  { 0x4F, 0x4F, 0x35 }, { 0xBF, 0xFF, 0x00 }, // 60
  { 0xEA, 0xFF, 0xAA }, { 0x8D, 0xBD, 0x00 }, { 0xAD, 0xBD, 0x7E },
  { 0x60, 0x81, 0x00 }, { 0x76, 0x81, 0x56 }, // 65
  { 0x4E, 0x68, 0x00 }, { 0x5F, 0x68, 0x45 }, { 0x3B, 0x4F, 0x00 },
  { 0x49, 0x4F, 0x35 }, { 0x7F, 0xFF, 0x00 }, // 70
  { 0xD4, 0xFF, 0xAA }, { 0x5E, 0xBD, 0x00 }, { 0x9D, 0xBD, 0x7E },
  { 0x40, 0x81, 0x00 }, { 0x6B, 0x81, 0x56 }, // 75
  { 0x34, 0x68, 0x00 }, { 0x56, 0x68, 0x45 }, { 0x27, 0x4F, 0x00 },
  { 0x42, 0x4F, 0x35 }, { 0x3F, 0xFF, 0x00 }, // 80
  { 0xBF, 0xFF, 0xAA }, { 0x2E, 0xBD, 0x00 }, { 0x8D, 0xBD, 0x7E },
  { 0x1F, 0x81, 0x00 }, { 0x60, 0x81, 0x56 }, // 85
  { 0x19, 0x68, 0x00 }, { 0x4E, 0x68, 0x45 }, { 0x13, 0x4F, 0x00 },
  { 0x3B, 0x4F, 0x35 }, { 0x00, 0xFF, 0x00 }, // 90
  { 0xAA, 0xFF, 0xAA }, { 0x00, 0xBD, 0x00 }, { 0x7E, 0xBD, 0x7E },
  { 0x00, 0x81, 0x00 }, { 0x56, 0x81, 0x56 }, // 95
  { 0x00, 0x68, 0x00 }, { 0x45, 0x68, 0x45 }, { 0x00, 0x4F, 0x00 },
  { 0x35, 0x4F, 0x35 }, { 0x00, 0xFF, 0x3F }, // 100
  { 0xAA, 0xFF, 0xBF }, { 0x00, 0xBD, 0x2E }, { 0x7E, 0xBD, 0x8D },
  { 0x00, 0x81, 0x1F }, { 0x56, 0x81, 0x60 }, // 105
  { 0x00, 0x68, 0x19 }, { 0x45, 0x68, 0x4E }, { 0x00, 0x4F, 0x13 },
  { 0x35, 0x4F, 0x3B }, { 0x00, 0xFF, 0x7F }, // 110
  { 0xAA, 0xFF, 0xD4 }, { 0x00, 0xBD, 0x5E }, { 0x7E, 0xBD, 0x9D },
  { 0x00, 0x81, 0x40 }, { 0x56, 0x81, 0x6B }, // 115
  { 0x00, 0x68, 0x34 }, { 0x45, 0x68, 0x56 }, { 0x00, 0x4F, 0x27 },
  { 0x35, 0x4F, 0x42 }, { 0x00, 0xFF, 0xBF }, // 120
  { 0xAA, 0xFF, 0xEA }, { 0x00, 0xBD, 0x8D }, { 0x7E, 0xBD, 0xAD },
  { 0x00, 0x81, 0x60 }, { 0x56, 0x81, 0x76 }, // 125
  { 0x00, 0x68, 0x4E }, { 0x45, 0x68, 0x5F }, { 0x00, 0x4F, 0x3B },
  { 0x35, 0x4F, 0x49 }, { 0x00, 0xFF, 0xFF }, // 130
  { 0xAA, 0xFF, 0xFF }, { 0x00, 0xBD, 0xBD }, { 0x7E, 0xBD, 0xBD },
  { 0x00, 0x81, 0x81 }, { 0x56, 0x81, 0x81 }, // 135
  { 0x00, 0x68, 0x68 }, { 0x45, 0x68, 0x68 }, { 0x00, 0x4F, 0x4F },
  { 0x35, 0x4F, 0x4F }, { 0x00, 0xBF, 0xFF }, // 140
  { 0xAA, 0xEA, 0xFF }, { 0x00, 0x8D, 0xBD }, { 0x7E, 0xAD, 0xBD },
  { 0x00, 0x60, 0x81 }, { 0x56, 0x76, 0x81 }, // 145
  { 0x00, 0x4E, 0x68 }, { 0x45, 0x5F, 0x68 }, { 0x00, 0x3B, 0x4F },
  { 0x35, 0x49, 0x4F }, { 0x00, 0x7F, 0xFF }, // 150
  { 0xAA, 0xD4, 0xFF }, { 0x00, 0x5E, 0xBD }, { 0x7E, 0x9D, 0xBD },
  { 0x00, 0x40, 0x81 }, { 0x56, 0x6B, 0x81 }, // 155
  { 0x00, 0x34, 0x68 }, { 0x45, 0x56, 0x68 }, { 0x00, 0x27, 0x4F },
  { 0x35, 0x42, 0x4F }, { 0x00, 0x3F, 0xFF }, // 160
  { 0xAA, 0xBF, 0xFF }, { 0x00, 0x2E, 0xBD }, { 0x7E, 0x8D, 0xBD },
  { 0x00, 0x1F, 0x81 }, { 0x56, 0x60, 0x81 }, // 165
  { 0x00, 0x19, 0x68 }, { 0x45, 0x4E, 0x68 }, { 0x00, 0x13, 0x4F },
  { 0x35, 0x3B, 0x4F }, { 0x00, 0x00, 0xFF }, // 170
  { 0xAA, 0xAA, 0xFF }, { 0x00, 0x00, 0xBD }, { 0x7E, 0x7E, 0xBD },
  { 0x00, 0x00, 0x81 }, { 0x56, 0x56, 0x81 }, // 175
  { 0x00, 0x00, 0x68 }, { 0x45, 0x45, 0x68 }, { 0x00, 0x00, 0x4F },
  { 0x35, 0x35, 0x4F }, { 0x3F, 0x00, 0xFF }, // 180
  { 0xBF, 0xAA, 0xFF }, { 0x2E, 0x00, 0xBD }, { 0x8D, 0x7E, 0xBD },
  { 0x1F, 0x00, 0x81 }, { 0x60, 0x56, 0x81 }, // 185
  { 0x19, 0x00, 0x68 }, { 0x4E, 0x45, 0x68 }, { 0x13, 0x00, 0x4F },
  { 0x3B, 0x35, 0x4F }, { 0x7F, 0x00, 0xFF }, // 190
  { 0xD4, 0xAA, 0xFF }, { 0x5E, 0x00, 0xBD }, { 0x9D, 0x7E, 0xBD },
  { 0x40, 0x00, 0x81 }, { 0x6B, 0x56, 0x81 }, // 195
  { 0x34, 0x00, 0x68 }, { 0x56, 0x45, 0x68 }, { 0x27, 0x00, 0x4F },
  { 0x42, 0x35, 0x4F }, { 0xBF, 0x00, 0xFF }, // 200
  { 0xEA, 0xAA, 0xFF }, { 0x8D, 0x00, 0xBD }, { 0xAD, 0x7E, 0xBD },
  { 0x60, 0x00, 0x81 }, { 0x76, 0x56, 0x81 }, // 205
  { 0x4E, 0x00, 0x68 }, { 0x5F, 0x45, 0x68 }, { 0x3B, 0x00, 0x4F },
  { 0x49, 0x35, 0x4F }, { 0xFF, 0x00, 0xFF }, // 210
  { 0xFF, 0xAA, 0xFF }, { 0xBD, 0x00, 0xBD }, { 0xBD, 0x7E, 0xBD },
  { 0x81, 0x00, 0x81 }, { 0x81, 0x56, 0x81 }, // 215
  { 0x68, 0x00, 0x68 }, { 0x68, 0x45, 0x68 }, { 0x4F, 0x00, 0x4F },
  { 0x4F, 0x35, 0x4F }, { 0xFF, 0x00, 0xBF }, // 220
  { 0xFF, 0xAA, 0xEA }, { 0xBD, 0x00, 0x8D }, { 0xBD, 0x7E, 0xAD },
  { 0x81, 0x00, 0x60 }, { 0x81, 0x56, 0x76 }, // 225
  { 0x68, 0x00, 0x4E }, { 0x68, 0x45, 0x5F }, { 0x4F, 0x00, 0x3B },
  { 0x4F, 0x35, 0x49 }, { 0xFF, 0x00, 0x7F }, // 230
  { 0xFF, 0xAA, 0xD4 }, { 0xBD, 0x00, 0x5E }, { 0xBD, 0x7E, 0x9D },
  { 0x81, 0x00, 0x40 }, { 0x81, 0x56, 0x6B }, // 235
  { 0x68, 0x00, 0x34 }, { 0x68, 0x45, 0x56 }, { 0x4F, 0x00, 0x27 },
  { 0x4F, 0x35, 0x42 }, { 0xFF, 0x00, 0x3F }, // 240
  { 0xFF, 0xAA, 0xBF }, { 0xBD, 0x00, 0x2E }, { 0xBD, 0x7E, 0x8D },
  { 0x81, 0x00, 0x1F }, { 0x81, 0x56, 0x60 }, // 245
  { 0x68, 0x00, 0x19 }, { 0x68, 0x45, 0x4E }, { 0x4F, 0x00, 0x13 },
  { 0x4F, 0x35, 0x3B }, { 0x33, 0x33, 0x33 }, // 250
  { 0x50, 0x50, 0x50 }, { 0x69, 0x69, 0x69 }, { 0x82, 0x82, 0x82 },
  { 0xBE, 0xBE, 0xBE }, { 0xFF, 0xFF, 0xFF } // 255
};

EXPORT const Dwg_RGB_Palette *
dwg_rgb_palette (void)
{
  return rgb_palette;
}

EXPORT BITCODE_BL
dwg_rgb_palette_index (BITCODE_BS index)
{
  // BS in unsigned
  if (index >= 256)
    return 0;
  else
    {
      Dwg_RGB_Palette rgb;
      assert (index < 256);
      rgb = rgb_palette[index];
      return (rgb.r << 16) | (rgb.g << 8) | rgb.b;
    }
}

EXPORT BITCODE_BS
dwg_find_color_index (BITCODE_BL rgb)
{
  Dwg_RGB_Palette pal;
  rgb &= 0x00ffffff;
  pal.r = (rgb >> 16) & 0xFF;
  pal.g = (rgb >> 8) & 0xFF;
  pal.b = rgb & 0xFF;
  // linear search is good enough for 256. the palette is unsorted.
  for (BITCODE_BS i = 0; i < 256; i++)
    {
      if (memcmp (&pal, &rgb_palette[i], 3) == 0)
        return i;
    }
  return 256;
}

// print errors as string to stderr
EXPORT void
dwg_errstrings (int error)
{
  if (error & 1)
    HANDLER (OUTPUT, "WRONGCRC ");
  if (error & 2)
    HANDLER (OUTPUT, "NOTYETSUPPORTED ");
  if (error & 4)
    HANDLER (OUTPUT, "UNHANDLEDCLASS ");
  if (error & 8)
    HANDLER (OUTPUT, "INVALIDTYPE ");
  if (error & 16)
    HANDLER (OUTPUT, "INVALIDHANDLE ");
  if (error & 32)
    HANDLER (OUTPUT, "INVALIDEED ");
  if (error & 64)
    HANDLER (OUTPUT, "VALUEOUTOFBOUNDS ");
  // -- critical --
  if (error > 127)
    HANDLER (OUTPUT, "\nCritical: ");
  if (error & 128)
    HANDLER (OUTPUT, "CLASSESNOTFOUND ");
  if (error & 256)
    HANDLER (OUTPUT, "SECTIONNOTFOUND ");
  if (error & 512)
    HANDLER (OUTPUT, "PAGENOTFOUND ");
  if (error & 1024)
    HANDLER (OUTPUT, "INTERNALERROR ");
  if (error & 2048)
    HANDLER (OUTPUT, "INVALIDDWG ");
  if (error & 4096)
    HANDLER (OUTPUT, "IOERROR ");
  if (error & 8192)
    HANDLER (OUTPUT, "OUTOFMEM ");
  HANDLER (OUTPUT, "\n");
}

// return name of color.method
EXPORT const char *
dwg_color_method_name (unsigned m)
{
  switch (m)
    {
    case 0xc0:
      return "ByLayer";
    case 0xc1:
      return "ByBlock";
    case 0xc2:
      return "entity (default)";
    case 0xc3:
      return "Truecolor";
    case 0xc5:
      return "Foreground";
    case 0xc8:
      return "none";
    default:
      return "";
    }
}

// not utf-8 converted, native (TV or TU)
const char *
dwg_ref_objname (const Dwg_Data *restrict dwg, Dwg_Object_Ref *restrict ref)
{
  Dwg_Object *restrict obj = dwg_ref_object_silent ((Dwg_Data *)dwg, ref);
  return obj ? obj->name : "";
}

// supports tables entries and everything with a name.
// r2007 names are returned as malloc'ed utf-8
// TV names in the resp. charset.
ATTRIBUTE_MALLOC
const char *
dwg_ref_tblname (const Dwg_Data *restrict dwg, Dwg_Object_Ref *restrict ref,
                 int *alloced)
{
  const char *restrict name = dwg_dynapi_handle_name (dwg, ref, alloced);
  if (!name)
    *alloced = 0;
  return name ? name : "";
}

static Dwg_Object *
search_r11_section (const Dwg_Data *dwg,
                    const Dwg_Entity_Sections jump_section,
                    const BITCODE_RL jump_address)
{
  BITCODE_RL section_addr, abs_addr;
  switch (jump_section)
    {
    case DWG_ENTITY_SECTION:
      section_addr = dwg->header.entities_start;
      break;
    case DWG_BLOCKS_SECTION:
      section_addr = dwg->header.blocks_start;
      break;
    case DWG_EXTRA_SECTION:
      section_addr = dwg->header.extras_start;
      break;
    default:
      LOG_ERROR ("Invalid jump_section %u", jump_section);
      section_addr = 0;
    }
  abs_addr = jump_address + section_addr;
  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  LOG_HANDLE ("resolve JUMP %u %x => addr %x\n", jump_section, jump_address,
              abs_addr);
  for (BITCODE_BL i = 0; i < dwg->num_objects; i++)
    {
      Dwg_Object *obj = &dwg->object[i];
      if (obj->address == abs_addr)
        return obj;
    }
  LOG_WARN ("JUMP %u %x not found", jump_section, jump_address);
  return NULL;
}

EXPORT Dwg_Object *
dwg_resolve_jump (const Dwg_Object *obj)
{
  Dwg_Entity_JUMP *_obj = obj->tio.entity->tio.JUMP;
  if (obj->fixedtype != DWG_TYPE_JUMP)
    return (Dwg_Object *)obj;
  else
    return search_r11_section (obj->parent, _obj->jump_entity_section,
                               _obj->jump_address);
}

EXPORT BITCODE_RLL
dwg_next_handseed (Dwg_Data *dwg)
{
  if (dwg->header_vars.HANDSEED && dwg->header_vars.HANDSEED->absolute_ref)
    {
      BITCODE_RLL seed;
      seed = dwg->header_vars.HANDSEED->absolute_ref + 1;
      dwg->header_vars.HANDSEED->absolute_ref
          = dwg->header_vars.HANDSEED->handleref.value = seed;
      dwg->auxheader.HANDSEED = seed;
      return seed;
    }
  else
    return dwg_next_handle (dwg);
}

EXPORT BITCODE_RLL
dwg_next_handle (const Dwg_Data *dwg)
{
  long j;
  BITCODE_RLL seed = 0;
  // check the objects for the highest handle
  for (j = dwg->num_objects; !seed && j > 0; j--)
    {
      Dwg_Object *o = j > 0 ? &dwg->object[j - 1] : NULL;
      if (o && o->handle.value)
        {
          seed = o->handle.value;
          break;
        }
    }
  // compare against relative handles (deleted and purged?)
  LOG_INSANE ("compute HANDSEED " FORMAT_HV, seed);
  for (BITCODE_BL i = 0; i < dwg->num_object_refs; i++)
    {
      Dwg_Object_Ref *ref = dwg->object_ref[i];
      if (ref && ref->absolute_ref > seed)
        seed = ref->absolute_ref;
    }
  LOG_INSANE (" => " FORMAT_HV "\n", seed + 1);
  return seed + 1;
}

// on init some handles have holes on purpose.
void dwg_set_next_hdl (Dwg_Data *dwg, const BITCODE_RLL value);
void dwg_set_next_objhandle (Dwg_Object *obj);

void
dwg_set_next_hdl (Dwg_Data *dwg, const BITCODE_RLL value)
{
  dwg->next_hdl = value;
}

static void
dwg_init_handseed (Dwg_Data *dwg)
{
  if (!dwg->num_objects)
    {
      dwg->header_vars.HANDSEED = dwg_add_handleref (dwg, 0, 0xC, NULL);
      return;
    }
  else
    {
      Dwg_Object *lastobj = &dwg->object[dwg->num_objects - 1];
      /* ADD_OBJECT might have just added a zeroed object */
      if (!lastobj->handle.value && dwg->num_objects > 1)
        lastobj = &dwg->object[dwg->num_objects - 2];
      dwg->header_vars.HANDSEED
        = dwg_add_handleref (dwg, 0, lastobj->handle.value + 1, NULL);
    }
}

void
dwg_set_next_objhandle (Dwg_Object *obj)
{
  Dwg_Data *dwg = obj->parent;
  BITCODE_RLL seed;
  if (dwg->next_hdl)
    {
      obj->handle.value = dwg->next_hdl;
      dwg_set_handle_size (&obj->handle);
      hash_set (dwg->object_map, obj->handle.value, (uint64_t)obj->index);
      dwg->next_hdl = 0;
      return;
    }
  seed = dwg_new_handseed (dwg);
  if (!dwg->object_map)
    dwg->object_map = hash_new (200);
  else
    {
      bool found = true;
      while (found) {
        uint64_t i = hash_get (dwg->object_map, seed);
        if (i != HASH_NOT_FOUND)
          {
            LOG_WARN ("HANDSEED " FORMAT_HV " already exists: "
                      FORMAT_BLL, seed, i);
            seed = dwg_new_handseed (dwg);
          }
        else
          found = false;
      }
    }
  obj->handle.value = seed;
  dwg_set_handle_size (&obj->handle);
  hash_set (dwg->object_map, obj->handle.value, (uint64_t)obj->index);
}

// <path-to>/dxf.ext => copy of "dxf", "ext"
// returns a malloc'ed copy of basename without extension, and
// sets ext to the char behind the last "." of basename
ATTRIBUTE_MALLOC
char *
split_filepath (const char *filepath, char **extp)
{
  char *copy, *base, *dot;
  // int len;
#ifndef HAVE_NONNULL
  if (!filepath)
    return NULL;
#endif
  copy = strdup (filepath);
#ifdef HAVE_BASENAME
  base = basename (copy);
#else
  base = strrchr (copy, '/');
#endif
  if (!base)
    base = copy;
  // len = strlen (base);
  if ((dot = strrchr (base, '.')) && *dot)
    {
      *extp = dot + 1;
      *dot = '\0';
    }
  return base;
}

int
dwg_sections_init (Dwg_Data *dwg)
{
  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  // num_sections: how many sections we need to allocate internally (with
  // holes)
  if (dwg->header.version < R_13b1)
    {
      if (!dwg->header.num_sections)
        dwg->header.num_sections = 5;
      if (!dwg->header.sections)
        dwg->header.sections = 5;
      // HEADER.sections is always 3 or 5 even if it needs to be 8 or 10,
      // probably because the additional sections are embedded in HEADER_VARS.
      // 5 tables + header + block. VIEW = 6
      if (dwg->header.numheader_vars)
        {
          if (dwg->header.numheader_vars > 129) // r10
            // UCS, VPORT
            dwg->header.num_sections += 2;
          if (dwg->header.numheader_vars > 158) // r10 one
            // APPID
            dwg->header.num_sections += 1;
          if (dwg->header.numheader_vars > 160) // r11
            // DIMSTYLE, VX
            dwg->header.num_sections += 2;
        }
      else
        {
          if (dwg->header.version >= R_10)
            {
              dwg->header.numheader_vars = 160;
              dwg->header.num_sections += 3;
            }
          if (dwg->header.version >= R_11)
            {
              dwg->header.numheader_vars = 205;
              dwg->header.num_sections += 2;
            }
        }
      // and there is one hole 1,2,3,5,6 we need to skip over.
      dwg->header.num_sections += 1;
    }
  else
    {
      /* section 0: header vars
       *         1: class section
       *         2: object map (i.e. handles)
       *         3: optional ObjFreeSpace (r13+, no sentinels) + 2ndheader
       * (r13+, sentinels) 4: optional: Template (MEASUREMENT) 5: optional:
       * AuxHeader (no sentinels, since R_2000b) 6: optional: THUMBNAIL (not a
       * section, but treated as one)
       */
      if (!dwg->header.num_sections
          || (dwg->header.from_version > R_2000
              && dwg->header.version <= R_2000))
        {
          dwg->header.num_sections = dwg->header.version < R_13c3    ? 3
                                     : dwg->header.version < R_2000b ? 5
                                                                     : 6;
          if (dwg->header.num_sections == 3 && dwg->objfreespace.numnums)
            dwg->header.num_sections = 5;
        }
      if (!dwg->header.sections
          || (dwg->header.from_version > R_2000
              && dwg->header.version <= R_2000))
        // ODA writes zeros
        dwg->header.sections = dwg->header.num_sections;
      // newer DWG's have proper HEADER.sections
      if (dwg->header.num_sections != dwg->header.sections)
        dwg->header.num_sections = dwg->header.sections;
    }
  LOG_TRACE ("num_sections => " FORMAT_RL "\n", dwg->header.num_sections);
  if (dwg->header.num_sections < 3)
    {
      LOG_ERROR ("Not enough sections: " FORMAT_RL, dwg->header.num_sections);
      return DWG_ERR_INVALIDDWG;
    }
  else if (dwg->header.num_sections > 28)
    {
      LOG_ERROR ("Too many sections: " FORMAT_RL, dwg->header.num_sections);
      return DWG_ERR_INVALIDDWG;
    }

  if (dwg->header.section)
    // zero-based, including THUMBNAIL
    dwg->header.section = (Dwg_Section *)realloc (
        dwg->header.section,
        sizeof (Dwg_Section) * (dwg->header.num_sections + 2));
  else
    dwg->header.section = (Dwg_Section *)calloc (dwg->header.num_sections + 2,
                                                 sizeof (Dwg_Section));
  if (!dwg->header.section)
    {
      LOG_ERROR ("Out of memory");
      return DWG_ERR_OUTOFMEM;
    }
  return 0;
}

void
dwg_log_proxyflag (const int _loglevel, const int maxlevel,
                   const BITCODE_BS flag)
{
  if (flag > 0 && _loglevel >= maxlevel)
    {
      if (flag & 1)
        HANDLER (OUTPUT, "  erase allowed (1)\n");
      if (flag & 2)
        HANDLER (OUTPUT, "  transform allowed (2)\n");
      if (flag & 4)
        HANDLER (OUTPUT, "  color change allowed (4)\n");
      if (flag & 8)
        HANDLER (OUTPUT, "  layer change allowed (8)\n");
      if (flag & 16)
        HANDLER (OUTPUT, "  LTYPE change allowed (16)\n");
      if (flag & 32)
        HANDLER (OUTPUT, "  LTYPE.scale change allowed (32)\n");
      if (flag & 64)
        HANDLER (OUTPUT, "  visibility change allowed (64)\n");
      if (flag & 128)
        HANDLER (OUTPUT, "  PLOTSTYLE.name change allowed (128)\n");
      if (flag & 256)
        HANDLER (OUTPUT, "  layer change allowed (256)\n");
      if (flag & 512)
        HANDLER (OUTPUT, "  layer change allowed (512)\n");
      if (flag & 1024)
        HANDLER (OUTPUT, "  Disables proxy warning dialog (1024)\n");
      if (flag == 32768)
        HANDLER (OUTPUT, "  is R13 format proxy (32768)\n");
    }
}

void
dwg_log_dataflags (const int _loglevel, const int maxlevel,
                   const BITCODE_RC flag)
{
  if (flag > 0 && _loglevel >= maxlevel)
    {
      if (flag & 1)
        HANDLER (OUTPUT, "  elevation (1)\n");
      if (flag & 2)
        HANDLER (OUTPUT, "  alignment_pt (2)\n");
      if (flag & 4)
        HANDLER (OUTPUT, "  oblique_angle (4)\n");
      if (flag & 8)
        HANDLER (OUTPUT, "  rotation (8)\n");
      if (flag & 16)
        HANDLER (OUTPUT, "  width_factor (16)\n");
      if (flag & 32)
        HANDLER (OUTPUT, "  generation (32)\n");
      if (flag & 64)
        HANDLER (OUTPUT, "  horiz_alignment (64)\n");
      if (flag & 128)
        HANDLER (OUTPUT, "  vert_alignment (128)\n");
    }
}

bool
dwg_has_eed_appid (Dwg_Object_Object *restrict obj, const BITCODE_RLL absref)
{
  if (!obj->num_eed)
    return false;
  for (unsigned i = 0; i < obj->num_eed; i++)
    if (obj->eed[i].handle.value == absref)
      return true;
  return false;
}

// Note that ODA doesn't spec it. ODA's code does take bit 1 from
// path_type instead
// Hook lines appears if the last leader line segment is at an angle
// greater than 15 degrees from horizontal.  If the leader has no
// annotation (annot_type & 0x3) or a spline path (path_type & 0x1),
// it has no hook line.
void
dwg_calc_hookline_on (Dwg_Entity_LEADER *_obj)
{
  const double hookline_offsetR = M_PI / 12;
  double angleR = M_PI / 2;
  if (_obj->num_points > 2)
    {
      BITCODE_3DPOINT pt1 = _obj->points[_obj->num_points - 2];
      BITCODE_3DPOINT pt2 = _obj->points[_obj->num_points - 1];
      angleR = atan2 (pt1.y - pt2.y, pt1.x - pt2.x);
    }
  _obj->hookline_on = (_obj->annot_type & 0x3 || _obj->path_type & 0x1
                       || fabs (angleR) <= hookline_offsetR
                       || fabs (angleR - M_PI) <= hookline_offsetR)
                          ? 0
                          : 1;
}

EXPORT int
dwg_supports_eed (const Dwg_Data *dwg)
{
  return dwg->header.version >= R_11;
}

EXPORT int
dwg_supports_obj (const Dwg_Data *restrict dwg, const Dwg_Object *restrict obj)
{
  const Dwg_Object_Type type = obj->fixedtype;
  Dwg_Version_Type ver;
  if (type == DWG_TYPE_UNUSED || type == DWG_TYPE_UNKNOWN_OBJ
      || type == DWG_TYPE_UNKNOWN_ENT || type == DWG_TYPE_FREED)
    return 0;
  ver = dwg->header.version;
  if (type > DWG_TYPE_GROUP)
    {
      // WIPEOUT causes hang, TABLEGEOMETRY crash on encode
      if (dwg->opts & DWG_OPTS_IN
          && (/*type == DWG_TYPE_WIPEOUT || */ type == DWG_TYPE_TABLEGEOMETRY))
        return 0;
      return ver >= R_13b1 && is_type_stable (type);
    }
  else if (type == DWG_TYPE_GROUP)
    return ver >= R_13b1;
  /*
  else if (type == DWG_TYPE_LWPOLYLINE)
    return ver >= R_13b1;
  else if (type == DWG_TYPE_REGION || type == DWG_TYPE__3DSOLID
           || type == DWG_TYPE_BODY)
    return ver >= R_13b1;
  else if (type == DWG_TYPE_RAY || type == DWG_TYPE_XLINE
           || type == DWG_TYPE_ELLIPSE)
    return ver >= R_13b1;
  else if (type == DWG_TYPE_DICTIONARY || type == DWG_TYPE_OLEFRAME
           || type == DWG_TYPE_MTEXT || type == DWG_TYPE_LEADER
           || type == DWG_TYPE_TOLERANCE || type == DWG_TYPE_MLINE)
    return ver >= R_13b1;
  else if (type == DWG_TYPE_HATCH || type == DWG_TYPE_XRECORD
           || type == DWG_TYPE_PLACEHOLDER || type == DWG_TYPE_VBA_PROJECT
           || type == DWG_TYPE_LAYOUT || type == DWG_TYPE_PROXY_ENTITY
           || type == DWG_TYPE_PROXY_OBJECT)
    return ver >= R_13b1;
  */
  // preR13:
  else if (type == DWG_TYPE_JUMP)
    return ver >= R_2_0b && ver < R_13b1;
  else if (type == DWG_TYPE__3DLINE)
    return ver >= R_2_4 && ver <= R_10;
  else if (type == DWG_TYPE_LOAD)
    return ver < R_2_0b;
  else if (type == DWG_TYPE_ENDREP || type == DWG_TYPE_REPEAT)
    return ver < R_2_10;
  else if (type == DWG_TYPE_ATTRIB || type == DWG_TYPE_ATTDEF)
    return ver >= R_2_0b;
  else if (type == DWG_TYPE_VPORT || type == DWG_TYPE_VPORT_CONTROL
           || type == DWG_TYPE_UCS || type == DWG_TYPE_UCS_CONTROL)
    return ver >= R_10;
  else if (type == DWG_TYPE_APPID || type == DWG_TYPE_APPID_CONTROL
           || type == DWG_TYPE_DIMSTYLE || type == DWG_TYPE_DIMSTYLE_CONTROL)
    return ver >= R_11;
  else if (type == DWG_TYPE_VX_TABLE_RECORD || type == DWG_TYPE_VX_CONTROL)
    return ver >= R_11 && ver < R_2004;
  else if (type == DWG_TYPE__3DFACE)
    return ver >= R_2_0b;
  else if (type == DWG_TYPE_DIMENSION_RADIUS
           || type == DWG_TYPE_DIMENSION_ORDINATE
           || type == DWG_TYPE_DIMENSION_LINEAR
           || type == DWG_TYPE_DIMENSION_ALIGNED
           || type == DWG_TYPE_DIMENSION_ANG3PT
           || type == DWG_TYPE_DIMENSION_ANG2LN
           || type == DWG_TYPE_DIMENSION_RADIUS
           || type == DWG_TYPE_DIMENSION_DIAMETER)
    return ver >= R_2_0b;
  else if (type == DWG_TYPE_POLYLINE_2D || type == DWG_TYPE_POLYLINE_3D
           || type == DWG_TYPE_POLYLINE_MESH || type == DWG_TYPE_POLYLINE_PFACE
           || type == DWG_TYPE_VERTEX_2D || type == DWG_TYPE_VERTEX_3D
           || type == DWG_TYPE_VERTEX_MESH || type == DWG_TYPE_VERTEX_PFACE
           || type == DWG_TYPE_VERTEX_PFACE_FACE || type == DWG_TYPE_SEQEND)
    return ver >= R_2_0b;
  else if (type == DWG_TYPE_MINSERT)
    return ver >= R_2_0b;
  else if (type == DWG_TYPE_SPLINE)
    return ver >= R_9;
  return 1;
}

/* from 2018 to earlier */
void
dwg_downgrade_MLINESTYLE (Dwg_Object_MLINESTYLE *o)
{
  BITCODE_BSd lt_index
      = strEQc (o->name, "CONTINUOUS") || strEQc (o->name, "Continuous")
            ? 0
            : 32767; // or 32766 for BYBLOCK
  for (BITCODE_RC j = 0; j < o->num_lines; j++)
    {
      // TODO lookup lt.ltype
      o->lines[j].lt.index = lt_index;
    }
}

/* to 2018 */
void
dwg_upgrade_MLINESTYLE (Dwg_Data *restrict dwg,
                        Dwg_Object_MLINESTYLE *restrict o)
{
  // lookup on LTYPE_CONTROL list
  for (BITCODE_RC j = 0; j < o->num_lines; j++)
    {
      BITCODE_BSd lt_index = o->lines[j].lt.index;
      LOG_TRACE ("MLINESTYLE.lines[%d].lt.index = %d [BSd 6]\n", j,
                 (int)lt_index);
      if (lt_index == 0)
        o->lines[j].lt.ltype = dwg->header_vars.LTYPE_CONTINUOUS;
      else if (lt_index == 32767)
        o->lines[j].lt.ltype = dwg->header_vars.LTYPE_BYLAYER;
      else if (lt_index == 32766)
        o->lines[j].lt.ltype = dwg->header_vars.LTYPE_BYBLOCK;
      else if (lt_index > 0)
        {
          BITCODE_H hdl
              = dwg_find_tablehandle_index (dwg, (int)lt_index, "LTYPE");
          o->lines[j].lt.ltype
              = dwg_add_handleref (dwg, 5, hdl ? hdl->absolute_ref : 0, NULL);
          if (hdl)
            LOG_TRACE ("MLINESTYLE.lines[%d].lt.ltype %s => " FORMAT_REF
                       " [H]\n",
                       j, o->name, ARGS_REF (hdl));
        }
      else
        o->lines[j].lt.ltype = dwg_add_handleref (dwg, 5, 0, NULL);
    }
}

static const void *
bsearch_ex (const void *pKey, const void *pBase, size_t numBase,
            size_t nItemWidth,
            int (*compare) (const void *pItemL, const void *pItemR),
            const void **ppBefore)
{
  size_t numNow = numBase;
  char *pLo = (char *)pBase;
  char *pHi = (char *)pBase + (numNow - 1) * nItemWidth;
  if (NULL != ppBefore)
    {
      *ppBefore = NULL;
    }
  if (0 == numBase)
    {
      return NULL;
    }
  for (; pLo <= pHi;)
    {
      if (0 == numNow)
        {
          break;
        }
      if (1 == numNow)
        {
          if (0 == compare (pKey, pLo))
            {
              // pKey == pLo
              return pLo;
            }
          break;
        }
      else
        {
          size_t numHalf;
          char *pMid = NULL;
          int result;

          numHalf = numNow / 2;
          pMid = pLo + (numNow & 1 ? numHalf : (numHalf - 1)) * nItemWidth;
          result = compare (pKey, pMid);
          if (0 == result)
            {
              // pKey == pMid
              return pMid;
            }
          if (result < 0)
            {
              // pKey < pMid
              pHi = pMid - nItemWidth;
              numNow = numNow & 1 ? numHalf : numHalf - 1;
            }
          else
            {
              // pKey > pMid
              pLo = pMid + nItemWidth;
              numNow = numHalf;
            }
        }
    }
  if (NULL != ppBefore)
    {
      if (compare (pKey, pLo) > 0)
        {
          // pKey > pLo
          pLo += nItemWidth;
          if (pLo < (char *)pBase + numBase * nItemWidth)
            {
              *ppBefore = pLo;
            }
        }
      else
        {
          // pKey < pLo
          *ppBefore = pLo;
        }
    }
  return NULL;
}

static int
Ref_cmp (const Dwg_Object_Ref *pKey, const Dwg_Object_Ref **ppR)
{
  int retVal = (int)pKey->handleref.code - (int)(*ppR)->handleref.code;
  if (0 != retVal)
    return retVal;
  return pKey->absolute_ref > (*ppR)->absolute_ref    ? 1
         : pKey->absolute_ref == (*ppR)->absolute_ref ? 0
                                                      : 1;
}

void
ordered_ref_add (Dwg_Data *dwg, Dwg_Object_Ref *ref)
{
  Dwg_Object_Ref **pBefore = NULL;
  Dwg_Object_Ref **pFound = NULL;
  if (DWG_HDL_OWNER != ref->handleref.code
      && DWG_HDL_SOFTOWN != ref->handleref.code
      && DWG_HDL_HARDOWN != ref->handleref.code
      && DWG_HDL_SOFTPTR != ref->handleref.code
      && DWG_HDL_HARDPTR != ref->handleref.code)
    {
      return;
    }
  if ((BITCODE_BL)-1 == dwg->num_object_ordered_refs)
    {
      return;
    }
  if (0 == dwg->num_object_ordered_refs)
    {
      dwg->object_ordered_ref = (Dwg_Object_Ref **)calloc (
          REFS_PER_REALLOC, sizeof (Dwg_Object_Ref *));
    }
  else
    {
      if (0 == (dwg->num_object_ordered_refs % REFS_PER_REALLOC))
        {
          Dwg_Object_Ref **pNew = (Dwg_Object_Ref **)realloc (
              dwg->object_ordered_ref,
              sizeof (Dwg_Object_Ref *)
                  * (dwg->num_object_ordered_refs + REFS_PER_REALLOC));
          if (NULL == pNew)
            {
              // realloc failure,back to linear search
              if (NULL != dwg->object_ordered_ref)
                {
                  free (dwg->object_ordered_ref);
                  dwg->object_ordered_ref = NULL;
                }
              dwg->num_object_ordered_refs = (BITCODE_BL)-1;
              return;
            }
          else
            {
              dwg->object_ordered_ref = pNew;
              memset (&dwg->object_ordered_ref[dwg->num_object_ordered_refs], 0,
                      REFS_PER_REALLOC * sizeof (Dwg_Object_Ref *));
            }
        }
    }
  pFound = (Dwg_Object_Ref **)bsearch_ex (
      ref, dwg->object_ordered_ref, dwg->num_object_ordered_refs,
      sizeof (Dwg_Object_Ref *), (int (*) (const void *, const void *))Ref_cmp,
      (const void **)&pBefore);
  if (NULL == pFound)
    {
      // OK, no duplicates
      if (NULL != pBefore)
        {
          // Find the insertion point and insert before it
          size_t numCopy = dwg->num_object_ordered_refs
                           - (pBefore - dwg->object_ordered_ref);
          memmove (&pBefore[1], pBefore, sizeof (Dwg_Object_Ref *) * numCopy);
          *pBefore = ref;
        }
      else
        {
          // The new key is the largest and attached to the last
          dwg->object_ordered_ref[dwg->num_object_ordered_refs] = ref;
        }
      dwg->num_object_ordered_refs++;
    }
  else
    {
      // There are duplicates, disable bsearch
      // It should never happen !
      free (dwg->object_ordered_ref);
      dwg->object_ordered_ref = NULL;
      dwg->num_object_ordered_refs = -1;
    }
}

const Dwg_Object_Ref *
ordered_ref_find (Dwg_Data *dwg, const BITCODE_RC code,
                  const unsigned long absref)
{
  Dwg_Object_Ref nKey;
  if (0 == dwg->num_object_refs)
    {
      return NULL;
    }

  nKey.handleref.code = code;
  nKey.absolute_ref = absref;

  // bsearch first,if possible
  if (0 != dwg->num_object_ordered_refs && NULL != dwg->object_ordered_ref)
    {
      Dwg_Object_Ref **pFound = (Dwg_Object_Ref **)bsearch (
          &nKey, dwg->object_ordered_ref, dwg->num_object_ordered_refs,
          sizeof (Dwg_Object_Ref *),
          (int (*) (const void *, const void *))Ref_cmp);
      if (NULL != pFound)
        {
          return *pFound;
        }
      return NULL;
    }

  // lsearch, if can't bsearch
  for (BITCODE_BL i = 0; i < dwg->num_object_refs; i++)
    {
      const Dwg_Object_Ref *pRef = dwg->object_ref[i];
      if (0 == Ref_cmp (&nKey, &pRef))
        {
          return pRef;
        }
    }

  return NULL;
}
