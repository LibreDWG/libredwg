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
 * decode.c: decoding functions
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Till Heuschmann
 * modified by Reini Urban
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#ifdef HAVE_WCHAR_H
# include <wchar.h>
#endif

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "hash.h"
#include "decode.h"
#include "print.h"

/* The logging level for the read (decode) path.  */
static unsigned int loglevel;
/* the current version per spec block */
static int cur_ver = 0;

#ifdef USE_TRACING
/* This flag means we have checked the environment variable
   LIBREDWG_TRACE and set `loglevel' appropriately.  */
static bool env_var_checked_p;
#endif  /* USE_TRACING */
#define DWG_LOGLEVEL loglevel

#include "logging.h"
#include "dec_macros.h"

#define REFS_PER_REALLOC 128

#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

/*------------------------------------------------------------------------------
 * Private functions
 */

static int
resolve_objectref_vector(Bit_Chain* dat, Dwg_Data * dwg);

static void
decode_preR13_section_ptr(const char* name, Dwg_Section_Type_r11 id,
                          Bit_Chain* dat, Dwg_Data * dwg);
static void
decode_preR13_section_chk(Dwg_Section_Type_r11 id, Bit_Chain* dat,
                          Dwg_Data * dwg);
static int
decode_preR13_section(Dwg_Section_Type_r11 id, Bit_Chain* dat, Dwg_Data * dwg);
static int
decode_preR13_entities(unsigned long start, unsigned long end,
                       unsigned long offset, Bit_Chain* dat, Dwg_Data * dwg);

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
  //dwg->num_layers = 0; // see now dwg->layer_control->num_entries
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
  dwg->object_map = hash_new(dat->size/1000);
  if (!dwg->object_map)
    {
      dwg->object_map = hash_new(1024); //whatever, we are obviously on a tiny system
      if (!dwg->object_map)
        {
          LOG_ERROR("Out of memory");
          return DWG_ERR_OUTOFMEM;
        }
    }

  memset(&dwg->header_vars, 0, sizeof(Dwg_Header_Variables));
  memset(&dwg->r2004_header.file_ID_string[0], 0, sizeof(dwg->r2004_header));
  memset(&dwg->auxheader.aux_intro[0], 0, sizeof(dwg->auxheader));
  memset(&dwg->second_header, 0, sizeof(dwg->second_header));

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
  if (!dwg->header.version)
    {
      if (strncmp(version, "AC", 2)) // let's ignore MC0.0 for now
        {
          LOG_ERROR("Invalid DWG, magic: %s", version);
        }
      else
        {
          LOG_ERROR("Invalid or unimplemented DWG version code %s",
                    version);
        }
      return DWG_ERR_INVALIDDWG;
    }
  dat->version = dwg->header.version;
  dwg->header.from_version = dat->version;
  dat->from_version = dat->version;
  LOG_INFO("This file's version code is: %s\n", version)

#define WE_CAN \
    "This version of LibreDWG is only capable of decoding " \
    "version R13-R2018 (code: AC1012-AC1032) DWG files.\n"

  PRE(R_13)
    {
      LOG_ERROR(WE_CAN "We don't decode many entities and no blocks yet.")
#ifndef IS_RELEASE
      return decode_preR13(dat, dwg);
#endif
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
      return decode_R2007(dat, dwg);
    }
  SINCE(R_2010)
    {
      read_r2007_init(dwg);
      return decode_R2004(dat, dwg);
    }

  // This line should not be reached
  LOG_ERROR("LibreDWG does not support this version: %s.", version)
  return DWG_ERR_INVALIDDWG;
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
static int
decode_preR13_section(Dwg_Section_Type_r11 id, Bit_Chain* dat, Dwg_Data * dwg)
{
  Dwg_Section *tbl = &dwg->header.section[id];
  int i;
  BITCODE_BL vcount;
  int error = 0;
  long unsigned int num = dwg->num_objects;
  long unsigned int old_size = num * sizeof(Dwg_Object);
  long unsigned int size = tbl->number * sizeof(Dwg_Object);
  long unsigned int pos;

  LOG_TRACE("\ncontents table %-8s [%2d]: size:%-4u nr:%-3ld (0x%x-0x%lx)\n",
            tbl->name, id, tbl->size, tbl->number, tbl->address,
            (long)(tbl->address + tbl->number * tbl->size))
  dat->byte = tbl->address;
  if (dwg->num_objects % REFS_PER_REALLOC == 0)
    dwg->object = realloc(dwg->object, old_size + size + REFS_PER_REALLOC);

  // TODO: move to a spec dwg_r11.spec, and dwg_decode_r11_NAME
#define PREP_TABLE(name)\
  Dwg_Object *obj = &dwg->object[num + i];                              \
  Dwg_Object_##name *_obj = calloc (1, sizeof(Dwg_Object_##name));      \
  obj->tio.object = calloc (1, sizeof(Dwg_Object_Object));              \
  obj->tio.object->tio.name = _obj;                                     \
  obj->tio.object->objid = obj->index;                                  \
  obj->parent = dwg;                                                    \
  obj->dxfname = (char*)#name;                                          \
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
          //TODO DXF 8: layer name
          FIELD_RC (flag, 70);
          FIELD_TF (entry_name, 32, 2);
          FIELD_RS (used, 0);

          //TODO RD elevation 30, 2RD base_pt 10: 24
          FIELD_RC (block_scaling, 0);
          FIELD_CAST (num_owned, RS, BL, 0);
          FIELD_RC (flag2, 0);
          FIELD_CAST (num_inserts, RS, RL, 0);
          FIELD_RS (flag3, 0);
          CHK_ENDPOS;
        }
      break;

    case SECTION_LAYER:
      for (i=0; i < tbl->number; i++)
        {
          PREP_TABLE (LAYER);

          FIELD_CAST (flag, RC, RS, 70); //860
          FIELD_TF (entry_name, 32, 2);
          FIELD_RS (used, 0);

          FIELD_RS (color_rs, 62);   // color, off if negative
          FIELD_RS (linetype_rs, 6); // style
          //FIELD_RS (crc, 0);
          CHK_ENDPOS;
        }
      break;

    // was a text STYLE table, became a STYLE object
    case SECTION_STYLE:
      for (i=0; i < tbl->number; i++)
        {
          PREP_TABLE (STYLE);

          FIELD_RC (flag, 70);
          FIELD_TF (entry_name, 32, 2);
          FIELD_RS (used, 0);

          FIELD_RD (fixed_height, 40); //ok
          FIELD_RD (width_factor, 41);
          FIELD_RD (oblique_ang, 50);
          FIELD_RC (generation, 71);
          FIELD_RD (last_height, 42);
          FIELD_TF (font_name, 64, 3); //8ed
          FIELD_TF (bigfont_name, 64, 4); //92d
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
          FIELD_CAST (VIEWMODE, RS, 4BITS, 71);
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
          FIELD_CAST (VIEWMODE, RS, 4BITS, 71);

          FIELD_2RD (lower_left, 10);
          FIELD_2RD (upper_right, 11);
          FIELD_RC (UCSFOLLOW, 71);
          FIELD_RS (circle_zoom, 72);
          FIELD_RC (FASTZOOM, 73);
          FIELD_RC (UCSICON, 74);
          FIELD_RC (GRIDMODE, 76);
          FIELD_2RD (GRIDUNIT, 15);
          FIELD_CAST (SNAPMODE, RS, B, 70); //75
          FIELD_RC (SNAPSTYLE, 70); //77
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
          //unsigned long off;
          PREP_TABLE (DIMSTYLE); //d1f

          FIELD_RC (flag, 70);
          FIELD_TF (entry_name, 32, 2);
          //off = dat->byte;
          FIELD_RS (used, 0);    //d40
          FIELD_RD (DIMSCALE, 40); //d42
          FIELD_RD (DIMASZ, 41);
          FIELD_RD (DIMEXO, 42);
          FIELD_RD (DIMDLI, 43);
          FIELD_RD (DIMEXE, 44);
          FIELD_RD (DIMRND, 45);
          FIELD_RD (DIMDLE, 46);
          FIELD_RD (DIMTP, 47);
          FIELD_RD (DIMTM, 48); //ok
          FIELD_RD (DIMTXT, 140);
          FIELD_RD (DIMCEN, 141); //ok
          FIELD_RD (DIMTSZ, 142);
          FIELD_RD (DIMALTF, 143);
          FIELD_RD (DIMLFAC, 144);
          FIELD_RD (DIMTVP, 145); //db2
          FIELD_RC (DIMTOL, 71);  //dba
          FIELD_RC (DIMLIM, 72);  //dbb
          FIELD_RC (DIMTIH, 73);
          FIELD_RC (DIMTOH, 74);
          FIELD_RC (DIMSE1, 75);
          FIELD_RC (DIMSE2, 76);
          FIELD_CAST (DIMTAD, RC, RS, 77); //ok
          FIELD_CAST (DIMZIN, RC, BS, 78); //dc1
          FIELD_RC (DIMALT, 170);
          FIELD_CAST (DIMALTD, RC, BS, 171); //ok
          FIELD_RC (DIMTOFL, 172); //ok
          FIELD_RC (DIMSAH, 173);  //ok
          FIELD_RC (DIMTIX, 174);  //ok
          FIELD_RC (DIMSOXD, 175); //ok
          FIELD_TF (DIMPOST, 16, 3); //ok dc8
          FIELD_TF (DIMAPOST, 16, 4); //dd8
          FIELD_TF (DIMBLK_T, 16, 5);  //?? unsupported by ODA
          FIELD_TF (DIMBLK1_T, 16, 6); //?? unsupported by ODA
          FIELD_TF (DIMBLK2_T, 66, 7); //?? unsupported by ODA
          //DEBUG_HERE; //e18
          //dat->byte += 50; //unknown: DIMSHO, DIMASO (global)
          FIELD_RS (DIMCLRD_N, 176); //e4a
          FIELD_RS (DIMCLRE_N, 177);
          FIELD_RS (DIMCLRT_N, 178); //e4e
          FIELD_RC (DIMUPT, 0); //??
          FIELD_RD (DIMTFAC, 146); //e51
          FIELD_RD (DIMGAP, 147);  //e59
          CHK_ENDPOS; //-e63
        }
      break;

    case SECTION_VPORT_ENTITY:
      if (tbl->number) {
        LOG_WARN("VPORT_ENTITY table");
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
  return error;
}

static int
decode_entity_preR13(Bit_Chain* dat, Dwg_Object *obj, Dwg_Object_Entity *ent)
{
  Dwg_Object_Entity *_obj = ent;
  obj->type = bit_read_RC(dat);
  _obj->flag_r11 = bit_read_RC(dat); // dxf 70
  obj->size = bit_read_RS(dat);
  LOG_INFO("\n===========================\n"
           "Entity number: %d, Type: %d, Size: %d/0x%x\n",
           obj->index, obj->type, obj->size, obj->size);
  LOG_TRACE("flag_r11: " FORMAT_RC "\n", _obj->flag_r11);
  FIELD_RS (layer_r11, 8);
  FIELD_RS (opts_r11, 0);
  //LOG_TRACE("Layer: %d, Opts: 0x%x\n", ent->layer_r11, ent->opts_r11)
  if (ent->flag_r11 & 1)
    {
      FIELD_RC (color_r11, 0);
    }
  if (ent->flag_r11 & 0x40)
    {
      FIELD_RC (extra_r11, 0);
    }
  /* Common entity preR13 header: */
  if (ent->extra_r11 & 2)
    {
      int error = dwg_decode_eed(dat, (Dwg_Object_Object *)ent);
      if (error & (DWG_ERR_INVALIDEED|DWG_ERR_VALUEOUTOFBOUNDS))
        return error;
    }
  if (FIELD_VALUE(flag_r11) & 2)
     FIELD_RS (kind_r11, 0);

  return 0;
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
  LOG_TRACE("@0x%lx\n", dat->byte); //0x14

  // tables really
  dwg->header.num_sections = 12;
  dwg->header.section = (Dwg_Section*) calloc(1, sizeof(Dwg_Section)
      * dwg->header.num_sections);
  if (!dwg->header.section)
    {
      LOG_ERROR("Out of memory");
      return DWG_ERR_OUTOFMEM;
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
  LOG_TRACE("?long+short: 0x%x 0x%x\n", rl1, (unsigned)rs2);
  decode_preR13_section_ptr("DIMSTYLE", SECTION_DIMSTYLE, dat, dwg);
  // skip: 0x69f - dat->bytes
  dat->byte = 0x69f;
  decode_preR13_section_ptr("VPORT_ENTITY", SECTION_VPORT_ENTITY, dat, dwg);
  dat->byte += 38;
  // entities
  decode_preR13_entities(entities_start, entities_end, 0, dat, dwg);
  dat->byte += 19; /* crc + sentinel? */
  decode_preR13_section(SECTION_BLOCK, dat, dwg);
  decode_preR13_section(SECTION_LAYER, dat, dwg);
  decode_preR13_section(SECTION_STYLE, dat, dwg);
  decode_preR13_section(SECTION_LTYPE, dat, dwg);
  decode_preR13_section(SECTION_VIEW, dat, dwg);
  decode_preR13_section(SECTION_UCS, dat, dwg);
  decode_preR13_section(SECTION_VPORT, dat, dwg);
  decode_preR13_section(SECTION_APPID, dat, dwg);
  decode_preR13_section(SECTION_DIMSTYLE, dat, dwg);
  decode_preR13_section(SECTION_VPORT_ENTITY, dat, dwg);
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
  LOG_TRACE("@0x%lx\n", dat->byte);
  rl1 = bit_read_RL(dat);
  rl2 = bit_read_RL(dat);
  LOG_TRACE("?2long: 0x%x 0x%x\n", rl1, rl2);
  rl1 = bit_read_RL(dat);
  LOG_TRACE("?1long: 0x%x\n", rl1);

  dat->byte = blocks_end + 36 + 4*4 + 12;
  LOG_TRACE("@0x%lx\n", dat->byte);
  decode_preR13_section_chk(SECTION_BLOCK, dat, dwg);
  decode_preR13_section_chk(SECTION_LAYER, dat, dwg);
  decode_preR13_section_chk(SECTION_STYLE, dat, dwg);
  decode_preR13_section_chk(SECTION_LTYPE, dat, dwg);
  decode_preR13_section_chk(SECTION_VIEW, dat, dwg);
  decode_preR13_section_chk(SECTION_UCS, dat, dwg);
  decode_preR13_section_chk(SECTION_VPORT, dat, dwg);
  decode_preR13_section_chk(SECTION_APPID, dat, dwg);
  decode_preR13_section_chk(SECTION_DIMSTYLE, dat, dwg);
  decode_preR13_section_chk(SECTION_VPORT_ENTITY, dat, dwg);
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
  BITCODE_BL j, k;
  int error = 0;

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
      return DWG_ERR_OUTOFMEM;
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
  ckr = bit_calc_CRC(0xC0C1, dat->chain, dat->byte);
  ckr2 = bit_read_RS(dat);
  LOG_TRACE("crc:  %X [RS]\n", ckr2);
  if (ckr != ckr2)
    {
      LOG_ERROR("Header CRC mismatch %X <=> %X", ckr, ckr2);
      //if (dwg->header.version != R_2000)
      //  return DWG_ERR_WRONGCRC;
      /* The CRC depends on num_sections. XOR result with
         3: 0xa598
         4: 0x8101
         5: 0x3cc4
         6: 0x8461
      */
      error |= DWG_ERR_WRONGCRC;
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
          dwg->picture.chain = (unsigned char *) calloc(dwg->picture.size, 1);
          if (!dwg->picture.chain)
            {
              LOG_ERROR("Out of memory");
              return DWG_ERR_OUTOFMEM;
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
  dwg->header_vars.size = bit_read_RL(dat);
  LOG_TRACE("         Length: " FORMAT_RL "\n", dwg->header_vars.size)
  dat->bit = 0;

  dwg_decode_header_variables(dat, dat, dat, dwg);

  // Check CRC-on
  dat->bit = 0;
  ckr = dwg->header_vars.crc;
  pvz = dwg->header.section[SECTION_HEADER_R13].address + 16;
  LOG_TRACE("HEADER_R13.address 0x%lx\n", pvz);
  LOG_TRACE("HEADER_R13.size %d\n", dwg->header.section[SECTION_HEADER_R13].size);
  // typical sizes: 400-599
  if (dwg->header.section[SECTION_HEADER_R13].size < 0xfff &&
      pvz < dat->byte &&
      pvz + dwg->header.section[SECTION_HEADER_R13].size < dat->size)
    {
      ckr2 = bit_calc_CRC(0xC0C1, &(dat->chain[pvz]),
                      dwg->header.section[SECTION_HEADER_R13].size - 34);
    }
  if (ckr != ckr2 && dat->version != R_2000)
    {
      // Typically with r2000: EFA0 <=> ADEF
      LOG_WARN("Section[%ld] CRC mismatch %X <=> %X",
               dwg->header.section[SECTION_HEADER_R13].number, ckr, ckr2);
      // TODO: xor with num_sections
      if (dwg->header.version != R_2000)
        error |= DWG_ERR_WRONGCRC;
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
      BITCODE_BS i;
      Dwg_Class *klass;

      i = dwg->num_classes;
      if (i == 0)
        dwg->dwg_class = malloc(sizeof(Dwg_Class));
      else
        dwg->dwg_class = realloc(dwg->dwg_class, (i + 1) * sizeof(Dwg_Class));
      if (!dwg->dwg_class)
        {
          LOG_ERROR("Out of memory");
          return DWG_ERR_OUTOFMEM;
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
                " %s \"%s\" %d 0x%x\n",
                klass->number, klass->proxyflag, klass->dxfname,
                klass->cppname, klass->appname,
                klass->wasazombie, klass->item_class_id)

      SINCE(R_2007)
        {
          klass->num_instances = bit_read_BL(dat);
          klass->dwg_version = bit_read_BL(dat);
          klass->maint_version = bit_read_BL(dat);
          klass->unknown_1 = bit_read_BL(dat);
          klass->unknown_2 = bit_read_BL(dat);
          LOG_TRACE(" num_instances: %d, dwg/maint version: %d/%d, unk: %d/%d\n",
                    klass->num_instances, klass->dwg_version, klass->maint_version,
                    klass->unknown_1, klass->unknown_2);
        }

      if (strcmp((const char *)klass->dxfname, "LAYOUT") == 0)
        dwg->layout_number = klass->number;

      dwg->num_classes++;
      if (dwg->num_classes > 500)
        {
          LOG_ERROR("number of classes is greater than 500");
          break;
        }
    }
  while (dat->byte < (lasta - 1));

  // Check CRC-on
  dat->byte = dwg->header.section[SECTION_CLASSES_R13].address
              + dwg->header.section[SECTION_CLASSES_R13].size - 18;
  dat->bit = 0;
  ckr = bit_read_RS(dat);
  LOG_TRACE("crc:  %X [RS]\n", ckr);
  pvz = dwg->header.section[SECTION_CLASSES_R13].address + 16;
  if (dwg->header.section[SECTION_CLASSES_R13].size < 0xfff &&
      pvz < dat->byte &&
      pvz + dwg->header.section[SECTION_CLASSES_R13].size < dat->size)
    {
      ckr2 = bit_calc_CRC(0xC0C1, &(dat->chain[pvz]),
                      dwg->header.section[SECTION_CLASSES_R13].size - 34);
    }
  if (ckr != ckr2 && dwg->header.version != R_2000)
    {
      LOG_ERROR("Section[%ld] CRC mismatch %X <=> %X",
                dwg->header.section[SECTION_CLASSES_R13].number, ckr, ckr2);
      //if (dwg->header.version != R_2000)
      //  return DWG_ERR_WRONGCRC;
      error |= DWG_ERR_WRONGCRC;
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
      int added;
      startpos = dat->byte;

      section_size = bit_read_RS_LE(dat);
      LOG_TRACE("\nSection size: %u\n", section_size);
      if (section_size > 2040)
        {
          LOG_ERROR("Object-map section size greater than 2040!")
          return DWG_ERR_VALUEOUTOFBOUNDS;
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
          LOG_TRACE("\nNext object: %lu\t", (unsigned long)dwg->num_objects)
          LOG_TRACE("Handle: %li\tOffset: %ld @%lu\n", handle, offset, last_offset)

          if (dat->byte == oldpos)
            break;

          if (object_end < last_offset)
            object_end = last_offset;
          if (object_begin > last_offset)
            object_begin = last_offset;

          added = dwg_decode_add_object(dwg, dat, dat, last_offset);
          if (added > 0)
            error |= added; // else not added (skipped) or -1 for re-allocated
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

      ckr = bit_read_RS(dat);
      LOG_TRACE("crc:  %X [RS]\n", ckr);
      ckr2 = bit_calc_CRC(0xC0C1, dat->chain + startpos, section_size);
      if (ckr != ckr2)
        {
          LOG_ERROR("Section CRC mismatch %X <=> %X", ckr, ckr2);
          // fails with r14
          //if (dwg->header.version == R_2000)
          //  return DWG_ERR_WRONGCRC;
          if (dat->version != R_14)
            error |= DWG_ERR_WRONGCRC;
        }

      if (dat->byte >= lastmap)
        break;
    }
  while (section_size > 2);

  LOG_INFO("Num objects: %lu\n", (unsigned long)dwg->num_objects)
  LOG_INFO("\n"
           "=======> Object Data 2 (start)  : %8lX\n",
           (unsigned long) object_begin)
  dat->byte = object_end;
  object_begin = bit_read_MS(dat);
  LOG_INFO("         Object Data 2 (end)    : %8lX (%8lX)\n",
           (unsigned long) (object_end + object_begin+ 2), object_begin)

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
      BITCODE_BL vcount;
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
      //DEBUG_HERE;
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

        FIELD_BS (num_handlers, 0); // 14, resp. 16 in r14
        if (FIELD_VALUE(num_handlers) > 16) {
          LOG_ERROR("Second header num_handlers > 16: %d\n",
                    FIELD_VALUE(num_handlers));
          FIELD_VALUE(num_handlers) = 14;
        }
        for (i = 0; i < FIELD_VALUE(num_handlers); i++)
          {
            FIELD_RC (handlers[i].size, 0);
            FIELD_RC (handlers[i].nr, 0);
            FIELD_VECTOR (handlers[i].data, RC, handlers[i].size, 0);
          }

        // Check CRC-on
        ckr = bit_read_CRC(dat);

        VERSION(R_14) {
          FIELD_RL (junk_r14_1, 0);
          FIELD_RL (junk_r14_2, 0);
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
      dwg->measurement = bit_read_RL(dat); // TODO => header_vars.MEASUREMENT

      LOG_TRACE("         Size bytes :\t%lu\n", dat->size)
    }

  //step II of handles parsing: resolve pointers from handle value
  //XXX: move this somewhere else
  LOG_TRACE("\nResolving pointers from ObjectRef vector.\n")
  error |= resolve_objectref_vector(dat, dwg);
  return error;
}

static int
resolve_objectref_vector(Bit_Chain* dat, Dwg_Data * dwg)
{
  BITCODE_BL i;
  Dwg_Object * obj;

  for (i = 0; i < dwg->num_object_refs; i++)
    {
      Dwg_Object_Ref *ref = dwg->object_ref[i];
      LOG_INSANE("==========\n")
        LOG_TRACE("-objref[%3ld]: HANDLE(%d.%d.%lX) Absolute:%lX\n", (long)i,
                ref->handleref.code, ref->handleref.size,
                ref->handleref.value,
                ref->absolute_ref)

      // search the handle in all objects
      obj = dwg_resolve_handle(dwg, ref->absolute_ref);
      if (obj)
        {
          LOG_TRACE("-found:     HANDLE(%d.%d.%lX) => [%u]\n",
                    obj->handle.code, obj->handle.size,
                    obj->handle.value, obj->index)
        }
      //assign found pointer to objectref vector
      ref->obj = obj;

      if (DWG_LOGLEVEL >= DWG_LOGLEVEL_INSANE)
        {
          if (obj)
            dwg_print_object(dat, obj);
          else
            LOG_TRACE("Null object pointer: object_ref[%ld]\n", (long)i)
        }
    }
  return dwg->num_object_refs ? 0 : DWG_ERR_VALUEOUTOFBOUNDS;
}

void
dwg_resolve_objectrefs_silent(Dwg_Data *restrict dwg)
{
  BITCODE_BL i;
  Dwg_Object *restrict obj;
  int oldloglevel = loglevel;

  loglevel = 0;
  // Dwg_Object_Ref->obj are stored all over. dirty it to update dynamically.
  dwg->dirty_refs = 1; // TODO: this is now forever. find a way to resolve all objs also.
  for (i = 0; i < dwg->num_object_refs; i++)
    {
      //scan num_objects for the id (absolute_ref)
      obj = dwg_resolve_handle(dwg, dwg->object_ref[i]->absolute_ref);
      dwg->object_ref[i]->obj = obj;
    }
  //TODO: scan dwg->num_objects also to update it's handlerefs
  loglevel = oldloglevel;
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
read_two_byte_offset(Bit_Chain *restrict dat, int *restrict lit_length)
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
decompress_R2004_section(Bit_Chain *restrict dat, char *restrict decomp,
                         uint32_t decomp_data_size, uint32_t comp_data_size)
{
  int lit_length, i;
  uint32_t comp_offset, comp_bytes, bytes_left;
  unsigned char opcode1 = 0, opcode2;
  long unsigned int start_byte = dat->byte;
  char *src, *dst = decomp;

  bytes_left = decomp_data_size; //to write to
  if (comp_data_size > dat->size - start_byte) // bytes left to read from
    {
      LOG_WARN("Invalid comp_data_size %lu > %lu bytes left",
                (unsigned long)bytes_left, dat->size - dat->byte)
      return DWG_ERR_VALUEOUTOFBOUNDS;
    }
  // length of the first sequence of uncompressed or literal data.
  lit_length = read_literal_length(dat, &opcode1);
  bit_read_fixed(dat, decomp, lit_length);
  dst += lit_length;
  bytes_left -= lit_length;

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
      else if (opcode1 >= 0x21 && opcode1 <= 0x3F) // lgtm [cpp/constant-comparison]
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
        {
          LOG_ERROR("Invalid opcode 0x%x in input stream at pos %lu",
                    opcode1, dat->byte);
          return DWG_ERR_INTERNALERROR;  // error in input stream
        }

      LOG_INSANE("<C %d\n", comp_bytes)
      // copy "compressed data"
      src = dst - comp_offset - 1;
      assert(src >= decomp);
      if ((uint32_t)comp_bytes > bytes_left) // bytes left to write
        {
          LOG_ERROR("Invalid comp_bytes %lu > %lu bytes left",
                    (unsigned long)comp_bytes, (unsigned long)bytes_left)
          return DWG_ERR_VALUEOUTOFBOUNDS;
        }
      for (i = 0; (uint32_t)i < comp_bytes; ++i)
        *dst++ = *src++;
      bytes_left -= comp_bytes;

      // copy "literal data"
      LOG_INSANE("<L %d\n", lit_length)
      if ((uint32_t)lit_length > bytes_left) // bytes left to write
        {
          LOG_ERROR("Invalid lit_length %lu > %lu bytes left",
                    (unsigned long)lit_length, (unsigned long)bytes_left)
          return DWG_ERR_VALUEOUTOFBOUNDS;
        }
      for (i = 0; i < lit_length; ++i)
        *dst++ = bit_read_RC(dat);
    }

  return 0;  // Success
}

/* Read R2004, 2010+ Section Map
 * The Section Map is a vector of number, size, and address triples used
 * to locate the sections in the file.
 */
static int
read_R2004_section_map(Bit_Chain* dat, Dwg_Data * dwg)
{
  char *decomp, *ptr;
  int i, error;
  int section_address;
  int bytes_remaining;
  uint32_t comp_data_size   = dwg->r2004_header.comp_data_size;
  uint32_t decomp_data_size = dwg->r2004_header.decomp_data_size;

  dwg->header.num_sections = 0;
  dwg->header.section = 0;

  // decompressed data
  decomp = (char *)calloc(decomp_data_size+1024, sizeof(char));
  if (!decomp)
    {
      LOG_ERROR("Out of memory");
      return DWG_ERR_OUTOFMEM;
    }

  error = decompress_R2004_section(dat, decomp, decomp_data_size+1024, comp_data_size);
  if (error > DWG_ERR_CRITICAL)
    return error;
  LOG_TRACE("\n#### Read 2004 Section Page Map ####\n")

  section_address = 0x100;  // starting address
  i = 0;
  bytes_remaining = decomp_data_size;
  ptr = decomp;
  dwg->header.num_sections = 0;

  while (bytes_remaining)
    {
      if (dwg->header.num_sections == 0)
        dwg->header.section = calloc(1, sizeof(Dwg_Section));
      else
        dwg->header.section = realloc(dwg->header.section,
                       sizeof(Dwg_Section) * (dwg->header.num_sections+1));
      if (!dwg->header.section)
        {
          LOG_ERROR("Out of memory");
          return DWG_ERR_OUTOFMEM;
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
  return 0;
}

// index is the Section Number in the section map
static Dwg_Section*
find_section(Dwg_Data *dwg, unsigned long int index)
{
  BITCODE_BL i;
  if (dwg->header.section == 0 || index == 0)
    return 0;
  for (i = 0; i < dwg->header.num_sections; ++i)
    {
      if ((unsigned long int)dwg->header.section[i].number == index)
        return &dwg->header.section[i];
    }
  return NULL;
}

/* Read R2004, 2010+ Section Info
 */
static int
read_R2004_section_info(Bit_Chain* dat, Dwg_Data *dwg,
                        uint32_t comp_data_size,
                        uint32_t decomp_data_size)
{
  char *decomp, *ptr;
  BITCODE_BL i, j;
  BITCODE_BL section_number = 0;
  uint32_t data_size;
  uint64_t start_offset;
  int error;

  decomp = (char *)calloc(decomp_data_size+1024, 1);
  if (!decomp)
    {
      LOG_ERROR("Out of memory");
      return DWG_ERR_OUTOFMEM;
    }

  error = decompress_R2004_section(dat, decomp, decomp_data_size+1024, comp_data_size);
  if (error > DWG_ERR_CRITICAL)
    return error;

  dwg->header.num_infos = *(uint32_t*)decomp;
  dwg->header.section_info = (Dwg_Section_Info*)
    calloc(dwg->header.num_infos, sizeof(Dwg_Section_Info));
  if (!dwg->header.section_info)
    {
      LOG_ERROR("Out of memory");
      return error | DWG_ERR_OUTOFMEM;
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
      if (info->size == section_number+1 &&
          info->num_sections > 100000)
        {
	  LOG_WARN("Oops, Section[%d] => Section_Info %d", i, info->size);
          data_size = info->pagecount;
          section_number++;
          info = &dwg->header.section_info[i-1];
          goto next_section;
        }
      info->max_decomp_size = *((int32_t*)ptr + 3);
      info->unknown2        = *((int32_t*)ptr + 4);
      info->compressed      = *((int32_t*)ptr + 5);
      info->type            = *((int32_t*)ptr + 6);
      info->encrypted       = *((int32_t*)ptr + 7);
      ptr += 32;
      memcpy(info->name, ptr, 64);
      ptr += 64;

      LOG_TRACE("\nSection Info[%d] description fields\n", i)
      LOG_TRACE("Size:            %u\n", info->size)
      LOG_TRACE("PageCount:       %u\n", info->pagecount)
      LOG_TRACE("Num sections:    %u\n", info->num_sections)
      LOG_TRACE("Max decomp size: %u / 0x%x\n", // normally 0x7400
                info->max_decomp_size, info->max_decomp_size)
      LOG_TRACE("Unknown:         %u\n", info->unknown2)
      LOG_TRACE("Compressed:      %u (1=no, 2=yes)\n", info->compressed)
      LOG_TRACE("Section Type:  0x%x\n", (unsigned)info->type)
      LOG_TRACE("Encrypted:       %d (0=no, 1=yes, 2=unknown)\n", info->encrypted)
      LOG_TRACE("SectionName:     %s\n\n", info->name)

      if (info->num_sections < 100000)
	{
	  LOG_INFO("Section count %u in area %d\n", info->num_sections, i);
          info->sections = calloc(info->num_sections, sizeof(Dwg_Section*));
          if (!info->sections)
            {
              LOG_ERROR("Out of memory with %u sections", info->num_sections);
              return error | DWG_ERR_OUTOFMEM;
            }

	  for (j = 0; j < info->num_sections; j++)
	    {
	      section_number = *((uint32_t*)ptr);       // Index into SectionMap
	      data_size      = *((uint32_t*)ptr + 1);
	      //start_offset   = *((uint64_t*)ptr + 1); // avoid alignment ubsan
	      start_offset   = *((uint32_t*)ptr + 2);
              start_offset <<= 32;
              start_offset  += *((uint32_t*)ptr + 3);
	      ptr += 16;

	      info->sections[j] = find_section(dwg, section_number);

              if (section_number > info->num_sections + info->sections[0]->number)
                {
                  LOG_TRACE("Strange Section Number: 0x%lx\n",
                            (unsigned long)section_number)
                }
              else
                {
                  LOG_TRACE("Section Number: %lu\n", (unsigned long)section_number)
                }
              LOG_TRACE("Data size:      %d\n", data_size) //compressed
	      LOG_TRACE("Start offset:   0x%" PRIx64 "\n", start_offset)
	    }
	}// sanity check
      else if (info->size == section_number+1)
        {
          Dwg_Section* sec;
          // oops, this is really another info
	  LOG_WARN("Oops2, Section[%d] => Section_Info %d", i, info->size);
          section_number = info->size;      // Index into SectionMap
          data_size      = info->pagecount;
        next_section:
          start_offset   = *((uint32_t*)ptr + 2);
          start_offset <<= 32;
          start_offset  += *((uint32_t*)ptr + 3);
          ptr -= 32;
          ptr -= 64;
          ptr += 16;

          sec = find_section(dwg, section_number);
          if (section_number < dwg->header.num_infos)
            info->sections[section_number] = sec;

          LOG_TRACE("Section Number: %d\n", section_number)
          LOG_TRACE("Data size:      %d\n", data_size) //compressed
          LOG_TRACE("Start offset:   0x%" PRIx64 "\n", start_offset);
          free (decomp);
          return error | DWG_ERR_VALUEOUTOFBOUNDS;
        }
      else
	{
	  LOG_ERROR("Section count %u in area %d too high! Skipping",
                    info->num_sections, i);
          info->num_sections = 0;
          free (decomp);
          return error | DWG_ERR_VALUEOUTOFBOUNDS;
	}
    }
  free (decomp);
  return error;
}

/* Encrypted Section Header */
#pragma pack(1)
typedef union _encrypted_section_header
{
  uint32_t long_data[8];
  unsigned char char_data[32];
  struct
  {
    uint32_t tag;
    uint32_t section_type;
    uint32_t data_size;
    uint32_t section_size;
    uint32_t start_offset;
    uint32_t unknown;
    uint32_t checksum_1;
    uint32_t checksum_2;
  } fields;
} encrypted_section_header;

static int
read_2004_compressed_section(Bit_Chain* dat, Dwg_Data *dwg,
                            Bit_Chain* sec_dat, BITCODE_RL section_type)
{
  uint32_t address, sec_mask;
  uint32_t max_decomp_size;
  Dwg_Section_Info *info = NULL;
  encrypted_section_header es;
  char *decomp;
  BITCODE_BL i, j;
  int error = 0;

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
      LOG_WARN("Failed to find section_info with type 0x%x", section_type);
      return DWG_ERR_SECTIONNOTFOUND;
    }
  else
    {
      LOG_TRACE("\nFound section_info %s type 0x%x with %d sections\n",
                info->name, section_type, info->num_sections);
    }

  max_decomp_size = info->num_sections * info->max_decomp_size;
  decomp = (char *)calloc(max_decomp_size, sizeof(char));
  if (!decomp)
    {
      LOG_ERROR("Out of memory with %u sections", info->num_sections);
      return DWG_ERR_OUTOFMEM;
    }

  for (i=0; i < info->num_sections; ++i)
    {
      if (!info->sections[i])
        {
          LOG_WARN("Skip empty class section %d", i);
          continue;
        }
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
      LOG_TRACE("StartOffset:      0x%x\n",
              (unsigned int) es.fields.start_offset)
      LOG_HANDLE("Unknown:          0x%x\n",
              (unsigned int) es.fields.unknown);
      LOG_HANDLE("Checksum1:        0x%x\n",
            (unsigned int) es.fields.checksum_1)
      LOG_HANDLE("Checksum2:        0x%x\n\n",
            (unsigned int) es.fields.checksum_2)

      error = decompress_R2004_section
        (dat, &decomp[i * info->max_decomp_size],     //offset
         max_decomp_size - (i*info->max_decomp_size), //bytes left
         es.fields.data_size);
      if (error > DWG_ERR_CRITICAL) {
        free(decomp);
        decomp = NULL;
        return error;
      }
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
  BITCODE_RL size;
  BITCODE_BS max_num, i;
  unsigned long int num_objects, dwg_version, maint_version, unknown;
  char c;
  int error;
  Bit_Chain sec_dat = {0}, str_dat = {0};

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
      BITCODE_RL bitsize = 0;
      LOG_TRACE("\nClasses\n-------------------\n")
      size    = bit_read_RL(&sec_dat);  // size of class data area
      LOG_TRACE("size: " FORMAT_RL " [RL]\n", size)
      if ((dat->version >= R_2010 && dwg->header.maint_version > 3)
          || dat->version >= R_2018)
        {
          BITCODE_RL hsize = bit_read_RL(&sec_dat);
          LOG_TRACE("hsize: " FORMAT_RL " [RL]\n", hsize)
        }
      if (dat->version >= R_2007)
        {
          bitsize = bit_read_RL(&sec_dat);
          LOG_TRACE("bitsize: " FORMAT_RL " [RL]\n", bitsize)
        }
      max_num = bit_read_BS(&sec_dat);  // Maximum class number
      LOG_TRACE("max_num: " FORMAT_BS " [BS]\n", max_num)
      c = bit_read_RC(&sec_dat);        // 0x00
      LOG_HANDLE("c: " FORMAT_RC " [RC]\n", c)
      c = bit_read_RC(&sec_dat);        // 0x00
      LOG_HANDLE("c: " FORMAT_RC " [RC]\n", c)
      c = bit_read_B(&sec_dat);         // 1
      LOG_HANDLE("c: " FORMAT_B " [B]\n", c)

      dwg->layout_number = 0;
      dwg->num_classes = max_num - 499;
      if (max_num < 500 || max_num > 5000)
        {
          LOG_ERROR("Invalid max class number %d", max_num)
          return DWG_ERR_VALUEOUTOFBOUNDS;
        }
      assert(max_num >= 500);
      assert(max_num < 5000);

      if (dat->version >= R_2007)
        section_string_stream(&sec_dat, bitsize, &str_dat);

      dwg->dwg_class = (Dwg_Class *)calloc(dwg->num_classes, sizeof(Dwg_Class));
      if (!dwg->dwg_class)
        {
          LOG_ERROR("Out of memory");
          if (sec_dat.chain)
            free(sec_dat.chain);
          return DWG_ERR_OUTOFMEM;
        }

      for (i = 0; i < dwg->num_classes; i++)
        {
          dwg->dwg_class[i].number        = bit_read_BS(&sec_dat);
          dwg->dwg_class[i].proxyflag     = bit_read_BS(&sec_dat);
          LOG_TRACE("-------------------\n")
          LOG_TRACE("Number:           %d\n", dwg->dwg_class[i].number)
          LOG_TRACE("Proxyflag:        %x\n", dwg->dwg_class[i].proxyflag)
          if (dwg->header.version >= R_2007)
            {
              dwg->dwg_class[i].appname       = (char*)bit_read_TU(&str_dat);
              dwg->dwg_class[i].cppname       = (char*)bit_read_TU(&str_dat);
              dwg->dwg_class[i].dxfname_u     = bit_read_TU(&str_dat);
              LOG_TRACE_TU("Application name", dwg->dwg_class[i].appname,0)
              LOG_TRACE_TU("C++ class name  ", dwg->dwg_class[i].cppname,0)
              LOG_TRACE_TU("DXF record name ", dwg->dwg_class[i].dxfname_u,0)
              dwg->dwg_class[i].dxfname = bit_convert_TU(dwg->dwg_class[i].dxfname_u);
            }
          else
            {
              dwg->dwg_class[i].appname       = bit_read_TV(&sec_dat);
              dwg->dwg_class[i].cppname       = bit_read_TV(&sec_dat);
              dwg->dwg_class[i].dxfname       = bit_read_TV(&sec_dat);
              LOG_TRACE("Application name: %s\n", dwg->dwg_class[i].appname)
              LOG_TRACE("C++ class name:   %s\n", dwg->dwg_class[i].cppname)
              LOG_TRACE("DXF record name:  %s\n", dwg->dwg_class[i].dxfname)
            }
          dwg->dwg_class[i].wasazombie    = bit_read_B(&sec_dat);
          dwg->dwg_class[i].item_class_id = bit_read_BS(&sec_dat);
          LOG_TRACE("Class ID:         0x%x "
                    "(0x1f3 for object, 0x1f2 for entity)\n",
                    dwg->dwg_class[i].item_class_id)

          dwg->dwg_class[i].num_instances = bit_read_BL(&sec_dat);
          dwg->dwg_class[i].dwg_version    = bit_read_BS(&sec_dat);
          dwg->dwg_class[i].maint_version  = bit_read_BS(&sec_dat);
          dwg->dwg_class[i].unknown_1      = bit_read_BL(&sec_dat);
          dwg->dwg_class[i].unknown_1      = bit_read_BL(&sec_dat);
          LOG_TRACE( "num_instances:    %u\n",
                    dwg->dwg_class[i].num_instances)
          LOG_HANDLE("dwg version:      %u (%u)\n",
                    dwg->dwg_class[i].dwg_version,
                    dwg->dwg_class[i].maint_version)
          LOG_HANDLE("unknown:          %u %u\n", dwg->dwg_class[i].unknown_1,
                    dwg->dwg_class[i].unknown_2)

          if (strcmp(dwg->dwg_class[i].dxfname, "LAYOUT") == 0)
            dwg->layout_number = dwg->dwg_class[i].number;
        }
    }
  else
    {
      LOG_ERROR("Failed to find class section sentinel");
      free(sec_dat.chain);
      return DWG_ERR_CLASSESNOTFOUND;
    }

  // then RS: CRC
  // dwg_sentinel(DWG_SENTINEL_CLASS_END)
  // SINCE(R_2004) 8 unknown bytes
  
  free(sec_dat.chain);
  return 0;
}

/* R2004, 2010+ Header Section
 */
static int
read_2004_section_header(Bit_Chain* dat, Dwg_Data *dwg)
{
  int error;
  Bit_Chain sec_dat = { 0 };

  error = read_2004_compressed_section(dat, dwg, &sec_dat, SECTION_HEADER);
  if (error)
    return error;

  if (bit_search_sentinel(&sec_dat, dwg_sentinel(DWG_SENTINEL_VARIABLE_BEGIN)))
    {
      LOG_TRACE("\nHeader\n-------------------\n")
      dwg->header_vars.size = bit_read_RL(&sec_dat);
      LOG_TRACE("size: " FORMAT_RL "\n", dwg->header_vars.size);
      PRE (R_2007) {
        error |= dwg_decode_header_variables(&sec_dat, &sec_dat, &sec_dat, dwg);
      } else {
        Bit_Chain hdl_dat = { 0 }, str_dat = { 0 };
        BITCODE_RL endbits = 160; //start bit: 16 sentinel + 4 size
        hdl_dat = sec_dat;
        str_dat = sec_dat;
        if (dwg->header.maint_version > 3 || dat->version >= R_2018)
          {
            dwg->header_vars.bitsize_hi = bit_read_RL(&sec_dat);
            LOG_TRACE("bitsize_hi: " FORMAT_RL " [RL]\n", dwg->header_vars.bitsize_hi)
            endbits += 32; //192
          }
        dwg->header_vars.bitsize = bit_read_RL(&sec_dat);
        LOG_TRACE("bitsize: " FORMAT_RL " [RL]\n", dwg->header_vars.bitsize)
        endbits += dwg->header_vars.bitsize;
        bit_set_position(&hdl_dat, endbits);
        section_string_stream(&sec_dat, dwg->header_vars.bitsize, &str_dat);
        error |= dwg_decode_header_variables(&sec_dat, &hdl_dat, &str_dat, dwg);
      }
    }
  free(sec_dat.chain);
  return error;
}

/* R2004, 2010+ Handles Section
 */
static int
read_2004_section_handles(Bit_Chain* dat, Dwg_Data *dwg)
{
  Bit_Chain obj_dat = { 0 }, hdl_dat = { 0 };
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
      LOG_TRACE("\nSection size: %u\n", section_size);
	  /* *********************************************************************************************
      ** https://www.opendesign.com/files/guestdownloads/OpenDesign_Specification_for_.dwg_files.pdf
      ** page 251 "Note that each section is cut off at a maximum length of 2032."
      ** BUT in fact exist files with 2035 section size */
      if (section_size > 2040)
        {
          LOG_ERROR("Object-map section size greater than 2040!");
          free(obj_dat.chain);
          return DWG_ERR_VALUEOUTOFBOUNDS;
        }

      //last_handle = 0;
      last_offset = 0;
      while (hdl_dat.byte - startpos < section_size)
        {
          int added;
          long handle, offset;
          oldpos = dat->byte;
          handle = bit_read_MC(&hdl_dat);
          offset = bit_read_MC(&hdl_dat);
          //last_handle += handle;
          last_offset += offset;
          LOG_TRACE("\n< Next object: %lu\t", (unsigned long)dwg->num_objects)
          LOG_HANDLE("Handle: %lX\tOffset: %ld @%lu\n", handle, offset, last_offset)

          added = dwg_decode_add_object(dwg, &obj_dat, &obj_dat, last_offset);
          if (added > 0)
            error |= added;
          //else re-allocated
          // we don't stop encoding on single errors, but we sum them all up
          // as combined bitmask
        }

      if (hdl_dat.byte == oldpos)
        break;
      hdl_dat.byte += 2; // CRC

      if (hdl_dat.byte >= endpos)
        break;
    }
  while (section_size > 2);

  LOG_TRACE("\nNum objects: %lu\n", (unsigned long)dwg->num_objects);

  free(hdl_dat.chain);
  free(obj_dat.chain);
  return error;
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
    unsigned int rseed = 1;
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
    LOG_HANDLE("@0x%lx\n", dat->byte);

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

  error |= read_R2004_section_map(dat, dwg);
  if (!dwg->header.section || error >= DWG_ERR_CRITICAL)
    {
      LOG_ERROR("Failed to read R2004 Section Page Map.")
      return error | DWG_ERR_INTERNALERROR;
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
      error |= read_R2004_section_info(dat, dwg,
                 _obj->comp_data_size, _obj->decomp_data_size);
    }
  else
    error |= DWG_ERR_SECTIONNOTFOUND;

  error |= read_2004_section_classes(dat, dwg);
  error |= read_2004_section_header(dat, dwg);
  error |= read_2004_section_handles(dat, dwg);

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

  error |= resolve_objectref_vector(dat, dwg);
  return error;
}

static int
decode_R2007(Bit_Chain* dat, Dwg_Data * dwg)
{
  Bit_Chain hdl_dat = { 0 };
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
  if (error >= DWG_ERR_CRITICAL)
    {
      LOG_ERROR("Failed to read 2007 meta data")
      return error;
    }

  LOG_INFO("Num objects: %lu\n", (unsigned long)dwg->num_objects)
  LOG_TRACE("  num object_refs: %lu\n", (unsigned long)dwg->num_object_refs)
  return error | resolve_objectref_vector(dat, dwg);
}

/*--------------------------------------------------------------------------------
 * Private functions
 */

static int
dwg_decode_eed_data(Bit_Chain * dat, Dwg_Eed_Data* data, unsigned long int end)
{
  int lenc;
  BITCODE_BS j;
  BITCODE_RS lens;
  long size = MIN(end, dat->size) - dat->byte;

  switch (data->code)
    {
    case 0:
      PRE(R_2007) {
        data->u.eed_0.length = lenc = bit_read_RC(dat);
        data->u.eed_0.codepage = bit_read_RS_LE(dat);
        if ((long)lenc > size-4)
          {
            LOG_ERROR("Invalid EED string len %d, max %ld", lenc, size-4)
            dat->byte = end;
            break;
#if 0

            obj->num_eed = 0;
            if (obj->eed[idx].size)
              free(obj->eed[idx].raw);
            free(obj->eed[idx].data);
            free(obj->eed);
            obj->eed = NULL;
            dat->byte = end;
            return DWG_ERR_VALUEOUTOFBOUNDS; /* may not continue */
#endif
          }
        /* code:1 + len:1 + cp:2 */
        bit_read_fixed(dat, data->u.eed_0.string, lenc);
        data->u.eed_0.string[lenc] = '\0';
        LOG_TRACE("string: \"%s\" len=%d cp=%d\n",
                  data->u.eed_0.string, (int)lenc,
                  (int)data->u.eed_0.codepage);
      } LATER_VERSIONS {
        data->u.eed_0_r2007.length = lens = bit_read_RS(dat);
        /* code:1 + len:2 NUL? */
        for (j=0; j < MIN(lens,(size-3)/2); j++)
          data->u.eed_0_r2007.string[j] = bit_read_RS_LE(dat);
        //data->u.eed_0_r2007.string[j] = 0; //already calloc'ed
#ifdef _WIN32
          LOG_TRACE("string: \"" FORMAT_TU "\" len=%d\n",
                    data->u.eed_0_r2007.string, (int)lens);
#endif
        }
        break;
      case 2:
        data->u.eed_2.byte = bit_read_RC(dat);
        LOG_TRACE("byte: " FORMAT_RC "\n", data->u.eed_2.byte);
        break;
      case 3:
      case 5:
        data->u.eed_3.layer = bit_read_RL(dat);
        LOG_TRACE("layer/...: " FORMAT_RL "\n", data->u.eed_3.layer);
        break;
      case 4:
        data->u.eed_4.length = lenc = bit_read_RC(dat);
        /* code:1 + len:1 */
        for (j=0; j < MIN(lenc,size-2); j++)
          data->u.eed_4.data[j] = bit_read_RC(dat);
        LOG_TRACE("raw: %s\n", data->u.eed_4.data);
        break;
      case 10: case 11: case 12: case 13: /*case 14: case 15:*/
        data->u.eed_10.point.x = bit_read_RD(dat);
        data->u.eed_10.point.y = bit_read_RD(dat);
        data->u.eed_10.point.z = bit_read_RD(dat);
        LOG_TRACE("3dpoint: %f, %f, %f\n",
                  data->u.eed_10.point.x,
                  data->u.eed_10.point.y,
                  data->u.eed_10.point.z);
        break;
      case 40: case 41: case 42: /*case 43: case 44: case 45: case 46:
      case 51: case 54:*/
        data->u.eed_40.real = bit_read_RD(dat);
        LOG_TRACE("real: %f\n", data->u.eed_40.real);
        break;
      case 70:
        data->u.eed_70.rs = bit_read_RS(dat);
        LOG_TRACE("short: " FORMAT_RS "\n", data->u.eed_70.rs);
        break;
      case 71:
        data->u.eed_71.rl = bit_read_RL(dat);
        LOG_TRACE("long: " FORMAT_RL "\n", data->u.eed_71.rl);
        break;
      default:
        LOG_ERROR("unknown EED code %d", data->code);
        dat->byte = end;
        return DWG_ERR_INVALIDTYPE; /* may continue */
      }

  return 0;
}

/* for objects and entities */
static int
dwg_decode_eed(Bit_Chain * dat, Dwg_Object_Object * obj)
{
  BITCODE_BS size;
  unsigned int idx = 0;
  int error = 0;
  Dwg_Data *dwg = obj->dwg;
  if (!dwg)
    return DWG_ERR_INVALIDEED;

  obj->num_eed = 0;
  while ((size = bit_read_BS(dat)))
    {
      int i;
      BITCODE_BS j;
      BITCODE_RC code = 0;
      long unsigned int end, offset;
      long unsigned int sav_byte;
      Dwg_Object *_obj = &dwg->object[obj->objid];

      LOG_TRACE("EED[%u] size: " FORMAT_BS "\n", idx, size);
      if (size > _obj->size)
        {
          LOG_ERROR("Invalid EED size " FORMAT_BS " > %u", size, _obj->size)
          obj->num_eed = 0;
          return DWG_ERR_VALUEOUTOFBOUNDS; /* may not continue */
        }

      if (idx) {
        obj->eed = (Dwg_Eed*)realloc(obj->eed, (idx+1) * sizeof(Dwg_Eed));
        memset(&obj->eed[idx], 0, sizeof(Dwg_Eed));
      } else {
        obj->eed = (Dwg_Eed*)calloc(1, sizeof(Dwg_Eed));
      }
      obj->eed[idx].size = size;
      error |= bit_read_H(dat, &obj->eed[idx].handle);
      end = dat->byte + size;
      if (error) {
        LOG_ERROR("No EED[%d].handle", idx);
        obj->num_eed = 0;
        free(obj->eed);
        obj->eed = NULL;
        dat->byte = end; // skip eed
        continue; // continue for size = bit_read_BS(dat)
      } else {
        LOG_TRACE("EED[%u] handle: %d.%d.%lX\n", idx,
                  obj->eed[idx].handle.code, obj->eed[idx].handle.size,
                  obj->eed[idx].handle.value);
        if (_obj->supertype == DWG_SUPERTYPE_OBJECT &&
            _obj->dxfname &&
            !strcmp(_obj->dxfname, "MLEADERSTYLE"))
          { // check for is_new_format: has extended data for APPID “ACAD_MLEADERVER”
            Dwg_Object_Ref ref;
            ref.obj = NULL;
            ref.handleref = obj->eed[idx].handle;
            ref.absolute_ref = 0L;
            if (dwg_resolve_handleref(&ref, _obj))
              {
                Dwg_Object_APPID_CONTROL *appid = &dwg->appid_control;
                // search absref in APPID_CONTROL apps[]
                for (j=0; j < appid->num_entries; j++)
                  {
                    if (appid->apps[j] &&
                        appid->apps[j]->absolute_ref == ref.absolute_ref)
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

      sav_byte = dat->byte;
      obj->eed[idx].raw = bit_read_TF(dat, size);
      LOG_INSANE_TF(obj->eed[idx].raw, size);
      dat->byte = sav_byte;

      while (dat->byte < end)
        {
          obj->eed[idx].data = (Dwg_Eed_Data*)calloc(size + 8, 1);
          obj->eed[idx].data->code = code = bit_read_RC(dat);
          LOG_TRACE("EED[%u] code: %d\n", idx, (int)code);
          LOG_TRACE("EED[%u] ", idx);

          error |= dwg_decode_eed_data(dat, obj->eed[idx].data, end);

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
              LOG_INSANE("EED[%u] size remaining: %ld\n", idx, (long)size);

              obj->eed = (Dwg_Eed*)realloc(obj->eed, (idx+1) * sizeof(Dwg_Eed));
              obj->eed[idx].handle = obj->eed[idx-1].handle;
              obj->eed[idx].size = 0;
              obj->eed[idx].raw = NULL;
            }
          else
            {
              break;
            }
        }
      dat->byte = end;
    }
  return error;
}

// if to check for the has_strings bit after bitsize
static int
obj_has_strings(unsigned int type)
{
  switch (type)
    {
    case DWG_TYPE_TEXT:
    case DWG_TYPE_ATTRIB:
    case DWG_TYPE_ATTDEF:
    case DWG_TYPE_BLOCK:
      return 1;
    case DWG_TYPE_ENDBLK:
    case DWG_TYPE_SEQEND:
    case DWG_TYPE_INSERT:
    case DWG_TYPE_MINSERT:
    case DWG_TYPE_VERTEX_2D:
    case DWG_TYPE_VERTEX_3D:
    case DWG_TYPE_VERTEX_MESH:
    case DWG_TYPE_VERTEX_PFACE:
    case DWG_TYPE_VERTEX_PFACE_FACE:
    case DWG_TYPE_POLYLINE_2D:
    case DWG_TYPE_POLYLINE_3D:
    case DWG_TYPE_ARC:
    case DWG_TYPE_CIRCLE:
    case DWG_TYPE_LINE:
      return 0;
    case DWG_TYPE_DIMENSION_ORDINATE:
    case DWG_TYPE_DIMENSION_LINEAR:
    case DWG_TYPE_DIMENSION_ALIGNED:
    case DWG_TYPE_DIMENSION_ANG3PT:
    case DWG_TYPE_DIMENSION_ANG2LN:
    case DWG_TYPE_DIMENSION_RADIUS:
    case DWG_TYPE_DIMENSION_DIAMETER:
      return 1;
    case DWG_TYPE_POINT:
    case DWG_TYPE__3DFACE:
    case DWG_TYPE_POLYLINE_PFACE:
    case DWG_TYPE_POLYLINE_MESH:
    case DWG_TYPE_SOLID:
    case DWG_TYPE_TRACE:
    case DWG_TYPE_SHAPE:
      return 0;
    case DWG_TYPE_VIEWPORT:
      return 1;
    case DWG_TYPE_ELLIPSE:
    case DWG_TYPE_SPLINE:
      return 0;
    case DWG_TYPE_REGION:
    case DWG_TYPE__3DSOLID:
    case DWG_TYPE_BODY:
      return 1;
    case DWG_TYPE_RAY:
    case DWG_TYPE_XLINE:
      return 0;
    case DWG_TYPE_DICTIONARY:
    case DWG_TYPE_OLEFRAME:
    case DWG_TYPE_MTEXT:
    case DWG_TYPE_LEADER:
      return 1;
    case DWG_TYPE_TOLERANCE:
    case DWG_TYPE_MLINE:
      return 0;
    case DWG_TYPE_BLOCK_CONTROL:
    case DWG_TYPE_LAYER_CONTROL:
    case DWG_TYPE_STYLE_CONTROL:
    case DWG_TYPE_LTYPE_CONTROL:
    case DWG_TYPE_VIEW_CONTROL:
    case DWG_TYPE_UCS_CONTROL:
    case DWG_TYPE_VPORT_CONTROL:
    case DWG_TYPE_APPID_CONTROL:
    case DWG_TYPE_DIMSTYLE_CONTROL:
    case DWG_TYPE_VPORT_ENTITY_CONTROL:
      return 0;
    case DWG_TYPE_BLOCK_HEADER:
    case DWG_TYPE_LAYER:
    case DWG_TYPE_STYLE:
    case DWG_TYPE_LTYPE:
    case DWG_TYPE_VIEW:
    case DWG_TYPE_UCS:
    case DWG_TYPE_VPORT:
    case DWG_TYPE_APPID:
    case DWG_TYPE_DIMSTYLE:
    case DWG_TYPE_VPORT_ENTITY_HEADER:
      return 1;
    case DWG_TYPE_GROUP:
    case DWG_TYPE_MLINESTYLE:
    case DWG_TYPE_OLE2FRAME:
      return 1;
    case DWG_TYPE_DUMMY:
    case DWG_TYPE_LONG_TRANSACTION:
    case DWG_TYPE_LWPOLYLINE:
      return 0;
    case DWG_TYPE_HATCH:
    case DWG_TYPE_XRECORD:
      return 1;
    case DWG_TYPE_PLACEHOLDER:
      return 0;
    case DWG_TYPE_VBA_PROJECT:
    case DWG_TYPE_LAYOUT:
    case DWG_TYPE_PROXY_ENTITY:
    case DWG_TYPE_PROXY_OBJECT:
    default:
      return 1;
    }
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
  int error = 0;
  Dwg_Data *dwg = ent->dwg;
  Dwg_Object *obj = &dwg->object[ent->objid];
  Dwg_Object_Entity* _obj = ent;
  unsigned long object_address = bit_position(dat);

  PRE(R_13) {

    if (FIELD_VALUE(flag_r11) & 4 &&
        FIELD_VALUE(kind_r11) > 2 &&
        FIELD_VALUE(kind_r11) != 22)
      FIELD_RD (elevation_r11, 30);
    if (FIELD_VALUE(flag_r11) & 8)
      FIELD_RD (thickness_r11, 39);
    if (FIELD_VALUE(flag_r11) & 0x20) {
      Dwg_Object_Ref* hdl =
        dwg_decode_handleref_with_code(dat, obj, dwg, 0);
      if (hdl)
        obj->handle = hdl->handleref;
    }
    if (FIELD_VALUE(extra_r11) & 4)
      FIELD_RS (paper_r11, 0);
  }

  SINCE (R_2007) {
    *str_dat = *dat;
  }
  VERSIONS (R_2000, R_2007)
    {
      obj->bitsize = bit_read_RL(dat); // until the handles
      LOG_TRACE("bitsize: " FORMAT_RL " @%lu.%u\n", obj->bitsize, dat->byte, dat->bit)
    }
  SINCE (R_2007)
    {
      // The handle stream offset, i.e. end of the object, right after
      // the has_strings bit.
      obj->hdlpos = obj->address * 8 + obj->bitsize;
      obj->handle_offset = obj->bitsize;
      SINCE(R_2010)
      {
        obj->hdlpos += 8;
        obj->handle_offset += 8;
        LOG_HANDLE("(bitsize: " FORMAT_RL ", ", obj->bitsize);
        LOG_HANDLE("hdlpos: %lu)\n", obj->hdlpos);
      }
      // and set the string stream (restricted to size)
      // skip for all types without strings
      if (obj->type >= 500 || obj_has_strings(obj->type))
        error |= obj_string_stream(dat, obj, str_dat);
    }

  error |= bit_read_H(dat, &(obj->handle));
  if (error & DWG_ERR_INVALIDHANDLE)
    {
      LOG_WARN("dwg_decode_entity handle @%lu.%u", dat->byte, dat->bit);
      obj->bitsize = 0;
      ent->num_eed = 0;
      ent->picture_exists = 0;
      return error;
    }
  LOG_TRACE("handle: %d.%d.%lX [5]\n", obj->handle.code,
            obj->handle.size, obj->handle.value)

  PRE(R_13) {
    return DWG_ERR_NOTYETSUPPORTED;
  }

  error |= dwg_decode_eed(dat, (Dwg_Object_Object *)ent);
  if (error & (DWG_ERR_INVALIDEED|DWG_ERR_VALUEOUTOFBOUNDS))
    return error;

  #include "common_entity_data.spec"

  // elsewhere: object data, handles, padding bits, crc
  obj->common_size = bit_position(dat) - object_address;
  LOG_HANDLE("--common_size: %lu\n", obj->common_size); // needed for unknown

  return error;
}

/* The first common part of every object.

   There is no COMMON_ENTITY_DATA for objects.
   Check page 269, par 28 (Extended Object Data)
 */
static int
dwg_decode_object(Bit_Chain* dat, Bit_Chain* hdl_dat, Bit_Chain* str_dat,
                  Dwg_Object_Object* _obj)
{
  unsigned int i;
  BITCODE_BS size;
  int error = 0;
  Dwg_Data *dwg = _obj->dwg;
  Dwg_Object *obj = &dwg->object[_obj->objid];
  unsigned long object_address = bit_position(dat);

  _obj->datpos = dat->byte;     // the data stream offset
  SINCE(R_2007) {
    *str_dat = *dat;
  }
  VERSIONS(R_2000, R_2007)
    {
      obj->bitsize = bit_read_RL(dat);
      LOG_TRACE("bitsize: " FORMAT_RL " @%lu.%u\n", obj->bitsize, dat->byte, dat->bit)
    }
  SINCE(R_2007)
    {
      //LOG_HANDLE("Object bitsize: " FORMAT_RL " @%lu.%u %lu\n", _obj->bitsize,
      //         dat->byte, dat->bit, bit_position(dat));
      // The handle stream offset, i.e. end of the object, right after
      // the has_strings bit.
      obj->hdlpos = (obj->address * 8) + obj->bitsize;
      obj->handle_offset = obj->bitsize;
      SINCE(R_2010)
      {
        obj->hdlpos += 8;
        obj->handle_offset += 8;
        LOG_HANDLE("(bitsize: " FORMAT_RL ", ", obj->bitsize);
        LOG_HANDLE("hdlpos: %lu)\n", obj->hdlpos);
      }
      // and set the string stream (restricted to size)
      if (obj->type >= 500 || obj_has_strings(obj->type))
        error |= obj_string_stream(dat, obj, str_dat);
    }

  error = bit_read_H(dat, &obj->handle);
  if(error & DWG_ERR_INVALIDHANDLE)
    {
      LOG_ERROR("Wrong object handle at pos 0x%0lx", dat->byte)
      return error;
    }
  LOG_TRACE("handle: %x.%d.%lX [5]\n", obj->handle.code,
            obj->handle.size, obj->handle.value)

  error |= dwg_decode_eed(dat, _obj);
  if (error & (DWG_ERR_INVALIDEED|DWG_ERR_VALUEOUTOFBOUNDS))
    return error;

  VERSIONS(R_13,R_14)
    {
      obj->bitsize = bit_read_RL(dat);
      LOG_TRACE("bitsize: %u [RL]\n", obj->bitsize);
    }
  // documentation bug, same BL type as with entity
  FIELD_BL(num_reactors, 0);
  SINCE (R_2010)
  {
    if (_obj->num_reactors > 0x1000)
      {
        LOG_WARN("Invalid num_reactors %ld", (long)_obj->num_reactors)
        _obj->num_reactors = 0;
      }
  }
  SINCE(R_2004)
    {
      FIELD_B (xdic_missing_flag, 0);
    }
  SINCE(R_2013)
    {
      FIELD_B (has_ds_binary_data, 0);
    }
  obj->common_size = bit_position(dat) - object_address;
  LOG_HANDLE("--common_size: %lu\n", obj->common_size); // needed for unknown

  return error;
}

/* Store an object reference in a separate dwg->object_ref array
   which is the id for handles, i.e. DXF 5, 330. */
Dwg_Object_Ref *
dwg_decode_handleref(Bit_Chain *restrict dat, Dwg_Object *restrict obj,
                     Dwg_Data *restrict dwg)
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
      LOG_WARN("Invalid handleref: (%d.%d.%lX)",
               ref->handleref.code, ref->handleref.size, ref->handleref.value)
      free(ref);
      return NULL;
    }

  // If the handle size is 0 and not a relative handle, it is probably a null handle.
  // It shouldn't be placed in the object ref vector.
  if (ref->handleref.size || (obj && ref->handleref.code > 5))
    {
      // Reserve memory space for object references
      if (!dwg->num_object_refs)
        dwg->object_ref = calloc(REFS_PER_REALLOC, sizeof(Dwg_Object_Ref*));
      else if (dwg->num_object_refs % REFS_PER_REALLOC == 0)
        dwg->object_ref = realloc(dwg->object_ref,
              (dwg->num_object_refs + REFS_PER_REALLOC) * sizeof(Dwg_Object_Ref*));
      if (!dwg->object_ref)
        {
          LOG_ERROR("Out of memory");
          return NULL;
        }
      dwg->object_ref[dwg->num_object_refs++] = ref;
    }
  else if (!ref->handleref.value)
    {
      ref->absolute_ref = 0;
      ref->obj = NULL;
      return ref;
    }

  // We receive a null obj when we are reading
  // handles in the header variables section
  if (!obj && ref->handleref.value)
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
    case 0x0E: //eg 2007 REGION.history_id (some very high number)
      ref->absolute_ref = obj->handle.value;
      break;
    case 2: case 3: case 4: case 5:
      ref->absolute_ref = ref->handleref.value;
      break;
    case 0: // ignore?
      ref->absolute_ref = ref->handleref.value;
      break;
    default:
      dwg->object_ref[dwg->num_object_refs-1] = NULL;
      dwg->num_object_refs--;
      ref->absolute_ref = 0;
      ref->obj = NULL;
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
 *   e ??
 */
Dwg_Object_Ref *
dwg_decode_handleref_with_code(Bit_Chain *restrict dat,
                               Dwg_Object *restrict obj,
                               Dwg_Data *restrict dwg, unsigned int code)
{
  Dwg_Object_Ref* ref = (Dwg_Object_Ref *) calloc(1, sizeof(Dwg_Object_Ref));
  if (!ref)
    {
      LOG_ERROR("Out of memory");
      return NULL;
    }

  if (bit_read_H(dat, &ref->handleref))
    {
      LOG_WARN("Invalid handleref: wanted code %d, got (%d.%d.%lX)",
               code, ref->handleref.code, ref->handleref.size,
               ref->handleref.value)
      free(ref);
      return NULL;
    }

  // If the handle size is 0 and not a relative handle, it is probably
  // a null handle.
  // It shouldn't be placed in the object ref vector.
  if (ref->handleref.size || (obj && ref->handleref.code > 5))
    {
      // Reserve memory space for object references
      if (!dwg->num_object_refs)
        dwg->object_ref = calloc(REFS_PER_REALLOC, sizeof(Dwg_Object_Ref*));
      else if (dwg->num_object_refs % REFS_PER_REALLOC == 0)
        dwg->object_ref = realloc(dwg->object_ref,
              (dwg->num_object_refs + REFS_PER_REALLOC) * sizeof(Dwg_Object_Ref*));
      if (!dwg->object_ref)
        {
          LOG_ERROR("Out of memory");
          return NULL;
        }
      dwg->object_ref[dwg->num_object_refs++] = ref;
    }
  else if (!ref->handleref.value)
    {
      ref->obj = NULL;
      ref->absolute_ref = 0;
      return ref;
    }

  // We receive a null obj when we are reading
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
    case 0x0E: //eg 2007 REGION.history_id (some very high number)
      ref->absolute_ref = obj->handle.value;
      break;
    case 2: case 3: case 4: case 5:
      ref->absolute_ref = ref->handleref.value;
      break;
    case 0: // ignore?
      ref->absolute_ref = ref->handleref.value;
      break;
    default:
      dwg->object_ref[dwg->num_object_refs-1] = NULL;
      dwg->num_object_refs--;
      ref->absolute_ref = 0;
      ref->obj = NULL;
      LOG_WARN("Invalid handle pointer code %d", ref->handleref.code);
      break;
    }
  return ref;
}

int
dwg_decode_header_variables(Bit_Chain* dat,
                            Bit_Chain* hdl_dat,
                            Bit_Chain* str_dat,
                            Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables* _obj = &dwg->header_vars;
  Dwg_Object* obj = NULL;
  int error = 0;

  #include "header_variables.spec"

  return error;
}

static int
dwg_decode_common_entity_handle_data(Bit_Chain* dat, Bit_Chain* hdl_dat,
                                     Dwg_Object *restrict obj)
{

  Dwg_Data *dwg = obj->parent;
  Dwg_Object_Entity *_obj;
  BITCODE_BL vcount;
  Dwg_Object_Entity *_ent;
  int error = 0;

  _obj = _ent = obj->tio.entity;

  #include "common_entity_handle_data.spec"

  return error;
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
              if (gc == 999) return VT_STRING; // lgtm [cpp/constant-comparison]
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
              if (gc <= 439) return VT_STRING; // lgtm [cpp/constant-comparison]
            }
          else            // 330-389
            {
              if (gc <= 309) return VT_STRING;
              if (gc <= 319) return VT_BINARY;
              if (gc <= 329) return VT_HANDLE;
              if (gc <= 369) return VT_OBJECTID;
              if (gc <= 389) return VT_INT16; // lgtm [cpp/constant-comparison]
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
          if (gc <= 299) return VT_BOOL; // lgtm [cpp/constant-comparison]
        }
      else               // 105-209
        {
          if (gc == 105) return VT_HANDLE;
          if (gc <= 109) return VT_INVALID;
          if (gc <= 149) return VT_REAL;
          if (gc <= 169) return VT_INVALID;
          if (gc <= 179) return VT_INT16;
          if (gc <= 209) return VT_INVALID; // lgtm [cpp/constant-comparison]
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
          if (gc < 0)    return VT_HANDLE;
          if (gc <= 4)   return VT_STRING;
          if (gc == 5)   return VT_HANDLE;
          if (gc <= 9)   return VT_STRING;
          if (gc <= 37)  return VT_POINT3D; // lgtm [cpp/constant-comparison]
        }
    }
  return VT_INVALID;
}

void
dwg_free_xdata_resbuf(Dwg_Resbuf *rbuf)
{
  while (rbuf)
    {
      Dwg_Resbuf *next = rbuf->next;
      short type = get_base_value_type(rbuf->type);
      if (type == VT_STRING || type == VT_BINARY)
        free (rbuf->value.str.u.data);
      free (rbuf);
      rbuf = next;
    }
}

// TODO: unify with eed[], use an array not linked list.
static Dwg_Resbuf*
dwg_decode_xdata(Bit_Chain *restrict dat, Dwg_Object_XRECORD *restrict obj, int size)
{
  Dwg_Resbuf *rbuf, *root = NULL, *curr = NULL;
  unsigned char codepage;
  long unsigned int end_address;
  BITCODE_BL i, num_xdata = 0;
  BITCODE_RS length;

  static int cnt = 0;
  cnt++;

  end_address = dat->byte + (unsigned long int)size;
  LOG_INSANE("xdata:\n");
  LOG_INSANE_TF(&dat->chain[dat->byte], size);

  while (dat->byte < end_address)
    {
      rbuf = (Dwg_Resbuf *) calloc(1, sizeof(Dwg_Resbuf));
      if (!rbuf)
        {
          LOG_ERROR("Out of memory");
          dwg_free_xdata_resbuf(root);
          return NULL;
        }
      num_xdata++;
      rbuf->next = NULL;
      rbuf->type = bit_read_RS(dat);

      switch (get_base_value_type(rbuf->type))
        {
        case VT_STRING:
          UNTIL(R_2007) {
            length = rbuf->value.str.size = bit_read_RS(dat);
            rbuf->value.str.codepage = bit_read_RC(dat);
            rbuf->value.str.u.data = bit_read_TF(dat, length);
            LOG_TRACE("xdata[%d]: \"%s\" [%d]\n", num_xdata,
                      rbuf->value.str.u.data, rbuf->type);
          } LATER_VERSIONS {
            length = rbuf->value.str.size = bit_read_RS(dat);
            if (length > 0)
              {
                rbuf->value.str.u.wdata = calloc(length + 1, 2);
                if (!rbuf->value.str.u.wdata)
                  {
                    LOG_ERROR("Out of memory");
                    if (root)
                      {
                        dwg_free_xdata_resbuf(root);
                        if (rbuf) free(rbuf);
                      }
                    else
                      dwg_free_xdata_resbuf(rbuf);
                    return NULL;
                  }
                for (i = 0; i < length; i++)
                  rbuf->value.str.u.wdata[i] = bit_read_RS(dat);
                rbuf->value.str.u.wdata[i] = '\0';
                LOG_TRACE_TU("xdata", rbuf->value.str.u.wdata, rbuf->type);
              }
          }
          break;
        case VT_REAL:
          rbuf->value.dbl = bit_read_RD(dat);
          LOG_TRACE("xdata[%d]: %f [%d]\n", num_xdata, rbuf->value.dbl, rbuf->type);
          break;
        case VT_BOOL:
        case VT_INT8:
          rbuf->value.i8 = bit_read_RC(dat);
          LOG_TRACE("xdata[%d]: %d [%d]\n", num_xdata, (int)rbuf->value.i8, rbuf->type);
          break;
        case VT_INT16:
          rbuf->value.i16 = bit_read_RS(dat);
          LOG_TRACE("xdata[%d]: %d [%d]\n", num_xdata, (int)rbuf->value.i16, rbuf->type);
          break;
        case VT_INT32:
          rbuf->value.i32 = bit_read_RL(dat);
          LOG_TRACE("xdata[%d]: %d [%d]\n", num_xdata, (int)rbuf->value.i32, rbuf->type);
          break;
        case VT_POINT3D:
          rbuf->value.pt[0] = bit_read_RD(dat);
          rbuf->value.pt[1] = bit_read_RD(dat);
          rbuf->value.pt[2] = bit_read_RD(dat);
          LOG_TRACE("xdata[%d]: %f,%f,%f [%d]\n", num_xdata, rbuf->value.pt[0],
                    rbuf->value.pt[2], rbuf->value.pt[2], rbuf->type);
          break;
        case VT_BINARY:
          rbuf->value.str.size = bit_read_RC(dat);
          rbuf->value.str.u.data = bit_read_TF(dat, rbuf->value.str.size);
          LOG_TRACE("xdata[%d]: ", num_xdata);
          LOG_TRACE_TF(rbuf->value.str.u.data, rbuf->value.str.size);
          break;
        case VT_HANDLE:
        case VT_OBJECTID:
          bit_read_fixed(dat, (char*)rbuf->value.hdl, 8);
          LOG_TRACE("xdata[%d]: %lX [HDL %d]\n", num_xdata,
                    (unsigned long)*(uint64_t*)rbuf->value.hdl, rbuf->type);
          break;
        case VT_INVALID:
        default:
          LOG_ERROR("Invalid group code in xdata: %d", rbuf->type)
          dwg_free_xdata_resbuf(rbuf);
          dat->byte = end_address;
          obj->num_eed = num_xdata;
          return root;
        }

      if (!curr)
        {
          curr = root = rbuf;
        }
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

static int
decode_preR13_entities(unsigned long start, unsigned long end,
                       unsigned long offset,
                       Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int error = 0;
  BITCODE_BL num = dwg->num_objects;
  dat->bit = 0;
  LOG_TRACE("entities: (0x%lx-0x%lx, offset 0x%lx) TODO\n", start, end, offset)
  while (dat->byte < end)
    {
      Dwg_Object *obj;
      Dwg_Object_Entity* ent;
      BITCODE_RS crc;

      if (!num)
        dwg->object = (Dwg_Object *) malloc(REFS_PER_REALLOC * sizeof(Dwg_Object));
      else if (num % REFS_PER_REALLOC == 0)
        dwg->object = realloc(dwg->object,
            (num + REFS_PER_REALLOC) * sizeof(Dwg_Object));
      if (!dwg->object)
        {
          LOG_ERROR("Out of memory");
          return DWG_ERR_OUTOFMEM;
        }
      obj = &dwg->object[num];
      memset(obj, 0, sizeof(Dwg_Object));
      dwg->num_objects++;
      obj->index = num;
      obj->parent = dwg;
      obj->address = dat->byte;

      DEBUG_HERE;

      switch (obj->type)
        {
          case 1:
            error |= dwg_decode_LINE(dat, obj);
            break;
          case 2:
            error |= dwg_decode_POINT(dat, obj);
            break;
          case 3:
            error |= dwg_decode_CIRCLE(dat, obj);
            break;
          case 4:
            error |= dwg_decode_SHAPE(dat, obj);
            break;
          case 7:
            error |= dwg_decode_TEXT(dat, obj);
            break;
          case 8:
            error |= dwg_decode_ARC(dat, obj);
            break;
          case 9:
            error |= dwg_decode_TRACE(dat, obj);
            break;
          case 11:
            error |= dwg_decode_SOLID(dat, obj);
            break;
          case 12:
            error |= dwg_decode_BLOCK(dat, obj);
            break;
          case 13:
            error |= dwg_decode_ENDBLK(dat, obj);
            break;
          case 14:
            error |= dwg_decode_INSERT(dat, obj);
            break;
          case 15:
            error |= dwg_decode_ATTDEF(dat, obj);
            break;
          case 16:
            error |= dwg_decode_ATTRIB(dat, obj);
            break;
          case 17:
            error |= dwg_decode_SEQEND(dat, obj);
            break;
          case 19:
            error |= dwg_decode_POLYLINE_2D(dat, obj);
            break;
          case 20:
            error |= dwg_decode_VERTEX_2D(dat, obj);
            break;
          case 22:
            error |= dwg_decode__3DFACE(dat, obj);
            break;
          case 23:
            //TODO check opts for the type of dimension
            error |= dwg_decode_DIMENSION_LINEAR(dat, obj);
            break;
          case 24:
            error |= dwg_decode_VPORT(dat, obj);
            break;
          default:
            LOG_ERROR("Unknown object type %d", obj->type)
            break;
        }

      bit_set_position(dat, obj->address + obj->size - 2);
      crc = bit_read_RS(dat);
      LOG_TRACE("crc:  %X [RS]\n", crc);
      num++;

      if (obj->size < 2 || obj->size > 0x1000) //FIXME
        dat->byte = end;
    }

  dat->byte = end;
  return error;
}

/** dwg_decode_variable_type
 * decode object by class name, not type. if type > 500.
 * returns 0 on success
 */
static int
dwg_decode_variable_type(Dwg_Data *restrict dwg, Bit_Chain* dat, Bit_Chain* hdl_dat,
                         Dwg_Object *restrict obj)
{
  Dwg_Class *klass;
  int i;
  int is_entity;

  i = obj->type - 500;
  if (i < 0 || i >= dwg->num_classes)
    {
      LOG_ERROR("Invalid object type %d, only %u classes",
                obj->type, dwg->num_classes);
      return DWG_ERR_INVALIDTYPE;
    }

  klass = &dwg->dwg_class[i];
  if (!klass || ! klass->dxfname)
    return DWG_ERR_INTERNALERROR;
  obj->dxfname = klass->dxfname;
  // almost always false
  is_entity = dwg_class_is_entity(klass);

  // global class dispatcher
  #include "classes.inc"

  // TODO: ACSH_BOX_CLASS, ACAD_EVALUATION_GRAPH, ACSH_HISTORY_CLASS

  LOG_WARN("Unknown Class %s %d %s (0x%x%s)", is_entity ? "entity" : "object", \
           klass->number, klass->dxfname, klass->proxyflag,                    \
           klass->wasazombie ? " was proxy" : "")

  return DWG_ERR_UNHANDLEDCLASS;
}

/** Adds a new empty obj to the dwg->object[] array.
    The new object is at &dwg->object[dwg->num_objects-1].

    Returns 0 or some error codes on success.
    Returns -1 if the dwg->object pool was re-alloced, i.e. pointers within
    are invalidated.
    Returns DWG_ERR_OUTOFMEM otherwise.
 */
EXPORT int dwg_add_object (Dwg_Data *dwg)
{
  Dwg_Object *obj;
  BITCODE_BL num = dwg->num_objects;
  int realloced = 0;
  if (!num)
    dwg->object = calloc(REFS_PER_REALLOC, sizeof(Dwg_Object));
  else if (num % REFS_PER_REALLOC == 0) {
    Dwg_Object *old = dwg->object;
    dwg->object = realloc(dwg->object, (num + REFS_PER_REALLOC) * sizeof(Dwg_Object));
    realloced = old != dwg->object;
  }
  if (!dwg->object) return DWG_ERR_OUTOFMEM;

  obj = &dwg->object[num];
  memset(obj, 0, sizeof(Dwg_Object));
  obj->index = num;
  dwg->num_objects++;
  obj->parent = dwg;
  return realloced ? -1 : 0;
}

/** Adds an object to the DWG (i.e. dwg->object[dwg->num_objects])
    Returns 0 or some error codes on success.
    Returns -1 if the dwg->object pool was re-alloced.
    Returns some DWG_ERR_* otherwise.
 */
int
dwg_decode_add_object(Dwg_Data *restrict dwg, Bit_Chain* dat, Bit_Chain* hdl_dat,
                      long unsigned int address)
{
  long unsigned int oldpos;
  long unsigned int object_address, end_address;
  unsigned char previous_bit;
  Dwg_Object *obj;
  BITCODE_BL num = dwg->num_objects;
  int error = 0;
  int realloced = 0;

  /* Keep the previous address
   */
  oldpos = dat->byte;
  previous_bit = dat->bit;

  /* Use the indicated address for the object
   */
  dat->byte = address;
  dat->bit = 0;

  //DEBUG_HERE;
  /*
   * Reserve memory space for objects. A realloc violates all internal pointers.
   */
  realloced = dwg_add_object(dwg);
  if (realloced > 0)
    return realloced; // i.e. DWG_ERR_OUTOFMEM
  obj = &dwg->object[num];
  LOG_INFO("==========================================\n"
           "Object number: %lu/%lX", (unsigned long)num, (unsigned long)num)

  obj->size = bit_read_MS(dat);
  LOG_INFO(", Size: %d/0x%x", obj->size, obj->size)
  obj->address = dat->byte;
  end_address = obj->address + obj->size; /* (calculate the bitsize) */

  SINCE(R_2010)
  {
    obj->handlestream_size = bit_read_UMC(dat);
    LOG_INFO(", Hdlsize: " FORMAT_UMC, obj->handlestream_size);
    obj->bitsize = obj->size * 8 - obj->handlestream_size;
    obj->type = bit_read_BOT(dat);
  } else {
    obj->type = bit_read_BS(dat);
  }
  LOG_INFO(", Type: %d\n", obj->type)
  object_address = bit_position(dat);

  /* Check the type of the object
   */
  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      error = dwg_decode_TEXT(dat, obj);
      break;
    case DWG_TYPE_ATTRIB:
      error = dwg_decode_ATTRIB(dat, obj);
      break;
    case DWG_TYPE_ATTDEF:
      error = dwg_decode_ATTDEF(dat, obj);
      break;
    case DWG_TYPE_BLOCK:
      error = dwg_decode_BLOCK(dat, obj);
      break;
    case DWG_TYPE_ENDBLK:
      error = dwg_decode_ENDBLK(dat, obj);
      break;
    case DWG_TYPE_SEQEND:
      error = dwg_decode_SEQEND(dat, obj);
      break;
    case DWG_TYPE_INSERT:
      error = dwg_decode_INSERT(dat, obj);
      break;
    case DWG_TYPE_MINSERT:
      error = dwg_decode_MINSERT(dat, obj);
      break;
    case DWG_TYPE_VERTEX_2D:
      error = dwg_decode_VERTEX_2D(dat, obj);
      break;
    case DWG_TYPE_VERTEX_3D:
      error = dwg_decode_VERTEX_3D(dat, obj);
      break;
    case DWG_TYPE_VERTEX_MESH:
      error = dwg_decode_VERTEX_MESH(dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE:
      error = dwg_decode_VERTEX_PFACE(dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE_FACE:
      error = dwg_decode_VERTEX_PFACE_FACE(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_2D:
      error = dwg_decode_POLYLINE_2D(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_3D:
      error = dwg_decode_POLYLINE_3D(dat, obj);
      break;
    case DWG_TYPE_ARC:
      error = dwg_decode_ARC(dat, obj);
      break;
    case DWG_TYPE_CIRCLE:
      error = dwg_decode_CIRCLE(dat, obj);
      break;
    case DWG_TYPE_LINE:
      error = dwg_decode_LINE(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ORDINATE:
      error = dwg_decode_DIMENSION_ORDINATE(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_LINEAR:
      error = dwg_decode_DIMENSION_LINEAR(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ALIGNED:
      error = dwg_decode_DIMENSION_ALIGNED(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG3PT:
      error = dwg_decode_DIMENSION_ANG3PT(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG2LN:
      error = dwg_decode_DIMENSION_ANG2LN(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_RADIUS:
      error = dwg_decode_DIMENSION_RADIUS(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_DIAMETER:
      error = dwg_decode_DIMENSION_DIAMETER(dat, obj);
      break;
    case DWG_TYPE_POINT:
      error = dwg_decode_POINT(dat, obj);
      break;
    case DWG_TYPE__3DFACE:
      error = dwg_decode__3DFACE(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_PFACE:
      error = dwg_decode_POLYLINE_PFACE(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_MESH:
      error = dwg_decode_POLYLINE_MESH(dat, obj);
      break;
    case DWG_TYPE_SOLID:
      error = dwg_decode_SOLID(dat, obj);
      break;
    case DWG_TYPE_TRACE:
      error = dwg_decode_TRACE(dat, obj);
      break;
    case DWG_TYPE_SHAPE:
      error = dwg_decode_SHAPE(dat, obj);
      break;
    case DWG_TYPE_VIEWPORT:
      error = dwg_decode_VIEWPORT(dat, obj);
      break;
    case DWG_TYPE_ELLIPSE:
      error = dwg_decode_ELLIPSE(dat, obj);
      break;
    case DWG_TYPE_SPLINE:
      error = dwg_decode_SPLINE(dat, obj);
      break;
    case DWG_TYPE_REGION:
      error = dwg_decode_REGION(dat, obj);
      break;
    case DWG_TYPE__3DSOLID:
      error = dwg_decode__3DSOLID(dat, obj);
      break;
    case DWG_TYPE_BODY:
      error = dwg_decode_BODY(dat, obj);
      break;
    case DWG_TYPE_RAY:
      error = dwg_decode_RAY(dat, obj);
      break;
    case DWG_TYPE_XLINE:
      error = dwg_decode_XLINE(dat, obj);
      break;
    case DWG_TYPE_DICTIONARY:
      error = dwg_decode_DICTIONARY(dat, obj);
      break;
    case DWG_TYPE_MTEXT:
      error = dwg_decode_MTEXT(dat, obj);
      break;
    case DWG_TYPE_LEADER:
      error = dwg_decode_LEADER(dat, obj);
      break;
    case DWG_TYPE_TOLERANCE:
      error = dwg_decode_TOLERANCE(dat, obj);
      break;
    case DWG_TYPE_MLINE:
      error = dwg_decode_MLINE(dat, obj);
      break;
    case DWG_TYPE_BLOCK_CONTROL:
      error = dwg_decode_BLOCK_CONTROL(dat, obj);
      obj->tio.object->tio.BLOCK_CONTROL->objid = num;
      dwg->block_control = *obj->tio.object->tio.BLOCK_CONTROL;
      break;
    case DWG_TYPE_BLOCK_HEADER:
      error = dwg_decode_BLOCK_HEADER(dat, obj);
      /*
       * We cannot cache dwg->*space_block here as dwg->objects might get realloc'ed.
       * See dwg_model_space_object() and dwg_paper_space_object() instead.
       */
      break;
    case DWG_TYPE_LAYER_CONTROL:
      error = dwg_decode_LAYER_CONTROL(dat, obj);
      obj->tio.object->tio.LAYER_CONTROL->objid = num;
      dwg->layer_control = *obj->tio.object->tio.LAYER_CONTROL;
      break;
    case DWG_TYPE_LAYER:
      error = dwg_decode_LAYER(dat, obj);
      break;
    case DWG_TYPE_STYLE_CONTROL:
      error = dwg_decode_STYLE_CONTROL(dat, obj);
      obj->tio.object->tio.STYLE_CONTROL->objid = num;
      dwg->style_control = *obj->tio.object->tio.STYLE_CONTROL;
      break;
    case DWG_TYPE_STYLE:
      error = dwg_decode_STYLE(dat, obj);
      break;
    case DWG_TYPE_LTYPE_CONTROL:
      error = dwg_decode_LTYPE_CONTROL(dat, obj);
      obj->tio.object->tio.LTYPE_CONTROL->objid = num;
      dwg->ltype_control = *obj->tio.object->tio.LTYPE_CONTROL;
      break;
    case DWG_TYPE_LTYPE:
      error = dwg_decode_LTYPE(dat, obj);
      break;
    case DWG_TYPE_VIEW_CONTROL:
      error = dwg_decode_VIEW_CONTROL(dat, obj);
      obj->tio.object->tio.VIEW_CONTROL->objid = num;
      dwg->view_control = *obj->tio.object->tio.VIEW_CONTROL;
      break;
    case DWG_TYPE_VIEW:
      error = dwg_decode_VIEW(dat, obj);
      break;
    case DWG_TYPE_UCS_CONTROL:
      error = dwg_decode_UCS_CONTROL(dat, obj);
      obj->tio.object->tio.UCS_CONTROL->objid = num;
      dwg->ucs_control = *obj->tio.object->tio.UCS_CONTROL;
      break;
    case DWG_TYPE_UCS:
      error = dwg_decode_UCS(dat, obj);
      break;
    case DWG_TYPE_VPORT_CONTROL:
      error = dwg_decode_VPORT_CONTROL(dat, obj);
      obj->tio.object->tio.VPORT_CONTROL->objid = num;
      dwg->vport_control = *obj->tio.object->tio.VPORT_CONTROL;
      break;
    case DWG_TYPE_VPORT:
      error = dwg_decode_VPORT(dat, obj);
      break;
    case DWG_TYPE_APPID_CONTROL:
      error = dwg_decode_APPID_CONTROL(dat, obj);
      obj->tio.object->tio.APPID_CONTROL->objid = num;
      dwg->appid_control = *obj->tio.object->tio.APPID_CONTROL;
      break;
    case DWG_TYPE_APPID:
      error = dwg_decode_APPID(dat, obj);
      break;
    case DWG_TYPE_DIMSTYLE_CONTROL:
      error = dwg_decode_DIMSTYLE_CONTROL(dat, obj);
      obj->tio.object->tio.DIMSTYLE_CONTROL->objid = num;
      dwg->dimstyle_control = *obj->tio.object->tio.DIMSTYLE_CONTROL;
      break;
    case DWG_TYPE_DIMSTYLE:
      error = dwg_decode_DIMSTYLE(dat, obj);
      break;
    case DWG_TYPE_VPORT_ENTITY_CONTROL:
      error = dwg_decode_VPORT_ENTITY_CONTROL(dat, obj);
      obj->tio.object->tio.VPORT_ENTITY_CONTROL->objid = num;
      dwg->vport_entity_control = *obj->tio.object->tio.VPORT_ENTITY_CONTROL;
      break;
    case DWG_TYPE_VPORT_ENTITY_HEADER:
      error = dwg_decode_VPORT_ENTITY_HEADER(dat, obj);
      break;
    case DWG_TYPE_GROUP:
      error = dwg_decode_GROUP(dat, obj);
      break;
    case DWG_TYPE_MLINESTYLE:
      error = dwg_decode_MLINESTYLE(dat, obj);
      break;
    case DWG_TYPE_OLE2FRAME:
      error = dwg_decode_OLE2FRAME(dat, obj);
      break;
    case DWG_TYPE_DUMMY:
      error = dwg_decode_DUMMY(dat, obj);
      break;
    case DWG_TYPE_LONG_TRANSACTION:
      error = dwg_decode_LONG_TRANSACTION(dat, obj);
      break;
    case DWG_TYPE_LWPOLYLINE:
      error = dwg_decode_LWPOLYLINE(dat, obj);
      break;
    case DWG_TYPE_HATCH:
      error = dwg_decode_HATCH(dat, obj);
      break;
    case DWG_TYPE_XRECORD:
      error = dwg_decode_XRECORD(dat, obj);
      break;
    case DWG_TYPE_PLACEHOLDER:
      error = dwg_decode_PLACEHOLDER(dat, obj);
      break;
    case DWG_TYPE_OLEFRAME:
      error = dwg_decode_OLEFRAME(dat, obj);
      break;
    case DWG_TYPE_VBA_PROJECT:
      LOG_ERROR("Unhandled Object VBA_PROJECT. Has its own section");
      //dwg_decode_VBA_PROJECT(dat, obj);
      error = DWG_ERR_UNHANDLEDCLASS;
      break;
    case DWG_TYPE_LAYOUT:
      error = dwg_decode_LAYOUT(dat, obj);
      break;
    case DWG_TYPE_PROXY_ENTITY:
      error = dwg_decode_PROXY_ENTITY(dat, obj);
      break;
    case DWG_TYPE_PROXY_OBJECT:
      error = dwg_decode_PROXY_OBJECT(dat, obj);
      break;
    default:
      if (obj->type == dwg->layout_number)
        error = dwg_decode_LAYOUT(dat, obj);
      /* > 500 */
      else if ((error = dwg_decode_variable_type(dwg, dat, hdl_dat, obj))
               & DWG_ERR_UNHANDLEDCLASS)
        {
          int is_entity = 0;
          int i = obj->type - 500;
          Dwg_Class *klass = NULL;

          /* restart and read into the UNKNOWN_OBJ object */
          /* the relative offset from type after common_entity_data */
          //obj->common_size = bit_position(dat) - object_address; 
          //LOG_HANDLE("common_size: %lu\n", obj->common_size); // needed for unknown
          bit_set_position(dat, object_address);
          //obj->unknown_off = obj->unknown_pos - object_address;
          //LOG_TRACE("Unknown pos %lu, offset %lu\n", obj->unknown_pos, obj->unknown_off);

          if (i > 0 && i <= (int)dwg->num_classes)
            {
              klass = &dwg->dwg_class[i];
              is_entity = dwg_class_is_entity(klass);
            }
          else {
            if (i <= 0)
              {
                LOG_ERROR("Invalid class index %d <0", i);
              }
            else
              {
                LOG_ERROR("Invalid class index %d >%d", i,
                          (int)dwg->num_classes);
              }
            obj->supertype = DWG_SUPERTYPE_UNKNOWN;
            obj->type = 0;
            dat->byte = oldpos;
            dat->bit  = previous_bit;
            return error | DWG_ERR_VALUEOUTOFBOUNDS;
          }
          // properly dwg_decode_object/_entity for eed, reactors, xdic
          if (klass && !is_entity)
            {
              error |= dwg_decode_UNKNOWN_OBJ(dat, obj);
              obj->supertype = DWG_SUPERTYPE_UNKNOWN;
            }
          else if (klass)
            {
#if 0 && !defined(IS_RELEASE)
              if (!strcmp(klass->dxfname, "MULTILEADER")) { //debug CED
                char *mleader = bit_read_TF(dat, obj->size);
                LOG_INSANE_TF(mleader, (int)obj->size)
                bit_set_position(dat, object_address);
                free (mleader);
              }
#endif
              error |= dwg_decode_UNKNOWN_ENT(dat, obj);
              obj->supertype = DWG_SUPERTYPE_UNKNOWN;
            }
          else // not a class
            {
              LOG_WARN("Unknown object, skipping eed/reactors/xdic");
              SINCE(R_2000)
                {
                  obj->bitsize = bit_read_RL(dat);
                  LOG_TRACE("bitsize: " FORMAT_RL " @%lu.%u\n", obj->bitsize,
                            dat->byte, dat->bit);
                }
              if (!bit_read_H(dat, &obj->handle))
                {
                  LOG_TRACE("handle: %d.%d.%lX [5]\n",
                            obj->handle.code, obj->handle.size, obj->handle.value);
                }
              object_address = dat->byte;
              obj->supertype = DWG_SUPERTYPE_UNKNOWN;
              obj->tio.unknown = (unsigned char *)bit_read_TF(dat, obj->size);
              dat->byte = object_address;
            }
        }
    }

  if (obj->handle.value) { // empty only with UNKNOWN
    LOG_HANDLE("object_map{%lX} = %lu\n", obj->handle.value, (unsigned long)num);
    hash_set(dwg->object_map, obj->handle.value, (uint32_t)num);
  }

  /* Now 1 padding bits until next byte, and then a RS CRC */
  if (dat->bit) {
    unsigned char r = 8 - dat->bit;
    LOG_HANDLE("padding: %X/%X (%d bits)\n", dat->chain[dat->byte],
              dat->chain[dat->byte] & ((1<<r)-1), r);
    bit_advance_position(dat, r);
  }
#if 1
  bit_check_CRC(dat, address, 0xC0C1);
#else  
  {
    BITCODE_RS seed, calc;
    BITCODE_RS crc = bit_read_RS(dat);
    LOG_TRACE("crc:  %X [RS]\n", crc);
    dat->byte -= 2;
    // experimentally verify the seed 0xC0C1, for all objects
    for (seed=0; seed<0xffff; seed++) {
      calc = bit_calc_CRC(seed, &(dat->chain[address]), dat->byte - address);
      if (calc == crc)
        break;
    }
    bit_check_CRC(dat, address, seed);
    LOG_TRACE("seed: %X [RS]\n", seed);
    LOG_TRACE("size: %d\n", (int)(dat->byte - address - 2));
  }
#endif

  /* Register the previous addresses for return
   */
  dat->byte = oldpos;
  dat->bit = previous_bit;
  return realloced ? -1 : error; //re-alloced or not
}

/** dwg_decode_unknown
   Container to hold a unknown class entity, see classes.inc
   Every DEBUGGING class holds a bits array, a bitsize, and the handle
   and string stream offsets.
   It starts after the common_entity|object_data until and goes until the end
   of final padding, to the CRC.
   (obj->address+obj->common_size/8 .. obj->address+obj->size)
 */
int
dwg_decode_unknown(Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  // bitsize does not include the handles size
  int num_bytes;
  unsigned long pos = bit_position(dat);
  long bitsize = 8*(obj->address + obj->size) - pos;
  if (bitsize < 0)
    return DWG_ERR_VALUEOUTOFBOUNDS;

  //*pre_bits = pos % 8;
  obj->num_unknown_bits = bitsize;
  num_bytes = bitsize / 8;
  if (bitsize % 8) num_bytes++;

  obj->unknown_bits = bit_read_TF(dat, num_bytes);
  LOG_TRACE("unknown_bits [%ld (%lu,%ld,%d) TF]: ",
            bitsize, obj->common_size,
            obj->handle_offset - obj->common_size,
            (int)obj->stringstream_size);
  LOG_TRACE_TF(obj->unknown_bits, num_bytes);
  bit_set_position(dat, pos);
  return 0;
}

#undef IS_DECODER
