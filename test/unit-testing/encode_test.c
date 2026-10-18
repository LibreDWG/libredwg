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
#ifndef DISABLE_JSON
#  include "in_json.h"
#endif
#include "tests_common.h"

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

#ifndef DISABLE_JSON
/* The in_json R2004+ section readers took a count key after its buffer as
   is, as json_OBJECTS did with num_unknown_bits before 0443a3df. The
   section writers in encode and out_json, and dwg_free_acds, then read
   past the 2-byte buffers, and past segidx[] and segments[]. The counts
   must come from the buffers. */
static void
in_json_section_counts_test (void)
{
  static const char json[]
      = "{\"FILEHEADER\":{\"version\":\"AC1018\"},"
        "\"AppInfo\":{\"unknown_bits\":\"4142\",\"size\":4096},"
        "\"AppInfoHistory\":{\"unknown_bits\":\"4142\",\"size\":4096},"
        "\"Security\":{\"encr_buffer\":\"4142\",\"encr_size\":4096},"
        "\"RevHistory\":{\"histories\":[1],\"num_histories\":4096},"
        "\"AcDs\":{\"segidx\":[{\"offset\":1,\"size\":1},"
        "{\"offset\":1,\"size\":1}],\"segments\":[{\"type\":0}],"
        "\"num_segidx\":1000}}";
  Dwg_Data dwg;
  Bit_Chain dat = { 0 };

  memset (&dwg, 0, sizeof (dwg));
  dat.chain = (unsigned char *)strdup (json);
  if (!dat.chain)
    {
      fail ("in_json_section_counts: strdup failed");
      return;
    }
  dat.size = strlen (json);
  if (dwg_read_json (&dat, &dwg) >= DWG_ERR_CRITICAL)
    fail ("in_json_section_counts: dwg_read_json");
  else
    {
      if (dwg.appinfo.size == 2)
        ok ("json_AppInfo: size from unknown_bits");
      else
        fail ("json_AppInfo: size " FORMAT_RL " != 2", dwg.appinfo.size);
      if (dwg.appinfohistory.size == 2)
        ok ("json_AppInfoHistory: size from unknown_bits");
      else
        fail ("json_AppInfoHistory: size " FORMAT_RL " != 2",
              dwg.appinfohistory.size);
      if (dwg.security.encr_size == 2)
        ok ("json_Security: encr_size from encr_buffer");
      else
        fail ("json_Security: encr_size " FORMAT_RL " != 2",
              dwg.security.encr_size);
      if (dwg.revhistory.num_histories == 1)
        ok ("json_RevHistory: num_histories from histories");
      else
        fail ("json_RevHistory: num_histories " FORMAT_RL " != 1",
              dwg.revhistory.num_histories);
      if (dwg.acds.num_segidx == 1)
        ok ("json_AcDs: num_segidx within segidx and segments");
      else
        fail ("json_AcDs: num_segidx " FORMAT_RL " != 1",
              dwg.acds.num_segidx);
    }
  dwg_free (&dwg);
  free (dat.chain);
}

/* In _set_struct_field only the num_ keys were ignored. The other counts
   and sizes were taken as is, also after their buffer, a vector of fixed
   size in the spec was allocated at the JSON array's length, num_hdls
   counted hdls but not shorts, and numrows * numcols counted exprs and
   lut unchecked. The writers then read past the buffers. The fixed size
   checks of transform and prop_states fail before only with ASan. */
static void
in_json_object_counts_test (void)
{
  static const char json[]
      = "{\"FILEHEADER\":{\"version\":\"AC1018\"},\"OBJECTS\":["
        "{\"object\":\"LTYPE\",\"index\":0,\"type\":57,\"handle\":[0,1,1],"
        "\"dashes\":[{\"length\":1.0}],\"numdashes\":200},"
        "{\"object\":\"DICTIONARY\",\"index\":1,\"type\":42,"
        "\"handle\":[0,1,2],\"items\":{\"A\":[2,1,1,1]},\"numitems\":4096},"
        "{\"entity\":\"LINE\",\"index\":2,\"type\":19,\"handle\":[0,1,3],"
        "\"preview\":\"4142\",\"preview_size\":4096},"
        "{\"object\":\"PROXY_OBJECT\",\"index\":3,\"type\":499,"
        "\"handle\":[0,1,4],\"data_numbits\":4096,\"data\":\"4142\"},"
        "{\"object\":\"FIELD\",\"index\":4,\"type\":500,\"handle\":[0,1,5],"
        "\"value.data_type\":8,\"value.data_date\":\"4142\","
        "\"value.data_size\":4096},"
        "{\"object\":\"SPATIAL_FILTER\",\"index\":5,\"type\":501,"
        "\"handle\":[0,1,6],\"inverse_transform\":[1.0],"
        "\"transform\":[1.0]},"
        "{\"object\":\"BLOCKFLIPPARAMETER\",\"index\":6,\"type\":502,"
        "\"handle\":[0,1,7],\"prop_states\":[1]},"
        "{\"object\":\"BLOCKPOLARSTRETCHACTION\",\"index\":7,\"type\":503,"
        "\"handle\":[0,1,8],\"shorts\":[1],"
        "\"hdls\":[[4,1,1,1],[4,1,2,2]]},"
        "{\"object\":\"BLOCKLOOKUPACTION\",\"index\":8,\"type\":504,"
        "\"handle\":[0,1,9],\"numrows\":2,\"numcols\":2,\"numelems\":4,"
        "\"exprs\":[\"a\"],\"lut\":[{\"b282\":0}]}]}";
  Dwg_Data dwg;
  Bit_Chain dat = { 0 };

  memset (&dwg, 0, sizeof (dwg));
  dat.chain = (unsigned char *)strdup (json);
  if (!dat.chain)
    {
      fail ("in_json_object_counts: strdup failed");
      return;
    }
  dat.size = strlen (json);
  if (dwg_read_json (&dat, &dwg) >= DWG_ERR_CRITICAL || dwg.num_objects != 9)
    fail ("in_json_object_counts: dwg_read_json");
  else
    {
      Dwg_Object_LTYPE *ltype = dwg.object[0].tio.object->tio.LTYPE;
      Dwg_Object_DICTIONARY *dict = dwg.object[1].tio.object->tio.DICTIONARY;
      Dwg_Object_Entity *line = dwg.object[2].tio.entity;
      Dwg_Object_PROXY_OBJECT *proxy
          = dwg.object[3].tio.object->tio.PROXY_OBJECT;
      Dwg_Object_FIELD *field = dwg.object[4].tio.object->tio.FIELD;
      Dwg_Object_SPATIAL_FILTER *filter
          = dwg.object[5].tio.object->tio.SPATIAL_FILTER;
      Dwg_Object_BLOCKFLIPPARAMETER *flip
          = dwg.object[6].tio.object->tio.BLOCKFLIPPARAMETER;
      Dwg_Object_BLOCKPOLARSTRETCHACTION *stretch
          = dwg.object[7].tio.object->tio.BLOCKPOLARSTRETCHACTION;
      Dwg_Object_BLOCKLOOKUPACTION *lookup
          = dwg.object[8].tio.object->tio.BLOCKLOOKUPACTION;

      if (ltype->numdashes == 1)
        ok ("_set_struct_field: LTYPE.numdashes from dashes");
      else
        fail ("_set_struct_field: LTYPE.numdashes %u != 1",
              (unsigned)ltype->numdashes);
      if (dict->numitems == 1)
        ok ("_set_struct_field: DICTIONARY.numitems from items");
      else
        fail ("_set_struct_field: DICTIONARY.numitems " FORMAT_BL " != 1",
              dict->numitems);
      if (line->preview_size == 2)
        ok ("_set_struct_field: preview_size from preview");
      else
        fail ("_set_struct_field: preview_size %u != 2",
              (unsigned)line->preview_size);
      if (proxy->data_numbits == 16)
        ok ("_set_struct_field: PROXY_OBJECT.data_numbits within data");
      else
        fail ("_set_struct_field: PROXY_OBJECT.data_numbits " FORMAT_BL
              " != 16",
              proxy->data_numbits);
      if (field->value.data_size == 2)
        ok ("_set_struct_field: FIELD.value.data_size from data_date");
      else
        fail ("_set_struct_field: FIELD.value.data_size " FORMAT_BL " != 2",
              field->value.data_size);
      if (filter->transform && filter->inverse_transform
          && filter->transform[0] == 1.0 && filter->transform[11] == 0.0
          && filter->inverse_transform[11] == 0.0)
        ok ("_set_struct_field: SPATIAL_FILTER.transform has 12");
      else
        fail ("_set_struct_field: SPATIAL_FILTER.transform");
      if (flip->prop_states && flip->prop_states[0] == 1
          && flip->prop_states[3] == 0)
        ok ("_set_struct_field: BLOCKFLIPPARAMETER.prop_states has 4");
      else
        fail ("_set_struct_field: BLOCKFLIPPARAMETER.prop_states");
      if (stretch->num_hdls == 1)
        ok ("_set_struct_field: BLOCKPOLARSTRETCHACTION.num_hdls within "
            "shorts");
      else
        fail ("_set_struct_field: BLOCKPOLARSTRETCHACTION.num_hdls " FORMAT_BL
              " != 1",
              stretch->num_hdls);
      if (lookup->numelems == 1
          && lookup->numrows * lookup->numcols <= lookup->numelems)
        ok ("_set_struct_field: BLOCKLOOKUPACTION.numelems within exprs");
      else
        fail ("_set_struct_field: BLOCKLOOKUPACTION.numelems " FORMAT_BL
              " numrows " FORMAT_BL " numcols " FORMAT_BL,
              lookup->numelems, lookup->numrows, lookup->numcols);
    }
  dwg_free (&dwg);
  free (dat.chain);
}

/* The specs walk DIMASSOC.ref 6 times, TABLESTYLE.rowstyles 3 times and
   their borders 6 times (REPEAT_CN), whatever the JSON arrays held, but
   _set_struct_field allocated the refs for max (4, the array's length)
   and the rowstyles and borders for the array's length. dwg_free and the
   writers then read past them. The checks of the last elements fail
   before only with ASan. */
static void
in_json_subclass_fixed_elems_test (void)
{
  static const char json[]
      = "{\"FILEHEADER\":{\"version\":\"AC1018\"},\"OBJECTS\":["
        "{\"object\":\"DIMASSOC\",\"index\":0,\"type\":500,"
        "\"handle\":[0,1,1],\"associativity\":1,"
        "\"ref\":[{\"classname\":\"AcDbOsnapPointRef\",\"osnap_type\":1}]},"
        "{\"object\":\"TABLESTYLE\",\"index\":1,\"type\":501,"
        "\"handle\":[0,1,2],\"rowstyles\":[{\"text_height\":1.0,"
        "\"borders\":[{\"linewt\":1,\"visible\":1}]}]}]}";
  Dwg_Data dwg;
  Bit_Chain dat = { 0 };

  memset (&dwg, 0, sizeof (dwg));
  dat.chain = (unsigned char *)strdup (json);
  if (!dat.chain)
    {
      fail ("in_json_subclass_fixed_elems: strdup failed");
      return;
    }
  dat.size = strlen (json);
  if (dwg_read_json (&dat, &dwg) >= DWG_ERR_CRITICAL || dwg.num_objects != 2)
    fail ("in_json_subclass_fixed_elems: dwg_read_json");
  else
    {
      Dwg_Object_DIMASSOC *dimassoc = dwg.object[0].tio.object->tio.DIMASSOC;
      Dwg_Object_TABLESTYLE *style
          = dwg.object[1].tio.object->tio.TABLESTYLE;

      if (dimassoc->ref && dimassoc->ref[0].osnap_type == 1
          && !dimassoc->ref[5].classname && !dimassoc->ref[5].osnap_type)
        ok ("_set_struct_field: DIMASSOC.ref has 6");
      else
        fail ("_set_struct_field: DIMASSOC.ref");
      if (style->rowstyles && style->rowstyles[0].text_height == 1.0
          && style->rowstyles[0].borders
          && style->rowstyles[0].borders[0].linewt == 1
          && style->rowstyles[0].borders[5].linewt == 0
          && style->rowstyles[2].text_height == 0.0
          && !style->rowstyles[2].borders)
        ok ("_set_struct_field: TABLESTYLE.rowstyles has 3, borders 6");
      else
        fail ("_set_struct_field: TABLESTYLE.rowstyles");
    }
  // as dwgwrite, so that dwg_free walks the objects with the R2004 spec
  dwg.header.version = dwg.header.from_version;
  dwg_free (&dwg);
  free (dat.chain);
}

/* dwg.spec MLINE walks MLINE.num_lines lines on every vertex, but
   _set_struct_field set num_lines from each vertex's lines in turn, so it
   ended as the last vertex's count, and allocated each vertex's lines for
   its own count only. dwg_free and the writers then read past the lines
   of an earlier vertex with fewer. The check of the first MLINE fails
   before only with ASan. */
static void
in_json_mline_lines_test (void)
{
  static const char json[]
      = "{\"FILEHEADER\":{\"version\":\"AC1018\"},\"OBJECTS\":["
        "{\"entity\":\"MLINE\",\"index\":0,\"type\":47,\"handle\":[0,1,1],"
        "\"verts\":[{\"lines\":[{\"segparms\":[1.0]}]},"
        "{\"lines\":[{\"segparms\":[1.0]},"
        "{\"segparms\":[2.0],\"areafillparms\":[3.0]}]}]},"
        "{\"entity\":\"MLINE\",\"index\":1,\"type\":47,\"handle\":[0,1,2],"
        "\"verts\":[{\"lines\":[{\"segparms\":[1.0]},{\"segparms\":[2.0]}]},"
        "{\"lines\":[{\"segparms\":[1.0]}]}]}]}";
  Dwg_Data dwg;
  Bit_Chain dat = { 0 };

  memset (&dwg, 0, sizeof (dwg));
  dat.chain = (unsigned char *)strdup (json);
  if (!dat.chain)
    {
      fail ("in_json_mline_lines: strdup failed");
      return;
    }
  dat.size = strlen (json);
  if (dwg_read_json (&dat, &dwg) >= DWG_ERR_CRITICAL || dwg.num_objects != 2)
    fail ("in_json_mline_lines: dwg_read_json");
  else
    {
      Dwg_Entity_MLINE *first = dwg.object[0].tio.entity->tio.MLINE;
      Dwg_Entity_MLINE *last = dwg.object[1].tio.entity->tio.MLINE;

      if (first->num_lines == 2 && first->verts && first->verts[0].lines
          && first->verts[0].lines[0].num_segparms == 1
          && !first->verts[0].lines[1].num_segparms
          && !first->verts[0].lines[1].segparms
          && first->verts[1].lines[1].num_areafillparms == 1)
        ok ("_set_struct_field: MLINE vertex lines grown to num_lines");
      else
        fail ("_set_struct_field: MLINE vertex lines, num_lines %u",
              (unsigned)first->num_lines);
      if (last->num_lines == 2 && last->verts && last->verts[1].lines
          && last->verts[1].lines[0].num_segparms == 1
          && !last->verts[1].lines[1].num_segparms
          && !last->verts[1].lines[1].segparms)
        ok ("_set_struct_field: MLINE.num_lines from the vertex with most");
      else
        fail ("_set_struct_field: MLINE.num_lines %u != 2",
              (unsigned)last->num_lines);
    }
  dwg.header.version = dwg.header.from_version;
  dwg_free (&dwg);
  free (dat.chain);
}
#endif

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
#ifndef DISABLE_JSON
  in_json_section_counts_test ();
  in_json_object_counts_test ();
  in_json_subclass_fixed_elems_test ();
  in_json_mline_lines_test ();
#endif
  return failed;
}
