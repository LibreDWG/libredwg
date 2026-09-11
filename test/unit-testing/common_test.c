#define COMMON_TEST_C
#include <stdlib.h>
#include <stddef.h>
#include <locale.h>
#include <assert.h>
#include "../../src/codepages.h"
#include "../../src/bits.h"
#include "../../src/common.c"
#include "../../programs/escape.c"

extern unsigned int loglevel;

#include "tests_common.h"

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
common_strcasecmp_tests (void)
{
  // equal
  if (my_strcasecmp ("abc", "abc") != 0)
    fail ("strcasecmp abc == abc");
  if (my_strcasecmp ("ABC", "abc") != 0)
    fail ("strcasecmp ABC == abc");
  if (my_strcasecmp ("abc", "ABC") != 0)
    fail ("strcasecmp abc == ABC");
  if (my_strcasecmp ("AbC", "aBc") != 0)
    fail ("strcasecmp AbC == aBc");
  if (my_strcasecmp ("", "") != 0)
    fail ("strcasecmp empty == empty");

  // less-than (negative result)
  if (my_strcasecmp ("abc", "abd") >= 0)
    fail ("strcasecmp abc < abd");
  if (my_strcasecmp ("abc", "abcd") >= 0)
    fail ("strcasecmp abc < abcd");
  if (my_strcasecmp ("", "a") >= 0)
    fail ("strcasecmp empty < a");

  // greater-than (positive result)
  if (my_strcasecmp ("abd", "abc") <= 0)
    fail ("strcasecmp abd > abc");
  if (my_strcasecmp ("abcd", "abc") <= 0)
    fail ("strcasecmp abcd > abc");
  if (my_strcasecmp ("a", "") <= 0)
    fail ("strcasecmp a > empty");

  if (!failed)
    ok ("my_strcasecmp");
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

static void
escape_htmlutf8escape_tests (void)
{
  char malformed[] = "ok\xC3\x28";
  char *s = htmlutf8escape ("Caff\xC3\xA8 \xE4\xB8\x96\xE7\x95\x8C & <");
  if (strEQc (s, "Caff\xC3\xA8 \xE4\xB8\x96\xE7\x95\x8C &amp; &lt;"))
    pass ();
  else
    fail ("htmlutf8escape Unicode/XML => %s", s);
  free (s);

  s = htmlutf8escape (malformed);
  if (strEQc (s, "ok&#xFFFD;("))
    pass ();
  else
    fail ("htmlutf8escape malformed UTF-8 => %s", s);
  free (s);
}

static void
mtext_escape_line_tests (void)
{
  static const char *const cases[] = { "\\U+",
                                       "\\U+1",
                                       "\\U+12G4tail",
                                       "\\U+0041",
                                       "\\U+D800",
                                       "\\U+0001",
                                       "\\U+FFFE",
                                       "\\U+FFFF",
                                       "Caff\xC3\xA8 \\U+4E16 & <",
                                       "Hello\xC2\xA0World" };
  static const char *const expected[] = { "\\U+",
                                          "\\U+1",
                                          "\\U+12G4tail",
                                          "&#x41;",
                                          "\\U+D800",
                                          "\\U+0001",
                                          "\\U+FFFE",
                                          "\\U+FFFF",
                                          "Caff\xC3\xA8 &#x4E16; &amp; &lt;",
                                          "Hello\xC2\xA0World" };
  size_t i;

  for (i = 0; i < sizeof (cases) / sizeof (cases[0]); i++)
    {
      char *s = mtext_escape_line (cases[i]);
      if (s && strcmp (s, expected[i]) == 0)
        pass ();
      else
        fail ("mtext_escape_line[%" PRIuSIZE "] => %s", i, s ? s : "(null)");
      free (s);
    }
}

static void
mtext_escape_line_many_unicode_tests (void)
{
  char input[7 * 128 + 1];
  char expected[6 * 128 + 1];
  char *s;
  size_t i;

  for (i = 0; i < 128; i++)
    {
      memcpy (input + i * 7, "\\U+0041", 7);
      memcpy (expected + i * 6, "&#x41;", 6);
    }
  input[sizeof (input) - 1] = '\0';
  expected[sizeof (expected) - 1] = '\0';
  s = mtext_escape_line (input);
  if (s && strcmp (s, expected) == 0)
    pass ();
  else
    fail ("mtext_escape_line repeated Unicode => %s", s ? s : "(null)");
  free (s);
}

static void
mtext_stacked_unicode_tests (void)
{
  char *plain;
  char *escaped;

  plain = mtext_plaintext ("\\S\\U+0041/2;");
  escaped = mtext_escape_line (plain);
  if (plain && escaped && strcmp (plain, "\\U+0041/2") == 0
      && strcmp (escaped, "&#x41;/2") == 0)
    pass ();
  else
    fail ("mtext stacked Unicode => %s / %s", plain ? plain : "(null)",
          escaped ? escaped : "(null)");
  free (escaped);
  free (plain);
}

static void
mtext_plaintext_tests (void)
{
  static const struct
  {
    const char *input;
    const char *expected;
  } cases[] = {
    { "plain", "plain" },
    { "one\\Ptwo\\\\three\\~four", "one\ntwo\\three\xC2\xA0"
                                   "four" },
    { "Hello\\~World", "Hello\xC2\xA0World" },
    { "\\Lunder\\l \\Oover\\o \\Kstrike\\k", "under over strike" },
    { "\\A1;A\\C256;C\\FArial|b1;F\\H1.5x;H\\Q15;Q\\T0.8;T\\W2;W\\pql;P",
      "ACFHQTWP" },
    { "\\pxi-2,l2;ok\\pbroken;tail", "okbroken;tail" },
    { "\\pxsm1,ql;ok\\pxt1;x\\pxql;y\\pqc;\\pq*;z", "okxyz" },
    { "one\\Xtwo\\X;three", "one\ntwo\nthree" },
    { "{outer {inner} \\{left\\}right}", "outer inner {left}right" },
    { "\\S1/2; \\S3#4; \\S5^ 6;", "1/2 3/4 5/6" },
    { "\\Sleft/right\\;more;", "left/right;more" },
    { "\\S1/2 tail", "1/2 tail" },
    { "\\Splain;tail", "plain;tail" },
    { "\\Hbad;text\\H1.5tail\\C12;ok\\pbroken tail",
      "bad;text1.5tailokbroken tail" },
    { "\\A?;a\\C?;c\\F;f\\H?;h\\Q?;q\\T?;t\\W?;w\\p;P",
      "?;a?;c;f?;h?;q?;t?;w;P" },
    { "\\A1tail \\C2tail \\FArial \\H1.5x \\Q0 \\T1 \\W1 \\pql",
      "1tail 2tail Arial 1.5x 0 1 1 ql" },
    { "before\\Zafter\\?mark\\", "beforeafter?mark" },
    { "a\\P\\L\\O\\Kb", "a\nb" },
    { "Caff\xC3\xA8 \\U+4E16 \\U+0041", "Caff\xC3\xA8 \\U+4E16 \\U+0041" },
    { "\\U+12tail \\U+12G4end", "U+12tail U+12G4end" },
  };
  size_t i;

  for (i = 0; i < sizeof (cases) / sizeof (cases[0]); i++)
    {
      char *s = mtext_plaintext (cases[i].input);
      if (s && strcmp (s, cases[i].expected) == 0)
        pass ();
      else
        fail ("mtext_plaintext[%" PRIuSIZE "] => %s", i, s ? s : "(null)");
      free (s);
    }
}

static void
mtext_normalization_tests (void)
{
  uint16_t tu[] = { 'C', 'a', 'f', 'f', 0x00E8, ' ', 0x4E16, 0x754C, 0 };
  char tv[] = "Caff\xC3\xA8 \xE4\xB8\x96\xE7\x95\x8C";
  char *u8;

  u8 = bit_convert_TU (tu);
  if (u8 && strcmp (u8, tv) == 0)
    pass ();
  else
    fail ("TU normalization => %s", u8 ? u8 : "(null)");
  free (u8);
  u8 = bit_TV_to_utf8 (tv, CP_UTF8);
  if (u8 == tv && strcmp (u8, tv) == 0)
    pass ();
  else
    fail ("TV normalization/alias => %s", u8 ? u8 : "(null)");
  if (u8 != tv)
    free (u8);
}

static void
mtext_wrap_text_tests (void)
{
  static const struct
  {
    const char *input;
    double rect_width;
    double text_height;
    double width_factor;
    const char *expected;
  } cases[] = {
    { "one two", 0.0, 10.0, 1.0, "one two" },
    { "one two", (double)NAN, 10.0, 1.0, "one two" },
    { "one two", -1.0, 10.0, 1.0, "one two" },
    { "one two", 100.0, 10.0, 1.0, "one two" },
    { "one two", 18.0, 10.0, 1.0, "one\ntwo" },
    { "one two\nthree four", 30.0, 10.0, 1.0, "one\ntwo\nthree\nfour" },
    { "one\n\ntwo", 100.0, 10.0, 1.0, "one\n\ntwo" },
    { "abcdef", 12.0, 10.0, 1.0, "ab\ncd\nef" },
    { "\xC3\xA8\xE4\xB8\x96", 6.0, 10.0, 1.0, "\xC3\xA8\n\xE4\xB8\x96" },
    { "\\U+0041B", 6.0, 10.0, 1.0, "\\U+0041\nB" },
    { "Hello World", 30.0, 10.0, 1.0, "Hello\nWorld" },
    { "Hello\xC2\xA0World", 30.0, 10.0, 1.0, "Hello\xC2\xA0W\norld" },
    { "A\xC2\xA0 B", 6.0, 10.0, 1.0, "A\xC2\xA0 \nB" },
    { "abcd", 12.0, 5.0, 1.0, "abcd" },
    { "abcd", 12.0, 10.0, 2.0, "a\nb\nc\nd" },
  };
  size_t i;

  for (i = 0; i < sizeof (cases) / sizeof (cases[0]); i++)
    {
      char *s = mtext_wrap_text (cases[i].input, cases[i].rect_width,
                                 cases[i].text_height, cases[i].width_factor);
      if (s && strcmp (s, cases[i].expected) == 0)
        pass ();
      else
        fail ("mtext_wrap_text[%" PRIuSIZE "] => %s", i, s ? s : "(null)");
      free (s);
    }
}

static void
mtext_attachment_tests (void)
{
  static const char *anchors[] = { "start", "middle", "end" };
  static const double expected[] = { 8.0,
                                     8.0,
                                     8.0,
                                     -13.6666666667,
                                     -13.6666666667,
                                     -13.6666666667,
                                     -35.3333333333,
                                     -35.3333333333,
                                     -35.3333333333 };
  unsigned int attachment;
  double line_height;

  line_height = mtext_line_height (10.0, 1.0);
  if (fabs (line_height - 16.6666666667) < 1e-9)
    pass ();
  else
    fail ("mtext_line_height default => %g", line_height);
  if (fabs (mtext_line_height (10.0, 2.0) - 33.3333333333) < 1e-9
      && fabs (mtext_line_height (10.0, 0.0) - 16.6666666667) < 1e-9
      && mtext_line_height ((double)NAN, 1.0) == 0.0)
    pass ();
  else
    fail ("mtext_line_height spacing/invalid");
  if (fabs (mtext_svg_angle (1.0, 0.0)) < 1e-9
      && fabs (mtext_svg_angle (sqrt (0.5), sqrt (0.5)) + 45.0) < 1e-9
      && fabs (mtext_svg_angle (0.0, 1.0) + 90.0) < 1e-9
      && fabs (mtext_svg_angle (cos (0.37), sin (0.37)) + 0.37 * 180.0 / M_PI)
             < 1e-9
      && mtext_svg_angle (0.0, 0.0) == 0.0
      && mtext_svg_angle ((double)NAN, 1.0) == 0.0)
    pass ();
  else
    fail ("mtext_svg_angle rotation/degenerate");

  for (attachment = 1; attachment <= 9; attachment++)
    {
      const char *anchor = mtext_attachment_anchor (attachment);
      const char *expected_anchor = anchors[(attachment - 1) % 3];
      double offset
          = mtext_attachment_first_offset (attachment, 10.0, line_height, 3);
      if (strcmp (anchor, expected_anchor) == 0
          && fabs (offset - expected[attachment - 1]) < 1e-9)
        pass ();
      else
        fail ("mtext_attachment[%u] => %s/%g", attachment, anchor, offset);
    }
  if (strcmp (mtext_attachment_anchor (0), "start") == 0
      && fabs (mtext_attachment_first_offset (0, 10.0, line_height, 0) - 8.0)
             < 1e-9)
    pass ();
  else
    fail ("mtext_attachment invalid values");
}

int
main (int argc, char const *argv[])
{
  loglevel = is_make_silent () ? 0 : 2;
  common_memmem_tests ();
  common_versions_tests ();
  common_cvt_TIMEBLL_tests ();
  dwg_find_color_index_tests ();
  common_strcasecmp_tests ();
  escape_htmlescape_tests ();
  escape_htmlwescape_tests ();
  escape_htmlutf8escape_tests ();
  mtext_escape_line_tests ();
  mtext_escape_line_many_unicode_tests ();
  mtext_stacked_unicode_tests ();
  mtext_plaintext_tests ();
  mtext_normalization_tests ();
  mtext_wrap_text_tests ();
  mtext_attachment_tests ();
  return failed;
}
