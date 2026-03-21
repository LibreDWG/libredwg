/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2010,2018-2026 Free Software Foundation, Inc.         */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwg_spec_shaed.h: Shared macros and helper functions used by both dwg.spec
   and dwg2.spec
 * written by Reini Urban
 * Extracted from dwg.spec during the spec split.
 */
#ifndef DWG_SPEC_SHARED_H
#define DWG_SPEC_SHARED_H

/**
 * DIMENSION_common declaration
 */
// clang-format off
#define COMMON_ENTITY_DIMENSION                                               \
    SUBCLASS (AcDbDimension)                                                  \
    SINCE (R_2010b)                                                           \
    {                                                                         \
      FIELD_RC (class_version, 280); /* 0=r2010 */                            \
      VALUEOUTOFBOUNDS (class_version, 10)                                    \
    }                                                                         \
    PRE (R_13b1)                                                              \
    {                                                                         \
      FIELD_HANDLE (block, 2, 2);                                             \
      if (dat->version >= R_10) {                                             \
        FIELD_3RD (def_pt, 10);                                               \
      } else { /* ANG2LN */                                                   \
        FIELD_2RD (def_pt, 10);                                               \
      }                                                                       \
      FIELD_2RD (text_midpt, 11);                                             \
      if (R11OPTS (1))                                                        \
        FIELD_2RD (clone_ins_pt, 12);                                         \
      if (R11OPTS (2))                                                        \
        FIELD_RC (flag, 70);                                                  \
      if (R11OPTS (4))                                                        \
        FIELD_TV (user_text, 1);                                              \
    }                                                                         \
    LATER_VERSIONS                                                            \
    {                                                                         \
      DXF                                                                     \
      { /* converted to utf8 */                                               \
        char *blockname = dwg_dim_blockname (dwg, obj);                       \
        VALUE_TV0 (blockname, 2);                                             \
        if (blockname)                                                        \
          free (blockname);                                                   \
        FIELD_3BD (def_pt, 10);                                               \
      }                                                                       \
      else { FIELD_3BD (extrusion, 210); }                                    \
      FIELD_2RD (text_midpt, 11);                                             \
      FIELD_BD (elevation, 31);                                               \
      DXF { FIELD_RC0 (flag, 70); }                                           \
      else { FIELD_RC (flag1, 0); }                                           \
    }                                                                         \
    DECODER                                                                   \
    {                                                                         \
      SINCE (R_13b1) { /* clear the upper flag bits, and fix them: */         \
        BITCODE_RC flag = FIELD_VALUE (flag1) & 0xe0;                         \
        /* bit 7 (non-default) is inverse of bit 0 */                         \
        flag = (FIELD_VALUE (flag1) & 1) ? flag & 0x7F : flag | 0x80;         \
        /* set bit 5 (use block) to bit 1. always set since r13 */            \
        flag = (FIELD_VALUE (flag1) & 2) ? flag | 0x20 : flag & 0xDF;         \
        if (obj->fixedtype == DWG_TYPE_DIMENSION_ALIGNED)                     \
          flag |= 1;                                                          \
        else if (obj->fixedtype == DWG_TYPE_DIMENSION_ANG2LN)                 \
          flag |= 2;                                                          \
        else if (obj->fixedtype == DWG_TYPE_DIMENSION_DIAMETER)               \
          flag |= 3;                                                          \
        else if (obj->fixedtype == DWG_TYPE_DIMENSION_RADIUS)                 \
          flag |= 4;                                                          \
        else if (obj->fixedtype == DWG_TYPE_DIMENSION_ANG3PT)                 \
          flag |= 5;                                                          \
        else if (obj->fixedtype == DWG_TYPE_DIMENSION_ORDINATE)               \
          flag |= 6;                                                          \
        FIELD_VALUE (flag) = flag;                                            \
        LOG_TRACE ("flag => 0x%x [RC 70]\n", flag);                           \
      }                                                                       \
    }                                                                         \
    DXF                                                                       \
    {                                                                         \
      if (dat->from_version >= R_2007)                                        \
        {                                                                     \
          FIELD_T (user_text, 1);                                             \
        }                                                                     \
      else if (_obj->user_text && strlen (_obj->user_text))                   \
        {                                                                     \
          FIELD_TV (user_text, 1);                                            \
        }                                                                     \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      SINCE (R_13b1)                                                          \
      {                                                                       \
        FIELD_T (user_text, 1);                                               \
        FIELD_BD0 (text_rotation, 53);                                        \
        FIELD_BD0 (horiz_dir, 51);                                            \
        FIELD_3BD_1 (ins_scale, 0);                                           \
        FIELD_BD0 (ins_rotation, 54);                                         \
      }                                                                       \
    }                                                                         \
    SINCE (R_2000b)                                                           \
    {                                                                         \
      FIELD_BS (attachment, 71);                                              \
      FIELD_BS1 (lspace_style, 72);                                           \
      FIELD_BD1 (lspace_factor, 41);                                          \
      FIELD_BD (act_measurement, 42);                                         \
    }                                                                         \
    SINCE (R_2007a)                                                           \
    {                                                                         \
      FIELD_B (unknown, 73); /* always 0 */                                   \
      FIELD_B (flip_arrow1, 74);                                              \
      FIELD_B (flip_arrow2, 75);                                              \
    }                                                                         \
    SINCE (R_13b1) {                                                          \
      FIELD_2RD0 (clone_ins_pt, 12);                                          \
    }                                                                         \
    DXF                                                                       \
    {                                                                         \
      FIELD_BD0 (ins_rotation, 54);                                           \
      FIELD_BD0 (horiz_dir, 51);                                              \
      FIELD_BE (extrusion, 210);                                              \
      FIELD_BD0 (text_rotation, 53);                                          \
      FIELD_HANDLE0 (dimstyle, 5, 3);                                         \
    }                                                                         \
    JSON { FIELD_RC (flag, 0); }
// clang-format on

// 37, 38 and 39 are ACIS entities
#define WIRESTRUCT_fields(name)                       \
  SUB_FIELD_RC (name, type, 0);                       \
  SUB_FIELD_BLd (name, selection_marker, 0);          \
  PRE (R_2004a) {                                     \
    FIELD_CAST (name.color, BS, BL, 0);               \
  } else {                                            \
    SUB_FIELD_BL (name, color, 0);                    \
  }                                                   \
  SUB_FIELD_BLd (name, acis_index, 0); /* TODO: align num_points to 255 */                 \
  SUB_FIELD_BL (name, num_points, 0);                 \
  FIELD_3DPOINT_VECTOR (name.points, name.num_points, 0); \
  SUB_FIELD_B (name, transform_present, 0);           \
  if (FIELD_VALUE (name.transform_present))           \
    {                                                 \
      SUB_FIELD_3BD (name, axis_x, 0);                \
      SUB_FIELD_3BD (name, axis_y, 0);                \
      SUB_FIELD_3BD (name, axis_z, 0);                \
      SUB_FIELD_3BD (name, translation, 0);           \
      SUB_FIELD_3BD (name, scale, 0);                 \
      SUB_FIELD_B (name, has_rotation, 0);            \
      SUB_FIELD_B (name, has_reflection, 0);          \
      SUB_FIELD_B (name, has_shear, 0);               \
    }

#if defined(IS_DECODER)

#  define DECODE_3DSOLID                                                      \
    decode_3dsolid (dat, hdl_dat, obj, (Dwg_Entity_3DSOLID *)_obj);           \
    if (FIELD_VALUE (encr_sat_data) && !FIELD_VALUE (encr_sat_data[0]))       \
      FIELD_VALUE (block_size[0]) = 0;

static int
decode_3dsolid (Bit_Chain *dat, Bit_Chain *hdl_dat, Dwg_Object *restrict obj,
                Dwg_Entity_3DSOLID *restrict _obj)
{
  Dwg_Data *dwg = obj->parent;
  BITCODE_BL j;
  BITCODE_BL vcount;
  BITCODE_BL i = 0;
  BITCODE_BL total_size = 0;
  BITCODE_BL num_blocks = 0;
  int acis_data_idx;
  int error = 0;

  FIELD_B (acis_empty, 290);
  if (dat->byte >= dat->size)
    return DWG_ERR_VALUEOUTOFBOUNDS;
  if (!FIELD_VALUE (acis_empty))
    {
      FIELD_B (unknown, 0);
      IF_ENCODE_FROM_EARLIER { FIELD_VALUE (version) = 1; }
      FIELD_BS (version, 70);
      // which is SAT format ACIS 4.0 (since r2000+)
      if (FIELD_VALUE (version) == 1)
        {
          do
            {
              FIELD_VALUE (encr_sat_data) = (char **)realloc (
                  FIELD_VALUE (encr_sat_data), (i + 1) * sizeof (char *));
              FIELD_VALUE (block_size) = (BITCODE_BL *)realloc (
                  FIELD_VALUE (block_size), (i + 1) * sizeof (BITCODE_BL));
              if (!FIELD_VALUE (encr_sat_data) || !FIELD_VALUE (block_size))
                return DWG_ERR_OUTOFMEM;
              FIELD_BL (block_size[i], 0);
              if (FIELD_VALUE (block_size[i]) > 0
                  && AVAIL_BITS (dat) > 8 * FIELD_VALUE (block_size[i]))
                {
                  BITCODE_BL len = FIELD_VALUE (block_size[i]);
                  FIELD_TFv (encr_sat_data[i], len, 1);
                  if (!FIELD_VALUE (encr_sat_data)
                      || !FIELD_VALUE (encr_sat_data[i]))
                    FIELD_VALUE (block_size[i]) = 0;
                  FIELD_VALUE (encr_sat_data[i][len]) = 0;
                  total_size += FIELD_VALUE (block_size[i]);
                }
              else
                {
                  FIELD_VALUE (encr_sat_data[i]) = (char *)calloc (1, 1);
                  FIELD_VALUE (block_size[i]) = 0;
                }
            }
          while (FIELD_VALUE (block_size[i++]) > 0
                 && AVAIL_BITS (dat) >= 16); // crc RS
          if (!FIELD_VALUE (encr_sat_data) || !FIELD_VALUE (block_size))
            {
              free (FIELD_VALUE (block_size));
              free (FIELD_VALUE (encr_sat_data));
              return DWG_ERR_VALUEOUTOFBOUNDS;
            }

          // de-obfuscate SAT data
          FIELD_VALUE (acis_data) = (BITCODE_RC *)malloc (total_size + 1);
          num_blocks = i - 1;
          FIELD_VALUE (num_blocks) = num_blocks;
          LOG_TRACE ("num_blocks: " FORMAT_BL "\n", FIELD_VALUE (num_blocks));
          acis_data_idx = 0;
          for (i = 0; i < num_blocks; i++)
            {
              for (j = 0; j < FIELD_VALUE (block_size[i]); j++)
                {
                  if ((BITCODE_RC)FIELD_VALUE (encr_sat_data[i][j]) <= 32)
                    {
                      FIELD_VALUE (acis_data)
                      [acis_data_idx++]
                          = (BITCODE_RC)FIELD_VALUE (encr_sat_data[i][j]);
                    }
                  else
                    {
                      FIELD_VALUE (acis_data)
                      [acis_data_idx++]
                          = 159
                            - (BITCODE_RC)FIELD_VALUE (encr_sat_data[i][j]);
                    }
                }
            }
          FIELD_VALUE (acis_data)[acis_data_idx] = '\0';
          // DXF 1 + 3 if >255
          LOG_TRACE ("acis_data:\n%s\n", FIELD_VALUE (acis_data));
        }
      else if (FIELD_VALUE (version) == 2)
        /* version 2, SAB: binary, unencrypted SAT format for
           ACIS 7.0/ShapeManager. ACIS versions: R14 release            106
           (ACIS 1.6) R15 (2000) release     400   (ACIS 4.0) R18 (2004)
           release     20800 (ASM ShapeManager, forked from ACIS 7.0) R21
           (2007) release     21200 R24 (2010) release     21500 R27 (2013)
           release     21800 R?? (2018) release            223.0.1.1930
        */
        {
          FIELD_VALUE (block_size)
              = (BITCODE_BL *)calloc (2, sizeof (BITCODE_BL));
          FIELD_VALUE (encr_sat_data) = NULL;
          // TODO string in strhdl, even <r2007
          //  either has_ds_data (r2013+) or the blob is here
          if (!obj->tio.entity->has_ds_data && dat->size > dat->byte + 1)
            {
              char *p;
              // Note that r2013+ has End-of-ASM-data (not ACIS anymore, but
              // their fork)
              const char end[] = "\016\003End\016\002of\016\004ACIS\r\004data";
              const char end1[] = "\016\003End\016\002of\016\003ASM\r\004data";
              size_t pos = dat->byte;
              size_t size = dat->size - pos - 1;
              FIELD_VALUE (acis_data) = (unsigned char *)calloc (size, 1);
              // Binary SAB. unencrypted, documented format until
              // "End-of-ACIS-data"
              // TODO There exist also SAB streams with a given number of
              // records, but I haven't seen them here. See
              // dwg_convert_SAB_to_SAT1 Better read the first header line
              // here, to check for num_records 0. Or even parse the whole SAB
              // format here, and store the SAB different to the ASCII
              // acis_data.
              FIELD_TFF (acis_data, size, 1); // SAB "ACIS BinaryFile"
              LOG_TRACE ("Unknown ACIS 2 SAB sab_size %" PRIuSIZE
                         " starting at %" PRIuSIZE "\n",
                         size, pos);
              if ((p = (char *)memmem (_obj->acis_data, size, end,
                                       strlen (end))))
                {
                  size = p - (char *)_obj->acis_data;
                  size += strlen (end);
                  dat->byte = pos + size;
                  _obj->sab_size = size & 0xFFFFFFFF;
                  LOG_TRACE ("Found End-of-ACIS-data. sab_size: %" PRIuSIZE
                             ", new pos: %" PRIuSIZE "\n",
                             size, dat->byte);
                }
              else if ((p = (char *)memmem (_obj->acis_data, size, end1,
                                            strlen (end1))))
                {
                  size = p - (char *)_obj->acis_data;
                  size += strlen (end1);
                  dat->byte = pos + size;
                  _obj->sab_size = size & 0xFFFFFFFF;
                  LOG_TRACE ("Found End-of-ASM-data. sab_size: %" PRIuSIZE
                             ", new pos: %" PRIuSIZE "\n",
                             size, dat->byte);
                }
              else
                LOG_TRACE ("No End-of-ACIS or ASM data marker found\n");
              _obj->block_size[0] = _obj->sab_size = size & 0xFFFFFFFF;
            }
          else
            LOG_WARN ("SAB from AcDs blob not yet implemented");
          // total_size = FIELD_VALUE (_obj->block_size[0]);
        }
    }
  return error;
}
#else
#  define DECODE_3DSOLID {}
#  define FREE_3DSOLID {}
#endif // #if IS_DECODER

#ifdef IS_ENCODER
#  define ENCODE_3DSOLID                                                      \
    encode_3dsolid (dat, hdl_dat, obj, (Dwg_Entity_3DSOLID *)_obj);
static int
encode_3dsolid (Bit_Chain *dat, Bit_Chain *hdl_dat, Dwg_Object *restrict obj,
                Dwg_Entity_3DSOLID *restrict _obj)
{
  Dwg_Data *dwg = obj->parent;
  BITCODE_BL i = 0;
  BITCODE_BL num_blocks = FIELD_VALUE (num_blocks);
  int acis_data_idx = 0;
  int error = 0;

  FIELD_B (acis_empty, 290);
  if (!FIELD_VALUE (acis_empty))
    {
      FIELD_B (unknown, 0);
      FIELD_BS (version, 70);
      // which is SAT format ACIS 4.0 (since r2000+)
      if (FIELD_VALUE (version) == 1)
        {
          // from decode and indxf we already have all fields.
          // from other importers we have acis_data, but maybe not
          // encr_sat_data.
          if (!FIELD_VALUE (block_size))
            {
              if (!FIELD_VALUE (acis_data))
                {
                  VALUE_RL (0, 0);
                  return error;
                }
              // Later split into 4096 byte sized blocks
              FIELD_VALUE (block_size)
                  = (BITCODE_BL *)calloc (2, sizeof (BITCODE_BL));
              FIELD_VALUE (block_size[0])
                  = strlen ((char *)FIELD_VALUE (acis_data)) & 0xFFFFFFFF;
              FIELD_VALUE (block_size[1]) = 0;
              LOG_TRACE ("default block_size[0] = %d\n",
                         (int)FIELD_VALUE (block_size[0]));
              num_blocks = 1;
            }
          /* insecure. e.g. oss-fuzz issue 32165
             all inputs: dwg, injson and indxf have correct num_blocks values
          else if (!num_blocks)
            {
              num_blocks = 0;
              for (i = 0; FIELD_VALUE (block_size[i]); i++)
                num_blocks++;
            }
          */
          LOG_TRACE ("acis_data:\n%s\n", FIELD_VALUE (acis_data));
          for (i = 0; FIELD_VALUE (block_size[i]) && i < num_blocks; i++)
            {
              if (!FIELD_VALUE (encr_sat_data[i]))
                {
                  if (!FIELD_VALUE (acis_data))
                    {
                      VALUE_RL (0, 0);
                      return error;
                    }
                  // global acis_data_idx is needed for the global acis_data
                  FIELD_VALUE (encr_sat_data[i]) = dwg_encrypt_SAT1 (
                      FIELD_VALUE (block_size[i]), FIELD_VALUE (acis_data),
                      &acis_data_idx);
                  LOG_TRACE ("dwg_encrypt_SAT1 %d\n", i);
                }
              FIELD_BL (block_size[i], 0);
              FIELD_TF (encr_sat_data[i], FIELD_VALUE (block_size[i]), 1);
            }
          /*
          if (num_blocks > FIELD_VALUE (num_blocks))
            {
              FIELD_VALUE (block_size) = (BITCODE_BL*)realloc (FIELD_VALUE
          (block_size), (num_blocks + 1) * sizeof (BITCODE_BL)); FIELD_VALUE
          (num_blocks) = num_blocks;
            }
          */
          FIELD_BL (block_size[num_blocks], 0);
        }
      else // if (FIELD_VALUE (version)==2)
        {
          if (_obj->acis_data && _obj->sab_size)
            {
              LOG_TRACE ("acis_data [TF %u 1]:\n%.*s\n",
                         (unsigned)FIELD_VALUE (sab_size), 15,
                         FIELD_VALUE (acis_data));
              // Binary SAB, unencrypted
              if (obj->tio.entity->has_ds_data)
                {
                  LOG_WARN ("Disable SAB from AcDs blob"); // TODO AcDs support
                  obj->tio.entity->has_ds_data = 0;
                }
              bit_write_TF (dat, _obj->acis_data, _obj->sab_size);
              LOG_TRACE_TF (&_obj->acis_data[15], (int)(_obj->sab_size - 15));
            }
        }
    }
  return error;
}
#else
#  define ENCODE_3DSOLID {}
#  define FREE_3DSOLID {}
#endif // #if IS_ENCODER

#ifdef IS_FREE
#  undef FREE_3DSOLID
#  define FREE_3DSOLID {} free_3dsolid (obj, (Dwg_Entity_3DSOLID *)_obj);
static int
free_3dsolid (Dwg_Object *restrict obj, Dwg_Entity_3DSOLID *restrict _obj)
{
  int error = 0;
  Bit_Chain *dat = &pdat;

  if (!FIELD_VALUE (acis_empty))
    {
      if (FIELD_VALUE (encr_sat_data))
        {
          LOG_HANDLE ("Free %s.num_blocks %u\n", obj->name, _obj->num_blocks)
          for (BITCODE_BL i = 0; i <= FIELD_VALUE (num_blocks); i++)
            {
              if (FIELD_VALUE (encr_sat_data[i]) != NULL)
                FIELD_TF (encr_sat_data[i], block_size[i], 0);
            }
        }
      FREE_IF (FIELD_VALUE (encr_sat_data));
      FREE_IF (FIELD_VALUE (block_size));
    }
  FREE_IF (FIELD_VALUE (acis_data));
  return error;
}
#else
#  define FREE_3DSOLID {}
#endif

#define COMMON_3DSOLID                                                        \
  FIELD_B (wireframe_data_present, 0);                                        \
  if (FIELD_VALUE (wireframe_data_present))                                   \
    {                                                                         \
      FIELD_B (point_present, 0);                                             \
      if (FIELD_VALUE (point_present))                                        \
        {                                                                     \
          FIELD_3BD (point, 0);                                               \
        }                                                                     \
      else                                                                    \
        {                                                                     \
          FIELD_VALUE (point.x) = 0;                                          \
          FIELD_VALUE (point.y) = 0;                                          \
          FIELD_VALUE (point.z) = 0;                                          \
        }                                                                     \
      FIELD_BL (isolines, 0);                                                 \
      FIELD_B (isoline_present, 0);                                           \
      if (FIELD_VALUE (isoline_present))                                      \
        {                                                                     \
          FIELD_BL (num_wires, 0);                                            \
          REPEAT (num_wires, wires, Dwg_3DSOLID_wire)                         \
          REPEAT_BLOCK                                                        \
            WIRESTRUCT_fields (wires[rcount1])                                \
            SET_PARENT (wires[rcount1], (Dwg_Entity__3DSOLID *)_obj);         \
          END_REPEAT_BLOCK                                                    \
          END_REPEAT (wires);                                                 \
          FIELD_BL (num_silhouettes, 0);                                      \
          REPEAT (num_silhouettes, silhouettes, Dwg_3DSOLID_silhouette)       \
          REPEAT_BLOCK                                                        \
            SUB_FIELD_BL (silhouettes[rcount1], vp_id, 0);                    \
            SUB_FIELD_3BD (silhouettes[rcount1], vp_target, 0); /* ?? */      \
            SUB_FIELD_3BD (silhouettes[rcount1], vp_dir_from_target, 0);      \
            SUB_FIELD_3BD (silhouettes[rcount1], vp_up_dir, 0);               \
            SUB_FIELD_B (silhouettes[rcount1], vp_perspective, 0);            \
            SUB_FIELD_B (silhouettes[rcount1], has_wires, 0);                 \
            if (_obj->silhouettes[rcount1].has_wires)                         \
              {                                                               \
                SUB_FIELD_BL (silhouettes[rcount1], num_wires, 0);            \
                REPEAT2 (silhouettes[rcount1].num_wires,                      \
                         silhouettes[rcount1].wires, Dwg_3DSOLID_wire)        \
                REPEAT_BLOCK                                                  \
                  WIRESTRUCT_fields (silhouettes[rcount1].wires[rcount2])     \
                  SET_PARENT (silhouettes[rcount1].wires[rcount2],            \
                              (Dwg_Entity__3DSOLID *)_obj);                   \
                END_REPEAT_BLOCK                                              \
                END_REPEAT (silhouettes[rcount1].wires);                      \
              }                                                               \
            SET_PARENT (silhouettes[rcount1], (Dwg_Entity__3DSOLID *)_obj);   \
          END_REPEAT_BLOCK                                                    \
          END_REPEAT (silhouettes);                                           \
        }                                                                     \
    }                                                                         \
                                                                              \
  FIELD_B (acis_empty_bit, 0); /* ?? */                                       \
  if (FIELD_VALUE (version) > 1)                                              \
    {                                                                         \
      SINCE (R_2007a)                                                         \
      {                                                                       \
        FIELD_BL (num_materials, 0);                                          \
        REPEAT (num_materials, materials, Dwg_3DSOLID_material)               \
        REPEAT_BLOCK                                                          \
          SUB_FIELD_BL (materials[rcount1], array_index, 0);                  \
          SUB_FIELD_BL (materials[rcount1], mat_absref, 0); /* ?? */          \
          SUB_FIELD_HANDLE (materials[rcount1], material_handle, 5, 0);       \
          SET_PARENT (materials[rcount1], (Dwg_Entity__3DSOLID *)_obj);       \
        END_REPEAT_BLOCK                                                      \
        END_REPEAT (materials);                                               \
      }                                                                       \
    }                                                                         \
  SINCE (R_2013b)                                                             \
  {                                                                           \
    FIELD_B (has_revision_guid, 0);                                           \
    DXF {                                                                     \
      if (!_obj->revision_guid[0])                                            \
        {                                                                     \
          VALUE_TFF ("{00000000-0000-0000-0000-000000000000}", 2)             \
          LOG_WARN ("empty revision_guid");                                   \
        }                                                                     \
      else                                                                    \
        {                                                                     \
          FIELD_TFF (revision_guid, 38, 2);                                   \
        }                                                                     \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      FIELD_BL (revision_major, 0);                                           \
      FIELD_BS (revision_minor1, 0);                                          \
      FIELD_BS (revision_minor2, 0);                                          \
      FIELD_TFFx (revision_bytes, 8, 0);                                      \
      DECODER { dxf_3dsolid_revisionguid ((Dwg_Entity_3DSOLID *)_obj); }      \
    }                                                                         \
    FIELD_BL (end_marker, 0);                                                 \
  }                                                                           \
                                                                              \
  DXF {                                                                       \
    SINCE (R_2007a) {                                                         \
      SUBCLASS (AcDb3dSolid);                                                 \
      FIELD_HANDLE (history_id, 4, 350);                                      \
    }                                                                         \
  } else if (FIELD_VALUE (version) > 1                                        \
             && (!IF_IS_DECODER || AVAIL_BITS (hdl_dat) >= 8)) {              \
      FIELD_HANDLE (history_id, 4, 350);                                      \
  }                                                                           \
  FREE { FIELD_HANDLE (history_id, 4, 350); }

#define ACTION_3DSOLID \
  SUBCLASS (AcDbModelerGeometry); \
  DXF_OR_PRINT { \
    DXF_3DSOLID \
  } \
  DECODER { \
    DECODE_3DSOLID \
  } \
  ENCODER { \
    ENCODE_3DSOLID \
  } \
  JSON { \
    JSON_3DSOLID \
  } \
  FREE_3DSOLID \
  COMMON_3DSOLID

#define TABLE_value_fields(value)                                             \
  PRE (R_2007a) { FIELD_VALUE (value.data_type) &= ~0x200; }                  \
  LATER_VERSIONS { FIELD_BL (value.format_flags, 93); }                       \
  FIELD_BL (value.data_type, 90);                                             \
  if (!(dat->version >= R_2007 && FIELD_VALUE (value.format_flags) & 1))      \
    {                                                                         \
      switch (FIELD_VALUE (value.data_type))                                  \
        {                                                                     \
        case 0: /* kUnknown */                                                \
          FIELD_BL (value.data_long, 91);                                     \
          break;                                                              \
        case 1: /* kLong */                                                   \
          FIELD_BL (value.data_long, 91);                                     \
          break;                                                              \
        case 2: /* kDouble */                                                 \
          FIELD_BD (value.data_double, 140);                                  \
          break;                                                              \
        case 4: /* kString */                                                 \
          FIELD_T (value.data_string, 1); /* and 2. TODO multiple lines */    \
          break;                                                              \
        case 8: /* kDate */                                                   \
          FIELD_BL (value.data_size, 92);                                     \
          FIELD_BINARY (value.data_date, FIELD_VALUE (value.data_size), 310); \
          break;                                                              \
        case 16: /* kPoint */                                                 \
          FIELD_2RD (value.data_point, 11);                                   \
          break;                                                              \
        case 32: /* k3dPoint */                                               \
          FIELD_3RD (value.data_3dpoint, 11);                                 \
          break;                                                              \
        case 64: /* kObjectId */                                              \
          FIELD_HANDLE (value.data_handle, -1, 330);                          \
          break;                                                              \
        case 128: /* kBuffer */                                               \
          LOG_ERROR ("Unknown data type in TABLE entity: \"kBuffer\".\n")     \
          break;                                                              \
        case 256: /* kResBuf */                                               \
          LOG_ERROR ("Unknown data type in TABLE entity: \"kResBuf\".\n")     \
          break;                                                              \
        case 512: /* kGeneral since r2007*/                                   \
          SINCE (R_2007a) { FIELD_BL (value.data_size, 0); }                  \
          else                                                                \
          {                                                                   \
            LOG_ERROR (                                                       \
                "Unknown data type in TABLE entity: \"kGeneral before "       \
                "R_2007\".\n")                                                \
          }                                                                   \
          break;                                                              \
        default:                                                              \
          LOG_ERROR ("Invalid data type in TABLE entity\n")                   \
          DEBUG_HERE_OBJ                                                      \
          FIELD_VALUE (value.format_flags) = 0;                               \
          FIELD_VALUE (value.data_type) = 0;                                  \
          error |= DWG_ERR_INVALIDTYPE;                                       \
          break; /*return DWG_ERR_INVALIDTYPE; */                             \
        }                                                                     \
    }                                                                         \
  SINCE (R_2007a)                                                             \
  {                                                                           \
    FIELD_BL (value.unit_type, 94);                                           \
    FIELD_T (value.format_string, 300);                                       \
    if (FIELD_VALUE (value.unit_type) != 12)                                  \
      FIELD_T (value.value_string, 302);                                      \
  }

#ifdef IS_JSON
#  define JSON_END_REPEAT(f)  ENDHASH; ENDARRAY;
#else
#  define JSON_END_REPEAT(f)  END_REPEAT (f)
#endif

// R2010+ Subclass of ATTRIB, ATTDEF, GEOPOSITIONMARKER.
// Similar to MTEXTOBJECTCONTEXTDATA?
// clang-format off
#define AcDbMTextObjectEmbedded_fields                    \
  FIELD_B (is_really_locked, 0);                          \
  DXF                                                     \
  {                                                       \
    FIELD_RD (alignment_pt.x, 11);                        \
    FIELD_RD (alignment_pt.y, 21);                        \
    VALUE_RD (0.0, 31);                                   \
    VALUE_TFF ("Embedded Object", 101);                   \
    FIELD_RD (ins_pt.x, 10);                              \
    FIELD_RD (ins_pt.y, 20);                              \
    VALUE_RD (0.0, 30);                                   \
    SUB_FIELD_BD (mtext, rect_width, 40);                 \
    SUB_FIELD_BD (mtext, rect_height, 41);                \
    SUB_FIELD_BL (mtext, column_type, 71);                \
    SUB_VALUEOUTOFBOUNDS (mtext, column_type, 2)          \
    if (SUB_FIELD_VALUE (mtext, column_type))             \
      {                                                   \
        SUB_FIELD_BL (mtext, num_column_heights, 72);     \
        SUB_FIELD_BD (mtext, column_width, 44);           \
        SUB_FIELD_BD (mtext, gutter, 45);                 \
        SUB_FIELD_B (mtext, auto_height, 73);             \
        SUB_FIELD_B (mtext, flow_reversed, 74);           \
        if (!SUB_FIELD_VALUE (mtext, auto_height)         \
            && SUB_FIELD_VALUE (mtext, column_type) == 2) \
          {                                               \
            SUB_FIELD_VECTOR (mtext, column_heights, BD,  \
                              num_column_heights, 46);    \
          }                                               \
      }                                                   \
    }                                                     \
  else                                                    \
    {                                                     \
      FIELD_BS (annotative_data_size, 70);                \
      if (FIELD_VALUE (annotative_data_size) > 0)         \
        {                                                 \
        /*FIELD_TF (annotative_data, _obj->annotative_data_size, 0);*/  \
        SUB_FIELD_BS (mtext, attachment, 70);             \
        DEBUG_HERE_OBJ                                    \
        SUB_FIELD_3BD (mtext, ins_pt, 10);                \
        SUB_FIELD_3BD (mtext, x_axis_dir, 11);            \
        SUB_FIELD_BD (mtext, rect_height, 41);            \
        SUB_FIELD_BD (mtext, rect_width, 40); /* 1.2: @112 */ \
        /*SUB_FIELD_BD (mtext, extents_width, 42);        \
          SUB_FIELD_BD (mtext, extents_height, 43);*/     \
        SUB_FIELD_BS (mtext, column_type, 71); /* 8: @180 */ \
        SUB_VALUEOUTOFBOUNDS (mtext, column_type, 2)      \
        if (SUB_FIELD_VALUE (mtext, column_type))         \
          {                                                             \
            SUB_FIELD_BS (mtext, num_column_heights, 72); /* 5: @23.5 */\
            SUB_FIELD_BD (mtext, column_width, 44);                     \
            SUB_FIELD_BD (mtext, gutter, 45);                           \
            SUB_FIELD_BS (mtext, auto_height, 73); /* 1: 25.3 */        \
            SUB_FIELD_B (mtext, flow_reversed, 74);                     \
            if (!SUB_FIELD_VALUE (mtext, auto_height)                   \
                && SUB_FIELD_VALUE (mtext, column_type) == 2)           \
              {                                                         \
                SUB_FIELD_VECTOR (mtext, column_heights, BD,            \
                                  num_column_heights, 46);              \
              }                                                         \
            }                                                           \
        FIELD_BS (annotative_flag, 0);                                  \
        }                                                               \
  }
// clang-format on
#endif /* DWG_SPEC_SHARED_H */
