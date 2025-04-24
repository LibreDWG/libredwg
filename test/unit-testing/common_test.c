#define COMMON_TEST_C
#include <stdlib.h>
#include <stddef.h>
#include <locale.h>
#include "tests_common.h"
#include "../../src/codepages.h"
#include "../../src/common.c"
#include "../../programs/escape.c"

static void
common_memmem_tests (void)
{
  static char *hay = (char *)"0123456789";
  char *needle = (char *)"01";
  void *p;

  // positive
  if (my_memmem (hay, 10, needle, 2) != hay)
    fail ("memmem %s not at 0", needle);
  needle = (char *)"1234567890";
  if (my_memmem (hay, 10, needle, 9) != &hay[1])
    fail ("memmem %s not at 0", needle);
  needle = (char *)"789";
  if ((p = my_memmem (hay, 10, needle, 3)) != &hay[7])
    fail ("memmem %s not at 7 but at %p of %p", needle, p, hay);

  // not found
  needle = (char *)"012344567890";
  if (my_memmem (hay, 10, needle, 11))
    fail ("memmem %s found", needle);
  needle = (char *)"1234456780";
  if (my_memmem (hay, 10, needle, 9))
    fail ("memmem %s found", needle);
  needle = (char *)"7890";
  if (my_memmem (hay, 10, needle, 4))
    fail ("memmem %s found", needle);
  else
    ok ("memmem");
}

// test versions:
// strict ordering of dwg_version
// matching enum <=> release type
static void
common_versions_tests (void)
{
  uint8_t prev_version = 0;
  assert (R_AFTER + 1 == ARRAY_SIZE (dwg_versions));
  assert (strEQc (dwg_versions[R_2018].type, "r2018"));
  assert (strEQc (dwg_versions[R_2007].type, "r2007"));
  assert (strEQc (dwg_versions[R_2004].type, "r2004"));
  assert (strEQc (dwg_versions[R_2000].type, "r2000"));
  assert (strEQc (dwg_versions[R_13].type, "r13"));
  assert (strEQc (dwg_versions[R_11].type, "r11"));
  assert (strEQc (dwg_versions[R_2_0].type, "r2.0"));
  assert (strEQc (dwg_versions[R_AFTER].type, "r>2022"));
  assert (!dwg_versions[R_AFTER].hdr);
  if (!failed)
    ok ("dwg_versions messup");

  for (int i = 0; i < R_AFTER; i++)
    {
      const uint8_t dwg_version = dwg_versions[i].dwg_version;
      // strict ordering of dwg_versions
      if (dwg_version < prev_version)
        fail ("dwg_versions[%d].dwg_version %x >= %x", i, dwg_version,
              prev_version);
      prev_version = dwg_version;
    }
  if (!failed)
    ok ("dwg_versions ordered");

  if (!dwg_version_struct (R_AFTER))
    ok ("!dwg_version_struct (R_AFTER)");
  else
    fail ("!dwg_version_struct (R_AFTER)");

  assert (strEQc (dwg_version_codes (R_2018), "AC1032"));
  assert (strEQc (dwg_version_codes (R_2000), "AC1015"));
  assert (dwg_version_as ("r2000") == R_2000);
  assert (dwg_version_as ("r2018") == R_2018);

  assert (dwg_version_hdr_type ("AC1032") == R_2018);
  assert (dwg_version_hdr_type ("AC1015") == R_2000);
  assert (dwg_version_hdr_type2 ("AC1500", 0x16) == R_2000b);
  assert (dwg_version_hdr_type2 ("AC1016", 0x17) == R_2000i);
  assert (dwg_version_hdr_type2 ("AC1017", 0x17) == R_2002);
  assert (dwg_version_hdr_type2 ("AC1018", 0x18) == R_2004c);
  assert (dwg_version_hdr_type2 ("AC402b", 0) == R_2004b);
  assert (dwg_version_hdr_type2 ("AC1018", 0x19) == R_2004);
  assert (dwg_version_hdr_type2 ("AC1019", 0x18) == R_INVALID);
  assert (dwg_version_hdr_type2 ("AC1021", 0x1a) == R_2007b);
  assert (dwg_version_hdr_type2 ("AC1021", 0x1b) == R_2007);
  assert (dwg_version_hdr_type ("AC1018") == R_2004);

  ok ("versions");
}

// basic sanity: no illegal struct tm fields
static void
common_cvt_TIMEBLL_tests (void)
{
  const unsigned long maxtries = 10000000LU;
  static struct tm tm = { 0 };
  static BITCODE_TIMEBLL date = { 0U, 0U, 0.0 };
  int g_failed = failed;
  failed = 0;
  setlocale (LC_TIME, "en_UK.utf8");

  date.days = 2456795;
  date.ms = 18527023;
  cvt_TIMEBLL (&tm, date);
  if (tm.tm_year + 1900 != 2014)
    fail ("tm.tm_year %d != 2014 with 2456795.18527023", tm.tm_year + 1900);

  // check over- and underflows of each field
  for (unsigned long i = 0; i < maxtries; i++)
    {
      time_t time;
      struct tm tm1;
      char buf[30];
      char buf1[30];

      date.days = rand ();
      date.ms = rand ();
      if (sizeof (long) > sizeof (int))
        {
          date.days |= (uint64_t)rand () << 32;
          date.ms |= (uint64_t)rand () << 32;
        }
      // 2020 is the latest possible year for these tests
      if (date.days > 2459191)
        date.days %= 2459191;
      // 1970 is the oldest possible year for gmtime cross-checks
      if (date.days < 25567)
        date.days += 25567;
      if (date.ms > 24 * 60 * 60 * 1000)
        date.ms %= 24 * 60 * 60 * 1000;

      cvt_TIMEBLL (&tm, date);

      if (tm.tm_mon < 0 || tm.tm_mon > 11)
        fail ("tm.tm_mon %d [0-11] with %u.%u", tm.tm_mon, date.days, date.ms);
      if (tm.tm_mday < 1 || tm.tm_mday > 31)
        fail ("tm.tm_mday %d [1-31] with %u.%u", tm.tm_mday, date.days,
              date.ms);
      if (tm.tm_hour < 0 || tm.tm_hour > 23)
        fail ("tm.tm_hour %d [0-23] with %u.%u", tm.tm_hour, date.days,
              date.ms);
      if (tm.tm_min < 0 || tm.tm_min > 60)
        fail ("tm.tm_min %d [0-60] with %u.%u", tm.tm_min, date.days, date.ms);
      if (tm.tm_sec < 0 || tm.tm_sec > 60)
        fail ("tm.tm_sec %d [0-60] with %u.%u", tm.tm_sec, date.days, date.ms);

#if 0
    // and compare against UTC time
    time = 3600 * (date.days - 25567); // 1970 - 1900 in days => seconds since 1970
    time += (date.ms / 1000);
    tm1 = *gmtime(&time);
    strftime (buf, sizeof (buf), "%F %X", &tm);
    strftime (buf1, sizeof (buf1), "%F %X", &tm1);

    if (tm.tm_year != tm1.tm_year)
      fail ("tm.tm_year %d != %d with %u.%u\n    %s vs %s", tm.tm_year + 1900,
            tm1.tm_year + 1900, date.days, date.ms, buf, buf1);
    if (tm.tm_mon != tm1.tm_mon)
      fail ("tm.tm_mon %d != %d", tm.tm_mon, tm1.tm_mon);
    if (tm.tm_mday != tm1.tm_mday)
      fail ("tm.tm_mday %d != %d", tm.tm_mday, tm1.tm_mday);
    if (tm.tm_hour != tm1.tm_hour)
      fail ("tm.tm_hour %d != %d", tm.tm_hour, tm1.tm_hour);
    if (tm.tm_min != tm1.tm_min)
      fail ("tm.tm_mon %d != %d", tm.tm_mon, tm1.tm_mon);
    if (tm.tm_sec != tm1.tm_sec)
      fail ("tm.tm_sec %d != %d", tm.tm_sec, tm1.tm_sec);
#endif

      if (failed)
        break;
    }

  failed += g_failed;
  if (g_failed == failed)
    ok ("cvt_TIMEBLL");
}

static void
dwg_find_color_index_tests (void)
{
  BITCODE_BL rgb;
  BITCODE_BS ret;

#define test_rgb_case(_rgb, _ret)                                             \
  rgb = _rgb;                                                                 \
  ret = dwg_find_color_index (rgb);                                           \
  if (ret == _ret)                                                            \
    ok ("dwg_find_color_index (0x%0x) -> %u", rgb, ret);                      \
  else                                                                        \
    fail ("dwg_find_color_index (0x%0x) -> %u != %u", rgb, ret, _ret);

  test_rgb_case (0xabff0000, 1);
  test_rgb_case (0xab00ff00, 3);
  test_rgb_case (0xab0000ff, 5);
  test_rgb_case (0xabffffff, 7);
  test_rgb_case (0xabbebebe, 254);
  test_rgb_case (0xab000007, 256);
}

static void
escape_htmlescape_tests (void)
{
  char *s = htmlescape ("'test'&{}", CP_ISO_8859_1); // forces a realloc
  if (strEQc (s, "&#39;test&#39;&amp;&#123;&#125;"))
    pass ();
  else
    fail ("htmlescape => %s", s);
  free (s);

  // to multi-byte
  s = htmlescape ("'%test'&{}", CP_CP864);
  if (strEQc (s, "&#39;&#x66A;test&#39;&amp;&#123;&#125;"))
    pass ();
  else
    fail ("htmlescape CP864 => %s", s);
  free (s);

  // from multi-byte: echo -n "시험" | iconv -f utf-8 -t cp949 | od -t x1
  // to: echo -n "시험" | iconv -f utf-8 -t ucs-2 | od -t x2
  s = htmlescape ("'\xbc\xc3\xc7\xe8", CP_CP949); // "시험"
  if (strEQc (s, "&#39;&#xC14C;&#xD5D8;"))
    pass ();
  else
    fail ("htmlescape CP949 => %s", s);
  free (s);
}

static void
escape_htmlwescape_tests (void)
{
  uint16_t tu[] = { 'T', 'e', 'i', 'g', 'h', 'a', 0x2122, 0 };
  char *s = htmlwescape (tu);
  if (strEQc (s, "Teigha&#x2122;"))
    pass ();
  else
    fail ("htmlwescape => %s", s);
  free (s);
}

int
main (int argc, char const *argv[])
{
  loglevel = loglevel_from_env ();

  common_memmem_tests ();
  common_versions_tests ();
  common_cvt_TIMEBLL_tests ();
  dwg_find_color_index_tests ();
  escape_htmlescape_tests ();
  escape_htmlwescape_tests ();

  return failed;
}
