/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2026 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * acds.h: the r2013+ AcDs datastorage section, with the SAB data of the ACIS
 * entities. See acds.c
 */

#ifndef ACDS_H
#define ACDS_H

#include "bits.h"
#include "dwg.h"

/* the schema indices of the constant schema, which AutoCAD writes:
   AcDb_Thumbnail_Schema and AcDb3DSolid_ASM_Data, which is also used for the
   REGION, BODY and surfaces */
#define ACDS_SCHEMA_THUMBNAIL 0
#define ACDS_SCHEMA_ASM_DATA 1

/* A data record to write */
typedef struct
{
  BITCODE_RLL handle;
  BITCODE_RL schidx;
  const unsigned char *data; // not owned
  size_t size;
} AcDs_Rec;

/* Assign the SAB data records to their ACIS entities, by handle. Other records
   are kept in acds->records. */
void acds_assign_sab (Dwg_Data *restrict dwg, const Bit_Chain *restrict dat,
                      Dwg_AcDs *restrict acds);
/* The records to write, sorted. Free it. NULL if none. */
AcDs_Rec *acds_collect_records (const Dwg_Data *restrict dwg,
                                unsigned *restrict pnum);

#endif
