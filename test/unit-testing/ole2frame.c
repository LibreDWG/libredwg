#define DWG_TYPE DWG_TYPE_OLE2FRAME
#include "tests_common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS type;         /*!< DXF 71, 1: Link, 2: Embedded, 3: Static */
  BITCODE_BS mode;         /*!< r2000+ DXF 72, tile_mode, 0: mspace, 1: pspace */
  BITCODE_RC lock_aspect;  /*!< r2000+ DXF 73, 0 or 1 (locked) */
  BITCODE_BL data_size;
  BITCODE_BS oleversion;   /*!< DXF 70, always 2 */
  // the 128-byte data header
  BITCODE_RS unknown_rs;   /*!< always 0x80 0x55 */
  BITCODE_3RD pt1;         /*!< DXF 10, upper left corner */
  BITCODE_3RD pt_ur;       /*!< upper right corner */
  BITCODE_3RD pt2;         /*!< DXF 11, lower right corner */
  BITCODE_3RD pt_ll;       /*!< lower left corner */
  BITCODE_RL unknown_rl1;  /*!< some id or hash */
  BITCODE_RL unknown_rl2;  /*!< always 1 */
  BITCODE_RL unknown_rl3;  /*!< always 0x100 */
  BITCODE_RL unknown_rl4;  /*!< 0, 1, 2, 4 */
  BITCODE_RL unknown_rl5;  /*!< 0 or 1 */
  BITCODE_RL unknown_rl6;  /*!< always 0x10000 */
  BITCODE_RS unknown_rs2;  /*!< always 0 */
  BITCODE_RL cfb_size;     /*!< size of the MS-CFB at data[30] */
  BITCODE_TF data;         /*!< DXF 310, the MS-CFB (ole2 stream), without the header */
  // from the data MS-CFB stream
  BITCODE_T oleclient;     /*!< DXF 3, e.g. OLE or Paintbrush Picture */

  Dwg_Version_Type version = obj->parent->header.version;
  dwg_ent_ole2frame *ole2frame = dwg_object_to_OLE2FRAME (obj);

  CHK_ENTITY_TYPE_W_OLD (ole2frame, OLE2FRAME, type, BS);
  CHK_ENTITY_TYPE_W_OLD (ole2frame, OLE2FRAME, mode, BS);
  CHK_ENTITY_TYPE (ole2frame, OLE2FRAME, lock_aspect, RC);

  CHK_ENTITY_TYPE_W_OLD (ole2frame, OLE2FRAME, data_size, BL);
  CHK_ENTITY_TYPE_W_OLD (ole2frame, OLE2FRAME, data, TF);
  CHK_ENTITY_TYPE (ole2frame, OLE2FRAME, unknown_rs, RS);
  CHK_ENTITY_3RD (ole2frame, OLE2FRAME, pt1);
  CHK_ENTITY_3RD (ole2frame, OLE2FRAME, pt2);
  CHK_ENTITY_3RD (ole2frame, OLE2FRAME, pt_ur);
  CHK_ENTITY_3RD (ole2frame, OLE2FRAME, pt_ll);
  CHK_ENTITY_TYPE (ole2frame, OLE2FRAME, unknown_rl1, RL);
  CHK_ENTITY_TYPE (ole2frame, OLE2FRAME, unknown_rl2, RL);
  CHK_ENTITY_TYPE (ole2frame, OLE2FRAME, unknown_rl3, RL);
  CHK_ENTITY_TYPE (ole2frame, OLE2FRAME, unknown_rl4, RL);
  CHK_ENTITY_TYPE (ole2frame, OLE2FRAME, unknown_rl5, RL);
  CHK_ENTITY_TYPE (ole2frame, OLE2FRAME, unknown_rl6, RL);
  CHK_ENTITY_TYPE (ole2frame, OLE2FRAME, unknown_rs2, RS);
  CHK_ENTITY_TYPE (ole2frame, OLE2FRAME, cfb_size, RL);

  CHK_ENTITY_UTF8TEXT (ole2frame, OLE2FRAME, oleclient);
}
