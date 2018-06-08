/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009, 2010, 2018 Free Software Foundation, Inc.            */
/*  Copyright (C) 2010 Thien-Thi Nguyen                                      */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * encode.c: encoding functions to write a DWG
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Thien-Thi Nguyen
 * modified by Till Heuschmann
 * modified by Anderson Pierre Cardoso
 * modified by Reini Urban
 */

#include "config.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1 /* for strdup */
#endif
#include <string.h>
#include <assert.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "encode.h"
#include "decode.h"

/* The logging level for the write (encode) path.  */
static unsigned int loglevel;
/* the current version per spec block */
static unsigned int cur_ver = 0;

#ifdef USE_TRACING
/* This flag means we have checked the environment variable
   LIBREDWG_TRACE and set `loglevel' appropriately.  */
static bool env_var_checked_p;
#endif  /* USE_TRACING */
#define DWG_LOGLEVEL loglevel

#include "logging.h"

/*--------------------------------------------------------------------------------
 * spec MACROS
 */

#define IS_ENCODER

#define ANYCODE -1
#define REFS_PER_REALLOC 100

#define VALUE(value,type,dxf) \
  { bit_write_##type(dat, value); \
    LOG_TRACE(FORMAT_##type " [" #type " %d]\n", value, dxf); }
#define VALUE_RC(value,dxf) VALUE(value, RC, dxf)
#define VALUE_RS(value,dxf) VALUE(value, RS, dxf)
#define VALUE_RL(value,dxf) VALUE(value, RL, dxf)
#define VALUE_RD(value,dxf) VALUE(value, RD, dxf)

#define FIELD(name,type)\
  { bit_write_##type(dat, _obj->name); \
    FIELD_TRACE(name, type); }
#define FIELDG(name,type,dxf) \
  { bit_write_##type(dat, _obj->name); \
    FIELD_G_TRACE(name, type, dxf); }
#define FIELD_TRACE(name,type) \
  LOG_TRACE(#name ": " FORMAT_##type "\n", _obj->name)
#define FIELD_G_TRACE(name,type,dxfgroup) \
  LOG_TRACE(#name ": " FORMAT_##type " [" #type " " #dxfgroup "]\n", _obj->name)
#define FIELD_CAST(name,type,cast,dxf)                    \
  { bit_write_##type(dat, (BITCODE_##type)_obj->name); \
    FIELD_G_TRACE(name,cast,dxf); }

#define FIELD_VALUE(name) _obj->name

#define FIELD_B(name,dxf) FIELDG(name, B, dxf)
#define FIELD_BB(name,dxf) FIELDG(name, BB, dxf)
#define FIELD_3B(name,dxf) FIELDG(name, 3B, dxf)
#define FIELD_BS(name,dxf) FIELDG(name, BS, dxf)
#define FIELD_BL(name,dxf) FIELDG(name, BL, dxf)
#define FIELD_BLL(name,dxf) FIELDG(name, BLL, dxf)
#define FIELD_BD(name,dxf) FIELDG(name, BD, dxf)
#define FIELD_RC(name,dxf) FIELDG(name, RC, dxf)
#define FIELD_RS(name,dxf) FIELDG(name, RS, dxf)
#define FIELD_RD(name,dxf) FIELDG(name, RD, dxf)
#define FIELD_RL(name,dxf) FIELDG(name, RL, dxf)
#define FIELD_RLL(name,dxf) FIELDG(name, RLL, dxf)
#define FIELD_MC(name,dxf) FIELDG(name, MC, dxf)
#define FIELD_MS(name,dxf) FIELDG(name, MS, dxf)
#define FIELD_TV(name,dxf) \
  { IF_ENCODE_FROM_EARLIER { _obj->name = strdup(""); } FIELDG(name, TV, dxf); }
#define FIELD_T(name,dxf) \
  { if (dat->version < R_2007) { \
      FIELD_TV(name,dxf) \
    } else { \
      if (obj->has_strings) { \
        FIELD_TU(name,dxf) \
      } else { \
        LOG_TRACE_TU(#name, L"", dxf); \
      } \
    } \
  }
#define FIELD_TF(name,len,dxf) \
  { if (_obj->name && len) bit_write_TF(dat, _obj->name, len); \
    FIELD_G_TRACE(name, TF, dxf); }
#define FIELD_TFF(name,len,dxf) FIELD_TF(name,len,dxf)
#define FIELD_TU(name,dxf) \
  { if (_obj->name) bit_write_TU(dat, (BITCODE_TU)_obj->name); \
    LOG_TRACE_TU(#name, (BITCODE_TU)_obj->name,dxf); }
#define FIELD_BT(name, dxf) FIELDG(name, BT, dxf);

#define FIELD_DD(name, _default, dxf) bit_write_DD(dat, FIELD_VALUE(name), _default);
#define FIELD_2DD(name, d1, d2, dxf) { FIELD_DD(name.x, d1, dxf); FIELD_DD(name.y, d2, dxf+10); }
#define FIELD_3DD(name, def, dxf) { \
    FIELD_DD(name.x, FIELD_VALUE(def.x), dxf); \
    FIELD_DD(name.y, FIELD_VALUE(def.y), dxf+10); \
    FIELD_DD(name.z, FIELD_VALUE(def.z), dxf+20); }
#define FIELD_2RD(name,dxf) { FIELDG(name.x, RD, dxf); FIELDG(name.y, RD, dxf+10); }
#define FIELD_2BD(name,dxf) { FIELDG(name.x, BD, dxf); FIELDG(name.y, BD, dxf+10); }
#define FIELD_2BD_1(name,dxf) { FIELDG(name.x, BD, dxf); FIELDG(name.y, BD, dxf+1); }
#define FIELD_3RD(name,dxf) { FIELDG(name.x, RD, dxf); FIELDG(name.y, RD, dxf+10); \
                              FIELDG(name.z, RD,dxf+20); }
#define FIELD_3BD(name,dxf) { FIELDG(name.x, BD, dxf); FIELDG(name.y, BD, dxf+10); \
                              FIELDG(name.z, BD, dxf+20); }
#define FIELD_3BD_1(name,dxf) { FIELDG(name.x, BD, dxf); FIELDG(name.y, BD, dxf+1); \
                                FIELDG(name.z, BD, dxf+2); }
#define FIELD_3DPOINT(name,dxf) FIELD_3BD(name,dxf)
#define FIELD_4BITS(name,dxf) bit_write_4BITS(dat,_obj->name);
#define FIELD_TIMEBLL(name,dxf) \
  { bit_write_TIMEBLL(dat, (BITCODE_TIMEBLL)_obj->name); \
    LOG_TRACE(#name ": " FORMAT_BL "." FORMAT_BL "\n", _obj->name.days, _obj->name.ms); }

#define FIELD_CMC(name, dxf) bit_write_CMC(dat, &_obj->name)

#define FIELD_BE(name, dxf)\
  bit_write_BE(dat, FIELD_VALUE(name.x), FIELD_VALUE(name.y), FIELD_VALUE(name.z));

#define FIELD_2RD_VECTOR(name, size, dxf)                   \
  for (vcount=0; vcount < (long)_obj->size; vcount++)\
    {\
      FIELD_2RD(name[vcount], dxf);\
    }

#define FIELD_2DD_VECTOR(name, size, dxf)\
  FIELD_2RD(name[0], dxf); \
  for (vcount = 1; vcount < (long)_obj->size; vcount++)\
    {\
      FIELD_2DD(name[vcount], FIELD_VALUE(name[vcount - 1].x), FIELD_VALUE(name[vcount - 1].y), dxf);\
    }

#define FIELD_3DPOINT_VECTOR(name, size, dxf)\
  for (vcount=0; vcount < (long)_obj->size; vcount++)   \
    {\
      FIELD_3DPOINT(name[vcount], dxf);\
    }

#define REACTORS(code)\
  if (dat->version >= R_2000 && obj->tio.object->num_reactors > 0x1000) { \
    fprintf(stderr, "Invalid num_reactors: %ld\n", (long)obj->tio.object->num_reactors); return; } \
  for (vcount=0; vcount < (long)obj->tio.object->num_reactors; vcount++) \
    {\
      FIELD_HANDLE_N(reactors[vcount], vcount, code, -5);    \
    }
    
#define XDICOBJHANDLE(code)\
  RESET_VER \
  SINCE (R_2004)\
    {\
      if (!obj->tio.object->xdic_missing_flag) \
        {\
          FIELD_HANDLE(xdicobjhandle, code, 0); \
        }\
    }\
  PRIOR_VERSIONS\
    {\
      FIELD_HANDLE(xdicobjhandle, code, 0); \
    } \
  RESET_VER

#define ENT_XDICOBJHANDLE(code)\
  RESET_VER \
  SINCE (R_2004)\
    {\
      if (!obj->tio.entity->xdic_missing_flag)\
        {\
          FIELD_HANDLE(xdicobjhandle, code, 0);   \
        }\
    }\
  PRIOR_VERSIONS\
    {\
      FIELD_HANDLE(xdicobjhandle, code, 0); \
    } \
  RESET_VER

//FIELD_VECTOR_N(name, type, size):
// writes a 'size' elements vector of data of the type indicated by 'type'
#define FIELD_VECTOR_N(name, type, size, dxf)\
  if (size > 0)\
    {\
      for (vcount=0; vcount < (long)size; vcount++)\
        {\
          bit_write_##type(dat, _obj->name[vcount]);\
          if (loglevel>=2)\
            {\
              LOG_TRACE(#name "[%ld]: " FORMAT_##type "\n", (long)vcount, _obj->name[vcount]) \
            }\
        }\
    }
#define FIELD_VECTOR_T(name, size, dxf)\
  if (_obj->size > 0)\
    {\
      for (vcount=0; vcount < (long)_obj->size; vcount++)\
        {\
          PRE (R_2007) { \
            bit_write_TV(dat, _obj->name[vcount]);\
            LOG_TRACE(#name "[%d]: %s\n", (int)vcount, _obj->name[vcount]) \
          } else { \
            bit_write_TU(dat, (BITCODE_TU)_obj->name[vcount]); \
            LOG_TRACE_TU(#name, _obj->name[vcount], dxf) \
          } \
        }\
      RESET_VER \
    }

#define FIELD_VECTOR(name, type, size, dxf) \
  FIELD_VECTOR_N(name, type, _obj->size, dxf)

#define FIELD_HANDLE(name, handle_code, dxf) \
  IF_ENCODE_SINCE_R13 { \
    RESET_VER \
    if (!_obj->name) { \
      Dwg_Handle null_handle = {0,0,0}; \
      bit_write_H(hdl_dat, &null_handle); \
      LOG_TRACE(#name ": HANDLE(0.0.0) absolute:0 [%d]\n", dxf) \
    } else { \
      if (handle_code != ANYCODE && _obj->name->handleref.code != handle_code) \
        { \
          LOG_WARN("Expected a CODE %d handle, got a %d", \
                    handle_code, _obj->name->handleref.code); \
        } \
      bit_write_H(hdl_dat, &_obj->name->handleref); \
      LOG_TRACE(#name ": HANDLE(%d.%d.%lu) absolute:%lu [%d]\n", \
                _obj->name->handleref.code,\
                _obj->name->handleref.size,\
                _obj->name->handleref.value,\
                _obj->name->absolute_ref, dxf) \
      } \
    }
#define FIELD_DATAHANDLE(name, handle_code, dxf) \
  { bit_write_H(dat, _obj->name ? &_obj->name->handleref : NULL); }

#define FIELD_HANDLE_N(name, vcount, handle_code, dxf)\
    IF_ENCODE_SINCE_R13 { \
      RESET_VER \
      if (!_obj->name) \
        bit_write_H(hdl_dat, NULL); \
      else { \
        if (handle_code != ANYCODE && _obj->name->handleref.code != handle_code) \
          { \
            LOG_WARN("Expected a CODE %d handle, got a %d", \
                      handle_code, _obj->name->handleref.code); \
          } \
        bit_write_H(hdl_dat, &_obj->name->handleref); \
        LOG_TRACE(#name "[%d]: HANDLE(%d.%d.%lu) absolute:%lu [%d]\n", \
                  (int)vcount, \
                  _obj->name->handleref.code,\
                  _obj->name->handleref.size,\
                  _obj->name->handleref.value,\
                  _obj->name->absolute_ref, dxf) \
      }\
    }

#define HANDLE_VECTOR_N(name, size, code, dxf)\
  if (size>0) \
    assert(_obj->name); \
  for (vcount=0; vcount < (long)size; vcount++)\
    {\
      assert(_obj->name[vcount]); \
      FIELD_HANDLE_N(name[vcount], vcount, code, dxf);   \
    }

#define FIELD_NUM_INSERTS(num_inserts, type, dxf)   \
  FIELD_RL(num_inserts, dxf)

#define HANDLE_VECTOR(name, sizefield, code, dxf) \
  HANDLE_VECTOR_N(name, FIELD_VALUE(sizefield), code, dxf)

#define FIELD_XDATA(name, size) \
  dwg_encode_xdata(dat, _obj, _obj->size)

#define COMMON_ENTITY_HANDLE_DATA  \
  SINCE(R_13) {\
    dwg_encode_common_entity_handle_data(dat, hdl_dat, obj); \
  } \
  RESET_VER

#define SECTION_STRING_STREAM \
  { \
    Bit_Chain sav_dat = *dat; \
    dat = str_dat;

/* TODO: dump all TU strings here */
#define START_STRING_STREAM \
  bit_write_B(dat, obj->has_strings); \
  RESET_VER \
  if (obj->has_strings) { \
    Bit_Chain sav_dat = *dat; \
    obj_string_stream(dat, obj, dat);

#define END_STRING_STREAM \
    *dat = sav_dat; \
  }
#define START_HANDLE_STREAM \
  *hdl_dat = *dat; \
  if (dat->version >= R_2007) bit_set_position(hdl_dat, obj->hdlpos); \
  RESET_VER

/** Returns -1 if not added, else returns the new objid.
   Does a complete handleref rescan to invalidate and resolve
   all internal obj pointers after a object[] realloc.
*/
#define DWG_ENTITY(token) \
EXPORT long dwg_add_##token (Dwg_Data * dwg)    \
{                                               \
  Bit_Chain dat;                                \
  BITCODE_BL num_objs  = dwg->num_objects;      \
  dat.size = sizeof(Dwg_Entity_##token) + 40;   \
  dat.chain = calloc(dat.size, 1);              \
  dat.version = dwg->header.version;            \
  dat.from_version = dwg->header.from_version;  \
  bit_write_MS(&dat, dat.size);                 \
  if (dat.version >= R_2010) {                  \
    bit_write_MC(&dat, 8*sizeof(Dwg_Entity_##token)); \
    bit_write_BOT(&dat, DWG_TYPE_##token);      \
  } else {                                      \
    bit_write_BS(&dat, DWG_TYPE_##token);       \
  }                                             \
  bit_set_position(&dat, 0);                    \
  if (-1 == dwg_decode_add_object(dwg, &dat, &dat, 0)) \
    dwg_resolve_objectrefs_silent(dwg);         \
  if (num_objs == dwg->num_objects)             \
    return -1;                                  \
  else                                          \
    return (long)dwg->num_objects;              \
} \
\
static void dwg_encode_##token (Bit_Chain *restrict dat, Dwg_Object *restrict obj) \
{ \
  long vcount, rcount1, rcount2, rcount3, rcount4; \
  Dwg_Data* dwg = obj->parent; \
  Dwg_Object_Entity *_ent = obj->tio.entity; \
  Dwg_Entity_##token * _obj = _ent->tio.token; \
  Bit_Chain* hdl_dat = dat; \
  Bit_Chain* str_dat = dat; \
  if (dwg_encode_entity(obj, dat, hdl_dat, str_dat)) return; \
  LOG_INFO("Entity " #token ":\n")

#define DWG_ENTITY_END \
  if (obj->bitsize == 0 && dat->version >= R_13 && dat->version <= R_2010) \
    { \
      unsigned long address = bit_position(dat); \
      unsigned long bitsize = address - obj->bitsize_address; \
      bit_set_position(dat, obj->bitsize_address); \
      bit_write_RL(dat, bitsize); \
      bit_set_position(dat, address); \
      /* CRC? */ \
    } \
}

/** Returns -1 if not added, else returns the new objid.
   Does a complete handleref rescan to invalidate and resolve
   all internal obj pointers after a object[] realloc.
*/
#define DWG_OBJECT(token) \
EXPORT long dwg_add_##token (Dwg_Data * dwg)     \
{                                                \
  Bit_Chain dat;                                 \
  BITCODE_BL num_objs  = dwg->num_objects;       \
  dat.size = sizeof(Dwg_Object_##token) + 40;    \
  dat.chain = calloc(dat.size, 1);               \
  dat.version = dwg->header.version;             \
  dat.from_version = dwg->header.from_version;   \
  bit_write_MS(&dat, dat.size);                  \
  if (dat.version >= R_2010) {                   \
    bit_write_MC(&dat, 8*sizeof(Dwg_Object_##token)); \
    bit_write_BOT(&dat, DWG_TYPE_##token);       \
  } else {                                       \
    bit_write_BS(&dat, DWG_TYPE_##token);        \
  }                                              \
  bit_set_position(&dat, 0);                     \
  if (-1 == dwg_decode_add_object(dwg, &dat, &dat, 0)) \
    dwg_resolve_objectrefs_silent(dwg);          \
  if (num_objs == dwg->num_objects)              \
    return -1;                                   \
  else                                           \
    return (long)dwg->num_objects;               \
} \
\
static void dwg_encode_##token (Bit_Chain *restrict dat, Dwg_Object *restrict obj) \
{ \
  long vcount, rcount1, rcount2, rcount3, rcount4; \
  Dwg_Data* dwg = obj->parent; \
  Bit_Chain* hdl_dat = dat; \
  Bit_Chain* str_dat = dat; \
  Dwg_Object_##token * _obj = obj->tio.object->tio.token; \
  if (dwg_encode_object(obj, dat, hdl_dat, str_dat)) return; \
  LOG_INFO("Object " #token " handle: %d.%d.%lX\n",\
    obj->handle.code, \
    obj->handle.size, \
    obj->handle.value)

#define DWG_OBJECT_END \
  if (obj->bitsize == 0 && dat->version >= R_13 && dat->version <= R_2007) \
    { \
      unsigned long address = bit_position(dat); \
      unsigned long bitsize = address - obj->bitsize_address; \
      bit_set_position(dat, obj->bitsize_address); \
      bit_write_RL(dat, bitsize); \
      bit_set_position(dat, address); \
      /* CRC? */ \
    } \
}

#define ENT_REACTORS(code)\
  if (dat->version >= R_2000 && _obj->num_reactors > 0x1000) { \
    fprintf(stderr, "Invalid num_reactors: %ld\n", (long)_obj->num_reactors); return; } \
  for (vcount=0; vcount < _obj->num_reactors; vcount++)\
    {\
      FIELD_HANDLE_N(reactors[vcount], vcount, code, -5); \
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
static int
encode_preR13(Dwg_Data *restrict dwg, Bit_Chain *restrict dat);

static int
dwg_encode_entity(Dwg_Object *restrict obj,
                  Bit_Chain* dat, Bit_Chain* hdl_dat, Bit_Chain* str_dat);
static int
dwg_encode_object(Dwg_Object *restrict obj,
                  Bit_Chain *restrict dat, Bit_Chain* hdl_dat, Bit_Chain* str_dat);
static void
dwg_encode_common_entity_handle_data(Bit_Chain* dat, Bit_Chain* hdl_dat,
                                     Dwg_Object *restrict obj);
static int
dwg_encode_header_variables(Bit_Chain* dat, Bit_Chain* hdl_dat, Bit_Chain* str_dat,
                            Dwg_Data* dwg);
static int
dwg_encode_variable_type(Dwg_Data *restrict dwg, Bit_Chain *restrict dat, Dwg_Object *restrict obj);
void
dwg_encode_handleref(Bit_Chain *hdl_dat, Dwg_Object * obj, Dwg_Data *restrict dwg,
                     Dwg_Object_Ref *restrict ref);
void 
dwg_encode_handleref_with_code(Bit_Chain* hdl_dat, Dwg_Object* obj, Dwg_Data* dwg,
                               Dwg_Object_Ref *restrict ref, unsigned int code);
void
dwg_encode_add_object(Dwg_Object *restrict obj, Bit_Chain *restrict dat, unsigned long address);

static void
dwg_encode_xdata(Bit_Chain *restrict dat, Dwg_Object_XRECORD *restrict obj, int size);

/*--------------------------------------------------------------------------------
 * Public functions
 */

/**
 * dwg_encode(): the current generic encoder entry point.
 *
 * TODO: preR13 tables, 2007 maps.
 * 2010+ uses the 2004 format.
 */
int
dwg_encode(Dwg_Data *restrict dwg, Bit_Chain *restrict dat)
{
  int ckr_missing = 1;
  int i;
  long unsigned int j;
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
  Bit_Chain *hdl_dat;

  if (dwg->opts)
    loglevel = dwg->opts & 0xf;
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
  hdl_dat = dat;

  /*------------------------------------------------------------
   * Header
   */
  strcpy ((char *)dat->chain, version_codes[dwg->header.version]); // Chain version
  dat->byte += 6;

  {
    struct Dwg_Header* _obj = &dwg->header;
    Dwg_Object *obj = NULL;

    #include "header.spec"
  }
  section_address = dat->byte;

#define WE_CAN \
    "This version of LibreDWG is only capable of encoding " \
    "version R13-R2000 (code: AC1012-AC1015) DWG files.\n"

  PRE(R_13)
    {
      // TODO: tables, entities, block entities
      LOG_ERROR(WE_CAN "We don't encode tables, entities, blocks yet")
#ifndef IS_RELEASE
      return encode_preR13(dwg, dat);
#endif
    }

  PRE(R_2004) {
    if (!dwg->header.num_sections) /* Usually 3-5, max 6 */
      dwg->header.num_sections = 6;
    bit_write_RL(dat, dwg->header.num_sections);
    if (!dwg->header.section)
      dwg->header.section = calloc(dwg->header.num_sections, sizeof(Dwg_Section));
    section_address = dat->byte; // Jump to section address
    dat->byte += (dwg->header.num_sections * 9);
    bit_write_CRC(dat, 0, 0xC0C1);

    bit_write_sentinel(dat, dwg_sentinel(DWG_SENTINEL_HEADER_END));

    /*------------------------------------------------------------
     * AuxHeader section 5
     * R2000+, mostly redundant file header information
     */
    dwg->header.section[5].number = 5;
    dwg->header.section[5].address = dat->byte;
    //dwg->header.section[5].size = 0;
    if (dwg->header.num_sections == 6)
      {
        struct Dwg_AuxHeader* _obj = &dwg->auxheader;
        Dwg_Object *obj = NULL;

        #include "auxheader.spec"
      }
  }

  VERSION(R_2007)
    {
      LOG_ERROR(WE_CAN "We don't encode R2007 sections yet")
      return DWG_ERR_NOTYETSUPPORTED;
    }

  /* r2004 file header (compressed + encrypted) */
  SINCE(R_2004)
  {
    /* System Section */
    typedef union _system_section
    {
      unsigned char data[0x14]; // 20byte: 5*4
      struct
      {
        uint32_t section_type;   /* 0x4163043b */
        uint32_t decomp_data_size;
        uint32_t comp_data_size;
        uint32_t compression_type;
        uint32_t checksum;
      } fields;
    } system_section;

    system_section ss;
    Dwg_Section *section;

    Dwg_Object *obj = NULL;
    struct Dwg_R2004_Header* _obj = &dwg->r2004_header;
    const int size = sizeof(struct Dwg_R2004_Header);
    char encrypted_data[size];
    unsigned int rseed = 1;

    LOG_ERROR(WE_CAN "We don't encode the R2004_section_map yet")

    if (dwg->header.num_infos && !dwg->header.section_info)
      dwg->header.section_info = calloc(dwg->header.num_infos, sizeof(Dwg_Section_Info));

    dat->byte = 0x80;
    for (i = 0; i < size; i++)
      {
        rseed *= 0x343fd;
        rseed += 0x269ec3;
        encrypted_data[i] = bit_read_RC(dat) ^ (rseed >> 0x10);
      }
    LOG_TRACE("\n#### Write 2004 File Header ####\n");
    dat->byte = 0x80;
    if (dat->byte+0x80 >= dat->size - 1) {
      dat->size = dat->byte + 0x80;
      bit_chain_alloc(dat);
    }
    memcpy(&dat->chain[0x80], encrypted_data, size);
    LOG_INFO("@0x%lx\n", dat->byte);

    #include "r2004_file_header.spec"

    /*-------------------------------------------------------------------------
     * Section Page Map
     */
    dat->byte = dwg->r2004_header.section_map_address + 0x100;

    LOG_TRACE("\n=== Write System Section (Section Page Map) ===\n");
#ifndef HAVE_COMPRESS_R2004_SECTION
    dwg->r2004_header.comp_data_size   = dwg->r2004_header.decomp_data_size;
    dwg->r2004_header.compression_type = 0;
#endif
    FIELD_RL(section_type, 0); // should be 0x4163043b
    FIELD_RL(decomp_data_size, 0);
    FIELD_RL(comp_data_size, 0);
    FIELD_RL(compression_type, 0);
    FIELD_RL(checksum, 0);
    LOG_TRACE("\n")

    LOG_WARN("TODO write_R2004_section_map(dat, dwg)")
    LOG_TRACE("\n")
    return DWG_ERR_NOTYETSUPPORTED;
  }

  /*------------------------------------------------------------
   * Picture (Pre-R13C3?)
   */
  if (dwg->header.preview_addr)
    {
      dat->byte = dwg->header.preview_addr;
      //dwg->picture.size = 0; // If one desires not to copy preview pictures,
      // should un-comment this line
      bit_write_sentinel(dat, dwg_sentinel(DWG_SENTINEL_PICTURE_BEGIN));
      bit_write_TF(dat, (char *)dwg->picture.chain, dwg->picture.size);
      if (dwg->picture.size == 0)
        {
          bit_write_RL(dat, 5);
          bit_write_RC(dat, 0);
        }
      bit_write_sentinel(dat, dwg_sentinel(DWG_SENTINEL_PICTURE_END));
    }

  /*------------------------------------------------------------
   * Header Variables
   */
  dwg->header.section[0].number = 0;
  dwg->header.section[0].address = dat->byte;
  bit_write_sentinel(dat, dwg_sentinel(DWG_SENTINEL_VARIABLE_BEGIN));
  pvzadr = dat->byte; // Afterwards one must rewrite the correct values of size here

  bit_write_RL(dat, 0); // Size of the section

  // encode
  //if (dat->version >= R_2007)
  //  str_dat = dat;
  dwg_encode_header_variables(dat, hdl_dat, dat, dwg);

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

  //XXX trying to fix CRC 2-byte overflow. Must find actual reason
  dat->byte -= 2;

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

  for (j = 0; j < dwg->num_classes; j++)
    {
      Dwg_Class *klass;
      klass = &dwg->dwg_class[j];
      bit_write_BS(dat, klass->number);
      bit_write_BS(dat, klass->proxyflag);
      bit_write_TV(dat, klass->appname);
      bit_write_TV(dat, klass->cppname);
      bit_write_TV(dat, klass->dxfname);
      bit_write_B(dat,  klass->wasazombie);
      bit_write_BS(dat, klass->item_class_id);
      LOG_TRACE("Class %d 0x%x %s\n"
                "%s \"%s\" %d 0x%x\n",
                klass->number, klass->proxyflag, klass->dxfname,
                klass->cppname, klass->appname,
                klass->wasazombie, klass->item_class_id)

      SINCE(R_2007)
        {
          bit_write_BL(dat, klass->num_instances);
          bit_write_BL(dat, klass->dwg_version);
          bit_write_BL(dat, klass->maint_version);
          bit_write_BL(dat, klass->unknown_1);
          bit_write_BL(dat, klass->unknown_2);
        }
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

  bit_write_RL(dat, 0x00000000); // 0xDCA Unknown bitlong inter class and objects

  /*------------------------------------------------------------
   * Objects
   */

  pvzadr = dat->byte;

  /* Define object-map
   */
  omap = (Object_Map *) malloc(dwg->num_objects * sizeof(Object_Map));
  if (!omap) {
    LOG_ERROR("Out of memory");
    return DWG_ERR_OUTOFMEM;
  }
  for (j = 0; j < dwg->num_objects; j++)
    {

      /* Define the handle of each object, including unknown */
      omap[j].idc = j;
      if (dwg->object[j].supertype == DWG_SUPERTYPE_ENTITY)
        omap[j].handle = dwg->object[j].handle.value;
      else if (dwg->object[j].supertype == DWG_SUPERTYPE_OBJECT)
        omap[j].handle = dwg->object[j].handle.value;
      else if (dwg->object[j].supertype == DWG_SUPERTYPE_UNKNOWN)
        omap[j].handle = dwg->object[j].handle.value;
      else
        omap[j].handle = 0x7FFFFFFF; /* Error! */

      /* Arrange the sequence of handles according to a growing order  */
      if (j > 0)
        {
          unsigned long k = j;
          while (omap[k].handle < omap[k - 1].handle)
            {
              pvzmap.handle = omap[k].handle;
              pvzmap.idc    = omap[k].handle;

              omap[k - 1].handle = pvzmap.handle;
              omap[k - 1].idc    = pvzmap.idc;

              omap[k].handle = omap[k - 1].handle;
              omap[k].idc    = omap[k - 1].idc;

              k--;
              if (k == 0)
                break;
            }
        }
    }
  //for (i = 0; i < dwg->num_objects; i++)
  //  printf ("Handle(%i): %lu / Idc: %u\n", i, omap[i].handle, omap[i].idc);

  /* Write the objects
   */
  for (j = 0; j < dwg->num_objects; j++)
    {
      Dwg_Object *obj;
      omap[j].address = dat->byte;
      obj = &dwg->object[omap[j].idc];
      obj->address = dat->byte; // change the address to the linearily sorted one
      LOG_TRACE("\n> Next object: %lu\tHandle: %lu\tOffset: %lu\n"
                "==========================================\n",
                j, omap[j].handle, dat->byte);

      if (obj->supertype == DWG_SUPERTYPE_UNKNOWN)
        {
          if (dat->byte + obj->size >= dat->size)
            bit_chain_alloc(dat);
          LOG_INFO("Unknown object, size %d, type %d\n", obj->size, obj->type);
          bit_write_MS(dat, obj->size);
          memcpy(&dat->chain[dat->byte], obj->tio.unknown, obj->size);
          dat->byte += obj->size;
        }
      else
        {
	  if (obj->supertype == DWG_SUPERTYPE_ENTITY ||
              obj->supertype == DWG_SUPERTYPE_OBJECT)
	    dwg_encode_add_object(obj, dat, dat->byte);
	  /*
          if (obj->supertype == DWG_SUPERTYPE_ENTITY)
            dwg_encode_entity(obj, dat, dat, dat);
          else if (obj->supertype == DWG_SUPERTYPE_OBJECT)
            dwg_encode_object(obj, dat, dat, dat);
	  */
          else
            {
              LOG_ERROR("Error: undefined (super)type of object");
              exit(-1);
            }
        }
      bit_write_CRC(dat, omap[j].address, 0xC0C1);
    }
    for (j = 0; j < dwg->num_objects; j++) 
      LOG_INFO ("Object(%lu): %6lu / Address: %08lX / Idc: %u\n", 
		 j, omap[j].handle, omap[j].address, omap[j].idc);

  /* Unknown bitdouble between objects and object map (or short?)
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
  for (j = 0; j < dwg->num_objects; j++)
    {
      unsigned int idc;
      long int pvz;

      idc = omap[j].idc;

      pvz = omap[idc].handle - last_handle;
      bit_write_MC(dat, pvz);
      //printf ("Handle(%i): %6lu / ", j, pvz);
      last_handle = omap[idc].handle;

      pvz = omap[idc].address - last_address;
      bit_write_MC(dat, pvz);
      //printf ("Address: %08X\n", pvz);
      last_address = omap[idc].address;

      //dwg dwg_encode_add_object(dwg->object[j], dat, last_address);

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
   * Second header, section 3. R13-R2000 only.
   * But partially also since r2004.
   */
  SINCE(R_13)
  {
    struct _dwg_second_header* _obj = &dwg->second_header;
    Dwg_Object * obj = NULL;
    long vcount;

    dwg->header.section[3].number = 3;
    dwg->header.section[3].address = dwg->second_header.address;
    dwg->header.section[3].size = dwg->second_header.size;
    bit_write_sentinel(dat, dwg_sentinel(DWG_SENTINEL_SECOND_HEADER_BEGIN));

    LOG_INFO("\n=======> Second Header: %8X\n", (unsigned int) dat->byte - 16);
    pvzadr = dat->byte; // Keep the first address of the section to write its size later
    LOG_TRACE("pvzadr: %lx\n", pvzadr)

      FIELD_RL(size, 0);
    if (FIELD_VALUE(address) != (BITCODE_RL)(pvzadr - 16))
      {
        LOG_WARN("second_header->address %x != %x",
                 FIELD_VALUE(address), (unsigned)(pvzadr - 16));
        FIELD_VALUE(address) = pvzadr - 16;
      }
    FIELD_BL(address, 0);

    // AC1012, AC1014 or AC1015. This is a char[11], zero padded.
    // with \n at 12.
    bit_write_TF(dat, _obj->version, 12);
    LOG_TRACE("version: %s\n", _obj->version)

    for (i = 0; i < 4; i++)
      FIELD_B(null_b[i], 0);
    FIELD_RC(unknown_10, 0); // 0x10
    for (i = 0; i < 4; i++)
      FIELD_RC(unknown_rc4[i], 0);

    UNTIL (R_2000) {
      FIELD_RC(num_sections, 0); // r14: 5, r2000: 6
      for (i = 0; i < FIELD_VALUE(num_sections); i++)
        {
          FIELD_RC(section[i].nr, 0);
          FIELD_BL(section[i].address, 0);
          FIELD_BL(section[i].size, 0);
        }

      FIELD_BS(num_handlers, 0); // 14, resp. 16 in r14
      if (FIELD_VALUE(num_handlers) > 16) {
        LOG_ERROR("Second header num_handlers > 16: %d\n", FIELD_VALUE(num_handlers));
        FIELD_VALUE(num_handlers) = 14;
      }
      for (i = 0; i < FIELD_VALUE(num_handlers); i++)
        {
          FIELD_RC(handlers[i].size, 0);
          FIELD_RC(handlers[i].nr, 0);
          FIELD_VECTOR(handlers[i].data, RC, handlers[i].size, 0);
        }

      bit_write_CRC(dat, pvzadr, 0xC0C1);

      VERSION(R_14) {
        FIELD_RL(junk_r14_1, 0);
        FIELD_RL(junk_r14_2, 0);
      }
    }
    bit_write_sentinel(dat, dwg_sentinel(DWG_SENTINEL_SECOND_HEADER_END));

  } else if (dwg->header.num_sections >= 3) {
    dwg->header.section[3].number = 3;
    dwg->header.section[3].address = 0;
    dwg->header.section[3].size = 0;
  }

  /*------------------------------------------------------------
   * MEASUREMENT Section 4
   */
  if (dwg->header.num_sections >= 4)
    {
      dwg->header.section[4].number = 4;
      dwg->header.section[4].address = dat->byte;
      dwg->header.section[4].size = 4;
      bit_write_RL(dat, dwg->measurement);
    }

  /* End of the file
   */
  dat->size = dat->byte;

  /* Write section addresses (XXX pre2004 only?)
   */
  dat->byte = section_address;
  dat->bit = 0;
  for (j = 0; j < dwg->header.num_sections; j++)
    {
      bit_write_RC(dat, dwg->header.section[j].number);
      bit_write_RL(dat, dwg->header.section[j].address);
      bit_write_RL(dat, dwg->header.section[j].size);
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

static int
encode_preR13(Dwg_Data* dwg, Bit_Chain* dat)
{
  return DWG_ERR_NOTYETSUPPORTED;
}


#include "dwg.spec"

/** dwg_encode_variable_type
 * Encode object by class name, not type. if type > 500.
 * Returns 1 if object could be encoded and 0 otherwise.
 * FIXME: pass thru error code
 */
static int
dwg_encode_variable_type(Dwg_Data* dwg, Bit_Chain* dat, Dwg_Object* obj)
{
  int i;
  char *dxfname;
  int is_entity;
  Dwg_Class *klass;

  if ((obj->type - 500) > dwg->num_classes)
    {
      LOG_WARN("Invalid object type %d, only %d classes", obj->type, dwg->num_classes);
      return 0;
    }

  i = obj->type - 500;
  klass = &dwg->dwg_class[i];
  dxfname = obj->dxfname = klass->dxfname;
  // almost always false
  is_entity = dwg_class_is_entity(klass);

#define UNHANDLED_CLASS \
      LOG_WARN("Unhandled Class %s %d %s (0x%x%s)", is_entity ? "entity" : "object",\
               klass->number, dxfname, klass->proxyflag,\
               klass->wasazombie ? " was proxy" : "")
#define UNTESTED_CLASS \
      LOG_WARN("Untested Class %s %d %s (0x%x%s)", is_entity ? "entity" : "object",\
               klass->number, dxfname, klass->proxyflag,\
               klass->wasazombie ? " was proxy" : "")

  if (!strcmp(dxfname, "ACDBDICTIONARYWDFLT"))
    {
      assert(!is_entity);
      dwg_encode_DICTIONARYWDLFT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "DICTIONARYVAR"))
    {
      assert(!is_entity);
      dwg_encode_DICTIONARYVAR(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "HATCH"))
    {
      assert(!is_entity);
      dwg_encode_HATCH(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "GROUP"))
    {
      assert(!is_entity);
      dwg_encode_GROUP(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IDBUFFER"))
    {
      assert(!is_entity);
      dwg_encode_IDBUFFER(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IMAGE"))
    {
      assert(is_entity);
      dwg_encode_IMAGE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IMAGEDEF"))
    {
      assert(!is_entity);
      dwg_encode_IMAGEDEF(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IMAGEDEF_REACTOR"))
    {
      assert(!is_entity);
      dwg_encode_IMAGEDEF_REACTOR(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "LAYER_INDEX"))
    {
      assert(!is_entity);
      dwg_encode_LAYER_INDEX(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "LAYOUT"))
    {
      assert(!is_entity);
      dwg_encode_LAYOUT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "LWPOLYLINE"))
    {
      assert(!is_entity);
      dwg_encode_LWPOLYLINE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "OLE2FRAME"))
    {
      assert(!is_entity);
      dwg_encode_OLE2FRAME(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "OBJECTCONTEXTDATA")
      || !strcmp(klass->cppname, "AcDbObjectContextData"))
    {
      assert(!is_entity);
      dwg_encode_OBJECTCONTEXTDATA(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "OBJECT_PTR")
      || !strcmp(klass->cppname, "CAseDLPNTableRecord"))
    {
      assert(!is_entity);
      dwg_encode_OBJECT_PTR(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "ACDBPLACEHOLDER"))
    {
      assert(!is_entity);
      dwg_encode_PLACEHOLDER(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "PROXY"))
    {
      assert(!is_entity);
      dwg_encode_PROXY_OBJECT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "RASTERVARIABLES"))
    {
      assert(!is_entity);
      dwg_encode_RASTERVARIABLES(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SORTENTSTABLE"))
    {
      assert(!is_entity);
      dwg_encode_SORTENTSTABLE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SPATIAL_FILTER"))
    {
      assert(!is_entity);
      dwg_encode_SPATIAL_FILTER(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SPATIAL_INDEX"))
    {
      assert(!is_entity);
      dwg_encode_SPATIAL_INDEX(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "TABLE"))
    {
      assert(is_entity);
      dwg_encode_TABLE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "XRECORD"))
    {
      assert(!is_entity);
      dwg_encode_XRECORD(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "WIPEOUT"))
    {
      assert(is_entity);
      dwg_encode_WIPEOUT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "FIELDLIST"))
    {
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_encode_FIELDLIST(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SCALE"))
    {
      assert(!is_entity);
      dwg_encode_SCALE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "MULTILEADER"))
    {
      assert(is_entity);
#ifdef DEBUG_MULTILEADER
      UNTESTED_CLASS; // broken decoder
      dwg_encode_MULTILEADER(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      return 0;
#endif
    }
  if (!strcmp(dxfname, "MLEADERSTYLE"))
    {
      assert(!is_entity);
      dwg_encode_MLEADERSTYLE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "AcDbField")) //???
    {
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_encode_FIELD(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "GEODATA"))
    {
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_encode_GEODATA(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "VBA_PROJECT"))
    {
      assert(!is_entity);
#ifdef DEBUG_VBA_PROJECT
      // Has its own section?
      UNTESTED_CLASS;
      dwg_encode_VBA_PROJECT(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      return 0;
#endif
    }
  if (!strcmp(dxfname, "WIPEOUTVARIABLES"))
    {
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_encode_WIPEOUTVARIABLES(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "CELLSTYLEMAP"))
    {
      assert(!is_entity);
#ifdef DEBUG_CELLSTYLEMAP
      UNTESTED_CLASS; //broken
      dwg_encode_CELLSTYLEMAP(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      return 0;
#endif
    }
  if (!strcmp(dxfname, "VISUALSTYLE"))
    {
      assert(!is_entity);
      dwg_encode_VISUALSTYLE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "ACDBSECTIONVIEWSTYLE"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_encode_SECTIONVIEWSTYLE(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDBDETAILVIEWSTYLE"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_encode_DETAILVIEWSTYLE(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "DIMASSOC"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_encode_DIMASSOC(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "MATERIAL"))
    {
#ifdef DEBUG_MATERIAL
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_encode_MATERIAL(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      assert(!is_entity);
      return 0;
#endif
    }
  if (!strcmp(dxfname, "PLOTSETTINGS"))
    {
#ifdef DEBUG_PLOTSETTINGS
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_encode_PLOTSETTINGS(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      assert(!is_entity);
      return 0;
#endif
    }
  if (!strcmp(dxfname, "LIGHT"))
    {
#ifdef DEBUG_LIGHT
      UNTESTED_CLASS;
      assert(is_entity);
      dwg_encode_LIGHT(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      assert(is_entity);
      return 0;
#endif
    }
  if (!strcmp(dxfname, "SUN"))
    {
#ifdef DEBUG_SUN // i.e. test/test-data/2000/1.dwg, referenced by VIEW 361
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_encode_SUN(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      assert(!is_entity);
      return 0;
#endif
    }
  if (!strcmp(dxfname, "GEOPOSITIONMARKER"))
    {
#ifdef DEBUG_GEOPOSITIONMARKER
      UNTESTED_CLASS;
      assert(is_entity);
      dwg_encode_GEOPOSITIONMARKER(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      assert(is_entity);
      return 0;
#endif
    }
  if (!strcmp(dxfname, "EXTRUDEDSURFACE"))
    {
#ifdef DEBUG_EXTRUDEDSURFACE
      UNTESTED_CLASS;
      assert(is_entity);
      dwg_encode_EXTRUDEDSURFACE(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      assert(is_entity);
      return 0;
#endif
    }
  if (!strcmp(dxfname, "SCALE"))
    {
      UNTESTED_CLASS;
      assert(!is_entity);
      //SCALE has a name, bitsizes: 199,207,215,343,335,351,319
      dwg_encode_SCALE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "TABLEGEOMETRY"))
    {
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_encode_TABLEGEOMETRY(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "TABLECONTENT"))
    {
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_encode_TABLECONTENT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "TABLESTYLE"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_encode_TABLESTYLE(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "DBCOLOR"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_encode_DBCOLOR(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDBASSOCNETWORK"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_encode_ASSOCNETWORK(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDBASSOC2DCONSTRAINTGROUP"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_encode_ASSOC2DCONSTRAINTGROUP(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDBASSOCGEOMDEPENDENCY"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_encode_ASSOCGEOMDEPENDENCY(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDB_LEADEROBJECTCONTEXTDATA_CLASS"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_encode_LEADEROBJECTCONTEXTDATA(dat, obj);
      return 0;
    }

  LOG_WARN("Unknown Class %s %d %s (0x%x%s)", is_entity ? "entity" : "object", \
           klass->number, dxfname, klass->proxyflag,                    \
           klass->wasazombie ? " was proxy" : "")

#undef UNHANDLED_CLASS
#undef UNTESTED_CLASS

  return 0;
}

void
dwg_encode_add_object(Dwg_Object* obj, Bit_Chain* dat,
                      unsigned long address)
{
  unsigned long previous_address;
  unsigned long object_address;
  unsigned char previous_bit;

  previous_address = dat->byte;
  previous_bit = dat->bit;
  dat->byte = address;
  dat->bit = 0;

  LOG_INFO("Object number: %u", obj->index);
  if (dat->byte + obj->size >= dat->size)
    bit_chain_alloc(dat);
  // TODO: calculate size from the fields. either <0x7fff or more
  bit_write_MS(dat, obj->size);

  PRE(R_2010) {
    bit_write_BS(dat, obj->type);
  } LATER_VERSIONS {
    bit_write_BOT(dat, obj->type);
  }

  LOG_INFO(", Size: %d, Type: %d\n", obj->size, obj->type)

  /* Check the type of the object
   */
  switch (obj->type)
    {
  case DWG_TYPE_TEXT:
    dwg_encode_TEXT(dat, obj);
    break;
  case DWG_TYPE_ATTRIB:
    dwg_encode_ATTRIB(dat, obj);
    break;
  case DWG_TYPE_ATTDEF:
    dwg_encode_ATTDEF(dat, obj);
    break;
  case DWG_TYPE_BLOCK:
    dwg_encode_BLOCK(dat, obj);
    break;
  case DWG_TYPE_ENDBLK:
    dwg_encode_ENDBLK(dat, obj);
    break;
  case DWG_TYPE_SEQEND:
    dwg_encode_SEQEND(dat, obj);
    break;
  case DWG_TYPE_INSERT:
    dwg_encode_INSERT(dat, obj);
    break;
  case DWG_TYPE_MINSERT:
    dwg_encode_MINSERT(dat, obj);
    break;
  case DWG_TYPE_VERTEX_2D:
    dwg_encode_VERTEX_2D(dat, obj);
    break;
  case DWG_TYPE_VERTEX_3D:
    dwg_encode_VERTEX_3D(dat, obj);
    break;
  case DWG_TYPE_VERTEX_MESH:
    dwg_encode_VERTEX_MESH(dat, obj);
    break;
  case DWG_TYPE_VERTEX_PFACE:
    dwg_encode_VERTEX_PFACE(dat, obj);
    break;
  case DWG_TYPE_VERTEX_PFACE_FACE:
    dwg_encode_VERTEX_PFACE_FACE(dat, obj);
    break;
  case DWG_TYPE_POLYLINE_2D:
    dwg_encode_POLYLINE_2D(dat, obj);
    break;
  case DWG_TYPE_POLYLINE_3D:
    dwg_encode_POLYLINE_3D(dat, obj);
    break;
  case DWG_TYPE_ARC:
    dwg_encode_ARC(dat, obj);
    break;
  case DWG_TYPE_CIRCLE:
    dwg_encode_CIRCLE(dat, obj);
    break;
  case DWG_TYPE_LINE:
    dwg_encode_LINE(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_ORDINATE:
    dwg_encode_DIMENSION_ORDINATE(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_LINEAR:
    dwg_encode_DIMENSION_LINEAR(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_ALIGNED:
    dwg_encode_DIMENSION_ALIGNED(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_ANG3PT:
    dwg_encode_DIMENSION_ANG3PT(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_ANG2LN:
    dwg_encode_DIMENSION_ANG2LN(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_RADIUS:
    dwg_encode_DIMENSION_RADIUS(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_DIAMETER:
    dwg_encode_DIMENSION_DIAMETER(dat, obj);
    break;
  case DWG_TYPE_POINT:
    dwg_encode_POINT(dat, obj);
    break;
  case DWG_TYPE__3DFACE:
    dwg_encode__3DFACE(dat, obj);
    break;
  case DWG_TYPE_POLYLINE_PFACE:
    dwg_encode_POLYLINE_PFACE(dat, obj);
    break;
  case DWG_TYPE_POLYLINE_MESH:
    dwg_encode_POLYLINE_MESH(dat, obj);
    break;
  case DWG_TYPE_SOLID:
    dwg_encode_SOLID(dat, obj);
    break;
  case DWG_TYPE_TRACE:
    dwg_encode_TRACE(dat, obj);
    break;
  case DWG_TYPE_SHAPE:
    dwg_encode_SHAPE(dat, obj);
    break;
  case DWG_TYPE_VIEWPORT:
    dwg_encode_VIEWPORT(dat, obj);
    break;
  case DWG_TYPE_ELLIPSE:
    dwg_encode_ELLIPSE(dat, obj);
    break;
  case DWG_TYPE_SPLINE:
    dwg_encode_SPLINE(dat, obj);
    break;
  case DWG_TYPE_REGION:
    dwg_encode_REGION(dat, obj);
    break;
  case DWG_TYPE__3DSOLID:
    dwg_encode__3DSOLID(dat, obj);
    break;
  case DWG_TYPE_BODY:
    dwg_encode_BODY(dat, obj);
    break;
  case DWG_TYPE_RAY:
    dwg_encode_RAY(dat, obj);
    break;
  case DWG_TYPE_XLINE:
    dwg_encode_XLINE(dat, obj);
    break;
  case DWG_TYPE_DICTIONARY:
    dwg_encode_DICTIONARY(dat, obj);
    break;
  case DWG_TYPE_MTEXT:
    dwg_encode_MTEXT(dat, obj);
    break;
  case DWG_TYPE_LEADER:
    dwg_encode_LEADER(dat, obj);
    break;
  case DWG_TYPE_TOLERANCE:
    dwg_encode_TOLERANCE(dat, obj);
    break;
  case DWG_TYPE_MLINE:
    dwg_encode_MLINE(dat, obj);
    break;
  case DWG_TYPE_BLOCK_CONTROL:
    dwg_encode_BLOCK_CONTROL(dat, obj);
    break;
  case DWG_TYPE_BLOCK_HEADER:
    dwg_encode_BLOCK_HEADER(dat, obj);
    break;
  case DWG_TYPE_LAYER_CONTROL:
    dwg_encode_LAYER_CONTROL(dat, obj);
    break;
  case DWG_TYPE_LAYER:
    dwg_encode_LAYER(dat, obj);
    break;
  case DWG_TYPE_STYLE_CONTROL:
    dwg_encode_STYLE_CONTROL(dat, obj);
    break;
  case DWG_TYPE_STYLE:
    dwg_encode_STYLE(dat, obj);
    break;
  case DWG_TYPE_LTYPE_CONTROL:
    dwg_encode_LTYPE_CONTROL(dat, obj);
    break;
  case DWG_TYPE_LTYPE:
    dwg_encode_LTYPE(dat, obj);
    break;
  case DWG_TYPE_VIEW_CONTROL:
    dwg_encode_VIEW_CONTROL(dat, obj);
    break;
  case DWG_TYPE_VIEW:
    dwg_encode_VIEW(dat, obj);
    break;
  case DWG_TYPE_UCS_CONTROL:
    dwg_encode_UCS_CONTROL(dat, obj);
    break;
  case DWG_TYPE_UCS:
    dwg_encode_UCS(dat, obj);
    break;
  case DWG_TYPE_VPORT_CONTROL:
    dwg_encode_VPORT_CONTROL(dat, obj);
    break;
  case DWG_TYPE_VPORT:
    dwg_encode_VPORT(dat, obj);
    break;
  case DWG_TYPE_APPID_CONTROL:
    dwg_encode_APPID_CONTROL(dat, obj);
    break;
  case DWG_TYPE_APPID:
    dwg_encode_APPID(dat, obj);
    break;
  case DWG_TYPE_DIMSTYLE_CONTROL:
    dwg_encode_DIMSTYLE_CONTROL(dat, obj);
    break;
  case DWG_TYPE_DIMSTYLE:
    dwg_encode_DIMSTYLE(dat, obj);
    break;
  case DWG_TYPE_VPORT_ENTITY_CONTROL:
    dwg_encode_VPORT_ENTITY_CONTROL(dat, obj);
    break;
  case DWG_TYPE_VPORT_ENTITY_HEADER:
    dwg_encode_VPORT_ENTITY_HEADER(dat, obj);
    break;
  case DWG_TYPE_GROUP:
    dwg_encode_GROUP(dat, obj);
    break;
  case DWG_TYPE_MLINESTYLE:
    dwg_encode_MLINESTYLE(dat, obj);
    break;
  case DWG_TYPE_OLE2FRAME:
    dwg_encode_OLE2FRAME(dat, obj);
    break;
  case DWG_TYPE_DUMMY:
    dwg_encode_DUMMY(dat, obj);
    break;
  case DWG_TYPE_LONG_TRANSACTION:
    dwg_encode_LONG_TRANSACTION(dat, obj);
    break;
  case DWG_TYPE_LWPOLYLINE:
    dwg_encode_LWPOLYLINE(dat, obj);
    break;
  case DWG_TYPE_HATCH:
    dwg_encode_HATCH(dat, obj);
    break;
  case DWG_TYPE_XRECORD:
    dwg_encode_XRECORD(dat, obj);
    break;
  case DWG_TYPE_PLACEHOLDER:
    dwg_encode_PLACEHOLDER(dat, obj);
    break;
  case DWG_TYPE_PROXY_ENTITY:
    dwg_encode_PROXY_ENTITY(dat, obj);
    break;
  case DWG_TYPE_OLEFRAME:
    dwg_encode_OLEFRAME(dat, obj);
    break;
  case DWG_TYPE_VBA_PROJECT:
    LOG_ERROR("Unhandled Object VBA_PROJECT. Has its own section");
    //dwg_encode_VBA_PROJECT(dat, obj);
    break;
  case DWG_TYPE_LAYOUT:
    dwg_encode_LAYOUT(dat, obj);
    break;
  default:
      if (obj->type == obj->parent->layout_number)
        dwg_encode_LAYOUT(dat, obj);

      else if (!dwg_encode_variable_type(obj->parent, dat, obj))
        {
          Dwg_Data *dwg = obj->parent;
          int is_entity;
          int i = obj->type - 500;
          Dwg_Class *klass = NULL;

          dat->byte = address;   // restart and write into the UNKNOWN_OBJ object
          dat->bit = 0;
          bit_write_MS(dat, obj->size); // unknown blobs have a known size
          bit_write_BS(dat, obj->type); // type

          if (i <= (int)dwg->num_classes)
            {
              klass = &dwg->dwg_class[i];
              is_entity = dwg_class_is_entity(klass);
            }
          // properly dwg_decode_object/_entity for eed, reactors, xdic
          if (klass && !is_entity)
            {
              dwg_encode_UNKNOWN_OBJ(dat, obj);
            }
          else if (klass)
            {
              dwg_encode_UNKNOWN_ENT(dat, obj);
            }
          else // not a class
            {
              LOG_WARN("Unknown object, skipping eed/reactors/xdic");
              SINCE(R_2000)
              {
                bit_write_RL(dat, obj->bitsize);
                LOG_INFO("Object bitsize: " FORMAT_RL " @%lu.%u\n", obj->bitsize,
                         dat->byte, dat->bit);
              }
              bit_write_H(dat, &(obj->handle));
              LOG_INFO("Object handle: %d.%d.%lX\n",
                       obj->handle.code, obj->handle.size, obj->handle.value);
              // write obj->size bytes, excl. bitsize and handle.
              // overshoot the bitsize and handle size.
              bit_write_TF(dat, (char*)obj->tio.unknown, obj->size);
            }
        }
    }

  /*
   if (obj->supertype != DWG_SUPERTYPE_UNKNOWN)
   {
     fprintf (stderr, "Begin address:\t%10lu\n", address);
     fprintf (stderr, "Last address:\t%10lu\tSize: %10lu\n", dat->byte, obj->size);
     fprintf (stderr, "End address:\t%10lu (calculated)\n", address + 2 + obj->size);
   }
   */

  /* Now 1 padding bits until next byte, and then a RS CRC */
  if (dat->bit) {
    for (int i=dat->bit; i<8; i++) {
      bit_write_B(dat, 1);
    }
  }
  bit_write_CRC(dat, address, 0xC0C1);

  {
    unsigned long next_addr = address + obj->size + 4;
    if (next_addr != dat->byte)
      {
        if (obj->size)
          LOG_WARN("Wrong object size: %lu + %u + 4 != %lu: %ld off",
                   address, obj->size, dat->byte,
                   (long)(next_addr - dat->byte));
        dat->byte = next_addr;
      }
  }
  dat->bit = 0;
}

/* The first common part of every entity.

   The last common part is common_entity_handle_data.spec
   called by COMMON_ENTITY_HANDLE_DATA in dwg.spec
   See DWG_SUPERTYPE_ENTITY in dwg_encode().
 */
static int
dwg_encode_entity(Dwg_Object* obj,
                  Bit_Chain* hdl_dat, Bit_Chain* str_dat, Bit_Chain* dat)
{
  BITCODE_BS i;
  Dwg_Object_Entity* ent = obj->tio.entity;

  SINCE(R_2000)
    {
      obj->bitsize_address = bit_position(dat);
      if (!obj->bitsize)
        bit_write_RL(dat, obj->size * 8);
      else
        bit_write_RL(dat, obj->bitsize);
    }
  bit_write_H(dat, &(obj->handle));

  for (i = 0; i < ent->num_eed; i++)
    {
      BITCODE_BS size;
      size = ent->eed[i].size;
      bit_write_BS(dat, size);
      LOG_TRACE("EED[%u] size: " FORMAT_BS "\n", i, size);
      if (size) // not all eed's have a new handle
        {
          bit_write_H(dat, &(ent->eed[i].handle));
          LOG_TRACE("EED[%u] code: %d\n", i, (int)ent->eed[i].data->code);
          bit_write_TF(dat, ent->eed[i].raw, size);
        }
    }
  bit_write_BS(dat, 0);

  bit_write_B(dat, ent->picture_exists);
  if (ent->picture_exists)
    {
      VERSIONS(R_13,R_2007)
        {
          bit_write_RL(dat, (BITCODE_RL)ent->picture_size);
        }
      SINCE(R_2007)
        {
          bit_write_BLL(dat, ent->picture_size);
        }
      if (ent->picture_size < 210210)
        {
          bit_write_TF(dat, ent->picture, ent->picture_size);
          /*
          for (i=0; i< ent->picture_size; i++)
            bit_write_RC(dat, ent->picture[i]);
          */
        }
      else 
        {
          LOG_ERROR(
              "dwg_encode_entity:  Absurd! Picture-size: %ld kB. "
              "Object: %lu (handle).",
              (long)(ent->picture_size / 1000), obj->handle.value)
          bit_advance_position(dat, -(4 * 8 + 1));
        }
     }
  
  VERSIONS(R_13, R_14)
    {
      obj->bitsize_address = bit_position(dat);
      if (!obj->bitsize)
        bit_write_RL(dat, obj->size * 8);
      else
        bit_write_RL(dat, obj->bitsize);
    }

  bit_write_BB(dat, ent->entity_mode);
  bit_write_BL(dat, ent->num_reactors);

  SINCE(R_2004)
    {
     bit_write_B(dat, ent->xdic_missing_flag);
    }

  SINCE(R_2013)
    {
      bit_write_B(dat, ent->has_ds_binary_data);
    }

  VERSIONS(R_13,R_14)
    {
      bit_write_B(dat, ent->isbylayerlt );
    }

  bit_write_B(dat, ent->nolinks );
  bit_write_CMC(dat, &ent->color);
  bit_write_BD(dat, ent->linetype_scale);

  SINCE(R_2000)
    {
       bit_write_BB(dat, ent->linetype_flags);
       bit_write_BB(dat, ent->plotstyle_flags);
    }

  SINCE(R_2007)
    {
       bit_write_BB(dat, ent->material_flags);
       bit_write_RC(dat, ent->shadow_flags);
    }

  SINCE(R_2010)
    {
      bit_write_B(dat, ent->has_full_visualstyle);
      bit_write_B(dat, ent->has_face_visualstyle);
      bit_write_B(dat, ent->has_edge_visualstyle);
    }

   bit_write_BS(dat, ent->invisible);

  SINCE(R_2000)
    {
       bit_write_RC(dat, ent->lineweight);
    }
  return 0;
}

static void
dwg_encode_common_entity_handle_data(Bit_Chain* dat, Bit_Chain* hdl_dat,
                                     Dwg_Object* obj)
{
  Dwg_Object_Entity *ent;
  //Dwg_Data *dwg = obj->parent;
  Dwg_Object_Entity *_obj;
  int i;
  long unsigned int vcount;
  ent = obj->tio.entity;
  _obj = ent;

  #include "common_entity_handle_data.spec"
  
}

void
dwg_encode_handleref(Bit_Chain* hdl_dat, Dwg_Object* obj, Dwg_Data* dwg, Dwg_Object_Ref* ref)
{
  //this function should receive a Object_Ref without an abs_ref, calculate it and return a Dwg_Handle
  //this should be a higher level function 
  //not sure if the prototype is correct
  assert(obj);
}

/**
 * code:
 *  TYPEDOBJHANDLE:
 *   2 Soft owner
 *   3 Hard owner
 *   4 Soft pointer
 *   5 Hard pointer
 *  OFFSETOBJHANDLE for soft owners or pointers:
 *   6 ref + 1
 *   8 ref - 1
 *   a ref + offset
 *   c ref - offset
 */
void 
dwg_encode_handleref_with_code(Bit_Chain* hdl_dat, Dwg_Object* obj, Dwg_Data* dwg,
                               Dwg_Object_Ref* ref, unsigned int code)
{
  //XXX fixme. create the handle, then check the code. allow relative handle soft codes.
  dwg_encode_handleref(hdl_dat, obj, dwg, ref);
  if (ref->absolute_ref == 0 && ref->handleref.code != code)
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
        case 0: // ignore (ANYCODE)
          ref->absolute_ref = ref->handleref.value;
          break;
        default:
          LOG_WARN("Invalid handle pointer code %d", ref->handleref.code);
          break;
        }
    }
}

/* The first common part of every object.

   There is no COMMON_ENTITY_HANDLE_DATA for objects.
   See DWG_SUPERTYPE_OBJECT in dwg_encode().
*/
static int
dwg_encode_object(Dwg_Object* obj,
                  Bit_Chain* hdl_dat, Bit_Chain* str_dat, Bit_Chain* dat)
{
  BITCODE_BS i, num_eed;
  Dwg_Object_Object* ord = obj->tio.object;
  
  VERSIONS(R_2000, R_2007)
    {
      obj->bitsize_address = bit_position(dat);
      if (!obj->bitsize)
        bit_write_RL(dat, obj->size * 8);
      else
        bit_write_RL(dat, obj->bitsize);
      LOG_INFO("Object bitsize: " FORMAT_RL " @%lu.%u\n", obj->bitsize,
                dat->byte, dat->bit);
    }
  SINCE(R_2010)
    {
      obj->bitsize = dat->size - 0;
      LOG_INFO("Object bitsize: " FORMAT_RL " @%lu.%u\n", obj->bitsize,
               dat->byte, dat->bit);
    }
  obj->hdlpos = bit_position(dat) + obj->bitsize; // the handle stream offset
  SINCE(R_2007)
    {
      obj_string_stream(dat, obj, str_dat);
    }

  bit_write_H(dat, &(obj->handle));

  num_eed = ord->num_eed;
  if (!num_eed) {
    bit_write_BS(dat, 0);
  } else {
    bit_write_BS(dat, ord->eed[0].size);
    for (i = 0; i < num_eed; i++)
      {
        BITCODE_BS j;
        LOG_TRACE("EED[%u] size: " FORMAT_BS "\n", i, ord->eed[i].size)
        bit_write_H(dat, &(ord->eed[i].handle));
        LOG_TRACE("EED[%u] code: " FORMAT_RC "\n", i, ord->eed[i].data->code)
        bit_write_TF(dat, ord->eed[i].raw, ord->eed[i].size);
        /*
        bit_write_RC(dat, ord->eed[i].data->code);
        for (j=1; j < ord->eed[i].size-1; j++)
          bit_write_RC(dat, ord->eed[i].raw[j]);
        */
        if (i+1 < num_eed)
          bit_write_BS(dat, ord->eed[i+1].size);
        else
          bit_write_BS(dat, 0);
      }
  }

  VERSIONS(R_13, R_14)
    {
      obj->bitsize_address = bit_position(dat);
      if (!obj->bitsize)
        bit_write_RL(dat, obj->size * 8);
      else
        bit_write_RL(dat, obj->bitsize);
    }

   bit_write_BL(dat, ord->num_reactors);

  SINCE(R_2004)
    {
      bit_write_B(dat, ord->xdic_missing_flag);
    }
  return 0;
}

static int
dwg_encode_header_variables(Bit_Chain* dat, Bit_Chain* hdl_dat, Bit_Chain* str_dat,
                            Dwg_Data * dwg)
{
  Dwg_Header_Variables* _obj = &dwg->header_vars;
  Dwg_Object* obj = NULL;

  #include "header_variables.spec"

  return 0;
}

static void
dwg_encode_xdata(Bit_Chain* dat, Dwg_Object_XRECORD *obj, int size)
{
  Dwg_Resbuf *tmp, *rbuf = obj->xdata;
  short type;
  int i;

  while (rbuf)
    {
      tmp = rbuf->next;
      type = get_base_value_type(rbuf->type);
      switch (type)
        {
        case VT_STRING:
          UNTIL(R_2007) {
            bit_write_RS(dat, rbuf->value.str.size);
            bit_write_RC(dat, rbuf->value.str.codepage);
            bit_write_TF(dat, rbuf->value.str.u.data, rbuf->value.str.size);
          } LATER_VERSIONS {
            bit_write_RS(dat, rbuf->value.str.size);
            for (i = 0; i < rbuf->value.str.size; i++)
              bit_write_RS(dat, rbuf->value.str.u.wdata[i]);
          }
          break;
        case VT_REAL:
          bit_write_RD(dat, rbuf->value.dbl);
          break;
        case VT_BOOL:
        case VT_INT8:
          bit_write_RC(dat, rbuf->value.i8);
          break;
        case VT_INT16:
          bit_write_RS(dat, rbuf->value.i16);
          break;
        case VT_INT32:
          bit_write_RL(dat, rbuf->value.i32);
          break;
        case VT_POINT3D:
          bit_write_RD(dat, rbuf->value.pt[0]);
          bit_write_RD(dat, rbuf->value.pt[1]);
          bit_write_RD(dat, rbuf->value.pt[2]);
          break;
        case VT_BINARY:
          bit_write_RC(dat, rbuf->value.str.size);
          bit_write_TF(dat, rbuf->value.str.u.data, rbuf->value.str.size);
          break;
        case VT_HANDLE:
        case VT_OBJECTID:
          for (i = 0; i < 8; i++)
             bit_write_RC(dat, rbuf->value.hdl[i]);
          break;
        case VT_INVALID:
        default:
          LOG_ERROR("Invalid group code in xdata: %d", rbuf->type)
          break;
        }
      rbuf = tmp;
    }
}

#undef IS_ENCODER
