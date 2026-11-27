#define DWG_TYPE DWG_TYPE_MTEXT
static int test_mtext_dwg (int argc, char *argv[]);
#define main test_mtext_dwg
#include "tests_common.c"
#undef main

void
api_process (dwg_object *obj)
{
  int error, isnew;
  double rect_height, rect_width, text_height, extents_height, extents_width,
      linespace_factor;
  BITCODE_BS attachment, flow_dir, linespace_style, class_version;
  BITCODE_B unknown_b0, is_not_annotative, default_flag;
  BITCODE_BL bg_fill_flag, bg_fill_trans;
  BITCODE_BD bg_fill_scale;
  BITCODE_CMC bg_fill_color;
  char *text;
  dwg_point_3d ins_pt, extrusion, x_axis_dir;
  BITCODE_H appid, style;
  BITCODE_BL ignore_attachment;
  BITCODE_BL numfragments;
  BITCODE_BD column_width;
  BITCODE_BD gutter;
  BITCODE_B auto_height;
  BITCODE_B flow_reversed;
  BITCODE_BS column_type;
  BITCODE_BL i, num_column_heights;
  BITCODE_BD *column_heights;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_ent_mtext *mtext = dwg_object_to_MTEXT (obj);

  CHK_ENTITY_UTF8TEXT_W_OLD (mtext, MTEXT, text);
  CHK_ENTITY_3RD (mtext, MTEXT, ins_pt);
  CHK_ENTITY_3RD_W_OLD (mtext, MTEXT, extrusion);
  CHK_ENTITY_3RD_W_OLD (mtext, MTEXT, x_axis_dir);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, rect_width, BD);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, rect_height, BD);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, text_height, BD);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, attachment, BS);
  CHK_ENTITY_MAX (mtext, MTEXT, attachment, BS, 9);
  CHK_ENTITY_TYPE (mtext, MTEXT, flow_dir, BS); // was drawing_dir in dwg_api
  CHK_ENTITY_MAX (mtext, MTEXT, flow_dir, BS, 5);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, extents_height, BD);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, extents_width, BD);
  CHK_ENTITY_H (mtext, MTEXT, style);
  SINCE (R_2000b)
  {
    CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, linespace_style, BS);
    CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, linespace_factor, BD);
    CHK_ENTITY_TYPE (mtext, MTEXT, unknown_b0, B);
  }
  SINCE (R_2004a)
  {
    CHK_ENTITY_TYPE (mtext, MTEXT, bg_fill_flag, BL);
    CHK_ENTITY_MAX (mtext, MTEXT, bg_fill_flag, BL, 31);
    CHK_ENTITY_TYPE (mtext, MTEXT, bg_fill_scale, BD);
    CHK_ENTITY_CMC (mtext, MTEXT, bg_fill_color);
    CHK_ENTITY_TYPE (mtext, MTEXT, bg_fill_trans, BL);
  }
  SINCE (R_2018)
  {
    CHK_ENTITY_TYPE (mtext, MTEXT, is_not_annotative, B);
    CHK_ENTITY_TYPE (mtext, MTEXT, class_version, BS);
    CHK_ENTITY_MAX (mtext, MTEXT, class_version, BS, 10);
    CHK_ENTITY_TYPE (mtext, MTEXT, default_flag, B);
    CHK_ENTITY_H (mtext, MTEXT, appid);
    CHK_ENTITY_TYPE (mtext, MTEXT, column_type, BS);
    CHK_ENTITY_MAX (mtext, MTEXT, column_type, BS, 2);
    CHK_ENTITY_TYPE (mtext, MTEXT, ignore_attachment, BL);
    if ((BITCODE_BL)attachment != ignore_attachment)
      fprintf (stderr,
               "attachment " FORMAT_BS " != ignore_attachment " FORMAT_BL "\n",
               attachment, ignore_attachment);
    CHK_ENTITY_TYPE (mtext, MTEXT, numfragments,
                     BL); // only for column_type == 1
    CHK_ENTITY_TYPE (mtext, MTEXT, num_column_heights, BL);
    if (column_type == 1)
      {
        if (num_column_heights)
          fail ("num_column_heights with column_type == 1");
      }
    else
      {
        if (numfragments)
          fail ("numfragments with column_type != 1");
      }
    CHK_ENTITY_TYPE (mtext, MTEXT, column_width, BD);
    CHK_ENTITY_TYPE (mtext, MTEXT, gutter, BD);
    CHK_ENTITY_TYPE (mtext, MTEXT, auto_height, B);
    CHK_ENTITY_TYPE (mtext, MTEXT, flow_reversed, B);
    CHK_ENTITY_VECTOR_TYPE (mtext, MTEXT, column_heights, num_column_heights,
                            BD);
  }
}

#ifndef DISABLE_DXF
#  include "decode.h"
#  include "in_dxf.h"

static void
check_dxf_height (bool condition, unsigned kind, unsigned placement,
                  const char *what)
{
  if (condition)
    ok ("MTEXT DXF height case%u order%u %s", kind, placement, what);
  else
    fail ("MTEXT DXF height case%u order%u %s", kind, placement, what);
}

static void
check_dxf_mtext (const Dwg_Entity_MTEXT *mtext, unsigned kind,
                 unsigned placement)
{
  const double expected = kind >= 2 ? 8.75 : 0.0;

  check_dxf_height (mtext->rect_height == expected, kind, placement,
                    "defined rectangle height retained");
  /* Case 4 adds an Embedded Object column height.  */
  if (kind == 4)
    {
      check_dxf_height (mtext->num_column_heights == 1 && mtext->column_heights
                            && mtext->column_heights[0] == 3.25,
                        kind, placement,
                        "embedded group 46 kept as column height");
      return;
    }
  check_dxf_height (mtext->num_column_heights == 0, kind, placement,
                    "no column count invented");
  check_dxf_height (!mtext->column_heights, kind, placement,
                    "no column array allocated");
  check_dxf_height (mtext->rect_width == 12.0, kind, placement,
                    "reference width retained");
  check_dxf_height (mtext->text_height == 2.5, kind, placement,
                    "character height retained");
  check_dxf_height (mtext->ins_pt.x == 12.5 && mtext->ins_pt.y == -23.25
                        && mtext->ins_pt.z == 1.5,
                    kind, placement, "insertion point retained");
  check_dxf_height (mtext->x_axis_dir.x == 1.0 && mtext->x_axis_dir.y == 0.0
                        && mtext->x_axis_dir.z == 0.0,
                    kind, placement, "direction retained");
  check_dxf_height (mtext->attachment == 1 && mtext->flow_dir == 1, kind,
                    placement, "attachment and flow retained");
}

static void
test_dxf_height (unsigned kind, unsigned placement)
{
  static const char *const heights[]
      = { "", "46\n0\n", "46\n8.75\n", "46\n2.5\n46\n8.75\n" };
  const bool embedded = kind == 4;
  const char *height = heights[embedded ? 2 : kind];
  const int saved_loglevel = loglevel;
  Dwg_Data dwg = { 0 };
  Bit_Chain dat = { 0 };
  Dwg_Entity_MTEXT *mtext = NULL;
  BITCODE_BL i, count = 0;
  char dxf[512];
  int error;
  /* Pad the synthetic drawing to the importer's 256-byte minimum. */
  const int size = snprintf (
      dxf, sizeof (dxf),
      "0\nSECTION\n2\nHEADER\n9\n$ACADVER\n1\n%s\n"
      "999\nSynthetic MTEXT rectangle-height regression. The group 46 "
      "outside the Embedded Object is the defined rectangle height.\n"
      "0\nENDSEC\n0\nSECTION\n2\nENTITIES\n0\nMTEXT\n5\n40\n"
      "100\nAcDbEntity\n8\n0\n100\nAcDbMText\n"
      "10\n12.5\n20\n-23.25\n30\n1.5\n40\n2.5\n41\n12\n%s"
      "71\n1\n72\n1\n1\nKnown MTEXT height\n"
      "11\n1\n21\n0\n31\n0\n210\n0\n220\n0\n230\n1\n%s%s"
      "0\nENDSEC\n0\nEOF\n",
      embedded ? "AC1032" : "AC1027", placement ? "" : height,
      placement ? height : "",
      embedded ? "101\nEmbedded Object\n46\n3.25\n" : "");
  if (size < 0 || (size_t)size >= sizeof (dxf))
    {
      fail ("MTEXT DXF height case%u order%u: fixture too large", kind,
            placement);
      return;
    }

  dat.chain = (unsigned char *)dxf;
  dat.size = (size_t)size;
  dwg.header.version = R_2013;
  error = dwg_read_dxf (&dat, &dwg);
  loglevel = saved_loglevel;
  check_dxf_height (error < DWG_ERR_CRITICAL, kind, placement,
                    "read completed");
  for (i = 0; i < dwg.num_objects; i++)
    if (dwg.object[i].fixedtype == DWG_TYPE_MTEXT)
      {
        mtext = dwg.object[i].tio.entity->tio.MTEXT;
        count++;
      }
  check_dxf_height (count == 1 && mtext, kind, placement,
                    "one MTEXT retained");
  if (mtext)
    check_dxf_mtext (mtext, kind, placement);
  dwg_free (&dwg);
  /* dwg_free syncs the shared loglevel from dwg.opts.  */
  loglevel = saved_loglevel;
}
#endif

int
main (int argc, char *argv[])
{
  int status = test_mtext_dwg (argc, argv);
#ifndef DISABLE_DXF
  const char *skip = getenv ("LESS_TESTS");
  if (!skip || !atoi (skip))
    {
      const int failed_before = numfailed ();
      unsigned kind, placement;
      for (kind = 0; kind < 4; kind++)
        for (placement = 0; placement < 2; placement++)
          test_dxf_height (kind, placement);
      /* Keep a nonzero embedded column distinct from the ordinary height. */
      test_dxf_height (4, 0);
      if (numfailed () != failed_before)
        return EXIT_FAILURE;
    }
#endif
  return status;
}
