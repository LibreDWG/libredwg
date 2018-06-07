/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009, 2010, 2018 Free Software Foundation, Inc.            */
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

#include "bits.h"
#include "common.h"
#include "decode.h"
#include "dwg.h"
#include "encode.h"
#include "in_dxf.h"
#include "free.h"

/* The logging level per .o */
static unsigned int loglevel;
#ifdef USE_TRACING
/* This flag means we have checked the environment variable
   LIBREDWG_TRACE and set `loglevel' appropriately.  */
static bool env_var_checked_p;
#endif  /* USE_TRACING */
#define DWG_LOGLEVEL loglevel
#include "logging.h"

// used by free.c:
int dwg_obj_is_control(const Dwg_Object *obj);

/*------------------------------------------------------------------------------
 * Public functions
 */

static int dat_read_file (Bit_Chain *restrict dat, FILE *restrict fp,
                          const char *restrict filename)
{
  size_t size;
  dat->chain = (unsigned char *) calloc(1, dat->size);
  if (!dat->chain)
    {
      LOG_ERROR("Not enough memory.\n")
        fclose(fp);
      return -1;
    }

  size = fread(dat->chain, sizeof(char), dat->size, fp);
  if (size != dat->size)
    {
      LOG_ERROR("Could not read file (%lu out of %lu): %s\n",
                (long unsigned int) size, dat->size, filename)
        fclose(fp);
      free(dat->chain);
      dat->chain = NULL;
      dat->size = 0;
      return -1;
    }
  return 0;
}

static int dat_read_stream (Bit_Chain *restrict dat, FILE *restrict fp)
{
  size_t size = 0;

  do {
    if (dat->chain)
      dat->chain = (unsigned char *) realloc(dat->chain, dat->size + 4096);
    else {
      dat->chain = (unsigned char *) calloc(1, 4096);
      dat->size = 0;
    }
    if (!dat->chain)
      {
        LOG_ERROR("Not enough memory.\n");
        fclose(fp);
        return -1;
      }
    size = fread(&dat->chain[dat->size], sizeof(char), 4096, fp);
    dat->size += size;
  } while (size == 4096);

  if (dat->size == 0)
    {
      LOG_ERROR("Could not read from stream (%lu out of %lu)\n",
                (long unsigned int)size, dat->size);
      fclose(fp);
      free(dat->chain);
      dat->chain = NULL;
      return -1;
    }

  // clear the slack and realloc
  size = dat->size & 0xfff;
  if (size)
    {
      memset(&dat->chain[dat->size], 0, 0xfff - size);
      dat->chain = (unsigned char *) realloc(dat->chain, dat->size);
    }
  return 0;
}

/** dwg_read_file
 * returns 0 on success.
 *
 * everything in dwg is cleared
 * and then either read from dat, or set to a default.
 */
int
dwg_read_file(const char *restrict filename, Dwg_Data *restrict dwg)
{
  FILE *fp;
  struct stat attrib;
  size_t size;
  Bit_Chain bit_chain;

  loglevel = dwg->opts;
  memset(dwg, 0, sizeof(Dwg_Data));
  dwg->opts = loglevel;

  if (!strcmp(filename, "-"))
    {
      fp = stdin;
    }
  else
    {
      if (stat(filename, &attrib))
        {
          LOG_ERROR("File not found: %s\n", filename);
          return -1;
        }
      if (!(S_ISREG (attrib.st_mode)
#ifndef _WIN32
            || S_ISLNK (attrib.st_mode)
#endif
            ))
        {
          LOG_ERROR("Error: %s\n", filename);
          return -1;
        }
      fp = fopen(filename, "rb");
    }
  if (!fp)
    {
      LOG_ERROR("Could not open file: %s\n", filename)
      return -1;
    }

  /* Load whole file into memory, even if streamed (for now)
   */
  memset(&bit_chain, 0, sizeof(Bit_Chain));
  if (fp == stdin)
    {
      int error;
      error = dat_read_stream(&bit_chain, fp);
      if (error)
        return error;
    }
  else
    {
      int error;
      bit_chain.size = attrib.st_size;
      error = dat_read_file(&bit_chain, fp, filename);
      if (error)
        return error;
    }
  fclose(fp);

  /* Decode the dwg structure */
  if (dwg_decode(&bit_chain, dwg))
    {
      LOG_ERROR("Failed to decode file: %s\n", filename)
      free(bit_chain.chain);
      bit_chain.chain = NULL;
      bit_chain.size = 0;
      return -1;
    }

  //TODO: does dwg hold any char* pointers to the bit_chain or are they all copied?
  free(bit_chain.chain);
  bit_chain.chain = NULL;
  bit_chain.size = 0;

  return 0;
}

/* if write support is enabled */
#ifdef USE_WRITE

/** dxf_read_file
 * returns 0 on success.
 *
 * detects binary or ascii file.
 * everything in dwg is cleared
 * and then either read from dat, or set to a default.
 */
int
dxf_read_file(const char *restrict filename, Dwg_Data *restrict dwg)
{
  int error;
  FILE *fp;
  struct stat attrib;
  size_t size;
  Bit_Chain dat;

  if (stat(filename, &attrib))
    {
      LOG_ERROR("File not found: %s\n", filename)
      return -1;
    }
  if (!(S_ISREG (attrib.st_mode)
#ifndef _WIN32
        || S_ISLNK (attrib.st_mode)
#endif
        ))
    {
      LOG_ERROR("Error: %s\n", filename)
      return -1;
    }
  fp = fopen(filename, "r");
  if (!fp)
    {
      LOG_ERROR("Could not open file: %s\n", filename)
      return -1;
    }

  /* Load whole file into memory
   */
  loglevel = dwg->opts;
  memset(dwg, 0, sizeof(Dwg_Data));
  dwg->opts = loglevel;
  memset(&dat, 0, sizeof(Bit_Chain));
  dat.size = attrib.st_size;
  dat.chain = (unsigned char *) calloc(1, dat.size);
  if (!dat.chain)
    {
      LOG_ERROR("Not enough memory.\n")
      fclose(fp);
      return -1;
    }
  dat.byte = 0;
  dat.bit = 0;
  dat.from_version = dwg->header.from_version;
  dat.version = dwg->header.version;

  size = fread(dat.chain, sizeof(char), dat.size, fp);
  if (size != dat.size)
    {
      LOG_ERROR("Could not read the entire file (%lu out of %lu): %s\n",
          (long unsigned int) size, dat.size, filename)
      fclose(fp);
      free(dat.chain);
      dat.chain = NULL;
      dat.size = 0;
      return -1;
    }
  fclose(fp);

  /* Fail on DWG */
  if (!memcmp(dat.chain, "AC10", 4))
    {
      LOG_ERROR("This is a DWG, not a DXF file: %s\n", filename)
      free(dat.chain);
      dat.chain = NULL;
      dat.size = 0;
      return -1;
    }
  /* See if ascii or binary */
  if (!memcmp(dat.chain, "AutoCAD Binary DXF", sizeof("AutoCAD Binary DXF")-1))
    error = dwg_read_dxfb(&dat, dwg);
  else
    error = dwg_read_dxf(&dat, dwg);
  
  if (error)
    {
      LOG_ERROR("Failed to decode DXF file: %s\n", filename)
      free(dat.chain);
      dat.chain = NULL;
      dat.size = 0;
      return -1;
    }

  //TODO: does dwg hold any char* pointers to the dat or are they all copied?
  free(dat.chain);
  dat.chain = NULL;
  dat.size = 0;

  return 0;
}

int
dwg_write_file(const char *restrict filename, const Dwg_Data *restrict dwg)
{
  FILE *fh;
  struct stat attrib;
  Bit_Chain dat;

  assert(filename);
  assert(dwg);
  dat.version = (Dwg_Version_Type)dwg->header.version;
  dat.from_version = (Dwg_Version_Type)dwg->header.from_version;

  // Encode the DWG struct
  dat.size = 0;
  if (dwg_encode ((Dwg_Data *)dwg, &dat))
    {
      LOG_ERROR("Failed to encode datastructure.\n")
      if (dat.size > 0) {
        free (dat.chain);
        dat.chain = NULL;
        dat.size = 0;
      }
      return -1;
    }
 
  // try opening the output file in write mode
  if (!stat (filename, &attrib))
    {
      LOG_ERROR("The file already exists. We won't overwrite it.")
      return -1;
    }
  fh = fopen (filename, "wb");
  if (!fh)
    {
      LOG_ERROR("Failed to create the file: %s\n", filename)
      return -1;
    }

  // Write the data into the file
  if (fwrite (dat.chain, sizeof (char), dat.size, fh) != dat.size)
    {
      LOG_ERROR("Failed to write data into the file: %s\n", filename)
      fclose (fh);
      free (dat.chain);
      dat.chain = NULL;
      dat.size = 0;
      return -1;
    }
  fclose (fh);

  if (dat.size > 0) {
    free (dat.chain);
    dat.chain = NULL;
    dat.size = 0;
  }

  return 0;
}
#endif /* USE_WRITE */ 

/* THUMBNAIL IMAGE DATA (R13C3+) */
unsigned char *
dwg_bmp(const Dwg_Data *restrict dwg, BITCODE_RL *restrict size)
{
  BITCODE_RC i, num_pictures, code;
  int plene;
  BITCODE_RL header_size, address, osize;
  Bit_Chain *dat;

  *size = 0;
  assert(dwg);
  dat = (Bit_Chain*) &dwg->picture;
  if (!dat || !dat->size)
    {
      LOG_INFO("no THUMBNAIL Image Data\n")
      return NULL;
    }
  dat->bit = 0;
  dat->byte = 0;

#ifdef USE_TRACING
  /* Before starting, set the logging level, but only do so once.  */
  if (! env_var_checked_p)
    {
      char *probe = getenv ("LIBREDWG_TRACE");
      if (probe)
        loglevel = atoi (probe);
      env_var_checked_p = true;
    }
#endif  /* USE_TRACING */
  
  osize = bit_read_RL(dat); /* overall size of all images */
  LOG_TRACE("overall size: " FORMAT_RL "\n", osize)
  num_pictures = bit_read_RC(dat);
  LOG_INFO("num_pictures: " FORMAT_RC "\n", num_pictures)

  plene = 0;
  header_size = 0;
  for (i = 0; i < num_pictures; i++)
    {
      code = bit_read_RC(dat);
      LOG_TRACE("\t[%i] Code: %i\n", i, code)
      address = bit_read_RL(dat);
      LOG_TRACE("\t\tHeader data start: 0x%x\n", address)
      if (code == 1)
        {
          header_size += bit_read_RL(dat);
          LOG_TRACE("\t\tHeader data size: %i\n", header_size)
        }
      else if (code == 2 && plene == 0)
        {
          *size = bit_read_RL(dat);
          plene = 1;
          LOG_TRACE("\t\tBMP size: %i\n", *size)
        }
      else if (code == 3)
        {
          osize = bit_read_RL(dat);
          LOG_TRACE("\t\tWMF size: 0x%x\n", osize)
        }
      else
        {
          osize = bit_read_RL(dat);
          LOG_TRACE("\t\tSize of unknown code %d: 0x%x\n", code, osize)
        }
    }
  dat->byte += header_size;
  LOG_TRACE("Current adress: 0x%lx\n", dat->byte)

  if (*size > 0)
    return (dat->chain + dat->byte);
  else
    return NULL;
}

double
dwg_model_x_min(const Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.EXTMIN.x;
}

double
dwg_model_x_max(const Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.EXTMAX.x;
}

double
dwg_model_y_min(const Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.EXTMIN.y;
}

double
dwg_model_y_max(const Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.EXTMAX.y;
}

double
dwg_model_z_min(const Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.EXTMIN.z;
}

double
dwg_model_z_max(const Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.EXTMAX.z;
}

double
dwg_page_x_min(const Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.EXTMIN.x;
}

double
dwg_page_x_max(const Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.PEXTMAX.x;
}

double
dwg_page_y_min(const Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.PEXTMIN.y;
}

double
dwg_page_y_max(const Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.PEXTMAX.y;
}

unsigned int
dwg_get_layer_count(const Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->layer_control.num_entries;
}

Dwg_Object_LAYER **
dwg_get_layers(const Dwg_Data *dwg)
{
  unsigned int i;
  unsigned int num_layers = dwg_get_layer_count(dwg);
  Dwg_Object_LAYER ** layers;

  assert(dwg);
  layers = (Dwg_Object_LAYER **) calloc(num_layers,
                                        sizeof (Dwg_Object_LAYER*));
  for (i=0; i < num_layers; i++)
    layers[i] = dwg->layer_control.layers[i]->obj->tio.object->tio.LAYER;
  return layers;
}

long unsigned int
dwg_get_object_num_objects(const Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->num_objects - dwg->num_entities;
}

long unsigned int
dwg_get_num_objects(const Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->num_objects;
}

long unsigned int
dwg_get_num_entities(const Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->num_entities;
}

/** Returns a copy of all entities */
Dwg_Object_Entity **
dwg_get_entities(const Dwg_Data *dwg)
{
  long unsigned int i, ent_count = 0;
  Dwg_Object_Entity ** entities;

  assert(dwg);
  entities = (Dwg_Object_Entity **) calloc(dwg_get_num_entities(dwg),
                                           sizeof (Dwg_Object_Entity*));
  for (i=0; i < dwg->num_objects; i++)
    {
      if (dwg->object[i].supertype == DWG_SUPERTYPE_ENTITY)
        {
          entities[ent_count] = dwg->object[i].tio.entity;
          ent_count++;
          assert(ent_count < dwg->num_objects);
        }
    }
  return entities;
}

Dwg_Object_LAYER *
dwg_get_entity_layer(const Dwg_Object_Entity * ent)
{
  //TODO: empty means default layer 0
  return ent->layer ? ent->layer->obj->tio.object->tio.LAYER : NULL;
}

Dwg_Object*
dwg_next_object(const Dwg_Object* obj)
{
  const Dwg_Data *dwg = obj->parent;
  if ((obj->index+1) > (dwg->num_objects-1))
    return NULL;
  return &dwg->object[obj->index+1];
}

/**
 * Find an object given its handle
 */
Dwg_Object*
dwg_ref_get_object(const Dwg_Data *restrict dwg, Dwg_Object_Ref *restrict ref)
{
  if (ref->obj && !dwg->dirty_refs)
    return ref->obj;
  // Without obj we don't get an absolute_ref from relative OFFSETOBJHANDLE handle types.
  if (ref->handleref.code < 6 &&
      dwg_resolve_handleref((Dwg_Object_Ref*)ref, NULL))
    {
      ref->obj = dwg_resolve_handle(dwg, ref->absolute_ref);
      return ref->obj;
    }
  else
    return NULL;
}

/**
 * Find an object given its handle and relative base object.
 * OFFSETOBJHANDLE, handleref.code > 6.
 */
Dwg_Object*
dwg_ref_get_object_relative(const Dwg_Data *restrict dwg,
                            Dwg_Object_Ref *restrict ref,
                            const Dwg_Object *restrict obj)
{
  if (ref->obj && !dwg->dirty_refs)
    return ref->obj;
  if (dwg_resolve_handleref((Dwg_Object_Ref*)ref, obj))
    {
      ref->obj = dwg_resolve_handle(dwg, ref->absolute_ref);
      return ref->obj;
    }
  else
    return NULL;
}

/**
 * Find a pointer to an object given it's absolute id (handle).
 * TODO: Check and update each handleref obj cache.
 */
Dwg_Object *
dwg_resolve_handle(const Dwg_Data * dwg, const long unsigned int absref)
{
  // TODO hash table or sorted
  // This is linear search, absref's are currently unsorted. encode sorts them.
  long unsigned int i;
  for (i = 0; i < dwg->num_objects; i++)
    {
      if (dwg->object[i].handle.value == absref)
        return &dwg->object[i];
    }
  if (absref)
    {
      LOG_WARN("Object not found: %lu in %ld objects", absref, dwg->num_objects);
    }
  return NULL;
}

/* set ref->absolute_ref from obj, for a subsequent dwg_resolve_handle() */
int
dwg_resolve_handleref(Dwg_Object_Ref *restrict ref, const Dwg_Object *restrict obj)
{
  /*
   * With TYPEDOBJHANDLE 2-5 the code indicates the type of ownership.
   * With OFFSETOBJHANDLE >5 the handle is stored as an offset from some other handle.
   */
 switch (ref->handleref.code)
    {
    case 0x06:
      ref->absolute_ref = (obj->handle.value + 1);
      break;
    case 0x08:
      ref->absolute_ref = (obj->handle.value - 1);
      break;
    case 0x0A:
      ref->absolute_ref = (obj->handle.value + ref->handleref.value);
      break;
    case 0x0C:
      ref->absolute_ref = (obj->handle.value - ref->handleref.value);
      break;
    case 2: case 3: case 4: case 5:
      ref->absolute_ref = ref->handleref.value;
      break;
    case 0: // ignore?
      ref->absolute_ref = ref->handleref.value;
      break;
    default:
      ref->absolute_ref = ref->handleref.value;
      LOG_WARN("Invalid handle pointer code %d", ref->handleref.code);
      return 0;
    }
  return 1;
}

Dwg_Object*
get_first_owned_object(const Dwg_Object* hdr_obj, Dwg_Object_BLOCK_HEADER* hdr)
{
  unsigned int version = hdr_obj->parent->header.version;

  if (R_13 <= version && version <= R_2000)
    {
      return hdr->first_entity->obj;
    }

  if (version >= R_2004)
    {
      hdr->__iterator = 0;
      if (hdr->entities && hdr->num_owned && hdr->entities[0])
        return hdr->entities[0]->obj;
      else
        return NULL;
    }
  //TODO: preR13 block table
  LOG_ERROR("Unsupported version: %d\n", version);
  return NULL;
}

Dwg_Object*
get_next_owned_object(const Dwg_Object* hdr_obj, const Dwg_Object* current,
                      Dwg_Object_BLOCK_HEADER* hdr)
{
  unsigned int version = hdr_obj->parent->header.version;

  if (R_13 <= version && version <= R_2000)
    {
      if (current == hdr->last_entity->obj) return 0;
      return dwg_next_object(current);
    }

  if (version >= R_2004)
    {
      hdr->__iterator++;
      if (hdr->__iterator == hdr->num_owned) return 0;
      return hdr->entities[hdr->__iterator]->obj;
    }

  LOG_ERROR("Unsupported version: %d\n", version);
  return NULL;
}

int
dwg_class_is_entity(const Dwg_Class *klass)
{
  return klass && klass->item_class_id == 0x1f2;
}

int
dwg_obj_is_control(const Dwg_Object *obj)
{
  unsigned int type = obj->type;
  return type == DWG_TYPE_BLOCK_CONTROL ||
         type == DWG_TYPE_LAYER_CONTROL ||
         type == DWG_TYPE_STYLE_CONTROL ||
         type == DWG_TYPE_LTYPE_CONTROL ||
         type == DWG_TYPE_VIEW_CONTROL ||
         type == DWG_TYPE_UCS_CONTROL ||
         type == DWG_TYPE_VPORT_CONTROL ||
         type == DWG_TYPE_APPID_CONTROL ||
         type == DWG_TYPE_DIMSTYLE_CONTROL ||
         type == DWG_TYPE_VPORT_ENTITY_CONTROL;
}

int
dwg_obj_is_table(const Dwg_Object *obj)
{
  unsigned int type = obj->type;
  return type == DWG_TYPE_BLOCK_HEADER ||
         type == DWG_TYPE_LAYER ||
         type == DWG_TYPE_STYLE ||
         type == DWG_TYPE_LTYPE ||
         type == DWG_TYPE_VIEW ||
         type == DWG_TYPE_UCS ||
         type == DWG_TYPE_VPORT ||
         type == DWG_TYPE_APPID ||
         type == DWG_TYPE_DIMSTYLE ||
         type == DWG_TYPE_VPORT_ENTITY_HEADER;
}

Dwg_Section_Type
dwg_section_type(const DWGCHAR *wname)
{
  DWGCHAR *wp;
  char name[24];
  uint16_t c;
  int i = 0;

  wp = (DWGCHAR *)wname;
  while ((c = *wp++)) {
    name[i++] = (char)(c & 0xff);
  }
  name[i] = '\0';

  if (!strcmp(name, "AcDb:Header")) {
    return SECTION_HEADER;
  } else if (!strcmp(name, "AcDb:Classes")) {
    return SECTION_CLASSES;
  } else if (!strcmp(name, "AcDb:SummaryInfo")) {
    return SECTION_SUMMARYINFO;
  } else if (!strcmp(name, "AcDb:Preview")) {
    return SECTION_PREVIEW;
  } else if (!strcmp(name, "AcDb:VBAProject")) {
    return SECTION_VBAPROJECT;
  } else if (!strcmp(name, "AcDb:AppInfo")) {
    return SECTION_APPINFO;
  } else if (!strcmp(name, "AcDb:FileDepList")) {
    return SECTION_FILEDEPLIST;
  } else if (!strcmp(name, "AcDb:RevHistory")) {
    return SECTION_REVHISTORY;
  } else if (!strcmp(name, "AcDb:Security")) {
    return SECTION_SECURITY;
  } else if (!strcmp(name, "AcDb:AcDbObjects")) {
    return SECTION_OBJECTS;
  } else if (!strcmp(name, "AcDb:ObjFreeSpace")) {
    return SECTION_OBJFREESPACE;
  } else if (!strcmp(name, "AcDb:Template")) {
    return SECTION_TEMPLATE;
  } else if (!strcmp(name, "AcDb:Handles")) {
    return SECTION_HANDLES;
  } else if (!strcmp(name, "AcDb:AcDsPrototype_1b")) {
    return SECTION_PROTOTYPE;
  } else if (!strcmp(name, "AcDb:AuxHeader")) {
    return SECTION_AUXHEADER;
  } else if (!strcmp(name, "AcDb:Signature")) {
    return SECTION_SIGNATURE;
  } else if (!strcmp(name, "AcDb:AppInfoHistory")) { // AC1021
    return SECTION_APPINFOHISTORY;
  }
  return SECTION_UNKNOWN;
}
