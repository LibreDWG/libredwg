/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * decode.c: decoding functions
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Till Heuschmann
 */

#include "config.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "decode.h"
#include "print.h"

extern unsigned int
bit_ckr8(unsigned int dx, unsigned char *adr, long n);

/* The logging level for the read (decode) path.  */
static unsigned int loglevel;

#ifdef USE_TRACING
/* This flag means we have checked the environment variable
   LIBREDWG_TRACE and set `loglevel' appropriately.  */
static bool env_var_checked_p;

#define DWG_LOGLEVEL loglevel
#endif  /* USE_TRACING */

#include "logging.h"

#define REFS_PER_REALLOC 100

/*--------------------------------------------------------------------------------
 * Welcome to the dark side of the moon...
 * MACROS
 */

#define IS_DECODER

#define FIELD(name,type)\
  _obj->name = bit_read_##type(dat);\
  LOG_TRACE(#name ": " FORMAT_##type "\n", _obj->name)

#define FIELD_VALUE(name) _obj->name

#define ANYCODE -1
#define FIELD_HANDLE(name, handle_code)\
  if (handle_code>=0)\
    {\
      _obj->name = dwg_decode_handleref_with_code(dat, obj, dwg, handle_code);\
    }\
  else\
    {\
      _obj->name = dwg_decode_handleref(dat, obj, dwg);\
    }\
  LOG_TRACE(#name ": HANDLE(%d.%d.%lu) absolute:%lu\n",\
        _obj->name->handleref.code,\
        _obj->name->handleref.size,\
        _obj->name->handleref.value,\
        _obj->name->absolute_ref)

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
#define FIELD_T FIELD_TV /*TODO: implement version dependant string fields */
#define FIELD_BT(name) FIELD(name, BT);
#define FIELD_4BITS(name) _obj->name = bit_read_4BITS(dat);

#define FIELD_BE(name) bit_read_BE(dat, &_obj->name.x, &_obj->name.y, &_obj->name.z);
#define FIELD_DD(name, _default) FIELD_VALUE(name) = bit_read_DD(dat, _default);
#define FIELD_2DD(name, d1, d2) FIELD_DD(name.x, d1); FIELD_DD(name.y, d2);
#define FIELD_2RD(name) FIELD(name.x, RD); FIELD(name.y, RD);
#define FIELD_2BD(name) FIELD(name.x, BD); FIELD(name.y, BD);
#define FIELD_3RD(name) FIELD(name.x, RD); FIELD(name.y, RD); FIELD(name.z, RD);
#define FIELD_3BD(name) FIELD(name.x, BD); FIELD(name.y, BD); FIELD(name.z, BD);
#define FIELD_3DPOINT(name) FIELD_3BD(name)
#define FIELD_CMC(name)\
    bit_read_CMC(dat, &_obj->name);\
    LOG_TRACE(#name ": index %d\n", _obj->name.index)

//FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size)\
  if (size>0)\
    {\
      _obj->name = (BITCODE_##type*) malloc(size * sizeof(BITCODE_##type));\
      for (vcount=0; vcount< size; vcount++)\
        {\
          _obj->name[vcount] = bit_read_##type(dat);\
          LOG_TRACE(#name "[%d]: " FORMAT_##type "\n", vcount, _obj->name[vcount])\
        }\
    }

#define FIELD_VECTOR(name, type, size) FIELD_VECTOR_N(name, type, _obj->size)

#define FIELD_2RD_VECTOR(name, size)\
  _obj->name = (BITCODE_2RD *) malloc(_obj->size * sizeof(BITCODE_2RD));\
  for (vcount=0; vcount< _obj->size; vcount++)\
    {\
      FIELD_2RD(name[vcount]);\
    }

#define FIELD_2DD_VECTOR(name, size)\
  _obj->name = (BITCODE_2RD *) malloc(_obj->size * sizeof(BITCODE_2RD));\
  FIELD_2RD(name[0]);\
  for (vcount = 1; vcount < _obj->size; vcount++)\
    {\
      FIELD_2DD(name[vcount], FIELD_VALUE(name[vcount - 1].x), FIELD_VALUE(name[vcount - 1].y));\
    }

#define FIELD_3DPOINT_VECTOR(name, size)\
  _obj->name = (BITCODE_3DPOINT *) malloc(_obj->size * sizeof(BITCODE_3DPOINT));\
  for (vcount=0; vcount< _obj->size; vcount++)\
    {\
      FIELD_3DPOINT(name[vcount]);\
    }

#define HANDLE_VECTOR_N(name, size, code)\
  FIELD_VALUE(name) = (BITCODE_H*) malloc(sizeof(BITCODE_H) * size);\
  for (vcount=0; vcount<size; vcount++)\
    {\
      FIELD_HANDLE(name[vcount], code);\
    }

#define HANDLE_VECTOR(name, sizefield, code) HANDLE_VECTOR_N(name, FIELD_VALUE(sizefield), code)

#define FIELD_XDATA(name, size)\
  _obj->name = dwg_decode_xdata(dat, _obj->size)

#define REACTORS(code)\
  FIELD_VALUE(reactors) = (BITCODE_H*) malloc(sizeof(BITCODE_H) * obj->tio.object->num_reactors);\
  for (vcount=0; vcount<obj->tio.object->num_reactors; vcount++)\
    {\
      FIELD_HANDLE(reactors[vcount], code);\
    }

#define ENT_REACTORS(code)\
  FIELD_VALUE(reactors) = (BITCODE_H*) malloc(sizeof(BITCODE_H) * obj->tio.entity->num_reactors);\
  for (vcount=0; vcount<obj->tio.entity->num_reactors; vcount++)\
    {\
      FIELD_HANDLE(reactors[vcount], code);\
    }

#define XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.object->xdic_missing_flag)\
        {\
          FIELD_HANDLE(xdicobjhandle, code);\
        }\
    }\
  PRIOR_VERSIONS\
    {\
      FIELD_HANDLE(xdicobjhandle, code);\
    }

#define ENT_XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.entity->xdic_missing_flag)\
        {\
          FIELD_HANDLE(xdicobjhandle, code);\
        }\
    }\
  PRIOR_VERSIONS\
    {\
      FIELD_HANDLE(xdicobjhandle, code);\
    }

#define REPEAT_N(times, name, type) \
  _obj->name = (type *) malloc(times * sizeof(type));\
  for (rcount=0; rcount<times; rcount++)

#define REPEAT(times, name, type) \
  _obj->name = (type *) malloc(_obj->times * sizeof(type));\
  for (rcount=0; rcount<_obj->times; rcount++)

#define REPEAT2(times, name, type) \
  _obj->name = (type *) malloc(_obj->times * sizeof(type));\
  for (rcount2=0; rcount2<_obj->times; rcount2++)

#define REPEAT3(times, name, type) \
  _obj->name = (type *) malloc(_obj->times * sizeof(type));\
  for (rcount3=0; rcount3<_obj->times; rcount3++)

//TODO unify REPEAT macros!

#define COMMON_ENTITY_HANDLE_DATA \
  dwg_decode_common_entity_handle_data(dat, obj)

#define DWG_ENTITY(token) \
static void \
 dwg_decode_##token (Bit_Chain * dat, Dwg_Object * obj)\
{\
  int vcount, rcount, rcount2, rcount3;\
  Dwg_Entity_##token *ent, *_obj;\
  Dwg_Data* dwg = obj->parent;\
  LOG_INFO("Entity " #token ":\n")\
  dwg->num_entities++;\
  obj->supertype = DWG_SUPERTYPE_ENTITY;\
  obj->tio.entity = (Dwg_Object_Entity*)malloc (sizeof (Dwg_Object_Entity));	\
  obj->tio.entity->tio.token = (Dwg_Entity_##token *)calloc (1, sizeof (Dwg_Entity_##token)); \
  ent = obj->tio.entity->tio.token;\
  _obj=ent;\
  obj->tio.entity->object = obj;\
  if (dwg_decode_entity (dat, obj->tio.entity)) return;\
  LOG_INFO("Entity handle: %d.%d.%lu\n",\
    obj->handle.code,\
    obj->handle.size,\
    obj->handle.value)

#define DWG_ENTITY_END }

#define DWG_OBJECT(token) static void  dwg_decode_ ## token (Bit_Chain * dat, Dwg_Object * obj) {\
  int vcount, rcount, rcount2, rcount3;\
  Dwg_Object_##token *_obj;\
  Dwg_Data* dwg = obj->parent;\
  LOG_INFO("Object " #token ":\n")\
  obj->supertype = DWG_SUPERTYPE_OBJECT;\
  obj->tio.object = (Dwg_Object_Object*)malloc (sizeof (Dwg_Object_Object));	\
  obj->tio.object->tio.token = (Dwg_Object_##token * ) calloc (1, sizeof (Dwg_Object_##token)); \
  obj->tio.object->object = obj;\
  if (dwg_decode_object (dat, obj->tio.object)) return;\
  _obj = obj->tio.object->tio.token;\
  LOG_INFO("Object handle: %d.%d.%lu\n",\
    obj->handle.code,\
    obj->handle.size,\
    obj->handle.value)

#define DWG_OBJECT_END }

/*--------------------------------------------------------------------------------
 * Private functions
 */

static void
dwg_decode_add_object(Dwg_Data * dwg, Bit_Chain * dat,
    long unsigned int address);

static Dwg_Object *
dwg_resolve_handle(Dwg_Data* dwg, unsigned long int handle);

static void
dwg_decode_header_variables(Bit_Chain* dat, Dwg_Data * dwg);

static void
resolve_objectref_vector(Dwg_Data * dwg);

int decode_R13_R15(Bit_Chain* dat, Dwg_Data * dwg); // froward
int read_r2007_meta_data(Bit_Chain *dat, Dwg_Data *dwg);

/*--------------------------------------------------------------------------------
 * Public variables
 */
long unsigned int ktl_lastaddress;

/*--------------------------------------------------------------------------------
 * Public function definitions
 */
int
dwg_decode_data(Bit_Chain * dat, Dwg_Data * dwg)
{
  char version[7];
  dwg->num_object_refs = 0;
  dwg->num_layers = 0;
  dwg->num_entities = 0;
  dwg->num_objects = 0;
  dwg->num_classes = 0;

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

  /* Version */
  dat->byte = 0;
  dat->bit = 0;
  strncpy(version, (const char *)dat->chain, 6);
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
      LOG_ERROR("Invalid or unimplemented version code! "
        "This file's version code is: %s\n", version)
      return -1;
    }
  dat->version = (Dwg_Version_Type)dwg->header.version;
  LOG_INFO("This file's version code is: %s\n", version)

  PRE(R_2000)
    {
      LOG_INFO(
          "WARNING: This version of LibreDWG is only capable of safely decoding version R2000 (code: AC1015) dwg-files.\n"
            "This file's version code is: %s Support for this version is still experimental."
            "It might crash or give you invalid output.\n", version)
      return decode_R13_R15(dat, dwg);
    }

  VERSION(R_2000)
    {
      return decode_R13_R15(dat, dwg);
    }

  VERSION(R_2004)
    {
      LOG_INFO(
          "WARNING: This version of LibreDWG is only capable of properly decoding version R2000 (code: AC1015) dwg-files.\n"
            "This file's version code is: %s\n This version is not yet actively developed."
            "It will probably crash and/or give you invalid output.\n", version)
      return decode_R2004(dat, dwg);
    }

  VERSION(R_2007)
    {
      LOG_INFO(
          "WARNING: This version of LibreDWG is only capable of properly decoding version R2000 (code: AC1015) dwg-files.\n"
            "This file's version code is: %s\n This version is not yet actively developed."
            "It will probably crash and/or give you invalid output.\n", version)
      return decode_R2007(dat, dwg);
    }

  //This line should not be reached!
  LOG_ERROR(
      "ERROR: LibreDWG does not support this version: %s.\n",
      version)
  return -1;
}

int
decode_R13_R15(Bit_Chain* dat, Dwg_Data * dwg)
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
  LOG_TRACE("Still unknown values: 6 'zeroes' and a 'one': ")
  for (i = 0; i < 7; i++)
    {
      sig = bit_read_RC(dat);
      LOG_TRACE("0x%02X ", sig)
    }
  LOG_TRACE("\n")

  /* Image Seeker */
  pvz = bit_read_RL(dat);
  LOG_TRACE("Image seeker: 0x%08X\n", (unsigned int) pvz)

  // unknown
  sig = bit_read_RC(dat);
  LOG_INFO("Version: %u\n", sig);
  sig = bit_read_RC(dat);
  LOG_INFO("Release: %u\n", sig);

  /* Codepage */
  dat->byte = 0x13;
  dwg->header.codepage = bit_read_RS(dat);
  LOG_INFO("Codepage: %u\n", dwg->header.codepage);

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
      dwg->header.section[i].number = bit_read_RC(dat);
      dwg->header.section[i].address = bit_read_RL(dat);
      dwg->header.section[i].size = bit_read_RL(dat);
    }

  // Check CRC-on
  ckr = bit_ckr8(0xc0c1, dat->chain, dat->byte);
  ckr2 = bit_read_RS(dat);
  if (ckr != ckr2)
    {
      printf("header crc todo ckr:%x ckr2:%x\n", ckr, ckr2);
      return 1;
    }

  if (bit_search_sentinel(dat, dwg_sentinel(DWG_SENTINEL_HEADER_END)))
    LOG_TRACE("\n=======> HEADER (end): %8X\n", (unsigned int) dat->byte)

  /*-------------------------------------------------------------------------
   * Unknown section 1
   */

  if (dwg->header.num_sections == 6)
    {
      LOG_TRACE("\n=======> UNKNOWN 1: %8X\n",
              (unsigned int) dwg->header.section[5].address)
      LOG_TRACE("         UNKNOWN 1 (end): %8X\n",
              (unsigned int) (dwg->header.section[5].address
                  + dwg->header.section[5].size))
      dat->byte = dwg->header.section[5].address;
      dwg->unknown1.size = DWG_UNKNOWN1_SIZE;
      dwg->unknown1.byte = dwg->unknown1.bit = 0;
      dwg->unknown1.chain = (unsigned char*)malloc(dwg->unknown1.size);
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
      LOG_TRACE("\n=======> PICTURE: %8X\n",
            (unsigned int) start_address - 16)
      if (bit_search_sentinel(dat, dwg_sentinel(DWG_SENTINEL_PICTURE_END)))
        {
          LOG_TRACE("         PICTURE (end): %8X\n",
                (unsigned int) dat->byte)
          dwg->picture.size = (dat->byte - 16) - start_address;
          dwg->picture.chain = (unsigned char *) malloc(dwg->picture.size);
          memcpy(dwg->picture.chain, &dat->chain[start_address],
              dwg->picture.size);
        }
      else
        dwg->picture.size = 0;
    }

  /*-------------------------------------------------------------------------
   * Header Variables
   */

  LOG_INFO("\n=======> Header Variables: %8X\n",
          (unsigned int) dwg->header.section[0].address)
  LOG_INFO("         Header Variables (end): %8X\n",
          (unsigned int) (dwg->header.section[0].address
              + dwg->header.section[0].size))
  dat->byte = dwg->header.section[0].address + 16;
  pvz = bit_read_RL(dat);
  LOG_TRACE("         Length: %lu\n", pvz)

  dat->bit = 0;

  dwg_decode_header_variables(dat, dwg);

  // Check CRC-on
  dat->byte = dwg->header.section[0].address + dwg->header.section[0].size - 18;
  dat->bit = 0;

  ckr = bit_read_RS(dat);
  ckr2 = bit_ckr8(0xc0c1, dat->chain + dwg->header.section[0].address + 16,
          dwg->header.section[0].size - 34);

  if (ckr != ckr2)
    {
      printf("section %d crc todo ckr:%x ckr2:%x\n",
              dwg->header.section[0].number, ckr, ckr2);
      return -1;
    }

  /*-------------------------------------------------------------------------
   * Classes
   */
  LOG_INFO("\n=======> CLASS: %8X\n",
          (unsigned int) dwg->header.section[1].address)
  LOG_INFO("         CLASS (end): %8X\n",
          (unsigned int) (dwg->header.section[1].address
              + dwg->header.section[1].size))
  dat->byte = dwg->header.section[1].address + 16;
  dat->bit = 0;

  size = bit_read_RL(dat);
  lasta = dat->byte + size;
  LOG_TRACE("         Length: %lu\n", size);

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
        dwg->dwg_class = (Dwg_Class *) malloc(sizeof(Dwg_Class));
      else
        dwg->dwg_class = (Dwg_Class *) realloc(dwg->dwg_class, (idc + 1)
            * sizeof(Dwg_Class));

      dwg->dwg_class[idc].number = bit_read_BS(dat);
      dwg->dwg_class[idc].version = bit_read_BS(dat);
      dwg->dwg_class[idc].appname = bit_read_TV(dat);
      dwg->dwg_class[idc].cppname = bit_read_TV(dat);
      dwg->dwg_class[idc].dxfname = bit_read_TV(dat);
      dwg->dwg_class[idc].wasazombie = bit_read_B(dat);
      dwg->dwg_class[idc].item_class_id = bit_read_BS(dat);

      if (strcmp((const char *)dwg->dwg_class[idc].dxfname, "LAYOUT") == 0)
        dwg->dwg_ot_layout = dwg->dwg_class[idc].number;

      dwg->num_classes++;
      /*
      if (dwg->num_classes > 100)
	{
	  fprintf(stderr, "number of classes is greater than 100. TODO: Why should we stop here?\n");
	  break;//TODO: Why?!
	}
      */
    }
  while (dat->byte < (lasta - 1));

  // Check CRC-on
  dat->byte = dwg->header.section[1].address + dwg->header.section[1].size - 18;
  dat->bit = 0;

  ckr = bit_read_RS(dat);
  ckr2 = bit_ckr8(0xc0c1, dat->chain + dwg->header.section[1].address + 16,
          dwg->header.section[1].size - 34);

  if (ckr != ckr2)
    {
      printf("section %d crc todo ckr:%x ckr2:%x\n",
              dwg->header.section[1].number, ckr, ckr2);
      return -1;
    }

  dat->byte += 16;
  pvz = bit_read_RL(dat); // Unknown bitlong inter class and object
  LOG_TRACE("Address: %lu / Content: 0x%#lX\n", dat->byte - 4, pvz)
  LOG_INFO("Number of classes read: %u\n", dwg->num_classes)

  /*-------------------------------------------------------------------------
   * Object-map
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
      long unsigned int previous_address = 0;

      duabyte = dat->byte;
      sgdc[0] = bit_read_RC(dat);
      sgdc[1] = bit_read_RC(dat);
      section_size = (sgdc[0] << 8) | sgdc[1];
      //LOG_TRACE("section_size: %u\n", section_size)
      if (section_size > 2035)
        {
          LOG_ERROR("Object-map section size greater than 2035!\n")
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
          // LOG_TRACE("Idc: %li\t", dwg->num_objects)
          // LOG_TRACE("Handle: %li\tAddress: %li", pvztkt, pvzadr)
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

      // CRC on
      if (dat->bit > 0)
        {
          dat->byte += 1;
          dat->bit = 0;
        }

      sgdc[0] = bit_read_RC(dat);
      sgdc[1] = bit_read_RC(dat);
      ckr = (sgdc[0] << 8) | sgdc[1];

      ckr2 = bit_ckr8(0xc0c1, dat->chain + duabyte, section_size);

      if (ckr != ckr2)
        {
          printf("section %d crc todo ckr:%x ckr2:%x\n",
                  dwg->header.section[2].number, ckr, ckr2);
          return -1;
        }

      if (dat->byte >= maplasta)
        break;
    }
  while (section_size > 2);

  LOG_INFO("Num objects: %lu\n", dwg->num_objects)
  LOG_INFO("\n=======> Object Data: %8X\n", (unsigned int) object_begin)
  dat->byte = object_end;
  object_begin = bit_read_MS(dat);
  LOG_INFO("         Object Data (end): %8X\n",
      (unsigned int) (object_end + object_begin+ 2))

  /*
   dat->byte = dwg->header.section[2].address - 2;
   antckr = bit_read_CRC (dat); // Unknown bitdouble inter object data and object map
   LOG_TRACE("Address: %08X / Content: 0x%04X", dat->byte - 2, antckr)

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
  LOG_INFO("\n=======> Object Map: %8X\n",
          (unsigned int) dwg->header.section[2].address)
  LOG_INFO("         Object Map (end): %8X\n",
          (unsigned int) (dwg->header.section[2].address
              + dwg->header.section[2].size))

  /*-------------------------------------------------------------------------
   * Second header
   */

  if (bit_search_sentinel(dat, dwg_sentinel(DWG_SENTINEL_SECOND_HEADER_BEGIN)))
    {
      long unsigned int pvzadr;
      long unsigned int pvz;
      unsigned char sig, sig2;

      LOG_INFO("\n=======> Second Header: %8X\n",
          (unsigned int) dat->byte-16)
      pvzadr = dat->byte;

      pvz = bit_read_RL(dat);
      //LOG_TRACE("Size: %lu\n", pvz)

      pvz = bit_read_BL(dat);
      //LOG_TRACE("Begin address: %8X\n", pvz)

      //LOG_TRACE("AC1015?: ")
      for (i = 0; i < 6; i++)
        {
          sig = bit_read_RC(dat);
          //LOG_TRACE("%c", sig >= ' ' && sig < 128 ? sig : '.')
        }

      //LOG_TRACE("Null?:")
      for (i = 0; i < 5; i++) // 6 if is older...
        {
          sig = bit_read_RC(dat);
          //LOG_TRACE(" 0x%02X", sig)
        }

      //LOG_TRACE"4 null bits?: ")
      for (i = 0; i < 4; i++)
        {
          sig = bit_read_B(dat);
          //LOG_TRACE(" %c", sig ? '1' : '0')
        }

      //LOG_TRACE(stderr, "Chain?: ")
      for (i = 0; i < 6; i++)
        {
          dwg->second_header.unknown[i] = bit_read_RC(dat);
          //LOG_TRACE(" 0x%02X", dwg->second_header.unknown[i])
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

      if (bit_search_sentinel(dat, dwg_sentinel(
          DWG_SENTINEL_SECOND_HEADER_END)))
        LOG_INFO("         Second Header (end): %8X\n", (unsigned int) dat->byte)
    }

  /*-------------------------------------------------------------------------
   * Section MEASUREMENT
   */

  LOG_INFO("\n=======> Unknown 2: %8X\n",
          (unsigned int) dwg->header.section[4].address)
  LOG_INFO("         Unknown 2 (end): %8X\n",
          (unsigned int) (dwg->header.section[4].address
              + dwg->header.section[4].size))
  dat->byte = dwg->header.section[4].address;
  dat->bit = 0;
  dwg->measurement = bit_read_RL(dat);

  LOG_TRACE("         Size bytes :\t%lu\n", dat->size)

  //step II of handles parsing: resolve pointers from handle value
  //XXX: move this somewhere else
  LOG_TRACE("\n\nResolving pointers from ObjectRef vector.\n")
  resolve_objectref_vector(dwg);
  LOG_TRACE("\n")

  return 0;
}

static void
resolve_objectref_vector(Dwg_Data * dwg)
{
  long unsigned int i;
  Dwg_Object * obj;
  for (i = 0; i < dwg->num_object_refs; i++)
    {
      LOG_TRACE("\n==========\n")
      LOG_TRACE("-objref: HANDLE(%d.%d.%lu) Absolute:%lu\n",
          dwg->object_ref[i]->handleref.code,
          dwg->object_ref[i]->handleref.size,
          dwg->object_ref[i]->handleref.value,
          dwg->object_ref[i]->absolute_ref)

      //look for object
      obj = dwg_resolve_handle(dwg, dwg->object_ref[i]->absolute_ref);

      if(obj)
        {
          LOG_TRACE("-found:  HANDLE(%d.%d.%lu)\n",
              obj->handle.code,
              obj->handle.size,
              obj->handle.value)
        }

      //assign found pointer to objectref vector
      dwg->object_ref[i]->obj = obj;


      if (DWG_LOGLEVEL >= DWG_LOGLEVEL_INSANE)
        {
          if (obj)
            dwg_print_object(obj);
          else
            LOG_ERROR("Null object pointer: object_ref[%lu]\n", i)
        }
    }
}

/* R2004 Literal Length
 */
static int
read_literal_length(Bit_Chain* dat, unsigned char *opcode)
{
  int total = 0;
  unsigned char byte = bit_read_RC(dat);

  *opcode = 0x00;

  if (byte >= 0x01 && byte <= 0x0F)
    return byte + 3;
  else if (byte == 0)
    {
      total = 0x0F;
      while ((byte = bit_read_RC(dat)) == 0x00)
        {
          total += 0xFF;
        }
      return total + byte + 3;
    }
  else if (byte & 0xF0)
    *opcode = byte;

  return 0;
}

/* R2004 Long Compression Offset
 */
static int
read_long_compression_offset(Bit_Chain* dat)
{
  int total = 0;
  unsigned char byte = bit_read_RC(dat);
  if (byte == 0)
    {
      total = 0xFF;
      while ((byte = bit_read_RC(dat)) == 0x00)
        total += 0xFF;
	}
  return total + byte;
}

/* R2004 Two Byte Offset
 */
static int
read_two_byte_offset(Bit_Chain* dat, int* lit_length)
{
  int offset;
  unsigned char firstByte = bit_read_RC(dat);
  unsigned char secondByte = bit_read_RC(dat);
  offset = (firstByte >> 2) | (secondByte << 6);
  *lit_length = (firstByte & 0x03);
  return offset;
}

/* Decompresses a system section of a 2004 DWG flie
 */
static int
decompress_R2004_section(Bit_Chain* dat, char *decomp,
                         unsigned long int comp_data_size)
{
  int lit_length, i;
  int comp_offset, comp_bytes;
  unsigned char opcode1 = 0, opcode2;
  long unsigned int start_byte = dat->byte;
  char *src, *dst = decomp;

  // length of the first sequence of uncompressed or literal data.
  lit_length = read_literal_length(dat, &opcode1);

  for (i = 0; i < lit_length; ++i)
    *dst++ = bit_read_RC(dat);

  opcode1 = 0x00;
  while (dat->byte - start_byte < comp_data_size)
    {
      if (opcode1 == 0x00)
        opcode1 = bit_read_RC(dat);

      if (opcode1 >= 0x40)
        {
          comp_bytes = ((opcode1 & 0xF0) >> 4) - 1;
          opcode2 = bit_read_RC(dat);
          comp_offset = (opcode2 << 2) | ((opcode1 & 0x0C) >> 2);

          if (opcode1 & 0x03)
            {
              lit_length = (opcode1 & 0x03);
              opcode1  = 0x00;
            }
          else
            lit_length = read_literal_length(dat, &opcode1);
        }
      else if (opcode1 >= 0x21 && opcode1 <= 0x3F)
        {
          comp_bytes  = opcode1 - 0x1E;
          comp_offset = read_two_byte_offset(dat, &lit_length);

          if (lit_length != 0)
            opcode1 = 0x00;
          else
            lit_length = read_literal_length(dat, &opcode1);
        }
      else if (opcode1 == 0x20)
        {
          comp_bytes  = read_long_compression_offset(dat) + 0x21;
          comp_offset = read_two_byte_offset(dat, &lit_length);

          if (lit_length != 0)
            opcode1 = 0x00;
          else
            lit_length = read_literal_length(dat, &opcode1);
        }
      else if (opcode1 >= 0x12 && opcode1 <= 0x1F)
        {
          comp_bytes  = (opcode1 & 0x0F) + 2;
          comp_offset = read_two_byte_offset(dat, &lit_length) + 0x3FFF;

          if (lit_length != 0)
            opcode1 = 0x00;
          else
            lit_length = read_literal_length(dat, &opcode1);
        }
      else if (opcode1 == 0x10)
        {
          comp_bytes  = read_long_compression_offset(dat) + 9;
          comp_offset = read_two_byte_offset(dat, &lit_length) + 0x3FFF;

          if (lit_length != 0)
            opcode1 = 0x00;
          else
            lit_length = read_literal_length(dat, &opcode1);
        }
      else if (opcode1 == 0x11)
          break;     // Terminates the input stream, everything is ok!
      else
          return 1;  // error in input stream

      //LOG_TRACE("got compressed data %d\n",comp_bytes)
      // copy "compressed data"
      src = dst - comp_offset - 1;
      assert(src > decomp);
      for (i = 0; i < comp_bytes; ++i)
        *dst++ = *src++;

      // copy "literal data"
      //LOG_TRACE("got literal data %d\n",lit_length)
      for (i = 0; i < lit_length; ++i)
        *dst++ = bit_read_RC(dat);
    }

  return 0;  // Success
}

/* Read R2004 Section Map
 * The Section Map is a vector of number, size, and address triples used
 * to locate the sections in the file.
 */
static void
read_R2004_section_map(Bit_Chain* dat, Dwg_Data * dwg,
                       unsigned long int comp_data_size,
                       unsigned long int decomp_data_size)
{
  char *decomp, *ptr;
  int i;
  int section_address;
  int bytes_remaining;

  dwg->header.num_sections = 0;
  dwg->header.section = 0;

  // allocate memory to hold decompressed data
  decomp = (char *)malloc(decomp_data_size * sizeof(char));
  if (decomp == 0)
    return;   // No memory

  decompress_R2004_section(dat, decomp, comp_data_size);

  LOG_TRACE("\n#### 2004 Section Map fields ####\n")

  section_address = 0x100;  // starting address
	i = 0;
	bytes_remaining = decomp_data_size;
	ptr = decomp;
	dwg->header.num_sections = 0;

	while(bytes_remaining)
		{
			if (dwg->header.num_sections==0)
				dwg->header.section = (Dwg_Section*) malloc(sizeof(Dwg_Section));
			else
				dwg->header.section = (Dwg_Section*) realloc(dwg->header.section,
          sizeof(Dwg_Section) * (dwg->header.num_sections+1));

		  dwg->header.section[i].number  = *((int*)ptr);
		  dwg->header.section[i].size    = *((int*)ptr+1);
		  dwg->header.section[i].address = section_address;
		  section_address += dwg->header.section[i].size;
			bytes_remaining -= 8;
			ptr+=8;

		  LOG_TRACE("SectionNumber: %d\n",   dwg->header.section[i].number)
		  LOG_TRACE("SectionSize:   %x\n",   dwg->header.section[i].size)
		  LOG_TRACE("SectionAddr:   %x\n", dwg->header.section[i].address)

			if (dwg->header.section[i].number<0)
				{
			    dwg->header.section[i].parent  = *((int*)ptr);
			    dwg->header.section[i].left  = *((int*)ptr+1);
			    dwg->header.section[i].right  = *((int*)ptr+2);
			    dwg->header.section[i].x00  = *((int*)ptr+3);
					bytes_remaining -= 16;
					ptr+=16;

                            LOG_TRACE("Parent: %d\n",   (int)dwg->header.section[i].parent)
                            LOG_TRACE("Left: %d\n",   (int)dwg->header.section[i].left)
                            LOG_TRACE("Right: %d\n",   (int)dwg->header.section[i].right)
                            LOG_TRACE("0x00: %d\n",   (int)dwg->header.section[i].x00)
                            }

			dwg->header.num_sections++;
			i++;
		}
  free(decomp);
}

static Dwg_Section*
find_section(Dwg_Data *dwg, unsigned long int index)
{
  int i;
  if (dwg->header.section == 0 || index == 0)
    return 0;
  for (i = 0; i < dwg->header.num_sections; ++i)
    {
      if (dwg->header.section[i].number == index)
        return (&dwg->header.section[i]);
    }
  return 0;
}

/* Read R2004 Section Info
 */
static void
read_R2004_section_info(Bit_Chain* dat, Dwg_Data *dwg,
                        unsigned long int comp_data_size,
                        unsigned long int decomp_data_size)
{
  char *decomp, *ptr;
  int i, j;
  int section_number;
  int data_size;
  int start_offset;
  int unknown;

  decomp = (char *)malloc(decomp_data_size * sizeof(char));
  if (decomp == 0)
    return;   // No memory

  decompress_R2004_section(dat, decomp, comp_data_size);

  memcpy(&dwg->header.num_descriptions, decomp, 4);
  dwg->header.section_info = (Dwg_Section_Info*)
    malloc(sizeof(Dwg_Section_Info) * dwg->header.num_descriptions);

  LOG_TRACE("\n#### 2004 Section Info fields ####\n")
  LOG_TRACE("NumDescriptions: %d\n", *((int*)decomp))
  LOG_TRACE("0x02:            %x\n", *((int*)decomp + 1))
  LOG_TRACE("0x00007400:      %x\n", *((int*)decomp + 2))
  LOG_TRACE("0x00:            %x\n", *((int*)decomp + 3))
  LOG_TRACE("Unknown:         %x\n", *((int*)decomp + 4))

  ptr = decomp + 20;
  for (i = 0; i < dwg->header.num_descriptions; ++i)
    {
      dwg->header.section_info[i].size            = *((int*)ptr);
      dwg->header.section_info[i].unknown1 	      = *((int*)ptr + 1);
      dwg->header.section_info[i].num_sections    = *((int*)ptr + 2);
      dwg->header.section_info[i].max_decomp_size = *((int*)ptr + 3);
      dwg->header.section_info[i].unknown2        = *((int*)ptr + 4);
      dwg->header.section_info[i].compressed      = *((int*)ptr + 5);
      dwg->header.section_info[i].type            = *((int*)ptr + 6);
      dwg->header.section_info[i].encrypted       = *((int*)ptr + 7);
      ptr += 32;
      memcpy(dwg->header.section_info[i].name, ptr, 64);
      ptr += 64;

      LOG_TRACE("\nSection Info description fields\n")
      LOG_TRACE("Size:                  %d",
           (int) dwg->header.section_info[i].size)
      LOG_TRACE("Unknown:               %d\n",
           (int) dwg->header.section_info[i].unknown1)
      LOG_TRACE("Number of sections:    %d\n",
           (int) dwg->header.section_info[i].num_sections)
      LOG_TRACE("Max decompressed size: %d\n",
           (int) dwg->header.section_info[i].max_decomp_size)
      LOG_TRACE("Unknown:               %d\n",
           (int) dwg->header.section_info[i].unknown2)
      LOG_TRACE("Compressed (0x02):     %x\n",
           (unsigned int) dwg->header.section_info[i].compressed)
      LOG_TRACE("Section Type:          %d\n",
           (int) dwg->header.section_info[i].type)
      LOG_TRACE("Encrypted:             %d\n",
           (int) dwg->header.section_info[i].encrypted)
      LOG_TRACE("SectionName:           %s\n\n",
            dwg->header.section_info[i].name)

      dwg->header.section_info[i].sections = (Dwg_Section**)
        malloc(dwg->header.section_info[i].num_sections * sizeof(Dwg_Section*));

      if (dwg->header.section_info[i].num_sections < 10000)
	{
	  LOG_INFO("section count %ld in area %d\n",dwg->header.section_info[i].num_sections,i)

	  for (j = 0; j < dwg->header.section_info[i].num_sections; j++)
	    {
	      section_number = *((int*)ptr);      // Index into SectionMap
	      data_size      = *((int*)ptr + 1);
	      start_offset   = *((int*)ptr + 2);
	      unknown        = *((int*)ptr + 3);  // high 32 bits of 64-bit start offset?
	      ptr += 16;

	      dwg->header.section_info[i].sections[j] = find_section(dwg, section_number);

	      LOG_TRACE("Section Number: %d\n", section_number)
	      LOG_TRACE("Data size:      %d\n", data_size)
	      LOG_TRACE("Start offset:   %x\n", start_offset)
	      LOG_TRACE("Unknown:        %d\n", unknown)
	    }
	}// sanity check
      else
	{
	  LOG_ERROR("section count %ld in area %d too high! skipping\n",dwg->header.section_info[i].num_sections,i)
	}
    }
  free(decomp);
}

/* Encrypted Section Header */
typedef union _encrypted_section_header
{
  unsigned long int long_data[8];
  unsigned char char_data[32];
  struct
  {
    unsigned long int tag;
    unsigned long int section_type;
    unsigned long int data_size;
    unsigned long int section_size;
    unsigned long int start_offset;
    unsigned long int unknown;
    unsigned long int checksum_1;
    unsigned long int checksum_2;
  } fields;
} encrypted_section_header;

static int
read_2004_compressed_section(Bit_Chain* dat, Dwg_Data *dwg,
                            Bit_Chain* sec_dat,
                            long unsigned int section_type)
{
  long unsigned int address, sec_mask;
  long unsigned int max_decomp_size;
  Dwg_Section_Info *info = 0;
  encrypted_section_header es;
  char *decomp;
  int i, j;

  for (i = 0; i < dwg->header.num_descriptions && info == 0; ++i)
    if (dwg->header.section_info[i].type == section_type)
      info = &dwg->header.section_info[i];

  if (info == 0)
    return 1;   // Failed to find section

  max_decomp_size = info->num_sections * info->max_decomp_size;

  decomp = (char *)malloc(max_decomp_size * sizeof(char));
  if (decomp == 0)
    return 2;   // No memory

  for (i = 0; i < info->num_sections; ++i)
    {
      address = info->sections[i]->address;
      dat->byte = address;

      for (j = 0; j < 0x20; j++)
        es.char_data[j] = bit_read_RC(dat);

      sec_mask = 0x4164536b ^ address;
      for (j = 0; j < 8; ++j)
        es.long_data[j] ^= sec_mask;

  LOG_INFO("\n=== Section (Class) ===\n")
  LOG_INFO("Section Tag (should be 0x4163043b): %x\n",
          (unsigned int) es.fields.tag)
  LOG_INFO("Section Type:     %x\n",
          (unsigned int) es.fields.section_type)
  LOG_INFO("Data size:   %x\n",
          (unsigned int) es.fields.data_size)   // this is the number of bytes that is read in decompress_R2004_section (+ 2bytes)
  LOG_INFO("Comp data size:     %x\n",
          (unsigned int) es.fields.section_size)
  LOG_INFO("StartOffset:      %x\n",
          (unsigned int) es.fields.start_offset)
  LOG_INFO("Unknown:          %x\n",
          (unsigned int) es.fields.unknown);
  LOG_INFO("Checksum1:        %x\n",
        (unsigned int) es.fields.checksum_1)
  LOG_INFO("Checksum2:        %x\n\n",
        (unsigned int) es.fields.checksum_2)

      decompress_R2004_section(dat, &decomp[i * info->max_decomp_size],
        es.fields.data_size);
    }

  sec_dat->bit     = 0;
  sec_dat->byte    = 0;
  sec_dat->chain   = (unsigned char *)decomp;
  sec_dat->size    = max_decomp_size;
  sec_dat->version = dat->version;

  return 0;
}

/* R2004 Class Section
 */
static void
read_2004_section_classes(Bit_Chain* dat, Dwg_Data *dwg)
{
  unsigned long int size;
  unsigned long int max_num;
  unsigned long int num_objects, dwg_version, maint_version, unknown;
  char c;
  Bit_Chain sec_dat;

  if (read_2004_compressed_section(dat, dwg, &sec_dat, SECTION_CLASSES) != 0)
    return;

  if (bit_search_sentinel(&sec_dat, dwg_sentinel(DWG_SENTINEL_CLASS_BEGIN)))
    {
      size    = bit_read_RL(&sec_dat);  // size of class data area
      max_num = bit_read_BS(&sec_dat);  // Maxiumum class number
      c = bit_read_RC(&sec_dat);        // 0x00
      c = bit_read_RC(&sec_dat);        // 0x00
      c = bit_read_B(&sec_dat);         // 1

      dwg->dwg_ot_layout = 0;
      dwg->num_classes = 0;

      do
        {
          unsigned int idc;

          idc = dwg->num_classes;
          if (idc == 0)
            dwg->dwg_class = (Dwg_Class *) malloc(sizeof(Dwg_Class));
          else
            dwg->dwg_class = (Dwg_Class *) realloc(dwg->dwg_class, (idc + 1)
                * sizeof(Dwg_Class));

          dwg->dwg_class[idc].number        = bit_read_BS(&sec_dat);
          dwg->dwg_class[idc].version       = bit_read_BS(&sec_dat);
          dwg->dwg_class[idc].appname       = bit_read_TV(&sec_dat);
          dwg->dwg_class[idc].cppname       = bit_read_TV(&sec_dat);
          dwg->dwg_class[idc].dxfname       = bit_read_TV(&sec_dat);
          dwg->dwg_class[idc].wasazombie    = bit_read_B(&sec_dat);
          dwg->dwg_class[idc].item_class_id = bit_read_BS(&sec_dat);

          num_objects   = bit_read_BL(&sec_dat);  // DXF 91
          dwg_version   = bit_read_BS(&sec_dat);  // Dwg Version
          maint_version = bit_read_BS(&sec_dat);  // Maintenance release version.
          unknown       = bit_read_BL(&sec_dat);  // Unknown (normally 0L)
          unknown       = bit_read_BL(&sec_dat);  // Unknown (normally 0L)

          LOG_TRACE("-------------------\n")
          LOG_TRACE("Number:           %d\n", dwg->dwg_class[idc].number)
          LOG_TRACE("Version:          %x\n", dwg->dwg_class[idc].version)
          LOG_TRACE("Application name: %s\n", dwg->dwg_class[idc].appname)
          LOG_TRACE("C++ class name:   %s\n", dwg->dwg_class[idc].cppname)
          LOG_TRACE("DXF record name:  %s\n", dwg->dwg_class[idc].dxfname)

          if (strcmp((const char *)dwg->dwg_class[idc].dxfname, "LAYOUT") == 0)
            dwg->dwg_ot_layout = dwg->dwg_class[idc].number;

          dwg->num_classes++;

        } while (sec_dat.byte < (size - 1));
    }
    free(sec_dat.chain);
}

/* R2004 Header Section
 */
static void
read_2004_section_header(Bit_Chain* dat, Dwg_Data *dwg)
{
  Bit_Chain sec_dat;

  if (read_2004_compressed_section(dat, dwg, &sec_dat, SECTION_HEADER) != 0)
    return;

  if (bit_search_sentinel(&sec_dat, dwg_sentinel(DWG_SENTINEL_VARIABLE_BEGIN)))
    {
      unsigned long int size = bit_read_RL(&sec_dat);
      LOG_TRACE("Length: %lu\n", size);
      dwg_decode_header_variables(&sec_dat, dwg);
    }
  free(sec_dat.chain);
}

/* R2004 Handles Section
 */
static void
read_2004_section_handles(Bit_Chain* dat, Dwg_Data *dwg)
{
  unsigned int section_size = 0;
  unsigned char sgdc[2];
  long unsigned int duabyte;
  long unsigned int maplasta;
  Bit_Chain hdl_dat;
  Bit_Chain obj_dat;

  if (read_2004_compressed_section(dat, dwg, &obj_dat, SECTION_DBOBJECTS) != 0)
    return;

  if (read_2004_compressed_section(dat, dwg, &hdl_dat, SECTION_HANDLES) != 0)
    {
      free(obj_dat.chain);
      return;
    }

  maplasta = hdl_dat.byte + hdl_dat.size;
  dwg->num_objects = 0;

  do
    {
      long unsigned int last_offset;
      long unsigned int last_handle;
      long unsigned int previous_address = 0;

      duabyte = hdl_dat.byte;
      sgdc[0] = bit_read_RC(&hdl_dat);
      sgdc[1] = bit_read_RC(&hdl_dat);
      section_size = (sgdc[0] << 8) | sgdc[1];

      LOG_TRACE("section_size: %u\n", section_size);

      if (section_size > 2034)
        LOG_INFO("Error: Object-map section size greater than 2034!\n");

      last_handle = 0;
      last_offset = 0;
      while (hdl_dat.byte - duabyte < section_size)
        {
          long int pvztkt;
          long int pvzadr;

          previous_address = hdl_dat.byte;

          pvztkt = bit_read_MC(&hdl_dat);
          last_handle += pvztkt;

          pvzadr = bit_read_MC(&hdl_dat);
          last_offset += pvzadr;

          dwg_decode_add_object(dwg, &obj_dat, last_offset);
        }

      if (hdl_dat.byte == previous_address)
        break;
      hdl_dat.byte += 2; // CRC

      if (hdl_dat.byte >= maplasta)
        break;
    }
  while (section_size > 2);

  LOG_TRACE("\nNum objects: %lu\n", dwg->num_objects);

  free(hdl_dat.chain);
  free(obj_dat.chain);
}

int
decode_R2004(Bit_Chain* dat, Dwg_Data * dwg)
{
  /* Encripted Data */
  union
  {
    unsigned char encripted_data[0x6c];
    struct
    {
      unsigned char file_ID_string[12];
      unsigned int x00;
      unsigned int x6c;
      unsigned int x04;
      unsigned int root_tree_node_gap;
      unsigned int lowermost_left_tree_node_gap;
      unsigned int lowermost_right_tree_node_gap;
      unsigned int unknown_long;
      unsigned int last_section_id;
      unsigned int last_section_address;
      unsigned int x00_2;
      unsigned int second_header_address;
      unsigned int x00_3;
      unsigned int gap_amount;
      unsigned int section_amount;
      unsigned int x20;
      unsigned int x80;
      unsigned int x40;
      unsigned int section_map_id;
      unsigned int section_map_address;
      unsigned int x00_4;
      unsigned int section_info_id;
      unsigned int section_array_size;
      unsigned int gap_array_size;
      unsigned int CRC;
    } fields;
  } _2004_header_data;

  /* System Section */
  typedef union _system_section
  {
    unsigned char data[0x14];
    struct
    {
      unsigned int section_type;   //0x4163043b
      unsigned int decomp_data_size;
      unsigned int comp_data_size;
      unsigned int compression_type;
      unsigned int checksum;
    } fields;
  } system_section;

  system_section ss;
  int rseed = 1;

  Dwg_Section *section;

  int i;
  unsigned long int preview_address, security_type, unknown_long,
      dwg_property_address, vba_proj_address;
  unsigned char sig, dwg_ver, maint_release_ver;

  //6 bytes of 0x00
  dat->byte = 0x06;
  LOG_INFO("6 bytes of 0x00: ")
  for (i = 0; i < 6; i++)
    {
      sig = bit_read_RC(dat);
      LOG_INFO( "0x%02X ", sig)
    }
  LOG_INFO("\n")

  /* Byte 0x00, 0x01, or 0x03 */
  dat->byte = 0x0C;
  sig = bit_read_RC(dat);
  LOG_INFO("Byte 0x00, 0x01, or 0x03: 0x%02X\n", sig)

  /* Preview Address */
  dat->byte = 0x0D;
  preview_address = bit_read_RL(dat);
  LOG_INFO("Preview Address: 0x%08X\n", (unsigned int) preview_address)

  /* DwgVer */
  dat->byte = 0x11;
  dwg_ver = bit_read_RC(dat);
  LOG_INFO("DwgVer: %u\n", dwg_ver)

  /* MaintReleaseVer */
  dat->byte = 0x12;
  maint_release_ver = bit_read_RC(dat);
  LOG_INFO("MaintRelease: %u\n", maint_release_ver)

  /* Codepage */
  dat->byte = 0x13;
  dwg->header.codepage = bit_read_RS(dat);
  LOG_INFO("Codepage: %u\n", dwg->header.codepage)

  /* 3 0x00 bytes */
  dat->byte = 0x15;
  LOG_INFO("3 0x00 bytes: ")
  for (i = 0; i < 3; i++)
    {
      sig = bit_read_RC(dat);
      LOG_INFO("0x%02X ", sig)
    }
  LOG_INFO("\n")

  /* SecurityType */
  dat->byte = 0x18;
  security_type = bit_read_RL(dat);
  LOG_INFO("SecurityType: 0x%08X\n", (unsigned int) security_type)

  /* Unknown long */
  dat->byte = 0x1C;
  unknown_long = bit_read_RL(dat);
  LOG_INFO("Unknown long: 0x%08X\n", (unsigned int) unknown_long)

  /* DWG Property Addr */
  dat->byte = 0x20;
  dwg_property_address = bit_read_RL(dat);
  LOG_INFO("DWG Property Addr: 0x%08X\n",
        (unsigned int) dwg_property_address)

  /* VBA Project Addr */
  dat->byte = 0x24;
  vba_proj_address = bit_read_RL(dat);
  LOG_INFO("VBA Project Addr: 0x%08X\n",
        (unsigned int) vba_proj_address)

  /* 0x00000080 */
  dat->byte = 0x28;
  unknown_long = bit_read_RL(dat);
  LOG_INFO("0x00000080: 0x%08X\n", (unsigned int) unknown_long)

  /* 0x00 bytes (length = 0x54 bytes) */
  dat->byte = 0x2C;
  for (i = 0; i < 0x54; i++)
    {
      sig = bit_read_RC(dat);
      if (sig != 0 && loglevel)
        LOG_ERROR(
            "Warning: Byte should be zero! But a value=%x was read instead.\n",
            sig)
    }

  dat->byte = 0x80;
  for (i = 0; i < 0x6c; i++)
    {
      rseed *= 0x343fd;
      rseed += 0x269ec3;
      _2004_header_data.encripted_data[i] = bit_read_RC(dat) ^ (rseed >> 0x10);
    }

  if (loglevel)
    {
      LOG_TRACE("\n#### 2004 File Header Data fields ####\n")
      LOG_TRACE("File ID string (must be AcFssFcAJMB): ");
      for (i = 0; i < 12; i++)
        {
          LOG_TRACE("%c", _2004_header_data.fields.file_ID_string[i])
        }
      LOG_TRACE("\n")
      LOG_TRACE("0x00 (long): %x\n",
          (unsigned int) _2004_header_data.fields.x00)
      LOG_TRACE("0x6c (long): %x\n",
          (unsigned int) _2004_header_data.fields.x6c)
      LOG_TRACE("0x04 (long): %x\n",
          (unsigned int) _2004_header_data.fields.x04)
      LOG_TRACE("Root tree node gap: %x\n",
          (unsigned int) _2004_header_data.fields.root_tree_node_gap)
      LOG_TRACE("Lowermost left tree node gap: %x\n",
          (unsigned int) _2004_header_data.fields.lowermost_left_tree_node_gap)
      LOG_TRACE("Lowermost right tree node gap: %x\n",
          (unsigned int) _2004_header_data.fields.lowermost_right_tree_node_gap)
      LOG_TRACE("Unknown long: %x\n",
          (unsigned int) _2004_header_data.fields.unknown_long)
      LOG_TRACE("Last section id: %x\n",
          (unsigned int) _2004_header_data.fields.last_section_id)
      LOG_TRACE("Last section address: %x\n",
          (unsigned int) _2004_header_data.fields.last_section_address)
      LOG_TRACE("0x00 (long): %x\n",
          (unsigned int) _2004_header_data.fields.x00_2)
      LOG_TRACE("Second header address: %x\n",
          (unsigned int) _2004_header_data.fields.second_header_address)
      LOG_TRACE("0x00 (long): %x\n",
          (unsigned int) _2004_header_data.fields.x00_3)
      LOG_TRACE("Gap amount: %x\n",
          (unsigned int) _2004_header_data.fields.gap_amount)
      LOG_TRACE("Section amount: %x\n",
          (unsigned int) _2004_header_data.fields.section_amount)
      LOG_TRACE("0x20 (long): %x\n",
          (unsigned int) _2004_header_data.fields.x20)
      LOG_TRACE("0x80 (long): %x\n",
          (unsigned int) _2004_header_data.fields.x80)
      LOG_TRACE("0x40 (long): %x\n",
          (unsigned int) _2004_header_data.fields.x40)
      LOG_TRACE("Section map id: %x\n",
          (unsigned int) _2004_header_data.fields.section_map_id)
      LOG_TRACE("Section map address: %x\n",
          (unsigned int) _2004_header_data.fields.section_map_address + 0x100)
      LOG_TRACE("0x00 (long): %x\n",
          (unsigned int) _2004_header_data.fields.x00_4)
      LOG_TRACE("Section Info id: %x\n",
          (unsigned int) _2004_header_data.fields.section_info_id)
      LOG_TRACE("Section array size: %x\n",
          (unsigned int) _2004_header_data.fields.section_array_size)
      LOG_TRACE("Gap array size: %x\n",
          (unsigned int) _2004_header_data.fields.gap_array_size)
      LOG_TRACE("CRC: %x\n", (unsigned int) _2004_header_data.fields.CRC)
    }

  /*-------------------------------------------------------------------------
   * Section Map
   */
  dat->byte = _2004_header_data.fields.section_map_address + 0x100;

  LOG_TRACE("\n\nRaw system section bytes:\n");
  for (i = 0; i < 0x14; i++)
    {
      ss.data[i] = bit_read_RC(dat);
      LOG_TRACE("%x ", ss.data[i])
    }

  LOG_TRACE("\n=== System Section (Section Map) ===\n")
  LOG_TRACE("Section Type (should be 0x4163043b): %x\n",
          (unsigned int) ss.fields.section_type)
  LOG_TRACE("DecompDataSize: %x\n",
          (unsigned int) ss.fields.decomp_data_size)
  LOG_TRACE("CompDataSize: %x\n",
          (unsigned int) ss.fields.comp_data_size)
  LOG_TRACE("Compression Type: %x\n",
          (unsigned int) ss.fields.compression_type)
  LOG_TRACE("Checksum: %x\n\n", (unsigned int) ss.fields.checksum)

  read_R2004_section_map(dat, dwg,
      ss.fields.comp_data_size, ss.fields.decomp_data_size);

  if (dwg->header.section == 0)
    {
      LOG_ERROR("Failed to read R2004 Section Map.\n")
      return -1;
    }

  /*-------------------------------------------------------------------------
   * Section Info
   */
  section = find_section(dwg, _2004_header_data.fields.section_info_id);

  if (section != 0)
    {
      dat->byte = section->address;
      LOG_TRACE("\nRaw system section bytes:\n")
      for (i = 0; i < 0x14; i++)
        {
          ss.data[i] = bit_read_RC(dat);
          LOG_TRACE("%x ", ss.data[i])
        }

      LOG_TRACE("\n=== System Section (Section Info) ===\n")
      LOG_TRACE("Section Type (should be 0x4163043b): %x\n",
              (unsigned int) ss.fields.section_type)
      LOG_TRACE("DecompDataSize: %x\n",
              (unsigned int) ss.fields.decomp_data_size)
      LOG_TRACE("CompDataSize: %x\n",
              (unsigned int) ss.fields.comp_data_size)
      LOG_TRACE("Compression Type: %x\n",
              (unsigned int) ss.fields.compression_type)
      LOG_TRACE("Checksum: %x\n\n", (unsigned int) ss.fields.checksum)

       read_R2004_section_info(dat, dwg,
         ss.fields.comp_data_size, ss.fields.decomp_data_size);
    }

  read_2004_section_classes(dat, dwg);
  read_2004_section_header(dat, dwg);
  read_2004_section_handles(dat, dwg);

  /* Clean up */
  if (dwg->header.section_info != 0)
    {
      for (i = 0; i < dwg->header.num_descriptions; ++i)
        if (dwg->header.section_info[i].sections != 0)
          free(dwg->header.section_info[i].sections);

      free(dwg->header.section_info);
      dwg->header.num_descriptions = 0;
    }

  resolve_objectref_vector(dwg);

  LOG_ERROR(
	  "Decoding of DWG version R2004 header is not fully implemented yet. We are going to try\n")
  return 0;
}

int
decode_R2007(Bit_Chain* dat, Dwg_Data * dwg)
{
  int i;
  unsigned long int preview_address, security_type, unknown_long,
      dwg_property_address, vba_proj_address, app_info_address;
  unsigned char sig, DwgVer, MaintReleaseVer;

  /* 5 bytes of 0x00 */
  dat->byte = 0x06;
  LOG_TRACE("5 bytes of 0x00: ")
  for (i = 0; i < 5; i++)
    {
      sig = bit_read_RC(dat);
      if (loglevel)
        LOG_TRACE("0x%02X ", sig)
    }
  LOG_TRACE("\n")

  /* Unknown */
  dat->byte = 0x0B;
  sig = bit_read_RC(dat);
  LOG_TRACE("Unknown: 0x%02X\n", sig)

  /* Byte 0x00, 0x01, or 0x03 */
  dat->byte = 0x0C;
  sig = bit_read_RC(dat);
  LOG_TRACE("Byte 0x00, 0x01, or 0x03: 0x%02X\n", sig)

  /* Preview Address */
  dat->byte = 0x0D;
  preview_address = bit_read_RL(dat);
  LOG_TRACE("Preview Address: 0x%08X\n", (unsigned int) preview_address)

  /* DwgVer */
  dat->byte = 0x11;
  DwgVer = bit_read_RC(dat);
  LOG_INFO("DwgVer: %u\n", DwgVer)

  /* MaintReleaseVer */
  dat->byte = 0x12;
  MaintReleaseVer = bit_read_RC(dat);
  LOG_INFO("MaintRelease: %u\n", MaintReleaseVer)

  /* Codepage */
  dat->byte = 0x13;
  dwg->header.codepage = bit_read_RS(dat);
  LOG_TRACE("Codepage: %u\n", dwg->header.codepage)

  /* Unknown */
  dat->byte = 0x15;
  LOG_TRACE("Unknown: ")
  for (i = 0; i < 3; i++)
    {
      sig = bit_read_RC(dat);
      LOG_TRACE("0x%02X ", sig)
    }
  LOG_TRACE("\n")

  /* SecurityType */
  dat->byte = 0x18;
  security_type = bit_read_RL(dat);
  LOG_TRACE("SecurityType: 0x%08X\n", (unsigned int) security_type)

  /* Unknown long */
  dat->byte = 0x1C;
  unknown_long = bit_read_RL(dat);
  LOG_TRACE("Unknown long: 0x%08X\n", (unsigned int) unknown_long)

  /* DWG Property Addr */
  dat->byte = 0x20;
  dwg_property_address = bit_read_RL(dat);
  LOG_TRACE("DWG Property Addr: 0x%08X\n",
        (unsigned int) dwg_property_address)

  /* VBA Project Addr */
  dat->byte = 0x24;
  vba_proj_address = bit_read_RL(dat);
  LOG_TRACE("VBA Project Addr: 0x%08X\n",
        (unsigned int) vba_proj_address)

  /* 0x00000080 */
  dat->byte = 0x28;
  unknown_long = bit_read_RL(dat);
  LOG_TRACE("0x00000080: 0x%08X\n", (unsigned int) unknown_long)

  /* Application Info Address */
  dat->byte = 0x2C;
  app_info_address = bit_read_RL(dat);
  LOG_TRACE("Application Info Address: 0x%08X\n",
        (unsigned int) app_info_address)

  read_r2007_meta_data(dat, dwg);
  
  LOG_TRACE("\n\n")

  /////////////////////////////////////////
  //	incomplete implementation!
  /////////////////////////////////////////
  resolve_objectref_vector(dwg);

  LOG_ERROR(
      "Decoding of DWG version R2007 header is not fully implemented yet. we are going to try\n")
  return 0;
}

/*--------------------------------------------------------------------------------
 * Private functions
 */

static int
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
      LOG_ERROR(
          "dwg_decode_entity:\tError in object handle! Current Bit_Chain address: 0x%0x\n",
          (unsigned int) dat->byte)
      ent->bitsize = 0;
      ent->extended_size = 0;
      ent->picture_exists = 0;
      ent->num_handles = 0;
      return 0;
    }

  ent->extended_size = 0;
  while (size = bit_read_BS(dat))
    {
      LOG_TRACE("EED size: %lu\n", (long unsigned int)size)
      if (size > 10210)
        {
          LOG_ERROR(
              "dwg_decode_entity: Absurd! Extended object data size: %lu. Object: %lu (handle).\n",
              (long unsigned int) size, ent->object->handle.value)
          ent->bitsize = 0;
          ent->extended_size = 0;
          ent->picture_exists = 0;
          ent->num_handles = 0;
          //XXX
          return -1;
          //break;
        }
      if (ent->extended_size == 0)
        {
          ent->extended = (char *)malloc(size);
          ent->extended_size = size;
        }
      else
        {
          ent->extended_size += size;
          ent->extended = (char *)realloc(ent->extended, ent->extended_size);
        }
      error = bit_read_H(dat, &ent->extended_handle);
      if (error)
        LOG_ERROR("Error reading extended handle!\n");
      for (i = ent->extended_size - size; i < ent->extended_size; i++)
        ent->extended[i] = bit_read_RC(dat);
    }
  ent->picture_exists = bit_read_B(dat);
  if (ent->picture_exists)
    {
      ent->picture_size = bit_read_RL(dat);
      if (ent->picture_size < 210210)
        {
          ent->picture = (char *)malloc(ent->picture_size);
          for (i = 0; i < ent->picture_size; i++)
            ent->picture[i] = bit_read_RC(dat);
        }
      else
        {
          LOG_ERROR(
              "dwg_decode_entity:  Absurd! Picture-size: %lu kB. Object: %lu (handle).\n",
              ent->picture_size / 1000, ent->object->handle.value)
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
      ent->xdic_missing_flag = bit_read_B(dat);
    }

  VERSIONS(R_13,R_14)
    {
      ent->isbylayerlt = bit_read_B(dat);
    }

  ent->nolinks = bit_read_B(dat);

  SINCE(R_2004)
    {
      char color_mode = 0;
      unsigned char index;
      unsigned int flags;
    
      if (ent->nolinks == 0)
        {        
          color_mode = bit_read_B(dat);
        
          if (color_mode == 1)
            index = bit_read_RC(dat);  // color index
          else
            {              
              flags = bit_read_RS(dat);
            
              if (flags & 0x8000)
                {
                  unsigned char c1, c2, c3, c4;
                  char *name=0;
              
                  c1 = bit_read_RC(dat);  // rgb color
                  c2 = bit_read_RC(dat);
                  c3 = bit_read_RC(dat);
                  c4 = bit_read_RC(dat);
              
                  name = bit_read_TV(dat);
                }
            
              if (flags & 0x4000)
                flags = flags;   // has AcDbColor reference (handle)
            
              if (flags & 0x2000)
                {
                  int transparency = bit_read_BL(dat);
                }
            }
        }
      else
        {
          char color = bit_read_B(dat);
        }
    }
  OTHER_VERSIONS
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

  return 0;
}

static int
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
      LOG_ERROR(
          "\tError in object handle! Bit_Chain current address: 0x%0x\n",
          (unsigned int) dat->byte)
      ord->bitsize = 0;
      ord->extended_size = 0;
      ord->num_handles = 0;
      return -1;
    }
  ord->extended_size = 0;
  while (size = bit_read_BS(dat))
    {
      if (size > 10210)
        {
          LOG_ERROR(
              "dwg_decode_object: Absurd! Extended object data size: %lu. Object: %lu (handle).\n",
              (long unsigned int) size, ord->object->handle.value)
          ord->bitsize = 0;
          ord->extended_size = 0;
          ord->num_handles = 0;
          return 0;
        }
      if (ord->extended_size == 0)
        {
          ord->extended = (unsigned char *)malloc(size);
          ord->extended_size = size;
        }
      else
        {
          ord->extended_size += size;
          ord->extended = (unsigned char *)realloc(ord->extended, ord->extended_size);
        }
      error = bit_read_H(dat, &ord->extended_handle);
      if (error)
        LOG_ERROR("Error reading extended handle!\n")
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

  return 0;
}

/**
 * Find a pointer to an object given it's id (handle)
 */
static Dwg_Object *
dwg_resolve_handle(Dwg_Data* dwg, long unsigned int absref)
{
  //FIXME find a faster algorithm
  long unsigned int i;
  for (i = 0; i < dwg->num_objects; i++)
    {
      if (dwg->object[i].handle.value == absref)
        {
          return &dwg->object[i];
        }
    }
  LOG_ERROR("Object not found: %lu\n", absref)
  return 0;
}

static Dwg_Object_Ref *
dwg_decode_handleref(Bit_Chain * dat, Dwg_Object * obj, Dwg_Data* dwg)
{
  // Welcome to the house of evil code!
  Dwg_Object_Ref* ref = (Dwg_Object_Ref *) malloc(sizeof(Dwg_Object_Ref));

  if (bit_read_H(dat, &ref->handleref))
    {
      if (obj)
        {
          LOG_ERROR(
            "Could not read handleref in object whose handle is: %d.%d.%lu\n",
            obj->handle.code, obj->handle.size, obj->handle.value)
        }
      else
        {
          LOG_ERROR("Could not read handleref in the header variables section\n")
        }
      free(ref);
      return 0;
    }

  //if the handle size is 0, it is probably a null handle. It
  //shouldn't be placed in the object ref vector
  if (ref->handleref.size)
    {
      //Reserve memory space for object references
      if (dwg->num_object_refs == 0)
        dwg->object_ref = (Dwg_Object_Ref **) malloc(REFS_PER_REALLOC * sizeof(Dwg_Object_Ref*));
      else
        if (dwg->num_object_refs % REFS_PER_REALLOC == 0)
          {
            dwg->object_ref = (Dwg_Object_Ref **) realloc(dwg->object_ref,
                                (dwg->num_object_refs + REFS_PER_REALLOC) * sizeof(Dwg_Object_Ref*));
          }
      dwg->object_ref[dwg->num_object_refs++] = ref;
    }
  else
    {
      ref->obj = 0;
      ref->absolute_ref=0;
      return ref;
    }
  //we receive a null obj when we are reading
  // handles in the header variables section
  if (!obj)
    {
      ref->absolute_ref = ref->handleref.value;
      ref->obj = 0;
      return ref;
    }
  /*
   * sometimes the code indicates the type of ownership
   * in other cases the handle is stored as an offset from some other handle
   * how is it determined?
   */
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
  return ref;
}

static Dwg_Object_Ref *
dwg_decode_handleref_with_code(Bit_Chain * dat, Dwg_Object * obj, Dwg_Data* dwg, unsigned int code)
{
  Dwg_Object_Ref * ref;
  ref = dwg_decode_handleref(dat, obj, dwg);

  if (!ref)
    {
      LOG_ERROR("dwg_decode_handleref_with_code: ref is a null pointer\n");
      return 0;
    }

  if (ref->absolute_ref == 0 && ref->handleref.code != code)
    {
      LOG_ERROR("Expected a CODE %d handle, got a %d\n", code, ref->handleref.code)
      //TODO: At the moment we are tolerating wrong codes in handles.
      // in the future we might want to get strict and return 0 here so that code will crash
      // whenever it reaches the first handle parsing error. This might make debugging easier.
      //return 0;
    }
  return ref;
}

static void
dwg_decode_header_variables(Bit_Chain* dat, Dwg_Data * dwg)
{
  Dwg_Header_Variables* _obj = &dwg->header_vars;
  Dwg_Object* obj=0;

  #include "header_variables.spec"
}

static void
dwg_decode_common_entity_handle_data(Bit_Chain * dat, Dwg_Object * obj)
{

  //XXX setup required to use macros
  Dwg_Object_Entity *ent;
  Dwg_Data *dwg = obj->parent;
  int i;
  long unsigned int vcount;
  Dwg_Object_Entity *_obj;
  ent = obj->tio.entity;
  _obj = ent;

  #include "common_entity_handle_data.spec"

}

enum RES_BUF_VALUE_TYPE
{
  VT_INVALID = 0,
  VT_STRING = 1,
  VT_POINT3D = 2,
  VT_REAL = 3,
  VT_INT16 = 4,
  VT_INT32 = 5,
  VT_INT8 = 6,
  VT_BINARY = 7,
  VT_HANDLE = 8,
  VT_OBJECTID = 9,
  VT_BOOL = 10
};

enum RES_BUF_VALUE_TYPE
get_base_value_type(short gc)
{
  if (gc >= 300)
    {
      if (gc >= 440) 
        {
          if (gc >= 1000)  // 1000-1071
            {
              if (gc == 1004) return VT_BINARY;
              if (gc <= 1009) return VT_STRING;  
              if (gc <= 1059) return VT_REAL;
              if (gc <= 1070) return VT_INT16;
              if (gc == 1071) return VT_INT32;
            }
          else            // 440-999
            {
              if (gc <= 459) return VT_INT32;
              if (gc <= 469) return VT_REAL;
              if (gc <= 479) return VT_STRING;
              if (gc <= 998) return VT_INVALID;
              if (gc == 999) return VT_STRING;
            }         
        }
      else // <440 
        {
          if (gc >= 390)  // 390-439
            {
              if (gc <= 399) return VT_HANDLE;
              if (gc <= 409) return VT_INT16;
              if (gc <= 419) return VT_STRING;
              if (gc <= 429) return VT_INT32;
              if (gc <= 439) return VT_STRING;
            }
          else            // 330-389
            {
              if (gc <= 309) return VT_STRING;
              if (gc <= 319) return VT_BINARY;
              if (gc <= 329) return VT_HANDLE;
              if (gc <= 369) return VT_OBJECTID;
              if (gc <= 389) return VT_INT16;
            }
        }
    }
  else if (gc >= 105)
    {
      if (gc >= 210)      // 210-299
        {
          if (gc <= 239) return VT_REAL;
          if (gc <= 269) return VT_INVALID;
          if (gc <= 279) return VT_INT16;
          if (gc <= 289) return VT_INT8;
          if (gc <= 299) return VT_BOOL;
        }
      else               // 105-209
        {
          if (gc == 105) return VT_HANDLE;
          if (gc <= 109) return VT_INVALID;
          if (gc <= 149) return VT_REAL;
          if (gc <= 169) return VT_INVALID;
          if (gc <= 179) return VT_INT16;
          if (gc <= 209) return VT_INVALID;
        }
    }
  else  // <105
    {
      if (gc >= 38)     // 38-102
        {
          if (gc <= 59)  return VT_REAL;
          if (gc <= 79)  return VT_INT16;
          if (gc <= 99)  return VT_INT32;
          if (gc <= 101) return VT_STRING;
          if (gc == 102) return VT_STRING;
        }
      else              // 0-37
        {
          if (gc <= 0)   return VT_INVALID;
          if (gc <= 4)   return VT_STRING;
          if (gc == 5)   return VT_HANDLE;
          if (gc <= 9)   return VT_STRING;
          if (gc <= 37)  return VT_POINT3D;
        }
    }
  return VT_INVALID;
}

Dwg_Resbuf*
dwg_decode_xdata(Bit_Chain * dat, int size)
{
  char group_code;
  Dwg_Resbuf *rbuf, *root=0, *curr=0;
  unsigned char codepage;
  long unsigned int end_address;
  char hdl[8];
  int i, length;

  static int cnt = 0;
  cnt++;

  end_address = dat->byte + (unsigned long int)size;

  while (dat->byte < end_address)
    {
      rbuf = (Dwg_Resbuf *) malloc(sizeof(Dwg_Resbuf));
      rbuf->next = 0;
      rbuf->type = bit_read_RS(dat);

      switch (get_base_value_type(rbuf->type))
        {
        case VT_STRING:
          length   = bit_read_RS(dat);          
          codepage = bit_read_RC(dat);
          if (length > 0)
            {
              rbuf->value.str = (char *)malloc((length + 1) * sizeof(char));
              for (i = 0; i < length; i++)
                rbuf->value.str[i] = bit_read_RC(dat);
              rbuf->value.str[i] = '\0';
            }
          break;
        case VT_REAL:
          rbuf->value.dbl = bit_read_RD(dat);
          break;
        case VT_BOOL:
        case VT_INT8:
          rbuf->value.i8 = bit_read_RC(dat);
          break;
        case VT_INT16:
          rbuf->value.i16 = bit_read_RS(dat);
          break;
        case VT_INT32:
          rbuf->value.i32 = bit_read_RL(dat);
          break;
        case VT_POINT3D:
          rbuf->value.pt[0] = bit_read_RD(dat);
          rbuf->value.pt[1] = bit_read_RD(dat);
          rbuf->value.pt[2] = bit_read_RD(dat);
          break;
        case VT_BINARY:
          rbuf->value.chunk.size = bit_read_RC(dat);          
          if (rbuf->value.chunk.size > 0)
            {
              rbuf->value.chunk.data = (char *)malloc(rbuf->value.chunk.size * sizeof(char));
              for (i = 0; i < rbuf->value.chunk.size; i++)
                rbuf->value.chunk.data[i] = bit_read_RC(dat);
            }
          break;
        case VT_HANDLE:
        case VT_OBJECTID:
          for (i = 0; i < 8; i++)
             rbuf->value.hdl[i] = bit_read_RC(dat);
          break;
        default:
          LOG_ERROR("Invalid group code in xdata: %d!\n", rbuf->type)
          free(rbuf);
          dat->byte = end_address;
          return root;
          break;
        }

      if (curr == 0)
        curr = root = rbuf;
      else
        {
          curr->next = rbuf;
          curr = rbuf;
        }
    }
    return root;
}

/* OBJECTS *******************************************************************/

#include<dwg.spec>


/*--------------------------------------------------------------------------------
 * Private functions which depend on the preceding
 */

/* returns 1 if object could be decoded and 0 otherwise
 */
static int
dwg_decode_variable_type(Dwg_Data * dwg, Bit_Chain * dat, Dwg_Object* obj)
{
  int i;

  if ((obj->type - 500) > dwg->num_classes)
    return 0;

  i = obj->type - 500;

  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "DICTIONARYVAR"))
    {
      dwg_decode_DICTIONARYVAR(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "ACDBDICTIONARYWDFLT"))
    {
      dwg_decode_DICTIONARYWDLFT(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "HATCH"))
    {
      dwg_decode_HATCH(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "IDBUFFER"))
    {
      dwg_decode_IDBUFFER(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "IMAGE"))
    {
      dwg_decode_IMAGE(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "IMAGEDEF"))
    {
      dwg_decode_IMAGEDEF(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "IMAGEDEF_REACTOR"))
    {
      dwg_decode_IMAGEDEFREACTOR(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "LAYER_INDEX"))
    {
      dwg_decode_LAYER_INDEX(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "LAYOUT"))
    {
      dwg_decode_LAYOUT(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "LWPLINE"))
    {
      dwg_decode_LWPLINE(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "OLE2FRAME"))
    {
      dwg_decode_OLE2FRAME(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "ACDBPLACEHOLDER"))
    {
      dwg_decode_PLACEHOLDER(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "RASTERVARIABLES"))
    {
      dwg_decode_RASTERVARIABLES(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "SORTENTSTABLE"))
    {
      dwg_decode_SORTENTSTABLE(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "SPATIAL_FILTER"))
    {
      dwg_decode_SPATIAL_FILTER(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "SPATIAL_INDEX"))
    {
      dwg_decode_SPATIAL_INDEX(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "VBA_PROJECT"))
    {
//TODO:      dwg_decode_VBA_PROJECT(dat, obj);
      return 0;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "WIPEOUTVARIABLE"))
    {
//TODO:      dwg_decode_WIPEOUTVARIABLE(dat, obj);
      return 0;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "XRECORD"))
    {
      dwg_decode_XRECORD(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "DIMASSOC"))
    {
//TODO:      dwg_decode_DIMASSOC(dat, obj);
      return 0;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "MATERIAL"))
    {
//TODO:      dwg_decode_MATERIAL(dat, obj);
      return 0;
    }

  return 0;
}

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

  if (loglevel)
      LOG_INFO("\n\n======================\nObject number: %lu",
          dwg->num_objects)

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

  LOG_INFO(" Type: %d\n", obj->type)

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
    dwg_decode__3DFACE(dat, obj);
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
  case DWG_TYPE_REGION:
    dwg_decode_REGION(dat, obj);
    break;
  case DWG_TYPE_3DSOLID:
    dwg_decode__3DSOLID(dat, obj);
    break;
  case DWG_TYPE_BODY:
    dwg_decode_BODY(dat, obj);
    break;
  case DWG_TYPE_RAY:
    dwg_decode_RAY(dat, obj);
    break;
  case DWG_TYPE_XLINE:
    dwg_decode_XLINE(dat, obj);
    break;
  case DWG_TYPE_DICTIONARY:
    dwg_decode_DICTIONARY(dat, obj);
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
  case DWG_TYPE_BLOCK_HEADER:
    dwg_decode_BLOCK_HEADER(dat, obj);
    break;
  case DWG_TYPE_LAYER_CONTROL:
    //set LAYER_CONTROL object - helps keep track of layers
    obj->parent->layer_control = obj;
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
  case DWG_TYPE_VPORT_CONTROL:
    dwg_decode_VPORT_CONTROL(dat, obj);
    break;
  case DWG_TYPE_VPORT:
    dwg_decode_VPORT(dat, obj);
    break;
  case DWG_TYPE_APPID_CONTROL:
    dwg_decode_APPID_CONTROL(dat, obj);
    break;
  case DWG_TYPE_APPID:
    dwg_decode_APPID(dat, obj);
    break;
  case DWG_TYPE_DIMSTYLE_CONTROL:
    dwg_decode_DIMSTYLE_CONTROL(dat, obj);
    break;
  case DWG_TYPE_DIMSTYLE:
    dwg_decode_DIMSTYLE(dat, obj);
    break;
  case DWG_TYPE_VP_ENT_HDR_CONTROL:
    dwg_decode_VP_ENT_HDR_CONTROL(dat, obj);
    break;
  case DWG_TYPE_VP_ENT_HDR:
    dwg_decode_VP_ENT_HDR(dat, obj);
    break;
  case DWG_TYPE_GROUP:
    dwg_decode_GROUP(dat, obj);
    break;
  case DWG_TYPE_MLINESTYLE:
    dwg_decode_MLINESTYLE(dat, obj);
    break;
  case DWG_TYPE_LWPLINE:
    dwg_decode_LWPLINE(dat, obj);
    break;
  case DWG_TYPE_HATCH:
    dwg_decode_HATCH(dat, obj);
    break;
  case DWG_TYPE_XRECORD:
    dwg_decode_XRECORD(dat, obj);
    break;
  case DWG_TYPE_PLACEHOLDER:
    dwg_decode_PLACEHOLDER(dat, obj);
    break;
  case DWG_TYPE_LAYOUT:
    dwg_decode_LAYOUT(dat, obj);
    break;
  default:
    if (!dwg_decode_variable_type(dwg, dat, obj))
      {
        LOG_INFO("Object UNKNOWN:\n")

        SINCE(R_2000)
          {
            bit_read_RL(dat);  // skip bitsize
          }

        if (!bit_read_H(dat, &obj->handle))
          {
            LOG_INFO("Object handle: %x.%x.%lx\n", 
              obj->handle.code, obj->handle.size, obj->handle.value)
          }

        obj->supertype = DWG_SUPERTYPE_UNKNOWN;
        obj->tio.unknown = (unsigned char*)malloc(obj->size);
        memcpy(obj->tio.unknown, &dat->chain[object_address], obj->size);
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

#undef IS_DECODER
