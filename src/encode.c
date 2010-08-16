/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009, 2010 Free Software Foundation, Inc.                  */
/*  Copyright (C) 2010 Thien-Thi Nguyen                                      */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * encode.c: encoding functions
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Thien-Thi Nguyen
 * modified by Till Heuschmann
 * modified by Anderson Pierre Cardoso
 */

#include "config.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "encode.h"

/* The logging level for the write (encode) path.  */
static unsigned int loglevel;

#ifdef USE_TRACING
/* This flag means we have checked the environment variable
   LIBREDWG_TRACE and set `loglevel' appropriately.  */
static bool env_var_checked_p;

#define DWG_LOGLEVEL loglevel
#endif  /* USE_TRACING */

#include "logging.h"

/*--------------------------------------------------------------------------------
 * Welcome to the dark side of the moon...
 * MACROS
 */

#define IS_ENCODER

#define ANYCODE -1
#define REFS_PER_REALLOC 100

#define FIELD(name,type)\
  bit_write_##type(dat, _obj->name);\
  if (loglevel>=2)\
    {\
        LOG_TRACE(#name ": " FORMAT_##type "\n", _obj->name)\
    }

#define FIELD_VALUE(name) _obj->name

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

#define FIELD_DD(name, _default) bit_write_DD(dat, FIELD_VALUE(name), _default);
#define FIELD_2DD(name, d1, d2) FIELD_DD(name.x, d1); FIELD_DD(name.y, d2);

#define FIELD_2RD(name) FIELD(name.x, RD); FIELD(name.y, RD);
#define FIELD_2BD(name) FIELD(name.x, BD); FIELD(name.y, BD);
#define FIELD_3RD(name) FIELD(name.x, RD); FIELD(name.y, RD); FIELD(name.z, RD);
#define FIELD_3BD(name) FIELD(name.x, BD); FIELD(name.y, BD); FIELD(name.z, BD);
#define FIELD_3DPOINT(name) FIELD_3BD(name)
#define FIELD_4BITS(name) bit_write_4BITS(dat,_obj->name);

#define FIELD_CMC(name)\
  {\
    bit_write_CMC(dat, &_obj->name);\
  }

#define FIELD_BE(name)\
bit_write_BE(dat, FIELD_VALUE(name.x), FIELD_VALUE(name.y), FIELD_VALUE(name.z));

#define FIELD_2RD_VECTOR(name, size)\
  for (vcount=0; vcount< _obj->size; vcount++)\
    {\
      FIELD_2RD(name[vcount]);\
    }

#define FIELD_2DD_VECTOR(name, size)\
  FIELD_2RD(name[0]);\
  for (vcount = 1; vcount < _obj->size; vcount++)\
    {\
      FIELD_2DD(name[vcount], FIELD_VALUE(name[vcount - 1].x), FIELD_VALUE(name[vcount - 1].y));\
    }

#define FIELD_3DPOINT_VECTOR(name, size)\
  for (vcount=0; vcount< _obj->size; vcount++)\
    {\
      FIELD_3DPOINT(name[vcount]);\
    }

#define REACTORS(code)\
  for (vcount=0; vcount<obj->tio.object->num_reactors; vcount++)\
    {\
      FIELD_HANDLE(reactors[vcount], code);\
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

//XXX need a review
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


//FIELD_VECTOR_N(name, type, size):
// writes a 'size' elements vector of data of the type indicated by 'type'
#define FIELD_VECTOR_N(name, type, size)\
  if (size>0)\
    {\
      for (vcount=0; vcount< size; vcount++)\
        {\
          bit_write_##type(dat, _obj->name[vcount]);\
          if (loglevel>=2)\
            {\
                LOG_TRACE(#name "[%d]: " FORMAT_##type "\n", vcount, _obj->name[vcount])\
            }\
        }\
    }

#define FIELD_VECTOR(name, type, size) FIELD_VECTOR_N(name, type, _obj->size)

// XXX need a review
#define FIELD_HANDLE(name, handle_code) \
    bit_write_H(dat, &_obj->name->handleref)

#define HANDLE_VECTOR_N(name, size, code)\
  for (vcount=0; vcount<size; vcount++)\
    {\
      FIELD_HANDLE(name[vcount], code);\
    }

#define HANDLE_VECTOR(name, sizefield, code) HANDLE_VECTOR_N(name, FIELD_VALUE(sizefield), code)

#define FIELD_XDATA(name, size)

#define COMMON_ENTITY_HANDLE_DATA  \
 dwg_encode_common_entity_handle_data(dat, obj);

#define REPEAT_N(times, name, type) \
  for (rcount=0; rcount<times; rcount++)

#define REPEAT(times, name, type) \
  for (rcount=0; rcount<_obj->times; rcount++)

#define REPEAT2(times, name, type) \
  for (rcount2=0; rcount2<_obj->times; rcount2++)

#define REPEAT3(times, name, type) \
  for (rcount3=0; rcount3<_obj->times; rcount3++)

//TODO unify REPEAT macros!

#define DWG_ENTITY(token) \
static void dwg_encode_##token (Dwg_Object* obj, Dwg_Entity_##token * _obj, Bit_Chain * dat)\
{\
  int vcount, rcount, rcount2, rcount3;\
	Dwg_Data* dwg = obj->parent;\
  if (loglevel)\
    LOG_INFO("Entity " #token ":\n")\

#define DWG_ENTITY_END }

#define DWG_OBJECT(token) static void dwg_encode_##token (Dwg_Object* obj, Dwg_Object_##token *_obj, Bit_Chain * dat)\
{\
  int vcount, rcount, rcount2, rcount3;\
	Dwg_Data* dwg = obj->parent;

#define DWG_OBJECT_END }

#define ENT_REACTORS(code)\
  FIELD_VALUE(reactors) = (BITCODE_H*) malloc(sizeof(BITCODE_H) * obj->tio.entity->num_reactors);\
  for (vcount=0; vcount<obj->tio.entity->num_reactors; vcount++)\
    {\
      FIELD_HANDLE(reactors[vcount], code);\
    }


/*--------------------------------------------------------------------------------*/
typedef struct
{
  long int handle;
  long int address;
  unsigned int idc;
} Object_Map;

/*--------------------------------------------------------------------------------
 * Private functions prototypes
 */

static void
dwg_encode_entity(Dwg_Object * obj, Bit_Chain * dat);
static void
dwg_encode_object(Dwg_Object * obj, Bit_Chain * dat);
static void
dwg_encode_header_variables(Bit_Chain* dat, Dwg_Data * dwg);
void
dwg_encode_handleref(Bit_Chain * dat, Dwg_Object * obj, Dwg_Data* dwg, Dwg_Object_Ref* ref);
void 
dwg_encode_handleref_with_code(Bit_Chain * dat, Dwg_Object * obj,Dwg_Data* dwg, Dwg_Object_Ref* ref, int code);


/*--------------------------------------------------------------------------------
 * Public variables
 */

/*--------------------------------------------------------------------------------
 * Public functions
 */
int
dwg_encode_chains(Dwg_Data * dwg, Bit_Chain * dat)
{
  int ckr_missing;
  long unsigned int i, j;
  long unsigned int section_address;
  unsigned char pvzbit;
  long unsigned int pvzadr;
  long unsigned int pvzadr_2;
  unsigned int ckr;
  unsigned int sekcisize = 0;
  long unsigned int last_address;
  long unsigned int last_handle;
  Object_Map *omap;
  Object_Map pvzmap;
  Dwg_Object *obj;

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

  bit_chain_alloc(dat);

  /*------------------------------------------------------------
   * Header variables
   */
  strcpy ((char *)dat->chain, version_codes[dwg->header.version]); // Chain version
  dat->byte += 6;

  for (i = 0; i < 5; i++)
    bit_write_RC(dat, 0); // Unknown section
  bit_write_RC(dat, 0x0F); // Unknown
  bit_write_RC(dat, 0x01); // Unknown
  bit_write_RL(dat, 0); // Picture address
  bit_write_RC(dat, 25); // Version
  bit_write_RC(dat, 0); // ?
  bit_write_RS(dat, dwg->header.codepage); // Codepage

  dwg->header.num_sections = 6; // hide unknownn sectionn 1 ? 
  bit_write_RL(dat, dwg->header.num_sections);
  section_address = dat->byte; // Jump to section address
  dat->byte += (dwg->header.num_sections * 9);
  bit_read_CRC(dat); // Check crc

  bit_write_sentinel(dat, dwg_sentinel(DWG_SENTINEL_HEADER_END));

  /*------------------------------------------------------------
   * Unknown section 1 
   */ 
  dwg->header.section[5].number = 5;
  dwg->header.section[5].address = 0;
  dwg->header.section[5].size = 0;
  if (dwg->header.num_sections == 6)
    {
      dwg->header.section[5].address = dat->byte;
      dwg->header.section[5].size = DWG_UNKNOWN1_SIZE;

      dwg->unknown1.size = dwg->header.section[5].size;
      dwg->unknown1.byte = dwg->unknown1.bit = 0;
      while (dat->byte + dwg->unknown1.size >= dat->size)
        bit_chain_alloc(dat);
      memcpy(&dat->chain[dat->byte], dwg->unknown1.chain, dwg->unknown1.size);
      dat->byte += dwg->unknown1.size;

    }

  /*------------------------------------------------------------
   * Picture (Pre-R13C3?)
   */

  /* Write the address of the picture
   */
  pvzadr = dat->byte;
  dat->byte = 0x0D;
  bit_write_RL(dat, pvzadr);
  dat->byte = pvzadr;

  /* Copy picture
   */
  //dwg->picture.size = 0; // If one desires not to copy pictures,
                           // should un-comment this line
  bit_write_sentinel(dat, dwg_sentinel(DWG_SENTINEL_PICTURE_BEGIN));
  for (i = 0; i < dwg->picture.size; i++)
    bit_write_RC(dat, dwg->picture.chain[i]);
  if (dwg->picture.size == 0)
    {
      bit_write_RL(dat, 5);
      bit_write_RC(dat, 0);
    }
  bit_write_sentinel(dat, dwg_sentinel(DWG_SENTINEL_PICTURE_END));

  /*------------------------------------------------------------
   * Header Variables
   */

  dwg->header.section[0].number = 0;
  dwg->header.section[0].address = dat->byte;
  bit_write_sentinel(dat, dwg_sentinel(DWG_SENTINEL_VARIABLE_BEGIN));
  pvzadr = dat->byte; // Afterwards one must rewrite the correct values of size here

  bit_write_RL(dat, 0); // Size of the section

  // encode 
  dwg_encode_header_variables(dat, dwg);

  /* Write the size of the section at its beginning
   */
  pvzadr_2 = dat->byte;
  pvzbit = dat->bit;
  dat->byte = pvzadr;
  dat->bit = 0;
  bit_write_RL(dat, pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));
  dat->byte = pvzadr_2;
  dat->bit = pvzbit;
  //printf ("Size: %lu\n", pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));

  /* CRC and sentinel
   */
  bit_write_CRC(dat, pvzadr, 0xC0C1);
  bit_write_sentinel(dat, dwg_sentinel(DWG_SENTINEL_VARIABLE_END));
  dwg->header.section[0].size = dat->byte - dwg->header.section[0].address;

  /*------------------------------------------------------------
   * Classes
   */
  dwg->header.section[1].number = 1;
  dwg->header.section[1].address = dat->byte;
  bit_write_sentinel(dat, dwg_sentinel(DWG_SENTINEL_CLASS_BEGIN));
  pvzadr = dat->byte; // Afterwards one must rewrite the correct values of size here
  bit_write_RL(dat, 0); // Size of the section

  for (i = 0; i < dwg->num_classes; i++)
    {
      bit_write_BS(dat, dwg->dwg_class[i].number);
      bit_write_BS(dat, dwg->dwg_class[i].version);
      bit_write_TV(dat, dwg->dwg_class[i].appname);
      bit_write_TV(dat, dwg->dwg_class[i].cppname);
      bit_write_TV(dat, dwg->dwg_class[i].dxfname);
      bit_write_B(dat, dwg->dwg_class[i].wasazombie);
      bit_write_BS(dat, dwg->dwg_class[i].item_class_id);
    }

  /* Write the size of the section at its beginning
   */
  pvzadr_2 = dat->byte;
  pvzbit = dat->bit;
  dat->byte = pvzadr;
  dat->bit = 0;
  bit_write_RL(dat, pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));
  dat->byte = pvzadr_2;
  dat->bit = pvzbit;
  //printf ("Size: %lu\n", pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));

  /* CRC and sentinel
   */
  bit_write_CRC(dat, pvzadr, 0xC0C1);

  bit_write_sentinel(dat, dwg_sentinel(DWG_SENTINEL_CLASS_END));
  dwg->header.section[1].size = dat->byte - dwg->header.section[1].address;

  /*------------------------------------------------------------
   * Objects
   */
  bit_write_RL(dat, 0x00000000); // 0xDCA Unknown bitlong inter class and objects
  pvzadr = dat->byte;

  /* Define object-map
   */
  omap = (Object_Map *) malloc(dwg->num_objects * sizeof(Object_Map));
  for (i = 0; i < dwg->num_objects; i++)
    {
      Bit_Chain nkn;
      Dwg_Handle tkt;

      /* Define the handle of each object, including unknown */
      omap[i].idc = i;
      if (dwg->object[i].supertype == DWG_SUPERTYPE_ENTITY)
        omap[i].handle = dwg->object[i].handle.value;
      else if (dwg->object[i].supertype == DWG_SUPERTYPE_OBJECT)
        omap[i].handle = dwg->object[i].handle.value;
      else if (dwg->object[i].supertype == DWG_SUPERTYPE_UNKNOWN)
        {
          nkn.chain = dwg->object[i].tio.unknown;
          nkn.size = dwg->object[i].size;
          nkn.byte = nkn.bit = 0;
          bit_read_BS(&nkn);
          bit_read_RL(&nkn);
          bit_read_H(&nkn, &tkt);
          omap[i].handle = tkt.value;
        }
      else
        omap[i].handle = 0x7FFFFFFF; /* Error! */

      /* Arrange the sequence of handles according to a growing order  */
      if (i > 0)
        {
          j = i;
          while (omap[j].handle < omap[j - 1].handle)
            {
              omap[j - 1].handle = pvzmap.handle;
              omap[j - 1].idc = pvzmap.idc;

              omap[j - 1].handle = omap[j].handle;
              omap[j - 1].idc = omap[j].idc;

              omap[j].handle = pvzmap.handle;
              omap[j].idc = pvzmap.idc;
              j--;
              if (j == 0)
                break;
            }
        }
    }
  //for (i = 0; i < dwg->num_objects; i++) printf ("Handle(%i): %lu / Idc: %u\n", i, omap[i].handle, omap[i].idc);

  /* Write the objects
   */
  for (i = 0; i < dwg->num_objects; i++)
    {
      omap[i].address = dat->byte;
      obj = &dwg->object[omap[i].idc];
      if (obj->supertype == DWG_SUPERTYPE_UNKNOWN)
        {
          bit_write_MS(dat, obj->size);
          if (dat->byte + obj->size >= dat->size - 2)
            bit_chain_alloc(dat);
          memcpy(&dat->chain[dat->byte], obj->tio.unknown, obj->size);
          dat->byte += obj->size;
        }
      else
        {
          if (obj->supertype == DWG_SUPERTYPE_ENTITY)
            dwg_encode_entity(obj, dat);
          else if (obj->supertype == DWG_SUPERTYPE_OBJECT)
            dwg_encode_object(obj, dat);
          else
            {
              fprintf(stderr, "Error: undefined (super)type of object\n");
              exit(-1);
            }
        }
      bit_write_CRC(dat, omap[i].address, 0xC0C1);
    }
  //for (i = 0; i < dwg->num_objects; i++) printf ("Trakt(%i): %6lu / Address: %08X / Idc: %u\n", i, omap[i].handle, omap[i].address, omap[i].idc);

  /* Unknown bitdouble between objects and object map
   */
  bit_write_RS(dat, 0);

  /*------------------------------------------------------------
   * Object-map
   */
  dwg->header.section[2].number = 2;
  dwg->header.section[2].address = dat->byte; // Value of size should be calculated later
  //printf ("Begin: 0x%08X\n", dat->byte);

  sekcisize = 0;
  pvzadr = dat->byte; // Correct value of section size must be written later
  dat->byte += 2;
  last_address = 0;
  last_handle = 0;
  for (i = 0; i < dwg->num_objects; i++)
    {
      unsigned int idc;
      long int pvz;

      idc = omap[i].idc;

      pvz = omap[idc].handle - last_handle;
      bit_write_MC(dat, pvz);
      //printf ("Handle(%i): %6lu / ", i, pvz);
      last_handle = omap[idc].handle;

      pvz = omap[idc].address - last_address;
      bit_write_MC(dat, pvz);
      //printf ("Address: %08X\n", pvz);
      last_address = omap[idc].address;

      ckr_missing = 1;
      if (dat->byte - pvzadr > 2030) // 2029
        {
          ckr_missing = 0;
          sekcisize = dat->byte - pvzadr;
          dat->chain[pvzadr] = sekcisize >> 8;
          dat->chain[pvzadr + 1] = sekcisize & 0xFF;
          bit_write_CRC(dat, pvzadr, 0xC0C1);

          pvzadr = dat->byte;
          dat->byte += 2;
          last_address = 0;
          last_handle = 0;
        }
    }
  //printf ("Obj size: %u\n", i);
  if (ckr_missing)
    {
      sekcisize = dat->byte - pvzadr;
      dat->chain[pvzadr] = sekcisize >> 8;
      dat->chain[pvzadr + 1] = sekcisize & 0xFF;
      bit_write_CRC(dat, pvzadr, 0xC0C1);
    }
  pvzadr = dat->byte;
  bit_write_RC(dat, 0);
  bit_write_RC(dat, 2);
  bit_write_CRC(dat, pvzadr, 0xC0C1);

  /* Calculate and write the size of the object map
   */
  dwg->header.section[2].size = dat->byte - dwg->header.section[2].address;
  free(omap);

  /*------------------------------------------------------------
   * Second header
   */
  bit_write_sentinel(dat, dwg_sentinel(DWG_SENTINEL_SECOND_HEADER_BEGIN));

  pvzadr = dat->byte; // Keep the first address of the section to write its size later
  bit_write_RL(dat, 0);

  bit_write_BL(dat, pvzadr - 16); // start_address of the section

  /* Version Code
   */
  for (i = 0; i < 6; i++)
    bit_write_RC(dat, version_codes[dat->version][i]);

  /* 5 (aux 6) null
   */
  for (i = 0; i < 5; i++) // 6 if is older
    bit_write_RC(dat, 0);

  /* 4 nulll bits
   */
  bit_write_BB(dat, 0);
  bit_write_BB(dat, 0);

  /* Fixed chain
   */
  bit_write_RC(dat, 0x0F);
  bit_write_RC(dat, 0x14);
  bit_write_RC(dat, 0x64);
  bit_write_RC(dat, 0x78);
  bit_write_RC(dat, 0x01);
  bit_write_RC(dat, 0x06);

  /* Addresses
   */
  for (i = 0; i < 6; i++)
    {
      bit_write_RC(dat, 0);
      bit_write_BL(dat, dwg->header.section[0].address);
      bit_write_BL(dat, dwg->header.section[0].size);
    }

  /* Handles
   */
  bit_write_BS(dat, 14);
  for (i = 0; i < 14; i++)
    {
      bit_write_RC(dat, dwg->second_header.handlerik[i].size);
      bit_write_RC(dat, i);
      for (j = 0; j < dwg->second_header.handlerik[i].size; j++)
        bit_write_RC(dat, dwg->second_header.handlerik[i].chain[j]);
    }

  /* Go back to begin to write the size
   */
  pvzadr_2 = dat->byte;
  dat->byte = pvzadr;
  bit_write_RL(dat, pvzadr_2 - pvzadr + 10);
  dat->byte = pvzadr_2;

  /* CRC
   */
  bit_write_CRC(dat, pvzadr, 0xC0C1);

  /* 8 garbage bytes
   */
  bit_write_RL(dat, 0);
  bit_write_RL(dat, 0);

  bit_write_sentinel(dat, dwg_sentinel(DWG_SENTINEL_SECOND_HEADER_END));

  /*------------------------------------------------------------
   * MEASUREMENT
   */
  dwg->header.section[3].number = 3;
  dwg->header.section[3].address = 0;
  dwg->header.section[3].size = 0;
  dwg->header.section[4].number = 4;
  dwg->header.section[4].address = dat->byte;
  dwg->header.section[4].size = 4;
  bit_write_RL(dat, dwg->measurement);

  /* End of the file
   */
  dat->size = dat->byte;

  /* Write section addresses
   */
  dat->byte = section_address;
  dat->bit = 0;
  for (i = 0; i < dwg->header.num_sections; i++)
    {
      bit_write_RC(dat, dwg->header.section[i].number);
      bit_write_RL(dat, dwg->header.section[i].address);
      bit_write_RL(dat, dwg->header.section[i].size);
    }

  /* Write CRC's
   */
  bit_write_CRC(dat, 0, 0);
  dat->byte -= 2;
  ckr = bit_read_CRC(dat);
  dat->byte -= 2;
  switch (dwg->header.num_sections)
    {
  case 3:
    bit_write_RS(dat, ckr ^ 0xA598);
    break;
  case 4:
    bit_write_RS(dat, ckr ^ 0x8101);
    break;
  case 5:
    bit_write_RS(dat, ckr ^ 0x3CC4);
    break;
  case 6:
    bit_write_RS(dat, ckr ^ 0x8461);
    break;
  default:
    bit_write_RS(dat, ckr);
    }

  return 0;
}

#include<dwg.spec>

static void
dwg_encode_entity(Dwg_Object * obj, Bit_Chain * dat)
{
  /*unsigned int i;
  long unsigned int size;
  Bit_Chain gdadr;
  Bit_Chain ekadr;
  Bit_Chain bgadr;
  Bit_Chain pvadr;
  Dwg_Object_Entity *ent;

  ent = obj->tio.entity;

  gdadr.byte = dat->byte;
  gdadr.bit = dat->bit;

  bit_write_MS(dat, obj->size);

  ekadr.byte = dat->byte; // In order to calculate afterwards the byte and bit size of the object
  ekadr.bit = dat->bit;

  bit_write_BS(dat, obj->type);

  bgadr.byte = dat->byte;
  bgadr.bit = dat->bit;

  bit_write_RL(dat, 0); // Zero for now. Calculate and write later

  bit_write_H(dat, &ent->object->handle);
  bit_write_BS(dat, ent->extended_size);
  if (ent->extended_size > 0)
    {
      bit_write_H(dat, &ent->extended_handle);
      for (i = 0; i < ent->extended_size; i++)
        bit_write_RC(dat, ent->extended[i]);
    }

  bit_write_B(dat, ent->picture_exists);
  if (ent->picture_exists)
    {
      bit_write_RL(dat, ent->picture_size);
      for (i = 0; i < ent->picture_size; i++)
        bit_write_RC(dat, ent->picture[i]);
    }

  bit_write_BB(dat, ent->entity_mode);
  bit_write_BL(dat, ent->num_reactors);
  bit_write_B(dat, ent->nolinks);
  bit_write_CMC(dat, &ent->color);
  bit_write_BD(dat, ent->linetype_scale);
  bit_write_BB(dat, ent->linetype_flags);
  bit_write_BB(dat, ent->plotstyle_flags);
  bit_write_BS(dat, ent->invisible);
  bit_write_RC(dat, ent->lineweight);

  switch (obj->type)
    {
  case DWG_TYPE_TEXT:
    dwg_encode_TEXT(ent->object, ent->tio.TEXT, dat);
    break;
  case DWG_TYPE_ATTRIB:
    dwg_encode_ATTRIB(ent->object, ent->tio.ATTRIB, dat);
    break;
  case DWG_TYPE_ATTDEF:
    dwg_encode_ATTDEF(ent->object, ent->tio.ATTDEF, dat);
    break;
  case DWG_TYPE_BLOCK:
    dwg_encode_BLOCK(ent->object, ent->tio.BLOCK, dat);
    break;
  case DWG_TYPE_ENDBLK:
    dwg_encode_ENDBLK(ent->object, ent->tio.ENDBLK, dat);
    break;
  case DWG_TYPE_SEQEND:
    dwg_encode_SEQEND(ent->object, ent->tio.SEQEND, dat);
    break;
  case DWG_TYPE_INSERT:
    dwg_encode_INSERT(ent->object, ent->tio.INSERT, dat);
    break;
  case DWG_TYPE_MINSERT:
    dwg_encode_MINSERT(ent->object, ent->tio.MINSERT, dat);
    break;
  case DWG_TYPE_VERTEX_2D:
    dwg_encode_VERTEX_2D(ent->object, ent->tio.VERTEX_2D, dat);
    break;
  case DWG_TYPE_VERTEX_3D:
    dwg_encode_VERTEX_3D(ent->object, ent->tio.VERTEX_3D, dat);
    break;
  case DWG_TYPE_VERTEX_MESH:
    dwg_encode_VERTEX_MESH(ent->object, ent->tio.VERTEX_MESH, dat);
    break;
  case DWG_TYPE_VERTEX_PFACE:
    dwg_encode_VERTEX_PFACE(ent->object, ent->tio.VERTEX_PFACE, dat);
    break;
  case DWG_TYPE_VERTEX_PFACE_FACE:
    dwg_encode_VERTEX_PFACE_FACE(ent->object, ent->tio.VERTEX_PFACE_FACE, dat);
    break;
  case DWG_TYPE_POLYLINE_2D:
    dwg_encode_POLYLINE_2D(ent->object, ent->tio.POLYLINE_2D, dat);
    break;
  case DWG_TYPE_POLYLINE_3D:
    dwg_encode_POLYLINE_3D(ent->object, ent->tio.POLYLINE_3D, dat);
    break;
  case DWG_TYPE_ARC:
    dwg_encode_ARC(ent->object, ent->tio.ARC, dat);
    break;
  case DWG_TYPE_CIRCLE:
    dwg_encode_CIRCLE(ent->object, ent->tio.CIRCLE, dat);
    break;
  case DWG_TYPE_LINE:
    dwg_encode_LINE(ent->object, ent->tio.LINE, dat);
    break;
  case DWG_TYPE_DIMENSION_ORDINATE:
    dwg_encode_DIMENSION_ORDINATE(ent->object, ent->tio.DIMENSION_ORDINATE, dat);
    break;
  case DWG_TYPE_DIMENSION_LINEAR:
    dwg_encode_DIMENSION_LINEAR(ent->object, ent->tio.DIMENSION_LINEAR, dat);
    break;
  case DWG_TYPE_DIMENSION_ALIGNED:
    dwg_encode_DIMENSION_ALIGNED(ent->object, ent->tio.DIMENSION_ALIGNED, dat);
    break;
  case DWG_TYPE_DIMENSION_ANG3PT:
    dwg_encode_DIMENSION_ANG3PT(ent->object, ent->tio.DIMENSION_ANG3PT, dat);
    break;
  case DWG_TYPE_DIMENSION_ANG2LN:
    dwg_encode_DIMENSION_ANG2LN(ent->object, ent->tio.DIMENSION_ANG2LN, dat);
    break;
  case DWG_TYPE_DIMENSION_RADIUS:
    dwg_encode_DIMENSION_RADIUS(ent->object, ent->tio.DIMENSION_RADIUS, dat);
    break;
  case DWG_TYPE_DIMENSION_DIAMETER:
    dwg_encode_DIMENSION_DIAMETER(ent->object, ent->tio.DIMENSION_DIAMETER, dat);
    break;
  case DWG_TYPE_POINT:
    dwg_encode_POINT(ent->object, ent->tio.POINT, dat);
    break;
  case DWG_TYPE__3DFACE:
    dwg_encode__3DFACE(ent->object, ent->tio._3DFACE, dat);
    break;
  case DWG_TYPE_POLYLINE_PFACE:
    dwg_encode_POLYLINE_PFACE(ent->object, ent->tio.POLYLINE_PFACE, dat);
    break;
  case DWG_TYPE_POLYLINE_MESH:
    dwg_encode_POLYLINE_MESH(ent->object, ent->tio.POLYLINE_MESH, dat);
    break;
  case DWG_TYPE_SOLID:
    dwg_encode_SOLID(ent->object, ent->tio.SOLID, dat);
    break;
  case DWG_TYPE_TRACE:
    dwg_encode_TRACE(ent->object, ent->tio.TRACE, dat);
    break;
  case DWG_TYPE_SHAPE:
    dwg_encode_SHAPE(ent->object, ent->tio.SHAPE, dat);
    break;
  case DWG_TYPE_VIEWPORT:
    dwg_encode_VIEWPORT(ent->object, ent->tio.VIEWPORT, dat);
    break;
  case DWG_TYPE_ELLIPSE:
    dwg_encode_ELLIPSE(ent->object, ent->tio.ELLIPSE, dat);
    break;
  case DWG_TYPE_SPLINE:
    dwg_encode_SPLINE(ent->object, ent->tio.SPLINE, dat);
    break;
  case DWG_TYPE_REGION:
    dwg_encode_REGION(ent->object, ent->tio.REGION, dat);
    break;
  case DWG_TYPE_3DSOLID:
    dwg_encode__3DSOLID(ent->object, ent->tio._3DSOLID, dat);
    break;
  case DWG_TYPE_BODY:
    dwg_encode_BODY(ent->object, ent->tio.BODY, dat);
    break;
  case DWG_TYPE_RAY:
    dwg_encode_RAY(ent->object, ent->tio.RAY, dat);
    break;
  case DWG_TYPE_XLINE:
    dwg_encode_XLINE(ent->object, ent->tio.XLINE, dat);
    break;
  case DWG_TYPE_MTEXT:
    dwg_encode_MTEXT(ent->object, ent->tio.MTEXT, dat);
    break;
  case DWG_TYPE_LEADER:
    dwg_encode_LEADER(ent->object, ent->tio.LEADER, dat);
    break;
  case DWG_TYPE_TOLERANCE:
    dwg_encode_TOLERANCE(ent->object, ent->tio.TOLERANCE, dat);
    break;
  case DWG_TYPE_MLINE:
    dwg_encode_MLINE(ent->object, ent->tio.MLINE, dat);
    break;
    /* TODO: figure out how to deal with these types 
     case DWG_TYPE_IMAGE:
     //dwg_encode_IMAGE (ent->object, ent->tio.IMAGE, dat);
     break;
     case DWG_TYPE_LWPLINE:
     //dwg_encode_LWPLINE (ent->object, ent->tio.LWPLINE, dat);
     break;
     case DWG_TYPE_OLE2FRAME:
     //dwg_encode_OLE2FRAME (ent->object, ent->tio.OLE2FRAME, dat);
     break;
     case DWG_TYPE_TABLE:
     //dwg_encode_TABLE (ent->object, ent->tio.TABLE, dat);
     break;
     * /
  default:
    LOG_ERROR("Error: unknown object-type while encoding entity\n")
    //for now encode something null and skip (FIXME)
    dwg_encode_UNUSED(ent->object, ent->tio.UNUSED, dat);
    //exit(-1);
    }

  /* Finally calculate and write the bit-size of the object
   * /
  pvadr.byte = dat->byte;
  pvadr.bit = dat->bit;

  dat->byte = bgadr.byte;
  dat->bit = bgadr.bit;

  size = 8 * (pvadr.byte - ekadr.byte) + (pvadr.bit);
  bit_write_RL(dat, size);
  //printf ("Size (bit): %lu\t", size);

  dat->byte = pvadr.byte;
  dat->bit = pvadr.bit;

  /* Handle references
   * /
  //FIXME write new handle encoding routines like print_handleref
  //for (i = 0; i < ent->num_handles; i++)
    //bit_write_H (dat, &ent->handleref[i]);

    /* Finally calculate and write the bit-size of the object
     * /
    pvadr.byte = dat->byte;
  pvadr.bit = dat->bit;

  dat->byte = gdadr.byte;
  dat->bit = gdadr.bit;

  size = pvadr.byte - ekadr.byte;
  bit_write_MS(dat, size);
  //printf ("Size: %lu\n", size);

  dat->byte = pvadr.byte;
  dat->bit = pvadr.bit;
  */
  
  //XXX not sure about this
   Dwg_Object_Entity *ent;
   
   SINCE(R_2000)
    {
      bit_write_RL(dat, 0);
    }

   bit_write_H(dat, &(obj->handle));
}

void
dwg_encode_common_entity_handle_data(Bit_Chain * dat, Dwg_Object * obj)
{
  //XXX: not sure about this
  
  //setup required to use macros
  Dwg_Object_Entity *ent;
  Dwg_Data *dwg = obj->parent;
  int i;
  long unsigned int vcount;
  Dwg_Object_Entity *_obj;
  ent = obj->tio.entity;
  _obj = ent;

  //#include "common_entity_handle_data.spec"
  
}

void
dwg_encode_handleref(Bit_Chain * dat, Dwg_Object * obj, Dwg_Data* dwg, Dwg_Object_Ref* ref)
{
  //this function should receive a Object_Ref without an abs_ref, calculate it and return a Dwg_Handle
  //this should be a higher level function 
  //not sure if the prototype is correct
}

void 
dwg_encode_handleref_with_code(Bit_Chain * dat, Dwg_Object * obj,Dwg_Data* dwg, Dwg_Object_Ref* ref, int code){
  //XXX fixme. will this function be necessary?
  //create the handle, then check the code. will it be necessary?
  dwg_encode_handleref(dat, obj, dwg, ref);
  if (ref->handleref.code != code){
    LOG_INFO("warning: trying to write handle with wrong code. Expected code=%d, got %d.\n", code, ref->handleref.code)
  }
};

static void
dwg_encode_object(Dwg_Object * obj, Bit_Chain * dat)
{
  Bit_Chain ekadr;

  bit_write_MS(dat, obj->size);
  ekadr.byte = dat->byte; // Calculate later the bit size of the object
  ekadr.bit = dat->bit;
  //bit_write_BS(dat, obj->type);
  bit_write_BS(dat, 0);
  
}

static void
dwg_encode_header_variables(Bit_Chain* dat, Dwg_Data * dwg)
{
  Dwg_Header_Variables* _obj = &dwg->header_vars;
  Dwg_Object* obj=0;

  #include "header_variables.spec"
}
#undef IS_ENCODER
