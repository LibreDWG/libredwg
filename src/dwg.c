/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009, 2010 Free Software Foundation, Inc.                  */
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
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>

#include "bits.h"
#include "common.h"
#include "decode.h"
#include "dwg.h"
#include "encode.h"
#include "free.h"

#include "logging.h"

/*------------------------------------------------------------------------------
 * Public functions
 */

/** dwg_read_file
 * returns 0 on success.
 *
 * everything in dwg is cleared
 * and then either read from dat, or set to a default.
 */
int
dwg_read_file(char *filename, Dwg_Data * dwg_data)
{
  FILE *fp;
  struct stat attrib;
  size_t size;
  Bit_Chain bit_chain;

  if (stat(filename, &attrib))
    {
      LOG_ERROR("File not found: %s\n", filename)
      return -1;
    }
  if (!S_ISREG (attrib.st_mode))
    {
      LOG_ERROR("Error: %s\n", filename)
      return -1;
    }
  fp = fopen(filename, "rb");
  if (!fp)
    {
      LOG_ERROR("Could not open file: %s\n", filename)
      return -1;
    }

  /* Load whole file into memory
   */
  bit_chain.bit = 0;
  bit_chain.byte = 0;
  bit_chain.size = attrib.st_size;
  bit_chain.chain = (unsigned char *) calloc(1, bit_chain.size);
  if (!bit_chain.chain)
    {
      LOG_ERROR("Not enough memory.\n")
      fclose(fp);
      return -1;
    }

  size = fread(bit_chain.chain, sizeof(char), bit_chain.size, fp);
  if (size != bit_chain.size)
    {
      LOG_ERROR("Could not read the entire file (%lu out of %lu): %s\n",
          (long unsigned int) size, bit_chain.size, filename)
      fclose(fp);
      free(bit_chain.chain);
      bit_chain.chain = NULL;
      bit_chain.size = 0;
      return -1;
    }
  fclose(fp);

  /* Decode the dwg structure
   */
  if (dwg_decode_data(&bit_chain, dwg_data))
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

int
dwg_write_file(char *filename, Dwg_Data * dwg_data)
{
  FILE *dt;
  struct stat atrib;
  Bit_Chain bit_chain;

  assert(filename);
  assert(dwg_data);
  bit_chain.version = (Dwg_Version_Type)dwg_data->header.version;
  bit_chain.from_version = (Dwg_Version_Type)dwg_data->header.from_version;

  // Encode the DWG struct
  bit_chain.size = 0;
  if (dwg_encode_chains (dwg_data, &bit_chain))
    {
      LOG_ERROR("Failed to encode datastructure.\n")
      if (bit_chain.size > 0) {
        free (bit_chain.chain);
        bit_chain.chain = NULL;
        bit_chain.size = 0;
      }
      return -1;
    }
 
  // try opening the output file in write mode
  if (!stat (filename, &atrib))
    {
      LOG_ERROR("The file already exists. We won't overwrite it.")
      return -1;
    }
  dt = fopen (filename, "w");
  if (!dt)
    {
      LOG_ERROR("Failed to create the file: %s\n", filename)
      return -1;
    }
   

  // Write the data into the file
  if (fwrite (bit_chain.chain, sizeof (char), bit_chain.size, dt) != bit_chain.size)
    {
      LOG_ERROR("Failed to write data into the file: %s\n", filename)
      fclose (dt);
      free (bit_chain.chain);
      bit_chain.chain = NULL;
      bit_chain.size = 0;
      return -1;
    }
  fclose (dt);

  if (bit_chain.size > 0) {
    free (bit_chain.chain);
    bit_chain.chain = NULL;
    bit_chain.size = 0;
  }

  return 0;
}
#endif /* USE_WRITE */ 

/* IMAGE DATA (R13C3+) */
unsigned char *
dwg_bmp(Dwg_Data *dwg, BITCODE_RL *size)
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
      LOG_TRACE("no IMAGE DATA\n")
      return NULL;
    }
  dat->bit = 0;
  dat->byte = 0;

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
dwg_model_x_min(Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.EXTMIN.x;
}

double
dwg_model_x_max(Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.EXTMAX.x;
}

double
dwg_model_y_min(Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.EXTMIN.y;
}

double
dwg_model_y_max(Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.EXTMAX.y;
}

double
dwg_model_z_min(Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.EXTMIN.z;
}

double
dwg_model_z_max(Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.EXTMAX.z;
}

double
dwg_page_x_min(Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.EXTMIN.x;
}

double
dwg_page_x_max(Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.PEXTMAX.x;
}

double
dwg_page_y_min(Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.PEXTMIN.y;
}

double
dwg_page_y_max(Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->header_vars.PEXTMAX.y;
}

unsigned int
dwg_get_layer_count(Dwg_Data *dwg)
{
  assert(dwg);
  assert(dwg->layer_control->tio.object);
  return dwg->layer_control->tio.object->tio.LAYER_CONTROL->num_entries;
}

Dwg_Object_LAYER **
dwg_get_layers(Dwg_Data *dwg)
{
  unsigned int i;
  Dwg_Object_LAYER ** layers;
  
  assert(dwg);
  layers = (Dwg_Object_LAYER **) calloc(dwg_get_layer_count(dwg),
                                        sizeof (Dwg_Object_LAYER*));
  for (i=0; i < dwg_get_layer_count(dwg); i++)
    {
      layers[i] = dwg->layer_control->tio.object->tio.LAYER_CONTROL->
            layers[i]->obj->tio.object->tio.LAYER;
    }
  return layers;
}

long unsigned int
dwg_get_object_count(Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->num_objects;
}

long unsigned int
dwg_get_object_object_count(Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->num_objects - dwg->num_entities;
}

long unsigned int
dwg_get_entity_count(Dwg_Data *dwg)
{
  assert(dwg);
  return dwg->num_entities;
}

Dwg_Object_Entity **
dwg_get_entities(Dwg_Data *dwg)
{
  long unsigned int i, ent_count = 0;
  Dwg_Object_Entity ** entities;

  assert(dwg);
  entities = (Dwg_Object_Entity **) calloc(dwg_get_entity_count(dwg),
                                           sizeof (Dwg_Object_Entity*));
  for (i=0; i < dwg->num_objects; i++)
    {
      if (dwg->object[i].supertype == DWG_SUPERTYPE_ENTITY)
        {
          entities[ent_count] = dwg->object[i].tio.entity;
          ent_count++;
        }
    }
  return entities;
}

Dwg_Object_LAYER *
dwg_get_entity_layer(Dwg_Object_Entity * ent)
{
  return ent->layer->obj->tio.object->tio.LAYER;
}

Dwg_Object*
dwg_next_object(Dwg_Object* obj)
{
  if ((obj->index+1) > (obj->parent->num_objects-1))
    return NULL;
  return &obj->parent->object[obj->index+1];
}

Dwg_Object*
dwg_ref_get_object(Dwg_Object_Ref* ref)
{
  return ref->obj ? ref->obj : NULL;
}

Dwg_Object*
get_first_owned_object(Dwg_Object* hdr_obj, Dwg_Object_BLOCK_HEADER* hdr)
{
  unsigned int version = hdr_obj->parent->header.version;

  if (R_13 <= version && version <= R_2000)
    {
      return hdr->first_entity->obj;
    }

  if (version >= R_2004)
    {
      hdr->__iterator = 0;
      return hdr->entities[0]->obj;
    }

  LOG_ERROR("Unsupported version: %d\n", version);
  return NULL;
}

Dwg_Object*
get_next_owned_object(Dwg_Object* hdr_obj, Dwg_Object* current,
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
      if (hdr->__iterator == hdr->owned_object_count) return 0;
      return hdr->entities[hdr->__iterator]->obj;
    }

  LOG_ERROR("Unsupported version: %d\n", version);
  return NULL;
}

int
dwg_class_is_entity(Dwg_Class *klass)
{
  return klass->item_class_id == 0x1f2;
}
