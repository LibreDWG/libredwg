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
// from out_json.h
extern int dwg_write_json (Bit_Chain *restrict dat, Dwg_Data *restrict dwg);
// from dwg_api.h
extern int dwg_add_Document (Dwg_Data *restrict dwg, const int imperial);
extern Dwg_Entity_REGION *
dwg_add_REGION (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                const char *acis_data);

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

/* Regression test for GH #1487 (in_json dotted keys into unset arrays):
   a dotted key into a subclass array that is not set (e.g.
   "ref.classname" from example_2004.dwg, "rowstyles.text_height", or
   "dashes.length" with the array removed) now does what an array of one
   element does: the array gets the elements the specs walk,
   zero-filled, and its count is set to 1 (within the other array when
   shared, as in the LTYPE dashes.numdashes case). On master the specs
   and dwg_free walked past the single element. */
static void
in_json_dotted_array_test (void)
{
  const char *src =
      "{\n"
      " \"created_by\": \"libredwg test\",\n"
      " \"FILEHEADER\": { \"version\": \"AC1024\" },\n"
      " \"OBJECTS\": ["
      " { \"object\": \"DIMASSOC\", \"type\": 626,\n"
      "   \"handle\": [0, 1, 1],\n"
      "   \"associativity\": 63,\n"
      "   \"ref.classname\": \"AcDbOsnapPointRef\" },\n"
      " { \"object\": \"TABLESTYLE\", \"type\": 724,\n"
      "   \"handle\": [0, 1, 2], \"name\": \"TestTable\",\n"
      "   \"rowstyles.text_height\": 2.5 },\n"
      " { \"entity\": \"MLINE\", \"type\": 47,\n"
      "   \"handle\": [0, 1, 3], \"layer\": \"0\",\n"
      "   \"verts.lines\": [ {}, {} ] },\n"
      " { \"object\": \"LTYPE\", \"type\": 57,\n"
      "   \"handle\": [0, 1, 4], \"name\": \"TestLTYPE\",\n"
      "   \"numdashes\": 5, \"dashes.length\": 5.0 } ] }";
  char *json = (char *)malloc (strlen (src) + 1);
  Dwg_Data dwg;
  Bit_Chain dat;
  int error;

  if (!json)
    {
      fail ("in_json_dotted_array: malloc failed");
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
      fail ("in_json_dotted_array: dwg_read_json failed 0x%x", error);
      free (json);
      return;
    }
  if (dwg.num_objects != 4)
    fail ("in_json_dotted_array: num_objects %u != 4",
          (unsigned)dwg.num_objects);
  else
    {
      BITCODE_BL i;
      for (i = 0; i < dwg.num_objects; i++)
        {
          switch (dwg.object[i].fixedtype)
            {
            case DWG_TYPE_DIMASSOC:
              {
                Dwg_Object_DIMASSOC *o
                    = dwg.object[i].tio.object->tio.DIMASSOC;
                // the specs walk ref 6 times; 1 element would OOB
                if (!o->ref)
                  fail ("in_json_dotted_array: ref not allocated");
                else
                  {
                    if (!o->ref[0].classname
                        || strcmp (o->ref[0].classname,
                                   "AcDbOsnapPointRef") != 0)
                      fail ("in_json_dotted_array: ref[0].classname not set");
                    if (o->ref[5].classname || o->ref[5].osnap_type
                        || o->ref[5].xrefs)
                      fail ("in_json_dotted_array: ref[5] not zero-filled");
                  }
                break;
              }
            case DWG_TYPE_TABLESTYLE:
              {
                Dwg_Object_TABLESTYLE *o
                    = dwg.object[i].tio.object->tio.TABLESTYLE;
                // the specs walk rowstyles 3 times; 1 element would OOB
                if (!o->rowstyles)
                  fail ("in_json_dotted_array: rowstyles not allocated");
                else
                  {
                    if (o->rowstyles[0].text_height != 2.5)
                      fail ("in_json_dotted_array: rowstyles[0].text_height "
                            "not set");
                    if (o->rowstyles[2].text_height != 0.0
                        || o->rowstyles[2].borders)
                      fail ("in_json_dotted_array: rowstyles[2] not "
                            "zero-filled");
                  }
                if (o->num_rowstyles != 1)
                  fail ("in_json_dotted_array: num_rowstyles %u != 1",
                        (unsigned)o->num_rowstyles);
                break;
              }
            case DWG_TYPE_MLINE:
              {
                Dwg_Entity_MLINE *o
                    = dwg.object[i].tio.entity->tio.MLINE;
                if (o->num_verts != 1)
                  fail ("in_json_dotted_array: num_verts %u != 1",
                        (unsigned)o->num_verts);
                else if (o->verts[0].num_lines != 2 || o->num_lines != 2)
                  fail ("in_json_dotted_array: vertex lines %u != 2",
                        (unsigned)o->verts[0].num_lines);
                break;
              }
            case DWG_TYPE_LTYPE:
              {
                Dwg_Object_LTYPE *o = dwg.object[i].tio.object->tio.LTYPE;
                // the count given before the buffer is lowered by the
                // 1-element array it is now shared with
                if (o->numdashes != 1)
                  fail ("in_json_dotted_array: numdashes %u != 1",
                        (unsigned)o->numdashes);
                if (o->dashes && o->dashes[0].length != 5.0)
                  fail ("in_json_dotted_array: dashes[0].length not set");
                break;
              }
            default:
              break;
            }
        }
    }
  // on master: dwg_free walks the fixed elems past the 1-element
  // arrays
  dwg_free (&dwg);
  free (json);
  ok ("in_json_dotted_array: dotted keys into unset arrays get the "
      "spec-walked elements");
}

// R2010+ TABLE -> pre-R2010 cells: text from the UTF-16LE value, merged cells
// as in AutoCAD's own pre-R2010 tables (top left holds the extent, the
// covered cells are marked merged), column widths and row heights.
static void
downconvert_TABLE_test (void)
{
  Dwg_Data dwg;
  Dwg_Object obj;
  Dwg_Object_Entity ent;
  Dwg_Entity_TABLE table;
  Dwg_TableDataColumn cols[3];
  Dwg_TableRow rows[2];
  Dwg_TableCell row0[3], row1[3];
  Dwg_TableCellContent c00, c11;
  Dwg_FormattedTableMerged merged;
  // "ab" with the terminating NUL
  unsigned char ab[6] = { 'a', 0, 'b', 0, 0, 0 };
  BITCODE_RS *tu;

  memset (&dwg, 0, sizeof dwg);
  memset (&obj, 0, sizeof obj);
  memset (&ent, 0, sizeof ent);
  memset (&table, 0, sizeof table);
  memset (cols, 0, sizeof cols);
  memset (rows, 0, sizeof rows);
  memset (row0, 0, sizeof row0);
  memset (row1, 0, sizeof row1);
  memset (&c00, 0, sizeof c00);
  memset (&c11, 0, sizeof c11);
  dwg.header.from_version = R_2013;
  dwg.header.version = R_2013;
  obj.parent = &dwg;
  obj.fixedtype = DWG_TYPE_TABLE;
  obj.tio.entity = &ent;
  ent.tio.TABLE = &table;
  table.rotation = 0.0;
  table.tdata.num_cols = 3;
  table.tdata.cols = cols;
  table.tdata.num_rows = 2;
  table.tdata.rows = rows;
  cols[0].width = 10.0;
  cols[1].width = 20.0;
  cols[2].width = 30.0;
  rows[0].height = 4.0;
  rows[1].height = 5.0;
  rows[0].num_cells = rows[1].num_cells = 3;
  rows[0].cells = row0;
  rows[1].cells = row1;
  c00.type = 1;
  c00.value.data_type = 4;
  c00.value.data_size = sizeof ab;
  c00.value.data_date = (BITCODE_TF)ab;
  row0[0].num_cell_contents = 1;
  row0[0].cell_contents = &c00;
  c11.type = 1;
  c11.value.data_type = 1;
  c11.value.data_long = 42;
  row1[1].num_cell_contents = 1;
  row1[1].cell_contents = &c11;
  // merge the first row
  merged.top_row = 0;
  merged.left_col = 0;
  merged.bottom_row = 0;
  merged.right_col = 2;
  table.fdata.num_merged_cells = 1;
  table.fdata.merged_cells = &merged;

  downconvert_TABLE (&obj);

  if (table.num_cols != 3 || table.num_rows != 2 || table.num_cells != 6
      || !table.cells || !table.col_widths || !table.row_heights)
    {
      fail ("downconvert_TABLE: no cells");
      return;
    }
  if (table.col_widths[1] != 20.0 || table.row_heights[1] != 5.0)
    fail ("downconvert_TABLE: widths/heights");
  tu = (BITCODE_RS *)table.cells[0].text_value;
  if (!tu || tu[0] != 'a' || tu[1] != 'b' || tu[2] != 0)
    fail ("downconvert_TABLE: cell 0 text is not the UTF-16 \"ab\"");
  tu = (BITCODE_RS *)table.cells[4].text_value;
  if (!tu || tu[0] != '4' || tu[1] != '2' || tu[2] != 0)
    fail ("downconvert_TABLE: number cell text");
  if (table.cells[0].merged_width_flag != 3
      || table.cells[0].merged_height_flag != 1
      || table.cells[0].is_merged_value)
    fail ("downconvert_TABLE: merge master %u x %u merged %d",
          (unsigned)table.cells[0].merged_width_flag,
          (unsigned)table.cells[0].merged_height_flag,
          (int)table.cells[0].is_merged_value);
  if (!table.cells[1].is_merged_value || !table.cells[2].is_merged_value
      || table.cells[3].is_merged_value)
    fail ("downconvert_TABLE: covered cells");
  if (table.cells[5].merged_width_flag != 1
      || table.cells[5].merged_height_flag != 1)
    fail ("downconvert_TABLE: plain cell extent");
  if (table.horiz_direction.x != 1.0 || table.horiz_direction.y != 0.0)
    fail ("downconvert_TABLE: horiz_direction");
  if (table.flag_for_table_value != 0x16)
    fail ("downconvert_TABLE: flag_for_table_value 0x%x",
          (unsigned)table.flag_for_table_value);
  // not twice, and free what was made
  downconvert_TABLE (&obj);
  for (unsigned i = 0; i < table.num_cells; i++)
    free (table.cells[i].text_value);
  free (table.cells);
  free (table.col_widths);
  free (table.row_heights);
  ok ("downconvert_TABLE: text, merged cells and extents");
}

// R2010+ TABLESTYLE cell styles (title 1, header 2, data 3) to the pre-R2010
// rowstyles (0: data, 1: title, 2: header) and back.
static void
downconvert_TABLESTYLE_test (void)
{
  Dwg_Data dwg;
  Dwg_Object obj;
  Dwg_Object_Object oo;
  Dwg_Object_TABLESTYLE ts, back;
  Dwg_TABLESTYLE_CellStyle cs[3];
  Dwg_GridFormat data_borders[2];
  Dwg_GridFormat title_borders[1];
  Dwg_Object_Object oo2;
  Dwg_Object obj2;

  memset (&dwg, 0, sizeof dwg);
  memset (&obj, 0, sizeof obj);
  memset (&oo, 0, sizeof oo);
  memset (&ts, 0, sizeof ts);
  memset (&back, 0, sizeof back);
  memset (cs, 0, sizeof cs);
  memset (data_borders, 0, sizeof data_borders);
  memset (title_borders, 0, sizeof title_borders);
  memset (&oo2, 0, sizeof oo2);
  memset (&obj2, 0, sizeof obj2);
  dwg.header.from_version = R_2018;
  dwg.header.version = R_2018;
  obj.parent = &dwg;
  obj.fixedtype = DWG_TYPE_TABLESTYLE;
  obj.tio.object = &oo;
  oo.tio.TABLESTYLE = &ts;
  ts.sty.cellstyle.margin_override_flags = 1;
  ts.sty.cellstyle.horiz_margin = 1.5;
  ts.sty.cellstyle.vert_margin = 2.0;
  // title: with a background color
  cs[0].id = 1;
  cs[0].cellstyle.content_format.text_height = 6.0;
  cs[0].cellstyle.content_format.cell_alignment = 5;
  cs[0].cellstyle.bg_color.rgb = 0xc2123456;
  cs[0].cellstyle.num_borders = 1;
  cs[0].cellstyle.borders = title_borders;
  title_borders[0].index_mask = 0x20; // inside horizontal
  title_borders[0].linewt = 50;
  // header
  cs[1].id = 2;
  cs[1].cellstyle.content_format.text_height = 4.5;
  cs[1].cellstyle.content_format.cell_alignment = 5;
  cs[1].cellstyle.bg_color.rgb = 0xc8000000; // none
  // data: the top border is invisible, the right border is visible
  cs[2].id = 3;
  cs[2].cellstyle.content_format.text_height = 4.5;
  cs[2].cellstyle.content_format.cell_alignment = 2;
  cs[2].cellstyle.bg_color.rgb = 0xc8000000;
  cs[2].cellstyle.num_borders = 2;
  cs[2].cellstyle.borders = data_borders;
  data_borders[0].index_mask = 1;
  data_borders[0].linewt = 25;
  data_borders[0].visible = 1; // BL 93: invisible
  data_borders[1].index_mask = 2;
  data_borders[1].linewt = 35;
  data_borders[1].visible = 0;
  ts.num_cellstyles = 3;
  ts.cellstyles = cs;

  downconvert_TABLESTYLE (&obj);

  if (ts.num_rowstyles != 3 || !ts.rowstyles)
    {
      fail ("downconvert_TABLESTYLE: no rowstyles");
      return;
    }
  if (ts.rowstyles[0].text_height != 4.5 || ts.rowstyles[1].text_height != 6.0
      || ts.rowstyles[2].text_height != 4.5)
    fail ("downconvert_TABLESTYLE: order data, title, header");
  if (ts.rowstyles[0].text_alignment != 2 || ts.rowstyles[1].text_alignment != 5)
    fail ("downconvert_TABLESTYLE: alignment");
  if (ts.rowstyles[0].has_bgcolor || ts.rowstyles[0].fill_color.index != 256
      || !ts.rowstyles[1].has_bgcolor
      || ts.rowstyles[1].fill_color.rgb != 0xc2123456)
    fail ("downconvert_TABLESTYLE: fill color");
  // pre-R2010 border order: top, horizontal inside, bottom, left,
  // vertical inside, right
  if (ts.rowstyles[0].borders[0].linewt != 25
      || ts.rowstyles[0].borders[0].visible != 0
      || ts.rowstyles[0].borders[5].linewt != 35
      || ts.rowstyles[0].borders[5].visible != 1
      || ts.rowstyles[0].borders[2].linewt != -2
      || ts.rowstyles[0].borders[2].visible != 1)
    fail ("downconvert_TABLESTYLE: data borders");
  if (ts.rowstyles[1].borders[1].linewt != 50)
    fail ("downconvert_TABLESTYLE: title horizontal inside border");
  if (ts.horiz_cell_margin != 1.5 || ts.vert_cell_margin != 2.0)
    fail ("downconvert_TABLESTYLE: margins");

  // and back
  dwg.header.from_version = R_2000;
  dwg.header.version = R_2000;
  obj2.parent = &dwg;
  obj2.fixedtype = DWG_TYPE_TABLESTYLE;
  obj2.tio.object = &oo2;
  oo2.tio.TABLESTYLE = &back;
  back.num_rowstyles = 3;
  back.rowstyles = ts.rowstyles;
  back.horiz_cell_margin = 1.5;
  back.vert_cell_margin = 2.0;
  upconvert_TABLESTYLE (&obj2);
  if (back.num_cellstyles != 3 || !back.cellstyles)
    fail ("upconvert_TABLESTYLE: no cellstyles");
  else
    {
      Dwg_TABLESTYLE_CellStyle *t = &back.cellstyles[0];
      Dwg_TABLESTYLE_CellStyle *h = &back.cellstyles[1];
      Dwg_TABLESTYLE_CellStyle *d = &back.cellstyles[2];
      if (t->id != 1 || h->id != 2 || d->id != 3 || t->type != 1
          || d->type != 2)
        fail ("upconvert_TABLESTYLE: ids %u %u %u", (unsigned)t->id,
              (unsigned)h->id, (unsigned)d->id);
      if (t->cellstyle.merge_flags != 0x8000 || h->cellstyle.merge_flags)
        fail ("upconvert_TABLESTYLE: merge flags");
      if (t->cellstyle.content_format.text_height != 6.0
          || d->cellstyle.content_format.cell_alignment != 2)
        fail ("upconvert_TABLESTYLE: content formats");
      if (t->cellstyle.bg_color.rgb != 0xc2123456
          || d->cellstyle.bg_color.rgb != 0xc8000000)
        fail ("upconvert_TABLESTYLE: background colors");
      // the borders are in the cell style edge flag order 1, 2, 4, 8, ..
      if (d->cellstyle.num_borders != 6
          || d->cellstyle.borders[0].index_mask != 1
          || d->cellstyle.borders[0].linewt != 25
          || d->cellstyle.borders[0].visible != 1 // invisible
          || d->cellstyle.borders[1].index_mask != 2
          || d->cellstyle.borders[1].visible != 0
          || t->cellstyle.borders[5].index_mask != 0x20
          || t->cellstyle.borders[5].linewt != 50)
        fail ("upconvert_TABLESTYLE: borders");
      if (back.sty.cellstyle.vert_margin != 2.0
          || back.sty.cellstyle.horiz_margin != 1.5)
        fail ("upconvert_TABLESTYLE: margins");
    }

  for (unsigned i = 0; i < back.num_cellstyles; i++)
    {
      Dwg_CellStyle *c = &back.cellstyles[i].cellstyle;
      for (unsigned j = 0; j < c->num_borders; j++)
        free (c->borders[j].ltype);
      free (c->borders);
    }
  free (back.sty.cellstyle.content_format.text_style);
  free (back.cellstyles);
  for (unsigned i = 0; i < 3; i++)
    free (ts.rowstyles[i].borders);
  free (ts.rowstyles);
  ok ("downconvert_TABLESTYLE: rowstyles and back");
}

/* The SAB data of ACIS entities is in the AcDs section since r2013. It is
   written from the entities, and read back by the handle, for small data
   directly in the _data_ segment, for large data (> 0x40000) in a blob01
   segment. */
static void
acds_sab_test (void)
{
  const BITCODE_BL sizes[3] = { 5000, 0x40001, 0x90000 };
  const unsigned saved_loglevel = loglevel;
  char msg[160] = "";
  Dwg_Data dwg, dwg2;
  Bit_Chain dat;
  Dwg_Object *mspace;
  unsigned num_regions = 0, found = 0;
  int error;

  memset (&dwg, 0, sizeof dwg);
  memset (&dwg2, 0, sizeof dwg2);
  memset (&dat, 0, sizeof dat);
  dwg.header.version = dwg.header.from_version = R_2018;
  error = dwg_add_Document (&dwg, 0);
  mspace = error ? NULL : dwg_model_space_object (&dwg);
  if (!mspace)
    snprintf (msg, sizeof msg, "no document");
  for (unsigned i = 0; !*msg && i < 3; i++)
    {
      unsigned char *sab = (unsigned char *)malloc (sizes[i]);
      Dwg_Entity_REGION *reg
          = dwg_add_REGION (mspace->tio.object->tio.BLOCK_HEADER, "dummy");
      Dwg_Object *obj = &dwg.object[dwg.num_objects - 1];
      if (!sab || !reg || obj->fixedtype != DWG_TYPE_REGION)
        {
          free (sab);
          snprintf (msg, sizeof msg, "no REGION %u", i);
          break;
        }
      memcpy (sab, "ACIS BinaryFile", 15);
      for (BITCODE_BL j = 15; j < sizes[i]; j++)
        sab[j] = (unsigned char)((j * 131 + i * 7) & 0xff);
      if (dwg_obj_set_sab (obj, sab, sizes[i]))
        {
          free (sab);
          snprintf (msg, sizeof msg, "cannot set the SAB of REGION %u", i);
        }
    }
  if (!*msg)
    {
      dat.version = dat.from_version = R_2018;
      error = dwg_encode (&dwg, &dat);
      if (error >= DWG_ERR_CRITICAL || !dat.chain)
        snprintf (msg, sizeof msg, "dwg_encode error 0x%x", error);
    }
  if (!*msg)
    {
      dat.size = dat.byte;
      dat.byte = 0;
      dat.bit = 0;
      error = dwg_decode (&dat, &dwg2);
      if (error >= DWG_ERR_CRITICAL)
        snprintf (msg, sizeof msg, "dwg_decode error 0x%x", error);
    }
  for (unsigned i = 0; !*msg && i < dwg.num_objects; i++)
    {
      const Dwg_Object *obj = &dwg.object[i];
      const Dwg_Object *o2;
      const unsigned char *d1, *d2;
      BITCODE_BL s1, s2;
      if (obj->fixedtype != DWG_TYPE_REGION
          || !dwg_obj_get_sab (obj, &d1, &s1))
        continue;
      num_regions++;
      o2 = dwg_resolve_handle (&dwg2, obj->handle.value);
      if (!o2 || !dwg_obj_get_sab (o2, &d2, &s2))
        snprintf (msg, sizeof msg, "REGION %lX has no SAB after the roundtrip",
                  (unsigned long)obj->handle.value);
      else if (s1 != s2 || memcmp (d1, d2, s1))
        snprintf (msg, sizeof msg, "REGION %lX SAB %u differs from %u",
                  (unsigned long)obj->handle.value, (unsigned)s2,
                  (unsigned)s1);
      else
        found++;
    }
  if (!*msg && (num_regions != 3 || found != 3))
    snprintf (msg, sizeof msg, "%u of 3 REGIONs, %u found", num_regions, found);
  dwg_free (&dwg);
  dwg_free (&dwg2);
  free (dat.chain);
  // dwg_free reset the loglevel
  loglevel = saved_loglevel;
  if (*msg)
    fail ("acds_sab: %s", msg);
  else
    ok ("acds_sab: SAB of 3 REGIONs, one in a blob, roundtripped via AcDs");
}

/* The AcDs data records which do not belong to an ACIS entity, i.e. the
   thumbnail PNG, are in the JSON AcDs.records, so that a JSON roundtrip
   keeps them. */
static void
acds_records_json_test (void)
{
  const char *src
      = "{\n"
        " \"created_by\": \"libredwg test\",\n"
        " \"FILEHEADER\": { \"version\": \"AC1032\" },\n"
        " \"AcDs\": { \"records\": [\n"
        "   { \"handle\": 34, \"schidx\": 0, \"size\": 8,\n"
        "     \"data\": \"89504E470D0A1A0A\" },\n"
        "   { \"handle\": 1000, \"schidx\": 1, \"size\": 3,\n"
        "     \"data\": \"414243\" } ] },\n"
        " \"OBJECTS\": [] }";
  const unsigned saved_loglevel = loglevel;
  char msg[160] = "";
  char *json = (char *)malloc (strlen (src) + 1);
  char *json2 = NULL;
  Dwg_Data dwg, dwg2;
  Bit_Chain dat, dat2;
  long len = 0;
  int error;

  memset (&dwg, 0, sizeof dwg);
  memset (&dwg2, 0, sizeof dwg2);
  memset (&dat, 0, sizeof dat);
  memset (&dat2, 0, sizeof dat2);
  if (!json)
    {
      fail ("acds_records_json: malloc failed");
      return;
    }
  strcpy (json, src);
  dat.chain = (unsigned char *)json;
  dat.size = strlen (json);
  error = dwg_read_json (&dat, &dwg);
  if (error >= DWG_ERR_CRITICAL)
    snprintf (msg, sizeof msg, "dwg_read_json error 0x%x", error);
  else if (dwg.acds.num_records != 2 || !dwg.acds.records
           || dwg.acds.records[0].handle != 34
           || dwg.acds.records[0].schidx != 0
           || dwg.acds.records[0].size != 8
           || !dwg.acds.records[0].data
           || memcmp (dwg.acds.records[0].data, "\x89PNG\r\n\x1a\n", 8)
           || dwg.acds.records[1].handle != 1000
           || dwg.acds.records[1].size != 3
           || !dwg.acds.records[1].data
           || memcmp (dwg.acds.records[1].data, "ABC", 3))
    snprintf (msg, sizeof msg, "records not read, %u",
              (unsigned)dwg.acds.num_records);
  if (!*msg)
    {
      dat2.fh = tmpfile ();
      dwg.header.version = dwg.header.from_version;
      dat2.version = dwg.header.version;
      dat2.from_version = dwg.header.from_version;
      if (!dat2.fh)
        snprintf (msg, sizeof msg, "no tmpfile");
      else
        {
          error = dwg_write_json (&dat2, &dwg);
          if (error >= DWG_ERR_CRITICAL)
            snprintf (msg, sizeof msg, "dwg_write_json error 0x%x", error);
          else
            {
              len = ftell (dat2.fh);
              rewind (dat2.fh);
              json2 = (char *)malloc (len + 1);
              if (!json2 || fread (json2, 1, len, dat2.fh) != (size_t)len)
                snprintf (msg, sizeof msg, "cannot read the JSON back");
              else
                json2[len] = '\0';
            }
          fclose (dat2.fh);
        }
    }
  if (!*msg)
    {
      memset (&dat, 0, sizeof dat);
      dat.chain = (unsigned char *)json2;
      dat.size = len;
      error = dwg_read_json (&dat, &dwg2);
      if (error >= DWG_ERR_CRITICAL)
        snprintf (msg, sizeof msg, "second dwg_read_json error 0x%x", error);
      else if (dwg2.acds.num_records != 2
               || dwg2.acds.records[0].handle != 34
               || dwg2.acds.records[0].size != 8
               || memcmp (dwg2.acds.records[0].data, "\x89PNG\r\n\x1a\n", 8)
               || dwg2.acds.records[1].handle != 1000
               || dwg2.acds.records[1].schidx != 1
               || memcmp (dwg2.acds.records[1].data, "ABC", 3))
        snprintf (msg, sizeof msg, "records differ after the roundtrip");
    }
  dwg_free (&dwg);
  dwg_free (&dwg2);
  free (json);
  free (json2);
  loglevel = saved_loglevel;
  if (*msg)
    fail ("acds_records_json: %s", msg);
  else
    ok ("acds_records_json: the AcDs thumbnail records roundtrip via JSON");
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
  in_json_dotted_array_test ();
  downconvert_TABLE_test ();
  downconvert_TABLESTYLE_test ();
  acds_sab_test ();
  acds_records_json_test ();
  return failed;
}
