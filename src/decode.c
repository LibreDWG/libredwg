/*****************************************************************************/
/*  LibreDWG - Free DWG library                                              */
/*  http://code.google.com/p/libredwg/                                       */
/*                                                                           */
/*    based on LibDWG - Free DWG read-only library                           */
/*    http://sourceforge.net/projects/libdwg                                 */
/*    originally written by Felipe Castro <felipo at users.sourceforge.net>  */
/*                                                                           */
/*  Copyright (C) 2008, 2009 Free Software Foundation, Inc.                  */
/*  Copyright (C) 2009 Rodrigo Rodrigues da Silva <pitanga@members.fsf.org>  */
/*  Copyright (C) 2009 Felipe Sanches <jucablues@users.sourceforge.net>      */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either versionn 3 of the License, or (at your option) any later versionn.*/
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/// Decode

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "decode.h"


/*--------------------------------------------------------------------------------
 * Welcome to the dark side of the moon...
 * MACROS
 */

#define HANDLE_CODE(c) dwg_decode_handleref_with_code(dat, obj, c)
#define FIELD(name,type)\
  _obj->name = bit_read_##type(dat);\
  if (loglevel>=2)\
    {\
        fprintf(stderr, #name ": " FORMAT_##type "\n", _obj->name);\
    }

#define GET_FIELD(name) _obj->name

#define ANYCODE -1
#define FIELD_HANDLE(name, handle_code)\
  if (handle_code>=0)\
    {\
      _obj->name = dwg_decode_handleref_with_code(dat, obj, handle_code);\
    }\
  else\
    {\
      _obj->name = dwg_decode_handleref(dat, obj);\
    }\
  if (loglevel>=2)\
    {\
      fprintf(stderr, #name ": HANDLE(%d.%d.%lu) absolute:%lu\n",\
        _obj->name->handleref.code,\
        _obj->name->handleref.size,\
        _obj->name->handleref.value,\
        _obj->name->absolute_ref);\
    }

#define FIELD_B(name) FIELD(name, B);
#define FIELD_BB(name) FIELD(name, BB);
#define FIELD_BS(name) FIELD(name, BS);
#define FIELD_BL(name) FIELD(name, BL);
#define FIELD_BD(name) FIELD(name, BD);
#define FIELD_RC(name) FIELD(name, RC);
#define FIELD_RS(name) FIELD(name, RS);
#define FIELD_RD(name) FIELD(name, RD);
#define FIELD_RL(name) FIELD(name, RL);
#define FIELD_MC(name) FIELD(name, MC);
#define FIELD_MS(name) FIELD(name, MS);
#define FIELD_TV(name) FIELD(name, TV);
#define FIELD_BT(name) FIELD(name, BT);

#define FIELD_BE(name) bit_read_BE(dat, &_obj->name.x, &_obj->name.y, &_obj->name.z);
#define FIELD_DD(name, _default) GET_FIELD(name) = bit_read_DD(dat, _default);
#define FIELD_2DD(name, d1, d2) FIELD_DD(name.x, d1); FIELD_DD(name.y, d2);  
#define FIELD_2RD(name) FIELD(name.x, RD); FIELD(name.y, RD);
#define FIELD_2BD(name) FIELD(name.x, BD); FIELD(name.y, BD);
#define FIELD_3BD(name) FIELD(name.x, BD); FIELD(name.y, BD); FIELD(name.z, BD);
#define FIELD_3DPOINT(name) FIELD_3BD(name)
#define FIELD_CMC(name)\
  {\
    bit_read_CMC(dat, &_obj->name);\
    if (loglevel>=2)\
      fprintf(stderr, #name ": index %d\n", _obj->name.index);\
  }

//FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size)\
  if (size>0)\
    {\
      _obj->name = (BITCODE_##type*) malloc(size * sizeof(BITCODE_##type));\
      for (vector_counter=0; vector_counter< size; vector_counter++)\
        {\
          _obj->name[vector_counter] = bit_read_##type(dat);\
          if (loglevel>=2)\
            {\
                fprintf(stderr, #name "[%d]: " FORMAT_##type "\n", vector_counter, _obj->name[vector_counter]);\
            }\
        }\
    }

#define FIELD_VECTOR(name, type, size) FIELD_VECTOR_N(name, type, _obj->size)

#define FIELD_3DPOINT_VECTOR(name, size)\
  _obj->name = (BITCODE_3DPOINT *) malloc(_obj->size * sizeof(BITCODE_3DPOINT));\
  for (vector_counter=0; vector_counter< _obj->size; vector_counter++)\
    {\
      FIELD_3DPOINT(name[vector_counter]);\
    }

#define HANDLE_VECTOR(name, sizefield, code)\
  GET_FIELD(name) = (Dwg_Object_Ref**) malloc(sizeof(Dwg_Object_Ref*) * GET_FIELD(sizefield));\
  for (vector_counter=0; vector_counter<GET_FIELD(sizefield); vector_counter++)\
    {\
      FIELD_HANDLE(name[vector_counter], code);\
    }

#define REACTORS(code)\
  GET_FIELD(reactors) = malloc(sizeof(Dwg_Object_Ref*) * obj->tio.object->num_reactors);\
  for (vector_counter=0; vector_counter<obj->tio.object->num_reactors; vector_counter++)\
    {\
      FIELD_HANDLE(reactors[vector_counter], code);\
    }

#define XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.object->xdic_missing_flag)\
        FIELD_HANDLE(xdicobjhandle, code);\
    }\
  PRIOR_VERSIONS\
    {\
      FIELD_HANDLE(xdicobjhandle, code);\
    }

#define REPEAT(times, name, type) \
  _obj->name = (type *) malloc(_obj->times * sizeof(type));\
  for (vector_counter=0; vector_counter<_obj->times; vector_counter++)

#define COMMON_ENTITY_HANDLE_DATA \
  dwg_decode_common_entity_handle_data(dat, obj)

#define DWG_ENTITY(token) \
  int vector_counter;\
  if (loglevel)\
  fprintf (stderr, "Entity " #token ":\n");\
	Dwg_Entity_##token *ent, *_obj;\
	obj->supertype = DWG_SUPERTYPE_ENTITY;\
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));\
	obj->tio.entity->tio.token = calloc (sizeof (Dwg_Entity_##token), 1);\
	ent = obj->tio.entity->tio.token;\
  _obj=ent;\
  obj->tio.entity->object = obj;\
	dwg_decode_entity (dat, obj->tio.entity);\
  fprintf (stderr, "Entity handle: %d.%d.%lu\n",\
    obj->handle.code,\
    obj->handle.size,\
    obj->handle.value);

#define DWG_OBJECT(token) \
  int vector_counter;\
  if (loglevel)\
    fprintf (stderr, "Object " #token ":\n");\
	Dwg_Object_##token *_obj;\
	obj->supertype = DWG_SUPERTYPE_OBJECT;\
	obj->tio.object = malloc (sizeof (Dwg_Object_Object));\
	obj->tio.object->tio.token = calloc (sizeof (Dwg_Object_##token), 1);\
  obj->tio.object->object = obj;\
	dwg_decode_object (dat, obj->tio.object);\
	_obj = obj->tio.object->tio.token;\
  fprintf (stderr, "Object handle: %d.%d.%lu\n",\
    obj->handle.code,\
    obj->handle.size,\
    obj->handle.value);

/*--------------------------------------------------------------------------------
 * Private functions
 */
static void
dwg_decode_add_object(Dwg_Data * dwg, Bit_Chain * dat,
    long unsigned int address);

static Dwg_Object *
dwg_resolve_handle(Dwg_Data* dwg, unsigned long int handle);

/*--------------------------------------------------------------------------------
 * Public variables
 */
long unsigned int ktl_lastaddress;

static int loglevel = 2;

/*--------------------------------------------------------------------------------
 * Public function definitions
 */
int
dwg_decode_data(Bit_Chain * dat, Dwg_Data * dwg)
{
  char version[7];
  dwg->num_object_refs = 0;

  /* Version */
  dat->byte = 0;
  dat->bit = 0;
  strncpy(version, dat->chain, 6);
  version[6] = '\0';

  dwg->header.version = 0;
  if (!strcmp(version, version_codes[R_13]))
    dwg->header.version = R_13;
  if (!strcmp(version, version_codes[R_14]))
    dwg->header.version = R_14;
  if (!strcmp(version, version_codes[R_2000]))
    dwg->header.version = R_2000;
  if (!strcmp(version, version_codes[R_2004]))
    dwg->header.version = R_2004;
  if (!strcmp(version, version_codes[R_2007]))
    dwg->header.version = R_2007;
  if (dwg->header.version == 0)
    {
      fprintf(stderr, "Invalid or unimplemented version code!"
        "This file's version code is: %s\n", version);
      return -1;
    }
  dat->version = dwg->header.version;

  if (dat->version < R_2000)
    {
      fprintf(
          stderr,
          "WARNING: This version of LibreDWG is only capable of safely decoding version R2000 (code: AC1015) dwg-files.\n"
            "This file's version code is: %s Support for this version is still experimental.\n"
            "It might crash or give you invalid output.\n", version);
      return decode_R13_R15_header(dat, dwg);
    }

  VERSION(R_2000)
    {
      return decode_R13_R15_header(dat, dwg);
    }

  VERSION(R_2004)
    {
      fprintf(
          stderr,
          "WARNING: This version of LibreDWG is only capable of properly decoding version R2000 (code: AC1015) dwg-files.\n"
            "This file's version code is: %s\n This version is not yet actively developed.\n"
            "It will probably crash and/or give you invalid output.\n", version);
      return decode_R2004_header(dat, dwg);
    }

  VERSION(R_2007)
    {
      fprintf(
          stderr,
          "WARNING: This version of LibreDWG is only capable of properly decoding version R2000 (code: AC1015) dwg-files.\n"
            "This file's version code is: %s\n This version is not yet actively developed.\n"
            "It will probably crash and/or give you invalid output.\n", version);
      return decode_R2007_header(dat, dwg);
    }

  //This line should not be reached!
  fprintf(
      stderr,
      "ERROR: LibreDWG could not recognize the version string in this file: %s.\n",
      version);
  return -1;
}

Dwg_Object* dwg_next_object(Dwg_Object* obj){
  if ((obj->index+1) > obj->parent->num_objects-1)
    return 0;
  return &obj->parent->object[obj->index+1];
}

int
decode_R13_R15_header(Bit_Chain* dat, Dwg_Data * dwg)
{
  unsigned char sig;
  unsigned int section_size = 0;
  unsigned char sgdc[2];
  unsigned int ckr, ckr2, antckr;
  long unsigned int size;
  long unsigned int lasta;
  long unsigned int maplasta;
  long unsigned int duabyte;
  long unsigned int object_begin;
  long unsigned int object_end;
  long unsigned int pvz;
  unsigned int i, j;

  // Still unknown values: 6 'zeroes' and a 'one'
  dat->byte = 0x06;
  if (loglevel)
    fprintf(stderr, "Still unknown values: 6 'zeroes' and a 'one': ");
  for (i = 0; i < 7; i++)
    {
      sig = bit_read_RC(dat);
      if (loglevel)
        fprintf(stderr, "0x%02X ", sig);
    }
  if (loglevel)
    fprintf(stderr, "\n");

  /* Image Seeker */
  pvz = bit_read_RL(dat);
  if (loglevel)
    fprintf(stderr, "Image seeker: 0x%08X\n", (unsigned int) pvz);

  // unknown
  sig = bit_read_RC(dat);
  if (loglevel)
    fprintf(stderr, "Version: %u\n", sig);
  sig = bit_read_RC(dat);
  if (loglevel)
    fprintf(stderr, "Lancxo: %u\n", sig);

  /* Codepage */
  dat->byte = 0x13;
  dwg->header.codepage = bit_read_RS(dat);
  if (loglevel)
    fprintf(stderr, "Codepage: %u\n", dwg->header.codepage);

  /* Section Locator Records */
  dat->byte = 0x15;
  dwg->header.num_sections = bit_read_RL(dat);

  //  why do we have this limit to only 6 sections?
  //  It seems to be a bug, so I'll comment it out and will add dynamic
  //  allocation of the sections vector.
  //  OpenDWG spec speaks of 6 possible values for the record number
  //  Maybe the original libdwg author got confused about that.
  /*
   if (dwg->header.num_sections > 6)
   dwg->header.num_sections = 6;
   */
  dwg->header.section = (Dwg_Section*) malloc(sizeof(Dwg_Section)
      * dwg->header.num_sections);

  for (i = 0; i < dwg->header.num_sections; i++)
    {
      dwg->header.section[i].address = 0;
      dwg->header.section[i].size = 0;

      dwg->header.section[i].number = bit_read_RC(dat);
      dwg->header.section[i].address = bit_read_RL(dat);
      dwg->header.section[i].size = bit_read_RL(dat);
    }

  // Check CRC-on
  /*
   ckr = bit_read_CRC (dat);
   dat->byte -= 2;
   bit_create_CRC (dat, 0, 0);
   dat->byte -= 2;
   ckr2 = bit_read_CRC (dat);
   dat->byte -= 2;
   bit_write_RS (dat, ckr2 ^ 0x8461);
   dat->byte -= 2;
   ckr2 = bit_read_CRC (dat);
   if (loglevel) fprintf (stderr, "Read: %X\nCalculated: %X\n", ckr, ckr2);
   */

  if (bit_search_sentinel(dat, dwg_sentinel(DWG_SENTINEL_HEADER_END))
      && loglevel)
    fprintf(stderr, "=======> HEADER (end): %8X\n", (unsigned int) dat->byte);

  /*-------------------------------------------------------------------------
   * Unknown section 1
   */

  if (dwg->header.num_sections == 6)
    {
      if (loglevel)
        {
          fprintf(stderr, "========> UNKNOWN 1: %8X\n",
              (unsigned int) dwg->header.section[5].address);
          fprintf(stderr, "   UNKNOWN 1 (end): %8X\n",
              (unsigned int) (dwg->header.section[5].address
                  + dwg->header.section[5].size));
        }
      dat->byte = dwg->header.section[5].address;
      dwg->unknown1.size = DWG_UNKNOWN1_SIZE;
      dwg->unknown1.byte = dwg->unknown1.bit = 0;
      dwg->unknown1.chain = malloc(dwg->unknown1.size);
      memcpy(dwg->unknown1.chain, &dat->chain[dat->byte], dwg->unknown1.size);
      //bit_explore_chain ((Bit_Chain *) &dwg->unknown1, dwg->unknown1.size);
      //bit_print ((Bit_Chain *) &dwg->unknown1, dwg->unknown1.size);
    }

  /*-------------------------------------------------------------------------
   * Picture (Pre-R13C3?)
   */

  if (bit_search_sentinel(dat, dwg_sentinel(DWG_SENTINEL_PICTURE_BEGIN)))
    {
      unsigned long int start_address;

      dat->bit = 0;
      start_address = dat->byte;
      if (loglevel)
        fprintf(stderr, "=============> PICTURE: %8X\n",
            (unsigned int) start_address - 16);
      if (bit_search_sentinel(dat, dwg_sentinel(DWG_SENTINEL_PICTURE_END)))
        {
          if (loglevel)
            fprintf(stderr, "        PICTURE (end): %8X\n",
                (unsigned int) dat->byte);
          dwg->picture.size = (dat->byte - 16) - start_address;
          dwg->picture.chain = (char *) malloc(dwg->picture.size);
          memcpy(dwg->picture.chain, &dat->chain[start_address],
              dwg->picture.size);
        }
      else
        dwg->picture.size = 0;
    }

  /*-------------------------------------------------------------------------
   * Header Variables
   */

  if (loglevel)
    {
      fprintf(stderr, "=====> Header Variables: %8X\n",
          (unsigned int) dwg->header.section[0].address);
      fprintf(stderr, "Header Variables (end): %8X\n",
          (unsigned int) (dwg->header.section[0].address
              + dwg->header.section[0].size));
    }
  dat->byte = dwg->header.section[0].address + 16;
  pvz = bit_read_RL(dat);
  if (loglevel)
    fprintf(stderr, "Length: %lu\n", pvz);

  dat->bit = 0;

  /* Read header variables
   */
  for (i = 0; i < DWG_NUM_VARIABLES; i++)
    {
      if (loglevel)
        fprintf(stderr, "[%03i] - ", i + 1);
      if (i == 221 && dwg->var[220].bitdouble != 3)
        {
          dwg->var[i].handle.code = 0;
          dwg->var[i].handle.value = 0;
          continue;
        }
      switch (dwg_var_map(dwg->header.version, i))
        {
      case DWG_END_OF_HEADER_VARIABLES:
        break;
      case DWG_DT_B:
        dwg->var[i].bit = bit_read_B(dat);
        if (loglevel)
          fprintf(stderr, "B: %u", dwg->var[i].bit);
        break;
      case DWG_DT_BS:
        dwg->var[i].bitshort = bit_read_BS(dat);
        if (loglevel)
          fprintf(stderr, "BS: %u", dwg->var[i].bitshort);
        break;
      case DWG_DT_BL:
        dwg->var[i].bitlong = bit_read_BL(dat);
        if (loglevel)
          fprintf(stderr, "BL: %lu", dwg->var[i].bitlong);
        break;
      case DWG_DT_BD:
        dwg->var[i].bitdouble = bit_read_BD(dat);
        if (loglevel)
          fprintf(stderr, "BD: %lg", dwg->var[i].bitdouble);
        break;
      case DWG_DT_H:
        bit_read_H(dat, &dwg->var[i].handle);
        if (loglevel)
          fprintf(stderr, "H: %i.%i.0x%08X", dwg->var[i].handle.code,
              dwg->var[i].handle.size, (unsigned int) dwg->var[i].handle.value);
        break;
      case DWG_DT_T:
        dwg->var[i].text = bit_read_TV(dat);
        if (loglevel)
          fprintf(stderr, "T: \"%s\"", dwg->var[i].text);
        break;
      case DWG_DT_CMC:
        dwg->var[i].bitshort = bit_read_BS(dat);
        if (loglevel)
          fprintf(stderr, "CMC: %u", dwg->var[i].bitshort);
        break;
      case DWG_DT_2RD:
        dwg->var[i].xy[0] = bit_read_RD(dat);
        dwg->var[i].xy[1] = bit_read_RD(dat);
        if (loglevel)
          {
            fprintf(stderr, "X: %lg\t", dwg->var[i].xy[0]);
            fprintf(stderr, "Y: %lg", dwg->var[i].xy[1]);
          }
        break;
      case DWG_DT_3BD:
        dwg->var[i].xyz[0] = bit_read_BD(dat);
        dwg->var[i].xyz[1] = bit_read_BD(dat);
        dwg->var[i].xyz[2] = bit_read_BD(dat);
        if (loglevel)
          {
            fprintf(stderr, "X: %lg\t", dwg->var[i].xyz[0]);
            fprintf(stderr, "Y: %lg\t", dwg->var[i].xyz[1]);
            fprintf(stderr, "Z: %lg", dwg->var[i].xyz[2]);
          }
        break;
      default:
        if (loglevel)
          fprintf(stderr, "No handle type: %i (var: %i)\n", dwg_var_map(
              dwg->header.version, i), i);
        }
    }

  // Check CRC-on
  ckr = bit_read_CRC(dat);
  /*
   for (i = 0xC001; i != 0xC000; i++)
   {
   dat->byte -= 2;
   bit_write_CRC (dat, dwg->header.section[0].address + 16, i);
   dat->byte -= 2;
   ckr2 = bit_read_CRC (dat);
   if (ckr == ckr2)
   {
   if (loglevel) fprintf (stderr, "Read: %X\nCreated: %X\t SEMO: %02X\n", ckr, ckr2, i);
   break;
   }
   }
   */

  /*-------------------------------------------------------------------------
   * Classes
   */
  if (loglevel)
    {
      fprintf(stderr, "============> CLASS: %8X\n",
          (unsigned int) dwg->header.section[1].address);
      fprintf(stderr, "       CLASS (end): %8X\n",
          (unsigned int) (dwg->header.section[1].address
              + dwg->header.section[1].size));
    }
  dat->byte = dwg->header.section[1].address + 16;
  dat->bit = 0;

  size = bit_read_RL(dat);
  lasta = dat->byte + size;
  //if (loglevel) fprintf (stderr, "Length: %lu\n", size);

  /* read the classes
   */
  dwg->dwg_ot_layout = 0;
  dwg->num_classes = 0;
  i = 0;
  do
    {
      unsigned int idc;

      idc = dwg->num_classes;
      if (idc == 0)
        dwg->class = (Dwg_Class *) malloc(sizeof(Dwg_Class));
      else
        dwg->class = (Dwg_Class *) realloc(dwg->class, (idc + 1)
            * sizeof(Dwg_Class));

      dwg->class[idc].number = bit_read_BS(dat);
      dwg->class[idc].version = bit_read_BS(dat);
      dwg->class[idc].appname = bit_read_TV(dat);
      dwg->class[idc].cppname = bit_read_TV(dat);
      dwg->class[idc].dxfname = bit_read_TV(dat);
      dwg->class[idc].wasazombie = bit_read_B(dat);
      dwg->class[idc].item_class_id = bit_read_BS(dat);

      if (strcmp(dwg->class[idc].dxfname, "LAYOUT") == 0)
        dwg->dwg_ot_layout = dwg->class[idc].number;

      dwg->num_classes++;
      if (dwg->num_classes > 100)
        break;//TODO: Why?!
    }
  while (dat->byte < (lasta - 1));

  // Check CRC-on
  ckr = bit_read_CRC(dat);
  /*
   for (i = 0xC001; i != 0xC000; i++)
   {
   dat->byte -= 2;
   bit_write_CRC (dat, dwg->header.section[1].address + 16, i);
   dat->byte -= 2;
   ckr2 = bit_read_CRC (dat);
   if (ckr == ckr2)
   {
   if (loglevel) fprintf (stderr, "Read: %X\nCreated: %X\t SEMO: %02X\n", ckr, ckr2, i);
   break;
   }
   }
   */

  dat->byte += 16;
  pvz = bit_read_RL(dat); // Unknown bitlong inter class and object
  //if (loglevel) {
  // fprintf (stderr, "Address: %lu / Content: 0x%08X\n", dat->byte - 4, pvz);
  // fprintf (stderr, "Number of classes read: %u\n", dwg->num_classes);
  //}

  /*-------------------------------------------------------------------------
   * Object-map (object mem)
   */

  dat->byte = dwg->header.section[2].address;
  dat->bit = 0;

  maplasta = dat->byte + dwg->header.section[2].size; // 4
  dwg->num_objects = 0;
  object_begin = dat->size;
  object_end = 0;
  do
    {
      long unsigned int last_address;
      long unsigned int last_handle;
      long unsigned int previous_address;

      duabyte = dat->byte;
      sgdc[0] = bit_read_RC(dat);
      sgdc[1] = bit_read_RC(dat);
      section_size = (sgdc[0] << 8) | sgdc[1];
      //if (loglevel) fprintf (stderr, "section_size: %u\n", section_size);
      if (section_size > 2034) // 2032 + 2
        {
          fprintf(stderr, "Error: Object-map section size greater than 2034!");
          return -1;
        }

      last_handle = 0;
      last_address = 0;
      while (dat->byte - duabyte < section_size)
        {
          long unsigned int kobj;
          long int pvztkt;
          long int pvzadr;

          previous_address = dat->byte;
          pvztkt = bit_read_MC(dat);
          last_handle += pvztkt;
          pvzadr = bit_read_MC(dat);
          last_address += pvzadr;
          //if (loglevel) {
          // fprintf (stderr, "Idc: %li\t", dwg->num_objects);
          // fprintf (stderr, "Handle: %li\tAddress: %li\n", pvztkt, pvzadr);
          //}
          if (dat->byte == previous_address)
            break;
          //if (dat->byte - duabyte >= seksize)
          //break;

          if (object_end < last_address)
            object_end = last_address;
          if (object_begin > last_address)
            object_begin = last_address;

          kobj = dwg->num_objects;
          dwg_decode_add_object(dwg, dat, last_address);
          //if (dwg->num_objects > kobj)
          //dwg->object[dwg->num_objects - 1].handle.value = lastahandle;
          //TODO: blame Juca
        }
      if (dat->byte == previous_address)
        break;
      dat->byte += 2; // CRC

      if (dat->byte >= maplasta)
        break;
    }
  while (section_size > 2);

  if (loglevel)
    {
      fprintf(stderr, "Num objects: %lu\n", dwg->num_objects);
      fprintf(stderr, "=========> Object Data: %8X\n", (unsigned int) object_begin);
    }
  dat->byte = object_end;
  object_begin = bit_read_MS(dat);
  if (loglevel)
    fprintf(stderr, "    Object Data (end): %8X\n", (unsigned int) (object_end + object_begin
        + 2));

  /*
   dat->byte = dwg->header.section[2].address - 2;
   antckr = bit_read_CRC (dat); // Unknown bitdouble inter object data and object map
   if (loglevel) fprintf (stderr, "Address: %08X / Content: 0x%04X\n", dat->byte - 2, antckr);

   // check CRC-on
   antckr = 0xC0C1;
   do
   {
   duabyte = dat->byte;
   sgdc[0] = bit_read_RC (dat);
   sgdc[1] = bit_read_RC (dat);
   section_size = (sgdc[0] << 8) | sgdc[1];
   section_size -= 2;
   dat->byte += section_size;
   ckr = bit_read_CRC (dat);
   dat->byte -= 2;
   bit_write_CRC (dat, duabyte, antckr);
   dat->byte -= 2;
   ckr2 = bit_read_CRC (dat);
   if (loglevel) fprintf (stderr, "Read: %X\nCreated: %X\t SEMO: %X\n", ckr, ckr2, antckr);
   //antckr = ckr;
   } while (section_size > 0);
   */
  if (loglevel)
    {
      fprintf(stderr, "======> Object Map: %8X\n",
          (unsigned int) dwg->header.section[2].address);
      fprintf(stderr, " Object Map (end): %8X\n",
          (unsigned int) (dwg->header.section[2].address
              + dwg->header.section[2].size));
    }

  /*-------------------------------------------------------------------------
   * Dua kap-datenaro
   */

  if (bit_search_sentinel(dat, dwg_sentinel(DWG_SENTINEL_SECOND_HEADER_BEGIN)))
    {
      long unsigned int pvzadr;
      long unsigned int pvz;
      unsigned char sig, sig2;

      if (loglevel)
        fprintf(stderr, "==> Second Header: %8X\n", (unsigned int) dat->byte
            - 16);
      pvzadr = dat->byte;

      pvz = bit_read_RL(dat);
      //if (loglevel) fprintf (stderr, "Size: %lu\n", pvz);

      pvz = bit_read_BL(dat);
      //if (loglevel) fprintf (stderr, "Begin address: %8X\n", pvz);

      //if (loglevel) fprintf (stderr, "AC1015?: ");
      for (i = 0; i < 6; i++)
        {
          sig = bit_read_RC(dat);
          //if (loglevel) fprintf (stderr, "%c", sig >= ' ' && sig < 128 ? sig : '.');
        }

      //if (loglevel) fprintf (stderr, "\nNull?:");
      for (i = 0; i < 5; i++) // 6 if is older...
        {
          sig = bit_read_RC(dat);
          //if (loglevel) fprintf (stderr, " 0x%02X", sig);
        }

      //if (loglevel) fprintf (stderr, "\n4 null bits?: ");
      for (i = 0; i < 4; i++)
        {
          sig = bit_read_B(dat);
          //if (loglevel) fprintf (stderr, " %c", sig ? '1' : '0');
        }

      //if (loglevel) fprintf (stderr, "\nChain?: ");
      for (i = 0; i < 6; i++)
        {
          dwg->second_header.unknown[i] = bit_read_RC(dat);
          //if (loglevel) fprintf (stderr, " 0x%02X", dwg->second_header.unknown[i]);
        }
      if (dwg->second_header.unknown[3] != 0x78
          || dwg->second_header.unknown[5] != 0x06)
        sig = bit_read_RC(dat); // To compensate in the event of a contingent
                                // additional zero not read previously

      //puts("");
      for (i = 0; i < 6; i++)
        {
          sig = bit_read_RC(dat);
          //if (loglevel) fprintf (stderr, "[%u]\n", sig);
          pvz = bit_read_BL(dat);
          //if (loglevel) fprintf (stderr, " Address: %8X\n", pvz);
          pvz = bit_read_BL(dat);
          //if (loglevel) fprintf (stderr, "  Size: %8X\n", pvz);
        }

      bit_read_BS(dat);
      //if (loglevel) fprintf (stderr, "\n14 --------------");
      for (i = 0; i < 14; i++)
        {
          sig2 = bit_read_RC(dat);
          dwg->second_header.handlerik[i].size = sig2;
          //if (loglevel) fprintf (stderr, "\nSize: %u\n", sig2);
          sig = bit_read_RC(dat);
          //if (loglevel) fprintf (stderr, "\t[%u]\n", sig);
          //if (loglevel) fprintf (stderr, "\tChain:");
          for (j = 0; j < sig2; j++)
            {
              sig = bit_read_RC(dat);
              dwg->second_header.handlerik[i].chain[j] = sig;
              //if (loglevel) fprintf (stderr, " %02X", sig);
            }
        }

      // Check CRC-on
      ckr = bit_read_CRC(dat);
      /*
       puts ("");
       for (i = 0; i != 0xFFFF; i++)
       {
       dat->byte -= 2;
       bit_write_CRC (dat, pvzadr, i);
       dat->byte -= 2;
       ckr2 = bit_read_CRC (dat);
       if (ckr == ckr2)
       {
       if (loglevel) fprintf (stderr, "Read: %X\nCreated: %X\t SEMO: %02X\n", ckr, ckr2, i);
       break;
       }
       }
       if (loglevel) {
       fprintf (stderr, " Garbage 1: %08X\n", bit_read_RL (dat));
       fprintf (stderr, " Garbage 2: %08X\n", bit_read_RL (dat));
       }
       */

      if (loglevel && bit_search_sentinel(dat, dwg_sentinel(
          DWG_SENTINEL_SECOND_HEADER_END)))
        fprintf(stderr, " Second Header (end): %8X\n", (unsigned int) dat->byte);
    }

  /*-------------------------------------------------------------------------
   * Section MEASUREMENT
   */

  if (loglevel)
    {
      fprintf(stderr, "========> Unknown 2: %8X\n",
          (unsigned int) dwg->header.section[4].address);
      fprintf(stderr, "   Unknown 2 (end): %8X\n",
          (unsigned int) (dwg->header.section[4].address
              + dwg->header.section[4].size));
    }
  dat->byte = dwg->header.section[4].address;
  dat->bit = 0;
  dwg->measurement = bit_read_RL(dat);

  if (loglevel)
    fprintf(stderr, "Size bytes :\t%lu\n", dat->size);

  //step II of handles parsing: resolve pointers from handle value
  //XXX: move this somewhere else
  for (i = 0; i < dwg->num_object_refs; i++)
    {
      dwg->object_ref[i]->obj = dwg_resolve_handle(dwg,
          dwg->object_ref[i]->absolute_ref);
    }

  return 0;
}

//negative value indicates a compression opcode
int
read_literal_length(Bit_Chain* dat)
{
  int total = 0;
  unsigned char byte = bit_read_RC(dat);
  if (byte >= 0x01 && byte <= 0x0E)
    return byte + 3;
  if (byte == 0)
    {
      total = 0x0F;
      while ((byte = bit_read_RC(dat)) > 0x00)
        {
          total += 0xFF;
        }
      return total + 3;
    }
  if (byte & 0xF0)
    {
      return -byte;
    }
}

int
decode_R2004_header(Bit_Chain* dat, Dwg_Data * dwg)
{
  int i;
  unsigned long int preview_address, security_type, unknown_long,
      dwg_property_address, vba_proj_address;
  unsigned char sig, dwg_ver, maint_release_ver;

  //6 bytes of 0x00
  dat->byte = 0x06;
  if (loglevel)
    fprintf(stderr, "6 bytes of 0x00: ");
  for (i = 0; i < 6; i++)
    {
      sig = bit_read_RC(dat);
      if (loglevel)
        fprintf(stderr, "0x%02X ", sig);
    }
  if (loglevel)
    fprintf(stderr, "\n");

  /* Byte 0x00, 0x01, or 0x03 */
  dat->byte = 0x0C;
  sig = bit_read_RC(dat);
  if (loglevel)
    fprintf(stderr, "Byte 0x00, 0x01, or 0x03: 0x%02X\n", sig);

  /* Preview Address */
  dat->byte = 0x0D;
  preview_address = bit_read_RL(dat);
  if (loglevel)
    fprintf(stderr, "Preview Address: 0x%08X\n", (unsigned int) preview_address);

  /* DwgVer */
  dat->byte = 0x11;
  dwg_ver = bit_read_RC(dat);
  if (loglevel)
    fprintf(stderr, "DwgVer: %u\n", dwg_ver);

  /* MaintReleaseVer */
  dat->byte = 0x12;
  maint_release_ver = bit_read_RC(dat);
  if (loglevel)
    fprintf(stderr, "MaintRelease: %u\n", maint_release_ver);

  /* Codepage */
  dat->byte = 0x13;
  dwg->header.codepage = bit_read_RS(dat);
  if (loglevel)
    fprintf(stderr, "Codepage: %u\n", dwg->header.codepage);

  /* 3 0x00 bytes */
  dat->byte = 0x15;
  if (loglevel)
    fprintf(stderr, "3 0x00 bytes: ");
  for (i = 0; i < 3; i++)
    {
      sig = bit_read_RC(dat);
      if (loglevel)
        fprintf(stderr, "0x%02X ", sig);
    }
  if (loglevel)
    fprintf(stderr, "\n");

  /* SecurityType */
  dat->byte = 0x18;
  security_type = bit_read_RL(dat);
  if (loglevel)
    fprintf(stderr, "SecurityType: 0x%08X\n", (unsigned int) security_type);

  /* Unknown long */
  dat->byte = 0x1C;
  unknown_long = bit_read_RL(dat);
  if (loglevel)
    fprintf(stderr, "Unknown long: 0x%08X\n", (unsigned int) unknown_long);

  /* DWG Property Addr */
  dat->byte = 0x20;
  dwg_property_address = bit_read_RL(dat);
  if (loglevel)
    fprintf(stderr, "DWG Property Addr: 0x%08X\n",
        (unsigned int) dwg_property_address);

  /* VBA Project Addr */
  dat->byte = 0x24;
  vba_proj_address = bit_read_RL(dat);
  if (loglevel)
    fprintf(stderr, "VBA Project Addr: 0x%08X\n",
        (unsigned int) vba_proj_address);

  /* 0x00000080 */
  dat->byte = 0x28;
  unknown_long = bit_read_RL(dat);
  if (loglevel)
    fprintf(stderr, "0x00000080: 0x%08X\n", (unsigned int) unknown_long);

  /* 0x00 bytes (length = 0x54 bytes) */
  dat->byte = 0x2C;
  for (i = 0; i < 0x54; i++)
    {
      sig = bit_read_RC(dat);
      if (sig != 0 && loglevel)
        fprintf(stderr,
            "Warning: Byte should be zero! But a value=%x was read instead.\n",
            sig);
    }

  /* Encripted Data */
  union
  {
    unsigned char encripted_data[0x6c];
    struct
    {
      unsigned char file_ID_string[12];
      unsigned long int x00;
      unsigned long int x6c;
      unsigned long int x04;
      unsigned long int root_tree_node_gap;
      unsigned long int lowermost_left_tree_node_gap;
      unsigned long int lowermost_right_tree_node_gap;
      unsigned long int unknown_long;
      unsigned long int last_section_id;
      unsigned long int last_section_address;
      unsigned long int x00_2;
      unsigned long int second_header_address;
      unsigned long int x00_3;
      unsigned long int gap_amount;
      unsigned long int section_amount;
      unsigned long int x20;
      unsigned long int x80;
      unsigned long int x40;
      unsigned long int section_map_id;
      unsigned long int section_map_address;
      unsigned long int x00_4;
      unsigned long int section_info_id;
      unsigned long int section_array_size;
      unsigned long int gap_array_size;
      unsigned long int CRC;
    } fields;
  } _2004_header_data;

  int rseed = 1;
  dat->byte = 0x80;
  for (i = 0; i < 0x6c; i++)
    {
      rseed *= 0x343fd;
      rseed += 0x269ec3;
      _2004_header_data.encripted_data[i] = bit_read_RC(dat) ^ (rseed >> 0x10);
    }

  if (loglevel)
    {
      fprintf(stderr, "\n#### 2004 File Header Data fields ####\n\n");
      fprintf(stderr, "File ID string (must be AcFssFcAJMB): ");
      for (i = 0; i < 12; i++)
        fprintf(stderr, "%c", _2004_header_data.fields.file_ID_string[i]);
      fprintf(stderr, "\n");
      fprintf(stderr, "0x00 (long): %x\n",
          (unsigned int) _2004_header_data.fields.x00);
      fprintf(stderr, "0x6c (long): %x\n",
          (unsigned int) _2004_header_data.fields.x6c);
      fprintf(stderr, "0x04 (long): %x\n",
          (unsigned int) _2004_header_data.fields.x04);
      fprintf(stderr, "Root tree node gap: %x\n",
          (unsigned int) _2004_header_data.fields.root_tree_node_gap);
      fprintf(stderr, "Lowermost left tree node gap: %x\n",
          (unsigned int) _2004_header_data.fields.lowermost_left_tree_node_gap);
      fprintf(stderr, "Lowermost right tree node gap: %x\n",
          (unsigned int) _2004_header_data.fields.lowermost_right_tree_node_gap);
      fprintf(stderr, "Unknown long: %x\n",
          (unsigned int) _2004_header_data.fields.unknown_long);
      fprintf(stderr, "Last section id: %x\n",
          (unsigned int) _2004_header_data.fields.last_section_id);
      fprintf(stderr, "Last section address: %x\n",
          (unsigned int) _2004_header_data.fields.last_section_address);
      fprintf(stderr, "0x00 (long): %x\n",
          (unsigned int) _2004_header_data.fields.x00_2);
      fprintf(stderr, "Second header address: %x\n",
          (unsigned int) _2004_header_data.fields.second_header_address);
      fprintf(stderr, "0x00 (long): %x\n",
          (unsigned int) _2004_header_data.fields.x00_3);
      fprintf(stderr, "Gap amount: %x\n",
          (unsigned int) _2004_header_data.fields.gap_amount);
      fprintf(stderr, "Section amount: %x\n",
          (unsigned int) _2004_header_data.fields.section_amount);
      fprintf(stderr, "0x20 (long): %x\n",
          (unsigned int) _2004_header_data.fields.x20);
      fprintf(stderr, "0x80 (long): %x\n",
          (unsigned int) _2004_header_data.fields.x80);
      fprintf(stderr, "0x40 (long): %x\n",
          (unsigned int) _2004_header_data.fields.x40);
      fprintf(stderr, "Section map id: %x\n",
          (unsigned int) _2004_header_data.fields.section_map_id);
      fprintf(stderr, "Section map address: %x\n",
          (unsigned int) _2004_header_data.fields.section_map_address + 0x100);
      fprintf(stderr, "0x00 (long): %x\n",
          (unsigned int) _2004_header_data.fields.x00_4);
      fprintf(stderr, "Section Info id: %x\n",
          (unsigned int) _2004_header_data.fields.section_info_id);
      fprintf(stderr, "Section array size: %x\n",
          (unsigned int) _2004_header_data.fields.section_array_size);
      fprintf(stderr, "Gap array size: %x\n",
          (unsigned int) _2004_header_data.fields.gap_array_size);
      fprintf(stderr, "CRC: %x\n", (unsigned int) _2004_header_data.fields.CRC);
    }

  //////////////////////////////////////////////
  // up to this line the code seems to be ok. //
  //////////////////////////////////////////////

  dat->byte = _2004_header_data.fields.section_map_address + 0x100;
  fprintf(stderr, "section_map_address DUMP:\n");
  for (i = 0; i < 32; i++)
    {
      fprintf(stderr, "%x ", bit_read_RC(dat));
    }

  /* System Section */
  typedef union _system_section
  {
    unsigned char data[0x14];
    struct
    {
      unsigned long int section_type;
      unsigned long int DecompDataSize;
      unsigned long int CompDataSize;
      unsigned long int compression_type;
      unsigned long int checksum;
    } fields;
  } system_section;

  /* Encrypted Section Header */
  typedef union _encrypted_section_header
  {
    unsigned long int long_data[8];
    unsigned char char_data[32];
    struct
    {
      unsigned long int section_type;
      unsigned long int data_size;
      unsigned long int section_size;
      unsigned long int start_offset;
      unsigned long int unknown;
      unsigned long int checksum_1;
      unsigned long int checksum_2;
      unsigned long int unknown_2;
    } fields;
  } encrypted_section_header;

  encrypted_section_header es;
  for (i = 0; i < 32; i++)
    {
      es.char_data[i] = bit_read_RC(dat);
    }

  unsigned long int secMask = 0x4164536b;
  for (i = 0; i < 8; i++)
    {
      es.long_data[i] ^= secMask;
    }

  if (loglevel)
    {
      fprintf(stderr, "\n\n=== Encrypted Section Header ===\n");
      fprintf(stderr, "Section Type: %x\n",
          (unsigned int) es.fields.section_type);
      fprintf(stderr, "Data size: %x\n", (unsigned int) es.fields.data_size);
      fprintf(stderr, "Section size: %x\n",
          (unsigned int) es.fields.section_size);
      fprintf(stderr, "Start Offset: %x\n",
          (unsigned int) es.fields.start_offset);
      fprintf(stderr, "Unknown: %x\n", (unsigned int) es.fields.unknown);
      fprintf(stderr, "Checksum 1: %x\n", (unsigned int) es.fields.checksum_1);
      fprintf(stderr, "Checksum 2: %x\n", (unsigned int) es.fields.checksum_2);
      fprintf(stderr, "???: %x\n", (unsigned int) es.fields.unknown_2);
    }

  system_section ss;

  fprintf(stderr, "\n\nRaw system section bytes:\n");
  for (i = 0; i < 0x14; i++)
    {
      ss.data[i] = bit_read_RC(dat);
      fprintf(stderr, "%x ", ss.data[i]);
    }

  if (loglevel)
    {
      fprintf(stderr, "\n\n=== System Section ===\n");
      fprintf(stderr, "Section Type (should be 0x4163043b): %x\n",
          (unsigned int) ss.fields.section_type);
      fprintf(stderr, "DecompDataSize: %x\n",
          (unsigned int) ss.fields.DecompDataSize);
      fprintf(stderr, "CompDataSize: %x\n",
          (unsigned int) ss.fields.CompDataSize);
      fprintf(stderr, "Compression Type: %x\n",
          (unsigned int) ss.fields.compression_type);
      fprintf(stderr, "Checksum: %x\n\n", (unsigned int) ss.fields.checksum);
    }

  fprintf(stderr,
      "Decoding of DWG version R2004 header is not implemented yet.\n");
  return -1;
}

int
decode_R2007_header(Bit_Chain* dat, Dwg_Data * dwg)
{
  int i;
  unsigned long int preview_address, security_type, unknown_long,
      dwg_property_address, vba_proj_address, app_info_address;
  unsigned char sig, DwgVer, MaintReleaseVer;

  /* 5 bytes of 0x00 */
  dat->byte = 0x06;
  if (loglevel)
    fprintf(stderr, "5 bytes of 0x00: ");
  for (i = 0; i < 5; i++)
    {
      sig = bit_read_RC(dat);
      if (loglevel)
        fprintf(stderr, "0x%02X ", sig);
    }
  if (loglevel)
    fprintf(stderr, "\n");

  /* Unknown */
  dat->byte = 0x0B;
  sig = bit_read_RC(dat);
  if (loglevel)
    fprintf(stderr, "Unknown: 0x%02X\n", sig);

  /* Byte 0x00, 0x01, or 0x03 */
  dat->byte = 0x0C;
  sig = bit_read_RC(dat);
  if (loglevel)
    fprintf(stderr, "Byte 0x00, 0x01, or 0x03: 0x%02X\n", sig);

  /* Preview Address */
  dat->byte = 0x0D;
  preview_address = bit_read_RL(dat);
  if (loglevel)
    fprintf(stderr, "Preview Address: 0x%08X\n", (unsigned int) preview_address);

  /* DwgVer */
  dat->byte = 0x11;
  DwgVer = bit_read_RC(dat);
  if (loglevel)
    fprintf(stderr, "DwgVer: %u\n", DwgVer);

  /* MaintReleaseVer */
  dat->byte = 0x12;
  MaintReleaseVer = bit_read_RC(dat);
  if (loglevel)
    fprintf(stderr, "MaintRelease: %u\n", MaintReleaseVer);

  /* Codepage */
  dat->byte = 0x13;
  dwg->header.codepage = bit_read_RS(dat);
  if (loglevel)
    fprintf(stderr, "Codepage: %u\n", dwg->header.codepage);

  /* Unknown */
  dat->byte = 0x15;
  if (loglevel)
    fprintf(stderr, "Unknown: ");
  for (i = 0; i < 3; i++)
    {
      sig = bit_read_RC(dat);
      if (loglevel)
        fprintf(stderr, "0x%02X ", sig);
    }
  if (loglevel)
    fprintf(stderr, "\n");

  /* SecurityType */
  dat->byte = 0x18;
  security_type = bit_read_RL(dat);
  if (loglevel)
    fprintf(stderr, "SecurityType: 0x%08X\n", (unsigned int) security_type);

  /* Unknown long */
  dat->byte = 0x1C;
  unknown_long = bit_read_RL(dat);
  if (loglevel)
    fprintf(stderr, "Unknown long: 0x%08X\n", (unsigned int) unknown_long);

  /* DWG Property Addr */
  dat->byte = 0x20;
  dwg_property_address = bit_read_RL(dat);
  if (loglevel)
    fprintf(stderr, "DWG Property Addr: 0x%08X\n",
        (unsigned int) dwg_property_address);

  /* VBA Project Addr */
  dat->byte = 0x24;
  vba_proj_address = bit_read_RL(dat);
  if (loglevel)
    fprintf(stderr, "VBA Project Addr: 0x%08X\n",
        (unsigned int) vba_proj_address);

  /* 0x00000080 */
  dat->byte = 0x28;
  unknown_long = bit_read_RL(dat);
  if (loglevel)
    fprintf(stderr, "0x00000080: 0x%08X\n", (unsigned int) unknown_long);

  /* Application Info Address */
  dat->byte = 0x2C;
  app_info_address = bit_read_RL(dat);
  if (loglevel)
    fprintf(stderr, "Application Info Address: 0x%08X\n",
        (unsigned int) app_info_address);

  /* Reed-Solomon(255,239) encoded section */
  if (loglevel)
    fprintf(stderr, "Reed-Solomon(255,239) encoded section:\n\n");
  unsigned char solomon[0x3d8];
  dat->byte = 0x80;
  for (i = 0; i < 0x3d8; i++)
    {
      solomon[i] = bit_read_RC(dat);
      if (loglevel)
        fprintf(stderr, "%2x ", solomon[i]);
    }
  if (loglevel)
    fprintf(stderr, "\n\n");

  /////////////////////////////////////////
  //	incomplete implementation!
  /////////////////////////////////////////

  fprintf(stderr,
      "Decoding of DWG version R2007 header is not fully implemented yet.\n");
  return -1;
}

/*--------------------------------------------------------------------------------
 * Private functions
 */

static void
dwg_decode_entity(Bit_Chain * dat, Dwg_Object_Entity * ent)
{
  unsigned int i;
  unsigned int size;
  int error = 2;

  SINCE(R_2000)
    {
      ent->bitsize = bit_read_RL(dat);
    }

  error = bit_read_H(dat, &(ent->object->handle));
  if (error)
    {
      fprintf(
          stderr,
          "dwg_decode_entity:\tError in object handle! Current Bit_Chain address: 0x%0x\n",
          (unsigned int) dat->byte);
      ent->bitsize = 0;
      ent->extended_size = 0;
      ent->picture_exists = 0;
      ent->num_handles = 0;
      return;
    }

  fprintf(stderr, "this entity handle is %d.%d.%lu\n",
      ent->object->handle.code, ent->object->handle.size,
      ent->object->handle.value);

  ent->extended_size = 0;
  while (size = bit_read_BS(dat))
    {
      if (size > 10210)
        {
          fprintf(
              stderr,
              "dwg_decode_entity: Absurd! Extended object data size: %lu. Object: %lu (handle).\n",
              (long unsigned int) size, ent->object->handle.value);
          ent->bitsize = 0;
          ent->extended_size = 0;
          ent->picture_exists = 0;
          ent->num_handles = 0;
          return;
        }
      if (ent->extended_size == 0)
        {
          ent->extended = malloc(size);
          ent->extended_size = size;
        }
      else
        {
          ent->extended_size += size;
          ent->extended = realloc(ent->extended, ent->extended_size);
        }
      error = bit_read_H(dat, &ent->extended_handle);
      if (error)
        fprintf(stderr, "Ops...\n");
      for (i = ent->extended_size - size; i < ent->extended_size; i++)
        ent->extended[i] = bit_read_RC(dat);
    }
  ent->picture_exists = bit_read_B(dat);
  if (ent->picture_exists)
    {
      ent->picture_size = bit_read_RL(dat);
      if (ent->picture_size < 210210)
        {
          ent->picture = malloc(ent->picture_size);
          for (i = 0; i < ent->picture_size; i++)
            ent->picture[i] = bit_read_RC(dat);
        }
      else
        {
          fprintf(
              stderr,
              "dwg_decode_entity:  Absurd! Picture-size: %lu kB. Object: %lu (handle).\n",
              ent->picture_size / 1000, ent->object->handle.value);
          bit_advance_position(dat, -(4 * 8 + 1));
        }
    }

  VERSIONS(R_13,R_14)
    {
      ent->bitsize = bit_read_RL(dat);
    }

  ent->entity_mode = bit_read_BB(dat);
  ent->num_reactors = bit_read_BL(dat);

  SINCE(R_2004)
    {
      ent->xdict_missing_flag = bit_read_B(dat);
    }

  VERSIONS(R_13,R_14)
    {
      ent->isbylayerlt = bit_read_B(dat);
    }

  ent->nolinks = bit_read_B(dat);
  bit_read_CMC(dat, &ent->color);
  ent->linetype_scale = bit_read_BD(dat);

  SINCE(R_2000)
    {
      ent->linetype_flags = bit_read_BB(dat);
      ent->plotstyle_flags = bit_read_BB(dat);
    }

  SINCE(R_2007)
    {
      ent->material_flags = bit_read_BB(dat);
      ent->shadow_flags = bit_read_RC(dat);
    }

  ent->invisible = bit_read_BS(dat);

  SINCE(R_2000)
    {
      ent->lineweight = bit_read_RC(dat);
    }
}

static void
dwg_decode_object(Bit_Chain * dat, Dwg_Object_Object * ord)
{
  unsigned int i;
  unsigned int size;
  int error = 2;

  SINCE(R_2000)
    {
      ord->bitsize = bit_read_RL(dat);
    }

  error = bit_read_H(dat, &ord->object->handle);
  if (error)
    {
      fprintf(stderr,
          "\tError in object handle! Bit_Chain current address: 0x%0x\n",
          (unsigned int) dat->byte);
      ord->bitsize = 0;
      ord->extended_size = 0;
      ord->num_handles = 0;
      return;
    }
  ord->extended_size = 0;
  while (size = bit_read_BS(dat))
    {
      if (size > 10210)
        {
          fprintf(
              stderr,
              "dwg_decode_object: Absurd! Extended object data size: %lu. Object: %lu (handle).\n",
              (long unsigned int) size, ord->object->handle.value);
          ord->bitsize = 0;
          ord->extended_size = 0;
          ord->num_handles = 0;
          return;
        }
      if (ord->extended_size == 0)
        {
          ord->extended = malloc(size);
          ord->extended_size = size;
        }
      else
        {
          ord->extended_size += size;
          ord->extended = realloc(ord->extended, ord->extended_size);
        }
      error = bit_read_H(dat, &ord->extended_handle);
      if (error)
        fprintf(stderr, "Ops...\n");
      for (i = ord->extended_size - size; i < ord->extended_size; i++)
        ord->extended[i] = bit_read_RC(dat);
    }

  VERSIONS(R_13,R_14)
    {
      ord->bitsize = bit_read_RL(dat);
    }

  ord->num_reactors = bit_read_BL(dat);

  SINCE(R_2004)
    {
      ord->xdic_missing_flag = bit_read_B(dat);
    }

}

/**
 * Find a pointer to an object given it's id (handle)
 */
static Dwg_Object *
dwg_resolve_handle(Dwg_Data* dwg, unsigned long int absref)
{
  //FIXME find a faster algorithm
  int i;
  for (i = 0; i < dwg->num_objects; i++)
    if (dwg->object[i].handle.value == absref)
      return &dwg->object[i];
  return 0;
}

#define REFS_PER_REALLOC 100

static Dwg_Object_Ref *
dwg_decode_handleref(Bit_Chain * dat, Dwg_Object * obj)
{
  // Welcome to the house of evil code!
  Dwg_Object_Ref* ref = (Dwg_Object_Ref *) malloc(sizeof(Dwg_Object_Ref));
  Dwg_Data* dwg = obj->parent;

  if (bit_read_H(dat, &ref->handleref))
    {
      fprintf(stderr,
          "\tENTITY: Error reading handle in object whose handle is: %d.%d.%lu\n",
          obj->handle.code, obj->handle.size, obj->handle.value);
      free(ref);
      return 0;
    }

  //Reserve memory space for object references
  if (dwg->num_object_refs == 0)
    dwg->object_ref = (Dwg_Object_Ref **) malloc(REFS_PER_REALLOC * sizeof(Dwg_Object_Ref*));
  else
    if (dwg->num_object_refs % REFS_PER_REALLOC == 0)
      dwg->object_ref = (Dwg_Object_Ref **) realloc(dwg->object_ref,
          (dwg->num_object_refs + REFS_PER_REALLOC) * sizeof(Dwg_Object_Ref*));

  dwg->object_ref[dwg->num_object_refs++] = ref;

  /*
   * sometimes the code indicates the type of ownership
   * in other cases the handle is stored as an offset from some other handle
   * how is it determined?
   */
  ref->absolute_ref = 0;
  switch(ref->handleref.code) //that's right: don't bother the code on the spec.
    {
    case 0x06: //what if 6 means HARD_OWNER?
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
    default: //0x02, 0x03, 0x04, 0x05 or none
      ref->absolute_ref = ref->handleref.value;
      break;
    }

  ref->obj = 0;
  return ref;
}

static Dwg_Object_Ref *
dwg_decode_handleref_with_code(Bit_Chain * dat, Dwg_Object * obj,
    unsigned int code)
{
  Dwg_Object_Ref * ref;
  ref = dwg_decode_handleref(dat, obj);
  if (ref->absolute_ref == 0 && ref->handleref.code != code)
    {
      fprintf(stderr, "ERROR: expected a CODE %d handle\nERROR: ", code);
      //TODO: At the moment we are tolerating wrong codes in handles.
      // in the future we might want to get strict and return 0 here so that code will crash
      // whenever it reaches the first handle parsing error. This might make debugging easier.
      //return 0;
    }
  return ref;
}

static void
dwg_decode_header_variables(Bit_Chain * dat, Dwg_Object * obj)
{
  Dwg_Header_Variables header_vars;
  Dwg_Header_Variables* _obj = &header_vars;

  VERSION(R_2007)
    {
      FIELD_RL(bitsize);
    }

  FIELD_RL (bitsize);
  FIELD_BD (unknown_0);
  FIELD_BD (unknown_1);
  FIELD_BD (unknown_2);
  FIELD_BD (unknown_3);
  FIELD_TV (unknown_4);
  FIELD_TV (unknown_5);
  FIELD_TV (unknown_6);
  FIELD_TV (unknown_7);
  FIELD_BL (unknown_8);
  FIELD_BL (unknown_9);

  VERSIONS(R_13, R_14)
    {
      FIELD_BS(unknown_10);
    }

  PRE(R_2004)
    {
      FIELD_HANDLE (current_viewport_entity_header, ANYCODE);
    }

  FIELD_B (DIMASO);
  FIELD_B (DIMSHO);

  VERSIONS(R_13, R_14)
    {
      FIELD_B (DIMSAV); //undocumented
    }

  FIELD_B (PLINEGEN);
  FIELD_B (ORTHOMODE);
  FIELD_B (REGENMODE);
  FIELD_B (FILLMODE);
  FIELD_B (QTEXTMODE);
  FIELD_B (PSLTSCALE);
  FIELD_B (LIMCHECK);

  VERSIONS(R_13, R_14)
    {
      FIELD_B (BLIPMODE);
    }

  SINCE(R_2004)
    {
      FIELD_B (unknown_11); //undocumented
    }

  FIELD_B (user_timer_onoff);
  FIELD_B (SKPOLY);
  FIELD_B (ANGDIR);
  FIELD_B (SPLFRAME);

  VERSIONS(R_13, R_14)
    {
      FIELD_B (ATTREQ);
      FIELD_B (ATTDIA);
    }

  FIELD_B (MIRRTEXT);
  FIELD_B (WORLDVIEW);

  VERSIONS(R_13, R_14)
    {
      FIELD_B (WIREFRAME); //Undocumented
    }

  FIELD_B (TILEMODE);
  FIELD_B (PLIMCHECK);
  FIELD_B (VISRETAIN);

  VERSIONS(R_13, R_14)
    {
      FIELD_B (DELOBJ);
    }

  FIELD_B (DISPSILH);
  FIELD_B (PELLIPSE);

  VERSION(R_13)
    {
      FIELD_BS (SAVEIMAGES);
    }

  VERSIONS(R_14, R_2000)
    {
      FIELD_BS (PROXYGRAPHICS);
    }

  VERSIONS(R_13, R_14)
    {
      FIELD_BS (DRAGMODE);
    }

  FIELD_BS (TREEDEPTH);
  FIELD_BS (LUNITS);
  FIELD_BS (LUPREC);
  FIELD_BS (AUNITS);
  FIELD_BS (AUPREC);

  VERSIONS(R_13, R_14)
    {
      FIELD_BS (OSMODE);
    }

  FIELD_BS (ATTMODE);

  VERSIONS(R_13, R_14)
    {
      FIELD_BS (COORDS);
    }

  FIELD_BS (PDMODE);

  VERSIONS(R_13, R_14)
    {
      FIELD_BS (PICKSTYLE);
    }

  SINCE(R_2004)
    {
      FIELD_BL (unknown_12);
      FIELD_BL (unknown_13);
      FIELD_BL (unknown_14);
    }

  FIELD_BS (USERI1);
  FIELD_BS (USERI2);
  FIELD_BS (USERI3);
  FIELD_BS (USERI4);
  FIELD_BS (USERI5);
  FIELD_BS (SPLINESEGS);
  FIELD_BS (SURFU);
  FIELD_BS (SURFV);
  FIELD_BS (SURFTYPE);
  FIELD_BS (SURFTAB1);
  FIELD_BS (SURFTAB2);
  FIELD_BS (SPLINETYPE);
  FIELD_BS (SHADEDGE);
  FIELD_BS (SHADEDIF);
  FIELD_BS (UNITMODE);
  FIELD_BS (MAXACTVP);
  FIELD_BS (ISOLINES);
  FIELD_BS (CMLJUST);
  FIELD_BS (TEXTQLTY);
  FIELD_BD (LTSCALE);
  FIELD_BD (VTEXTSIZE);
  FIELD_BD (TRACEWID);
  FIELD_BD (SKETCHINC);
  FIELD_BD (FILLETRAD);
  FIELD_BD (THICKNESS);
  FIELD_BD (ANGBASE);
  FIELD_BD (PDSIZE);
  FIELD_BD (PLINEWID);
  FIELD_BD (USERR1);
  FIELD_BD (USERR2);
  FIELD_BD (USERR3);
  FIELD_BD (USERR4);
  FIELD_BD (USERR5);
  FIELD_BD (CHAMFERA);
  FIELD_BD (CHAMFERB);
  FIELD_BD (CHAMFERC);
  FIELD_BD (CHAMFERD);
  FIELD_BD (FACETRES);
  FIELD_BD (CMLSCALE);
  FIELD_BD (CELTSCALE);
  FIELD_TV (MENUNAME);
  FIELD_BL (TDCREATE_JULIAN_DAY);
  FIELD_BL (TDCREATE_MILLISECONDS);
  FIELD_BL (TDUPDATE_JULIAN_DAY);
  FIELD_BL (TDUPDATE_MILLISECONDS);

  SINCE(R_2004)
    {
      FIELD_BL (unknown_15);
      FIELD_BL (unknown_16);
      FIELD_BL (unknown_17);
    }

  FIELD_BL (TDINDWG_DAYS);
  FIELD_BL (TDINDWG_MILLISECONDS);
  FIELD_BL (TDUSRTIMER_DAYS);
  FIELD_BL (TDUSRTIMER_MILLISECONDS);
  FIELD_CMC (CECOLOR);
  FIELD_HANDLE (HANDSEED, ANYCODE);
  FIELD_HANDLE (CLAYER, ANYCODE);
  FIELD_HANDLE (TEXTSTYLE, ANYCODE);
  FIELD_HANDLE (CELTYPE, ANYCODE);

  SINCE(R_2007)
    {
      FIELD_HANDLE (CMATERIAL, ANYCODE);
    }

  FIELD_HANDLE (DIMSTYLE, ANYCODE);
  FIELD_HANDLE (CMLSTYLE, ANYCODE);

  SINCE(R_2000)
    {
      FIELD_BD (PSVPSCALE);
    }

  FIELD_3BD (INSBASE_PSPACE);
  FIELD_3BD (EXTMIN_PSPACE);
  FIELD_3BD (EXTMAX_PSPACE);
  FIELD_2RD (LIMMIN_PSPACE);
  FIELD_2RD (LIMMAX_PSPACE);
  FIELD_BD (ELEVATION_PSPACE);
  FIELD_3BD (UCSORG_PSPACE);
  FIELD_3BD (UCSXDIR_PSPACE);
  FIELD_3BD (UCSYDIR_PSPACE);
  FIELD_HANDLE (UCSNAME_PSPACE, ANYCODE);

  SINCE(R_2000)
    {
      FIELD_HANDLE (PUCSBASE, ANYCODE);
      FIELD_BS (PUCSORTHOVIEW);
      FIELD_HANDLE (PUCSORTHOREF, ANYCODE);
      FIELD_3BD (PUCSORGTOP);
      FIELD_3BD (PUCSORGBOTTOM);
      FIELD_3BD (PUCSORGLEFT);
      FIELD_3BD (PUCSORGRIGHT);
      FIELD_3BD (PUCSORGFRONT);
      FIELD_3BD (PUCSORGBACK);
    }

  FIELD_3BD (INSBASE_MSPACE);
  FIELD_3BD (EXTMIN_MSPACE);
  FIELD_3BD (EXTMAX_MSPACE);
  FIELD_2RD (LIMMIN_MSPACE);
  FIELD_2RD (LIMMAX_MSPACE);
  FIELD_BD (ELEVATION_MSPACE);
  FIELD_3BD (UCSORG_MSPACE);
  FIELD_3BD (UCSXDIR_MSPACE);
  FIELD_3BD (UCSYDIR_MSPACE);
  FIELD_HANDLE (UCSNAME_MSPACE, ANYCODE);

  SINCE(R_2000)
    {
      FIELD_HANDLE (UCSBASE, ANYCODE);
      FIELD_BS (UCSORTHOVIEW);
      FIELD_HANDLE (UCSORTHOREF, ANYCODE);
      FIELD_3BD (UCSORGTOP);
      FIELD_3BD (UCSORGBOTTOM);
      FIELD_3BD (UCSORGLEFT);
      FIELD_3BD (UCSORGRIGHT);
      FIELD_3BD (UCSORGFRONT);
      FIELD_3BD (UCSORGBACK);
      FIELD_TV (DIMPOST);
      FIELD_TV (DIMAPOST);
    }

  VERSIONS(R_13, R_14)
    {
      FIELD_B (DIMTOL);
      FIELD_B (DIMLIM);
      FIELD_B (DIMTIH);
      FIELD_B (DIMTOH);
      FIELD_B (DIMSE1);
      FIELD_B (DIMSE2);
      FIELD_B (DIMALT);
      FIELD_B (DIMTOFL);
      FIELD_B (DIMSAH);
      FIELD_B (DIMTIX);
      FIELD_B (DIMSOXD);
      FIELD_RC (DIMALTD);
      FIELD_RC (DIMZIN);
      FIELD_B (DIMSD1);
      FIELD_B (DIMSD2);
      FIELD_RC (DIMTOLJ);
      FIELD_RC (DIMJUST);
      FIELD_RC (DIMFIT);
      FIELD_B (DIMUPT);
      FIELD_RC (DIMTZIN);
      FIELD_RC (DIMMALTZ);
      FIELD_RC (DIMMALTTZ);
      FIELD_RC (DIMTAD);
      FIELD_BS (DIMUNIT);
      FIELD_BS (DIMAUNIT);
      FIELD_BS (DIMDEC);
      FIELD_BS (DIMTDEC);
      FIELD_BS (DIMALTU);
      FIELD_BS (DIMALTTD);
      FIELD_HANDLE (DIMTXSTY, ANYCODE);
    }

  FIELD_BD (DIMSCALE);
  FIELD_BD (DIMASZ);
  FIELD_BD (DIMEXO);
  FIELD_BD (DIMDLI);
  FIELD_BD (DIMEXE);
  FIELD_BD (DIMRND);
  FIELD_BD (DIMDLE);
  FIELD_BD (DIMMTP);
  FIELD_BD (DIMMTM);

  SINCE(R_2007)
    {
      FIELD_BD (DIMFXL);
      FIELD_BD (DIMJOGANG);
      FIELD_BS (DIMTFILL);
      FIELD_CMC (DIMTFILLCLR);
    }

  SINCE(R_2000)
    {
      FIELD_B (DIMTOL);
      FIELD_B (DIMLIM);
      FIELD_B (DIMTIH);
      FIELD_B (DIMTOH);
      FIELD_B (DIMSE1);
      FIELD_B (DIMSE2);
      FIELD_BS (DIMTAD);
      FIELD_BS (DIMZIN);
      FIELD_BS (DIMAZIN);
    }

  SINCE(R_2007)
    {
      FIELD_BS (DIMARCSYM);
    }

  FIELD_BD (DIMTXT);
  FIELD_BD (DIMCEN);
  FIELD_BD (DIMTSZ);
  FIELD_BD (DIMALTF);
  FIELD_BD (DIMLFAC);
  FIELD_BD (DIMTVP);
  FIELD_BD (DIMTFAC);
  FIELD_BD (DIMGAP);

  VERSIONS(R_13, R_14)
    {
      FIELD_T (DIMPOST_T);
      FIELD_T (DIMAPOST_T);
      FIELD_T (DIMBLK_T);
      FIELD_T (DIMBLK1_T);
      FIELD_T (DIMBLK2_T);
    }

  SINCE(R_2000)
    {
      FIELD_BD (DIMALTRND);
      FIELD_B (DIMALT);
      FIELD_BS (DIMALTD);
      FIELD_B (DIMTOFL);
      FIELD_B (DIMSAH);
      FIELD_B (DIMTIX);
      FIELD_B (DIMSOXD);
    }

  FIELD_CMC (DIMCLRD);
  FIELD_CMC (DIMCLRE);
  FIELD_CMC (DIMCLRT);

  SINCE(R_2000)
    {
      FIELD_BS (DIMADEC);
      FIELD_BS (DIMDEC);
      FIELD_BS (DIMALTU);
      FIELD_BS (DIMALTTD);
      FIELD_BS (DIMAUNIT);
      FIELD_BS (DIMFRAC);
      FIELD_BS (DIMLUNIT);
      FIELD_BS (DIMDSEP);
      FIELD_BS (DIMTMOVE);
      FIELD_BS (DIMJUST);
      FIELD_B (DIMSD1);
      FIELD_B (DIMSD2);
      FIELD_BS (DIMTOLJ);
      FIELD_BS (DIMTZIN);
      FIELD_BS (DIMALTZ);
      FIELD_BS (DIMALTTZ);
      FIELD_B (DIMUPT);
      FIELD_BS (DIMATFIT);
    }

  SINCE(R_2007)
    {
      FIELD_B (DIMFXLON);
    }

  SINCE(R_2000)
    {
      FIELD_HANDLE (DIMTXTSTY, ANYCODE);
      FIELD_HANDLE (DIMLDRBLK, ANYCODE);
      FIELD_HANDLE (DIMBLK, ANYCODE);
      FIELD_HANDLE (DIMBLK1, ANYCODE);
      FIELD_HANDLE (DIMBLK2, ANYCODE);
    }

  SINCE(R_2007)
    {
      FIELD_HANDLE (DIMLTYPE, ANYCODE);
      FIELD_HANDLE (DIMLTEX1, ANYCODE);
      FIELD_HANDLE (DIMLTEX2, ANYCODE);
    }

  SINCE(R_2000)
    {
      FIELD_HANDLE (DIMLWD, ANYCODE);
      FIELD_HANDLE (DIMLWE, ANYCODE);
    }

  FIELD_HANDLE (BLOCK_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (LAYER_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (STYLE_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (LINETYPE_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (VIEW_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (UCS_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (VPORT_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (APPID_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (DIMSTYLE_CONTROL_OBJECT, ANYCODE);

  VERSIONS(R_13, R_2000)
    {
      FIELD_HANDLE (VIEWPORT_ENTITY_HEADER_CONTROL_OBJECT, ANYCODE);
    }

  FIELD_HANDLE (DICTIONARY_ACAD_GROUP, ANYCODE);
  FIELD_HANDLE (DICTIONARY_ACAD_MLINESTYLE, ANYCODE);
  FIELD_HANDLE (DICTIONARY_NAMED_OBJECTS, ANYCODE);

  SINCE(R_2000)
    {
      FIELD_BS (unknown_18);
      FIELD_BS (unknown_19);
      FIELD_TV (HYPERLINKBASE);
      FIELD_TV (STYLESHEET);
      FIELD_HANDLE (DICTIONARY_LAYOUTS, ANYCODE);
      FIELD_HANDLE (DICTIONARY_PLOTSETTINGS, ANYCODE);
      FIELD_HANDLE (DICTIONARY_PLOTSTYLES, ANYCODE);
    }

  SINCE(R_2004)
    {
      FIELD_HANDLE (DICTIONARY_MATERIALS, ANYCODE);
      FIELD_HANDLE (DICTIONARY_COLORS, ANYCODE);
    }

  SINCE(R_2007)
    {
      FIELD_HANDLE (DICTIONARY_VISUALSTYLE, ANYCODE);
    }

  SINCE(R_2000)
    {
      FIELD_BL (FLAGS);
      FIELD_BS (INSUNITS);
      FIELD_BS (CEPSNTYPE);
      FIELD_HANDLE (CPSNID, ANYCODE);
      FIELD_TV (FINGERPRINTGUID);
      FIELD_TV (VERSIONGUID);
    }

  SINCE(R_2004)
    {
      FIELD_RC (SORTENTS);
      FIELD_RC (IDEXCTL);
      FIELD_RC (HIDETEXT);
      FIELD_RC (XCLIPFRAME);
      FIELD_RC (DIMASSOC);
      FIELD_RC (HALOGAP);
      FIELD_BS (OBSCUREDCOLOR);
      FIELD_BS (INTERSECTIONCOLOR);
      FIELD_RC (OBSCUREDLTYPE);
      FIELD_RC (INTERSECTIONDISPLAY);
      FIELD_TV (PROJECTNAME);
    }

  FIELD_HANDLE (BLOCK_RECORD_PAPER_SPACE, ANYCODE);
  FIELD_HANDLE (BLOCK_RECORD_MODEL_SPACE, ANYCODE);
  FIELD_HANDLE (LTYPE_BYLAYER, ANYCODE);
  FIELD_HANDLE (LTYPE_BYBLOCK, ANYCODE);
  FIELD_HANDLE (LTYPE_CONTINUOUS, ANYCODE);

  SINCE(R_2007)
    {
      FIELD_B (unknown_20);
      FIELD_BL (unknown_21);
      FIELD_BL (unknown_22);
      FIELD_BD (unknown_23);
      FIELD_BD (unknown_24);
      FIELD_BD (unknown_25);
      FIELD_BD (unknown_26);
      FIELD_BD (unknown_27);
      FIELD_BD (unknown_28);
      FIELD_RC (unknown_29);
      FIELD_RC (unknown_30);
      FIELD_BD (unknown_31);
      FIELD_BD (unknown_32);
      FIELD_BD (unknown_33);
      FIELD_BD (unknown_34);
      FIELD_BD (unknown_35);
      FIELD_BD (unknown_36);
      FIELD_BS (unknown_37);
      FIELD_RC (unknown_38);
      FIELD_BD (unknown_39);
      FIELD_BD (unknown_40);
      FIELD_BD (unknown_41);
      FIELD_BL (unknown_42);
      FIELD_RC (unknown_43);
      FIELD_RC (unknown_44);
      FIELD_RC (unknown_45);
      FIELD_RC (unknown_46);
      FIELD_B (unknown_47);
      FIELD_CMC (unknown_48);
      FIELD_HANDLE (unknown_49, ANYCODE);
      FIELD_HANDLE (unknown_50, ANYCODE);
      FIELD_HANDLE (unknown_51, ANYCODE);
      FIELD_RC (unknown_52);
      FIELD_BD (unknown_53);
    }

  SINCE(R_14)
    {
      FIELD_BS (unknown_54);
      FIELD_BS (unknown_55);
      FIELD_BS (unknown_56);
      FIELD_BS (unknown_57);
    }

  FIELD_RS (CRC);
}

static void
dwg_decode_common_entity_handle_data(Bit_Chain * dat, Dwg_Object * obj)
{

  Dwg_Object_Entity *ent;
  int i;
  ent = obj->tio.entity;

  //TODO: check what is the condition for the presence of this handle:
  //	ent->subentity_ref_handle = dwg_decode_handleref_with_code (dat, obj, 3);

  if (ent->num_reactors)
    ent->reactors = malloc(ent->num_reactors * sizeof(Dwg_Object_Ref*));
  for (i = 0; i < ent->num_reactors; i++)
    {
      ent->reactors[i] = dwg_decode_handleref_with_code(dat, obj, 4);
    }

  ent->xdicobjhandle = dwg_decode_handleref_with_code(dat, obj, 3);

  VERSIONS(R_13,R_14)
    {
      ent->layer = dwg_decode_handleref_with_code(dat, obj, 5);
      if (!ent->isbylayerlt)
        {
          ent->ltype = dwg_decode_handleref_with_code(dat, obj, 5);
        }
    }

  if (0)
    { //TODO: these are optional. Figure out what is the condition.
      ent->prev_entity = dwg_decode_handleref_with_code(dat, obj, 4);
      ent->next_entity = dwg_decode_handleref_with_code(dat, obj, 4);
    }

  SINCE(R_2000)
    {
      ent->layer = dwg_decode_handleref_with_code(dat, obj, 5);
      if (ent->linetype_flags == 3)
        {
          ent->ltype = dwg_decode_handleref_with_code(dat, obj, 5);
        }
      if (ent->plotstyle_flags == 3)
        {
          ent->plotstyle = dwg_decode_handleref_with_code(dat, obj, 5);
        }
    }

  if (dat->version >= R_2007 && ent->material_flags == 3)
    {
      ent->material = dwg_decode_handleref(dat, obj);
    }

}

/* OBJECTS *******************************************************************/

static void
dwg_decode_UNUSED(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(UNUSED);

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_TEXT(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY (TEXT);

  VERSIONS(R_13,R_14)
    {
      FIELD(elevation, BD);
      FIELD_2RD(insertion_pt);
      FIELD_2RD(alignment_pt);
      FIELD_3BD(extrusion);
      FIELD(thickness, BD);
      FIELD(oblique_ang, BD);
      FIELD(rotation_ang, BD);
      FIELD(height, BD);
      FIELD(width_factor, BD);
      FIELD(text_value, TV);
      FIELD(generation, BS);
      FIELD(horiz_alignment, BS);
      FIELD(vert_alignment, BS);
    }

  SINCE(R_2000)
    {
      FIELD(dataflags, RC);

      if (!(GET_FIELD(dataflags) & 0x01))
        {
          FIELD(elevation, RD);
        }

      FIELD_2RD(insertion_pt);

      if (!(GET_FIELD(dataflags) & 0x02))
        {
          FIELD_2DD(alignment_pt, 10, 20);
        }

      FIELD_BE(extrusion);
      FIELD(thickness, BT);

      if (!(GET_FIELD(dataflags) & 0x04))
        {
          FIELD(oblique_ang, RD);
        }

      if (!(GET_FIELD(dataflags) & 0x08))
        {
          FIELD(rotation_ang, RD);
        }

      FIELD(height, RD);

      if (!(GET_FIELD(dataflags) & 0x10))
        {
          FIELD(width_factor, RD);
        }

      FIELD(text_value, TV);

      if (!(GET_FIELD(dataflags) & 0x20))
        {
          FIELD(generation, BS);
        }

      if (!(GET_FIELD(dataflags) & 0x40)) 
        {
          FIELD(horiz_alignment, BS);
        }

      if (!(GET_FIELD(dataflags) & 0x80))
        {
          FIELD(vert_alignment, BS);
        }
    }

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE(style, 5);
}

static void
dwg_decode_ATTRIB(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(ATTRIB);

  VERSIONS(R_13,R_14)
    {
      FIELD(elevation, BD);
      FIELD_2RD(insertion_pt);
      FIELD_2RD(alignment_pt);
      FIELD_3BD(extrusion);
      FIELD(thickness, BD);
      FIELD(oblique_ang, BD);
      FIELD(rotation_ang, BD);
      FIELD(height, BD);
      FIELD(width_factor, BD);
      FIELD(text_value, TV);
      FIELD(generation, BS);
      FIELD(horiz_alignment, BS);
      FIELD(vert_alignment, BS);
    }

  SINCE(R_2000)
    {
      FIELD(dataflags, RC);
      if (!(GET_FIELD(dataflags) & 0x01))
        {
          FIELD(elevation, RD);
        }
      FIELD_2RD(insertion_pt);
      if (!(GET_FIELD(dataflags) & 0x02))
        {
          FIELD_2DD(alignment_pt, 10, 20);
        }
      FIELD_BE(extrusion);
      FIELD(thickness, BT);
      if (!(GET_FIELD(dataflags) & 0x04))
        {
          FIELD(oblique_ang, RD);
        }
      if (!(GET_FIELD(dataflags) & 0x08))
        {
          FIELD(rotation_ang, RD);
        }
      FIELD(height, RD);
      if (!(GET_FIELD(dataflags) & 0x10))
        {
          FIELD(width_factor, RD);
        }
      FIELD(text_value, TV);
      if (!(GET_FIELD(dataflags) & 0x20))
        {
          FIELD(generation, BS);
        }
      if (!(GET_FIELD(dataflags) & 0x40))
        {
          FIELD(horiz_alignment, BS);
        }
      if (!(GET_FIELD(dataflags) & 0x80))
        {
          FIELD(vert_alignment, BS);
        }
    }

  FIELD(tag, TV);
  FIELD(field_length, BS);
  FIELD(flags, RC);

  SINCE(R_2007)
    {
      FIELD(lock_position_flag, B);
    }

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE(style, 5);
}

static void
dwg_decode_ATTDEF(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(ATTDEF);

  VERSIONS(R_13,R_14)
    {
      FIELD(elevation, BD);
      FIELD_2RD(insertion_pt);
      FIELD_2RD(alignment_pt);
      FIELD_3BD(extrusion);
      FIELD(thickness, BD);
      FIELD(oblique_ang, BD);
      FIELD(rotation_ang, BD);
      FIELD(height, BD);
      FIELD(width_factor, BD);
      FIELD(default_value, TV);
      FIELD(generation, BS);
      FIELD(horiz_alignment, BS);
      FIELD(vert_alignment, BS);
    }

  SINCE(R_2000)
    {
      FIELD(dataflags, RC);
      if (!(GET_FIELD(dataflags) & 0x01))
        {
          FIELD(elevation, RD);
        }
      FIELD_2RD(insertion_pt);
      if (!(GET_FIELD(dataflags) & 0x02))
        {
          FIELD_2DD(alignment_pt, 10, 20);
        }
      FIELD_BE(extrusion);
      FIELD(thickness, BT);
      if (!(GET_FIELD(dataflags) & 0x04))
        {
          FIELD(oblique_ang, RD);
        }
      if (!(GET_FIELD(dataflags) & 0x08))
        {
          FIELD(rotation_ang, RD);
        }
      FIELD(height, RD);
      if (!(GET_FIELD(dataflags) & 0x10))
        {
          FIELD(width_factor, RD);
        }
      FIELD(default_value, TV);
      if (!(GET_FIELD(dataflags) & 0x20))
        {
          FIELD(generation, BS);
        }
      if (!(GET_FIELD(dataflags) & 0x40))
        {
          FIELD(horiz_alignment, BS);
        }
      if (!(GET_FIELD(dataflags) & 0x80))
        {
          FIELD(vert_alignment, BS);
        }
    }

  FIELD(tag, TV);
  FIELD(field_length, BS);
  FIELD(flags, RC);

  SINCE(R_2007)
    {
      FIELD(lock_position_flag, B);
    }

  FIELD(prompt, TV);

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE(style, 5);
}

static void
dwg_decode_BLOCK(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(BLOCK);

  FIELD(name, TV);

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_ENDBLK(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(ENDBLK);

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_SEQEND(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(SEQEND);

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_INSERT(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(INSERT);

  FIELD_3BD(ins_pt);

  VERSIONS(R_13,R_14)
    {
      FIELD_3BD(scale);
    }

  SINCE(R_2000)
    {
      FIELD(scale_flag, BB);
      if (GET_FIELD(scale_flag) == 3)
        {
          GET_FIELD(scale.x) = 1.0;
          GET_FIELD(scale.y) = 1.0;
          GET_FIELD(scale.z) = 1.0;
        }
      else if (GET_FIELD(scale_flag) == 1)
        {
          GET_FIELD(scale.x) = 1.0;
          FIELD_DD(scale.y, 1.0);
          FIELD_DD(scale.z, 1.0);
        }
      else if (GET_FIELD(scale_flag) == 2)
        {
          FIELD(scale.x, RD); 
          GET_FIELD(scale.y) = GET_FIELD(scale.x);
          GET_FIELD(scale.z) = GET_FIELD(scale.x);
        }
      else //if (GET_FIELD(scale_flag) == 0)
        {
          FIELD(scale.x, RD);
          FIELD_DD(scale.y, GET_FIELD(scale.x));
          FIELD_DD(scale.z, GET_FIELD(scale.x));
        }
    }

  FIELD(rotation_ang, BD);
  FIELD_3DPOINT(extrusion);
  FIELD(has_attribs, B);

  SINCE(R_2004)
    {
      FIELD(owned_obj_count, BL);
    }

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE(block_header, 5);

  //There is a typo in the spec. it says "R13-R200:".
  //I guess it means "R13-R2000:" (just like in MINSERT)
  VERSIONS(R_13,R_2000)
    {
    if(GET_FIELD(has_attribs))
      {
        FIELD_HANDLE(first_attrib, 4);
        FIELD_HANDLE(last_attrib, 4);
      }
    }

  //Spec typo? Spec says "2004:" but I think it should be "2004+:"
  // just like field owned_obj_count (AND just like in MINSERT)
  SINCE(R_2004)
    {
      HANDLE_VECTOR(attrib_handles, owned_obj_count, 4);
    }

  if (GET_FIELD(has_attribs))
    {
      FIELD_HANDLE(seqend, 3);
    }
}

static void
dwg_decode_MINSERT(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(MINSERT);

  FIELD_3BD(ins_pt);

  VERSIONS(R_13,R_14)
    {
      FIELD_3BD(scale);
    }

  SINCE(R_2000)
    {
      FIELD(scale_flag, BB);
      if (GET_FIELD(scale_flag) == 3)
        ent->scale.x = ent->scale.y = ent->scale.y = 1.0;
      else if (GET_FIELD(scale_flag) == 1)
        {
          GET_FIELD(scale.x) = 1.0;
          FIELD_DD(scale.y, 1.0);
          FIELD_DD(scale.z, 1.0);
        }
      else if (GET_FIELD(scale_flag) == 2)
        {
          FIELD(scale.x, RD);
          GET_FIELD(scale.y) = GET_FIELD(scale.x);
          GET_FIELD(scale.z) = GET_FIELD(scale.x);
        }
      else //if (GET_FIELD(scale_flag) == 0)
        {
          FIELD(scale.x, RD);
          FIELD_DD(scale.y, GET_FIELD(scale.x));
          FIELD_DD(scale.z, GET_FIELD(scale.x));
        }
    }

  FIELD(rotation_ang, BD);
  FIELD_3BD(extrusion);
  FIELD(has_attribs, B);

  SINCE(R_2004)
    {
      FIELD(owned_obj_count, BL);
    }

  FIELD(numcols, BS);
  FIELD(numrows, BS);
  FIELD(col_spacing, BD);
  FIELD(row_spacing, BD);

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE(block_header, 5);

  VERSIONS(R_13,R_2000)
    if (GET_FIELD(has_attribs))
      {
        FIELD_HANDLE(first_attrib, 4);
        FIELD_HANDLE(last_attrib, 4);
      }

  SINCE(R_2004)
    {
      HANDLE_VECTOR(attrib_handles, owned_obj_count, 4);
    }

  if (GET_FIELD(has_attribs))
    {
      FIELD_HANDLE(seqend, 3);
    }
}

static void
dwg_decode_VERTEX_2D(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(VERTEX_2D);

  FIELD(flags, RC);
  FIELD_3BD(point);
  FIELD(start_width, BD);
  if (GET_FIELD(start_width) < 0)
    {
      GET_FIELD(start_width) = -GET_FIELD(start_width);
      GET_FIELD(end_width) = GET_FIELD(start_width);
    }
  else
    {
      FIELD(end_width, BD);
    }
  FIELD(bulge, BD);
  FIELD(tangent_dir, BD);

  COMMON_ENTITY_HANDLE_DATA;

}

static void
dwg_decode_VERTEX_3D(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(VERTEX_3D);

  FIELD(flags, RC);
  FIELD_3BD(point);

  COMMON_ENTITY_HANDLE_DATA;

}

static void
dwg_decode_VERTEX_MESH(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(VERTEX_MESH);

  FIELD(flags, RC);
  FIELD_3BD(point);

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_VERTEX_PFACE(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(VERTEX_PFACE);

  FIELD(flags, RC);
  FIELD_3BD(point);

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_VERTEX_PFACE_FACE(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(VERTEX_PFACE_FACE);

  FIELD(vertind[0], BS);
  FIELD(vertind[1], BS);
  FIELD(vertind[2], BS);
  FIELD(vertind[3], BS);

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_POLYLINE_2D(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(POLYLINE_2D);

  FIELD(flags, BS);
  FIELD(curve_type, BS);
  FIELD(start_width, BD);
  FIELD(end_width, BD);
  FIELD(thickness, BT);
  FIELD(elevation, BD);
  FIELD_BE(extrusion);

  SINCE(R_2004)
    {
      FIELD(owned_obj_count, BL);
    }

  COMMON_ENTITY_HANDLE_DATA;

  VERSIONS(R_13,R_2000)
    {
      FIELD_HANDLE(first_vertex, 4);
      FIELD_HANDLE(last_vertex, 4);
    }

  SINCE(R_2004)
    {
      HANDLE_VECTOR(vertex, owned_obj_count, 4);
    }

  FIELD_HANDLE(seqend, 3);
}

static void
dwg_decode_POLYLINE_3D(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(POLYLINE_3D);

  FIELD(flags_1, RC);
  FIELD(flags_2, RC);

  SINCE(R_2004)
    {
      FIELD(owned_obj_count, BL);
    }

  COMMON_ENTITY_HANDLE_DATA;

  VERSIONS(R_13,R_2000)
    {
      FIELD_HANDLE(first_vertex, 4);
      FIELD_HANDLE(last_vertex, 4);
    }

  SINCE(R_2004)
    {
      HANDLE_VECTOR(vertex, owned_obj_count, 4);
    }

  FIELD_HANDLE(seqend, 3);
}

static void
dwg_decode_ARC(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(ARC);

  FIELD_3BD(center);
  FIELD(radius, BD);
  FIELD(thickness, BT);
  FIELD_BE(extrusion);
  FIELD(start_angle, BD);
  FIELD(end_angle, BD);

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_CIRCLE(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(CIRCLE);

  FIELD_3BD(center);
  FIELD(radius, BD);
  FIELD(thickness, BT);
  FIELD_BE(extrusion);

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_LINE(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(LINE);

  VERSIONS(R_13,R_14)
    {
      FIELD_3BD(start);
      FIELD_3BD(end);
    }

  SINCE(R_2000)
    {
      FIELD(Zs_are_zero, B);
      FIELD(start.x, RD);
      FIELD_DD(end.x, GET_FIELD(start.x));
      FIELD(start.y, RD);
      FIELD_DD(end.y, GET_FIELD(start.y));

      if (ent->Zs_are_zero)
        {
          GET_FIELD(start.z) = 0.0;
          GET_FIELD(end.z) = 0.0;
        }
      else
        {
          FIELD(start.z, RD);
          FIELD_DD(end.z, GET_FIELD(start.z));
        }
    }

  FIELD(thickness, BT);
  FIELD_BE(extrusion);

  COMMON_ENTITY_HANDLE_DATA;
}

/**
 * Macro for common DIMENSION declaration
 */
/*FIELD_BD(elevation.ecs_11); \ */
#define DIMENSION_COMMON_DECODE \
    FIELD_3BD(extrusion); \
    FIELD_2RD(text_midpt); \
    FIELD_BD(elevation.ecs_12); \
    FIELD_RC(flags_1); \
    FIELD_TV(user_text); \
    FIELD_BD(text_rot); \
    FIELD_BD(horiz_dir); \
    FIELD_3BD(ins_scale); \
    FIELD_BD(ins_rotation); \
    SINCE(R_2000) \
      { \
        FIELD_BS(attachment_point); \
        FIELD_BS(lspace_style); \
        FIELD_BD(lspace_factor); \
        FIELD_BD(act_measurement); \
      } \
    SINCE(R_2007) \
      { \
        FIELD_B(unknown); \
        FIELD_B(flip_arrow1); \
        FIELD_B(flip_arrow2); \
      }

static void
dwg_decode_DIMENSION_ORDINATE(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(DIMENSION_ORDINATE);

  DIMENSION_COMMON_DECODE;
  FIELD_2RD(_12_pt);
  FIELD_3BD(_10_pt);
  FIELD_3BD(_13_pt);
  FIELD_3BD(_14_pt);
  FIELD_RC(flags_2);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE(dimstyle, 5);
  FIELD_HANDLE(block, 5);
}

static void
dwg_decode_DIMENSION_LINEAR(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(DIMENSION_LINEAR);

  DIMENSION_COMMON_DECODE;
  FIELD_2RD(_12_pt);
  FIELD_3BD(_13_pt);
  FIELD_3BD(_14_pt);
  FIELD_3BD(_10_pt);
  FIELD_BD(ext_line_rot);
  FIELD_BD(dim_rot);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE(dimstyle, 5);
  FIELD_HANDLE(block, 5);
}

static void
dwg_decode_DIMENSION_ALIGNED(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(DIMENSION_ALIGNED);

  DIMENSION_COMMON_DECODE;
  FIELD_2RD(_12_pt);
  FIELD_3BD(_13_pt);
  FIELD_3BD(_14_pt);
  FIELD_3BD(_10_pt);
  FIELD_BD(ext_line_rot);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE(dimstyle, 5);
  FIELD_HANDLE(block, 5);
}

static void
dwg_decode_DIMENSION_ANG3PT(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(DIMENSION_ANG3PT);

  DIMENSION_COMMON_DECODE;
  FIELD_2RD(_12_pt);
  FIELD_3BD(_10_pt);
  FIELD_3BD(_13_pt);
  FIELD_3BD(_14_pt);
  FIELD_3BD(_15_pt);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE(dimstyle, 5);
  FIELD_HANDLE(block, 5);
}

static void
dwg_decode_DIMENSION_ANG2LN(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(DIMENSION_ANG2LN);

  DIMENSION_COMMON_DECODE;
  FIELD_2RD(_12_pt);
  FIELD_2RD(_16_pt);
  FIELD_3BD(_13_pt);
  FIELD_3BD(_14_pt);
  FIELD_3BD(_15_pt);
  FIELD_3BD(_10_pt);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE(dimstyle, 5);
  FIELD_HANDLE(block, 5);
}

static void
dwg_decode_DIMENSION_RADIUS(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(DIMENSION_RADIUS);

  DIMENSION_COMMON_DECODE;
  FIELD_2RD(_12_pt);
  FIELD_3BD(_10_pt);
  FIELD_3BD(_15_pt);
  FIELD_BD(leader_len);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE(dimstyle, 5);
  FIELD_HANDLE(block, 5);
}
static void
dwg_decode_DIMENSION_DIAMETER(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(DIMENSION_DIAMETER);

  DIMENSION_COMMON_DECODE;
  FIELD_2RD(_12_pt);
  FIELD_3BD(_15_pt);
  FIELD_3BD(_10_pt);
  FIELD_BD(leader_len);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE(dimstyle, 5);
  FIELD_HANDLE(block, 5);
}

static void
dwg_decode_POINT(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(POINT);

  FIELD_BD(x);
  FIELD_BD(y);
  FIELD_BD(z);
  FIELD_BT(thickness);
  FIELD_BE(extrusion);
  FIELD_BD(x_ang);

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_3DFACE(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(_3DFACE);

  VERSIONS(R_13,R_14)
    {
      FIELD_3BD(corner1);
      FIELD_3BD(corner2);
      FIELD_3BD(corner3);
      FIELD_3BD(corner4);
      FIELD_BS(invis_flags);
    }

  SINCE(R_2000)
    {
      FIELD_B(has_no_flags);
      FIELD_B(z_is_zero);
      FIELD_RD(corner1.x);
      FIELD_RD(corner1.y);
      if(GET_FIELD(z_is_zero))
        {
          GET_FIELD(corner1.z) = 0;
        }
      else
        {
          FIELD_RD(corner1.z);
        }
      FIELD_DD(corner2.x, GET_FIELD(corner1.x));
      FIELD_DD(corner2.y, GET_FIELD(corner1.y));
      FIELD_DD(corner2.z, GET_FIELD(corner1.z));
      FIELD_DD(corner3.x, GET_FIELD(corner2.x));
      FIELD_DD(corner3.y, GET_FIELD(corner2.y));
      FIELD_DD(corner3.z, GET_FIELD(corner2.z));
      FIELD_DD(corner4.x, GET_FIELD(corner3.x));
      FIELD_DD(corner4.y, GET_FIELD(corner3.y));
      FIELD_DD(corner4.z, GET_FIELD(corner3.z));
    }

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_POLYLINE_PFACE(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(POLYLINE_PFACE);

  FIELD_BS(numverts);
  FIELD_BS(numfaces);

  SINCE(R_2004)
    {
      FIELD_BL(owned_obj_count);
    }

  COMMON_ENTITY_HANDLE_DATA;

  VERSIONS(R_13, R_2000)
    {
      FIELD_HANDLE(first_vertex, 4);
      FIELD_HANDLE(last_vertex, 4);
    }
  SINCE(R_2004)
    {
      HANDLE_VECTOR(vertex, owned_obj_count, 4);
    }
  FIELD_HANDLE(seqend, 3);
}

static void
dwg_decode_POLYLINE_MESH(Bit_Chain * dat, Dwg_Object * obj)
{

  DWG_ENTITY(POLYLINE_MESH);

  FIELD_BS(flags);
  FIELD_BS(curve_type);
  FIELD_BS(m_vert_count);
  FIELD_BS(n_vert_count);
  FIELD_BS(m_density);
  FIELD_BS(n_density);

  SINCE(R_2004)
    {
      FIELD_BL(owned_obj_count);
    }

  COMMON_ENTITY_HANDLE_DATA;

  VERSIONS(R_13, R_2000)
    {
      FIELD_HANDLE(first_vertex, 4);
      FIELD_HANDLE(last_vertex, 4);
    }
  SINCE(R_2004)
    {
      HANDLE_VECTOR(vertex, owned_obj_count, 4);
    }
  FIELD_HANDLE(seqend, 3);
}

static void
dwg_decode_SOLID(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(SOLID);

  ent->thickness = bit_read_BT(dat);
  ent->corner1.z = ent->corner2.z = ent->corner3.z = ent->corner4.z
      = bit_read_BD(dat);
  ent->corner1.x = bit_read_RD(dat);
  ent->corner1.y = bit_read_RD(dat);
  ent->corner2.x = bit_read_RD(dat);
  ent->corner2.y = bit_read_RD(dat);
  ent->corner3.x = bit_read_RD(dat);
  ent->corner3.y = bit_read_RD(dat);
  ent->corner4.x = bit_read_RD(dat);
  ent->corner4.y = bit_read_RD(dat);
  bit_read_BE(dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_TRACE(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(TRACE);

  ent->thickness = bit_read_BT(dat);
  ent->corner1.z = ent->corner2.z = ent->corner3.z = ent->corner4.z
      = bit_read_BD(dat);
  ent->corner1.x = bit_read_RD(dat);
  ent->corner1.y = bit_read_RD(dat);
  ent->corner2.x = bit_read_RD(dat);
  ent->corner2.y = bit_read_RD(dat);
  ent->corner3.x = bit_read_RD(dat);
  ent->corner3.y = bit_read_RD(dat);
  ent->corner4.x = bit_read_RD(dat);
  ent->corner4.y = bit_read_RD(dat);
  bit_read_BE(dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_SHAPE(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(SHAPE);

  ent->ins_pt.x = bit_read_BD(dat);
  ent->ins_pt.y = bit_read_BD(dat);
  ent->ins_pt.z = bit_read_BD(dat);
  ent->scale = bit_read_BD(dat);
  ent->rotation = bit_read_BD(dat);
  ent->width_factor = bit_read_BD(dat);
  ent->oblique = bit_read_BD(dat);
  ent->thickness = bit_read_BD(dat);
  ent->shape_no = bit_read_BS(dat);
  ent->extrusion.x = bit_read_BD(dat);
  ent->extrusion.y = bit_read_BD(dat);
  ent->extrusion.z = bit_read_BD(dat);

  COMMON_ENTITY_HANDLE_DATA;
  ent->shapefile = HANDLE_CODE(5);
}

static void
dwg_decode_VIEWPORT(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(VIEWPORT);

  ent->center.x = bit_read_BD(dat);
  ent->center.y = bit_read_BD(dat);
  ent->center.z = bit_read_BD(dat);
  ent->width = bit_read_BD(dat);
  ent->height = bit_read_BD(dat);

  SINCE(R_2000)
    {
      ent->view_target.x = bit_read_BD(dat);
      ent->view_target.y = bit_read_BD(dat);
      ent->view_target.z = bit_read_BD(dat);
      ent->view_direction.x = bit_read_BD(dat);
      ent->view_direction.y = bit_read_BD(dat);
      ent->view_direction.z = bit_read_BD(dat);
      ent->view_twist_angle = bit_read_BD(dat);
      ent->view_height = bit_read_BD(dat);
      ent->lens_length = bit_read_BD(dat);
      ent->front_clip_z = bit_read_BD(dat);
      ent->back_clip_z = bit_read_BD(dat);
      ent->snap_angle = bit_read_BD(dat);
      ent->view_center.x = bit_read_RD(dat);
      ent->view_center.y = bit_read_RD(dat);
      ent->snap_base.x = bit_read_RD(dat);
      ent->snap_base.y = bit_read_RD(dat);
      ent->snap_spacing.x = bit_read_RD(dat);
      ent->snap_spacing.y = bit_read_RD(dat);
      ent->grid_spacing.x = bit_read_RD(dat);
      ent->grid_spacing.y = bit_read_RD(dat);
      ent->circle_zoom = bit_read_BS(dat);
    }

  SINCE(R_2007)
    {
      ent->grid_major = bit_read_BS(dat);
    }

  SINCE(R_2000)
    {
      ent->frozen_layer_count = bit_read_BL(dat);
      ent->status_flags = bit_read_BL(dat);
      ent->style_sheet = bit_read_TV(dat);
      ent->render_mode = bit_read_RC(dat);
      ent->ucs_at_origin = bit_read_B(dat);
      ent->ucs_per_viewport = bit_read_B(dat);
      ent->ucs_origin.x = bit_read_BD(dat);
      ent->ucs_origin.y = bit_read_BD(dat);
      ent->ucs_origin.z = bit_read_BD(dat);
      ent->ucs_x_axis.x = bit_read_BD(dat);
      ent->ucs_x_axis.y = bit_read_BD(dat);
      ent->ucs_x_axis.z = bit_read_BD(dat);
      ent->ucs_y_axis.x = bit_read_BD(dat);
      ent->ucs_y_axis.y = bit_read_BD(dat);
      ent->ucs_y_axis.z = bit_read_BD(dat);
      ent->ucs_elevation = bit_read_BD(dat);
      ent->ucs_ortho_view_type = bit_read_BS(dat);
    }

  SINCE(R_2004)
    {
      ent->shadeplot_mode = bit_read_BS(dat);
    }

  SINCE(R_2007)
    {
      ent->use_def_lights = bit_read_B(dat);
      ent->def_lighting_type = bit_read_RC(dat);
      ent->brightness = bit_read_BD(dat);
      ent->contrast = bit_read_BD(dat);
      bit_read_CMC(dat, &ent->ambient_light_color);
    }

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_ELLIPSE(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(ELLIPSE);

  ent->x0 = bit_read_BD(dat);
  ent->y0 = bit_read_BD(dat);
  ent->z0 = bit_read_BD(dat);
  ent->x1 = bit_read_BD(dat);
  ent->y1 = bit_read_BD(dat);
  ent->z1 = bit_read_BD(dat);
  ent->extrusion.x = bit_read_BD(dat);
  ent->extrusion.y = bit_read_BD(dat);
  ent->extrusion.z = bit_read_BD(dat);
  ent->axis_ratio = bit_read_BD(dat);
  ent->start_angle = bit_read_BD(dat);
  ent->end_angle = bit_read_BD(dat);

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_SPLINE(Bit_Chain * dat, Dwg_Object * obj)
{
  int i;
  DWG_ENTITY(SPLINE);

  ent->scenario = bit_read_BS(dat);
  if (ent->scenario != 1 && ent->scenario != 2)
    fprintf(stderr, "Error: unknown scenario %d", ent->scenario);

  ent->degree = bit_read_BS(dat);
  if (ent->scenario == 2)
    {
      ent->fit_tol = bit_read_BD(dat);
      ent->beg_tan_vec.x = bit_read_BD(dat);
      ent->beg_tan_vec.y = bit_read_BD(dat);
      ent->beg_tan_vec.z = bit_read_BD(dat);
      ent->end_tan_vec.x = bit_read_BD(dat);
      ent->end_tan_vec.y = bit_read_BD(dat);
      ent->end_tan_vec.z = bit_read_BD(dat);
      ent->num_fit_pts = bit_read_BS(dat);
      ent->fit_pts = (Dwg_Entity_SPLINE_point*) malloc(ent->num_fit_pts
          * sizeof(Dwg_Entity_SPLINE_point));
      for (i = 0; i < ent->num_fit_pts; i++)
        {
          ent->fit_pts[i].x = bit_read_BD(dat);
          ent->fit_pts[i].y = bit_read_BD(dat);
          ent->fit_pts[i].z = bit_read_BD(dat);
        }
    }
  if (ent->scenario == 1)
    {
      ent->rational = bit_read_B(dat);
      ent->closed_b = bit_read_B(dat);
      ent->periodic = bit_read_B(dat);
      ent->knot_tol = bit_read_BD(dat);
      ent->ctrl_tol = bit_read_BD(dat);
      ent->num_knots = bit_read_BL(dat);
      ent->num_ctrl_pts = bit_read_BL(dat);
      ent->weighted = bit_read_B(dat);

      ent->knots = malloc(ent->num_knots * sizeof(double));
      for (i = 0; i < ent->num_knots; i++)
        ent->knots[i] = bit_read_BD(dat);

      ent->ctrl_pts = malloc(ent->num_ctrl_pts * 3 * sizeof(unsigned int));
      for (i = 0; i < ent->num_ctrl_pts; i++)
        {
          ent->ctrl_pts[i].x = bit_read_BD(dat);
          ent->ctrl_pts[i].y = bit_read_BD(dat);
          ent->ctrl_pts[i].z = bit_read_BD(dat);
          if (ent->weighted)
            //TODO check what "D" means on spec.
            //assuming typo - should be BD
            ent->ctrl_pts[i].w = bit_read_BD(dat);
          else
            //assuming w=0 when not present.
            ent->ctrl_pts[i].w = 0;
        }
    }

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_RAY(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(RAY);

  ent->x0 = bit_read_BD(dat);
  ent->y0 = bit_read_BD(dat);
  ent->z0 = bit_read_BD(dat);
  ent->x1 = bit_read_BD(dat);
  ent->y1 = bit_read_BD(dat);
  ent->z1 = bit_read_BD(dat);

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_XLINE(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(XLINE);

  ent->x0 = bit_read_BD(dat);
  ent->y0 = bit_read_BD(dat);
  ent->z0 = bit_read_BD(dat);
  ent->x1 = bit_read_BD(dat);
  ent->y1 = bit_read_BD(dat);
  ent->z1 = bit_read_BD(dat);

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_DICTIONARY(Bit_Chain *dat, Dwg_Object *obj)
{
  DWG_OBJECT(DICTIONARY);

  FIELD(numitems, BS);

  VERSION(R_14)
    {
      FIELD(unknown_r14, RC);
    }

  SINCE(R_2000)
    {
      FIELD(cloning, BS);
      FIELD(hard_owner, RC);
    }

  if (GET_FIELD(numitems) > 10000)
    {
      fprintf(
          stderr,
          "Strange: dictionary with more than 10 thousand entries! Handle: %lu\n",
          obj->handle.value);
      return;
    }

  FIELD_VECTOR(text, TV, numitems);
  FIELD_HANDLE(parenthandle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(itemhandles, numitems, 2);
}

static void
dwg_decode_MTEXT(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_ENTITY(MTEXT);

  //spec-typo ? Spec says BD but we think it might be 3BD:
  FIELD_3BD(insertion_pt);
  FIELD_3BD(extrusion);
  FIELD_3BD(x_axis_dir);

  SINCE(R_2007)
    {
      FIELD(rect_height, BD);
    }

  FIELD(rect_width, BD);
  FIELD(text_height, BD);
  FIELD(attachment, BS);
  FIELD(drawing_dir, BS);
  FIELD(extends_ht, BD); //not documented
  FIELD(extends_wid, BD);
  FIELD(text, TV);

  SINCE(R_2000)
    {
      FIELD(linespace_style, BS);
      FIELD(linespace_factor, BD);
      FIELD(unknown_bit, B);
    }

  SINCE(R_2004)
    {
      FIELD(unknown_long, BL);
    }

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE(style, 5);
}


static void
dwg_decode_LEADER(Bit_Chain *dat, Dwg_Object *obj)
{
  DWG_ENTITY(LEADER);

  FIELD(unknown_bit_1, B);
  FIELD(annot_type, BS);
  FIELD(path_type, BS);
  FIELD(numpts, BL);
  FIELD_3DPOINT_VECTOR(points, numpts);
  FIELD_3DPOINT(end_pt_proj);
  FIELD_3DPOINT(extrusion);
  FIELD_3DPOINT(x_direction);

  SINCE(R_14)
    {
      FIELD_3DPOINT(unknown_pt);
    }

  VERSIONS(R_13,R_14)
    {
      FIELD(dimgap, BD);
    }

  FIELD(box_height, BD);
  FIELD(box_width, BD);
  FIELD(hooklineonxdir, B);
  FIELD(arrowhead_on, B);

  VERSIONS(R_13,R_14)
    {
      FIELD(arrowhead_type, BS);
      FIELD(dimasz, BD);
      FIELD(unknown_bit_2, B);
      FIELD(unknown_bit_3, B);
      FIELD(unknown_short_1, BS);
      FIELD(byblock_color, BS);
      FIELD(unknown_bit_4, B);
      FIELD(unknown_bit_5, B);
    }

  SINCE(R_2000)
    {
      FIELD(unknown_short_1, BS);
      FIELD(unknown_bit_4, B);
      FIELD(unknown_bit_5, B);
    }

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_TOLERANCE(Bit_Chain *dat, Dwg_Object *obj)
{
  DWG_ENTITY(TOLERANCE);

  VERSIONS(R_13, R_14)
    {
      FIELD(unknown_short, BS); //spec-typo? Spec says S instead of BS.
      FIELD(height, BD);
      FIELD(dimgap, BD);
    }

  FIELD_3DPOINT(ins_pt);
  FIELD_3DPOINT(x_direction);
  FIELD_3DPOINT(extrusion);
  FIELD(text_string, BS);

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_MLINE(Bit_Chain *dat, Dwg_Object *obj)
{
  DWG_ENTITY(MLINE);

  FIELD(scale, BD);
  FIELD(just, RC); //spec-typo? Spec says EC instead of RC...
  FIELD_3DPOINT(base_point);
  FIELD_3DPOINT(extrusion);
  FIELD(open_closed, BS);
  FIELD(num_lines, RC);
  FIELD(num_verts, BS);

  int i, j, k;
  GET_FIELD(verts) = malloc(GET_FIELD(num_verts) * sizeof(Dwg_Entity_MLINE_vert));
  for (i = 0; i < GET_FIELD(num_verts); i++)
    {
      FIELD_3DPOINT(verts[i].vertex);
      FIELD_3DPOINT(verts[i].vertex_direction);
      FIELD_3DPOINT(verts[i].miter_direction);
      GET_FIELD(verts[i].lines) = malloc(GET_FIELD(num_lines) * sizeof(Dwg_Entity_MLINE_line));
      for (j = 0; j < GET_FIELD(num_lines); j++)
        {
          FIELD(verts[i].lines[j].num_segparms, BS);
          GET_FIELD(verts[i].lines[j].segparms) = malloc(
              GET_FIELD(verts[i].lines[j].num_segparms) * sizeof(double));
          for (k = 0; k < GET_FIELD(verts[i].lines[j].num_segparms); k++)
            {
              FIELD(verts[i].lines[j].segparms[k], BD);
            }
          FIELD(verts[i].lines[j].num_areafillparms, BS);
          GET_FIELD(verts[i].lines[j].areafillparms) = malloc(
              GET_FIELD(verts[i].lines[j].num_areafillparms) * sizeof(double));
          for (k = 0; k < GET_FIELD(verts[i].lines[j].num_areafillparms); k++)
            {
              FIELD(verts[i].lines[j].areafillparms[k], BD);
            }
        }
    }

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_BLOCK_CONTROL(Bit_Chain *dat, Dwg_Object *obj)
{
  DWG_OBJECT(BLOCK_CONTROL);

  FIELD(num_entries, BS);
  FIELD_HANDLE(null_handle, 4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(block_headers, num_entries, 2);
  FIELD_HANDLE(model_space,3);
  FIELD_HANDLE(paper_space,3);
}

static void
dwg_decode_BLOCK_HEADER(Bit_Chain *dat, Dwg_Object *obj)
{
  int i;
  DWG_OBJECT(BLOCK_HEADER);

  FIELD(entry_name, TV);
  FIELD(_64_flag, B);
  FIELD(xrefindex_plus1, BS);
  FIELD(xdep, B);
  FIELD(anonymous, B);
  FIELD(hasattrs, B);
  FIELD(blkisxref, B);
  FIELD(xrefoverlaid, B);

  SINCE(R_2000)
    {
      FIELD(loaded_bit, B);
    }

  SINCE(R_2004)
    {
      FIELD(owned_object_count, BL);
    }

  FIELD_3DPOINT(base_pt);
  FIELD(xref_pname, TV);

  SINCE(R_2000)
    {

      //skip non-zero bytes and a terminating zero:
      GET_FIELD(insert_count)=0;
      while (bit_read_RC(dat))
        {
          GET_FIELD(insert_count)++;
        };

      FIELD(block_description, TV);

      FIELD(size_of_preview_data, BL);
      FIELD_VECTOR(binary_preview_data, RC, size_of_preview_data);
    }

  SINCE(R_2007)
    {
      FIELD(insert_units, BS);
      FIELD(explodable, B);
      FIELD(block_scaling, RC);
    }

  FIELD_HANDLE(block_control_handle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_HANDLE(NULL_handle, 5);
  FIELD_HANDLE(block_entity, 3);

  VERSIONS(R_13,R_2000)
    {
      if (!GET_FIELD(blkisxref) && !GET_FIELD(xrefoverlaid))
        {
          FIELD_HANDLE(first_entity, 4);
          FIELD_HANDLE(last_entity, 4);
        }
    }

  SINCE(R_2004)
    {
      HANDLE_VECTOR(entities, owned_object_count, 4);
    }

  FIELD_HANDLE(endblk_entity, 3);

  SINCE(R_2000)
    {
      HANDLE_VECTOR(insert_handles, insert_count, ANYCODE)
      FIELD_HANDLE(layout_handle, ANYCODE);
    }
}

static void
dwg_decode_LAYER_CONTROL(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_OBJECT(LAYER_CONTROL);

  FIELD(num_entries, BS);
  FIELD_HANDLE(null_handle, 4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(layers, num_entries, 2);
}

static void
dwg_decode_LAYER(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_OBJECT(LAYER);

  FIELD(entry_name, TV);
  FIELD(_64_flag, B);
  FIELD(xrefindex_plus1, BS);
  FIELD(xrefdep, B);
  VERSIONS(R_13, R_14)
    {
      FIELD(frozen, B);
      FIELD(on, B);
      FIELD(frozen_in_new, B);
      FIELD(locked, B);
    }

  SINCE(R_2000)
    {
      FIELD(values, BS);      
    }

  FIELD_CMC(color);

  FIELD_HANDLE(layer_control, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_HANDLE(null_handle, 5);

  SINCE(R_2000)
    {
      FIELD_HANDLE(plotstyle, 5);
    }

  SINCE(R_2007)
    {
      FIELD_HANDLE(material, ANYCODE);
    }

  FIELD_HANDLE(linetype, 5);
}


static void
dwg_decode_SHAPEFILE_CONTROL(Bit_Chain *dat, Dwg_Object *obj)
{
  DWG_OBJECT(SHAPEFILE_CONTROL);

  FIELD(num_entries, BS);
  FIELD_HANDLE(null_handle, 4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(shapefiles, num_entries, 2);
}

static void
dwg_decode_SHAPEFILE(Bit_Chain *dat, Dwg_Object *obj)
{
  DWG_OBJECT(SHAPEFILE);

  FIELD(entry_name, TV);
  FIELD(_64_flag, B);
  FIELD(xrefindex_plus1, BS);
  FIELD(xrefdep, B);
  FIELD(vertical, B);
  FIELD(shape_file, B);
  FIELD(fixed_height, BD);
  FIELD(width_factor, BD);
  FIELD(oblique_ang, BD);
  FIELD(generation, RC);
  FIELD(last_height, BD);
  FIELD(font_name, TV);
  FIELD(bigfont_name, TV);
  FIELD_HANDLE(shapefile_control, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_HANDLE(null_handle, 5);
}

static void
dwg_decode_LTYPE_CONTROL(Bit_Chain *dat, Dwg_Object *obj)
{
  DWG_OBJECT(LTYPE_CONTROL);

  FIELD(num_entries, BS);
  FIELD_HANDLE(null_handle, 4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(linetypes, num_entries, 2);
  FIELD_HANDLE(bylayer, 3);
  FIELD_HANDLE(byblock, 3);
}

static void
dwg_decode_LTYPE(Bit_Chain *dat, Dwg_Object *obj)
{
  char R2007plus_text_area_is_present = 0;
  DWG_OBJECT(LTYPE);

  FIELD(entry_name, TV);
  FIELD(_64_flag, B);
  FIELD(xrefindex_plus1, BS);
  FIELD(xrefdep, B);
  FIELD(description, TV);
  FIELD(pattern_len, BD);
  FIELD(alignment, RC);
  FIELD(num_dashes, RC);

  int i;
  GET_FIELD(dash) = (LTYPE_dash*) malloc(GET_FIELD(num_dashes) * sizeof(LTYPE_dash));
  for (i=0; i<GET_FIELD(num_dashes);i++)
    {
      FIELD(dash[i].length, BD);
      FIELD(dash[i].complex_shapecode, BS);
      FIELD(dash[i].x_offset, RD);
      FIELD(dash[i].y_offset, RD);
      FIELD(dash[i].scale, BD);
      FIELD(dash[i].rotation, BD);
      FIELD(dash[i].shape_flag, BS);
      if (GET_FIELD(dash[i].shape_flag) & 0x02)
        R2007plus_text_area_is_present = 1;
    }

  UNTIL(R_2004)
    {
      FIELD_VECTOR_N(strings_area, RC, 256);
    }
  LATER_VERSIONS
    {
      if (R2007plus_text_area_is_present)
        {
          FIELD_VECTOR_N(strings_area, RC, 512);
        }
    }

  FIELD_HANDLE(linetype_control, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_HANDLE(null_handle, 5);
  HANDLE_VECTOR(shapefiles, num_dashes, 5);
}

static void
dwg_decode_VIEW_CONTROL(Bit_Chain *dat, Dwg_Object *obj)
{
  DWG_OBJECT(VIEW_CONTROL);

  FIELD(num_entries, BS);
  FIELD_HANDLE(null_handle, 4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(views, num_entries, 2);
}

static void
dwg_decode_VIEW(Bit_Chain *dat, Dwg_Object *obj)
{
  DWG_OBJECT(VIEW);

  FIELD(entry_name, TV);
  FIELD(_64_flag, B);
  FIELD(xrefindex_plus1, BS);
  FIELD(xrefdep, B);
  FIELD(height, BD);
  FIELD(width, BD);
  FIELD_2RD(center);
  FIELD_3BD(target);
  FIELD_3BD(direction);
  FIELD(twist_angle, BD);
  FIELD(lens_legth, BD);
  FIELD(front_clip, BD);
  FIELD(back_clip, BD);
  FIELD(view_mode, RC); //??? 4bits

  SINCE(R_2000)
    {
      FIELD(render_mode, RC);
    }

  FIELD(pspace_flag, B);

  SINCE(R_2000)
    {
      FIELD(associated_ucs, B);
      FIELD_3BD(origin);
      FIELD_3BD(x_direction);
      FIELD_3BD(y_direction);
      FIELD(elevation, BD);
      FIELD(orthographic_view_type, BS);
    }

  SINCE(R_2007)
    {
      FIELD(camera_plottable, B);
    }

  FIELD_HANDLE(view_control_handle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_HANDLE(null_handle, 5);

  SINCE(R_2000)
    {
      FIELD_HANDLE(base_ucs_handle, ANYCODE);
      FIELD_HANDLE(named_ucs_handle, ANYCODE); 
    }

  SINCE(R_2007)
    {
      FIELD_HANDLE(live_section, ANYCODE);
    }
}

static void
dwg_decode_UCS_CONTROL(Bit_Chain *dat, Dwg_Object *obj)
{
  DWG_OBJECT(UCS_CONTROL);

  FIELD(num_entries, BS);
  FIELD_HANDLE(null_handle, 4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(ucs, num_entries, 2);
}

static void
dwg_decode_UCS(Bit_Chain *dat, Dwg_Object *obj)
{
  DWG_OBJECT(UCS);

  FIELD(entry_name, TV);
  FIELD(_64_flag, B);
  FIELD(xrefindex_plus1, BS);
  FIELD(xrefdep, B);
  FIELD_3BD(x_direction);
  FIELD_3BD(y_direction);

  SINCE(R_2000)
    {
      FIELD(elevation, BD);
      FIELD(orthographic_view_type, BS);
      FIELD(orthographic_type, BS);
    }

  FIELD_HANDLE(ucs_control_handle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_HANDLE(null_handle, 5);

  SINCE(R_2000)
    {
      FIELD_HANDLE(base_ucs_handle, ANYCODE);
      FIELD_HANDLE(unknown, ANYCODE); 
    }
}

static void
dwg_decode_VP_ENT_HDR(Bit_Chain *dat, Dwg_Object *obj)
{
  DWG_OBJECT(VP_ENT_HDR);

  FIELD(entry_name, TV);
  FIELD(_64_flag, B);
  FIELD(xrefindex_plus1, BS);
  FIELD(xrefdep, B);
  FIELD(one_flag, B);
  FIELD_HANDLE(vp_ent_ctrl, ANYCODE);
  XDICOBJHANDLE(3);
  FIELD_HANDLE(null, 5);
}

static void
dwg_decode_MLINESTYLE(Bit_Chain *dat, Dwg_Object *obj)
{
  DWG_OBJECT(MLINESTYLE);

  FIELD(name, TV);
  FIELD(desc, TV);
  FIELD(flags, BS);
  FIELD_CMC(fillcolor);
  FIELD(startang, BD);
  FIELD(endang, BD);
  FIELD(linesinstyle, RC);

  //XXX Ugly! We must find a better way to handle arbitrary vectors
  REPEAT(linesinstyle, lines, Dwg_Object_MLINESTYLE_line)
  {
    FIELD(lines[vector_counter].offset, BD);
    FIELD_CMC(lines[vector_counter].color);
    FIELD(lines[vector_counter].ltindex, BS);
  }
  FIELD_HANDLE(parenthandle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
}


static void
dwg_decode_IMAGE(Bit_Chain *dat, Dwg_Object *obj)
{
  int i;
  DWG_ENTITY(IMAGE);

  FIELD(class_version, BL);
  FIELD_3DPOINT(pt0);
  FIELD_3DPOINT(uvec);
  FIELD_3DPOINT(vvec);
  FIELD(size.width, RD);
  FIELD(size.height, RD);
  FIELD(display_props, BS);
  FIELD(clipping, B);
  FIELD(brightness, RC);
  FIELD(contrast, RC);
  FIELD(fade, RC);
  FIELD(clip_boundary_type, BS);
  if (GET_FIELD(clip_boundary_type) == 1)
    {
      FIELD_2RD(boundary_pt0);
      FIELD_2RD(boundary_pt1);
    }
  else
    {
      FIELD(num_clip_verts, BL);
      ent->clip_verts = (Dwg_Entity_IMAGE_clip_vert*) malloc(
          ent->num_clip_verts * sizeof(Dwg_Entity_IMAGE_clip_vert));
      for (i = 0; i < ent->num_clip_verts; i++)
        {
          FIELD_2RD(clip_verts[i]);
        }
    }

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_LAYOUT(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_OBJECT(LAYOUT);

  FIELD(page_setup_name, TV);
  FIELD(printer_or_config, TV);
  FIELD(plot_layout_flags, BS);
  FIELD(left_margin, BD);
  FIELD(bottom_margin, BD);
  FIELD(right_margin, BD);
  FIELD(top_margin, BD);
  FIELD(paper_width, BD);
  FIELD(paper_height, BD);
  FIELD(paper_size, TV);
  FIELD_2BD(plot_origin);
  FIELD(paper_units, BS);
  FIELD(plot_rotation, BS);
  FIELD(plot_type, BS);
  FIELD_2BD(window_min);
  FIELD_2BD(window_max);

  VERSIONS(R_13,R_2000)
    {
      FIELD(plot_view_name, TV);
    }

  FIELD(real_world_units, BD);
  FIELD(drawing_units, BD);
  FIELD(current_style_sheet, TV);
  FIELD(scale_type, BS);
  FIELD(scale_factor, BD);
  FIELD_2BD(paper_image_origin);

  SINCE(R_2004)
    {
      FIELD(shade_plot_mode, BS);
      FIELD(shade_plot_res_level, BS);
      FIELD(shade_plot_custom_dpi, BS);
    }

  FIELD(layout_name, TV);
  FIELD(tab_order, BS);
  FIELD(flags, BS);
  FIELD_3DPOINT(ucs_origin);
  FIELD_2RD(minimum_limits);
  FIELD_2RD(maximum_limits);
  FIELD_3DPOINT(ins_point);
  FIELD_3DPOINT(ucs_x_axis);
  FIELD_3DPOINT(ucs_y_axis);
  FIELD(elevation, BD);
  FIELD(orthoview_type, BS);
  FIELD_3DPOINT(extent_min);
  FIELD_3DPOINT(extent_max);

  SINCE(R_2004)
    {
      FIELD(viewport_count, RL);
    }

  FIELD_HANDLE(parenthandle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);

  SINCE(R_2004)
    {
      FIELD_HANDLE(plot_view_handle, ANYCODE);
    }

  SINCE(R_2007)
    {
      FIELD_HANDLE(visual_style_handle, ANYCODE);
    }

  FIELD_HANDLE(associated_paperspace_block_record_handle, ANYCODE);
  FIELD_HANDLE(last_active_viewport_handle, ANYCODE);
  FIELD_HANDLE(base_ucs_handle, ANYCODE);
  FIELD_HANDLE(named_ucs_handle, ANYCODE);

  SINCE(R_2004)
    {
      HANDLE_VECTOR(viewport_handles, viewport_count, ANYCODE);
    }
}

static void
dwg_decode_LWPLINE(Bit_Chain * dat, Dwg_Object * obj)
{
  int i;
  DWG_ENTITY(LWPLINE);

  ent->flags = bit_read_BS(dat);
  if (ent->flags & 4)
    ent->const_width = bit_read_BD(dat);
  if (ent->flags & 8)
    ent->elevation = bit_read_BD(dat);
  if (ent->flags & 2)
    ent->thickness = bit_read_BD(dat);
  if (ent->flags & 1)
    {
      ent->normal.x = bit_read_BD(dat);
      ent->normal.y = bit_read_BD(dat);
      ent->normal.z = bit_read_BD(dat);
    }
  ent->num_points = bit_read_BL(dat);
  if (ent->flags & 16)
    ent->num_bulges = bit_read_BL(dat);
  if (ent->flags & 32)
    ent->num_widths = bit_read_BL(dat);

  ent->points = (Dwg_Entity_LWPLINE_point*) malloc(ent->num_points
      * sizeof(Dwg_Entity_LWPLINE_point));

  VERSIONS(R_13,R_14)
    {
      for (i = 0; i < ent->num_points; i++)
        {
          ent->points[i].x = bit_read_RD(dat);
          ent->points[i].y = bit_read_RD(dat);
        }
    }

  SINCE(R_2000)
    {
      ent->points[0].x = bit_read_RD(dat);
      ent->points[0].y = bit_read_RD(dat);
      for (i = 1; i < ent->num_points; i++)
        {
          ent->points[i].x = bit_read_DD(dat, ent->points[i - 1].x);
          ent->points[i].y = bit_read_DD(dat, ent->points[i - 1].y);
        }
    }

  ent->bulges = (double*) malloc(ent->num_bulges * sizeof(double));
  for (i = 0; i < ent->num_bulges; i++)
    ent->bulges[i] = bit_read_BD(dat);

  ent->widths = (Dwg_Entity_LWPLINE_width*) malloc(ent->num_widths
      * sizeof(Dwg_Entity_LWPLINE_width));
  for (i = 0; i < ent->num_widths; i++)
    {
      ent->widths[i].start = bit_read_BD(dat);
      ent->widths[i].end = bit_read_BD(dat);
    }

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_OLE2FRAME(Bit_Chain * dat, Dwg_Object * obj)
{
  int i;
  DWG_ENTITY(OLE2FRAME);

  ent->flags = bit_read_BS(dat);

  SINCE(R_2000)
    {
      ent->mode = bit_read_BS(dat);
    }

  ent->data_length = bit_read_BL(dat);
  ent->data = (unsigned char*) malloc(ent->data_length * sizeof(unsigned char));
  for (i = 0; i < ent->data_length; i++)
    ent->data[i] = bit_read_RC(dat);

  SINCE(R_2000)
    {
      ent->unknown = bit_read_RC(dat);
    }

  COMMON_ENTITY_HANDLE_DATA;
}

static void
dwg_decode_PLACEHOLDER(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_OBJECT(PLACEHOLDER);

  //TODO: Implement-me!

  //dwg_decode_handleref (dat, object);
}

static void
dwg_decode_DICTIONARYVAR(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_OBJECT(DICTIONARYVAR);

  FIELD(intval, RC);
  FIELD(str, BS);

  FIELD_HANDLE(parenthandle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
}

static void
dwg_decode_WIPEOUTVARIABLE(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_OBJECT(WIPEOUTVARIABLE);

  //TODO: Implement-me!

  //dwg_decode_handleref (dat, object);
}

static void
dwg_decode_IMAGEDEF(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_OBJECT(IMAGEDEF);

  //TODO: Implement-me!

  //dwg_decode_handleref (dat, object);
}

static void
dwg_decode_RASTERVARIABLES(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_OBJECT(RASTERVARIABLES);

  //TODO: Implement-me!

  //dwg_decode_handleref (dat, object);
}

static void
dwg_decode_SPATIAL_INDEX(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_OBJECT(SPATIAL_INDEX);

  //TODO: Implement-me!

  //dwg_decode_handleref (dat, object);
}

static void
dwg_decode_XRECORD(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_OBJECT(XRECORD);

  //TODO: Implement-me!

  //dwg_decode_handleref (dat, object);
}

static void
dwg_decode_SPATIAL_FILTER(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_OBJECT(SPATIAL_FILTER);

  //TODO: Implement-me!

  //dwg_decode_handleref (dat, object);
}

static void
dwg_decode_LAYER_INDEX(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_OBJECT(LAYER_INDEX);

  //TODO: Implement-me!

  //dwg_decode_handleref (dat, object);
}

static void
dwg_decode_DICTIONARYWDLFT(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_OBJECT(DICTIONARYWDLFT);

  //TODO: Implement-me!

  //dwg_decode_handleref (dat, object);
}

static void
dwg_decode_IMAGEDEFREACTOR(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_OBJECT(IMAGEDEFREACTOR);

  //TODO: Implement-me!

  //dwg_decode_handleref (dat, object);
}

static void
dwg_decode_IDBUFFER(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_OBJECT(IDBUFFER);

  //TODO: Implement-me!

  //dwg_decode_handleref (dat, object);
}

static void
dwg_decode_HATCH(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_OBJECT(HATCH);

  //TODO: Implement-me!

  //dwg_decode_handleref (dat, object);
}

static void
dwg_decode_VBA_PROJECT(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_OBJECT(VBA_PROJECT);

  //TODO: Implement-me!

  //dwg_decode_handleref (dat, object);
}

static void
dwg_decode_SORTENTSTABLE(Bit_Chain * dat, Dwg_Object * obj)
{
  DWG_OBJECT(SORTENTSTABLE);

  //TODO: Implement-me!

  //dwg_decode_handleref (dat, object);
}

static int
dwg_decode_variable_type(Dwg_Data * dwg, Bit_Chain * dat, Dwg_Object* obj)
{
  if ((obj->type - 500) > dwg->num_classes)
    return 1;
  int i = obj->type - 500;

  if (!strcmp(dwg->class[i].dxfname, "DICTIONARYVAR"))
    {
      dwg_decode_DICTIONARYVAR(dat, obj);
      return 0;
    }
  if (!strcmp(dwg->class[i].dxfname, "DICTIONARYWDFLT"))
    {
      dwg_decode_DICTIONARYWDLFT(dat, obj);
      return 0;
    }
  if (!strcmp(dwg->class[i].dxfname, "HATCH"))
    {
      dwg_decode_HATCH(dat, obj);
      return 0;
    }
  if (!strcmp(dwg->class[i].dxfname, "IDBUFFER"))
    {
      dwg_decode_IDBUFFER(dat, obj);
      return 0;
    }
  if (!strcmp(dwg->class[i].dxfname, "IMAGE"))
    {
      dwg_decode_IMAGE(dat, obj);
      return 0;
    }
  if (!strcmp(dwg->class[i].dxfname, "IMAGEDEF"))
    {
      dwg_decode_IMAGEDEF(dat, obj);
      return 0;
    }
  if (!strcmp(dwg->class[i].dxfname, "IMAGEDEFREACTOR"))
    {
      dwg_decode_IMAGEDEFREACTOR(dat, obj);
      return 0;
    }
  if (!strcmp(dwg->class[i].dxfname, "LAYER_INDEX"))
    {
      dwg_decode_LAYER_INDEX(dat, obj);
      return 0;
    }
  if (!strcmp(dwg->class[i].dxfname, "LAYOUT"))
    {
      dwg_decode_LAYOUT(dat, obj);
      return 0;
    }
  if (!strcmp(dwg->class[i].dxfname, "LWPLINE"))
    {
      dwg_decode_LWPLINE(dat, obj);
      return 0;
    }
  if (!strcmp(dwg->class[i].dxfname, "OLE2FRAME"))
    {
      dwg_decode_OLE2FRAME(dat, obj);
      return 0;
    }
  if (!strcmp(dwg->class[i].dxfname, "PLACEHOLDER"))
    {
      dwg_decode_PLACEHOLDER(dat, obj);
      return 0;
    }
  if (!strcmp(dwg->class[i].dxfname, "RASTERVARIABLES"))
    {
      dwg_decode_RASTERVARIABLES(dat, obj);
      return 0;
    }
  if (!strcmp(dwg->class[i].dxfname, "SORTENTSTABLE"))
    {
      dwg_decode_SORTENTSTABLE(dat, obj);
      return 0;
    }
  if (!strcmp(dwg->class[i].dxfname, "SPATIAL_FILTER"))
    {
      dwg_decode_SPATIAL_FILTER(dat, obj);
      return 0;
    }
  if (!strcmp(dwg->class[i].dxfname, "SPATIAL_INDEX"))
    {
      dwg_decode_SPATIAL_INDEX(dat, obj);
      return 0;
    }
  if (!strcmp(dwg->class[i].dxfname, "VBA_PROJECT"))
    {
      dwg_decode_VBA_PROJECT(dat, obj);
      return 0;
    }
  if (!strcmp(dwg->class[i].dxfname, "WIPEOUTVARIABLE"))
    {
      dwg_decode_WIPEOUTVARIABLE(dat, obj);
      return 0;
    }
  if (!strcmp(dwg->class[i].dxfname, "XRECORD"))
    {
      dwg_decode_XRECORD(dat, obj);
      return 0;
    }

  return 1;
}

/*--------------------------------------------------------------------------------
 * Private functions, which depend from the preceding
 */
static void
dwg_decode_add_object(Dwg_Data * dwg, Bit_Chain * dat,
    long unsigned int address)
{
  long unsigned int previous_address;
  long unsigned int object_address;
  unsigned char previous_bit;
  Dwg_Object *obj;

  /* Keep the previous address
   */
  previous_address = dat->byte;
  previous_bit = dat->bit;

  /* Use the indicated address for the object
   */
  dat->byte = address;
  dat->bit = 0;

  /*
   * Reserve memory space for objects
   */
  if (dwg->num_objects == 0)
    dwg->object = (Dwg_Object *) malloc(sizeof(Dwg_Object));
  else
    dwg->object = (Dwg_Object *) realloc(dwg->object, (dwg->num_objects + 1)
        * sizeof(Dwg_Object));

  fprintf(stderr, "\n\n======================\nObject number: %lu",
      dwg->num_objects);

  obj = &dwg->object[dwg->num_objects];
  obj->index = dwg->num_objects;
  dwg->num_objects++;

  obj->handle.code = 0;
  obj->handle.size = 0;
  obj->handle.value = 0;

  obj->parent = dwg;
  obj->size = bit_read_MS(dat);
  object_address = dat->byte;
  ktl_lastaddress = dat->byte + obj->size; /* (calculate the bitsize) */
  obj->type = bit_read_BS(dat);
  fprintf(stderr, " Type: %d\n", obj->type);

  /* Check the type of the object
   */
  switch (obj->type)
    {
  case DWG_TYPE_TEXT:
    dwg_decode_TEXT(dat, obj);
    break;
  case DWG_TYPE_ATTRIB:
    dwg_decode_ATTRIB(dat, obj);
    break;
  case DWG_TYPE_ATTDEF:
    dwg_decode_ATTDEF(dat, obj);
    break;
  case DWG_TYPE_BLOCK:
    dwg_decode_BLOCK(dat, obj);
    break;
  case DWG_TYPE_ENDBLK:
    dwg_decode_ENDBLK(dat, obj);
    break;
  case DWG_TYPE_SEQEND:
    dwg_decode_SEQEND(dat, obj);
    break;
  case DWG_TYPE_INSERT:
    dwg_decode_INSERT(dat, obj);
    break;
  case DWG_TYPE_MINSERT:
    dwg_decode_MINSERT(dat, obj);
    break;
  case DWG_TYPE_VERTEX_2D:
    dwg_decode_VERTEX_2D(dat, obj);
    break;
  case DWG_TYPE_VERTEX_3D:
    dwg_decode_VERTEX_3D(dat, obj);
    break;
  case DWG_TYPE_VERTEX_MESH:
    dwg_decode_VERTEX_MESH(dat, obj);
    break;
  case DWG_TYPE_VERTEX_PFACE:
    dwg_decode_VERTEX_PFACE(dat, obj);
    break;
  case DWG_TYPE_VERTEX_PFACE_FACE:
    dwg_decode_VERTEX_PFACE_FACE(dat, obj);
    break;
  case DWG_TYPE_POLYLINE_2D:
    dwg_decode_POLYLINE_2D(dat, obj);
    break;
  case DWG_TYPE_POLYLINE_3D:
    dwg_decode_POLYLINE_3D(dat, obj);
    break;
  case DWG_TYPE_ARC:
    dwg_decode_ARC(dat, obj);
    break;
  case DWG_TYPE_CIRCLE:
    dwg_decode_CIRCLE(dat, obj);
    break;
  case DWG_TYPE_LINE:
    dwg_decode_LINE(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_ORDINATE:
    dwg_decode_DIMENSION_ORDINATE(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_LINEAR:
    dwg_decode_DIMENSION_LINEAR(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_ALIGNED:
    dwg_decode_DIMENSION_ALIGNED(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_ANG3PT:
    dwg_decode_DIMENSION_ANG3PT(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_ANG2LN:
    dwg_decode_DIMENSION_ANG2LN(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_RADIUS:
    dwg_decode_DIMENSION_RADIUS(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_DIAMETER:
    dwg_decode_DIMENSION_DIAMETER(dat, obj);
    break;
  case DWG_TYPE_POINT:
    dwg_decode_POINT(dat, obj);
    break;
  case DWG_TYPE__3DFACE:
    dwg_decode_3DFACE(dat, obj);
    break;
  case DWG_TYPE_POLYLINE_PFACE:
    dwg_decode_POLYLINE_PFACE(dat, obj);
    break;
  case DWG_TYPE_POLYLINE_MESH:
    dwg_decode_POLYLINE_MESH(dat, obj);
    break;
  case DWG_TYPE_SOLID:
    dwg_decode_SOLID(dat, obj);
    break;
  case DWG_TYPE_TRACE:
    dwg_decode_TRACE(dat, obj);
    break;
  case DWG_TYPE_SHAPE:
    dwg_decode_SHAPE(dat, obj);
    break;
  case DWG_TYPE_VIEWPORT:
    dwg_decode_VIEWPORT(dat, obj);
    break;
  case DWG_TYPE_ELLIPSE:
    dwg_decode_ELLIPSE(dat, obj);
    break;
  case DWG_TYPE_SPLINE:
    dwg_decode_SPLINE(dat, obj);
    break;
  case DWG_TYPE_RAY:
    dwg_decode_RAY(dat, obj);
    break;
  case DWG_TYPE_XLINE:
    dwg_decode_XLINE(dat, obj);
    break;
  case DWG_TYPE_MTEXT:
    dwg_decode_MTEXT(dat, obj);
    break;
  case DWG_TYPE_LEADER:
    dwg_decode_LEADER(dat, obj);
    break;
  case DWG_TYPE_TOLERANCE:
    dwg_decode_TOLERANCE(dat, obj);
    break;
  case DWG_TYPE_MLINE:
    dwg_decode_MLINE(dat, obj);
    break;
  case DWG_TYPE_BLOCK_CONTROL:
    dwg_decode_BLOCK_CONTROL(dat, obj);
    break;
  case DWG_TYPE_DICTIONARY:
    dwg_decode_DICTIONARY(dat, obj);
    break;
  case DWG_TYPE_LAYER_CONTROL:
    dwg_decode_LAYER_CONTROL(dat, obj);
    break;
  case DWG_TYPE_LAYER:
    dwg_decode_LAYER(dat, obj);
    break;
  case DWG_TYPE_SHAPEFILE_CONTROL:
    dwg_decode_SHAPEFILE_CONTROL(dat, obj);
    break;
  case DWG_TYPE_SHAPEFILE:
    dwg_decode_SHAPEFILE(dat, obj);
    break;
  case DWG_TYPE_LTYPE_CONTROL:
    dwg_decode_LTYPE_CONTROL(dat, obj);
    break;
  case DWG_TYPE_LTYPE:
    dwg_decode_LTYPE(dat, obj);
    break;
  case DWG_TYPE_VIEW_CONTROL:
    dwg_decode_VIEW_CONTROL(dat, obj);
    break;
  case DWG_TYPE_VIEW:
    dwg_decode_VIEW(dat, obj);
    break;
  case DWG_TYPE_UCS_CONTROL:
    dwg_decode_UCS_CONTROL(dat, obj);
    break;
  case DWG_TYPE_UCS:
    dwg_decode_UCS(dat, obj);
    break;
  case DWG_TYPE_VP_ENT_HDR:
    dwg_decode_VP_ENT_HDR(dat, obj);
    break;
  case DWG_TYPE_MLINESTYLE:
    dwg_decode_MLINESTYLE(dat, obj);
    break;
  case DWG_TYPE_BLOCK_HEADER:
    dwg_decode_BLOCK_HEADER(dat, obj);
    break;
  default:
    if (obj->type == dwg->dwg_ot_layout)
      dwg_decode_LAYOUT(dat, obj);
    else
      {
        if (!dwg_decode_variable_type(dwg, dat, obj))
          {
            obj->supertype = DWG_SUPERTYPE_UNKNOWN;
            obj->tio.unknown = malloc(obj->size);
            memcpy(obj->tio.unknown, &dat->chain[object_address], obj->size);
          }
      }
    }

  /*
   if (obj->supertype != DWG_SUPERTYPE_UNKNOWN)
   {
   fprintf (stderr, " Begin address:\t%10lu\n", address);
   fprintf (stderr, " Last address:\t%10lu\tSize: %10lu\n", dat->byte, obj->size);
   fprintf (stderr, "End address:\t%10lu (calculated)\n", address + 2 + obj->size);
   }
   */

  /* Register the previous addresses for return
   */
  dat->byte = previous_address;
  dat->bit = previous_bit;
}

