/* Test DXF import independently of DWG encoding.
   Copyright (C) 2026 Free Software Foundation, Inc.
   SPDX-License-Identifier: GPL-3.0-or-later */

#include "config.h"
#include "dwg.h"

#include "tests_common.h"
#include "bits.h"
#include "decode.h"
#include "in_dxf.h"

/* Autodesk's POLYLINE DXF reference, GUID
   ABF6B778-BE20-4B49-9B58-A94E64CEFFF3: group 30 is the 2D elevation
   in OCS; groups 10 and 20 are dummy coordinates, not vertex positions. */
static void
test_polyline_elevation (const char *version, Dwg_Version_Type target,
                         const char *height, double expected, int tilted)
{
  char buf[1024];
  Dwg_Data dwg = { 0 };
  Bit_Chain dat = { 0 };
  unsigned int i, parents = 0, vertices = 0;
  int length, error;
  int legacy = strcmp (version, "AC1009") == 0;
  const char *entity = legacy ? "" : "100\nAcDbEntity\n";
  const char *polyline = legacy ? "" : "100\nAcDb2dPolyline\n";
  const char *vertex = legacy ? "" : "100\nAcDbVertex\n100\nAcDb2dVertex\n";

  length = snprintf (
      buf, sizeof (buf),
      "0\nSECTION\n2\nHEADER\n9\n$ACADVER\n1\n%s\n0\nENDSEC\n"
      "0\nSECTION\n2\nENTITIES\n0\nPOLYLINE\n5\n10\n%s8\n0\n%s66\n1\n"
      "70\n1\n10\n0.0\n20\n0.0\n%s39\n0.75\n40\n0.25\n41\n0.5\n"
      "210\n0.0\n220\n%d\n230\n%d\n"
      "0\nVERTEX\n5\n11\n%s8\n0\n%s70\n0\n"
      "10\n1.25\n20\n2.5\n30\n0.0\n42\n0.125\n"
      "0\nVERTEX\n5\n12\n%s8\n0\n%s70\n0\n"
      "10\n4.0\n20\n5.5\n30\n0.0\n"
      "0\nSEQEND\n5\n13\n8\n0\n0\nENDSEC\n0\nEOF\n",
      version, entity, polyline, height, tilted, !tilted, entity, vertex,
      entity, vertex);
  if (length < 0 || (size_t)length >= sizeof (buf))
    {
      fail ("polyline_elevation: DXF buffer is too small");
      return;
    }
  dat.chain = (unsigned char *)buf;
  dat.size = (size_t)length;
  dwg.header.version = target;
  error = dwg_read_dxf (&dat, &dwg);
  loglevel = 2;
  if (error >= DWG_ERR_CRITICAL)
    {
      fail ("polyline_elevation: %s target %d import failed: 0x%x", version,
            (int)target, error);
      dwg_free (&dwg);
      return;
    }
  for (i = 0; i < dwg.num_objects; i++)
    {
      Dwg_Object *obj = &dwg.object[i];
      if (obj->fixedtype == DWG_TYPE_POLYLINE_2D)
        {
          Dwg_Entity_POLYLINE_2D *p = obj->tio.entity->tio.POLYLINE_2D;
          parents++;
          if (p->elevation != expected)
            fail ("polyline_elevation: %s target %d tilted %d: %.17g != %.17g",
                  version, (int)target, tilted, p->elevation, expected);
          else
            ok ("polyline_elevation: %s target %d tilted %d: %.17g", version,
                (int)target, tilted, expected);
          if (p->flag != 1 || p->start_width != 0.25 || p->end_width != 0.5
              || p->thickness != 0.75 || p->extrusion.x != 0.0
              || p->extrusion.y != tilted || p->extrusion.z != !tilted)
            fail ("polyline_elevation: 2D flags, widths or extrusion changed");
        }
      else if (obj->fixedtype == DWG_TYPE_VERTEX_2D)
        {
          BITCODE_3BD point = obj->tio.entity->tio.VERTEX_2D->point;
          if (point.x != (vertices ? 4.0 : 1.25)
              || point.y != (vertices ? 5.5 : 2.5) || point.z != 0.0)
            fail ("polyline_elevation: %s vertex coordinates changed",
                  version);
          if (!vertices && obj->tio.entity->tio.VERTEX_2D->bulge != 0.125)
            fail ("polyline_elevation: 2D vertex bulge changed");
          vertices++;
        }
    }
  if (parents != 1 || vertices != 2)
    fail ("polyline_elevation: %s expected one 2D parent and two vertices, "
          "got %u and %u",
          version, parents, vertices);
  else
    ok ("polyline_elevation: %s 2D parent and vertices retained", version);
  dwg_free (&dwg);
}

int
main (void)
{
  static const char *versions[] = { "AC1009", "AC1018" };
  static const Dwg_Version_Type targets[] = { R_2004, R_2013 };
  static const struct
  {
    const char *group;
    double value;
  } heights[] = { { "30\n1.5\n", 1.5 },
                  { "30\n-2.5\n", -2.5 },
                  { "30\n0.0\n", 0.0 },
                  { "", 0.0 } };
  size_t v, t, h;
  int tilted;

  loglevel = 2;
  for (v = 0; v < sizeof (versions) / sizeof (versions[0]); v++)
    for (t = 0; t < sizeof (targets) / sizeof (targets[0]); t++)
      for (tilted = 0; tilted <= 1; tilted++)
        {
          for (h = 0; h < sizeof (heights) / sizeof (heights[0]); h++)
            test_polyline_elevation (versions[v], targets[t], heights[h].group,
                                     heights[h].value, tilted);
        }
  printf ("DXF POLYLINE import: %d passed checks, %d failed checks\n",
          numpassed (), numfailed ());
  return numfailed () ? EXIT_FAILURE : EXIT_SUCCESS;
}
