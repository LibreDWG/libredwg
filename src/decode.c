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
 * modified by Reini Urban
 */

#include "config.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifdef HAVE_WCHAR_H
# include <wchar.h>
#endif

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "decode.h"
#include "print.h"

/* The logging level for the read (decode) path.  */
static unsigned int loglevel;
/* the current version per spec block */
static unsigned int cur_ver = 0;

#ifdef USE_TRACING
/* This flag means we have checked the environment variable
   LIBREDWG_TRACE and set `loglevel' appropriately.  */
static bool env_var_checked_p;
#define DWG_LOGLEVEL loglevel
#endif  /* USE_TRACING */

#include "logging.h"
#include "dec_macros.h"

#define REFS_PER_REALLOC 128

#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

/*------------------------------------------------------------------------------
 * Functions reused with decode_r2007
 */
Dwg_Object_Ref *
dwg_decode_handleref(Bit_Chain* hdl_dat, Dwg_Object* obj, Dwg_Data* dwg);
Dwg_Object_Ref *
dwg_decode_handleref_with_code(Bit_Chain* hdl_dat, Dwg_Object* obj,
                               Dwg_Data* dwg, unsigned int code);
void
dwg_decode_header_variables(Bit_Chain* dat, Bit_Chain* hdl_dat,
                            Bit_Chain* str_dat, Dwg_Data* dwg);
void
dwg_decode_add_object(Dwg_Data* dwg, Bit_Chain* dat, Bit_Chain* hdl_dat,
                      long unsigned int address);

extern int
read_r2007_meta_data(Bit_Chain *dat, Bit_Chain *hdl_dat, Dwg_Data *dwg);
extern void
obj_string_stream(Bit_Chain *dat, Dwg_Object *obj, Bit_Chain *str);

/*------------------------------------------------------------------------------
 * Private functions
 */

static Dwg_Object *
dwg_resolve_handle(Dwg_Data* dwg, unsigned long int handle);

static int
dwg_resolve_handleref(Dwg_Object_Ref *ref, Dwg_Object * obj);

static int
resolve_objectref_vector(Bit_Chain* dat, Dwg_Data * dwg);

static void
decode_preR13_section_ptr(const char* name, Dwg_Section_Type_r11 id,
                          Bit_Chain* dat, Dwg_Data * dwg);
static void
decode_preR13_section_chk(Dwg_Section_Type_r11 id, Bit_Chain* dat,
                          Dwg_Data * dwg);
static void
decode_preR13_section(Dwg_Section_Type_r11 id, Bit_Chain* dat, Dwg_Data * dwg);

static int
decode_preR13(Bit_Chain* dat, Dwg_Data * dwg);
static int
decode_R13_R2000(Bit_Chain* dat, Dwg_Data * dwg);
static int
decode_R2004(Bit_Chain* dat, Dwg_Data * dwg);
static int
decode_R2007(Bit_Chain* dat, Dwg_Data * dwg);

static Dwg_Resbuf*
dwg_decode_xdata(Bit_Chain * dat, Dwg_Object_XRECORD * obj, int size);

static int
dwg_decode_eed(Bit_Chain * dat, Dwg_Object_Object * obj);

static int
dwg_decode_object(Bit_Chain* dat, Bit_Chain* hdl_dat, Bit_Chain* str_dat,
                  Dwg_Object_Object * obj);

static int
dwg_decode_entity(Bit_Chain* dat, Bit_Chain* hdl_dat, Bit_Chain* str_dat,
                  Dwg_Object_Entity * ent);

/*----------------------------------------------------------------------------
 * Public variables
 */
//long unsigned int ktl_lastaddress;

/*----------------------------------------------------------------------------
 * Public function definitions
 */

/** dwg_decode
 * returns 0 on success.
 *
 * everything in dwg is cleared
 * and then either read from dat, or set to a default.
 */
int
dwg_decode(Bit_Chain * dat, Dwg_Data * dwg)
{
  int i;
  char version[7];

  dwg->num_object_refs = 0;
  dwg->num_layers = 0;
  dwg->num_entities = 0;
  dwg->num_objects = 0;
  dwg->num_classes = 0;
  dwg->picture.size = 0;
  dwg->picture.chain = NULL;
  dwg->header.num_sections = 0;
  dwg->header.num_infos = 0;
  dwg->measurement = 0;
  dwg->dwg_class = NULL;
  dwg->object_ref = NULL;
  dwg->object = NULL;

  memset(&dwg->header_vars, 0, sizeof(Dwg_Header_Variables));
  memset(&dwg->r2004_header.file_ID_string[0], 0, sizeof(dwg->r2004_header));
  memset(&dwg->auxheader.aux_intro[0], 0, sizeof(dwg->auxheader));
  memset(&dwg->second_header.size, 0, sizeof(dwg->second_header));

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
  for (i=0; i<R_AFTER; i++)
    {
      if (!strcmp(version, version_codes[(Dwg_Version_Type)i])) {
        dwg->header.version = (Dwg_Version_Type)i;
        break;
      }
    }
  if (dwg->header.version == 0)
    {
      LOG_ERROR("Invalid or unimplemented version code! "
        "This file's version code is: %s\n", version)
      return -1;
    }
  dat->version = dwg->header.version;
  dwg->header.from_version = dat->version;
  dat->from_version = dat->version;
  LOG_INFO("This file's version code is: %s\n", version)

#define WE_CAN \
    "This version of LibreDWG is only capable of safely decoding " \
    "version R13-R2004 (code: AC1012-AC1018) DWG files.\n"

  PRE(R_13)
    {
      // TODO: tables, entities, block entities
      LOG_ERROR(WE_CAN "We don't decode tables, entities, blocks yet")
      return decode_preR13(dat, dwg);
    }

  VERSIONS(R_13, R_2000)
    {
      return decode_R13_R2000(dat, dwg);
    }

  VERSION(R_2004)
    {
      return decode_R2004(dat, dwg);
    }

  VERSION(R_2007)
    {
      LOG_ERROR(WE_CAN
               "Support for this version is still experimental."
               " We don't decode handle and string streams correctly yet.\n"
               "It will probably crash and/or give you invalid output.")
      return decode_R2007(dat, dwg);
    }

  SINCE(R_2010)
    {
      LOG_ERROR(WE_CAN
               "Support for this version is still experimental."
               " We don't decode objects yet.\n"
               "It will probably crash and/or give you invalid output.")
      return decode_R2004(dat, dwg);
    }

  // This line should not be reached
  LOG_ERROR(
      "LibreDWG does not support this version: %s.", version)
  return -1;
}

// We put the 3x 10 table fields into sections.
// number is the number of elements in the table. >=r13 it is the id.
static void
decode_preR13_section_ptr(const char* name, Dwg_Section_Type_r11 id,
                          Bit_Chain* dat, Dwg_Data * dwg)
{
  Dwg_Section *tbl = &dwg->header.section[id];
  tbl->size    = bit_read_RS(dat);
  tbl->number  = (long)bit_read_RL(dat);
  tbl->address = bit_read_RL(dat);
  tbl->name    = (BITCODE_TV)name;
  LOG_TRACE("ptr table %-8s [%2d]: size:%-4u nr:%-2ld (0x%x-0x%lx)\n",
            tbl->name, id, tbl->size, tbl->number, tbl->address,
            (long)(tbl->address + tbl->number * tbl->size))
}

static void
decode_preR13_section_chk(Dwg_Section_Type_r11 id, Bit_Chain* dat,
                          Dwg_Data * dwg)
{
  Dwg_Section *tbl = &dwg->header.section[id];

#define CMP(f,type) if (tbl->f != f) \
  LOG_ERROR("decode_preR13_section_chk %s %s", tbl->name, #f)

  //LOG_ERROR(name "->" #f " " FORMAT_##type " != " #f " " FORMAT_##type)
  BITCODE_RS id1, size;
  BITCODE_RL address;
  long number;
  id1 = bit_read_RS(dat);
  size = bit_read_RS(dat); CMP(size, RS);
  number = (long)bit_read_RS(dat); CMP(number, RL);
  address = bit_read_RL(dat); CMP(address, RL)
#undef CMP
  LOG_TRACE("chk table %-8s [%2d]: size:%-4u nr:%-3ld (0x%x)\n",
            tbl->name, id, size, tbl->number, address)
}

// TABLES really
static void
decode_preR13_section(Dwg_Section_Type_r11 id, Bit_Chain* dat, Dwg_Data * dwg)
{
  Dwg_Section *tbl = &dwg->header.section[id];
  int i; long vcount;
  long unsigned int num = dwg->num_objects;
  long unsigned int old_size = num * sizeof(Dwg_Object);
  long unsigned int size = tbl->number * sizeof(Dwg_Object);
  long unsigned int pos;

  LOG_TRACE("\ncontents table %-8s [%2d]: size:%-4u nr:%-3ld (0x%x-0x%lx)\n",
            tbl->name, id, tbl->size, tbl->number, tbl->address,
            (long)(tbl->address + tbl->number * tbl->size))
  dat->byte = tbl->address;
  dwg->object = realloc(dwg->object, old_size + size);

  // TODO: move to a spec dwg_r11.spec, and dwg_decode_r11_NAME
#define PREP_TABLE(name)\
  Dwg_Object *obj = &dwg->object[num + i];                              \
  Dwg_Object_##name *_obj = calloc (1, sizeof(Dwg_Object_##name));      \
  obj->tio.object = calloc (1, sizeof(Dwg_Object_Object));              \
  obj->tio.object->tio.name = _obj;                                     \
  obj->tio.object->object = obj;                                        \
  obj->parent = dwg;                                                    \
  LOG_TRACE("\n-- table entry " #name " [%d]:\n", i)

#define CHK_ENDPOS \
  pos = tbl->address + (i+1) * tbl->size;\
  if ((long)(pos - dat->byte) != 2) \
    {\
      LOG_WARN("offset %ld", pos - dat->byte);\
    }\
  dat->byte = pos

  switch (id)
    {

    case SECTION_BLOCK:
      for (i=0; i < tbl->number; i++)
        {
          PREP_TABLE (BLOCK_HEADER);

          FIELD_RC (flag, 70);
          FIELD_TF (entry_name, 32, 2);
          FIELD_RS (used, 0);

          //TODO RD elevation 30, 2RD base_pt 10: 24
          FIELD_RC (block_scaling, 0);
          FIELD_CAST (owned_object_count, RS, BL, 0);
          FIELD_RC (flag2, 0);
          FIELD_CAST (insert_count, RS, RL, 0);
          FIELD_RS (flag3, 0);
          CHK_ENDPOS;
        }
      break;

    case SECTION_LAYER:
      for (i=0; i < tbl->number; i++)
        {
          PREP_TABLE (LAYER);

          FIELD_RC (flag, 70);
          FIELD_TF (entry_name, 32, 2);
          FIELD_RS (used, 0);

          FIELD_RS (color_rs, 62);   // color, off if negative
          FIELD_RS (linetype_rs, 6); // style
          CHK_ENDPOS;
        }
      break;

    // was a text STYLE table, became a SHAPEFILE object
    case SECTION_STYLE:
      for (i=0; i < tbl->number; i++)
        {
          PREP_TABLE (SHAPEFILE);

          FIELD_RC (flag, 70);
          FIELD_TF (entry_name, 32, 2);
          FIELD_RS (used, 0);

          FIELD_RD (fixed_height, 40);
          FIELD_RD (width_factor, 41);
          FIELD_RD (oblique_ang, 50);
          FIELD_RC (generation, 71);
          FIELD_RD (last_height, 42);
          FIELD_TF (font_name, 128, 3);
          CHK_ENDPOS;
        }
      break;

    case SECTION_LTYPE:
      for (i=0; i < tbl->number; i++)
        {
          PREP_TABLE (LTYPE);

          FIELD_RC (flag, 70);
          FIELD_TF (entry_name, 32, 2);
          FIELD_RS (used, 0);
          FIELD_TF (description, 48, 3);
          FIELD_RC (alignment, 72);
          FIELD_RC (num_dashes, 73);
          FIELD_VECTOR (dashes_r11, RD, num_dashes, 340);
          // ... 106 byte
          // 3, 40, 49, 74, 75, 340, 46, 50, 44, 45, 9

          CHK_ENDPOS;
        }
      break;

    case SECTION_VIEW:
      for (i=0; i< tbl->number; i++)
        {
          PREP_TABLE (VIEW);

          FIELD_RC (flag, 70);
          FIELD_TF (entry_name, 32, 2);
          FIELD_RS (used, 0);

          FIELD_RD (height, 40);
          FIELD_2RD (center, 10);
          FIELD_RD (width, 41);
          FIELD_3RD (target, 12);
          FIELD_3RD (direction, 11);
          FIELD_CAST (view_mode, RS, 4BITS, 71);
          FIELD_RD (lens_length, 42);
          FIELD_RD (front_clip, 43);
          FIELD_RD (back_clip, 44);
          FIELD_RD (twist_angle, 50);
          CHK_ENDPOS;
        }
      break;

    case SECTION_UCS:
      for (i=0; i< tbl->number; i++)
        {
          PREP_TABLE (UCS);

          FIELD_RC (flag, 70);
          FIELD_TF (entry_name, 32, 2);
          FIELD_RS (used, 0);
          FIELD_2RD (origin, 10);
          FIELD_2RD (x_direction, 11);
          FIELD_2RD (y_direction, 12);

          CHK_ENDPOS;
        }
      break;

    case SECTION_VPORT:
      for (i=0; i< tbl->number; i++)
        {
          PREP_TABLE (VPORT);

          FIELD_RC (flag, 70);
          FIELD_TF (entry_name, 32, 2);
          FIELD_RS (used, 0);

          FIELD_RD (VIEWSIZE, 40);
          FIELD_RD (aspect_ratio, 41);
          FIELD_2RD (VIEWCTR, 12);
          FIELD_3RD (view_target, 17);
          FIELD_3RD (VIEWDIR, 16);
          FIELD_RD (view_twist, 50);
          FIELD_RD (lens_length, 42);
          FIELD_RD (front_clip, 43);
          FIELD_RD (back_clip, 33);
          FIELD_CAST (view_mode, RS, 4BITS, 71);

          FIELD_2RD (lower_left, 10);
          FIELD_2RD (upper_right, 11);
          FIELD_RC (UCSFOLLOW, 71);
          FIELD_RS (circle_zoom, 72);
          FIELD_RC (FASTZOOM, 73);
          FIELD_RC (UCSICON, 74);
          FIELD_RC (GRIDMODE, 76);
          FIELD_2RD (GRIDUNIT, 15);
          FIELD_CAST (SNAPMODE, RS, B, 75);
          FIELD_RC (SNAPSTYLE, 77);
          FIELD_RS (SNAPISOPAIR, 78);
          FIELD_RD (SNAPANG, 50);
          FIELD_2RD (SNAPBASE, 13);
          FIELD_2RD (SNAPUNIT, 14);
          // ... 74 byte

          CHK_ENDPOS;
        }
      break;

    case SECTION_APPID:
      for (i=0; i< tbl->number; i++)
        {
          PREP_TABLE (APPID);

          FIELD_RC (flag, 70);
          FIELD_TF (entry_name, 32, 2);
          FIELD_RS (used, 0);
          CHK_ENDPOS;
        }
      break;

    case SECTION_DIMSTYLE:
      for (i=0; i< tbl->number; i++)
        {
          PREP_TABLE (DIMSTYLE);

          FIELD_RC (flag, 70);
          FIELD_TF (entry_name, 32, 2);
          FIELD_RS (used, 0);

          FIELD_RC (DIMTOL, 71);
          FIELD_RC (DIMLIM, 72);
          FIELD_RC (DIMTIH, 73);
          FIELD_RC (DIMTOH, 74);
          FIELD_RC (DIMSE1, 75);
          FIELD_RC (DIMSE2, 76);
          FIELD_RC (DIMALT, 170);
          FIELD_RC (DIMTOFL, 172);
          FIELD_RC (DIMSAH, 173);
          FIELD_RC (DIMTIX, 174);
          FIELD_RC (DIMSOXD, 175);
          FIELD_CAST (DIMALTD, RC, BS, 171);
          FIELD_CAST (DIMZIN, RC, BS, 78);
          FIELD_RC (DIMSD1, 281);
          FIELD_RC (DIMSD2, 282);
          FIELD_CAST (DIMTOLJ, RC, BS, 283);
          FIELD_CAST (DIMJUST, RC, BS, 280);
          FIELD_CAST (DIMFIT, RC, BS, 287);
          FIELD_RC (DIMUPT, 288);
          FIELD_CAST (DIMTZIN, RC, BS, 284);
          FIELD_CAST (DIMMALTZ, RC, BS, 285);
          FIELD_CAST (DIMMALTTZ, RC, BS, 286);
          FIELD_CAST (DIMTAD, RC, BS, 77);
          FIELD_RS (DIMUNIT, 270);
          FIELD_RS (DIMAUNIT, 275);
          FIELD_RS (DIMDEC, 271);
          FIELD_RS (DIMTDEC, 272);
          FIELD_RS (DIMALTU, 273);
          FIELD_RS (DIMALTTD, 274);
          FIELD_RD (DIMSCALE, 40);
          FIELD_RD (DIMASZ, 41);
          FIELD_RD (DIMEXO, 42);
          FIELD_RD (DIMDLI, 43);
          FIELD_RD (DIMEXE, 44);
          FIELD_RD (DIMRND, 45);
          FIELD_RD (DIMDLE, 46);
          FIELD_RD (DIMTP, 47);
          FIELD_RD (DIMTM, 48);
          FIELD_RD (DIMTXT, 140);
          FIELD_RD (DIMCEN, 141);
          FIELD_RD (DIMTSZ, 142);
          FIELD_RD (DIMALTF, 143);
          FIELD_RD (DIMLFAC, 144);
          FIELD_RD (DIMTVP, 145);
          FIELD_RD (DIMTFAC, 146);
          FIELD_RD (DIMGAP, 147);
          FIELD_TV (DIMPOST, 3);
          FIELD_TV (DIMAPOST, 4); //??
          FIELD_TV (DIMBLK_T, 5);
          FIELD_TV (DIMBLK1_T, 6);
          FIELD_TV (DIMBLK2_T, 7);
          FIELD_RC (DIMCLRD_N, 176);
          FIELD_RC (DIMCLRE_N, 177);
          FIELD_RC (DIMCLRT_N, 178);

          //??
          CHK_ENDPOS;
        }
      break;

    case SECTION_VP_ENT_HDR:
      if (tbl->number) {
        LOG_WARN("VP_ENT_HDR table");
      }
      break;

    case SECTION_HEADER_R11:
    default:
      LOG_ERROR("Invalid table id %d", id);
      tbl->number = 0;
      break;
    }
  dwg->num_objects += tbl->number;
  dat->byte = tbl->address + (tbl->number * tbl->size);
}

static void
decode_preR13_entities(unsigned long start, unsigned long end,
                       unsigned long offset,
                       Bit_Chain* dat, Dwg_Data * dwg)
{
  LOG_TRACE("entities: (0x%lx-0x%lx) TODO\n", start, end)
  //_obj, obj
  DEBUG_HERE();
  /* TODO Common entity preR13 header:
    FIELD_RC (flag, 70);
    FIELD_RS (size, 0);
    FIELD_RS (layer, 0);
    FIELD_RS (opts, 0);
    if (FIELD_VALUE(flag) & 1)
      FIELD_RC (color, 60);
    if (FIELD_VALUE(flag) & 0x40)
      FIELD_RC (extra, 0);
    if (FIELD_VALUE(extra) & 2)
      decode_eed();
    if (FIELD_VALUE(flag) & 2)
      FIELD_RS (type, 0);
    if (FIELD_VALUE(flag) & 4 && kind > 2 && kind != 22)
      FIELD_RD (elevation/pt.z, 30);
    if (FIELD_VALUE(flag) & 8)
      FIELD_RD (thickness, 39);
    if (FIELD_VALUE(flag) & 0x20)
      FIELD_HANDLE (handle, 0, 0);
    if (FIELD_VALUE(extra) & 4)
      FIELD_RS (paper, 0);
    */

  //#include "dwg.spec"

  dat->byte = end;
}

static int
decode_preR13(Bit_Chain* dat, Dwg_Data * dwg)
{
  BITCODE_RL entities_start, entities_end, blocks_start, blocks_end;
  BITCODE_RL rl1, rl2;
  BITCODE_RS rs2;
  int tbl_id;
  Dwg_Object *obj = NULL;

  {
    int i;
    struct Dwg_Header *_obj = &dwg->header;
    Bit_Chain* hdl_dat = dat;
    dat->byte = 0x06;

    #include "header.spec"
  }
  LOG_INFO("@0x%lx\n", dat->byte); //0x14

  // tables really
  dwg->header.num_sections = 12;
  dwg->header.section = (Dwg_Section*) calloc(1, sizeof(Dwg_Section)
      * dwg->header.num_sections);
  if (!dwg->header.section)
    {
      LOG_ERROR("Out of memory");
      return -2;
    }

  entities_start  = bit_read_RL(dat);
  entities_end    = bit_read_RL(dat);
  LOG_TRACE("entities 0x%x - 0x%x\n",
            entities_start, entities_end);
  blocks_start  = bit_read_RL(dat);
  rl1           = bit_read_RL(dat); //0x40
  blocks_end    = bit_read_RL(dat);
  rl2           = bit_read_RL(dat); //0x80
  LOG_TRACE("blocks   0x%x (0x%x) - 0x%x (0x%x)\n",
            blocks_start, rl1, blocks_end, rl2);

  tbl_id = 0;
  dwg->header.section[0].number = 0;
  dwg->header.section[0].type  = (Dwg_Section_Type)SECTION_HEADER_R11;

  decode_preR13_section_ptr("BLOCK", SECTION_BLOCK, dat, dwg);
  decode_preR13_section_ptr("LAYER", SECTION_LAYER, dat, dwg);
  decode_preR13_section_ptr("STYLE", SECTION_STYLE, dat, dwg);
  // skip one
  decode_preR13_section_ptr("LTYPE", SECTION_LTYPE, dat, dwg);
  decode_preR13_section_ptr("VIEW", SECTION_VIEW, dat, dwg);

  LOG_TRACE("@0x%lx\n", dat->byte);       //0x5e
  {
    Dwg_Header_Variables* _obj = &dwg->header_vars;
    Bit_Chain* hdl_dat = dat;
    #include "header_variables_r11.spec"
  }
  LOG_TRACE("@0x%lx\n", dat->byte);       //0x23a

  dat->byte = 0x3ef;
  LOG_TRACE("@0x%lx\n", dat->byte);
  decode_preR13_section_ptr("UCS", SECTION_UCS, dat, dwg);
  // skip: 0x500 - dat->bytes
  dat->byte = 0x500;
  LOG_TRACE("@0x%lx\n", dat->byte); //0x23a
  decode_preR13_section_ptr("VPORT", SECTION_VPORT, dat, dwg);
  rl1 = bit_read_RL(dat);
  rl2 = bit_read_RL(dat);
  LOG_TRACE("?2 long: 0x%x 0x%x\n", rl1, rl2);
  decode_preR13_section_ptr("APPID", SECTION_APPID, dat, dwg);
  rl1 = bit_read_RL(dat);
  rs2 = bit_read_RS(dat);
  LOG_TRACE("?long+short: 0x%x 0x%x\n", rl1, rs2);
  decode_preR13_section_ptr("DIMSTYLE", SECTION_DIMSTYLE, dat, dwg);
  // skip: 0x69f - dat->bytes
  dat->byte = 0x69f;
  decode_preR13_section_ptr("VP_ENT_HDR", SECTION_VP_ENT_HDR, dat, dwg);
  dat->byte += 38;
  // entities
  decode_preR13_entities(entities_start, entities_end, 0, dat, dwg);
  dat->byte += 19;
  decode_preR13_section(SECTION_BLOCK, dat, dwg);
  decode_preR13_section(SECTION_LAYER, dat, dwg);
  decode_preR13_section(SECTION_STYLE, dat, dwg);
  decode_preR13_section(SECTION_LTYPE, dat, dwg);
  decode_preR13_section(SECTION_VIEW, dat, dwg);
  decode_preR13_section(SECTION_UCS, dat, dwg);
  decode_preR13_section(SECTION_VPORT, dat, dwg);
  decode_preR13_section(SECTION_APPID, dat, dwg);
  decode_preR13_section(SECTION_DIMSTYLE, dat, dwg);
  decode_preR13_section(SECTION_VP_ENT_HDR, dat, dwg);
  // blocks
  decode_preR13_entities(blocks_start, blocks_end, blocks_start - 0x40000000,
                         dat, dwg);
  LOG_TRACE("@0x%lx\n", dat->byte);
  // 36 byte: 9x long
  rl1 = bit_read_RL(dat);
  rl2 = bit_read_RL(dat);
  LOG_TRACE("?2long: 0x%x 0x%x %f\n", rl1, rl2,
            (double)dat->chain[dat->byte-8]);
  rl1 = bit_read_RL(dat);
  rl2 = bit_read_RL(dat);
  LOG_TRACE("?2long: 0x%x 0x%x %f\n", rl1, rl2,
            (double)dat->chain[dat->byte-8]);
  rl1 = bit_read_RL(dat);
  rl2 = bit_read_RL(dat);
  LOG_TRACE("?2long: 0x%x 0x%x %f\n", rl1, rl2,
            (double)dat->chain[dat->byte-8]);
  rl1 = bit_read_RL(dat);
  rl2 = bit_read_RL(dat);
  LOG_TRACE("?2long: 0x%x 0x%x %f\n", rl1, rl2,
            (double)dat->chain[dat->byte-8]);
  rl1 = bit_read_RL(dat);
  LOG_TRACE("?1long: 0x%x\n", rl1);

  LOG_TRACE("@0x%lx: 4 block ptrs chk\n", dat->byte);
  if ((rl1 = bit_read_RL(dat)) != entities_start) {
    LOG_WARN("entities_start %x/%x", rl1, entities_start);
  }
  if ((rl1 = bit_read_RL(dat)) != entities_end) {
    LOG_WARN("entities_end %x/%x", rl1, entities_end);
  }
  if ((rl1 = bit_read_RL(dat)) != blocks_start) {
    LOG_WARN("blocks_start %x/%x", rl1, blocks_start);
  }
  if ((rl1 = bit_read_RL(dat)) != blocks_end) {
    LOG_WARN("blocks_end %x/%x", rl1, blocks_end);
  }
  // 12 byte
  LOG_INFO("@0x%lx\n", dat->byte);
  rl1 = bit_read_RL(dat);
  rl2 = bit_read_RL(dat);
  LOG_TRACE("?2long: 0x%x 0x%x\n", rl1, rl2);
  rl1 = bit_read_RL(dat);
  LOG_TRACE("?1long: 0x%x\n", rl1);

  dat->byte = blocks_end + 36 + 4*4 + 12;
  LOG_INFO("@0x%lx\n", dat->byte);
  decode_preR13_section_chk(SECTION_BLOCK, dat, dwg);
  decode_preR13_section_chk(SECTION_LAYER, dat, dwg);
  decode_preR13_section_chk(SECTION_STYLE, dat, dwg);
  decode_preR13_section_chk(SECTION_LTYPE, dat, dwg);
  decode_preR13_section_chk(SECTION_VIEW, dat, dwg);
  decode_preR13_section_chk(SECTION_UCS, dat, dwg);
  decode_preR13_section_chk(SECTION_VPORT, dat, dwg);
  decode_preR13_section_chk(SECTION_APPID, dat, dwg);
  decode_preR13_section_chk(SECTION_DIMSTYLE, dat, dwg);
  decode_preR13_section_chk(SECTION_VP_ENT_HDR, dat, dwg);
  rl1 = bit_read_RL(dat);
  LOG_TRACE("long 0x%x\n", rl1); // address

  return 0;
}

static int
decode_R13_R2000(Bit_Chain* dat, Dwg_Data * dwg)
{
  Dwg_Object *obj = NULL;
  unsigned int section_size = 0;
  unsigned char sgdc[2];
  unsigned int ckr, ckr2;
  long unsigned int size;
  long unsigned int lasta;
  long unsigned int lastmap;
  long unsigned int startpos;
  long unsigned int object_begin;
  long unsigned int object_end;
  long unsigned int pvz;
  unsigned int j, k;

  {
    int i;
    struct Dwg_Header *_obj = &dwg->header;
    Bit_Chain* hdl_dat = dat;
    dat->byte = 0x06;

    #include "header.spec"
  }

  /* Section Locator Records 0x15 */
  assert(dat->byte == 0x15);
  dwg->header.num_sections = bit_read_RL(dat);
  LOG_TRACE("\nNum sections: " FORMAT_RL "\n", dwg->header.num_sections)
  if (!dwg->header.num_sections) //ODA writes zeros.
    dwg->header.num_sections = 6;

  // So far seen 3-6 sections. Most emit only 3-5 sections.
  dwg->header.section = (Dwg_Section*) calloc(1, sizeof(Dwg_Section)
      * dwg->header.num_sections);
  if (!dwg->header.section)
    {
      LOG_ERROR("Out of memory");
      return -2;
    }
  /* section 0: header vars
   *         1: class section
   *         2: object map
   *         3: (R13 c3 and later): 2nd header (special table no sentinels)
   *         4: optional: MEASUREMENT
   *         5: optional: AuxHeader
   */
  for (j = 0; j < dwg->header.num_sections; j++)
    {
      dwg->header.section[j].number  = bit_read_RC(dat);
      dwg->header.section[j].address = bit_read_RL(dat);
      dwg->header.section[j].size    = bit_read_RL(dat);
      LOG_TRACE("section[%u]: number=%2d address=0x%8x size=%4d\n",
                j, (int)dwg->header.section[j].number,
                (unsigned)dwg->header.section[j].address,
                (int)dwg->header.section[j].size)
    }

  // Check CRC
  ckr = bit_calc_CRC(0xc0c1, dat->chain, dat->byte);
  ckr2 = bit_read_RS(dat);
  if (ckr != ckr2)
    {
      LOG_ERROR("Header CRC mismatch %d <=> %d", ckr, ckr2);
      //if (dwg->header.version == R_2000)
      //  return -1;
      /* The CRC depends on num_sections. XOR result with
         3: 0xa598
         4: 0x8101
         5: 0x3cc4
         6: 0x8461
      */
    }

  if (bit_search_sentinel(dat, dwg_sentinel(DWG_SENTINEL_HEADER_END)))
    LOG_TRACE("\n=======> HEADER (end): %8X\n", (unsigned int) dat->byte)

  /*-------------------------------------------------------------------------
   * Section 5 AuxHeader
   * R2000+, mostly redundant file header information
   */

  if (dwg->header.num_sections == 6 && dwg->header.version >= R_2000)
    {
      int i;
      struct Dwg_AuxHeader* _obj = &dwg->auxheader;
      Bit_Chain* hdl_dat = dat;
      obj = NULL;

      LOG_TRACE("\n=======> AuxHeader: %8X\n",
        (unsigned int)dwg->header.section[SECTION_AUXHEADER_R2000].address)
      LOG_TRACE("         AuxHeader (end): %8X\n",
        (unsigned int)(dwg->header.section[SECTION_AUXHEADER_R2000].address
                  + dwg->header.section[SECTION_AUXHEADER_R2000].size))
      dat->byte = dwg->header.section[SECTION_AUXHEADER_R2000].address;

      #include "auxheader.spec"

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
          dwg->picture.chain = (unsigned char *) calloc(1, dwg->picture.size);
          if (!dwg->picture.chain)
            {
              LOG_ERROR("Out of memory");
              return -2;
            }
          memcpy(dwg->picture.chain, &dat->chain[start_address],
              dwg->picture.size);
        }
    }

  /*-------------------------------------------------------------------------
   * Header Variables, section 0
   */

  LOG_INFO("\n=======> Header Variables: %8X\n",
          (unsigned int) dwg->header.section[SECTION_HEADER_R13].address)
  LOG_INFO("         Header Variables (end): %8X\n",
          (unsigned int) (dwg->header.section[SECTION_HEADER_R13].address
              + dwg->header.section[SECTION_HEADER_R13].size))
  dat->byte = dwg->header.section[SECTION_HEADER_R13].address + 16;
  pvz = bit_read_RL(dat);
  LOG_TRACE("         Length: %lu\n", pvz)

  dat->bit = 0;
  dwg_decode_header_variables(dat, dat, dat, dwg);

  // Check CRC-on
  dat->bit = 0;
  ckr = dwg->header_vars.CRC;
  pvz = dwg->header.section[SECTION_HEADER_R13].address + 16;
  ckr2 = bit_calc_CRC(0xc0c1, &(dat->chain[pvz]),
                      dwg->header.section[SECTION_HEADER_R13].size - 34);
  if (ckr != ckr2)
    {
      LOG_ERROR("Section[%ld] CRC mismatch %d <=> %d",
                dwg->header.section[SECTION_HEADER_R13].number, ckr, ckr2);
      // TODO: xor with num_sections
      //if (dwg->header.version == R_2000)
      //  return -1;
    }

  /*-------------------------------------------------------------------------
   * Classes, section 1
   */
  LOG_INFO("\n"
           "=======> CLASS 1 (start): %8lX\n",
           (long)dwg->header.section[SECTION_CLASSES_R13].address)
  LOG_INFO("         CLASS 1 (end)  : %8lX\n",
           (long)(dwg->header.section[SECTION_CLASSES_R13].address
              + dwg->header.section[SECTION_CLASSES_R13].size))
  dat->byte = dwg->header.section[SECTION_CLASSES_R13].address + 16;
  dat->bit = 0;

  size = bit_read_RL(dat);
  lasta = dat->byte + size;
  LOG_TRACE("         Length: %lu\n", size);

  /* Read the classes
   */
  dwg->layout_number = 0;
  dwg->num_classes = 0;
  j = 0;
  do
    {
      unsigned int i;
      Dwg_Class *klass;

      i = dwg->num_classes;
      if (i == 0)
        dwg->dwg_class = (Dwg_Class *) malloc(sizeof(Dwg_Class));
      else
        dwg->dwg_class = (Dwg_Class *) realloc(dwg->dwg_class, (i + 1)
            * sizeof(Dwg_Class));
      if (!dwg->dwg_class)
        {
          LOG_ERROR("Out of memory");
          return -2;
        }
      klass = &dwg->dwg_class[i];
      memset(klass, 0, sizeof(Dwg_Class));
      klass->number  = bit_read_BS(dat);
      klass->proxyflag = bit_read_BS(dat);
      klass->appname = bit_read_TV(dat);
      klass->cppname = bit_read_TV(dat);
      klass->dxfname = bit_read_TV(dat);
      klass->wasazombie = bit_read_B(dat);
      // 1f2 for entities, 1f3 for objects
      klass->item_class_id = bit_read_BS(dat);
      LOG_TRACE("Class %d 0x%x %s\n"
                "%s \"%s\" %d 0x%x\n",
                klass->number, klass->proxyflag, klass->dxfname,
                klass->cppname, klass->appname,
                klass->wasazombie, klass->item_class_id)

      SINCE(R_2007)
        {
          klass->instance_count = bit_read_BL(dat);
          klass->dwg_version = bit_read_BL(dat);
          klass->maint_version = bit_read_BL(dat);
          klass->unknown_1 = bit_read_BL(dat);
          klass->unknown_2 = bit_read_BL(dat);
        }

      if (strcmp((const char *)klass->dxfname, "LAYOUT") == 0)
        dwg->layout_number = klass->number;

      dwg->num_classes++;
      if (dwg->num_classes > 100)
        {
          LOG_ERROR("number of classes is greater than 100");
          break;
        }
    }
  while (dat->byte < (lasta - 1));

  // Check CRC-on
  dat->byte = dwg->header.section[SECTION_CLASSES_R13].address
              + dwg->header.section[SECTION_CLASSES_R13].size - 18;
  dat->bit = 0;
  ckr = bit_read_RS(dat);
  pvz = dwg->header.section[SECTION_CLASSES_R13].address + 16;
  ckr2 = bit_calc_CRC(0xc0c1, &(dat->chain[pvz]),
                      dwg->header.section[SECTION_CLASSES_R13].size - 34);
  if (ckr != ckr2)
    {
      LOG_ERROR("Section[%ld] CRC mismatch %d <=> %d",
                dwg->header.section[SECTION_CLASSES_R13].number, ckr, ckr2);
      //if (dwg->header.version == R_2000)
      //  return -1;
    }

  dat->byte += 16;
  pvz = bit_read_RL(dat); // Unknown bitlong inter class and object
  LOG_TRACE("@ %lu RL: 0x%lx\n", dat->byte - 4, pvz)
  LOG_INFO("Number of classes read: %u\n", dwg->num_classes)

  /*-------------------------------------------------------------------------
   * Object-map, section 2
   */

  dat->byte = dwg->header.section[SECTION_OBJECTS_R13].address;
  dat->bit = 0;

  lastmap = dat->byte + dwg->header.section[SECTION_OBJECTS_R13].size; // 4
  dwg->num_objects = 0;
  object_begin = dat->size;
  object_end = 0;
  LOG_TRACE("@ %lu RL Object-map section 2, size %u\n", dat->byte,
            (unsigned)dwg->header.section[SECTION_OBJECTS_R13].size)
  do
    {
      long unsigned int last_offset;
      //long unsigned int last_handle;
      long unsigned int oldpos = 0;
      startpos = dat->byte;

      section_size = bit_read_RS_LE(dat);
      LOG_TRACE("Section size: %u\n", section_size);
      if (section_size > 2035)
        {
          LOG_ERROR("Object-map section size greater than 2035!")
          return -1;
        }

      //last_handle = 0;
      last_offset = 0;
      while (dat->byte - startpos < section_size)
        {
          long handle, offset;
          oldpos = dat->byte;
          handle = bit_read_MC(dat);
          offset = bit_read_MC(dat);
          //last_handle += handle;
          last_offset += offset;
          LOG_TRACE("\nNext object: %lu\t", dwg->num_objects)
          LOG_TRACE("Handle: %li\tOffset: %ld @%lu\n", handle, offset, last_offset)

          if (dat->byte == oldpos)
            break;

          if (object_end < last_offset)
            object_end = last_offset;
          if (object_begin > last_offset)
            object_begin = last_offset;

          dwg_decode_add_object(dwg, dat, dat, last_offset);
#if 0
          kobj = dwg->num_objects;
          if (dwg->num_objects > kobj)
            dwg->object[dwg->num_objects - 1].handle.value = last_handle;
          //TODO: blame Juca
#endif
        }
      if (dat->byte == oldpos)
        break;

      // CRC on
      if (dat->bit > 0)
        {
          dat->byte += 1;
          dat->bit = 0;
        }

      ckr = bit_read_RS_LE(dat);
      ckr2 = bit_calc_CRC(0xc0c1, dat->chain + startpos, section_size);
      if (ckr != ckr2)
        {
          LOG_ERROR("Section CRC mismatch %d <=> %d", ckr, ckr2);
          // fails with r14
          //if (dwg->header.version == R_2000)
          //  return -1;
        }

      if (dat->byte >= lastmap)
        break;
    }
  while (section_size > 2);

  LOG_INFO("Num objects: %lu\n", dwg->num_objects)
  LOG_INFO("\n"
           "=======> Object Data 2 (start)  : %8X\n",
           (unsigned int) object_begin)
  dat->byte = object_end;
  object_begin = bit_read_MS(dat);
  LOG_INFO("         Object Data 2 (end)    : %8X\n",
      (unsigned int) (object_end + object_begin+ 2))

  /*
   dat->byte = dwg->header.section[SECTION_OBJECTS_R13].address - 2;
   // Unknown bitdouble inter object data and object map
   antckr = bit_read_CRC (dat);
   LOG_TRACE("Address: %08X / Content: 0x%04X", dat->byte - 2, antckr)

   // check CRC-on
   antckr = 0xC0C1;
   do
   {
     startpos = dat->byte;
     sgdc[0] = bit_read_RC (dat);
     sgdc[1] = bit_read_RC (dat);
     section_size = (sgdc[0] << 8) | sgdc[1];
     section_size -= 2;
     dat->byte += section_size;
     ckr = bit_read_CRC (dat);
     dat->byte -= 2;
     bit_write_CRC (dat, startpos, antckr);
     dat->byte -= 2;
     ckr2 = bit_read_CRC (dat);
     if (loglevel) fprintf (stderr, "Read: %X\nCreated: %X\t SEMO: %X\n",
                            ckr, ckr2, antckr);
     //antckr = ckr;
   } while (section_size > 0);
   */
  LOG_INFO("\n"
           "=======> Object Map (start)     : %8X\n",
          (unsigned int) dwg->header.section[SECTION_OBJECTS_R13].address)
  LOG_INFO("         Object Map (end)       : %8X\n",
          (unsigned int) (dwg->header.section[SECTION_OBJECTS_R13].address
              + dwg->header.section[SECTION_OBJECTS_R13].size))

  /*-------------------------------------------------------------------------
   * Second header, section 3. R13-R2000 only.
   * But partially also since r2004.
   */

  if (bit_search_sentinel(dat, dwg_sentinel(DWG_SENTINEL_SECOND_HEADER_BEGIN)))
    {
      int i;
      BITCODE_RC sig, sig2;
      long vcount;
      long unsigned int pvzadr;
      struct _dwg_second_header* _obj = &dwg->second_header;
      obj = NULL;

      LOG_INFO("\n"
               "=======> Second Header 3 (start): %8X\n",
               (unsigned int) dat->byte - 16)
      pvzadr = dat->byte;
      LOG_TRACE("pvzadr: %lx\n", pvzadr)

      FIELD_RL(size, 0);
      FIELD_BL(address, 0);
      if (!dwg->header.section[SECTION_2NDHEADER_R13].address)
        {
          dwg->header.section[SECTION_2NDHEADER_R13].address =
            dwg->second_header.address;
          dwg->header.section[SECTION_2NDHEADER_R13].size =
            dwg->second_header.size;
        }

      // AC1012, AC1014 or AC1015. This is a char[11], zero padded.
      // with \n at 12.
      bit_read_fixed(dat, _obj->version, 12);
      LOG_TRACE("version: %s\n", _obj->version)
      for (i = 0; i < 4; i++)
        FIELD_B(null_b[i], 0);
      //DEBUG_HERE();
      // documented as 0x18,0x78,0x01,0x04 for R13, 0x18,0x78,0x01,0x05 for R14
      // r14:      7d f4 78 01
      // r2000:    14 64 78 01
      FIELD_RC(unknown_10, 0); // 0x10
      for (i = 0; i < 4; i++)
        FIELD_RC(unknown_rc4[i], 0);

      if (dat->version < R_2000 &&
          FIELD_VALUE(unknown_10) == 0x18 &&
          FIELD_VALUE(unknown_rc4[0]) == 0x78)
        dat->byte -= 2;
      UNTIL (R_2000) {
        FIELD_RC(num_sections, 0); // r14: 5, r2000: 6
        for (i = 0; i < MIN(6,FIELD_VALUE(num_sections)); i++)
          {
            // address+sizes of sections 0-2 is correct, 3+4 is empty
            FIELD_RC(section[i].nr, 0);
            FIELD_BL(section[i].address, 0);
            FIELD_BL(section[i].size, 0);
          }
        if (DWG_LOGLEVEL >= DWG_LOGLEVEL_HANDLE)
          {
            LOG_HANDLE("1st header was:\n");
            for (i = 0; i < (int)dwg->header.num_sections; i++)
              {
                LOG_HANDLE("section[%d] %ld %u %u\n", i,
                           dwg->header.section[i].number,
                           dwg->header.section[i].address,
                           dwg->header.section[i].size);
              }
            LOG_HANDLE("start 3: %lu\n", pvzadr-16);
          }

        FIELD_BS(num_handlers, 0); // 14, resp. 16 in r14
        if (FIELD_VALUE(num_handlers) > 16) {
          LOG_ERROR("Second header num_handlers > 16: %d\n",
                    FIELD_VALUE(num_handlers));
          FIELD_VALUE(num_handlers) = 14;
        }
        for (i = 0; i < FIELD_VALUE(num_handlers); i++)
          {
            FIELD_RC(handlers[i].size, 0);
            FIELD_RC(handlers[i].nr, 0);
            FIELD_VECTOR(handlers[i].data, RC, handlers[i].size, 0);
          }

        // Check CRC-on
        ckr = bit_read_CRC(dat);

        VERSION(R_14) {
          FIELD_RL(junk_r14_1, 0);
          FIELD_RL(junk_r14_2, 0);
        }
      }

      if (bit_search_sentinel(dat, dwg_sentinel(DWG_SENTINEL_SECOND_HEADER_END)))
        LOG_INFO("         Second Header 3 (end)  : %8X\n",
                 (unsigned int) dat->byte)
    }

  /*-------------------------------------------------------------------------
   * Section 4: MEASUREMENT
   */

  if (dwg->header.num_sections >= 4)
    {
      LOG_INFO("\n"
               "=======> MEASUREMENT 4 (start)  : %8X\n",
               (unsigned int) dwg->header.section[4].address)
      LOG_INFO("         MEASUREMENT 4 (end)    : %8X\n",
               (unsigned int) (dwg->header.section[4].address
                               + dwg->header.section[4].size))
      dat->byte = dwg->header.section[4].address;
      dat->bit = 0;
      dwg->measurement = bit_read_RL(dat);

      LOG_TRACE("         Size bytes :\t%lu\n", dat->size)
    }

  //step II of handles parsing: resolve pointers from handle value
  //XXX: move this somewhere else
  LOG_TRACE("\nResolving pointers from ObjectRef vector.\n")
  return resolve_objectref_vector(dat, dwg);
}

static int
resolve_objectref_vector(Bit_Chain* dat, Dwg_Data * dwg)
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

      if (obj)
        {
          LOG_TRACE("-found:  HANDLE(%d.%d.%lu)\n",
              obj->handle.code,
              obj->handle.size,
              obj->handle.value)
        }

      //assign found pointer to objectref vector
      dwg->object_ref[i]->obj = obj;

      if (DWG_LOGLEVEL >= DWG_LOGLEVEL_HANDLE)
        {
          if (obj)
            dwg_print_object(dat, obj);
          else
            LOG_WARN("Null object pointer: object_ref[%lu]", i)
        }
    }
  return dwg->num_object_refs ? 0 : 1;
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

/* Decompresses a system section of a 2004+ DWG file
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
  bit_read_fixed(dat, decomp, lit_length);
  dst += lit_length;

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
          break;     // Terminates the input stream, everything is ok
      else
          return 1;  // error in input stream

      LOG_INSANE("got compressed data %d\n", comp_bytes)
      // copy "compressed data"
      src = dst - comp_offset - 1;
      assert(src >= decomp);
      for (i = 0; i < comp_bytes; ++i)
        *dst++ = *src++;

      // copy "literal data"
      LOG_INSANE("got literal data %d\n", lit_length)
      for (i = 0; i < lit_length; ++i)
        *dst++ = bit_read_RC(dat);
    }

  return 0;  // Success
}

/* Read R2004, 2010+ Section Map
 * The Section Map is a vector of number, size, and address triples used
 * to locate the sections in the file.
 */
static void
read_R2004_section_map(Bit_Chain* dat, Dwg_Data * dwg)
{
  char *decomp, *ptr;
  int i;
  int section_address;
  int bytes_remaining;
  uint32_t comp_data_size   = dwg->r2004_header.comp_data_size;
  uint32_t decomp_data_size = dwg->r2004_header.decomp_data_size;

  dwg->header.num_sections = 0;
  dwg->header.section = 0;

  // decompressed data
  decomp = (char *)calloc(decomp_data_size, sizeof(char));
  if (!decomp)
    {
      LOG_ERROR("Out of memory");
      return;
    }

  decompress_R2004_section(dat, decomp, comp_data_size);

  LOG_TRACE("\n#### Read 2004 Section Page Map ####\n")

  section_address = 0x100;  // starting address
  i = 0;
  bytes_remaining = decomp_data_size;
  ptr = decomp;
  dwg->header.num_sections = 0;

  while (bytes_remaining)
    {
      if (dwg->header.num_sections==0)
        dwg->header.section = (Dwg_Section*) calloc(1, sizeof(Dwg_Section));
      else
        dwg->header.section = (Dwg_Section*) realloc(dwg->header.section,
                       sizeof(Dwg_Section) * (dwg->header.num_sections+1));
      if (!dwg->header.section)
        {
          LOG_ERROR("Out of memory");
          return;
        }

      dwg->header.section[i].number  = *((int32_t*)ptr);
      dwg->header.section[i].size    = *((uint32_t*)ptr+1);
      dwg->header.section[i].address = section_address;
      section_address += dwg->header.section[i].size;
      bytes_remaining -= 8;
      ptr += 8;

      LOG_TRACE("Section[%2d]  %2ld:", i, dwg->header.section[i].number)
      LOG_TRACE(" size=0x%04x", dwg->header.section[i].size)
      LOG_TRACE(" addr=0x%04x\n", dwg->header.section[i].address)

      if (dwg->header.section[i].number < 0) // negative: gap/unused data
        {
          dwg->header.section[i].parent  = *((int32_t*)ptr);
          dwg->header.section[i].left    = *((int32_t*)ptr+1);
          dwg->header.section[i].right   = *((int32_t*)ptr+2);
          dwg->header.section[i].x00     = *((int32_t*)ptr+3);
          bytes_remaining -= 16;
          ptr += 16;

          LOG_TRACE("Parent: %d ", dwg->header.section[i].parent)
          LOG_TRACE("Left:   %d ", dwg->header.section[i].left)
          LOG_TRACE("Right:  %d ", dwg->header.section[i].right)
          LOG_TRACE("0x00:   %d\n", dwg->header.section[i].x00)
        }

      dwg->header.num_sections++;
      i++;
    }
  free(decomp);
}

static Dwg_Section*
find_section(Dwg_Data *dwg, unsigned long int index)
{
  long unsigned int i;
  if (dwg->header.section == 0 || index == 0)
    return 0;
  for (i = 0; i < dwg->header.num_sections; ++i)
    {
      if ((unsigned long int)dwg->header.section[i].number == index)
        return &dwg->header.section[i];
    }
  return 0;
}

/* Read R2004, 2010+ Section Info
 */
static void
read_R2004_section_info(Bit_Chain* dat, Dwg_Data *dwg,
                        unsigned long int comp_data_size,
                        unsigned long int decomp_data_size)
{
  char *decomp, *ptr;
  unsigned int i, j;
  uint32_t section_number;
  uint32_t data_size;
  uint64_t start_offset;

  decomp = (char *)calloc(decomp_data_size, sizeof(char));
  if (!decomp)
    {
      LOG_ERROR("Out of memory");
      return;
    }

  decompress_R2004_section(dat, decomp, comp_data_size);

  dwg->header.num_infos = *(uint32_t*)decomp;
  dwg->header.section_info = (Dwg_Section_Info*)
    calloc(dwg->header.num_infos, sizeof(Dwg_Section_Info));
  if (!dwg->header.section_info)
    {
      LOG_ERROR("Out of memory");
      return;
    }

  LOG_TRACE("\n#### Read 2004 Section Info fields ####\n")
  LOG_TRACE("NumDescriptions:   %d\n", *((int32_t*)decomp))
  LOG_TRACE("Compressed:      0x%x\n", *((int32_t*)decomp + 1))
  LOG_TRACE("MaxSize:         0x%x\n", *((int32_t*)decomp + 2))
  LOG_TRACE("Encrypted:       0x%x\n", *((int32_t*)decomp + 3))
  LOG_TRACE("NumDescriptions2:  %d/0x%x\n", *((int32_t*)decomp + 4),
                                            *((int32_t*)decomp + 4))

  ptr = decomp + 20; // section name char[64]
  for (i = 0; i < dwg->header.num_infos; ++i)
    {
      Dwg_Section_Info* info = &dwg->header.section_info[i];
      info->size            = *((int32_t*)ptr);
      info->pagecount 	    = *((int32_t*)ptr + 1);
      info->num_sections    = *((int32_t*)ptr + 2);
      info->max_decomp_size = *((int32_t*)ptr + 3);
      info->unknown2        = *((int32_t*)ptr + 4);
      info->compressed      = *((int32_t*)ptr + 5);
      info->type            = *((int32_t*)ptr + 6);
      info->encrypted       = *((int32_t*)ptr + 7);
      ptr += 32;
      memcpy(info->name, ptr, 64);
      ptr += 64;

      LOG_TRACE("\nSection Info description fields\n")
      LOG_TRACE("Size:            %u\n", info->size)
      LOG_TRACE("PageCount:       %u\n", info->pagecount)
      LOG_TRACE("Num sections:    %u\n", info->num_sections)
      LOG_TRACE("Max decomp size: %u / 0x%x\n", // normally 0x7400
                info->max_decomp_size, info->max_decomp_size)
      LOG_TRACE("Unknown:         %u\n", info->unknown2)
      LOG_TRACE("Compressed:      %u (1=no, 2=yes)\n", info->compressed)
      LOG_TRACE("Section Type:    %d\n", info->type)
      LOG_TRACE("Encrypted:       %d (0=no, 1=yes, 2=unknown)\n", info->encrypted)
      LOG_TRACE("SectionName:     %s\n\n", info->name)

      info->sections = (Dwg_Section**)
        calloc(info->num_sections, sizeof(Dwg_Section*));
      if (!info->sections)
        {
          LOG_ERROR("Out of memory");
          return;
        }

      if (info->num_sections < 10000)
	{
	  LOG_INFO("Section count %u in area %d\n", info->num_sections, i)

	  for (j = 0; j < info->num_sections; j++)
	    {
	      section_number = *((uint32_t*)ptr);      // Index into SectionMap
	      data_size      = *((uint32_t*)ptr + 1);
	      //start_offset   = *((uint64_t*)ptr + 1); // avoid alignment ubsan
	      start_offset   = *((uint32_t*)ptr + 2);
              start_offset <<= 32;
              start_offset  += *((uint32_t*)ptr + 3);
	      ptr += 16;

	      info->sections[j] = find_section(dwg, section_number);

	      LOG_TRACE("Section Number: %d\n", section_number)
              LOG_TRACE("Data size:      %d\n", data_size) //compressed
	      LOG_TRACE("Start offset:   0x%" PRIx64 "\n", start_offset)
	    }
	}// sanity check
      else
	{
	  LOG_ERROR("Section count %u in area %d too high! Skipping",
                    info->num_sections, i)
	}
    }
  free(decomp);
}

/* Encrypted Section Header */
#pragma pack(1)
typedef union _encrypted_section_header
{
  unsigned long int long_data[8];
  unsigned char char_data[32];
  struct
  {
    unsigned long int tag;
    BITCODE_RL section_type;
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
                            Bit_Chain* sec_dat, BITCODE_RL section_type)
{
  long unsigned int address, sec_mask;
  long unsigned int max_decomp_size;
  Dwg_Section_Info *info = NULL;
  encrypted_section_header es;
  char *decomp;
  unsigned int i, j;

  for (i=0; i < dwg->header.num_infos && !info; ++i)
    {
      if (dwg->header.section_info[i].type == section_type)
        {
          info = &dwg->header.section_info[i];
          break;
        }
    }
  if (!info)
    {
      LOG_WARN("Failed to find section %d", (int)section_type);
      return 1;
    }
  else
    {
      LOG_TRACE("\nFound section %s %d with %d sections\n",
                info->name, (int)section_type, info->num_sections);
    }

  max_decomp_size = info->num_sections * info->max_decomp_size;
  decomp = (char *)calloc(max_decomp_size, sizeof(char));
  if (!decomp)
    {
      LOG_ERROR("Out of memory");
      return 2;
    }

  for (i=0; i < info->num_sections; ++i)
    {
      address = info->sections[i]->address;
      dat->byte = address;
      bit_read_fixed(dat, (char*)es.char_data, 32);

      sec_mask = 0x4164536b ^ address;
      for (j = 0; j < 8; ++j)
        es.long_data[j] ^= sec_mask;

      LOG_INFO("\n=== Section (Class) ===\n")
      LOG_INFO("Section Tag:      0x%x (should be 0x4163043b)\n",
              (unsigned int) es.fields.tag)
      LOG_INFO("Section Type:     0x%x\n",
              (unsigned int) es.fields.section_type)
      // this is the number of bytes that is read in decompress_R2004_section (+ 2bytes)
      LOG_INFO("Data size:        0x%x\n",
              (unsigned int) es.fields.data_size)
      LOG_INFO("Comp data size:   0x%x\n",
              (unsigned int) es.fields.section_size)
      LOG_INFO("StartOffset:      0x%x\n",
              (unsigned int) es.fields.start_offset)
      LOG_INFO("Unknown:          0x%x\n",
              (unsigned int) es.fields.unknown);
      LOG_INFO("Checksum1:        0x%x\n",
            (unsigned int) es.fields.checksum_1)
      LOG_INFO("Checksum2:        0x%x\n\n",
            (unsigned int) es.fields.checksum_2)

      decompress_R2004_section(dat, &decomp[i * info->max_decomp_size],
        es.fields.data_size);
    }

  sec_dat->bit     = 0;
  sec_dat->byte    = 0;
  sec_dat->chain   = (unsigned char *)decomp;
  sec_dat->size    = max_decomp_size;
  sec_dat->version = dat->version;
  sec_dat->from_version = dat->from_version;

  return 0;
}

/* R2004, 2010+ Class Section
 */
static int
read_2004_section_classes(Bit_Chain* dat, Dwg_Data *dwg)
{
  unsigned long int size;
  unsigned long int max_num;
  unsigned long int num_objects, dwg_version, maint_version, unknown;
  char c;
  int error;
  Bit_Chain sec_dat;

  sec_dat.chain = NULL;
  error = read_2004_compressed_section(dat, dwg, &sec_dat, SECTION_CLASSES);
  if (error)
    {
      LOG_ERROR("Failed to read compressed class section");
      if (sec_dat.chain)
        free(sec_dat.chain);
      return error;
    }

  if (bit_search_sentinel(&sec_dat, dwg_sentinel(DWG_SENTINEL_CLASS_BEGIN)))
    {
      size    = bit_read_RL(&sec_dat);  // size of class data area
      max_num = bit_read_BS(&sec_dat);  // Maximum class number
      c = bit_read_RC(&sec_dat);        // 0x00
      c = bit_read_RC(&sec_dat);        // 0x00
      c = bit_read_B(&sec_dat);         // 1

      dwg->layout_number = 0;
      dwg->num_classes = 0;

      do
        {
          unsigned int idc;

          idc = dwg->num_classes;
          if (idc == 0)
            dwg->dwg_class = (Dwg_Class *) calloc(1, sizeof(Dwg_Class));
          else
            dwg->dwg_class = (Dwg_Class *) realloc(dwg->dwg_class, (idc + 1)
                * sizeof(Dwg_Class));
          if (!dwg->dwg_class)
            {
              LOG_ERROR("Out of memory");
              if (sec_dat.chain)
                free(sec_dat.chain);
              return 2;
            }

          dwg->dwg_class[idc].number        = bit_read_BS(&sec_dat);
          dwg->dwg_class[idc].proxyflag     = bit_read_BS(&sec_dat);
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
          LOG_TRACE("Proxyflag:        %x\n", dwg->dwg_class[idc].proxyflag)
          LOG_TRACE("Application name: %s\n", dwg->dwg_class[idc].appname)
          LOG_TRACE("C++ class name:   %s\n", dwg->dwg_class[idc].cppname)
          LOG_TRACE("DXF record name:  %s\n", dwg->dwg_class[idc].dxfname)
          LOG_TRACE("Class ID:         %x\n", dwg->dwg_class[idc].item_class_id)

          if (strcmp((const char *)dwg->dwg_class[idc].dxfname, "LAYOUT") == 0)
            dwg->layout_number = dwg->dwg_class[idc].number;

          dwg->num_classes++;

        } while (sec_dat.byte < (size - 1));
    }
  else
    {
      LOG_ERROR("Failed to find class section sentinel");
      free(sec_dat.chain);
      return 1;
    }
  free(sec_dat.chain);
  return 0;
}

/* R2004, 2010+ Header Section
 */
static int
read_2004_section_header(Bit_Chain* dat, Dwg_Data *dwg)
{
  int error;
  Bit_Chain sec_dat;

  error = read_2004_compressed_section(dat, dwg, &sec_dat, SECTION_HEADER);
  if (error)
    return error;

  if (bit_search_sentinel(&sec_dat, dwg_sentinel(DWG_SENTINEL_VARIABLE_BEGIN)))
    {
      unsigned long int size = bit_read_RL(&sec_dat);
      LOG_TRACE("Length: %lu\n", size);
      dwg_decode_header_variables(&sec_dat, &sec_dat, &sec_dat, dwg);
    }
  free(sec_dat.chain);
  return 0;
}

/* R2004, 2010+ Handles Section
 */
static int
read_2004_section_handles(Bit_Chain* dat, Dwg_Data *dwg)
{
  Bit_Chain obj_dat, hdl_dat;
  BITCODE_RS section_size = 0;
  long unsigned int endpos;
  int error;

  error = read_2004_compressed_section(dat, dwg, &obj_dat, SECTION_OBJECTS);
  if (error)
    return error;

  error = read_2004_compressed_section(dat, dwg, &hdl_dat, SECTION_HANDLES);
  if (error)
    {
      free(obj_dat.chain);
      return error;
    }

  endpos = hdl_dat.byte + hdl_dat.size;
  dwg->num_objects = 0;

  do
    {
      long unsigned int last_offset;
      //long unsigned int last_handle;
      long unsigned int oldpos = 0;
      long unsigned int startpos = hdl_dat.byte;

      section_size = bit_read_RS_LE(&hdl_dat);
      LOG_TRACE("Section size: %u\n", section_size);
      if (section_size > 2034)
        {
          LOG_ERROR("Object-map section size greater than 2034!");
          return 1;
        }

      //last_handle = 0;
      last_offset = 0;
      while (hdl_dat.byte - startpos < section_size)
        {
          long handle, offset;
          oldpos = dat->byte;
          handle = bit_read_MC(&hdl_dat);
          offset = bit_read_MC(&hdl_dat);
          //last_handle += handle;
          last_offset += offset;
          LOG_TRACE("\nNext object: %lu\t", dwg->num_objects)
          LOG_TRACE("Handle: %li\tOffset: %ld @%lu\n", handle, offset, last_offset)

          dwg_decode_add_object(dwg, &obj_dat, &obj_dat, last_offset);
        }

      if (hdl_dat.byte == oldpos)
        break;
      hdl_dat.byte += 2; // CRC

      if (hdl_dat.byte >= endpos)
        break;
    }
  while (section_size > 2);

  LOG_TRACE("\nNum objects: %lu\n", dwg->num_objects);

  free(hdl_dat.chain);
  free(obj_dat.chain);
  return 0;
}

/* for 2004 and 2010+ */
static int
decode_R2004(Bit_Chain* dat, Dwg_Data * dwg)
{
  int j, error = 0;
  Dwg_Section *section;

  {
    struct Dwg_Header* _obj = &dwg->header;
    Dwg_Object *obj = NULL;
    Bit_Chain* hdl_dat = dat;
    int i;

    dat->byte = 0x06;

    #include "header.spec"

  }

  {
    Dwg_Object *obj = NULL;
    struct Dwg_R2004_Header* _obj = &dwg->r2004_header;
    Bit_Chain* hdl_dat = dat;
    const unsigned size = sizeof(struct Dwg_R2004_Header);
    char encrypted_data[size];
    int rseed = 1;
    unsigned i;

    dat->byte = 0x80;
    /* Decrypt */
    for (i = 0; i < size; i++)
      {
        rseed *= 0x343fd;
        rseed += 0x269ec3;
        encrypted_data[i] = bit_read_RC(dat) ^ (rseed >> 0x10);
      }

    LOG_TRACE("\n#### 2004 File Header ####\n");
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

    LOG_TRACE("\n=== Read System Section (Section Page Map) ===\n\n")
    FIELD_RL(section_type, 0);
    if (FIELD_VALUE(section_type) != 0x41630e3b)
      {
        LOG_WARN("Invalid System Section Page Map type 0x%x != 0x41630e3b",
                 FIELD_VALUE(section_type));
      }
    FIELD_RL(decomp_data_size, 0);
    FIELD_RL(comp_data_size, 0);
    FIELD_RL(compression_type, 0);
    FIELD_RL(checksum, 0);

  }

  read_R2004_section_map(dat, dwg);

  if (!dwg->header.section)
    {
      LOG_ERROR("Failed to read R2004 Section Page Map.")
      return -1;
    }

  /*-------------------------------------------------------------------------
   * Section Info
   */
  section = find_section(dwg, dwg->r2004_header.section_info_id);
  if (section)
    {
      Dwg_Object *obj = NULL;
      Dwg_Section* _obj = section;
      LOG_TRACE("\n=== Data Section (Section Info %d) ===\n",
                dwg->r2004_header.section_info_id)
      dat->byte = section->address;

      FIELD_RL(section_type, 0);
      if (FIELD_VALUE(section_type) != 0x4163003b)
        {
          LOG_WARN("Invalid Data Section Page Map type 0x%x != 0x4163003b",
                   FIELD_VALUE(section_type));
        }
      FIELD_RL(decomp_data_size, 0);
      FIELD_RL(comp_data_size, 0);
      FIELD_RL(compression_type, 0);
      FIELD_RL(checksum, 0);

      // Data section map, par 4.5
      read_R2004_section_info(dat, dwg,
         _obj->comp_data_size, _obj->decomp_data_size);
    }

  error += read_2004_section_classes(dat, dwg);
  error += read_2004_section_header(dat, dwg);
  error += read_2004_section_handles(dat, dwg);

  /* Clean up. XXX? Need this to write the sections, at least the name and type */
#if 0
  if (dwg->header.section_info != NULL)
    {
      unsigned u;
      for (u = 0; u < dwg->header.num_infos; ++u)
        if (dwg->header.section_info[u].sections != 0)
          free(dwg->header.section_info[u].sections);

      free(dwg->header.section_info);
      dwg->header.num_infos = 0;
    }
#endif
  resolve_objectref_vector(dat, dwg);

  return error;
}

static int
decode_R2007(Bit_Chain* dat, Dwg_Data * dwg)
{
  Bit_Chain hdl_dat;
  int error;

  hdl_dat = *dat;
  {
    int i;
    struct Dwg_Header *_obj = &dwg->header;
    Dwg_Object *obj = NULL;

    dat->byte = 0x06;
    #include "header.spec"
  }

  // this includes classes, header, handles + objects
  error = read_r2007_meta_data(dat, &hdl_dat, dwg);
  if (error)
    {
      LOG_ERROR("Failed to read 2007 meta data")
      return error;
    }

  LOG_INFO("\nDecoding of DWG version R2007+ objectrefs is not implemented yet.\n")
  LOG_TRACE("Num objects: %lu\n", dwg->num_objects)
  LOG_TRACE("  num object_refs: %lu\n", dwg->num_object_refs)
  return resolve_objectref_vector(dat, dwg);
}

/*--------------------------------------------------------------------------------
 * Private functions
 */

/* for objects and entities */
static int
dwg_decode_eed(Bit_Chain * dat, Dwg_Object_Object * obj)
{
  BITCODE_BS size;
  unsigned int idx = 0;
  int error = 0;

  obj->num_eed = 0;
  while ((size = bit_read_BS(dat)))
    {
      int i;
      BITCODE_BS j;
      BITCODE_RC code = 0;
      long unsigned int end, offset;
      long unsigned int sav_byte;

      LOG_TRACE("EED[%u] size: " FORMAT_BS "\n", idx, size);
      if (size > 1024)
        {
          LOG_ERROR("dwg_decode_eed: Absurd extended object data size: %lu ignored."
                    " Object: %lu (handle)",
                    (long unsigned int) size, obj->object->handle.value)
            obj->bitsize = 0;
          obj->num_eed = 0;
          obj->num_handles = 0;
          obj->num_reactors = 0;
          return -1; //XXX
        }

      if (idx) {
        obj->eed = (Dwg_Eed*)realloc(obj->eed, (idx+1) * sizeof(Dwg_Eed));
      } else {
        obj->eed = (Dwg_Eed*)calloc(1, sizeof(Dwg_Eed));
      }
      obj->eed[idx].size = size;
      error = bit_read_H(dat, &obj->eed[idx].handle);
      if (error) {
        LOG_ERROR("No EED[%d].handle", idx);
        return error;
      } else {
        end = dat->byte + size;
        LOG_TRACE("EED[%u] handle: %d.%d.%lu\n", idx,
                  obj->eed[idx].handle.code, obj->eed[idx].handle.size,
                  obj->eed[idx].handle.value);
        if (obj->object->supertype == DWG_SUPERTYPE_OBJECT &&
            obj->object->dxfname &&
            !strcmp(obj->object->dxfname, "MLEADERSTYLE"))
          { // check for is_new_format: has extended data for APPID “ACAD_MLEADERVER”
            Dwg_Object_Ref ref;
            ref.obj = NULL;
            ref.handleref = obj->eed[idx].handle;
            ref.absolute_ref = 0L;
            if (dwg_resolve_handleref(&ref, obj->object))
              {
                Dwg_Data *dwg = obj->object->parent;
                Dwg_Object_APPID_CONTROL *appid = dwg->appid_control;
                if (appid)
                  {
                    // search absref in APPID_CONTROL apps[]
                    for (j=0; j < appid->num_entries; j++)
                      {
                        if ( appid->apps[j]->absolute_ref == ref.absolute_ref )
                          {
                            Dwg_Object_MLEADERSTYLE *this = obj->tio.MLEADERSTYLE;
                            this->is_new_format = 1;
                            LOG_TRACE("EED found ACAD_MLEADERVER %lu: new format\n",
                                      ref.absolute_ref);
                          }
                      }
                  }
              }
          }
      }

      sav_byte = dat->byte;
      obj->eed[idx].raw = bit_read_TF(dat, size);
      LOG_TRACE_TF(obj->eed[idx].raw, size);
      dat->byte = sav_byte;

      while (dat->byte < end)
        {
          int lenc;
          BITCODE_RS lens;

          obj->eed[idx].data = (Dwg_Eed_Data*)calloc(size + 1, 1);
          obj->eed[idx].data->code = code = bit_read_RC(dat);
          LOG_TRACE("EED[%u] code: %d\n", idx, (int)code);
          switch (code)
            {
            case 0:
              PRE(R_2007) {
                obj->eed[idx].data->u.eed_0.length = lenc = bit_read_RC(dat);
                obj->eed[idx].data->u.eed_0.codepage = bit_read_RS_LE(dat);
                if (lenc > size-4)
                  {
                    LOG_ERROR("Invalid EED string len %d, max %d", lenc, size-4);
                    dat->byte = end;
                    return 1;
                  }
                /* code:1 + len:1 + cp:2 */
                bit_read_fixed(dat, obj->eed[idx].data->u.eed_0.string, lenc);
                obj->eed[idx].data->u.eed_0.string[lenc] = '\0';
                LOG_TRACE("EED[%u] string: %s len=%d cp=%d\n", idx,
                          obj->eed[idx].data->u.eed_0.string, (int)lenc,
                          (int)obj->eed[idx].data->u.eed_0.codepage);
              } LATER_VERSIONS {
                obj->eed[idx].data->u.eed_0_r2007.length = lens = bit_read_RS(dat);
                /* code:1 + len:2 NUL? */
                for (j=0; j < MIN(lens,(size-3)/2); j++)
                  obj->eed[idx].data->u.eed_0_r2007.string[j] = bit_read_RS_LE(dat);
                obj->eed[idx].data->u.eed_0_r2007.string[j] = 0;
#ifdef _WIN32
                LOG_TRACE("EED[%u] string: " FORMAT_TU " len=%d\n", idx,
                          obj->eed[idx].data->u.eed_0_r2007.string, (int)lens);
#endif
              }
              break;
            case 2:
              obj->eed[idx].data->u.eed_2.byte = bit_read_RC(dat);
              LOG_TRACE("EED[%u] byte: " FORMAT_RC "\n", idx,
                        obj->eed[idx].data->u.eed_2.byte);
              break;
            case 3:
            case 5:
              obj->eed[idx].data->u.eed_3.layer = bit_read_RL(dat);
              LOG_TRACE("EED[%u] layer/...: " FORMAT_RL "\n", idx,
                        obj->eed[idx].data->u.eed_3.layer);
              break;
            case 4:
              obj->eed[idx].data->u.eed_4.length = lenc = bit_read_RC(dat);
              /* code:1 + len:1 */
              for (j=0; j < MIN(lenc,size-2); j++)
                obj->eed[idx].data->u.eed_4.data[j] = bit_read_RC(dat);
              LOG_TRACE("EED[%u] raw: %s\n", idx, obj->eed[idx].data->u.eed_4.data);
              break;
            case 10: case 11: case 12: case 13:
              obj->eed[idx].data->u.eed_10.point.x = bit_read_RD(dat);
              obj->eed[idx].data->u.eed_10.point.y = bit_read_RD(dat);
              obj->eed[idx].data->u.eed_10.point.z = bit_read_RD(dat);
              LOG_TRACE("EED[%u] 3dpoint: %f, %f, %f\n", idx,
                        obj->eed[idx].data->u.eed_10.point.x,
                        obj->eed[idx].data->u.eed_10.point.y,
                        obj->eed[idx].data->u.eed_10.point.z);
              break;
            case 40: case 41: case 42:
              obj->eed[idx].data->u.eed_40.real = bit_read_RD(dat);
              LOG_TRACE("EED[%u] real: %f\n", idx, obj->eed[idx].data->u.eed_40.real);
              break;
            case 70:
              obj->eed[idx].data->u.eed_70.rs = bit_read_RS(dat);
              LOG_TRACE("EED[%u] short: %d\n", idx, (int)obj->eed[idx].data->u.eed_70.rs);
              break;
            case 71:
              obj->eed[idx].data->u.eed_71.rl = bit_read_RL(dat);
              LOG_TRACE("EED[%u] long: %d\n", idx, (int)obj->eed[idx].data->u.eed_71.rl);
              break;
            default:
              LOG_WARN("Unknown EED code %d", code);
            }
#ifdef DEBUG
          // sanity checks
          if (code == 0 || code == 4)
            assert(obj->eed[idx].data->u.eed_0.length <= size-1);
          if (code == 10) // 3 double
            assert(size >= 1 + 3*8);
#endif

          idx++;
          obj->num_eed++;
          if (dat->byte < end-1)
            {
              size = (long)(end - dat->byte + 1);
              LOG_HANDLE("EED[%u] size remaining: %ld\n", idx, (long)size);

              obj->eed = (Dwg_Eed*)realloc(obj->eed, (idx+1) * sizeof(Dwg_Eed));
              obj->eed[idx].handle = obj->eed[idx-1].handle;
              obj->eed[idx].size = 0;
            }
        }
      dat->byte = end;
    }
  return error;
}

/* The first common part of every entity.

   The last common part is common_entity_handle_data.spec
   called by COMMON_ENTITY_HANDLE_DATA in dwg.spec

   For EED check page 269, par 28 (Extended Object Data)
 */
static int
dwg_decode_entity(Bit_Chain* dat, Bit_Chain* hdl_dat, Bit_Chain* str_dat,
                  Dwg_Object_Entity* ent)
{
  unsigned int i;
  BITCODE_BS size;
  int error;

  VERSIONS(R_2000, R_2010)
    {
      SINCE(R_2007)
        {
          *str_dat = *dat;
        }
      ent->bitsize = bit_read_RL(dat); // until the handles
      LOG_TRACE("Entity bitsize: " FORMAT_BL " @%lu.%u\n", ent->bitsize, dat->byte, dat->bit)
    }
  SINCE(R_2007)
    {
      // set the handle stream offset
      ent->object->hdlpos = bit_position(dat) + ent->bitsize;
      ent->object->bitsize = ent->bitsize;
      // and set the string stream (restricted to size)
      obj_string_stream(dat, ent->object, str_dat);
    }

  error = bit_read_H(dat, &(ent->object->handle));
  if (error)
    {
      LOG_WARN(
          "dwg_decode_entity read ent->object->handle:\tCurrent Bit_Chain address: 0x%0x",
          (unsigned int) dat->byte)
      ent->bitsize = 0;
      ent->num_eed = 0;
      ent->picture_exists = 0;
      ent->num_handles = 0;
      return 0;
    }
  LOG_INFO("Entity handle: %d.%d.%lu\n",
           ent->object->handle.code, ent->object->handle.size, ent->object->handle.value)

  error = dwg_decode_eed(dat, (Dwg_Object_Object *)ent);
  if (error)
    return error;

  ent->picture_exists = bit_read_B(dat);
  if (ent->picture_exists)
    {
      LOG_TRACE("picture_exists: 1\n")
      VERSIONS(R_13, R_2007)
        {
          ent->picture_size = (BITCODE_BLL)bit_read_RL(dat);
        }
      SINCE(R_2010)
        {
          ent->picture_size = bit_read_BLL(dat);
        }

      LOG_TRACE("picture_size: " FORMAT_BLL " \n", ent->picture_size)
      if (ent->picture_size < 210210)
        {
          ent->picture = bit_read_TF(dat, ent->picture_size);
        }
      else
        {
          LOG_ERROR(
              "dwg_decode_entity:  Absurd! Picture-size: %lu kB. Object: %lu (handle)",
              (unsigned long)(ent->picture_size / 1000), ent->object->handle.value)
          bit_advance_position(dat, -(4 * 8 + 1));
        }
    }

  VERSIONS(R_13, R_14)
    {
      ent->bitsize = bit_read_RL(dat);
    }

  ent->entity_mode  = bit_read_BB(dat);
  ent->num_reactors = bit_read_BL(dat);

  SINCE(R_2004)
    {
      ent->xdic_missing_flag = bit_read_B(dat);
    }
  SINCE(R_2013)
    {
      ent->has_ds_binary_data = bit_read_B(dat);
    }
  VERSIONS(R_13, R_14)
    {
      ent->isbylayerlt = bit_read_B(dat);
    }

  ent->nolinks = bit_read_B(dat);

  SINCE(R_2004)
    {
      BITCODE_B color_mode = 0;
      unsigned int flags;

      if (ent->nolinks == 0)
        {
          color_mode = bit_read_B(dat);

          if (color_mode == 1)
            {
              ent->color.index = bit_read_RC(dat);  // color index
              ent->color.rgb = 0L;
            }
          else
            {
              flags = bit_read_RS(dat);

              if (flags & 0x8000)
                {
                  unsigned char c1, c2, c3, c4;
                  char *name;

                  c1 = bit_read_RC(dat);  // rgb color
                  c2 = bit_read_RC(dat);
                  c3 = bit_read_RC(dat);
                  c4 = bit_read_RC(dat);
                  name = bit_read_TV(dat);
                  ent->color.index = 0;
                  ent->color.rgb   = c1 << 24 | c2 << 16 | c3 << 8 | c4;
                  strcpy(ent->color.name, name);
                }

              /*if (flags & 0x4000)
                flags = flags;   // has AcDbColor reference (handle)
              */
              if (flags & 0x2000)
                {
                  ent->color.transparency_type = bit_read_BL(dat);
                }
            }
        }
      else
        {
          char color = bit_read_B(dat);
          ent->color.index = color;
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

  SINCE(R_2010)
    {
      ent->has_full_visualstyle = bit_read_B(dat);
      ent->has_face_visualstyle = bit_read_B(dat);
      ent->has_edge_visualstyle = bit_read_B(dat);
    }

  ent->invisible = bit_read_BS(dat);

  SINCE(R_2000)
    {
      ent->lineweight = bit_read_RC(dat);
    }

  // elsewhere: object data, handles, padding bits, crc

  return 0;
}

/* The first common part of every object.

   There is no COMMON_ENTITY_HANDLE_DATA for objects.
   Check page 269, par 28 (Extended Object Data)
 */
static int
dwg_decode_object(Bit_Chain* dat, Bit_Chain* hdl_dat, Bit_Chain* str_dat,
                  Dwg_Object_Object* obj)
{
  unsigned int i;
  BITCODE_BS size;
  int error = 2;

  obj->datpos = dat->byte;     // the data stream offset
  VERSIONS(R_2000, R_2007)
    {
      SINCE(R_2007)
        {
          *str_dat = *dat;
        }
      obj->bitsize = bit_read_RL(dat);
    }
  SINCE(R_2010)
    {
      obj->bitsize = dat->size - 16;
    }
  SINCE(R_2007)
    {
      LOG_INFO("Object bitsize: " FORMAT_RL " @%lu.%u %lu\n", obj->bitsize,
               dat->byte, dat->bit, bit_position(dat));
      // the handle stream offset, i.e. end of the object, right after the has_strings bit.
      // minus 1* RL
      obj->object->hdlpos = bit_position(dat) + obj->bitsize;
      obj->object->bitsize = obj->bitsize;
      obj_string_stream(dat, obj->object, str_dat);
    }

  if (bit_read_H(dat, &obj->object->handle))
    {
      LOG_ERROR(
          "\tError in object handle! Bit_Chain current address: 0x%0x",
          (unsigned int) dat->byte)
      obj->bitsize = 0;
      obj->num_eed = 0;
      obj->num_handles = 0;
      obj->num_reactors = 0;
      return -1;
    }
  LOG_INFO("Object handle: %d.%d.%lu\n",
           obj->object->handle.code, obj->object->handle.size,
           obj->object->handle.value)

  error = dwg_decode_eed(dat, obj);
  if (error)
    return error;

  VERSIONS(R_13,R_14)
    {
      obj->bitsize = bit_read_RL(dat);
    }
  // documentation bug
  obj->num_reactors = bit_read_BL(dat);
  SINCE(R_2004)
    {
      obj->xdic_missing_flag = bit_read_B(dat);
    }
  SINCE(R_2013)
    {
      obj->has_ds_binary_data = bit_read_B(dat);
    }

  return 0;
}

/**
 * Find a pointer to an object given it's id (handle)
 */
static Dwg_Object *
dwg_resolve_handle(Dwg_Data * dwg, long unsigned int absref)
{
  //TODO hash table? this is linear search, absref's are unsorted.
  long unsigned int i;
  for (i = 0; i < dwg->num_objects; i++)
    {
      if (dwg->object[i].handle.value == absref)
        {
          return &dwg->object[i];
        }
    }
  LOG_WARN("Object not found: %lu in %ld objects", absref, dwg->num_objects)
  return NULL;
}

static int
dwg_resolve_handleref(Dwg_Object_Ref *ref, Dwg_Object * obj)
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

Dwg_Object_Ref *
dwg_decode_handleref(Bit_Chain * dat, Dwg_Object * obj, Dwg_Data* dwg)
{
  // Welcome to the house of evil code
  Dwg_Object_Ref* ref = (Dwg_Object_Ref *) calloc(1, sizeof(Dwg_Object_Ref));
  if (!ref)
    {
      LOG_ERROR("Out of memory");
      return NULL;
    }

  if (bit_read_H(dat, &ref->handleref))
    {
      if (obj)
        {
          LOG_ERROR(
            "Could not read handleref from object whose handle is: %d.%d.%lu",
            obj->handle.code, obj->handle.size, obj->handle.value)
        }
      else
        {
          LOG_ERROR("Could not read handleref in the header variables section")
        }
      free(ref);
      return NULL;
    }

  // If the handle size is 0, it is probably a null handle.
  // It shouldn't be placed in the object ref vector
  if (ref->handleref.size)
    {
      // Reserve memory space for object references
      if (!dwg->num_object_refs)
        dwg->object_ref = (Dwg_Object_Ref **) calloc(REFS_PER_REALLOC, sizeof(Dwg_Object_Ref*));
      else
        if (dwg->num_object_refs % REFS_PER_REALLOC == 0)
          {
            dwg->object_ref = (Dwg_Object_Ref **) realloc(dwg->object_ref,
                (dwg->num_object_refs + REFS_PER_REALLOC) * sizeof(Dwg_Object_Ref*));
          }

      if (!dwg->object_ref)
        {
          LOG_ERROR("Out of memory");
          return NULL;
        }
      dwg->object_ref[dwg->num_object_refs++] = ref;
    }
  else
    {
      ref->obj = NULL;
      ref->absolute_ref = 0;
      return ref;
    }

  // we receive a null obj when we are reading
  // handles in the header variables section
  if (!obj)
    {
      ref->absolute_ref = ref->handleref.value;
      ref->obj = NULL;
      return ref;
    }

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
      break;
    }
  return ref;
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
Dwg_Object_Ref *
dwg_decode_handleref_with_code(Bit_Chain * dat, Dwg_Object * obj, Dwg_Data* dwg,
                               unsigned int code)
{
  Dwg_Object_Ref * ref;
  ref = dwg_decode_handleref(dat, obj, dwg);
  if (!ref)
    {
      LOG_ERROR("Invalid handleref");
      return NULL;
    }

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
  return ref;
}

void
dwg_decode_header_variables(Bit_Chain* dat, Bit_Chain* hdl_dat, Bit_Chain* str_dat,
                            Dwg_Data * dwg)
{
  Dwg_Header_Variables* _obj = &dwg->header_vars;
  Dwg_Object* obj = NULL;

  #include "header_variables.spec"
}

static void
dwg_decode_common_entity_handle_data(Bit_Chain* dat, Bit_Chain* hdl_dat, Dwg_Object* obj)
{

  Dwg_Data *dwg = obj->parent;
  Dwg_Object_Entity *_obj;
  long unsigned int vcount;
  Dwg_Object_Entity *ent;

  ent = obj->tio.entity;
  _obj = ent;

  #include "common_entity_handle_data.spec"

}

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

// TODO: unify with eed[], use an array not linked list.
static Dwg_Resbuf*
dwg_decode_xdata(Bit_Chain * dat, Dwg_Object_XRECORD *obj, int size)
{
  Dwg_Resbuf *rbuf, *root=0, *curr=0;
  unsigned char codepage;
  long unsigned int end_address;
  int i, length, num_xdata = 0;

  static int cnt = 0;
  cnt++;

  end_address = dat->byte + (unsigned long int)size;

  while (dat->byte < end_address)
    {
      rbuf = (Dwg_Resbuf *) calloc(1, sizeof(Dwg_Resbuf));
      if (!rbuf)
        {
          LOG_ERROR("Out of memory");
          return NULL;
        }
      num_xdata++;
      rbuf->next = 0;
      rbuf->type = bit_read_RS(dat);

      switch (get_base_value_type(rbuf->type))
        {
        case VT_STRING:
          UNTIL(R_2007) {
            length = rbuf->value.str.size = bit_read_RS(dat);
            rbuf->value.str.codepage = bit_read_RC(dat);
            rbuf->value.str.u.data = bit_read_TF(dat, length);
          } LATER_VERSIONS {
            length = rbuf->value.str.size = bit_read_RS(dat);
            if (length > 0)
              {
                rbuf->value.str.u.wdata = calloc(length + 1, 2);
                if (!rbuf->value.str.u.wdata)
                  {
                    LOG_ERROR("Out of memory");
                    return NULL;
                  }
                for (i = 0; i < length; i++)
                  rbuf->value.str.u.wdata[i] = bit_read_RS(dat);
                rbuf->value.str.u.wdata[i] = '\0';
              }
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
          rbuf->value.str.size = bit_read_RC(dat);
          rbuf->value.str.u.data = bit_read_TF(dat, rbuf->value.str.size);
          break;
        case VT_HANDLE:
        case VT_OBJECTID:
          bit_read_fixed(dat, (char*)rbuf->value.hdl, 8);
          break;
        case VT_INVALID:
        default:
          LOG_ERROR("Invalid group code in xdata: %d", rbuf->type)
          free (rbuf);
          dat->byte = end_address;
          obj->num_eed = num_xdata;
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
    obj->num_eed = num_xdata;
    return root;
}

/* OBJECTS *******************************************************************/

#include "dwg.spec"

/*--------------------------------------------------------------------------------
 * Private functions which depend on the preceding
 */

/** dwg_decode_variable_type
 * decode object by class name, not type. if type > 500.
 * returns 1 if object could be decoded and 0 otherwise.
 */
static int
dwg_decode_variable_type(Dwg_Data* dwg, Bit_Chain* dat, Bit_Chain* hdl_dat, Dwg_Object* obj)
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
      //UNTESTED_CLASS;
      assert(!is_entity);
      dwg_decode_DICTIONARYWDLFT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "DICTIONARYVAR"))
    {
      assert(!is_entity);
      dwg_decode_DICTIONARYVAR(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "HATCH"))
    {
      assert(!is_entity);
      dwg_decode_HATCH(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "GROUP"))
    {
      assert(!is_entity);
      dwg_decode_GROUP(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IDBUFFER"))
    {
      assert(!is_entity);
      dwg_decode_IDBUFFER(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IMAGE"))
    {
      assert(is_entity);
      dwg_decode_IMAGE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IMAGEDEF"))
    {
      assert(!is_entity);
      dwg_decode_IMAGEDEF(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IMAGEDEF_REACTOR"))
    {
      assert(!is_entity);
      dwg_decode_IMAGEDEF_REACTOR(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "LAYER_INDEX"))
    {
      assert(!is_entity);
      dwg_decode_LAYER_INDEX(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "LAYOUT"))
    {
      assert(!is_entity);
      dwg_decode_LAYOUT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "LWPLINE"))
    {
      assert(is_entity);
      dwg_decode_LWPLINE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "OLE2FRAME"))
    {
      assert(is_entity);
      dwg_decode_OLE2FRAME(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "OBJECTCONTEXTDATA") ||
      !strcmp(klass->cppname, "AcDbObjectContextData"))
    {
      assert(!is_entity);
      dwg_decode_OBJECTCONTEXTDATA(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "ACDBPLACEHOLDER"))
    {
      assert(!is_entity);
      dwg_decode_PLACEHOLDER(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "PROXY"))
    {
      assert(!is_entity);
      dwg_decode_PROXY(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "RASTERVARIABLES"))
    {
      assert(!is_entity);
      dwg_decode_RASTERVARIABLES(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SORTENTSTABLE"))
    {
      assert(!is_entity);
      dwg_decode_SORTENTSTABLE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SPATIAL_FILTER"))
    {
      assert(!is_entity);
      dwg_decode_SPATIAL_FILTER(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SPATIAL_INDEX"))
    {
      assert(!is_entity);
      dwg_decode_SPATIAL_INDEX(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "TABLE"))
    {
      assert(is_entity);
      dwg_decode_TABLE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "XRECORD"))
    {
      assert(!is_entity);
      dwg_decode_XRECORD(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "WIPEOUT"))
    {
      assert(is_entity);
      dwg_decode_WIPEOUT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "FIELDLIST"))
    {
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_decode_FIELDLIST(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SCALE"))
    {
      //UNTESTED_CLASS;
      assert(!is_entity);
      dwg_decode_SCALE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "AcDbField"))
    {
      UNTESTED_CLASS;
      dwg_decode_FIELD(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "TABLECONTENT"))
    {
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_decode_TABLECONTENT(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "TABLEGEOMETRY"))
    {
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_decode_TABLEGEOMETRY(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "GEODATA"))
    {
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_decode_GEODATA(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "VBA_PROJECT"))
    {
      assert(!is_entity);
#ifdef DEBUG_VBA_PROJECT
      // Has its own section?
      UNTESTED_CLASS;
      dwg_decode_VBA_PROJECT(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      return 0;
#endif
    }
  if (!strcmp(dxfname, "MULTILEADER"))
    {
      assert(is_entity);
#ifdef DEBUG_MULTILEADER
      UNTESTED_CLASS; //broken Leader_Line's/Points
      dwg_decode_MULTILEADER(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      return 0;
#endif
    }
  if (!strcmp(dxfname, "MLEADERSTYLE"))
    {
      assert(!is_entity);
      dwg_decode_MLEADERSTYLE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "WIPEOUTVARIABLE"))
    {
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_decode_WIPEOUTVARIABLE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "CELLSTYLEMAP"))
    {
      assert(!is_entity);
#ifdef DEBUG_CELLSTYLEMAP
      UNTESTED_CLASS; //broken
      dwg_decode_CELLSTYLEMAP(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      return 0;
#endif
    }
  if (!strcmp(dxfname, "VISUALSTYLE"))
    {
      dwg_decode_VISUALSTYLE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "ARCALIGNEDTEXT"))
    {
      UNHANDLED_CLASS;
      //assert(!is_entity);
      //dwg_decode_ARCALIGNEDTEXT(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "DIMASSOC"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_decode_DIMASSOC(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "MATERIAL"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_decode_MATERIAL(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "TABLESTYLE"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_decode_TABLESTYLE(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "DBCOLOR"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_decode_DBCOLOR(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDBSECTIONVIEWSTYLE"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_decode_SECTIONVIEWSTYLE(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDBDETAILVIEWSTYLE"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_decode_DETAILVIEWSTYLE(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDBASSOCNETWORK"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_decode_ASSOCNETWORK(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDBASSOC2DCONSTRAINTGROUP"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_decode_ASSOC2DCONSTRAINTGROUP(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDBASSOCGEOMDEPENDENCY"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_decode_ASSOCGEOMDEPENDENCY(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDB_LEADEROBJECTCONTEXTDATA_CLASS"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_decode_LEADEROBJECTCONTEXTDATA(dat, obj);
      return 0;
    }

  LOG_WARN("Unknown Class %s %d %s (0x%x%s)", is_entity ? "entity" : "object", \
           klass->number, dxfname, klass->proxyflag,                    \
           klass->wasazombie ? " was proxy" : "")

  /* TODO: CELLSTYLEMAP, DBCOLOR, MATERIAL, MLEADER, MLEADERSTYLE,
     PLOTSETTINGS, SCALE, TABLEGEOMETRY,
     TABLESTYLE, VBA_PROJECT, VISUALSTYLE, WIPEOUTVARIABLE,
     ACDBSECTIONVIEWSTYLE, ACDBDETAILVIEWSTYLE,
     NPOCOLLECTION, EXACXREFPANELOBJECT,
     ARCALIGNEDTEXT (2000+), UNDERLAYDEFINITION (2 strings),
     AcDbObjectContextData,
     AcDbAnnotScaleObjectContextData (H 340)
  */
#undef UNHANDLED_CLASS
#undef UNTESTED_CLASS

  return 0;
}

void
dwg_decode_add_object(Dwg_Data* dwg, Bit_Chain* dat, Bit_Chain* hdl_dat,
                      long unsigned int address)
{
  long unsigned int oldpos;
  long unsigned int object_address, end_address;
  unsigned char previous_bit;
  Dwg_Object *obj;
  long unsigned int num = dwg->num_objects;

  /* Keep the previous address
   */
  oldpos = dat->byte;
  previous_bit = dat->bit;

  /* Use the indicated address for the object
   */
  dat->byte = address;
  dat->bit = 0;

  //DEBUG_HERE();
  /*
   * Reserve memory space for objects
   */
  if (!num)
    dwg->object = (Dwg_Object *) malloc(sizeof(Dwg_Object));
  else
    dwg->object = (Dwg_Object *) realloc(dwg->object, (num + 1)
        * sizeof(Dwg_Object));
  if (!dwg->object)
    {
      LOG_ERROR("Out of memory");
      return;
    }

  LOG_INFO("==========================================\n"
           "Object number: %lu", num)

  obj = &dwg->object[num];
  memset(obj, 0, sizeof(Dwg_Object));
  obj->index = num;
  dwg->num_objects++;
  obj->parent = dwg;
  obj->size = bit_read_MS(dat);
  LOG_INFO(", Size: %d/0x%x", obj->size, obj->size)
  object_address = dat->byte;
  end_address = object_address + obj->size; /* (calculate the bitsize) */

  SINCE(R_2010)
  {
    obj->handlestream_size = bit_read_MC(dat);
    LOG_INFO(", Hdlsize: %ld/0x%x",
             obj->handlestream_size, (unsigned)obj->handlestream_size)
    obj->type = bit_read_BOT(dat);
  } else {
    obj->type = bit_read_BS(dat);
  }
  LOG_INFO(", Type: %d\n", obj->type)

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
      /* XXX
       * We cannot cache dwg->*space_block here as dwg->objects might get realloc'ed
       */
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
      if (obj->tio.object->tio.APPID_CONTROL->num_entries)
        obj->parent->appid_control = obj->tio.object->tio.APPID_CONTROL;
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
    case DWG_TYPE_OLE2FRAME:
      dwg_decode_OLE2FRAME(dat, obj);
      break;
    case DWG_TYPE_DUMMY:
      dwg_decode_DUMMY(dat, obj);
      break;
    case DWG_TYPE_LONG_TRANSACTION:
      dwg_decode_LONG_TRANSACTION(dat, obj);
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
    case DWG_TYPE_PROXY_ENTITY:
      dwg_decode_PROXY_ENTITY(dat, obj);
      break;
    case DWG_TYPE_OLEFRAME:
      dwg_decode_OLEFRAME(dat, obj);
      break;
    case DWG_TYPE_VBA_PROJECT:
      LOG_ERROR("Unhandled Object VBA_PROJECT. Has its own section");
      //dwg_decode_VBA_PROJECT(dat, obj);
      break;
    case DWG_TYPE_LAYOUT:
      dwg_decode_LAYOUT(dat, obj);
      break;
    default:
      if (obj->type == obj->parent->layout_number)
        dwg_decode_LAYOUT(dat, obj);

      else if (!dwg_decode_variable_type(dwg, dat, hdl_dat, obj))
        {
          int is_entity;
          int i = obj->type - 500;
          Dwg_Class *klass = NULL;
          BITCODE_MS size;
          BITCODE_BS type;

          if (dat->byte != end_address)
            {
              LOG_TRACE("wrong offset: %lu, got %lu (%ld/%d)\n",
                        end_address, dat->byte, end_address - dat->byte,
                        obj->bitsize/8)
            }
          dat->byte = address;   // restart and read into the UNKNOWN_OBJ object
          dat->bit = 0;
          size = bit_read_MS(dat);
          type = bit_read_BS(dat);

          if (i <= (int)dwg->num_classes)
            {
              klass = &dwg->dwg_class[i];
              is_entity = dwg_class_is_entity(klass);
            }
          // properly dwg_decode_object/_entity for eed, reactors, xdic
          if (klass && !is_entity)
            {
              dwg_decode_UNKNOWN_OBJ(dat, obj);
            }
          else if (klass)
            {
              dwg_decode_UNKNOWN_ENT(dat, obj);
            }
          else // not a class
            {
              LOG_WARN("Unknown object, skipping eed/reactors/xdic");
              SINCE(R_2000)
              {
                obj->bitsize = bit_read_RL(dat);
                LOG_INFO("Object bitsize: " FORMAT_RL " @%lu.%u\n", obj->bitsize,
                         dat->byte, dat->bit);
              }
              if (!bit_read_H(dat, &obj->handle))
                {
                  LOG_INFO("Object handle: %d.%d.%lu\n",
                           obj->handle.code, obj->handle.size, obj->handle.value)
                }
              object_address = dat->byte;
              obj->supertype = DWG_SUPERTYPE_UNKNOWN;
              obj->tio.unknown = (unsigned char *)bit_read_TF(dat, obj->size);
              dat->byte = object_address;
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
  dat->byte = oldpos;
  dat->bit = previous_bit;
}

#undef IS_DECODER
