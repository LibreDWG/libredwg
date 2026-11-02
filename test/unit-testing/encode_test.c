#define ENCODE_TEST_C
#define _DEFAULT_SOURCE 1
#if defined(__linux__)
#  define _GNU_SOURCE 1 /* for memmem on linux */
#endif
#define _BSD_SOURCE 1
#ifdef __STDC_ALLOC_LIB__
#  define __STDC_WANT_LIB_EXT2__ 1 /* for strdup */
#else
#  define _USE_BSD 1
#endif

extern unsigned int loglevel;

#define IS_ENCODER
#include <stdlib.h>
// #include "common.h"
// CLANG_DIAG_IGNORE (-Wpragma-pack)
#include "encode.c"
#include "common.c"
// CLANG_DIAG_RESTORE
#include "decode.h"
#include "tests_common.h"

// from in_json.h
extern int dwg_read_json (Bit_Chain *restrict dat, Dwg_Data *restrict dwg);

BITCODE_RL size = SECTION_R13_SIZE;
// Dwg_Section_Type_r13 section_order[SECTION_R13_SIZE] = { 0 };

static int
find_duplicates (const Dwg_Data *dwg)
{
  for (Dwg_Section_Type_r13 i = 0; (unsigned)i < size; i++)
    {
      Dwg_Section_Type_r13 id = section_order[i];
      for (Dwg_Section_Type_r13 j = 0; (unsigned)j < size; j++)
        {
          if (i == j)
            continue;
          if (section_order[j] == id && (unsigned)id != size)
            {
              fail ("find_duplicate %u", (unsigned)id);
              section_order_trace (dwg, size,
                                   (Dwg_Section_Type_r13 *)&section_order);
              return 1;
            }
        }
    }
  return 0;
}

static void
section_reset (const Dwg_Data *dwg)
{
  memset (section_order, 0, sizeof section_order);
  for (Dwg_Section_Type_r13 i = 0; (unsigned)i < size; i++)
    section_order[i] = i;
  fprintf (stderr, "---\n");
  section_order_trace (dwg, size, (Dwg_Section_Type_r13 *)&section_order);
}

static void
test_section_find (const Dwg_Data *dwg)
{
  unsigned id;
  size = SECTION_R13_SIZE;
  section_reset (dwg);
  for (Dwg_Section_Type_r13 i = 0; (unsigned)i <= size; i++)
    {
      id = section_find ((Dwg_Section_Type_r13 *)&section_order, size, i);
      if (id != (unsigned)i) // 7 not found, returns SECTION_R13_SIZE
        {
          fail ("section_find %u => %u", (unsigned)i, (unsigned)id);
          section_order_trace (dwg, size,
                               (Dwg_Section_Type_r13 *)&section_order);
        }
    }
  id = section_find ((Dwg_Section_Type_r13 *)&section_order, size, size + 1);
  if (id != SECTION_R13_SIZE) // not found
    {
      fail ("section_find %u => %u", size + 1, id);
      section_order_trace (dwg, size, (Dwg_Section_Type_r13 *)&section_order);
    }
}

static void
test_section_move_top (const Dwg_Data *dwg)
{
  int err = 0;
  size = SECTION_R13_SIZE - 1;
  section_reset (dwg);
  for (Dwg_Section_Type_r13 i = 0; (unsigned)i < size - 1; i++)
    {
      // without insert
      if (section_move_top ((Dwg_Section_Type_r13 *)&section_order, &size, i))
        {
          err++;
          fail ("section_move_top existing %u", (unsigned)i);
        }
      section_order_trace (dwg, size, (Dwg_Section_Type_r13 *)&section_order);
      if (section_order[0] != i)
        {
          err++;
          fail ("section_move_top %u not first", (unsigned)i);
        }
      if (find_duplicates (dwg))
        {
          err++;
          fail ("duplicates");
        }
    }
  // with insert
  if (!section_move_top ((Dwg_Section_Type_r13 *)&section_order, &size, size))
    {
      err++;
      fail ("section_move_top insert");
    }
  section_order_trace (dwg, size, (Dwg_Section_Type_r13 *)&section_order);
  if (!err)
    ok ("test_section_move_top");
  else
    fail ("test_section_move_top");
  size = SECTION_R13_SIZE;
}

static inline unsigned
maxrand (unsigned max)
{
  unsigned rnd = (unsigned)rand ();
  return rnd % max;
}

static void
test_section_remove (const Dwg_Data *dwg)
{
  int err = 0;
  int sz = size - 1;
  section_reset (dwg);
  for (unsigned i = 0; i < size; i++)
    {
      Dwg_Section_Type_r13 id = maxrand (size);
      if (section_remove ((Dwg_Section_Type_r13 *)&section_order, &size, id))
        sz--;
      section_order_trace (dwg, size, (Dwg_Section_Type_r13 *)&section_order);
      if (find_duplicates (dwg))
        {
          err++;
          fail ("test_section_remove");
        }
    }
  if (!err)
    ok ("test_section_remove");
  else
    fail ("test_section_remove");
}

static void
test_section_move_before (const Dwg_Data *dwg)
{
  int err = 0;
  size = SECTION_R13_SIZE - 2;
  section_reset (dwg);
  for (unsigned i = 0; i < size; i++)
    {
      Dwg_Section_Type_r13 id = maxrand (size);
      Dwg_Section_Type_r13 before = maxrand (size);
      while (before == id)
        before = maxrand (size);
      if (section_move_before ((Dwg_Section_Type_r13 *)&section_order, &size,
                               id, before))
        {
          fail ("move_before %u", (unsigned)id);
          err++;
        }
      section_order_trace (dwg, size, (Dwg_Section_Type_r13 *)&section_order);
      err += find_duplicates (dwg);
    }
  if (!section_move_before ((Dwg_Section_Type_r13 *)&section_order, &size, 6,
                            4))
    {
      fail ("move_before %u inserts", 6);
      err++;
    }
  err += find_duplicates (dwg);
  if (!err)
    ok ("test_section_move_before");
  else
    fail ("test_section_move_before");
  size = SECTION_R13_SIZE;
}

static void
compress_R2004_section_tests (void)
{
  int result;
  uint32_t comp_data_size;
  Bit_Chain comp = { 0 }, dec = { 0 };
  /* from decode_test.c: decompressed AuxHeader data (123 bytes) */
  unsigned char decomp_auxh_bin[123]
      = { 0xff, 0x88, 0x01, 0x21, 0x00, 0x1d, 0x00, 0x19, 0x00, 0x00, 0x00,
          0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x16, 0x00, 0x2e, 0x00, 0x16, 0x00, 0x2e, 0x00, 0x04, 0x00,
          0x65, 0x05, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x07, 0x00, 0xea, 0x74, 0x25,
          0x00, 0x9a, 0xe6, 0x33, 0x04, 0xb0, 0x82, 0x25, 0x00, 0xe0, 0x1c,
          0xf7, 0x01, 0xe9, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x0d, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00 };
  unsigned char decomp_ofs_bin[53]
      = { 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xb0, 0x82, 0x25,
          0x00, 0xe0, 0x1c, 0xf7, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x32,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 };

  /* Test 1: compress auxh, decompress, compare */
  bit_chain_alloc_size (&comp, sizeof decomp_auxh_bin * 2);
  comp.byte = 0;
  comp.bit = 0;
  result = compress_R2004_section (&comp, decomp_auxh_bin,
                                   sizeof decomp_auxh_bin, &comp_data_size);
  if (result)
    {
      fail ("compress_R2004_section auxh returned %d", result);
      goto test2;
    }
  /* decompress and verify roundtrip */
  comp.byte = 0;
  comp.bit = 0;
  comp.size = comp_data_size;
  bit_chain_alloc_size (&dec, sizeof decomp_auxh_bin);
  dec.size = sizeof decomp_auxh_bin;
  dec.byte = 0;
  dec.bit = 0;
  result = decompress_R2004_section (&comp, &dec);
  if (result == 0 && dec.size == sizeof decomp_auxh_bin
      && memcmp (dec.chain, decomp_auxh_bin, sizeof decomp_auxh_bin) == 0)
    pass ();
  else
    fail ("compress_R2004_section auxh roundtrip %d %lu", result,
          (unsigned long)dec.size);

test2:
  /* Test 2: compress ofs, decompress, compare */
  comp.byte = 0;
  comp.bit = 0;
  if (comp.size < sizeof decomp_ofs_bin * 2)
    bit_chain_alloc_size (&comp, sizeof decomp_ofs_bin * 2);
  result = compress_R2004_section (&comp, decomp_ofs_bin,
                                   sizeof decomp_ofs_bin, &comp_data_size);
  if (result)
    {
      fail ("compress_R2004_section ofs returned %d", result);
      goto cleanup;
    }
  comp.byte = 0;
  comp.bit = 0;
  comp.size = comp_data_size;
  dec.byte = 0;
  dec.bit = 0;
  dec.size = sizeof decomp_ofs_bin;
  result = decompress_R2004_section (&comp, &dec);
  if (result == 0
      && memcmp (dec.chain, decomp_ofs_bin, sizeof decomp_ofs_bin) == 0)
    ok ("compress_R2004_section");
  else
    fail ("compress_R2004_section ofs roundtrip %d %lu", result,
          (unsigned long)dec.size);

cleanup:
  free (comp.chain);
  free (dec.chain);
}

/* Regression test for double-free fix in COMMON_ENTITY_HANDLE_DATA
   (commit 6deac10e).  The macro copies *hdl_dat to dat1 then NULLs
   hdl_dat->chain so dat1 owns the buffer exclusively.  Without the
   NULL, bit_chain_free(hdl_dat) after bit_chain_free(&dat1) would
   double-free. */
static void
common_entity_handle_data_double_free_test (void)
{
  Bit_Chain hdl_dat = { 0 };
  char *buf = (char *)calloc (64, 1);
  if (!buf)
    {
      fail ("common_entity_handle_data: calloc failed");
      return;
    }
  hdl_dat.chain = (unsigned char *)buf;
  hdl_dat.size = 64;
  hdl_dat.byte = 0;
  hdl_dat.bit = 0;

  /* Simulate the macro's copy-then-NULL pattern */
  {
    Bit_Chain dat1 = hdl_dat; /* copy: dat1.chain = hdl_dat.chain */
    hdl_dat.chain = NULL;     /* the fix: transfer ownership */
    bit_chain_free (&dat1);   /* free the buffer through dat1 */
  }
  /* hdl_dat.chain is now NULL — freeing it must be a no-op */
  bit_chain_free (&hdl_dat);
  ok ("common_entity_handle_data double-free regression");
}

/* Regression test for GHSA-6q9w-j39m-jg37 (incomplete fix for GH #357 from
   2022, commit 065a3119). remove_NOD_item's first bounds check
     if (i < numitems && itemhandles[i] != NULL)
   dereferenced _obj->itemhandles[i] without first checking
   _obj->itemhandles != NULL, unlike the second check a few lines below
   which already had the guard. A DICTIONARY with numitems > 0 but a NULL
   itemhandles array (a malformed/edge-case state reachable during encode)
   crashed with a NULL-pointer dereference. */
static void
remove_NOD_item_null_itemhandles_test (void)
{
  Dwg_Object_DICTIONARY obj;

  memset (&obj, 0, sizeof (obj));
  obj.numitems = 1;
  obj.itemhandles = NULL;
  obj.texts = NULL;

  remove_NOD_item (&obj, 0, "ACAD_TEST");
  ok ("remove_NOD_item: survives NULL itemhandles with numitems > 0");
}

/* Regression test for GH #1487 (in_json section counts): a count key
   given apart from the buffer it counts must never exceed the buffer.
   AppInfo.size, AppInfoHistory.size, Security.encr_size,
   RevHistory.num_histories and AcDs.num_segidx are now calculated from
   the buffer/array they count. On master, a large count after the
   buffer made dwg_free_acds walk 4096 entries of the 2-element
   segidx/segments arrays. */
static void
in_json_section_counts_test (void)
{
  const char *src =
      "{\n"
      " \"created_by\": \"libredwg test\",\n"
      " \"AppInfo\": { \"unknown_bits\": \"4142\", \"size\": 4096 },\n"
      " \"AppInfoHistory\": { \"unknown_bits\": \"4142\", \"size\": 4096 },\n"
      " \"RevHistory\": { \"histories\": [100], \"num_histories\": 4096 },\n"
      " \"AcDs\": { \"segidx\": [ { \"offset\": 0, \"size\": 0 },\n"
      "                { \"offset\": 0, \"size\": 0 } ],\n"
      "   \"segments\": [ { \"signature\": 0 },\n"
      "                  { \"signature\": 0 } ],\n"
      "   \"num_segidx\": 4096 } }";
  char *json = (char *)malloc (strlen (src) + 1);
  Dwg_Data dwg;
  Bit_Chain dat;
  int error;

  if (!json)
    {
      fail ("in_json_section_counts: malloc failed");
      return;
    }
  strcpy (json, src);
  memset (&dwg, 0, sizeof (dwg));
  memset (&dat, 0, sizeof (dat));
  dat.chain = (unsigned char *)json;
  dat.size = strlen (json);
  error = dwg_read_json (&dat, &dwg);
  if (error >= DWG_ERR_CRITICAL)
    {
      fail ("in_json_section_counts: dwg_read_json failed 0x%x", error);
      free (json);
      return;
    }
  if (dwg.appinfo.size != 2)
    fail ("in_json_section_counts: AppInfo.size %u != 2",
          (unsigned)dwg.appinfo.size);
  if (dwg.appinfohistory.size != 2)
    fail ("in_json_section_counts: AppInfoHistory.size %u != 2",
          (unsigned)dwg.appinfohistory.size);
  if (dwg.revhistory.num_histories != 1)
    fail ("in_json_section_counts: RevHistory.num_histories %u != 1",
          (unsigned)dwg.revhistory.num_histories);
  if (dwg.acds.num_segidx != 2)
    fail ("in_json_section_counts: AcDs.num_segidx %u != 2",
          (unsigned)dwg.acds.num_segidx);
  // on master: dwg_free_acds walks 4096 segidx/segments entries past the
  // 2-element arrays
  dwg_free (&dwg);
  free (json);
  ok ("in_json_section_counts: section counts stay within their buffers");
}

/* Regression test for GH #1487 (in_json object counts): object
   count/size keys given after the buffer they count are ignored, the
   buffer sets the count; arrays lower a count given before them.
   LTYPE.numdashes follows dashes, DICTIONARY.numitems the items map,
   BLOCK_HEADER.preview_size the preview buffer. On master the large
   count survived and encode/free walked past the short buffers. */
static void
in_json_object_counts_test (void)
{
  const char *src =
      "{\n"
      " \"created_by\": \"libredwg test\",\n"
      " \"OBJECTS\": [ { \"object\": \"LTYPE\", \"type\": 57,\n"
      "   \"handle\": [0, 1, 1], \"name\": \"TestLTYPE\",\n"
      "   \"dashes\": [ { \"length\": 1.0 } ], \"numdashes\": 200 },\n"
      " { \"object\": \"DICTIONARY\", \"type\": 42,\n"
      "   \"handle\": [0, 1, 2], \"name\": \"testdict\",\n"
      "   \"items\": { \"A\": [2, 1, 1, 1], \"B\": [2, 1, 2, 2] },\n"
      "   \"numitems\": 4096 },\n"
      " { \"object\": \"BLOCK_HEADER\", \"type\": 49,\n"
      "   \"handle\": [0, 1, 3], \"name\": \"*Model_Space\",\n"
      "   \"preview\": \"4142\", \"preview_size\": 3000 } ] }";
  char *json = (char *)malloc (strlen (src) + 1);
  Dwg_Data dwg;
  Bit_Chain dat;
  int error;

  if (!json)
    {
      fail ("in_json_object_counts: malloc failed");
      return;
    }
  strcpy (json, src);
  memset (&dwg, 0, sizeof (dwg));
  memset (&dat, 0, sizeof (dat));
  dat.chain = (unsigned char *)json;
  dat.size = strlen (json);
  error = dwg_read_json (&dat, &dwg);
  if (error >= DWG_ERR_CRITICAL)
    {
      fail ("in_json_object_counts: dwg_read_json failed 0x%x", error);
      free (json);
      return;
    }
  if (dwg.num_objects != 3)
    fail ("in_json_object_counts: num_objects %u != 3",
          (unsigned)dwg.num_objects);
  else
    {
      Dwg_Object_LTYPE *lt = dwg.object[0].tio.object->tio.LTYPE;
      Dwg_Object_DICTIONARY *dict
          = dwg.object[1].tio.object->tio.DICTIONARY;
      Dwg_Object_BLOCK_HEADER *bh
          = dwg.object[2].tio.object->tio.BLOCK_HEADER;
      if (lt->numdashes != 1)
        fail ("in_json_object_counts: LTYPE.numdashes %u != 1",
              (unsigned)lt->numdashes);
      if (dict->numitems != 2)
        fail ("in_json_object_counts: DICTIONARY.numitems %u != 2",
              (unsigned)dict->numitems);
      if (bh->preview_size != 2)
        fail ("in_json_object_counts: preview_size %u != 2",
              (unsigned)bh->preview_size);
    }
  dwg_free (&dwg);
  free (json);
  ok ("in_json_object_counts: object counts stay within their buffers");
}

/* Regression test for GH #1487 (in_json subclass fixed elems): subclass
   arrays the specs walk a fixed number of times are allocated at least
   that many elements, zero-filled: DIMASSOC.ref is walked 6 times
   (REPEAT_CN), TABLESTYLE.rowstyles 3 (and free_TABLESTYLE_r2010), their
   borders 6. On master a shorter JSON array left the walkers to read
   past the buffer, and dwg_free freed the pointers found there. */
static void
in_json_subclass_fixed_elems_test (void)
{
  const char *src =
      "{\n"
      " \"created_by\": \"libredwg test\",\n"
      " \"FILEHEADER\": { \"version\": \"AC1024\" },\n"
      " \"OBJECTS\": [ { \"object\": \"DIMASSOC\", \"type\": 626,\n"
      "   \"handle\": [0, 1, 1],\n"
      "   \"associativity\": 63,\n"
      "   \"ref\": [ {}, {}, {}, {} ] },\n"
      " { \"object\": \"TABLESTYLE\", \"type\": 724,\n"
      "   \"handle\": [0, 1, 2], \"name\": \"TestTable\",\n"
      "   \"rowstyles\": [ {} ] } ] }";
  char *json = (char *)malloc (strlen (src) + 1);
  Dwg_Data dwg;
  Bit_Chain dat;
  int error;

  if (!json)
    {
      fail ("in_json_subclass_fixed_elems: malloc failed");
      return;
    }
  strcpy (json, src);
  memset (&dwg, 0, sizeof (dwg));
  memset (&dat, 0, sizeof (dat));
  dat.chain = (unsigned char *)json;
  dat.size = strlen (json);
  error = dwg_read_json (&dat, &dwg);
  if (error >= DWG_ERR_CRITICAL)
    {
      fail ("in_json_subclass_fixed_elems: dwg_read_json failed 0x%x",
            error);
      free (json);
      return;
    }
  if (dwg.num_objects != 2)
    fail ("in_json_subclass_fixed_elems: num_objects %u != 2",
          (unsigned)dwg.num_objects);
  else
    {
      Dwg_Object_DIMASSOC *da = dwg.object[0].tio.object->tio.DIMASSOC;
      Dwg_Object_TABLESTYLE *ts
          = dwg.object[1].tio.object->tio.TABLESTYLE;
      // on master: ref[4]/ref[5] past the 4-element buffer,
      // rowstyles[1]/rowstyles[2] past the 1-element buffer
      if (!da->ref || !ts->rowstyles)
        fail ("in_json_subclass_fixed_elems: ref or rowstyles missing");
      else if (da->ref[4].osnap_type != 0 || da->ref[5].osnap_type != 0
               || da->ref[5].classname != NULL
               || ts->rowstyles[1].text_height != 0.0
               || ts->rowstyles[2].borders != NULL)
        fail ("in_json_subclass_fixed_elems: fixed elems not zero-filled");
    }
  // on master: dwg_free walks 6 DIMASSOC refs and 3 rowstyles x 6 borders
  // past the shorter buffers, and frees the pointers found there
  dwg_free (&dwg);
  free (json);
  ok ("in_json_subclass_fixed_elems: subclass arrays get the walked elems");
}

/* Regression test for GH #1487 (in_json MLINE lines): the spec sets
   every vertex's num_lines to MLINE.num_lines and walks that many lines
   on each vertex. in_json ended MLINE.num_lines at the last vertex's
   count, while each vertex's lines were allocated at its own count, so
   an earlier vertex with fewer lines left encode, out_dxf, out_json
   and dwg_free to read past it. Now MLINE.num_lines is the most lines
   of any vertex and the earlier vertices are grown, zero-filled. */
static void
in_json_mline_lines_test (void)
{
  const char *src =
      "{\n"
      " \"created_by\": \"libredwg test\",\n"
      " \"OBJECTS\": [ { \"entity\": \"MLINE\", \"type\": 47,\n"
      "   \"handle\": [0, 1, 1],\n"
      "   \"verts\": [ { \"lines\": [ {}, {} ] },\n"
      "                { \"lines\": [ {}, {}, {} ] } ] } ] }";
  char *json = (char *)malloc (strlen (src) + 1);
  Dwg_Data dwg;
  Bit_Chain dat;
  int error;

  if (!json)
    {
      fail ("in_json_mline_lines: malloc failed");
      return;
    }
  strcpy (json, src);
  memset (&dwg, 0, sizeof (dwg));
  memset (&dat, 0, sizeof (dat));
  dat.chain = (unsigned char *)json;
  dat.size = strlen (json);
  error = dwg_read_json (&dat, &dwg);
  if (error >= DWG_ERR_CRITICAL)
    {
      fail ("in_json_mline_lines: dwg_read_json failed 0x%x", error);
      free (json);
      return;
    }
  if (dwg.num_objects != 1)
    fail ("in_json_mline_lines: num_objects %u != 1",
          (unsigned)dwg.num_objects);
  else
    {
      Dwg_Entity_MLINE *o = dwg.object[0].tio.entity->tio.MLINE;
      // on master: MLINE.num_lines is the last vertex's count, and
      // verts[0].lines has 2 elements only
      if (o->num_lines != 3)
        fail ("in_json_mline_lines: MLINE.num_lines %u != 3",
              (unsigned)o->num_lines);
      if (o->verts[0].num_lines != 3)
        fail ("in_json_mline_lines: verts[0].num_lines %u != 3",
              (unsigned)o->verts[0].num_lines);
      if (o->verts[0].lines
          && o->verts[0].lines[2].num_segparms != 0)
        fail ("in_json_mline_lines: verts[0].lines[2] not zero-filled");
    }
  // on master: dwg_free walks 3 lines of the first vertex past its
  // 2-element lines array
  dwg_free (&dwg);
  free (json);
  ok ("in_json_mline_lines: every MLINE vertex gets num_lines lines");
}

int
main (int argc, char const *argv[])
{
  Dwg_Data dwg;
  loglevel = is_make_silent () ? 0 : 3;
  dwg.header.version = R_2000;

  test_section_find (&dwg);
  test_section_move_top (&dwg);
  // loglevel = is_make_silent () ? 0 : 3;
  test_section_remove (&dwg);
  test_section_move_before (&dwg);

  compress_R2004_section_tests ();
  common_entity_handle_data_double_free_test ();
  remove_NOD_item_null_itemhandles_test ();
  in_json_section_counts_test ();
  in_json_object_counts_test ();
  in_json_subclass_fixed_elems_test ();
  in_json_mline_lines_test ();
  return failed;
}
