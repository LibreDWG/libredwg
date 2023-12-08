/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2023 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/* convert.c: simple API to read/write any format.
 * Single read, multiple write supported.
 * You should call done() once you're done.
 * 
 * written by Frank van Leeuwen
 */

#include "dwg.h"
#include "decode.h"
#ifndef DISABLE_JSON
#  include "in_json.h"
#  include "out_json.h"
#endif
#ifndef DISABLE_DXF
#  include "in_dxf.h"
#  include "out_dxf.h"
#endif

Dwg_Data dwg;
int readError = DWG_ERR_IOERROR;

EXPORT
void done() {
  if (!readError)
    dwg_free(&dwg);
}

EXPORT
void read(unsigned char* buf, size_t size, const char* type, unsigned char level) {
  Bit_Chain dat = EMPTY_CHAIN (0);
  dat.chain = buf;
  dat.size = size;
  dat.opts = level & DWG_OPTS_LOGLEVEL;
  memset (&dwg, 0, sizeof (Dwg_Data));

  if (!readError)
    done();

  int error = 0;
  if (!strcmp(type, "dwg")) {
    error = dwg_decode (&dat, &dwg);
    if (error)
      dwg_free(&dwg);
  }
#ifndef DISABLE_DXF
  else if (!strcmp(type, "dxf")) {
    error = dwg_read_dxf (&dat, &dwg);
  }
  else if (!strcmp(type, "dxfb")) {
    error = dwg_read_dxfb (&dat, &dwg);
  }
#endif
#ifndef DISABLE_JSON
  else if (!strcmp(type, "json")) {
    error = dwg_read_json (&dat, &dwg);
  }
#endif
  else if (!strcmp(type, "geojson")) {
    error = DWG_ERR_NOTYETSUPPORTED;
  }
  else if (!strcmp(type, "yaml")) {
    error = DWG_ERR_NOTYETSUPPORTED;
  }
  else if (!strcmp(type, "xml")) {
    error = DWG_ERR_NOTYETSUPPORTED;
  }
  else {
    error = DWG_ERR_INVALIDTYPE;
  }
  readError = error;
  return error;
}

EXPORT
void write(unsigned char* buf, size_t size, const char* type, const char* version, int level, bool minimal) {
  Bit_Chain dat = EMPTY_CHAIN (0);
  dat.chain = buf;
  dat.size = size;
  dat.opts = (level & DWG_OPTS_LOGLEVEL) | (minimal ? DWG_OPTS_MINIMAL : 0);

  if (readError)
    return DWG_ERR_IOERROR;
  if (version && *version)
    dwg.header.version = dwg_version_as(version);
    if (dwg.header.version == R_INVALID) {
      return DWG_ERR_INVALIDTYPE;
    }

  int error = 0;
  if (!type) {
    error = DWG_ERR_INVALIDTYPE;
  }
  else if (!strcmp(type, "dwg")) {
    error = dwg_encode(&dat, &dwg);
  }
#ifndef DISABLE_DXF
  else if (!strcmp(type, "dxf")) {
    error = dwg_write_dxf(&dat, &dwg);
  }
  else if (!strcmp(type, "dxfb")) {
    error = dwg_write_dxfb(&dat, &dwg);
  }
#endif
#ifndef DISABLE_JSON
  else if (!strcmp(type, "json")) {
    error = dwg_write_json(&dat, &dwg);
  }
  else if (!strcmp(type, "geojson")) {
    error = dwg_write_geojson(&dat, &dwg);
  }
#endif
  else if (!strcmp(type, "yaml")) {
    error = DWG_ERR_NOTYETSUPPORTED;
  }
  else if (!strcmp(type, "xml")) {
    error = DWG_ERR_NOTYETSUPPORTED;
  }
  else {
    error = DWG_ERR_INVALIDTYPE;
  }
  return error;
}
