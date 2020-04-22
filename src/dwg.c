/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2010,2018-2020 Free Software Foundation, Inc.         */
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
#include <sys/stat.h>
#include <assert.h>
// strings.h or string.h
#ifdef AX_STRCASECMP_HEADER
#  include AX_STRCASECMP_HEADER
#endif

#include "bits.h"
#include "common.h"
#include "decode.h"
#include "dwg.h"
#include "hash.h"
#include "dynapi.h"
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
// used in encode.c
void set_handle_size (Dwg_Handle *restrict hdl);

/*------------------------------------------------------------------------------
 * Private functions
 */
int
dat_read_file (Bit_Chain *restrict dat, FILE *restrict fp,
               const char *restrict filename)
{
  size_t size;
  dat->chain = (unsigned char *)calloc (1, dat->size);
  if (!dat->chain)
    {
      LOG_ERROR ("Not enough memory.\n")
      fclose (fp);
      return DWG_ERR_OUTOFMEM;
    }

  size = fread (dat->chain, sizeof (char), dat->size, fp);
  if (size != dat->size)
    {
      LOG_ERROR ("Could not read file (%lu out of %lu): %s\n",
                 (long unsigned int)size, dat->size, filename)
      fclose (fp);
      free (dat->chain);
      dat->chain = NULL;
      dat->size = 0;
      return DWG_ERR_IOERROR;
    }
  return 0;
}

int
dat_read_stream (Bit_Chain *restrict dat, FILE *restrict fp)
{
  size_t size = 0;

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
          return DWG_ERR_OUTOFMEM;
        }
      size = fread (&dat->chain[dat->size], sizeof (char), 4096, fp);
      dat->size += size;
    }
  while (size == 4096);

  if (dat->size == 0)
    {
      LOG_ERROR ("Could not read from stream (%lu out of %lu)\n",
                 (long unsigned int)size, dat->size);
      fclose (fp);
      free (dat->chain);
      dat->chain = NULL;
      return DWG_ERR_IOERROR;
    }

  // clear the slack and realloc
  size = dat->size & 0xfff;
  if (size)
    {
      memset (&dat->chain[dat->size], 0, 0xfff - size);
      dat->chain = (unsigned char *)realloc (dat->chain, dat->size);
    }
  return 0;
}

/*------------------------------------------------------------------------------
 * Public functions
 */

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
  struct stat attrib;
  size_t size;
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
      bit_chain.size = attrib.st_size;
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
  struct stat attrib;
  size_t size;
  Bit_Chain dat = { 0 };

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
  memset (dwg, 0, sizeof (Dwg_Data));
  dwg->opts = loglevel | DWG_OPTS_INDXF;
  memset (&dat, 0, sizeof (Bit_Chain));
  dat.size = attrib.st_size;
  dat.chain = (unsigned char *)calloc (1, dat.size + 2);
  if (!dat.chain)
    {
      LOG_ERROR ("Not enough memory.\n")
      fclose (fp);
      return DWG_ERR_OUTOFMEM;
    }
  dat.byte = 0;
  dat.bit = 0;
  dat.from_version = dwg->header.from_version;
  dat.version = dwg->header.version;
  dat.opts = dwg->opts;

  size = fread (dat.chain, sizeof (char), dat.size, fp);
  if (size != dat.size)
    {
      LOG_ERROR ("Could not read the entire file (%lu out of %lu): %s\n",
                 (long unsigned int)size, dat.size, filename)
      fclose (fp);
      free (dat.chain);
      dat.chain = NULL;
      dat.size = 0;
      return DWG_ERR_IOERROR;
    }
  fclose (fp);
  // properly end the buffer for strtol()/... readers
  dat.chain[size] = '\n';
  dat.chain[size + 1] = '\0';

  /* Fail on DWG */
  if (!memcmp (dat.chain, "AC10", 4))
    {
      LOG_ERROR ("This is a DWG, not a DXF file: %s\n", filename)
      free (dat.chain);
      dat.chain = NULL;
      dat.size = 0;
      return DWG_ERR_INVALIDDWG;
    }
  /* See if ascii or binary */
  if (!memcmp (dat.chain, "AutoCAD Binary DXF",
               sizeof ("AutoCAD Binary DXF") - 1))
    error = dwg_read_dxfb (&dat, dwg);
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
  struct stat attrib;
  Bit_Chain dat = { 0 };
  int error;

  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  assert (filename);
  assert (dwg);
  dat.opts = dwg->opts;
  dat.version = (Dwg_Version_Type)dwg->header.version;
  dat.from_version = (Dwg_Version_Type)dwg->header.from_version;

  // json HACK. no wide chars from JSON, because we just encode to R_2000
  if (dwg->opts & DWG_OPTS_INJSON)
    dat.from_version = dat.version;

  if (dwg->header.version <= R_2000 && dwg->header.from_version > R_2000)
    dwg_fixup_BLOCKS_entities ((Dwg_Data *)dwg);

  dat.size = 0;
  error = dwg_encode ((Dwg_Data *)dwg, &dat);
  if (error >= DWG_ERR_CRITICAL)
    {
      LOG_ERROR ("Failed to encode Dwg_Data\n");
      /* In development we want to look at the corpses */
#ifdef IS_RELEASE
      if (dat.size > 0)
        {
          free (dat.chain);
          dat.chain = NULL;
          dat.size = 0;
        }
      return error;
#endif
    }

  // try opening the output file in write mode
  if (!stat (filename, &attrib)
#ifdef _WIN32
      && strNE (filename, "NUL")
#else
      && strNE (filename, "/dev/null")
#endif
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
   Currently 2 types: BMP and WMF.
 */
EXPORT unsigned char *
dwg_bmp (const Dwg_Data *restrict dwg, BITCODE_RL *restrict size)
{
  BITCODE_RC i, num_pictures, code;
  int found;
  BITCODE_RL header_size, address, osize;
  Bit_Chain dat = { NULL, 0, 0, 0 };

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
  dat.byte = 0;
  dat.bit = 0;

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

  osize = bit_read_RL (&dat); /* overall size of all images */
  LOG_TRACE ("overall size: " FORMAT_RL " [RL]\n", osize);
  if (osize > dat.size)
    {
      LOG_ERROR ("Preview overflow > %lu", dat.size);
      return NULL;
    }
  num_pictures = bit_read_RC (&dat);
  LOG_INFO ("num_pictures: %d [RC]\n", (int)num_pictures)

  found = 0;
  header_size = 0;
  for (i = 0; i < num_pictures; i++)
    {
      if (dat.byte > dat.size)
        {
          LOG_ERROR ("Preview overflow");
          break;
        }
      code = bit_read_RC (&dat);
      LOG_TRACE ("\t[%i] Code: %i [RC]\n", i, code)
      address = bit_read_RL (&dat);
      LOG_TRACE ("\t\tHeader data start: 0x%x [RL]\n", address)
      if (code == 1)
        {
          header_size += bit_read_RL (&dat);
          LOG_TRACE ("\t\tHeader data size: %i [RL]\n", header_size)
        }
      else if (code == 2 && found == 0)
        {
          *size = bit_read_RL (&dat);
          found = 1;
          LOG_INFO ("\t\tBMP size: %i [RL]\n", *size)
        }
      else if (code == 3)
        {
          osize = bit_read_RL (&dat);
          LOG_INFO ("\t\tWMF size: %i [RL]\n", osize)
        }
      else
        {
          osize = bit_read_RL (&dat);
          LOG_TRACE ("\t\tSize of unknown code %i: %i [RL]\n", code, osize)
        }
    }
  dat.byte += header_size;
  if (*size)
    LOG_TRACE ("BMP offset: %lu\n", dat.byte)

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
  assert (dwg);
  return dwg->layer_control.num_entries;
}

EXPORT Dwg_Object_LAYER **
dwg_get_layers (const Dwg_Data *dwg)
{
  unsigned int i;
  unsigned int num_layers = dwg_get_layer_count (dwg);
  Dwg_Object_LAYER **layers;

  assert (dwg);
  layers
      = (Dwg_Object_LAYER **)calloc (num_layers, sizeof (Dwg_Object_LAYER *));
  for (i = 0; i < num_layers; i++)
    layers[i] = dwg->layer_control.entries[i]->obj->tio.object->tio.LAYER;
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
  // TODO: empty means default layer 0
  return ent->layer ? ent->layer->obj->tio.object->tio.LAYER : NULL;
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
dwg_ref_object (const Dwg_Data *restrict dwg, Dwg_Object_Ref *restrict ref)
{
  if (!ref)
    return NULL;
  if (ref->obj && !dwg->dirty_refs)
    return ref->obj;
  // Without obj we don't get an absolute_ref from relative OFFSETOBJHANDLE
  // handle types.
  if ((ref->handleref.code < 6
       && dwg_resolve_handleref (ref, NULL))
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
dwg_ref_object_relative (const Dwg_Data *restrict dwg,
                         Dwg_Object_Ref *restrict ref,
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
 * Find a pointer to an object given it's absolute id (handle).
 * TODO: Check and update each handleref obj cache.
 * Note that absref 0 is illegal here, I think.
 */
EXPORT Dwg_Object *
dwg_resolve_handle (const Dwg_Data *dwg, const unsigned long absref)
{
  uint32_t i;
  if (!absref) // illegal usage
    return NULL;
  i = hash_get (dwg->object_map, (uint32_t)absref);
  if (i != HASH_NOT_FOUND)
    LOG_HANDLE ("object_map{%lX} => %u\n", absref, i);
  if (i == HASH_NOT_FOUND
      || (BITCODE_BL)i >= dwg->num_objects) // the latter being an invalid
                                            // handle (read from DWG)
    {
      // ignore warning on invalid handles. These are warned earlier already
      if (absref && absref < dwg->num_objects)
        {
          LOG_WARN ("Object handle not found, %lu/%lX in " FORMAT_BL
                    " objects",
                    absref, absref, dwg->num_objects);
        }
      return NULL;
    }
  return &dwg->object[i]; // allow value 0
}

/**
 * Silent variant of dwg_resolve_handle
 */
EXPORT Dwg_Object *
dwg_resolve_handle_silent (const Dwg_Data *dwg, const BITCODE_BL absref)
{
  uint32_t i;
  if (!absref) // illegal usage
    return NULL;
  i = hash_get (dwg->object_map, (uint32_t)absref);
  if (i == HASH_NOT_FOUND
      || (BITCODE_BL)i >= dwg->num_objects) // the latter being an invalid
                                            // handle (read from DWG)
    return NULL;
  return &dwg->object[i]; // allow value 0
}

EXPORT Dwg_Object *
dwg_ref_object_silent (const Dwg_Data *restrict dwg,
                       Dwg_Object_Ref *restrict ref)
{
  if (!ref)
    return NULL;
  if (ref->obj && !dwg->dirty_refs)
    return ref->obj;
  if ((ref->handleref.code < 6
       && dwg_resolve_handleref ((Dwg_Object_Ref *)ref, NULL))
      || ref->absolute_ref)
    {
      Dwg_Object *obj = dwg_resolve_handle_silent (dwg, ref->absolute_ref);
      if (!dwg->dirty_refs && obj)
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
      if (!ctrl || !(obj = dwg_ref_object (dwg, ctrl)) || obj->type != DWG_TYPE_BLOCK_CONTROL)
        {
          LOG_ERROR ("dwg.block_control and HEADER.BLOCK_CONTROL_OBJECT missing");
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
  if (dwg->header_vars.BLOCK_RECORD_MSPACE
      && dwg->header_vars.BLOCK_RECORD_MSPACE->obj)
    return dwg->header_vars.BLOCK_RECORD_MSPACE;
  return dwg->block_control.model_space && dwg->block_control.model_space->obj
             ? dwg->block_control.model_space
             : NULL;
}

/** Returns the paper space block object for the DWG.
 */
EXPORT Dwg_Object_Ref *
dwg_paper_space_ref (Dwg_Data *dwg)
{
  if (dwg->header_vars.BLOCK_RECORD_PSPACE
      && dwg->header_vars.BLOCK_RECORD_PSPACE->obj)
    return dwg->header_vars.BLOCK_RECORD_PSPACE;
  return dwg->block_control.paper_space && dwg->block_control.paper_space->obj
             ? dwg->block_control.paper_space
             : NULL;
}

/** Returns the model space block object for the DWG.
 */
EXPORT Dwg_Object *
dwg_model_space_object (Dwg_Data *dwg)
{
  Dwg_Object_Ref *msref = dwg_model_space_ref (dwg);
  Dwg_Object_BLOCK_CONTROL *ctrl;

  if (msref && msref->obj && msref->obj->type == DWG_TYPE_BLOCK_HEADER)
    return msref->obj;
  ctrl = dwg_block_control (dwg);
  if (ctrl && ctrl->model_space && ctrl->model_space->obj)
    return ctrl->model_space->obj;
  if (dwg->header_vars.BLOCK_RECORD_MSPACE
      && dwg->header_vars.BLOCK_RECORD_MSPACE->obj)
    return dwg->header_vars.BLOCK_RECORD_MSPACE->obj;
  return dwg_resolve_handle (dwg, dwg->header.version >= R_2000 ? 0x1F : 0x17);
}

/** Returns the first entity owned by the block hdr, or NULL.
 */
EXPORT Dwg_Object *
get_first_owned_entity (const Dwg_Object *hdr)
{
  unsigned int version = hdr->parent->header.version;
  Dwg_Object_BLOCK_HEADER *_hdr = hdr->tio.object->tio.BLOCK_HEADER;
  if (hdr->type != DWG_TYPE_BLOCK_HEADER)
    {
      LOG_ERROR ("Invalid BLOCK_HEADER type %d", hdr->type);
      return NULL;
    }

  if (R_13 <= version && version <= R_2000)
    {
      /* With r2000 we rather follow the next_entity chain */
      return _hdr->first_entity ? _hdr->first_entity->obj : NULL;
    }
  else if (version >= R_2004)
    {
      _hdr->__iterator = 0;
      if (_hdr->entities && _hdr->num_owned && _hdr->entities[0])
        return _hdr->entities[0]->obj;
      else
        return NULL;
    }

  // TODO: preR13 block table
  LOG_ERROR ("Unsupported version: %d\n", version);
  return NULL;
}

/** Returns the next entity or NULL.
 */
EXPORT Dwg_Object *
dwg_next_entity (const Dwg_Object *restrict obj)
{
  Dwg_Object_Ref *restrict next;

  if (obj->supertype != DWG_SUPERTYPE_ENTITY)
    return NULL;
  if (obj->parent->header.version < R_2004)
    {
      next = obj->tio.entity->next_entity;
      if (next && next->absolute_ref)
        return dwg_ref_object_silent (obj->parent, next);
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
      return (Dwg_Object*)obj;
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
  Dwg_Version_Type version = dwg->header.version;
  Dwg_Object_BLOCK_HEADER *_hdr = hdr->tio.object->tio.BLOCK_HEADER;
  if (hdr->type != DWG_TYPE_BLOCK_HEADER)
    {
      LOG_ERROR ("Invalid BLOCK_HEADER type %d", hdr->type);
      return NULL;
    }

  if (R_13 <= version && version <= R_2000)
    {
      Dwg_Object *obj;
      if (_hdr->last_entity == NULL
          || current->handle.value >= _hdr->last_entity->absolute_ref)
        return NULL;
      obj = dwg_next_entity (current);
      while (obj
             && (obj->supertype != DWG_SUPERTYPE_ENTITY
                 || obj->type == DWG_TYPE_ATTDEF
                 || obj->type == DWG_TYPE_ATTRIB
                 || obj->type == DWG_TYPE_VERTEX_2D
                 || obj->type == DWG_TYPE_VERTEX_3D
                 || obj->type == DWG_TYPE_VERTEX_MESH
                 || obj->type == DWG_TYPE_VERTEX_PFACE
                 || obj->type == DWG_TYPE_VERTEX_PFACE_FACE))
        {
          obj = dwg_next_entity (obj);
          // this may happen with r2000 attribs
          if (obj && obj->supertype == DWG_SUPERTYPE_ENTITY
              && obj->tio.entity != NULL
              && obj->tio.entity->ownerhandle != NULL
              && obj->tio.entity->ownerhandle->absolute_ref
                     != hdr->handle.value)
            obj = NULL;
          if (obj == _hdr->last_entity->obj) // early exit
            return obj;
        }
      return obj;
    }
  else if (version >= R_2004)
    {
      Dwg_Object_Ref *ref;
      _hdr->__iterator++;
      if (_hdr->__iterator == _hdr->num_owned)
        return NULL;
      ref = _hdr->entities ? _hdr->entities[_hdr->__iterator] : NULL;
      return ref ? dwg_ref_object (dwg, ref) : NULL;
    }

  LOG_ERROR ("Unsupported version: %d\n", version);
  return NULL;
}

/** Returns the first subentity owned by the insert or polyline.
 */
EXPORT Dwg_Object *
get_first_owned_subentity (const Dwg_Object *owner)
{
  Dwg_Data *dwg = owner->parent;
  Dwg_Version_Type version = dwg->header.version;
  const unsigned int type = owner->type;
  if (type == DWG_TYPE_INSERT)
    {
      Dwg_Entity_INSERT *_obj = owner->tio.entity->tio.INSERT;
      if (version <= R_2000)
        return _obj->first_attrib ? _obj->first_attrib->obj : NULL;
      else
        return _obj->attrib_handles && _obj->attrib_handles[0]
                   ? dwg_ref_object (dwg, _obj->attrib_handles[0])
                   : NULL;
    }
  else if (type == DWG_TYPE_MINSERT)
    {
      Dwg_Entity_MINSERT *_obj = owner->tio.entity->tio.MINSERT;
      if (version <= R_2000)
        return _obj->first_attrib ? dwg_ref_object (dwg, _obj->first_attrib) : NULL;
      else
        return _obj->attrib_handles && _obj->attrib_handles[0]
                   ? dwg_ref_object (dwg, _obj->attrib_handles[0])
                   : NULL;
    }
  else if (type == DWG_TYPE_POLYLINE_2D || type == DWG_TYPE_POLYLINE_3D
           || type == DWG_TYPE_POLYLINE_PFACE
           || type == DWG_TYPE_POLYLINE_MESH)
    {
      // guaranteed structure
      Dwg_Entity_POLYLINE_2D *_obj = owner->tio.entity->tio.POLYLINE_2D;
      if (version <= R_2000)
        return _obj->first_vertex ? dwg_ref_object (dwg, _obj->first_vertex) : NULL;
      else
        return _obj->vertex && _obj->vertex[0] ? dwg_ref_object (dwg, _obj->vertex[0]) : NULL;
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
  const Dwg_Object_Type type = owner->type;
  Dwg_Object_Entity *ent = owner->tio.entity;
  Dwg_Object *obj = dwg_next_object (current);

  if (type == DWG_TYPE_INSERT)
    {
      Dwg_Entity_INSERT *_obj = owner->tio.entity->tio.INSERT;
      if (version <= R_2000)
        return (_obj->last_attrib && current != _obj->last_attrib->obj
                && obj->type == DWG_TYPE_ATTRIB)
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
            return _obj->attrib_handles
                       ? dwg_ref_object (dwg, _obj->attrib_handles[ent->__iterator])
                       : NULL;
        }
    }
  else if (type == DWG_TYPE_MINSERT)
    {
      Dwg_Entity_MINSERT *_obj = owner->tio.entity->tio.MINSERT;
      if (version <= R_2000)
        return (_obj->last_attrib && current != _obj->last_attrib->obj
                && obj->type == DWG_TYPE_ATTRIB)
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
            return _obj->attrib_handles
                      ? dwg_ref_object (dwg, _obj->attrib_handles[ent->__iterator])
                      : NULL;
        }
    }
  else if (type == DWG_TYPE_POLYLINE_2D || type == DWG_TYPE_POLYLINE_3D
           || type == DWG_TYPE_POLYLINE_PFACE
           || type == DWG_TYPE_POLYLINE_MESH)
    {
      // guaranteed structure
      Dwg_Entity_POLYLINE_2D *_obj = owner->tio.entity->tio.POLYLINE_2D;
      if (version <= R_2000)
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
            return _obj->vertex ? dwg_ref_object (dwg, _obj->vertex[ent->__iterator]) : NULL;
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
  if (hdr->type != DWG_TYPE_BLOCK_HEADER)
    {
      LOG_ERROR ("Invalid BLOCK_HEADER type %d", hdr->type);
      return NULL;
    }

  if (version >= R_13)
    {
      if (_hdr->block_entity)
        return dwg_ref_object (dwg, _hdr->block_entity);
      else
        {
          Dwg_Object *obj = (Dwg_Object *)hdr;
          while (obj && obj->type != DWG_TYPE_BLOCK)
            obj = dwg_next_object (obj);
          return obj;
        }
    }

  // TODO: preR13 block table
  LOG_ERROR ("Unsupported version: %s\n", dwg_version_type (version));
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
  if (hdr->type != DWG_TYPE_BLOCK_HEADER)
    {
      LOG_ERROR ("Invalid BLOCK_HEADER type %d", hdr->type);
      return NULL;
    }

  if (version >= R_13)
    {
      if (!_hdr->endblk_entity || current->handle.value >= _hdr->endblk_entity->absolute_ref)
        return NULL;
      return dwg_next_object (current);
    }

  LOG_ERROR ("Unsupported version: %s\n", dwg_version_type (version));
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

  if (hdr->type != DWG_TYPE_BLOCK_HEADER)
    {
      LOG_ERROR ("Invalid BLOCK_HEADER type %d", hdr->type);
      return NULL;
    }

  dwg = hdr->parent;
  version = dwg->header.version;
  _hdr = hdr->tio.object->tio.BLOCK_HEADER;

  if (R_13 <= version && version <= R_2000)
    {
      /* With r2000 we rather follow the next_entity chain. It may jump around the linked list. */
      if (!_hdr->last_entity
          || current->handle.value == _hdr->last_entity->absolute_ref)
        return NULL;
      return dwg_next_entity (current);
    }
  if (version > R_2000)
    {
      Dwg_Object_Ref *ref;
      _hdr->__iterator++;
      if (_hdr->__iterator == _hdr->num_owned)
        return NULL;
      ref = _hdr->entities ? _hdr->entities[_hdr->__iterator] : NULL;
      return ref ? dwg_ref_object (dwg, ref) : NULL;
    }

  LOG_ERROR ("Unsupported version: %s\n", dwg_version_type (version));
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
  unsigned int version = dwg->header.version;
  if (hdr->type != DWG_TYPE_BLOCK_HEADER)
    {
      LOG_ERROR ("Invalid BLOCK_HEADER type %d", hdr->type);
      return NULL;
    }

  if (version >= R_13)
    {
      if (_hdr->endblk_entity && _hdr->endblk_entity->obj)
        return _hdr->endblk_entity->obj;
      else
        {
          Dwg_Object *obj = (Dwg_Object *)hdr;
          while (obj && obj->type != DWG_TYPE_ENDBLK)
            obj = dwg_next_object (obj);
          if (obj && obj->type == DWG_TYPE_ENDBLK)
            {
              if (!_hdr->endblk_entity)
                {
                  _hdr->endblk_entity = calloc (1, sizeof (Dwg_Object_Ref));
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

  LOG_ERROR ("Unsupported version: %d\n", version);
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
  const unsigned int type = obj->type;
  return (obj->supertype == DWG_SUPERTYPE_OBJECT)
         && (type == DWG_TYPE_BLOCK_CONTROL || type == DWG_TYPE_LAYER_CONTROL
             || type == DWG_TYPE_STYLE_CONTROL
             || type == DWG_TYPE_LTYPE_CONTROL || type == DWG_TYPE_VIEW_CONTROL
             || type == DWG_TYPE_UCS_CONTROL || type == DWG_TYPE_VPORT_CONTROL
             || type == DWG_TYPE_APPID_CONTROL
             || type == DWG_TYPE_DIMSTYLE_CONTROL
             || type == DWG_TYPE_VPORT_ENTITY_CONTROL);
}

EXPORT int
dwg_obj_is_table (const Dwg_Object *obj)
{
  const unsigned int type = obj->type;
  return (obj->supertype == DWG_SUPERTYPE_OBJECT)
         && (type == DWG_TYPE_BLOCK_HEADER || type == DWG_TYPE_LAYER
             || type == DWG_TYPE_STYLE || type == DWG_TYPE_LTYPE
             || type == DWG_TYPE_VIEW || type == DWG_TYPE_UCS
             || type == DWG_TYPE_VPORT || type == DWG_TYPE_APPID
             || type == DWG_TYPE_DIMSTYLE
             || type == DWG_TYPE_VPORT_ENTITY_HEADER);
}

EXPORT int
dwg_obj_is_subentity (const Dwg_Object *obj)
{
  const unsigned int type = obj->type;
  return (obj->supertype == DWG_SUPERTYPE_ENTITY)
         && (type == DWG_TYPE_ATTRIB || type == DWG_TYPE_VERTEX_2D
             || type == DWG_TYPE_VERTEX_3D || type == DWG_TYPE_VERTEX_MESH
             || type == DWG_TYPE_VERTEX_PFACE
             || type == DWG_TYPE_VERTEX_PFACE_FACE);
}

EXPORT int
dwg_obj_has_subentity (const Dwg_Object *obj)
{
  const unsigned int type = obj->type;
  return (obj->supertype == DWG_SUPERTYPE_ENTITY)
         && (type == DWG_TYPE_INSERT || type == DWG_TYPE_MINSERT
             || type == DWG_TYPE_POLYLINE_2D || type == DWG_TYPE_POLYLINE_3D
             || type == DWG_TYPE_POLYLINE_PFACE
             || type == DWG_TYPE_POLYLINE_MESH);
}

EXPORT Dwg_Section_Type
dwg_section_type (const char* restrict name)
{
  if (name == NULL)
    {
      return SECTION_EMPTY;
    }
  else if (strEQc (name, "AcDb:Header"))
    {
      return SECTION_HEADER;
    }
  else if (strEQc (name, "AcDb:Classes"))
    {
      return SECTION_CLASSES;
    }
  else if (strEQc (name, "AcDb:SummaryInfo"))
    {
      return SECTION_SUMMARYINFO;
    }
  else if (strEQc (name, "AcDb:Preview"))
    {
      return SECTION_PREVIEW;
    }
  else if (strEQc (name, "AcDb:VBAProject"))
    {
      return SECTION_VBAPROJECT;
    }
  else if (strEQc (name, "AcDb:AppInfo"))
    {
      return SECTION_APPINFO;
    }
  else if (strEQc (name, "AcDb:FileDepList"))
    {
      return SECTION_FILEDEPLIST;
    }
  else if (strEQc (name, "AcDb:RevHistory"))
    {
      return SECTION_REVHISTORY;
    }
  else if (strEQc (name, "AcDb:Security"))
    {
      return SECTION_SECURITY;
    }
  else if (strEQc (name, "AcDb:AcDbObjects"))
    {
      return SECTION_OBJECTS;
    }
  else if (strEQc (name, "AcDb:ObjFreeSpace"))
    {
      return SECTION_OBJFREESPACE;
    }
  else if (strEQc (name, "AcDb:Template"))
    {
      return SECTION_TEMPLATE;
    }
  else if (strEQc (name, "AcDb:Handles"))
    {
      return SECTION_HANDLES;
    }
  else if (strEQc (name, "AcDb:AcDsPrototype_1b"))
    { // r2013+
      return SECTION_ACDS; // 0xc or 0xd
    }
  else if (strEQc (name, "AcDb:AuxHeader"))
    {
      return SECTION_AUXHEADER;
    }
  else if (strEQc (name, "AcDb:Signature"))
    {
      return SECTION_SIGNATURE;
    }
  else if (strEQc (name, "AcDb:AppInfoHistory"))
    { // AC1021
      return SECTION_APPINFOHISTORY;
    }
  return SECTION_EMPTY;
}

EXPORT Dwg_Section_Type
dwg_section_wtype (const DWGCHAR *restrict wname)
{
  DWGCHAR *wp;
  char name[24];
  uint16_t c;
  int i = 0;

  if (wname == NULL)
    return SECTION_EMPTY;
  wp = (DWGCHAR *)wname;
  while ((c = *wp++))
    {
      name[i++] = (char)(c & 0xff);
    }
  name[i] = '\0';
  return dwg_section_type (name);
}

static const char * const dwg_section_r2004_names[] =
{
  "AcDb:Header",
  "AcDb:AuxHeader",
  "AcDb:Classes",
  "AcDb:Handles",
  "AcDb:Template",
  "AcDb:ObjFreeSpace",
  "AcDb:AcDbObjects",
  "AcDb:RevHistory",
  "AcDb:SummaryInfo",
  "AcDb:Preview",
  "AcDb:AppInfo",
  "AcDb:AppInfoHistory",
  "AcDb:FileDepList",
  "AcDb:Security",
  "AcDb:VBAProject",
  "AcDb:Signature",
  "AcDb:AcDsPrototype_1b",
  "UNKNOWN",
  "SYSTEM_MAP",
  "INFO",
};
static const char * const dwg_section_r13_names[] =
{
  "Header",
  "Classes",
  "Handles",
  "2ndHeader",
  "Template",
  "AuxHeader",
};
static const char * const dwg_section_r11_names[] =
{
  "HEADER",
  "BLOCK",
  "LAYER"
  "STYLE",
  "LTYPE",
  "VIEW",
  "UCS",
  "VPORT",
  "APPID",
  "DIMSTYLE",
  "VPORT_ENTITY",
};

const char *
dwg_section_name (const Dwg_Data *dwg, const unsigned int sec_id)
{
  if (dwg->header.version >= R_2004)
    {
      return (sec_id <= SECTION_EMPTY) ? dwg_section_r2004_names[sec_id] : NULL;
    }
  else if (dwg->header.version > R_11)
    {
      return (sec_id <= SECTION_AUXHEADER_R2000) ? dwg_section_r13_names[sec_id] : NULL;
    }
  else
    {
      return (sec_id <= SECTION_VPORT_ENTITY) ? dwg_section_r11_names[sec_id] : NULL;
    }
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
                         140,
                         158,
                         200,
                         211,
                         /*illegal/reserved:*/ 0,
                         0,
                         0,
                         0,
                         0,
                         /*29:*/ -1, // BYLAYER
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
set_handle_size (Dwg_Handle *restrict hdl)
{
  if (hdl->value)
    {
      int i;
      unsigned char *val;
      memset (&val, 0, sizeof(val));
      val = (unsigned char *)&hdl->value;
      // FIXME: little endian only
      for (i = sizeof(val) - 1; i >= 0; i--)
        if (val[i])
          break;
      hdl->size = i + 1;
    }
  else
     hdl->size = 0;
}

/** For encode:
 * May need obj to shorten the code to a relative offset, but not in
 * header_vars. There obj is NULL.
 */
EXPORT int
dwg_add_handle (Dwg_Handle *restrict hdl, const BITCODE_RC code,
                const unsigned long absref, const Dwg_Object *restrict obj)
{
  int offset = obj ? (absref - (int)obj->handle.value) : 0;
  hdl->code = code;
  hdl->value = absref;
  if (obj && (code == 0 || !offset) && absref) // only if same obj
    {
      Dwg_Data *dwg = obj->parent;
      LOG_HANDLE ("object_map{%lX} = %u\n", absref, obj->index);
      assert (dwg);
      assert (dwg->object_map);
      hash_set (dwg->object_map, absref, (uint32_t)obj->index);
    }

  set_handle_size (hdl);
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
          set_handle_size (hdl);
        }
      else if (offset < 0)
        {
          hdl->code = 12;
          hdl->value = -offset;
          set_handle_size (hdl);
        }
    }
  return 0;
}



// Returns an existing ref with the same ownership (hard/soft, owner/pointer)
// or creates it. May return a freshly allocated ref via dwg_new_ref.
EXPORT Dwg_Object_Ref *
dwg_add_handleref (Dwg_Data *restrict dwg, const BITCODE_RC code,
                   const unsigned long absref, const Dwg_Object *restrict obj)
{
  Dwg_Object_Ref *ref;
  // DICTIONARY, XRECORD or class may need to be relative.
  // TODO: prev_entity/next_entity also
  // skip the search for existing absolute ref then.
  if (code > 5
      || (code == 4 && obj
          && ((obj->fixedtype == DWG_TYPE_DICTIONARY
               || obj->fixedtype == DWG_TYPE_XRECORD
               || obj->type >= DWG_TYPE_GROUP))))
    ;
  else
    {
      // search of this code-absref pair already exists
      for (BITCODE_BL i = 0; i < dwg->num_object_refs; i++)
        {
          Dwg_Object_Ref *refi = dwg->object_ref[i];
          if (refi->absolute_ref == absref && refi->handleref.code == code)
            return refi;
        }
    }
  // else create a new global ref
  ref = dwg_new_ref (dwg);
  dwg_add_handle (&ref->handleref, code, absref, obj);
  ref->absolute_ref = absref;
  // fill ->obj later
  return ref;
}

// Not checking the header_vars entry, only searching the objects
// Returning a hardowner ref (code 3) to it, as stored in header_vars.
EXPORT BITCODE_H
dwg_find_table_control (Dwg_Data *restrict dwg, const char *restrict table)
{
  BITCODE_BL i;
  for (i = 0; i < dwg->num_objects; i++)
    {
      if (strEQ (dwg->object[i].name, table))
        {
          Dwg_Handle *hdl = &dwg->object[i].handle;
          return dwg_add_handleref (dwg, 3, hdl->value, NULL);
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
  // The NOD (Named Object Dict) is always the very first DICTIONARY
  for (BITCODE_BL i = 0; i < dwg->num_objects; i++)
    {
      Dwg_Object *obj = &dwg->object[i];
      // ACAD_GROUP => 1st DICTIONARY: search handle to "ACAD_GROUP"
      if (obj->fixedtype == DWG_TYPE_DICTIONARY)
        {
          Dwg_Object_DICTIONARY *_obj = obj->tio.object->tio.DICTIONARY;
          for (BITCODE_BL j = 0; j < _obj->numitems; j++)
            {
              char *u8;
              if (!_obj->texts || !_obj->itemhandles)
                continue;
              u8 = _obj->texts[j];
              if (!u8)
                continue;
              if (dwg->header.version >= R_2007)
                u8 = bit_convert_TU ((BITCODE_TU)u8);
              if (u8 && strEQ (u8, name))
                {
                  Dwg_Object_Ref *ref = _obj->itemhandles[j];
                  if (!ref)
                    continue;
                  // relative? (8.0.0, 6.0.0, ...)
                  dwg_resolve_handleref (ref, obj);
                  if (dwg->header.version >= R_2007)
                    free (u8);
                  return dwg_add_handleref (dwg, 5, ref->absolute_ref, NULL);
                }
              if (dwg->header.version >= R_2007)
                free (u8);
            }
        }
    }
  LOG_TRACE ("dwg_find_dictionary: DICTIONARY with %s not found\n", name)
  return NULL;
}

EXPORT BITCODE_H
dwg_find_dicthandle (Dwg_Data *restrict dwg, BITCODE_H dict, const char *restrict name)
{
  BITCODE_BL i;
  Dwg_Object_DICTIONARY *_obj;
  Dwg_Object *obj = dwg_resolve_handle (dwg, dict->absolute_ref);

  if (!obj || !obj->tio.object)
    {
      LOG_TRACE ("dwg_find_dicthandle: Could not resolve dict " FORMAT_REF "\n",
                 ARGS_REF(dict));
      return NULL;
    }
  if (obj->type != DWG_TYPE_DICTIONARY)
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
      if (!hobj || !hobj->tio.object || !hobj->tio.object->tio.APPID || !hobj->name)
        continue;
      _o = hobj->tio.object->tio.APPID;
      ok = dwg_dynapi_entity_utf8text (_o, hobj->name, "name", &hdlname, &isnew, NULL);
      LOG_HANDLE (" %s.%s[%d] => %s.name: %s\n", obj->name, "entries", i,
                  hobj->name, hdlname ? hdlname : "NULL");
      if (ok && hdlname && (strEQ (name, hdlname) || !strcasecmp (name, hdlname)))
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

// Search for name in associated table, and return its handle.
// Note that newer tables, like MATERIAL are stored in a DICTIONARY instead.
// Note that we cannot set the ref->obj here, as it may still move by realloc
// dwg->object[]
EXPORT BITCODE_H
dwg_find_tablehandle (Dwg_Data *restrict dwg, const char *restrict name,
                      const char *restrict table)
{
  BITCODE_BL i, num_entries = 0;
  BITCODE_H ctrl = NULL, *hdlv = NULL;
  Dwg_Object *obj;
  Dwg_Object_APPID_CONTROL *_obj; // just some random generic type
  Dwg_Header_Variables *vars = &dwg->header_vars;

  if (!dwg || !name || !table)
    return NULL;
  // look for the _CONTROL table, and search for name in all entries
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
  else if (strEQc (table, "VPORT_ENTITY"))
    {
      if (!(ctrl = vars->VPORT_ENTITY_CONTROL_OBJECT))
        vars->VPORT_ENTITY_CONTROL_OBJECT = ctrl
            = dwg_find_table_control (dwg, "VPORT_ENTITY_CONTROL");
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
      LOG_ERROR ("dwg_find_tablehandle: Unsupported table %s", table);
      return 0;
    }
  if (!ctrl)
    { // TODO: silently search table_control. header_vars can be empty
      LOG_TRACE ("dwg_find_tablehandle: Empty header_vars table %s\n", table);
      return 0;
    }
  obj = dwg_resolve_handle (dwg, ctrl->absolute_ref);
  if (!obj)
    {
      LOG_TRACE ("dwg_find_tablehandle: Could not resolve table %s\n", table);
      return 0;
    }
  if (obj->type == DWG_TYPE_DICTIONARY)
    return dwg_find_dicthandle (dwg, ctrl, name);
  if (!dwg_obj_is_control (obj))
    {
      LOG_ERROR ("dwg_find_tablehandle: Could not resolve CONTROL object %s "
                 "for table %s",
                 obj->name, table);
      return 0;
    }
  _obj = obj->tio.object->tio.APPID_CONTROL; // just random type
  dwg_dynapi_entity_value (_obj, obj->name, "num_entries", &num_entries, NULL);
  if (!num_entries)
    return 0;
  dwg_dynapi_entity_value (_obj, obj->name, "entries", &hdlv, NULL);
  if (!hdlv)
    return 0;
  for (i = 0; i < num_entries; i++)
    {
      char *hdlname;
      Dwg_Object *hobj;
      Dwg_Object_APPID *_o;
      int isnew = 0;
      bool ok;

      if (!hdlv[i])
        continue;
      hobj = dwg_resolve_handle (dwg, hdlv[i]->absolute_ref);
      if (!hobj || !hobj->tio.object || !hobj->tio.object->tio.APPID)
        continue;
      _o = hobj->tio.object->tio.APPID;
      ok = dwg_dynapi_entity_utf8text (_o, hobj->name, "name", &hdlname, &isnew, NULL);
      LOG_HANDLE (" %s.%s[%d] => %s.name: %s\n", obj->name, "entries", i,
                  hobj->name, hdlname ? hdlname : "NULL");
      if (ok && hdlname && (strEQ (name, hdlname) || !strcasecmp (name, hdlname)))
        {
          if (isnew)
            free (hdlname);
          return hdlv[i];
        }
      if (ok && isnew && hdlname)
        free (hdlname);
    }

  return 0;
}

static bool
xdata_string_match (Dwg_Data *restrict dwg, Dwg_Resbuf *restrict xdata,
                    int type, char *restrict str)
{
  if (xdata->type != type)
    return 0;
  if (dwg->header.from_version < R_2007)
    {
      return strEQ (xdata->value.str.u.data, str);
    }
  else
    {
      return memcmp (xdata->value.str.u.wdata, str, xdata->value.str.size * 2) == 0;
    }
}

static bool
is_extnames_xrecord (Dwg_Data *restrict dwg, Dwg_Object *restrict xrec,
                     Dwg_Object *restrict xdic)
{
  const int16_t w[8] = { 'E', 'X', 'T', 'N', 'A', 'M', 'E', 'S' };
  const char *extnames = dwg->header.from_version < R_2007 ? "EXTNAMES" : (const char*)w;

  return (xrec
          && xdic
          && dwg
          && xrec->fixedtype == DWG_TYPE_XRECORD
          && xrec->tio.object->ownerhandle
          && xrec->tio.object->ownerhandle->absolute_ref
               == xdic->handle.value
          && xrec->tio.object->tio.XRECORD->num_xdata >= 2
          && xrec->tio.object->tio.XRECORD->xdata
          && xdata_string_match (dwg, xrec->tio.object->tio.XRECORD->xdata,
                                 102, (char *)extnames));
}

// Return a table EXTNAME or NULL. extnames only exist for r13-r14 dwgs
EXPORT char*
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
  if (!xdic || xdic->type != DWG_TYPE_DICTIONARY)
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
  xdata = xdata->next;
  if (xdata->type == 1) // pairs of 1: old name, 2: new name
    {
      // step to the matching name
    cmp:
      if (!xdata_string_match (dwg, xdata, 1, name))
        {
          xdata = xdata->next;
          while (xdata && xdata->type != 1 && xdata->type != 102)
            xdata = xdata->next;
          if (xdata)
            goto cmp;
        }
      if (!xdata)
        return NULL;
      xdata = xdata->next;
      if (xdata->type == 2) // new name
        {
          if (dwg->header.from_version < R_2007)
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

EXPORT const Dwg_RGB_Palette *dwg_rgb_palette (void)
{
  return rgb_palette;
}

// map [rVER] to our enum number, not the dwg->header.dwgversion
// Acad 2018 offers SaveAs DWG: 2018,2013,2010,2007,2004,2004,2000,r14
//                         DXF: 2018,2013,2010,2007,2004,2004,2000,r12
// libdxfrw dwg2dxf offers R12, v2000, v2004, v2007, v2010
EXPORT Dwg_Version_Type
dwg_version_as (const char *version)
{
  if (strEQc (version, "r2000"))
    return R_2000;
  else if (strEQc (version, "r2004"))
    return R_2004;
  else if (strEQc (version, "r2007"))
    return R_2007;
  else if (strEQc (version, "r2010"))
    return R_2010;
  else if (strEQc (version, "r2013"))
    return R_2013;
  else if (strEQc (version, "r2018"))
    return R_2018;
  else if (strEQc (version, "r14"))
    return R_14;
  else if (strEQc (version, "r13"))
    return R_13;
  else if (strEQc (version, "r11") || strEQc (version, "r12"))
    return R_11;
  else if (strEQc (version, "r10"))
    return R_10;
  else if (strEQc (version, "r9"))
    return R_9;
  else if (strEQc (version, "r2.6"))
    return R_2_6;
  else if (strEQc (version, "r2.5"))
    return R_2_5;
  else if (strEQc (version, "r2.1"))
    return R_2_1;
  else if (strEQc (version, "r2.0"))
    return R_2_0;
  else if (strEQc (version, "r1.4"))
    return R_1_4;
  else if (strEQc (version, "r1.2"))
    return R_1_2;
  else if (strEQc (version, "r1.1"))
    return R_1_1;
  else
    return R_INVALID;
}

/** The reverse of dwg_version_as(char*) */
const char *
dwg_version_type (const Dwg_Version_Type version)
{
  switch (version)
    {
    case R_INVALID:
      return "invalid version";
    case R_1_1:
      return "r1.1";
    case R_1_2:
      return "r1.2";
    case R_1_4:
      return "r1.4";
    case R_2_0:
      return "r2.0";
    case R_2_1:
      return "r2.1";
    case R_2_5:
      return "r2.5";
    case R_2_6:
      return "r2.6";
    case R_9:
      return "r9";
    case R_10:
      return "r10";
    case R_11:
      return "r11";
    case R_13:
      return "r13";
    case R_14:
      return "r14";
    case R_2000:
      return "r2000";
    case R_2004:
      return "r2004";
    case R_2007:
      return "r2007";
    case R_2010:
      return "r2010";
    case R_2013:
      return "r2013";
    case R_2018:
      return "r2018";
    case R_AFTER:
      return "invalid after";
    default:
      return "";
    }
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
